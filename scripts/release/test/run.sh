#!/usr/bin/env bash
# Copyright © 2026 Mikołaj Mikołajczyk
# SPDX-License-Identifier: MIT
#
# The test suite for scripts/release/. Run it with no arguments; it needs no network, no SDK,
# no Mesa bundle and no console, and it finishes in seconds.
#
#   scripts/release/test/run.sh [--keep] [--list] [--no-skips] [<case-id> ...]
#
# WHY THIS EXISTS, AND WHAT IT IS AIMED AT. bundle-gate.sh's first full run on 2026-09-17 took
# ten attempts and SIX of the failures were in the checking machinery rather than in any bundle
# it judged (the list is in that script's header and in scripts/release/README.md). ~1800 lines
# of release scripts had, until this file, exactly one way of being exercised: push, wait for
# CI, read a log. Three of eight CI failures in the last release round were again in this
# machinery. Every case below is either a regression test for one of those six defects or a row
# of the pairing matrix they live in.
#
# ------------------------------------------------------------------ the rule this suite follows
#
# ⚠ A TEST THAT CANNOT FAIL IS NOT A TEST. test/crt_abi.sh states the principle for this
# repository and build.sh enforces it by running that script against a decoy object and
# REQUIRING a non-zero exit. The same discipline here, in three forms:
#
#   * every "must refuse" case is paired with a "must accept" case over inputs that differ in
#     exactly the one field under test, so a check that refuses everything shows up as a failure
#     rather than as a green run;
#   * every static check (the ones that read a script's text because the behaviour needs a
#     three-hour Mesa build to reach) is run twice - once over the real file, once over a
#     deliberately broken copy - and the case fails unless the decoy is caught;
#   * the three verdicts the scripts use are asserted EXACTLY. OK is exit 0, INCOMPLETE is
#     exit 4, FAILED is exit 1, and "non-zero" is never accepted as a result. Conflating
#     INCOMPLETE with FAILED is defect 2 - it made bundle-gate.sh refuse every bundle it exists
#     to fix - and a suite that accepted "it errored" would have shipped it too.
#
# ------------------------------------------------------------------ what it cannot reach
#
# Stated here rather than faked. bundle-gate.sh stages 3-6 need a cross toolchain, a real SDK
# and a real port; stage 5 alone is a three-hour OpenGothic build. What this suite runs of that
# script is stage 0 (host tools), stage 1 (unpack) and stage 1b (the offline verify and its
# INCOMPLETE tolerance, which IS defect 2's other half); the two defects that live past that
# point - the SIGPIPE count in stage 4 and the build-directory isolation in stage 5 - are
# covered by evaluating the real source line (F1) and by a decoy-checked read of the real
# invocation (F2). Each case says so in its own "why" line.
#
# Exit codes: 0 every case passed · 1 at least one case FAILED · 2 usage.
set -uo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
REL="$(cd "$HERE/.." && pwd -P)"
REPO="$(cd "$REL/../.." && pwd -P)"
# shellcheck source=scripts/release/test/fixtures.sh
. "$HERE/fixtures.sh"

KEEP=0; LIST=0; NOSKIP=0; ONLY=()
while [ $# -gt 0 ]; do
  case "$1" in
    --keep) KEEP=1; shift ;;
    --list) LIST=1; shift ;;
    # ⚠ FOR CI ONLY. A skip is the honest answer on a developer laptop that has no llvm-nm,
    # and it is a FAILURE on a runner that just installed one: the two cases that can skip are
    # the two that cover defects 5 and 2, and a green CI run that quietly skipped both would be
    # worth nothing. The runner knows it installed the tools; the suite does not.
    --no-skips) NOSKIP=1; shift ;;
    -h|--help) sed -n '1,50p' "$0"; exit 0 ;;
    -*) echo "unknown argument: $1" >&2; exit 2 ;;
    *) ONLY+=("$1"); shift ;;
  esac
done

# ------------------------------------------------------------------ harness
C_OK=$'\033[1;32m'; C_NO=$'\033[1;31m'; C_SK=$'\033[1;33m'; C_HD=$'\033[1;34m'; C_Z=$'\033[0m'
[ -t 1 ] || { C_OK=""; C_NO=""; C_SK=""; C_HD=""; C_Z=""; }

N_PASS=0; N_FAIL=0; N_SKIP=0; FAILED=(); SKIPPED=()
CASE=""; CASE_BAD=0; CASE_SKIP=""; CAPN=0; CAP=""; RC=0

case_begin(){ # case_begin <id> <what it asserts> <why this case exists>
  CASE="$1"; CASE_BAD=0; CASE_SKIP=""
  printf '%s==>%s %-38s %s\n' "$C_HD" "$C_Z" "$1" "$2"
  printf '    why: %s\n' "$3"
}
case_end(){
  if [ -n "$CASE_SKIP" ]; then
    N_SKIP=$((N_SKIP+1)); SKIPPED+=("$CASE: $CASE_SKIP")
    printf '    %sSKIP%s %s\n\n' "$C_SK" "$C_Z" "$CASE_SKIP"
  elif [ "$CASE_BAD" -ne 0 ]; then
    N_FAIL=$((N_FAIL+1)); FAILED+=("$CASE")
    # The last thing this case ran, so a failure hands over the evidence instead of a verdict.
    [ -s "${CAP:-}" ] && { printf '     --- last 25 lines of %s\n' "$CAP"; sed 's/^/     | /' \
        <(tail -25 "$CAP"); }
    printf '    %sFAIL%s\n\n' "$C_NO" "$C_Z"
  else
    N_PASS=$((N_PASS+1)); printf '    %spass%s\n\n' "$C_OK" "$C_Z"
  fi
}
bad(){  printf '     %s!!%s %s\n' "$C_NO" "$C_Z" "$*"; CASE_BAD=1; }
note(){ printf '     .. %s\n' "$*"; }
skip(){ CASE_SKIP="$*"; }

# The three verdicts, by name. Everything that asserts an exit status goes through this, so a
# case can never say "it failed somehow" - defect 2 is exactly that distinction going missing.
verdict(){ case "$1" in 0) echo OK ;; 1) echo FAILED ;; 2) echo USAGE ;; 4) echo INCOMPLETE ;;
                        *) echo "rc=$1" ;; esac; }
