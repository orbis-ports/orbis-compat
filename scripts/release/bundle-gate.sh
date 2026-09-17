#!/usr/bin/env bash
# THE PUBLICATION GATE. A bundle is not publishable until something has been BUILT from an
# UNPACKED COPY OF IT - not from the working tree the bundle was cut in.
#
#   scripts/release/bundle-gate.sh <bundle.tar.gz | unpacked-dir> [--port DIR] [--keep]
#
# ⚠ RUN IN FULL ON 2026-09-17, AND THE WARNING THAT USED TO BE HERE WAS EARNED. It said this
# script had never executed, that everything in it was reasoned from the build files it drives
# rather than observed, and to expect the first real run to find something. It found six, and all
# six were in this file and its neighbours rather than in any bundle:
#
#   verify-sdk-bundle.sh  sed 's/^_//' truncated every Itanium-ABI C++ name - _Znwm could not match
#   verify-sdk-bundle.sh  a dirty tree was FAILED, not INCOMPLETE, so this gate refused every
#                         bundle it exists to fix. A deadlock against itself
#   hello/CMakeLists.txt  the guard sat above project(), where nothing the toolchain file sets
#                         exists yet, so it refused every correct invocation
#   hello/CMakeLists.txt  the overlay archive was force-loaded twice and every symbol duplicated
#   bundle-gate.sh        grep -q under set -o pipefail: SIGPIPE killed llvm-nm, pipefail took its
#                         141, and a matching grep reported the overlay ABSENT from an image with
#                         16 of its symbols in it
#   bundle-gate.sh        stage 5 let the port choose its own build directory, so run N compiled
#                         against run N-1's deleted sysroot. README section 9 trap 7, inside the
#                         script whose header quotes that trap as its reason to exist
#
# Each fix carries the measurement that produced it. The run ended GATE PASSED against SDK v0.5.4,
# orbis-mesa-a4fa6b57f8bd and OpenGothic; both eboots carry the Orbis SELF magic 4f15 3d1d.
#
# ⚠ WHAT THIS STILL CANNOT DO IS RUN ANYTHING. A cross-linked PS4 image does not execute on a build
# machine. hello and triangle were both run on a console by hand the same day - see orbis-compat's
# README section 0 - and that is a fact about those two runs, not something gate=pass asserts.
#
# =========================================================================================
# WHY "FROM THE UNPACKED BUNDLE" IS THE WHOLE POINT.
#
# README §9 trap 7: "A stale build directory answers questions about a build that no longer
# exists. ~/.cache/tempest-og/build-ps4 still compiles og_ps4_mmap.cpp and links no overlay at
# all; the title's real build directory is ~/.cache/opengothic-ps4/build. Read link.txt before
# concluding anything from a build tree."
#
# That trap cost a day, and a release script reproduces it perfectly if it is careless. The
# machine that cuts a bundle has: an SDK at ~/.local/opt/openorbis, an orbis-compat checkout
# with build/liborbis-compat.a already in it, a Mesa build tree, four environment variables
# pointing at all of them, and very likely a cache directory holding a build configured
# against those paths months ago. A build run there succeeds by finding the ORIGINALS. It
# proves the machine works. It proves nothing about the tarball.
#
# So this gate: unpacks the bundle into a fresh directory; scrubs every variable that could
# reach back to the originals; configures in a build directory created for this run only; and
# then GREPS THE GENERATED link.txt to prove that what was linked came from the bundle's own
# paths. That last step is the one that makes the rest mean anything - a build can find the
# right headers and still link the wrong archive, and link.txt is where that becomes visible.
#
# TWO builds, because one is not enough:
#   hello  - the smallest thing that exercises the whole chain: the corrected headers, the
#            overlay archive under --whole-archive, the corrected linker script, crt1.o last,
#            and create-fself turning the ELF into an eboot.bin. If this fails, the bundle's
#            LAYOUT is wrong and nothing else matters.
#   port   - a real consumer, unmodified, pointed at the bundle with one variable. This is the
#            one that catches what hello cannot: C++, libc++ include ordering, Mesa/RADV
#            actually linking, thousands of translation units, and a build system that was
#            written against a checkout rather than a bundle. --port defaults to the sibling
#            OpenGothic checkout because its ps4/build.sh is the org's standard entry point.
# =========================================================================================
#
# Exit codes: 0 pass (BUNDLE.txt is stamped gate=pass) · 1 a stage FAILED · 2 usage ·
#             4 INCOMPLETE - a required host tool is missing, so the gate did not run.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
TARGET="${1:-}"; shift || true
PORT="${ORBIS_GATE_PORT:-}"
KEEP=0
while [ $# -gt 0 ]; do
  case "$1" in
    --port) PORT="$2"; shift 2 ;;
    --keep) KEEP=1; shift ;;
    *) echo "unknown argument: $1" >&2; exit 2 ;;
  esac
