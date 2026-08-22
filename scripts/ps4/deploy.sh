#!/usr/bin/env bash
# Put a .pkg on the console, and say which of the two things has to happen next.
#
#   scripts/ps4/deploy.sh --pkg <file> --name <short> [--host <ip>] [--also <file>:<remote>]...
#
#   --pkg    the package to upload
#   --name   short name for it; the remote file becomes /data/pkg/<name>-YYYYMMDD.pkg
#   --also   an extra file to upload, as <local>:<remote> - run configuration, case lists
#   --host   console address (default $ORBIS_CONSOLE, then 192.168.100.2)
#
# ⚠ /data/pkg AND NOWHERE ELSE. This console installs packages only from there.
#
# ⚠ AND THE UPLOAD IS VERIFIED BY READING BACK, not by ftp's exit status. Every artefact check in
# this project that trusted a status code has eventually reported success over a failure: a git push
# that disconnected mid-pack left an empty repository and still exited 0. Sizes are compared here,
# and text files are compared byte for byte.
#
# ⚠ SIZE DOES NOT PROVE FRESHNESS. Three different CTS packages built on three different days were
# all exactly 109117440 bytes. This checks that what ARRIVED matches what was SENT; whether what was
# sent is what you meant is a different question, and the build script that made it is where that
# gets answered.
#
# SPDX-License-Identifier: MIT
set -euo pipefail

PKG=""; NAME=""; HOST="${ORBIS_CONSOLE:-192.168.100.2}"; PORT="${ORBIS_FTP_PORT:-2121}"
ALSO=()

while [[ $# -gt 0 ]]; do
  case "$1" in
    --pkg)  PKG="$2"; shift 2 ;;
    --name) NAME="$2"; shift 2 ;;
    --host) HOST="$2"; shift 2 ;;
    --also) ALSO+=("$2"); shift 2 ;;
    -h|--help) sed -n '2,20p' "${BASH_SOURCE[0]}"; exit 0 ;;
    *) echo "!! unknown argument: $1" >&2; exit 1 ;;
  esac
done

die() { echo "!! $*" >&2; exit 1; }
command -v lftp >/dev/null || die "lftp is required"

REMOTE=""
if [[ -n "${PKG}" ]]; then
  [[ -f "${PKG}" ]] || die "no such package: ${PKG}"
  [[ -n "${NAME}" ]] || die "--name is required with --pkg, so the console file says what it is"
  # Dated, because the console keeps every package that was ever installed and an undated name
  # cannot be pointed at one build.
  REMOTE="/data/pkg/${NAME}-$(date '+%Y%m%d').pkg"
fi

# ⚠ NOTHING HERE MAY FAIL QUIETLY, AND THE FIRST VERSION OF THIS FILE DID EXACTLY THAT. Its helpers
# ran lftp with the output sent to /dev/null under `set -e`, so a refused transfer ended the script
# between two lines with no message at all - the upload looked half-done and said nothing about why.
# That is the shape this whole project spends its time hunting, built by hand into the tool meant to
# report it. Every lftp call now goes through one function that checks the status and prints what
# lftp said.
# ⚠ AND IT MAY NOT HANG EITHER. lftp retries a refused connection FOREVER by default, so a console
# that is off - or wedged, which this one does - turned the script into a process that printed the
# filename and then sat there. Measured: ninety seconds against an unroutable address with no output
# and no exit. Three settings bound it; a tool that hangs reports as little as one that lies.
LFTP_SETTINGS="set net:max-retries 2; set net:timeout 10; set net:reconnect-interval-base 4; set xfer:timeout 60"

lftp_do() {
  local script="$1" out status
  out="$(lftp -p "${PORT}" "${HOST}" -e "${LFTP_SETTINGS}; ${script}; bye" 2>&1)" && status=0 || status=$?
  if [[ ${status} -ne 0 ]]; then
    echo "!! lftp failed (status ${status}) running: ${script}" >&2
    [[ -n "${out}" ]] && echo "${out}" >&2
    exit 1
  fi
  printf '%s' "${out}"
}