expect_verdict(){ # expect_verdict <OK|FAILED|INCOMPLETE|USAGE>
  local got; got="$(verdict "$RC")"
  [ "$got" = "$1" ] || bad "verdict $got (exit $RC), wanted $1"
}
expect_out(){    grep -qF -- "$1" "$CAP" || bad "output does not contain: $1"; }
expect_out_re(){ grep -qE -- "$1" "$CAP" || bad "output does not match: $1"; }
expect_no_out(){ grep -qF -- "$1" "$CAP" && bad "output unexpectedly contains: $1"; return 0; }

cap(){ CAPN=$((CAPN+1)); CAP="$W/cap.$CAPN.log"; "$@" >"$CAP" 2>&1; RC=$?; }

# macOS ships no timeout(1). perl's alarm is present everywhere this repository is developed
# and is the cheapest portable fence around a case that shells out to cmake.
with_timeout(){ local s="$1"; shift
  if command -v perl >/dev/null 2>&1; then perl -e 'alarm shift; exec @ARGV' "$s" "$@"
  else "$@"; fi; }

# ------------------------------------------------------------------ the work tree
W="$(mktemp -d)"
cleanup(){ [ "$KEEP" -eq 1 ] && { printf '%s!!%s work tree kept at %s\n' "$C_SK" "$C_Z" "$W"; return; }
           rm -rf "$W"; }
trap cleanup EXIT

# ------------------------------------------------------------------ shared fixtures
#
# Built once. A cut takes ~1.3s against these (measured 2026-09-18, macOS/arm64); against the
# real 289 MB SDK it takes minutes, which is the whole reason the stand-in exists.
setup(){
  fx_sdk    "$W/sdk"
  fx_compat "$W/compat" "$REPO"
  SHA_A="$(fx_compat_sha "$W/compat" a)"
  SHA_B="$(fx_compat_sha "$W/compat" b)"
  SHA_C="$(fx_compat_sha "$W/compat" c)"
  fx_compat_at "$W/compat" a
  INC_A="$(fx_include_sha256 "$W/compat")"
  fx_compat_at "$W/compat" c
  INC_C="$(fx_include_sha256 "$W/compat")"
  fx_compat_at "$W/compat" a
  # A syntactically valid fingerprint that is not any tree here: the shape of a Mesa built from
  # a checkout somebody had touched.
  INC_BOGUS="$(printf 'not-the-tree' | _sha256 | cut -d' ' -f1)"
  [ "$INC_A" != "$INC_C" ] || { echo "fixture is broken: commit C did not change include/" >&2; exit 1; }
}

# do_cut <tag> <compat-commit a|b|c> <mesa-dir> [extra make-sdk-bundle args...]
# Leaves the tarball at $W/out.<tag>/release/orbis-sdk-<tag>.tar.gz.
do_cut(){
  local tag="$1" at="$2" mesa="$3"; shift 3
  fx_compat_at "$W/compat" "$at"
  cap env "ORBIS_BUNDLE_GATE=${GATE:-unproven}" "$REL/make-sdk-bundle.sh" \
        --version "$tag" --sdk "$W/sdk" --mesa "$mesa" --compat "$W/compat" \
        --out "$W/out.$tag" "$@"
  TARBALL="$W/out.$tag/release/orbis-sdk-$tag.tar.gz"
}
do_verify(){ cap "$REL/verify-sdk-bundle.sh" "$1"; }
bfield(){ sed -n "s/^$2=//p" "$1/BUNDLE.txt" | head -1; }

# =========================================================================================
# A. THE FINGERPRINT ITSELF
# =========================================================================================

# The extractor: the function text with leading indentation removed, because mesa-ps4's copy
# lives ten spaces deep inside a YAML `run:` block and indentation is the one difference that
# is not drift.
extract_fn(){ # extract_fn <file> <fn-name>
  sed -n "/^[[:space:]]*$2(){/,/^[[:space:]]*}[[:space:]]*\$/p" "$1" | sed 's/^[[:space:]]*//'
}
extract_sha_helper(){ grep -h '^[[:space:]]*_sha256(){' "$1" | sed 's/^[[:space:]]*//'; }

case_A1(){
  case_begin A1-fingerprint-three-copies \
    "the three copies of _include_sha256 are identical after indentation" \
    "make-sdk-bundle.sh's header makes this a hard rule: the three numbers are compared for EQUALITY across three machines that cannot share a file, so one byte tidied in any copy reads downstream as a mismatched pair and refuses a bundle that is fine."
  local canon copy mesa_yml
  canon="$(extract_fn "$REL/make-sdk-bundle.sh" _include_sha256)$(extract_sha_helper "$REL/make-sdk-bundle.sh")"
  copy="$(extract_fn "$REL/verify-sdk-bundle.sh" _include_sha256)$(extract_sha_helper "$REL/verify-sdk-bundle.sh")"
  [ -n "$canon" ] || bad "could not extract _include_sha256 from make-sdk-bundle.sh"
  [ "$canon" = "$copy" ] || bad "verify-sdk-bundle.sh's copy differs from the canonical one"

  mesa_yml="$REPO/../mesa-ps4/.github/workflows/release.yml"
  if [ -f "$mesa_yml" ]; then
    local third
    third="$(extract_fn "$mesa_yml" _include_sha256)$(extract_sha_helper "$mesa_yml")"
    [ "$canon" = "$third" ] || bad "mesa-ps4's release.yml copy differs from the canonical one"
    note "third copy read from $mesa_yml"
  else
    note "mesa-ps4 is not checked out beside this repository - the THIRD copy went unchecked."
    note "  In CI, check out orbis-ports/mesa-ps4 beside this one to close that hole."
  fi
  # ⚠ THE DECOY. Without it this case passes just as happily when extract_fn returns the empty
  # string from both files, which is what a rename would do.
  local decoy; decoy="$(printf '%s' "$canon" | sed 's/LC_ALL=C/LC_ALL=c/')"
  [ "$decoy" != "$canon" ] || bad "decoy is identical to the original - the comparison proves nothing"
  case_end
}