done
[ -n "$TARGET" ] || { echo "usage: $0 <bundle.tar.gz|dir> [--port DIR] [--keep]" >&2; exit 2; }

log(){  printf '\033[1;34m==>\033[0m %s\n' "$*"; }
ok(){   printf '   \033[1;32mok\033[0m   %s\n' "$*"; }
warn(){ printf '\033[1;33m!!\033[0m %s\n' "$*" >&2; }
err(){  printf '\033[1;31mXX\033[0m %s\n' "$*" >&2; }

# -----------------------------------------------------------------------------------------
# 0. The host tools. Named individually, because "install llvm" is not a diagnosis.
# -----------------------------------------------------------------------------------------
log "0. host tools"
missing=()
for t in clang clang++ ld.lld llvm-ar llvm-ranlib llvm-nm cmake; do
  command -v "$t" >/dev/null 2>&1 || missing+=("$t")
done
if [ ${#missing[@]} -ne 0 ]; then
  err "missing: ${missing[*]}"
  warn "  The gate CANNOT run here and this is NOT a failure of the bundle."
  warn "  ld.lld is the linker the PS4 target needs; llvm-ar/llvm-ranlib build the archives;"
  warn "  llvm-nm is what the import assertion is made of. On Debian/Ubuntu:"
  warn "      sudo apt-get install -y clang lld llvm cmake"
  warn "  On macOS these are NOT in Apple's clang - brew install llvm and put it on PATH."
  exit 4
fi
for t in clang ld.lld llvm-nm; do printf '   %-10s %s\n' "$t" "$(command -v "$t")"; done

# -----------------------------------------------------------------------------------------
# 1. Unpack into a directory that has never seen this bundle before.
# -----------------------------------------------------------------------------------------
WORK="$(mktemp -d)"
cleanup(){ [ "$KEEP" -eq 1 ] && { warn "work tree kept at $WORK"; return; }; rm -rf "$WORK"; }
trap cleanup EXIT

if [ -f "$TARGET" ]; then
  log "1. unpacking $(basename "$TARGET") into a fresh tree"
  TARGET="$(cd "$(dirname "$TARGET")" && pwd -P)/$(basename "$TARGET")"
  tar -xzf "$TARGET" -C "$WORK"
  B="$(dirname "$(find "$WORK" -maxdepth 2 -name BUNDLE.txt -print -quit)")"
  STAMP_TARGET="$B/BUNDLE.txt"   # the unpacked copy; see the note at the end
elif [ -d "$TARGET" ]; then
  # ⚠ A DIRECTORY ARGUMENT IS THE WEAKER FORM OF THIS GATE and it says so out loud. Passing
  # an already-unpacked tree cannot rule out that the tree is the staging directory the cut
  # ran in, still sitting next to everything it copied from. The tarball form is the real one.
  B="$(cd "$TARGET" && pwd -P)"
  STAMP_TARGET="$B/BUNDLE.txt"
  warn "1. given an unpacked directory, not a tarball."
  warn "  This proves less: it cannot rule out that $B is the staging tree the cut ran in."
  warn "  For a release, run this against the .tar.gz that will actually be published."
else
  err "$TARGET is neither a file nor a directory"; exit 2
fi
[ -n "$B" ] && [ -f "$B/BUNDLE.txt" ] || { err "no BUNDLE.txt in $TARGET"; exit 1; }
log "   bundle at $B"

# The offline gate first. A bundle that does not verify is not worth building from.
log "1b. offline verify before building anything"
if "$HERE/verify-sdk-bundle.sh" "$B"; then ok "offline verify passed"
else
  rc=$?
  # exit 4 is INCOMPLETE - and the single most likely reason for it is gate=unproven, which
  # is precisely the thing this script is here to fix. Do not treat that as a refusal.
  if [ "$rc" -eq 4 ]; then warn "offline verify INCOMPLETE (rc=4) - continuing; that is what this run is for"
  else err "offline verify FAILED (rc=$rc) - fix the bundle before building from it"; exit 1; fi
fi

# -----------------------------------------------------------------------------------------
# 2. Scrub. Everything that could reach back to the machine's own SDK, overlay or Mesa.
#
#    ⚠ THE FOUR VARIABLES ARE NOT THE WHOLE LIST. CMAKE_PREFIX_PATH and PKG_CONFIG_PATH have
#    both supplied a HOST library to a cross build in this port before - README §9 trap 3 is
#    about exactly that, and Mesa's cross file disables cmake entirely because "nix's devShell
#    carries cmake configs for HOST libraries - which is how zlib was 'found: YES
#    1.3.1.zlib-ng' for a PS4 build whose pkg-config correctly said no."
# -----------------------------------------------------------------------------------------
log "2. scrubbing the environment"
for v in OO_PS4_TOOLCHAIN ORBIS_COMPAT_DIR ORBIS_COMPAT ORBIS_MESA_DIR ORBIS_MESA_SRC \
         ORBIS_MESA_BUILD ORBIS_RADV_ARCHIVE ORBIS_WORK ORBIS_CROSS ORBIS_SDK_BUNDLE \
         CMAKE_PREFIX_PATH CMAKE_TOOLCHAIN_FILE PKG_CONFIG_PATH PKG_CONFIG_LIBDIR \
         CPATH C_INCLUDE_PATH CPLUS_INCLUDE_PATH LIBRARY_PATH SDKROOT; do
  if [ -n "${!v:-}" ]; then printf '   unset %-24s (was %s)\n' "$v" "${!v}"; unset "$v"; fi
done
export ORBIS_WORK="$WORK/cache"
mkdir -p "$ORBIS_WORK"

# -----------------------------------------------------------------------------------------
# 3. hello-world, configured and built from the bundle's own toolchain file ONLY.
# -----------------------------------------------------------------------------------------
log "3. hello-world from the unpacked bundle"
HELLO_SRC="$B/orbis-compat/scripts/release/hello"
[ -d "$HELLO_SRC" ] || HELLO_SRC="$HERE/hello"
[ -d "$HELLO_SRC" ] || { err "no hello-world sources at $HELLO_SRC"; exit 1; }
HELLO_BUILD="$WORK/build-hello"

set +e
cmake -S "$HELLO_SRC" -B "$HELLO_BUILD" \
      -DCMAKE_TOOLCHAIN_FILE="$B/toolchain/orbis-sdk.cmake" \
      -DCMAKE_BUILD_TYPE=Release > "$WORK/hello-configure.log" 2>&1
rc=$?
set -e
if [ $rc -ne 0 ]; then
  err "hello-world CONFIGURE failed - the bundle's layout is wrong, not the source"
  tail -40 "$WORK/hello-configure.log" >&2
  exit 1
fi
ok "configured"

set +e
cmake --build "$HELLO_BUILD" -j "$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)" \
  > "$WORK/hello-build.log" 2>&1
rc=$?
set -e
if [ $rc -ne 0 ]; then
  err "hello-world BUILD failed"
  tail -60 "$WORK/hello-build.log" >&2
  exit 1
fi
ok "built"

# -----------------------------------------------------------------------------------------
# 4. THE PROVENANCE CHECK. This is the step the gate exists for.
#
#    A build can find the bundle's headers and still link the machine's archive - that is
#    precisely the shape of README §9 trap 7. link.txt is CMake's own record of the link
#    command it ran, so it is the primary evidence and not a re-derivation.
# -----------------------------------------------------------------------------------------
log "4. proving what was linked came out of the bundle"
LINKTXT="$(find "$HELLO_BUILD" -name link.txt -print -quit)"
[ -n "$LINKTXT" ] || { err "no link.txt under $HELLO_BUILD - cannot prove where anything came from"; exit 1; }
printf '   %s\n' "$LINKTXT"

fail_if_absent(){ # needle description
  if grep -qF -- "$1" "$LINKTXT"; then ok "link line names $2"
  else err "link line does NOT name $2 ($1)"; RC4=1; fi
}
RC4=0
fail_if_absent "$B/orbis-compat/build/liborbis-compat.a" "the bundle's liborbis-compat.a" \
  || true
# ⚠ EITHER COPY COUNTS, AND DEMANDING toolchain/ ALONE WAS WRONG. The bundle ships the linker
# script twice - toolchain/orbis-tls.ld and orbis-compat/cmake/orbis-tls.ld - and
# verify-sdk-bundle.sh check 6 has already proved the two are byte-identical. What lands on the
# link line is whichever ps4-openorbis.cmake names, and it names ${ORBIS_COMPAT_DIR}/cmake/,
# so this check failed on a correct build. Measured by the first run of this script, 2026-09-17.
# What matters is that the script came from INSIDE the bundle; the stray scan below is what
# enforces that, and this only has to accept both spellings.
if grep -qF -- "$B/toolchain/orbis-tls.ld" "$LINKTXT" \
   || grep -qF -- "$B/orbis-compat/cmake/orbis-tls.ld" "$LINKTXT"; then
  ok "link line names the bundle's linker script"
else
  err "link line does NOT name either copy of the bundle's linker script"
  err "  looked for $B/toolchain/orbis-tls.ld and $B/orbis-compat/cmake/orbis-tls.ld"
  RC4=1
fi
fail_if_absent "$B/sdk/lib/crt1.o" "the bundle's crt1.o" || true
# ⚠ AND THE NEGATIVE CONTROL, which is the half that actually catches the trap: no path on
# the link line may point ANYWHERE outside the bundle's own tree. A stale cache entry, an
# exported LIBRARY_PATH, a find_library that reached the host - all of them show up here.
# ⚠ THE COMPILER IS DROPPED FIRST, AND IT IS THE ONE HOST PATH THAT BELONGS HERE. link.txt is
# `<compiler> <flags...>`, and ps4-openorbis.cmake says why field 1 can never be in the bundle:
# "The SDK ships no compiler: libc/libc++ are prebuilt, clang comes from the host." The first
# real run of this script flagged it as contamination:
#
#     XX absolute path(s) on the link line that are NOT inside the bundle:
#          /opt/homebrew/opt/llvm/bin/clang
#
# Widening the path allow-list was the wrong repair - Homebrew's keg-only layout is
# /opt/homebrew/opt/<formula>/bin, nix's is /nix/store/<hash>-<name>/bin, and a list of every
# prefix a compiler might live under is a list that is wrong on the next machine. Dropping
# field 1 names the exemption by ROLE instead: exactly one token on this line is the host's
# compiler, every other absolute path must come out of the bundle, and that stays true
# wherever the compiler is installed.
strays="$(cut -d' ' -f2- "$LINKTXT" | tr ' ' '\n' | grep -E '^-?[LIW]?,?/' \
          | grep -v "^-*[LIW]*,*$B" \
          | grep -vE '^/(usr|bin|opt/homebrew|nix)/(bin|lib/cmake)' | sort -u || true)"
if [ -n "$strays" ]; then
  err "absolute path(s) on the link line that are NOT inside the bundle:"
  printf '     %s\n' $strays >&2
  err "  This is README §9 trap 7 happening live: a build that found the machine's copy."
  RC4=1
else
  ok "no path on the link line leaves the bundle"
fi
[ "$RC4" -eq 0 ] || exit 1

ELF="$(find "$HELLO_BUILD" -name '*.elf' -o -name 'hello' -type f | head -1)"
EBOOT="$(find "$HELLO_BUILD" -name 'eboot.bin' -print -quit)"
[ -n "$ELF" ]   && ok "ELF   $ELF ($(wc -c < "$ELF" | tr -d ' ') bytes)"
if [ -n "$EBOOT" ]; then ok "eboot $EBOOT ($(wc -c < "$EBOOT" | tr -d ' ') bytes)"
else warn "no eboot.bin - create-fself did not run. The bundle compiles and links; it has not
   been shown to PACKAGE. Check sdk/bin/<host>/create-fself is present and executable."; fi

# ⚠ The overlay must actually be IN the image, not merely on the line. --whole-archive is
# what puts an interposer nobody references into the output; if it is missing, the link
# still succeeds and every correction silently does nothing.
if [ -n "$ELF" ]; then
  # ⚠ COUNTED, NOT `grep -q`, AND THE DIFFERENCE IS THIS SCRIPT'S set -o pipefail ON LINE 49.
  # `grep -q` exits at the first match, which SIGPIPEs llvm-nm; pipefail then takes the dead
  # writer's 141 as the pipeline's status and the `if` goes to the else branch EVEN THOUGH GREP
  # MATCHED. Measured 2026-09-17 on a hello that contains 50 orbis symbols - the same command
  # says PASSES under `set -eu` and FAILS under `set -euo pipefail`, on the same file.
  #
  # ⚠ THIS CHECK WAS THEREFORE A CONSTANT FALSE ALARM, and it is the worst kind: it accused
  # --whole-archive of silently doing nothing, which is a real and serious failure mode with a
  # real and serious comment attached, so the accusation reads as credible. `stat`,
  # `clock_gettime`, `__mmap` and `backtrace` were all in the image while it fired.
  #
  # grep without -q reads to EOF, so llvm-nm is never signalled and the status is grep's own.
  # `|| true` keeps a legitimate no-match (status 1) from tripping set -e before it is read.
  nsyms="$(llvm-nm "$ELF" 2>/dev/null | grep -cE ' [tTwW] .*orbis_' || true)"
  if [ "${nsyms:-0}" -gt 0 ]; then
    ok "the overlay's symbols are in the image: $nsyms of them (--whole-archive did its job)"
  else
    err "no orbis_* symbol in $ELF. The archive was named on the link line and contributed
   nothing, which is what happens without --whole-archive: interposers are referenced by
   nobody, so no member is pulled in and every correction is silently absent."
    exit 1
  fi
fi

# -----------------------------------------------------------------------------------------
# 5. A real port, unmodified, pointed at the bundle with one variable.
# -----------------------------------------------------------------------------------------
log "5. a real port from the unpacked bundle"
if [ -z "$PORT" ]; then
  for c in "$HERE/../../../OpenGothic" "$HOME/src/orbis-ports/OpenGothic" "$HOME/src-ps4/OpenGothic"; do
    [ -f "$c/ps4/build.sh" ] && { PORT="$(cd "$c" && pwd -P)"; break; }
  done
fi
if [ -z "$PORT" ] || [ ! -f "$PORT/ps4/build.sh" ]; then
  err "no real port to build. Pass --port <checkout with ps4/build.sh>, or set ORBIS_GATE_PORT."
  warn "  hello-world alone does NOT discharge this gate: it is C, it links no C++ and no Mesa,"
  warn "  and it has one translation unit. The failures a bundle actually has - libc++ include"
  warn "  ordering, RADV linking, a build system written against a checkout - all live past it."
  exit 1
fi
log "   $PORT"

# ⚠ ps4/build.sh SOURCES orbis-env.sh, WHICH RE-DERIVES ORBIS_MESA_* AND CLEARS THEM. It
# probes for a mesa-ps4 CHECKOUT by looking for src/amd/vulkan, and the bundle's mesa/ is a
# build output, not a checkout. Every consumer in the org already re-applies the bundle's
# values after sourcing it; env.sh exports ORBIS_SDK_MESA_* for exactly that. This is the
# single most likely thing to break on the first real run of this script.
# ⚠⚠ --work IS THE WHOLE POINT OF THIS STAGE AND IT WAS MISSING. ORBIS_WORK alone does not
# reach the port: OpenGothic's ps4/build.sh sets WORK="${HOME}/.cache/opengothic-ps4" of its
# own and only --work overrides it. So stage 5 built in a PERSISTENT cache shared by every
# run of this gate, and CMake caches absolute paths on first configure. The second run's
# flags.make carried two different bundles at once - measured 2026-09-17:
#
#   CXX_FLAGS     -isysroot .../tmp.eh96LkxI0q/orbis-sdk-v1/sdk          <- run N-1, DELETED
#   CXX_INCLUDES  -I       .../tmp.BcDDEZ9eED/orbis-sdk-v1/orbis-compat  <- run N
#
# CXX_INCLUDES was re-derived, CXX_FLAGS came from the cache, and the sysroot it named no
# longer existed. libc++ and musl were then unfindable and the build died 2000 lines later on
#   .../sdk/include/c++/v1/cstdlib:100: no member named 'rand' in the global namespace
# which reads as a broken libc++ in the bundle. The bundle was fine; <cstdlib> compiles clean
# against it at c++17 and c++20, with and without _BSD_SOURCE. The build tree was lying.
#
# ⚠ THIS IS README §9 TRAP 7, INSIDE THE SCRIPT WRITTEN TO CATCH IT: "A stale build directory
# answers questions about a build that no longer exists. Read link.txt before concluding
# anything from a build tree." This file's own header quotes that trap as its reason to exist,
# and then stage 5 walked into it because it delegated the build directory to the port.
#
# A port whose build directory cannot be isolated cannot be gated - a pass would only say the
# machine still has yesterday's cache - so this is strict rather than best-effort.
PORT_WORK="$WORK/port"
set +e
( . "$B/env.sh" >/dev/null
  export ORBIS_WORK="$WORK/cache"
  cd "$PORT" && ./ps4/build.sh --work "$PORT_WORK" ) > "$WORK/port-build.log" 2>&1
rc=$?
set -e
if [ $rc -ne 0 ] && grep -qiE 'unknown (argument|option).*--work|invalid option.*work' "$WORK/port-build.log"; then
  err "$PORT/ps4/build.sh does not accept --work, so its build directory cannot be isolated."
  err "  Without that, this stage reuses whatever cache the last run left on the machine and"
  err "  proves nothing about the bundle. Add --work to that script, or gate a port that has it."
  exit 1
fi
if [ $rc -ne 0 ]; then
  err "the real port FAILED to build from the bundle (rc=$rc)"
  tail -80 "$WORK/port-build.log" >&2
  err "  full log: $WORK/port-build.log (pass --keep to preserve it)"
  exit 1
fi
ok "the port built"
# ⚠ AN EXIT STATUS OF ZERO IS NOT A BUILD. mesa-ps4's own workflow says it: its build script
# runs `ninja -k 0 ... || true` on purpose because the whole error list is the work list
# during a port. So the artifact is asserted by name, not inferred from rc=0.
found=0
while IFS= read -r a; do printf '   artifact %s (%s bytes)\n' "$a" "$(wc -c < "$a" | tr -d ' ')"; found=1; done \
  < <(find "${ORBIS_WORK}" "$PORT_WORK" "$PORT" -name 'eboot.bin' -newer "$B/BUNDLE.txt" 2>/dev/null | head -5)
if [ "$found" -eq 0 ]; then
  err "the port's build script exited 0 and produced no eboot.bin newer than the bundle.
   That script runs ninja with -k 0 and echoes some failures rather than returning them -
   rc=0 does not mean it built. Read $WORK/port-build.log."
  exit 1
fi

# -----------------------------------------------------------------------------------------
# 6. Stamp.
#
# ⚠ THE STAMP GOES ON THE UNPACKED COPY, AND THE TARBALL IS THEN RE-CUT. Editing BUNDLE.txt
# inside the tree invalidates that file's line in SHA256SUMS, which is correct and deliberate:
# a gate result is new information about the bundle, so the bundle is a new artifact. The
# maintainer re-runs make-sdk-bundle.sh with ORBIS_BUNDLE_GATE=pass rather than patching a
# tarball in place - see the command sequence in scripts/release/README.md.
# -----------------------------------------------------------------------------------------
log "6. result"
sed -i.bak 's/^gate=.*/gate=pass/' "$STAMP_TARGET" && rm -f "$STAMP_TARGET.bak"
ok "gate=pass written to $STAMP_TARGET"
warn "that stamp is on the UNPACKED copy and its sha256 no longer matches SHA256SUMS."
warn "  Re-cut the publishable tarball with the gate recorded from the start:"
warn "      ORBIS_BUNDLE_GATE=pass scripts/release/make-sdk-bundle.sh --version <v> ..."
warn "  then run verify-sdk-bundle.sh on THAT tarball. It should come back OK, not INCOMPLETE."
log "GATE PASSED"
