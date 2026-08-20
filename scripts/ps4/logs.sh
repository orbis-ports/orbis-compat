#!/usr/bin/env bash
# Capture log output from the console into a timestamped file.
#
#   scripts/ps4/logs.sh              # primary:  app-level UDP netlog (port 18194)
#   scripts/ps4/logs.sh klog         # fallback: GoldHEN klog server over TCP 3232
#
# Start this BEFORE launching the title, then leave it running for the session.
# Output goes to build-ps4-logs/ps4-<mode>-<timestamp>.log (gitignored via the
# repo's `*build-*/` rule) and to the terminal.
#
# Why UDP is primary: the console's stdout write blocks the calling thread when
# nothing is draining GoldHEN's klog socket, which stalls a render loop for
# seconds per line. sceNetSendto is fire-and-forget and cannot stall the title.
# klog stays as the fallback because it is the only channel that shows output
# from before our code runs, and from panics we never get to log ourselves.
#
# Environment:
#   PS4_IP        console address, klog mode only  (default 192.168.100.2)
#   PS4_LOG_PORT  udp bind port                    (default 18194)
#   PS4_KLOG_PORT GoldHEN klog port                (default 3232)
#   PS4_LOG_DIR   capture directory                (default <repo>/build-ps4-logs)
#   PS4_LOG_ALLOW udp source-address filter        (default any)
#                   any        accept every source, and say so on the first line
#                   console    accept only $PS4_IP
#                   <addr>,…   accept only these addresses
#
# Why the filter is OFF by default here and was ALWAYS ON in the automated harness:
# that harness read what arrived on this port as evidence — a stray line
# from an emulator run in another session matched a FORBIDDEN rule once and failed a
# green console run — so there it must be impossible for a second sender to be heard.
# This script decides nothing. Its output is read by a human who is usually mid
# bring-up, and the console's address is exactly the thing that is unknown then: it
# comes from DHCP (link.sh hands out .50-.150), so a filter defaulted to $PS4_IP would
# answer "console says nothing" to a console that was talking. Instead the receiver
# names every foreign source the moment it appears, which is the diagnosis rather than
# the silence — and `PS4_LOG_ALLOW=console` is one word away once the address is known.
#
# UNTESTED against hardware from the session that wrote it; the transports and
# ports mirror ps4doom's field-proven `make netlog` / `make klog` rules.
set -euo pipefail

MODE="${1:-udp}"
case "$MODE" in
  udp | netlog) MODE="udp" ;;
  klog | tcp)   MODE="klog" ;;
  -h | --help)  sed -n '2,9p' "$0"; exit 0 ;;
  *) echo "logs: unknown mode '$MODE' (expected udp|klog)" >&2; exit 1 ;;
esac

# ⚠ FIRST, BECAUSE PS4_LOG_DIR READS IT. It was defined two lines BELOW its own use for a day: the
# capture directory expanded $HERE before this line ran, `set -u` refused, and the script died
# before binding the socket. Written and never run - the transport had moved and the receiver
# had not been started since.
HERE="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

PS4_IP="${PS4_IP:-192.168.100.2}"
PS4_LOG_PORT="${PS4_LOG_PORT:-18194}"
PS4_KLOG_PORT="${PS4_KLOG_PORT:-3232}"
# ⚠ RELATIVE TO THIS SCRIPT, NOT TO THE CALLER. It used to be a bare "build-ps4-logs", so captures
# landed wherever the shell happened to be - and on 2026-08-20, after this script moved repositories,
# a run's log went somewhere nobody looked and the transport was briefly blamed instead. One place,
# always; PS4_LOG_DIR still overrides.
PS4_LOG_DIR="${PS4_LOG_DIR:-$(cd "$HERE/../.." && pwd)/build-ps4-logs}"
command -v python3 > /dev/null || { echo "logs: python3 is required" >&2; exit 1; }

mkdir -p "$PS4_LOG_DIR"
OUT="${PS4_LOG_DIR}/ps4-${MODE}-$(date +%Y%m%d-%H%M%S).log"
echo "logs: capturing to $OUT"

if [[ "$MODE" == "udp" ]]; then
  ALLOW=()
  case "${PS4_LOG_ALLOW:-any}" in
    any | "")  ;;
    console)   ALLOW=(--allow-from "$PS4_IP") ;;
    *)         ALLOW=(--allow-from "$PS4_LOG_ALLOW") ;;
  esac
  exec python3 "$HERE/log-receiver.py" udp --port "$PS4_LOG_PORT" --out "$OUT" "${ALLOW[@]}"
fi
exec python3 "$HERE/log-receiver.py" klog --host "$PS4_IP" --port "$PS4_KLOG_PORT" --out "$OUT"
