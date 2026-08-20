#!/usr/bin/env python3
"""Receive PS4 log output on the dev host, timestamp it, tee it to a file.

Two transports, matching the two channels a GoldHEN console offers:

  udp   bind 0.0.0.0:<port> and print every datagram the title sends.
        This is the app-level channel: the title calls netlog() from
        ps4/common/ps4_netlog.c, which does a fire-and-forget sceNetSendto.
        Nothing on the console has to be running or draining for it to work.

  klog  connect to <console>:3232 and stream GoldHEN's kernel log server.
        Catches everything, including output from before our code runs and
        from crashes/panics we never get to log ourselves. Requires GoldHEN's
        klog server to be enabled, and anything written to stdout on the
        console *blocks* when nobody drains this socket.

Normally driven through scripts/ps4/logs.sh; usable standalone:

    scripts/ps4/log-receiver.py udp  --port 18194 --out build-ps4-logs/x.log
    scripts/ps4/log-receiver.py klog --host 192.168.100.2 --out .../x.log

`--allow-from ADDR` (udp only) makes the socket accept datagrams from that source
and no other; anything else is dropped, counted and named. scripts/ps4/run-tests.sh
always passes it, because on that path the received text decides verdicts and an
emulator run started elsewhere on this host aims at the same port — see
peerfilter.py for the incident that made this necessary. klog needs no filter: it
is a TCP connection this process opens to a host it names.
"""

from __future__ import annotations

import argparse
import datetime
import signal
import socket
import sys

import peerfilter

DEFAULT_UDP_PORT = 18194
DEFAULT_KLOG_PORT = 3232


def _stamp() -> str:
    return datetime.datetime.now().strftime("%H:%M:%S.%f")[:-3]


class Sink:
    """Timestamps whole lines, echoes to stdout, appends to a file."""

    def __init__(self, path: str | None):
        self._buf = ""
        self._fh = open(path, "a", encoding="utf-8", buffering=1) if path else None

    def feed(self, chunk: bytes) -> None:
        self._buf += chunk.decode("utf-8", "replace")
        while "\n" in self._buf:
            line, self._buf = self._buf.split("\n", 1)
            self._emit(line)

    def flush_partial(self) -> None:
        if self._buf:
            self._emit(self._buf)
            self._buf = ""

    def note(self, text: str) -> None:
        self._emit(text)

    def _emit(self, line: str) -> None:
        out = f"{_stamp()} {line.rstrip()}"
        print(out, flush=True)
        if self._fh:
            self._fh.write(out + "\n")


def run_udp(port: int, sink: Sink, peers: peerfilter.PeerFilter) -> int:
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("0.0.0.0", port))
    sink.note(f"[receiver] udp 0.0.0.0:{port} — waiting for the console (ctrl-c to stop)")
    sink.note(peers.arm_line())
    while True:
        data, addr = sock.recvfrom(4096)
        if not peers.accepts(addr[0]):
            # Dropped before it can reach the sink: the point of the filter is that
            # this text never enters the file a verdict is read from. The first one
            # from each source is announced immediately, the rest are counted and
            # stated once by the summary in main().
            first = peers.record_drop(addr[0])
            if first:
                sink.note(first)
            continue
        # One datagram is one log write; treat it as line-oriented anyway so a
        # sender that batches several lines still comes out right.
        if not data.endswith(b"\n"):
            data += b"\n"
        sink.feed(data)


def run_klog(host: str, port: int, sink: Sink) -> int:
    sink.note(f"[receiver] klog {host}:{port} — connecting")
    with socket.create_connection((host, port), timeout=10) as sock:
        sock.settimeout(None)
        sink.note("[receiver] klog connected (ctrl-c to stop)")
        while True:
            chunk = sock.recv(4096)
            if not chunk:
                sink.note("[receiver] klog closed by console")
                return 1
            sink.feed(chunk)


def _raise_interrupt(signum, frame) -> None:  # signal.signal handler signature
    raise KeyboardInterrupt


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("mode", choices=("udp", "klog"))
    ap.add_argument("--host", default="192.168.100.2", help="console address (klog only)")
    ap.add_argument("--port", type=int, default=None, help="udp bind port / klog console port")
    ap.add_argument("--out", default=None, help="append a copy of the capture here")
    ap.add_argument("--allow-from", action="append", default=[], metavar="ADDR",
                    help=peerfilter.ALLOW_FROM_HELP)
    args = ap.parse_args()
    if args.mode == "klog" and args.allow_from:
        # Refused rather than ignored: a filter silently doing nothing is the shape of
        # the bug this option exists to close.
        ap.error("--allow-from is a udp-mode filter; klog dials out to --host itself "
                 "and has no other peer it could accept")

    # The harness reaps this process with SIGTERM (run-tests.sh cleanup()), so the
    # default disposition would take the summary line with it. Turn it into the same
    # unwind ctrl-c takes.
    signal.signal(signal.SIGTERM, _raise_interrupt)

    sink = Sink(args.out)
    peers = peerfilter.PeerFilter(args.allow_from, "datagram")
    try:
        if args.mode == "udp":
            return run_udp(args.port or DEFAULT_UDP_PORT, sink, peers)
        return run_klog(args.host, args.port or DEFAULT_KLOG_PORT, sink)
    except KeyboardInterrupt:
        sink.flush_partial()
        sink.note("[receiver] stopped")
        return 0
    except OSError as err:
        sink.note(f"[receiver] {err}")
        return 1
    finally:
        # One line per run, and only when there was something to say.
        summary = peers.summary()
        if summary:
            sink.note(summary)


if __name__ == "__main__":
    sys.exit(main())
