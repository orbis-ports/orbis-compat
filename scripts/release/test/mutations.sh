#!/usr/bin/env bash
# Copyright © 2026 Mikołaj Mikołajczyk
# SPDX-License-Identifier: MIT
#
# THE SUITE IS ITSELF UNDER TEST. This script puts each of the 2026-09-17 defects back into the
# release scripts, one at a time, runs the case that is supposed to catch it, and requires that
# case to go RED. Then it puts the file back.
#
#   scripts/release/test/mutations.sh [<mutation-name-substring>]
#
# WHY IT EXISTS. run.sh going green proves that twenty-seven assertions held. It does NOT prove
# that any of them could have failed - and "a check that cannot fail" is precisely the shape of
# five of the six defects this whole exercise is about: `grep -q` under pipefail always said
# ABSENT, the truncating sed always said MISSING, the guard above project() always fired. A
# suite that inherited that shape would report green and stop anyone looking. build.sh already
# holds test/crt_abi.sh to this standard by running it against a decoy object and requiring a
# non-zero exit; this is the same idea applied to a suite instead of to one comparison.
#
# ⚠ IT EDITS THE WORKING TREE AND PUTS IT BACK. Every file it touches is copied first and
# restored after each mutation, including on failure, and the last thing it does is compare
# every file against its copy byte for byte and refuse to exit 0 if any differ. Run it on a
# tree you have committed or stashed if that still makes you nervous; it is not run by run.sh
# for exactly that reason.
#
# ⚠ A MUTATION THAT NO LONGER APPLIES IS A FAILURE, NOT A SKIP. If the string it patches has
# moved, the patch is a no-op, the suite stays green for the honest reason, and this script
# would read that as "the defect was caught". It is not. Each mutation asserts it matched
# exactly once.
#
# Exit codes: 0 every defect was caught and every file restored · 1 otherwise · 2 usage.
set -uo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
REL="$(cd "$HERE/.." && pwd -P)"
ONLY="${1:-}"

BK="$(mktemp -d)"
FILES=(verify-sdk-bundle.sh bundle-gate.sh make-sdk-bundle.sh hello/CMakeLists.txt)
for f in "${FILES[@]}"; do
  mkdir -p "$BK/$(dirname "$f")"; cp "$REL/$f" "$BK/$f"
done
restore(){ local f; for f in "${FILES[@]}"; do cp "$BK/$f" "$REL/$f"; done; }
trap 'restore; rm -rf "$BK"' EXIT

RC=0
# ⚠ python rather than sed: these patches contain slashes, backslashes, dollars and newlines,
# and the point of the exercise is defeated by a patch that silently half-applies.
patch_file(){ # patch_file <path> <old> <new>
  python3 - "$1" "$2" "$3" <<'PY'
import sys
p, old, new = sys.argv[1], sys.argv[2], sys.argv[3]
s = open(p).read()
n = s.count(old)
if n != 1:
    sys.exit("expected exactly 1 occurrence of the anchor, found %d" % n)
open(p, 'w').write(s.replace(old, new, 1))
PY
}

mutate(){ # mutate <name> <file-relative-to-scripts/release> <old> <new> <case...>
  local name="$1" file="$2" old="$3" new="$4"; shift 4
  [ -z "$ONLY" ] || case "$name" in *"$ONLY"*) ;; *) return 0 ;; esac
  printf '\033[1;34m==>\033[0m %s\n' "$name"
  printf '    expected to be caught by: %s\n' "$*"
  if ! patch_file "$REL/$file" "$old" "$new"; then
    printf '    \033[1;31mFAIL\033[0m the mutation no longer applies - the anchor moved, so this\n'
    printf '         proves nothing and the suite going green would be meaningless here.\n'
    RC=1; restore; return
  fi
  # bash -n first: a mutation that only breaks the syntax would make every case fail for the
  # wrong reason, and that is not evidence about the assertion under test.
  case "$file" in
    *.sh) bash -n "$REL/$file" || { printf '    \033[1;31mFAIL\033[0m the mutation broke the shell syntax\n'; RC=1; restore; return; } ;;
  esac
  if "$HERE/run.sh" "$@" >"$BK/out.log" 2>&1; then
    printf '    \033[1;31mFAIL\033[0m the suite stayed GREEN with the defect back in\n'
    sed 's/^/         | /' "$BK/out.log" | tail -20
    RC=1
  else
    printf '    \033[1;32mcaught\033[0m - %s assertion(s) fired\n' "$(grep -c '^     !!' "$BK/out.log" || true)"
    grep '^     !!' "$BK/out.log" | sed 's/^     !!/         >/' | head -6
  fi
  restore
}