put() {
  local local_f="$1" remote_f="$2"
  echo "== $(stat -c %s "${local_f}") B  ${local_f}"
  echo "   -> ${remote_f}"
  lftp_do "put \"${local_f}\" -o \"${remote_f}\"" >/dev/null
}

# ⚠ ONLY `cls -l <directory>` WORKS ON THIS SERVER. Measured 2026-08-22: `cls -l <file>` returns an
# empty listing and `ls -l <file>` answers "550 Requested action not taken", both for a file that is
# demonstrably there. Asking the directory and picking the line out is the form that answers.
# ⚠ FIELD 4, AND THE FIRST VERSION READ FIELD 5 - which is the MONTH. It reported
# "is sie bytes on the console, sent 51314688" on a transfer that had in fact succeeded, and it took
# a Polish locale printing `sie` to make the mistake visible at all; under an English locale it
# would have compared 51314688 against `Aug` and failed just as wrongly, but less obviously.
#
#   -rwxrwxrwx 0        0        51314688 sie 23 10:16 /data/pkg/og-plan-20260823.pkg
#   $1         $2       $3       $4       $5  $6 $7    $8
#
# So the value is validated as a number before it is compared. A check that can compare a month
# against a byte count is not a check.
remote_size() {
  local remote_f="$1" v
  v="$(lftp_do "cls -l \"$(dirname "${remote_f}")/\"" | awk -v n="${remote_f}" '$NF == n { print $4 }' | head -1)"
  [[ "${v}" =~ ^[0-9]+$ ]] || return 1
  printf '%s' "${v}"
}

verify_size() {
  local local_f="$1" remote_f="$2" want got
  want="$(stat -c %s "${local_f}")"
  got="$(remote_size "${remote_f}")" || die \
    "could not read a size for ${remote_f} from the console - it is missing, or the listing format changed"
  [[ -n "${got}" ]] || die "${remote_f} is not on the console after the upload"
  [[ "${want}" == "${got}" ]] || die "${remote_f} is ${got} bytes on the console, sent ${want}"
  echo "   verified: ${got} B on the console"
}

verify_bytes() {
  local local_f="$1" remote_f="$2" dir
  # ⚠ mktemp -d, NOT mktemp. lftp's `get -o` refuses to overwrite an existing destination, and
  # mktemp creates the file - so the download failed every time, which is how the silent-exit bug
  # above was found.
  dir="$(mktemp -d)"
  lftp_do "get \"${remote_f}\" -o \"${dir}/f\"" >/dev/null
  if ! cmp -s "${local_f}" "${dir}/f"; then
    rm -rf "${dir}"
    die "read-back of ${remote_f} differs from what was sent"
  fi
  rm -rf "${dir}"
  echo "   verified: identical byte for byte"
}

for spec in "${ALSO[@]}"; do
  l="${spec%%:*}"; r="${spec#*:}"
  [[ -f "$l" ]] || die "no such file: $l"
  put "$l" "$r"
  # Small enough to compare properly. Run configuration is exactly the kind of file where one wrong
  # line costs a console trip, so it gets the strong check rather than the cheap one.
  if [[ "$(stat -c %s "$l")" -lt 1048576 ]]; then verify_bytes "$l" "$r"; else verify_size "$l" "$r"; fi
done

if [[ -n "${PKG}" ]]; then
  put "${PKG}" "${REMOTE}"
  verify_size "${PKG}" "${REMOTE}"
  echo
  echo "== INSTALL + RUN"
  echo "   Install ${REMOTE} on the console, then start it."
  echo "   ⚠ An older package under the same title id is already installed; without installing"
  echo "     this one the run uses the old binary and ignores everything built above."
elif [[ ${#ALSO[@]} -gt 0 ]]; then
  echo
  echo "== RUN, no install"
  echo "   Only configuration changed; the installed package is unaffected."
else
  die "nothing to do - pass --pkg and/or --also"
fi
