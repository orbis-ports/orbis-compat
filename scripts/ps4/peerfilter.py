#!/usr/bin/env python3
"""Source-address filter shared by the two host receivers of the PS4 test link.

WHY IT EXISTS (materialised on the console, 2026-08-02). Both receivers bind 0.0.0.0 and
used to accept whatever arrived. unemups4 forwards a guest `sceNetSendto` straight
to the host kernel (`crates/libs/src/libscenet/mod.rs` — "homebrew that logs over
the network then sends real datagrams from the emulator, so the same listener
receives them"), and every ps4/ demo is compiled with NETLOG_HOST=192.168.100.1,
so an emulator run on the dev host aims at the very port the console leg is
listening on. During a console churn run a concurrent emulator run in another
session put 294 foreign lines into `console-churn.raw.log`; one of them matched
churn's FORBIDDEN `gnm: swapchain WxH` rule and turned a fully green hardware run
into verdict-1 FAIL 1/57. The datagrams were not corrupt — they were somebody
else's, and nothing said so.

The filter is one set of allowed source addresses plus a counter, and it is one
file rather than two copies because the two receivers must word the same event
identically: `[receiver] dropped N foreign …` is what a maintainer greps for after
a run, and two hand-maintained copies of that wording would drift apart.

Line formats — STABLE, grep against them:

    [receiver] peer filter armed: <addr>[, <addr>…]  (dropping every other source)
    [receiver] peer filter OFF — accepting <noun>s from any source
    [receiver] foreign <noun> from <addr> — dropped (allowed: <addr>[, …])
    [receiver] dropped N foreign <noun>(s) from <addr>[, <addr>…]

The first-occurrence line is emitted the moment a foreign source appears, so
cross-talk is visible while the run is still going; the summary is emitted once,
at exit, so a run that dropped nothing says nothing.
"""

from __future__ import annotations

import socket


class PeerFilter:
    """Accept traffic only from a fixed set of source addresses; count the rest.

    `allow` is a list of address specs — dotted quads or names, comma-separated
    entries already split — resolved to IPv4 literals once at construction, so a
    per-datagram check is a set lookup and a DNS hiccup mid-run cannot change the
    verdict. Empty/None means unfiltered, which is what the manual paths use.

    `noun` is "datagram" for UDP and "connection" for TCP; it only ever appears in
    the messages.
    """

    def __init__(self, allow: list[str] | None, noun: str = "datagram"):
        self.noun = noun
        self.allowed = resolve(allow or [])
        self.dropped = 0
        self.sources: list[str] = []  # foreign sources, first-seen order

    @property
    def armed(self) -> bool:
        return bool(self.allowed)

    def arm_line(self) -> str:
        if not self.armed:
            return (f"[receiver] peer filter OFF — accepting {self.noun}s from any source")
        return (f"[receiver] peer filter armed: {', '.join(self.allowed)}"
                f"  (dropping every other source)")

    def accepts(self, addr: str) -> bool:
        return (not self.allowed) or addr in self.allowed

    def record_drop(self, addr: str) -> str | None:
        """Count one dropped item; return a line to emit the first time `addr` shows up."""
        self.dropped += 1
        if addr in self.sources:
            return None
        self.sources.append(addr)
        return (f"[receiver] foreign {self.noun} from {addr} — dropped "
                f"(allowed: {', '.join(self.allowed)})")

    def summary(self) -> str | None:
        """The one line per run. None when nothing was dropped — silence is the good case."""
        if not self.dropped:
            return None
        return (f"[receiver] dropped {self.dropped} foreign {self.noun}(s) "
                f"from {', '.join(self.sources)}")


def split(specs: list[str] | None) -> list[str]:
    """Flatten repeated and comma-separated --allow-from values into one list."""
    out: list[str] = []
    for spec in specs or []:
        out += [part.strip() for part in spec.split(",") if part.strip()]
    return out


def resolve(specs: list[str]) -> list[str]:
    """Address specs -> IPv4 literals, order preserved, duplicates dropped.

    A name that does not resolve is kept verbatim rather than dropped: a filter
    that silently forgot an entry would be a filter that silently accepted a
    source, which is the failure this whole file exists to prevent. It will simply
    never match, and the armed line prints what it will compare against.
    """
    out: list[str] = []
    for spec in split(specs):
        try:
            addrs = [info[4][0] for info in socket.getaddrinfo(spec, None, socket.AF_INET)]
        except OSError:
            addrs = [spec]
        for addr in addrs:
            if addr not in out:
                out.append(addr)
    return out


ALLOW_FROM_HELP = ("accept only this source address (repeatable, or comma-separated). "
                   "Foreign traffic is dropped, counted, and reported. Omitted = accept "
                   "any source, which is what the manual paths want and what the harness "
                   "must never do")