case_A2(){
  case_begin A2-fingerprint-independent \
    "an independent implementation of the documented algorithm reproduces the cut's number" \
    "BUNDLE.txt's orbis-compat-include-sha256 is a claim a stranger must be able to re-derive. If the only implementation is the one that wrote it, the check is one number read twice - which is what verify's 4b comment says it used to be."
  local b; b="$(fx_unpack "$BASE_TARBALL" "$W/u.A2")"
  local mine cut
  mine="$(fx_include_sha256 "$b/orbis-compat")"
  cut="$(bfield "$b" orbis-compat-include-sha256)"
  [ ${#mine} -eq 64 ] || bad "the independent implementation produced '$mine', not 64 hex"
  [ "$mine" = "$cut" ] || bad "independent=$mine cut=$cut"
  [ "$cut" = "$INC_A" ] || bad "the cut recorded $cut, the fixture measured $INC_A"
  case_end
}

case_A3(){
  case_begin A3-verify-travels-in-the-bundle \
    "the bundle's ./verify.sh is byte-identical to scripts/release/verify-sdk-bundle.sh" \
    "verify-sdk-bundle.sh's header promises 'a copy of this script ships INSIDE every bundle as ./verify.sh, so the check travels with the thing it checks'. A stranger runs that copy and nothing else ever compares them."
  local b; b="$(fx_unpack "$BASE_TARBALL" "$W/u.A3")"
  cmp -s "$b/verify.sh" "$REL/verify-sdk-bundle.sh" || bad "verify.sh in the bundle is not the checked-in script"
  [ -x "$b/verify.sh" ] || bad "verify.sh in the bundle is not executable"
  case_end
}

# =========================================================================================
# B. THE PAIRING MATRIX. Six rows, and the only difference between them is which of
#    {orbis-compat commit, mesa's recorded commit, mesa's recorded include/ fingerprint}
#    agrees with which.
# =========================================================================================

case_B1(){
  case_begin B1-pair-same-commit-fingerprint-match \
    "the coherent pair cuts clean and verifies OK (exit 0)" \
    "THE POSITIVE CONTROL for every refusal below. Without one case that reaches exit 0, a verify that refused everything would read as a green suite - and exit 0 is only reachable with gate=pass, so this also proves the gate field is honoured."
  [ -f "$BASE_TARBALL" ] || { bad "the baseline cut did not produce a tarball"; case_end; return; }
  local b; b="$(fx_unpack "$BASE_TARBALL" "$W/u.B1")"
  [ "$(bfield "$b" pairing)" = ok ] || bad "pairing=$(bfield "$b" pairing), wanted ok"
  [ "$(bfield "$b" pairing-basis)" = same-commit ] \
    || bad "pairing-basis=$(bfield "$b" pairing-basis), wanted same-commit"
  do_verify "$BASE_TARBALL"
  expect_verdict OK
  expect_out "OK - every check ran and every check passed"
  case_end
}

case_B2(){
  case_begin B2-commits-differ-include-identical \
    "commits differ, include/ byte-identical, Mesa recorded no fingerprint -> cut OK, verify INCOMPLETE (exit 4)" \
    "The row that cost a rebuild per README typo under the old commit-id rule; make-sdk-bundle.sh measured it over cc75949..4f61d6f, three commits whose 'git diff -- include/' is empty. It must be ACCEPTED by the cut and reported NOT CHECKED by verify, because a Mesa bundle predating 2026-09-18 carries no fingerprint to settle it."
  fx_mesa "$W/mesa.B2" "$SHA_A" none
  GATE=pass do_cut b2 b "$W/mesa.B2"
  expect_verdict OK
  expect_out "include/ is identical across those commits - the pair is coherent"
  local b; b="$(fx_unpack "$TARBALL" "$W/u.B2")"
  [ "$(bfield "$b" pairing-basis)" = include-identical ] \
    || bad "pairing-basis=$(bfield "$b" pairing-basis), wanted include-identical"
  do_verify "$TARBALL"
  expect_verdict INCOMPLETE
  expect_out "this Mesa bundle predates the"
  expect_no_out "MISMATCHED PAIR"
  case_end
}

case_B3(){
  case_begin B3-commits-differ-fingerprint-match \
    "commits differ and Mesa's recorded include/ fingerprint IS the one shipping -> OK (exit 0)" \
    "The row the 2026-09-18 fingerprint was added for: two numbers measured on two machines settle the pair with no git history, so a stranger holding only the tarball can re-derive the verdict. It must reach pairing-basis=include-sha-match and a clean exit 0, or the field bought nothing."
  fx_mesa "$W/mesa.B3" "$SHA_A" "$INC_A"
  GATE=pass do_cut b3 b "$W/mesa.B3"
  expect_verdict OK
  expect_out "from Mesa's own manifest) - the pair is coherent"
  local b; b="$(fx_unpack "$TARBALL" "$W/u.B3")"
  [ "$(bfield "$b" pairing-basis)" = include-sha-match ] \
    || bad "pairing-basis=$(bfield "$b" pairing-basis), wanted include-sha-match"
  do_verify "$TARBALL"
  expect_verdict OK
  case_end
}

case_B4(){
  case_begin B4-old-manifest-same-commit \
    "same commit, Mesa manifest predating the fingerprint field -> INCOMPLETE (exit 4), never FAILED" \
    "Every Mesa bundle published before 2026-09-18 lands here. verify's 4b comment commits to 'NOT CHECKED, not FAILED' for exactly this input; getting it wrong would refuse every bundle built from the Mesa releases that exist today."
  fx_mesa "$W/mesa.B4" "$SHA_A" none
  GATE=pass do_cut b4 a "$W/mesa.B4"
  expect_verdict OK
  local b; b="$(fx_unpack "$TARBALL" "$W/u.B4")"
  [ "$(bfield "$b" mesa-built-against-include-sha256)" = unrecorded ] \
    || bad "mesa-built-against-include-sha256=$(bfield "$b" mesa-built-against-include-sha256), wanted unrecorded"
  do_verify "$TARBALL"
  expect_verdict INCOMPLETE
  expect_out "NOT CHECKED"
  B4_TARBALL="$TARBALL"
  case_end
}

case_B5(){
  case_begin B5-same-commit-different-include-sha \
    "Mesa recorded a DIFFERENT include/ under the same commit id -> the cut REFUSES (exit 1)" \
    "The case no commit-id test and no git diff can see: two checkouts of one commit whose headers are not the same bytes - a dirty tree, a hand-patched header, a cache restored over the wrong ref. It is the entire reason the fingerprint exists, and make-sdk-bundle.sh has a dedicated message for it."
  fx_mesa "$W/mesa.B5" "$SHA_A" "$INC_BOGUS"
  do_cut b5 a "$W/mesa.B5"
  expect_verdict FAILED
  expect_out "the commit ids AGREE and the header BYTES do not"
  expect_out "REFUSED."
  # ⚠ the drift figures must NOT be printed here: they read "0 commits, no change under
  # include/" under a refusal, which looks like a bug in the script rather than the thing it
  # just caught. That is what the branch above them is for.
  expect_no_out "between them: 0 commits"
  case_end
}

case_B6(){
  case_begin B6-forced-mismatch-fails-verify \
    "--allow-pair-mismatch cuts, stamps pairing=MISMATCH, and verify then FAILS (exit 1)" \
    "make-sdk-bundle.sh's header promises the escape hatch 'stamps BUNDLE.txt so loudly that verify-sdk-bundle.sh fails on it afterwards'. Same inputs as B5, one flag different: if this reached exit 0 the hatch would be a silent bypass."
  fx_mesa "$W/mesa.B6" "$SHA_A" "$INC_BOGUS"
  GATE=pass do_cut b6 a "$W/mesa.B6" --allow-pair-mismatch
  expect_verdict OK
  local b; b="$(fx_unpack "$TARBALL" "$W/u.B6")"
  [ "$(bfield "$b" pairing)" = MISMATCH ] || bad "pairing=$(bfield "$b" pairing), wanted MISMATCH"
  do_verify "$TARBALL"
  expect_verdict FAILED
  expect_out "It was cut with --allow-pair-mismatch."
  case_end
}

case_B7(){
  case_begin B7-include-really-differs \
    "commits differ AND include/ differs -> the cut REFUSES (exit 1) and prints the real drift" \
    "The pair the whole gate exists to stop: Mesa compiled one set of headers and would link a different archive. The drift figures are the most useful line the refusal has, and they printed blanks the first time this fired in CI, so they are asserted rather than assumed."
  fx_mesa "$W/mesa.B7" "$SHA_A" none
  do_cut b7 c "$W/mesa.B7"
  expect_verdict FAILED
  expect_out_re 'between them: [0-9]+ commits,'
  expect_out "under include/"
  expect_out "REFUSED."
  case_end
}

# =========================================================================================
# C. POST-CUT TAMPERING. Everything here happens to an UNPACKED bundle, which is the only
#    thing a stranger ever has. C5 is the one case that leaves SHA256SUMS stale on purpose.
# =========================================================================================

case_C1(){
  case_begin C1-headers-edited-after-the-cut \
    "a header changed after the cut -> FAILED (exit 1), naming the fingerprint that moved" \
    "SHA256SUMS alone would catch this, so the case re-hashes first: what is under test is 4b's ability to say WHICH of the three numbers disagrees, on a bundle whose every file matches its recorded hash."
  local b; b="$(fx_unpack "$BASE_TARBALL" "$W/u.C1")"
  printf '\n/* edited after the cut */\n' >> "$b/orbis-compat/include/errno.h"
  fx_resum "$b"
  do_verify "$b"
  expect_verdict FAILED
  expect_out "the headers were altered after"
  expect_out "neither what Mesa compiled nor what was cut"
  # The re-hash has to have worked, or this is C5 wearing C1's name.
  expect_no_out "does not match its recorded sha256"
  case_end
}

case_C2(){
  case_begin C2-mesa-fingerprint-stripped \
    "the fingerprint line removed from mesa/manifest.txt after the cut -> FAILED, not INCOMPLETE" \
    "verify's 4b comment: 'an empty field in the manifest while BUNDLE.txt names a hash is not an old manifest - it is a manifest somebody stripped after the cut, and it must not be able to buy itself the lenient branch.' The lenient branch is exit 4; this must be exit 1."
  local b; b="$(fx_unpack "$BASE_TARBALL" "$W/u.C2")"
  sed -i.bak '/^orbis-compat-include-sha256=/d' "$b/mesa/manifest.txt"; rm -f "$b/mesa/manifest.txt.bak"
  fx_resum "$b"
  do_verify "$b"
  expect_verdict FAILED
  expect_out "disagree about the include/ tree Mesa compiled"
  case_end
}

case_C3(){
  case_begin C3-mesa-fingerprint-grown \
    "a fingerprint ADDED to mesa/manifest.txt after a cut that recorded 'unrecorded' -> FAILED" \
    "The same trap in the other direction, which 4b's comment calls out by name. B4's bundle is the input, so this case also proves B4's INCOMPLETE was leniency towards a genuinely old manifest and not blindness."
  [ -f "${B4_TARBALL:-}" ] || { skip "B4 did not produce a tarball to tamper with"; case_end; return; }
  local b; b="$(fx_unpack "$B4_TARBALL" "$W/u.C3")"
  sed -i.bak "s|^\[orbis-compat-imports\]|orbis-compat-include-sha256=$INC_A\n[orbis-compat-imports]|" \
      "$b/mesa/manifest.txt"; rm -f "$b/mesa/manifest.txt.bak"
  grep -q '^orbis-compat-include-sha256=' "$b/mesa/manifest.txt" || bad "the tamper did not take"
  fx_resum "$b"
  do_verify "$b"
  expect_verdict FAILED
  expect_out "disagree about the include/ tree Mesa compiled"
  case_end
}

case_C4(){
  case_begin C4-commit-disagreement \
    "BUNDLE.txt and mesa/manifest.txt disagreeing about the commit -> FAILED (exit 1)" \
    "verify treats mesa/manifest.txt as the primary record and BUNDLE.txt's copy as a transcription. A disagreement means one of the two was edited after the cut, and check 4 exists to say so rather than to pick a winner."
  local b; b="$(fx_unpack "$BASE_TARBALL" "$W/u.C4")"
  sed -i.bak "s/^orbis-compat-commit=.*/orbis-compat-commit=$SHA_C/" "$b/mesa/manifest.txt"
  rm -f "$b/mesa/manifest.txt.bak"
  fx_resum "$b"
  do_verify "$b"
  expect_verdict FAILED
  expect_out "disagree about which orbis-compat Mesa saw"
  case_end
}

case_C5(){
  case_begin C5-unaccounted-extra-file \
    "a file SHA256SUMS does not account for -> FAILED (exit 1)" \
    "THE CONTROL FOR EVERY OTHER C CASE. Those all re-hash before verifying; if fx_resum silently did nothing they would be testing check 1 by accident. This one does not re-hash, so check 1 has to bite - and an EXTRA file is drift too, which is the half a plain checksum run misses."
  local b; b="$(fx_unpack "$BASE_TARBALL" "$W/u.C5")"
  printf 'added by hand\n' > "$b/orbis-compat/SMUGGLED.txt"
  do_verify "$b"
  expect_verdict FAILED
  expect_out "SHA256SUMS does not account for"
  case_end
}

case_C6(){
  case_begin C6-mesa-manifest-removed \
    "mesa/manifest.txt gone -> FAILED with check 4's message, not a silent early death" \
    "REGRESSION for verify's mesa_field guard: 'sed missing-file | head -1' exits 2, pipefail hands that to the assignment and set -e ends the run there - so a bundle with no mesa/manifest.txt died at rc=2 printing nothing instead of reaching the message that says what is wrong with it."
  local b; b="$(fx_unpack "$BASE_TARBALL" "$W/u.C6")"
  rm -f "$b/mesa/manifest.txt"
  fx_resum "$b"
  do_verify "$b"
  expect_verdict FAILED
  expect_out "The pairing cannot be checked at all"
  expect_out "6. the layout a consumer's build reads"   # it got all the way to the end
  case_end
}

# =========================================================================================
# D. THE DIRTY TREE - defect 2, the deadlock the gate had against itself.
# =========================================================================================

# D1 and D2 share one fixture and either may run alone - `run.sh D2` has to mean something, or
# a bisect over a suspected regression cannot narrow to one case. Idempotent on purpose.
setup_D(){
  [ -f "$W/mesa.D/manifest.txt" ] || fx_mesa "$W/mesa.D" "$SHA_A" "$INC_A"
  fx_compat_at "$W/compat" a
  # ⚠ README.md, NOT A HEADER. Dirtying include/ moves the fingerprint too, and the cut would
  # then refuse on the PAIRING - a different check, a different message, and D2 below could no
  # longer claim the dirt is the only thing wrong with its bundle. Found 2026-09-18 by the first
  # run of this suite, which did exactly that and read the pairing refusal as a dirt refusal.
  printf '\nuncommitted\n' >> "$W/compat/README.md"
}

case_D1(){
  case_begin D1-dirty-tree-refused-at-the-cut \
    "a dirty orbis-compat refuses to be cut without --allow-dirty (exit 1)" \
    "The control for D2: the dirt has to be REFUSED somewhere, or D2's leniency would be the absence of a check rather than the right verdict in the right place. The cut is where publication is decided."
  setup_D
  cap env "ORBIS_BUNDLE_GATE=unproven" "$REL/make-sdk-bundle.sh" --version d1 --sdk "$W/sdk" \
        --mesa "$W/mesa.D" --compat "$W/compat" --out "$W/out.d1"
  expect_verdict FAILED
  expect_out "working tree is dirty"
  case_end
}

case_D2(){
  case_begin D2-dirty-is-INCOMPLETE-not-FAILED \
    "a bundle cut --allow-dirty verifies INCOMPLETE (exit 4), never FAILED (exit 1)" \
    "DEFECT 2, verbatim: as rc=1 this was a deadlock. bundle-gate.sh runs verify as a precondition and tolerates exit 4 because gate=unproven is the state it exists to fix - but exit 1 made it refuse, so a bundle cut from a working tree could never reach the only thing that can stamp gate=pass. Development bundles are dirty by definition."
  setup_D
  cap env "ORBIS_BUNDLE_GATE=pass" "$REL/make-sdk-bundle.sh" --version d2 --sdk "$W/sdk" \
        --mesa "$W/mesa.D" --compat "$W/compat" --out "$W/out.d2" --allow-dirty
  expect_verdict OK
  D2_TARBALL="$W/out.d2/release/orbis-sdk-d2.tar.gz"
  local b; b="$(fx_unpack "$D2_TARBALL" "$W/u.D2")"
  [ "$(bfield "$b" orbis-compat-dirty)" = yes ] || bad "orbis-compat-dirty=$(bfield "$b" orbis-compat-dirty), wanted yes"
  do_verify "$D2_TARBALL"
  expect_verdict INCOMPLETE
  expect_out "orbis-compat-dirty=yes"
  expect_out "Buildable and gateable, but NOT publishable"
  expect_no_out "FAILED - do not publish"
  # The dirt is the ONLY reason this is not OK: everything else about the bundle is the
  # baseline. If some other check had also gone INCOMPLETE this case would be proving nothing.
  expect_no_out "NOT CHECKED"
  fx_compat_at "$W/compat" a
  case_end
}

case_D3(){
  case_begin D3-gate-continues-past-INCOMPLETE \
    "bundle-gate.sh does NOT refuse a bundle whose offline verify came back INCOMPLETE" \
    "The other half of defect 2, and the half that was the actual deadlock. Only stages 0, 1 and 1b are reachable here - the synthetic SDK has no cross compiler behind it, so stage 3 fails immediately afterwards and that is expected. What is asserted is that stage 1b let it through."
  [ -f "${D2_TARBALL:-}" ] || { skip "D2 produced no tarball"; case_end; return; }
  local miss=() t
  for t in clang clang++ ld.lld llvm-ar llvm-ranlib llvm-nm cmake; do
    command -v "$t" >/dev/null 2>&1 || miss+=("$t")
  done
  if [ ${#miss[@]} -ne 0 ]; then
    skip "bundle-gate.sh stage 0 needs ${miss[*]} - not on this host's PATH"; case_end; return
  fi
  cap with_timeout 300 "$REL/bundle-gate.sh" "$D2_TARBALL"
  expect_out "offline verify INCOMPLETE (rc=4) - continuing"
  expect_no_out "offline verify FAILED"
  # It must then fail in stage 3, because a synthetic SDK cannot compile anything. A pass here
  # would mean the gate had been satisfied by something that is not a cross build.
  expect_verdict FAILED
  expect_out "3. hello-world from the unpacked bundle"
  case_end
}

# =========================================================================================
# E. THE IMPORT LIST - defect 1.
# =========================================================================================

case_E1(){
  case_begin E1-cxx-import-names-match \
    "every Itanium-ABI C++ name Mesa imports is found in the real archive" \
    "DEFECT 1: 'sed s/^_//' was there for Mach-O's leading underscore and on an ELF archive truncated every _Z name - _Znwm, _Znam, _ZnwmRKSt9nothrow_t - none of which could then match the unstripped wanted list. operator new and operator new[] are imported by libvulkan_radeon.a, libEGL.a and libgallium, so this was every C++ import in the bundle, on every host."
  local b; b="$(fx_unpack "$BASE_TARBALL" "$W/u.E1")"
  sed -n '/^\[orbis-compat-imports\]/,$p' "$b/BUNDLE.txt" | grep -qx '_Znwm' \
    || bad "the fixture's import list has no _Z name - this case would assert nothing"
  do_verify "$BASE_TARBALL"
  expect_verdict OK
  expect_out "all 12 imports satisfied"
  case_end
}

case_E2(){
  case_begin E2-truncated-names-are-refused \
    "an import list carrying the TRUNCATED spellings is refused (exit 1)" \
    "THE DECOY FOR E1. If check 5 matched loosely, or stripped underscores on both sides, or read no symbols at all, E1 would pass anyway. These are the exact strings the old sed produced; the archive defines _Znwm and does not define Znwm, so the leading underscore has to be load-bearing in one direction and only one."
  { printf 'Znam\nZnwm\nZnwmRKSt9nothrow_t\n'; fx_imports_default | tail -n +4; } > "$W/imports.trunc"
  fx_mesa "$W/mesa.E2" "$SHA_A" "$INC_A" "$W/imports.trunc"
  GATE=pass do_cut e2 a "$W/mesa.E2"
  expect_verdict OK
  do_verify "$TARBALL"
  expect_verdict FAILED
  expect_out "does not define names the bundle's Mesa imports"
  expect_out_re '^ +Znwm$'
  case_end
}

case_E3(){
  case_begin E3-empty-import-list-is-a-failure \
    "a bundle recording no imports at all is refused (exit 1)" \
    "verify's own reasoning: 'Mesa's archives are known to import at least orbis_sysconf - orbis-compat's <unistd.h> makes sysconf a macro for it - so an empty list means the measurement failed, not that the coupling is gone.' An empty list must never read as 'nothing to check, therefore fine'."
  : > "$W/imports.empty"
  fx_mesa "$W/mesa.E3" "$SHA_A" "$INC_A" "$W/imports.empty"
  GATE=pass do_cut e3 a "$W/mesa.E3"
  expect_verdict OK
  do_verify "$TARBALL"
  expect_verdict FAILED
  expect_out "records no orbis-compat imports at all"
  case_end
}

# =========================================================================================
# F. bundle-gate.sh's two defects. Neither stage is reachable on this host, so each is tested
#    against the REAL SOURCE LINE rather than a paraphrase of it.
# =========================================================================================

case_F1(){
  case_begin F1-nm-count-survives-pipefail \
    "stage 4's overlay-presence line, evaluated as written, reports the symbols that are there" \
    "DEFECT 5: 'grep -q' exits at the first match, SIGPIPEs llvm-nm, and pipefail takes the dead writer's 141 - so a MATCHING grep reported the overlay ABSENT from an image containing 16 of its symbols. The accusation it printed (--whole-archive silently did nothing) is credible enough to have cost a real investigation."
  local line; line="$(grep -n 'nsyms=' "$REL/bundle-gate.sh" | head -1 | cut -d: -f2-)"
  [ -n "$line" ] && printf '%s' "$line" | grep -q 'grep -c' \
    || bad "could not find stage 4's counted nsyms= line in bundle-gate.sh"
  command -v llvm-nm >/dev/null 2>&1 || { skip "no llvm-nm on this host; stage 4's line names it directly"; case_end; return; }
  local elf="$REPO/build/liborbis-compat.a"
  [ -f "$elf" ] || { skip "no build/liborbis-compat.a - run ./build.sh first"; case_end; return; }

  # The real line, under the real shell options, against a real ELF archive with real orbis_*
  # symbols in it.
  local got
  got="$(ELF="$elf" bash -c "set -euo pipefail; ELF=\"\$ELF\"; $line; echo \"\${nsyms:-0}\"" 2>/dev/null | tail -1)"
  [ "${got:-0}" -gt 0 ] 2>/dev/null || bad "the counted form reported '$got' orbis_ symbols; the archive has some"
  note "counted form: $got orbis_ symbols"

  # ⚠ THE DECOY IS THE HISTORICAL LINE. If it no longer misreports on this host the race is not
  # reproducing today and this case is not evidence - which is reported, not papered over.
  local old
  old="$(ELF="$elf" bash -c 'set -euo pipefail
    if llvm-nm "$ELF" 2>/dev/null | grep -qE " [tTwW] .*orbis_"; then echo PRESENT; else echo "ABSENT($?)"; fi' 2>/dev/null | tail -1)"
  note "historical 'grep -q' form on the same file: $old"
  case "$old" in
    ABSENT*) : ;;
    *) skip "the SIGPIPE race did not reproduce here (grep -q said $old), so the decoy proves nothing on this host today" ;;
  esac
  case_end
}

# The checker F2 runs twice: once over bundle-gate.sh, once over a copy with the --work
# argument taken back out.
check_stage5_isolation(){ # check_stage5_isolation <file> ; 0 = isolated, 1 = not
  local f="$1"
  grep -q 'PORT_WORK="\$WORK/port"' "$f" || return 1
  grep -q '\./ps4/build\.sh --work "\$PORT_WORK"' "$f" || return 1
  grep -q 'WORK="\$(mktemp -d)"' "$f" || return 1
  return 0
}

case_F2(){
  case_begin F2-port-build-dir-is-per-run \
    "stage 5 hands the port a build directory created for this run only" \
    "DEFECT 6: ORBIS_WORK alone does not reach the port - OpenGothic's ps4/build.sh sets its own WORK=~/.cache/opengothic-ps4 and only --work overrides it - so stage 5 built in a persistent cache and run N compiled against run N-1's DELETED sysroot. That is README §9 trap 7 happening inside the script whose header quotes that trap as its reason to exist. Stage 5 needs a three-hour port build to reach, so this reads the invocation instead."
  check_stage5_isolation "$REL/bundle-gate.sh" \
    || bad "stage 5 no longer passes --work \"\$PORT_WORK\" out of a per-run mktemp -d"
  # ⚠ THE DECOY: the script as it was before the fix.
  local decoy="$W/bundle-gate.decoy.sh"
  sed 's|\./ps4/build\.sh --work "\$PORT_WORK"|./ps4/build.sh|' "$REL/bundle-gate.sh" > "$decoy"
  cmp -s "$decoy" "$REL/bundle-gate.sh" && bad "the decoy is identical to the original - the check proves nothing"
  check_stage5_isolation "$decoy" && bad "the checker passed a copy that lets the port pick its own build directory"
  case_end
}

# =========================================================================================
# G. hello/CMakeLists.txt - defects 3 and 4. The gate's stage 3 is what found both, and
#    stage 3 needs a cross toolchain; these reach them another way.
# =========================================================================================

check_guard_below_project(){ # 0 = below, 1 = above/absent
  local f="$1" p g
  p="$(grep -n '^project(' "$f" | head -1 | cut -d: -f1)"
  g="$(grep -n 'NOT DEFINED ORBIS_COMPAT_DIR' "$f" | head -1 | cut -d: -f1)"
  [ -n "$p" ] && [ -n "$g" ] || return 1
  [ "$g" -gt "$p" ]
}

case_G1(){
  case_begin G1-hello-guard-below-project \
    "the ORBIS_COMPAT_DIR/PS4 guard sits after project()" \
    "DEFECT 3: CMake reads CMAKE_TOOLCHAIN_FILE when project() runs, not when the cache is populated, so above that line nothing the toolchain file sets exists yet. The guard therefore refused EVERY configure with a message telling the caller to pass exactly the flag they had just passed."
  local f="$REL/hello/CMakeLists.txt"
  check_guard_below_project "$f" || bad "the guard is at or above project() in $f"
  local decoy="$W/hello-decoy-CMakeLists.txt"
  { echo 'cmake_minimum_required(VERSION 3.16)'
    echo 'if(NOT DEFINED ORBIS_COMPAT_DIR OR NOT PS4)'
    echo '  message(FATAL_ERROR "configure with -DCMAKE_TOOLCHAIN_FILE=...")'
    echo 'endif()'
    echo 'project(orbis_hello C)'; } > "$decoy"
  check_guard_below_project "$decoy" && bad "the checker accepted a guard placed above project()"
  case_end
}

case_G2(){
  case_begin G2-toolchain-vars-invisible-above-project \
    "CMake really does leave toolchain-file variables undefined until project() runs" \
    "G1 asserts a line ordering; this asserts the FACT that makes the ordering matter, by running cmake. If this ever stopped being true the guard could move back and G1 would be enforcing a rule with no reason behind it. project(NONE) needs no compiler, so it costs about a second."
  command -v cmake >/dev/null 2>&1 || { skip "no cmake on this host"; case_end; return; }
  local d="$W/g2"; mkdir -p "$d/src"
  echo 'set(FX_FROM_TOOLCHAIN 1)' > "$d/tc.cmake"
  { echo 'cmake_minimum_required(VERSION 3.16)'
    echo 'if(DEFINED FX_FROM_TOOLCHAIN)'
    echo '  message(FATAL_ERROR "FX-VISIBLE-ABOVE-PROJECT")'
    echo 'endif()'
    echo 'project(fx NONE)'
    echo 'if(NOT DEFINED FX_FROM_TOOLCHAIN)'
    echo '  message(FATAL_ERROR "FX-INVISIBLE-BELOW-PROJECT")'
    echo 'endif()'
    echo 'message(STATUS "FX-AS-DOCUMENTED")'; } > "$d/src/CMakeLists.txt"
  cap with_timeout 120 cmake -S "$d/src" -B "$d/build" -DCMAKE_TOOLCHAIN_FILE="$d/tc.cmake"
  expect_verdict OK
  expect_out "FX-AS-DOCUMENTED"
  case_end
}

# Configures scripts/release/hello against a STAND-IN toolchain file that reproduces the one
# thing under test - ps4-openorbis.cmake's CMAKE_EXE_LINKER_FLAGS_INIT force-loading the
# archive - and counts how many times the archive lands on the generated link line.
# Echoes the count, or "-" if link.txt was never generated.
hello_archive_count(){ # hello_archive_count <srcdir> <builddir> <fake-archive>
  # ⚠ TWO `local` STATEMENTS, NOT ONE. bash expands every word of a `local` line before the
  # builtin assigns any of them, so `local bld="$2" d="${bld}.tc"` reads $bld while it is still
  # unset and dies under set -u. Found 2026-09-18 by the first run of this suite.
  local src="$1" bld="$2" lib="$3"
  local d="${bld}.tc"
  mkdir -p "$d"
  { echo 'set(CMAKE_SYSTEM_NAME Generic)'
    echo 'set(CMAKE_SYSTEM_PROCESSOR x86_64)'
    # Generic + STATIC_LIBRARY is what ps4-openorbis.cmake relies on too: CMake must not try to
    # LINK during its compiler check, because the link line under test is deliberately absurd
    # for the host.
    echo 'set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)'
    echo 'set(PS4 1)'
    echo "set(ORBIS_COMPAT_DIR \"$(dirname "$(dirname "$lib")")\")"
    echo "set(ORBIS_COMPAT_LIBRARY \"$lib\")"
    echo 'set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--whole-archive ${ORBIS_COMPAT_LIBRARY} -Wl,--no-whole-archive")'
    echo "set(CMAKE_PROJECT_INCLUDE \"$d/after-project.cmake\")"; } > "$d/tc.cmake"
  # The real orbis-compat.cmake defines orbis::compat with --whole-archive already on its
  # INTERFACE_LINK_LIBRARIES; the decoy has to be able to resolve it or it would fail for the
  # wrong reason.
  { echo 'add_library(orbis_compat INTERFACE)'
    echo 'set_property(TARGET orbis_compat PROPERTY INTERFACE_LINK_LIBRARIES'
    echo '  "-Wl,--whole-archive" "${ORBIS_COMPAT_LIBRARY}" "-Wl,--no-whole-archive")'
    echo 'add_library(orbis::compat ALIAS orbis_compat)'
    echo 'macro(orbis_compat_verify)'
    echo 'endmacro()'; } > "$d/after-project.cmake"
  with_timeout 180 cmake -G "Unix Makefiles" -S "$src" -B "$bld" \
      -DCMAKE_TOOLCHAIN_FILE="$d/tc.cmake" -DCMAKE_BUILD_TYPE=Release >"$d/configure.log" 2>&1
  local lt; lt="$(find "$bld" -name link.txt -print -quit 2>/dev/null)"
  [ -n "$lt" ] || { echo "-"; return; }
  grep -o -F -- "$lib" "$lt" | wc -l | tr -d ' '
}

case_G3(){
  case_begin G3-overlay-force-loaded-once \
    "hello's generated link line names the overlay archive exactly once" \
    "DEFECT 4: the toolchain file already force-loads the archive on every executable, and hello also carried target_link_libraries(orbis::compat) - so --whole-archive ran twice, every member was loaded twice, and ld.lld died on 'duplicate symbol: clock_gettime ... the same object, in the same archive, twice'. The real bundle's toolchain file needs a cross SDK, so a stand-in reproduces the one line that matters and CMake generates the real link.txt from hello's real CMakeLists.txt."
  command -v cmake >/dev/null 2>&1 || { skip "no cmake on this host"; case_end; return; }
  local lib="$W/g3/fake/build/liborbis-compat.a"
  mkdir -p "$(dirname "$lib")"; : > "$lib"
  cp -a "$REL/hello" "$W/g3/hello"
  local n; n="$(hello_archive_count "$W/g3/hello" "$W/g3/b" "$lib")"
  [ "$n" = 1 ] || bad "the archive appears $n time(s) on hello's link line, wanted exactly 1"

  # ⚠ THE DECOY: hello as it was when the gate first ran it.
  cp -a "$REL/hello" "$W/g3/hello-decoy"
  echo 'target_link_libraries(hello PRIVATE orbis::compat)' >> "$W/g3/hello-decoy/CMakeLists.txt"
  local m; m="$(hello_archive_count "$W/g3/hello-decoy" "$W/g3/bd" "$lib")"
  note "decoy (with the second force-load put back): $m"
  [ "$m" = 2 ] || bad "the decoy produced $m occurrence(s), not 2 - the counter is not measuring what it claims"
  case_end
}

# =========================================================================================
ALL=(A1 A2 A3 B1 B2 B3 B4 B5 B6 B7 C1 C2 C3 C4 C5 C6 D1 D2 D3 E1 E2 E3 F1 F2 G1 G2 G3)
if [ "$LIST" -eq 1 ]; then printf '%s\n' "${ALL[@]}"; exit 0; fi

printf '%s==>%s scripts/release test suite - %s\n' "$C_HD" "$C_Z" "$(date -u '+%Y-%m-%dT%H:%M:%SZ')"
printf '    repo   %s\n    work   %s\n\n' "$REPO" "$W"

[ -f "$REPO/build/liborbis-compat.a" ] || {
  echo "no $REPO/build/liborbis-compat.a - run ./build.sh first. Checks 5, E1 and E2 read the" >&2
  echo "real archive with real C++ symbols in it and a stub would make them assert nothing." >&2
  exit 2; }

setup
# The baseline bundle every A/C/E case tampers with a copy of. Cut once: it is the coherent
# pair, cut clean, with the gate already recorded, which is the only shape that reaches exit 0.
fx_mesa "$W/mesa.base" "$SHA_A" "$INC_A"
GATE=pass do_cut base a "$W/mesa.base"
BASE_TARBALL="$TARBALL"
[ "$RC" -eq 0 ] || { echo "the baseline cut failed (exit $RC); nothing below can mean anything:" >&2
                     cat "$CAP" >&2; exit 1; }
B4_TARBALL=""; D2_TARBALL=""

for c in "${ALL[@]}"; do
  if [ ${#ONLY[@]} -gt 0 ]; then
    printf '%s\n' "${ONLY[@]}" | grep -qx "$c" || continue
  fi
  "case_$c"
done

printf '%s==>%s %s passed, %s failed, %s skipped\n' "$C_HD" "$C_Z" "$N_PASS" "$N_FAIL" "$N_SKIP"
[ "$N_SKIP" -eq 0 ] || printf '    skipped: %s\n' "${SKIPPED[@]}"
if [ "$NOSKIP" -eq 1 ] && [ "$N_SKIP" -ne 0 ]; then
  printf '    %s--no-skips was given and %s case(s) skipped - on a host that has the tools,\n' "$C_NO" "$N_SKIP"
  printf '    a skip is a hole, not a result.%s\n' "$C_Z"
  exit 1
fi
if [ "$N_FAIL" -ne 0 ]; then
  printf '    %sfailed: %s%s\n' "$C_NO" "${FAILED[*]}" "$C_Z"
  exit 1
fi
printf '    %sall green%s\n' "$C_OK" "$C_Z"