# ------------------------------------------------------------------------------ defect 1
# verify-sdk-bundle.sh check 5. The strip was there for Mach-O's leading underscore; on an ELF
# archive it truncates every Itanium-ABI name, all of which begin `_Z`. The wanted list is not
# stripped, so _Znwm, _Znam and _ZnwmRKSt9nothrow_t could never match - every C++ import in the
# bundle, on every host.
mutate "defect 1: sed 's/^_//' back in verify check 5" verify-sdk-bundle.sh \
  '    | LC_ALL=C sort -u > "$have"' \
  "    | sed 's/^_//' \\
    | LC_ALL=C sort -u > \"\$have\"" \
  E1 E2

# ------------------------------------------------------------------------------ defect 2
# verify-sdk-bundle.sh, the dirty branch. As RC=1 this was a deadlock: bundle-gate.sh refuses a
# bundle whose verify FAILED, so a bundle cut from a working tree could never reach the only
# thing that can stamp gate=pass.
mutate "defect 2: a dirty tree FAILED instead of INCOMPLETE" verify-sdk-bundle.sh \
  'committed tree."
  INCOMPLETE=1' \
  'committed tree."
  RC=1' \
  D2 D3

# ------------------------------------------------------------------------------ defect 4
# hello/CMakeLists.txt. The toolchain file already force-loads the archive on every executable;
# adding the target on top of it loaded every member twice and ld.lld died on duplicate symbols.
mutate "defect 4: the overlay force-loaded twice" hello/CMakeLists.txt \
  'add_executable(hello hello.c)' \
  'add_executable(hello hello.c)
target_link_libraries(hello PRIVATE orbis::compat)' \
  G3

# ------------------------------------------------------------------------------ defect 6
# bundle-gate.sh stage 5. ORBIS_WORK alone does not reach the port, so without --work the gate
# built in a persistent cache and run N compiled against run N-1's deleted sysroot.
mutate "defect 6: stage 5 picks its own build directory" bundle-gate.sh \
  './ps4/build.sh --work "$PORT_WORK"' \
  './ps4/build.sh' \
  F2

# ------------------------------------------------------------------------------ defect 5
# bundle-gate.sh stage 4. `grep -q` exits at the first match, SIGPIPEs llvm-nm, pipefail takes
# the dead writer's 141 - so a MATCHING grep reported the overlay ABSENT.
mutate "defect 5: grep -q under pipefail back in stage 4" bundle-gate.sh \
  '  nsyms="$(llvm-nm "$ELF" 2>/dev/null | grep -cE '"'"' [tTwW] .*orbis_'"'"' || true)"
  if [ "${nsyms:-0}" -gt 0 ]; then' \
  '  nsyms=1
  if llvm-nm "$ELF" 2>/dev/null | grep -qE '"'"' [tTwW] .*orbis_'"'"'; then' \
  F1

# ------------------------------------------------------------------------------ the pairing rule
# make-sdk-bundle.sh. The rule before 2026-09-17 was commit-id equality, which refused a bundle
# over cc75949..4f61d6f - three commits touching LICENSING.md, build.sh, crt/orbis_crt1.c and
# make-sdk-bundle.sh, whose `git diff -- include/` is EMPTY. Taking the git comparison away
# leaves HEADERS_DIFFER=unknown for any pair of differing commits, which is that rule exactly.
#
# ⚠ B2 ONLY, AND B3 IS THE REASON WHY. B3's Mesa manifest carries the fingerprint, and the
# fingerprint block below this one overrides the git verdict on purpose - "it compares two
# TREES, one of them measured on the machine that ran the compiler". So B3 survives this
# mutation, correctly, and listing it here would make a caught defect read as an uncaught one.
mutate "the old rule: refuse on the commit id rather than on include/" make-sdk-bundle.sh \
  'elif git -C "$COMPAT" cat-file -e "${MESA_BUILT_AGAINST}^{tree}" 2>/dev/null; then' \
  'elif false; then' \
  B2

# ------------------------------------------------------------------------------ the fingerprint
# verify-sdk-bundle.sh 4b. Accepting an absent Mesa fingerprint as a pass - rather than as NOT
# CHECKED - is the failure mode the whole of 4b's header argues against: one number, read twice.
mutate "4b: a stripped Mesa fingerprint buys the lenient branch" verify-sdk-bundle.sh \
  'if [ -n "$INC_CUT_SAYS_MESA" ] && [ "$_cut_says" != "$INC_MESA" ]; then' \
  'if false; then' \
  C2 C3

printf '\n'
for f in "${FILES[@]}"; do
  cmp -s "$BK/$f" "$REL/$f" || { printf '\033[1;31mXX\033[0m %s was NOT restored\n' "$f"; RC=1; }
done
if [ "$RC" -eq 0 ]; then printf '\033[1;32m==>\033[0m every defect was caught, every file restored\n'
else printf '\033[1;31m==>\033[0m see above\n'; fi
exit "$RC"
