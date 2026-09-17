#!/usr/bin/env bash
# Copyright © 2026 Mikołaj Mikołajczyk
# SPDX-License-Identifier: MIT
# Builds liborbis-compat.a for the console, and checks it.
#
#   ./build.sh [--out <dir>] [--define NAME=VALUE] [--no-check]
#
# Consumers need exactly two flags, and both matter:
#
#   -isystem <orbis-compat>/include        AHEAD of the SDK's include directory
#   <orbis-compat>/build/liborbis-compat.a with --whole-archive
#
# Not CMake and not meson: the four things that consume this use meson (Mesa) and CMake (Tempest,
# OpenGothic, the CTS), and a plain archive plus an include directory is the one shape all of them
# take without argument. The checks live here rather than beside them because two of them need a
# DIFFERENT toolchain - they build and RUN host binaries - and one of them has to fail to compile.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TC="${OO_PS4_TOOLCHAIN:-$HOME/.local/opt/openorbis}"
OUT="${ROOT}/build"
DEFS=()
CHECK=1

while [[ $# -gt 0 ]]; do
  case "$1" in
    --out)      OUT="$2"; shift 2 ;;
    --define)   DEFS+=("-D$2"); shift 2 ;;   # reaches a knob a header documents
    --no-check) CHECK=0; shift ;;
    *) echo "unknown argument: $1" >&2; exit 2 ;;
  esac
done

[[ -d "${TC}/include" ]] || { echo "!! no toolchain at ${TC} - set OO_PS4_TOOLCHAIN" >&2; exit 1; }

# ⚠ THE OVERLAY'S OWN include COMES FIRST, and it must: bits/alltypes.h works by defining musl's
# __DEFINED_<name> guards before musl's copy is reached. Behind the SDK's directory it would compile,
# do nothing, and say nothing.
#
# ⚠ _BSD_SOURCE because this libc is musl, which hides its POSIX declarations when __STRICT_ANSI__ is
# set - and -std=c++NN always sets it. Without it nanosleep and posix_memalign are undeclared in the
# middle of libc++'s own headers.
DEFS_PS4=(-D__PS4__ -DPS4 -D__ORBIS__ -D_BSD_SOURCE=1)
BASE=(--target=x86_64-pc-freebsd12-elf -isysroot "${TC}" "${DEFS_PS4[@]}" -funwind-tables)

CFLAGS=("${BASE[@]}" -isystem "${ROOT}/include" -isystem "${TC}/include" -fPIC -O2 -Wall -Wextra)

# ⚠ libc++ ahead of the SDK's C headers, for the reason Tempest's toolchain file spends thirty lines
# on: libc++ wraps them and #include_next's them, and the wrong order yields an integer-only std::abs
# that truncates floats silently.
#
# -include orbis_prefix.h because the SDK's <orbis/> headers are not self-contained. It REPLACES the
# -include stdlib.h this port used to pass everywhere, and it is not a rename: measured over the
# SDK's 189 orbis/ headers, stdlib.h leaves 16 of them uncompilable and this leaves 7 - and it does
# it by declaring two type headers instead of a whole libc one. See include/orbis_prefix.h.
CXXFLAGS=("${BASE[@]}" -isystem "${TC}/include/c++/v1" -isystem "${ROOT}/include"
          -isystem "${TC}/include" -include orbis_prefix.h -std=c++17 -fPIC -O2 -Wall -Wextra)

# ---------------------------------------------------------------------------------- build
#
# ⚠ src/ ONLY. optional/ is deliberately not compiled into the archive: those files are policy rather
# than correction - an unlimited libc heap that competes with the driver's arena, and a leaking
# thread_local-destructor stub - and every consumer links this archive with --whole-archive. A
# consumer that wants one adds that source to its own target, by name, having read why.
mkdir -p "${OUT}"
objs=()
for src in "${ROOT}"/src/*.c;   do o="${OUT}/$(basename "${src}" .c).o";   clang   "${CFLAGS[@]}"   "${DEFS[@]+"${DEFS[@]}"}" -c "${src}" -o "${o}"; objs+=("${o}"); done
for src in "${ROOT}"/src/*.cpp; do [[ -e "${src}" ]] || continue
                                  o="${OUT}/$(basename "${src}" .cpp).o"; clang++ "${CXXFLAGS[@]}" "${DEFS[@]+"${DEFS[@]}"}" -c "${src}" -o "${o}"; objs+=("${o}"); done

rm -f "${OUT}/liborbis-compat.a"
llvm-ar rcs "${OUT}/liborbis-compat.a" "${objs[@]}" 2>/dev/null || ar rcs "${OUT}/liborbis-compat.a" "${objs[@]}"
echo "== ${OUT}/liborbis-compat.a"

# ---------------------------------------------------------------------------------- crt
#
# ⚠ NOT IN THE ARCHIVE, AND NOT BUILT WITH THE ARCHIVE'S FLAGS. crt/ is this repository's own C
# runtime startup - the objects a link line NAMES BY PATH, ahead of nothing and after everything, and
# which are therefore the opposite of an archive member: always linked, never selected.
#
# They exist for one reason, and crt/orbis_crt1.c argues it at length: the SDK's lib/crt1.o and
# lib/crtlib.o are the only objects that toolchain puts INSIDE a user's binary whose licence is not
# settled, and crtlib.o's source is in a GPL-3.0 tree with no linking exception. Everything else the
# SDK contributes to an eboot is musl, LLVM's runtimes or Sony's own modules; its GPL-3.0 tools -
# create-fself, create-gp4, readoelf, PkgTool - run at build time and stay behind.
#
# What is built, and what deliberately is not:
#
#   crt1.o    the executable entry point.               Built. This is the one that matters: it is
#                                                       what cmake/ps4-openorbis.cmake names on every
#                                                       link line the port makes.
#   crtlib.o  the module (.prx/.sprx) entry point.      Built. Its GPL-3.0 source is the only crt
#                                                       source actually present upstream.
#   crti.o    the .init/.fini prologue.                 Built, and byte-identical - but see the ⚠ in
#   crtn.o    the .init/.fini epilogue.                 crt/orbis_crti.S: NOTHING in this toolchain
#                                                       links either of them.
#   crt_dyn.o crt1 plus an inline .init_array walk.     ⚠ NOT BUILT, ON PURPOSE. This libc already
#                                                       walks .init_array: libc.a's
#                                                       __libc_start_main.lo defines libc_start_init,
#                                                       which calls _init() and then iterates
#                                                       __init_array_start..__init_array_end. The
#                                                       SDK's crt_dyn.o walks the SAME range again
#                                                       from _init_and_main before calling main, so
#                                                       every static constructor in an image linked
#                                                       with it runs TWICE. No link recipe in the SDK
#                                                       or in this repository uses it. Reproducing it
#                                                       would mean reproducing that.
#   Scrt1.o   crt1 with _start_ps4_c left undefined.    Not built. Nothing references it, and it is
#   rcrt1.o   crt1 plus musl's own static-PIE loader.   not among the objects this replaces; rcrt1.o
#                                                       carries 0x189 bytes of self-relocation for a
#                                                       job the PS4 loader does itself.
#
# ⚠ NO -fdata-sections HERE, and that is load-bearing rather than tidy. cmake/orbis-tls.ld matches
# `*(.data)` and NOT `*(.data.*)` - the same omission whose ⚠ block in that file cost a console launch
# over thread-locals - so a per-object data section would become an orphan and the ALIGN(0x4000) that
# starts the RW segment would stop starting anything. -ffunction-sections IS used for crt1.o, because
# the SDK's object has .text._start_ps4_c and the script matches `*(.text .text.*)`.
#
# ⚠ crt1.o WITHOUT UNWIND TABLES AND crtlib.o WITH THEM, matching the originals. An .eh_frame over
# _start would invite the unwinder to walk off the top of the process; crtlib.o's four FDEs are what
# the SDK's object has and a module's exceptions are a real path.
#
# ⚠ AND -fomit-frame-pointer IS NOT AN OPTIMISATION HERE. Without it clang builds a frame in
# _start_ps4_c, and that function must reach __libc_start_main without ever writing %rsp - the loader
# chooses the alignment and the SDK's crt preserves it through two jmps. test/crt_abi.sh checks the
# artifact for it rather than trusting this line.
CRT_OUT="${OUT}/crt"
CRT_BASE=(--target=x86_64-pc-freebsd12-elf -fPIC -O2 -fomit-frame-pointer -fno-stack-protector
          -Wall -Wextra -std=c11)
mkdir -p "${CRT_OUT}"
clang "${CRT_BASE[@]}" -ffunction-sections -fno-asynchronous-unwind-tables -fno-unwind-tables \
      -c "${ROOT}/crt/orbis_crt1.c"   -o "${CRT_OUT}/crt1.o"
clang "${CRT_BASE[@]}" -funwind-tables \
      -c "${ROOT}/crt/orbis_crtlib.c" -o "${CRT_OUT}/crtlib.o"
clang --target=x86_64-pc-freebsd12-elf -c "${ROOT}/crt/orbis_crti.S" -o "${CRT_OUT}/crti.o"
clang --target=x86_64-pc-freebsd12-elf -c "${ROOT}/crt/orbis_crtn.S" -o "${CRT_OUT}/crtn.o"
echo "== ${CRT_OUT}/{crt1,crtlib,crti,crtn}.o"

[[ ${CHECK} -eq 1 ]] || exit 0

# ---------------------------------------------------------------------------------- check
# The build above IS the "does everything compile" check; what follows is what it cannot tell you.
WORK="$(mktemp -d)"; trap 'rm -rf "${WORK}"' EXIT

# 1. The corrected types are corrected, and the ones libc++ embeds are left alone.
clang "${CFLAGS[@]}" -fsyntax-only "${ROOT}/test/sizes.c"
# ...and it must FAIL without the overlay, or it is asserting nothing.
if clang "${BASE[@]}" -isystem "${TC}/include" -fsyntax-only "${ROOT}/test/sizes.c" 2>/dev/null; then
  echo "!! sizes.c passes WITHOUT the overlay - the test proves nothing" >&2; exit 1
fi
echo "== sizes corrected, libc++'s types untouched, and the test fails without us"

# 2. The three names added to headers the SDK already ships, each written the way its consumer
#    writes it. Same shape as above: it has to fail without the overlay, or it is asserting nothing.
clang "${CFLAGS[@]}" -fsyntax-only "${ROOT}/test/declarations.c"
if clang "${BASE[@]}" -isystem "${TC}/include" -fsyntax-only "${ROOT}/test/declarations.c" 2>/dev/null; then
  echo "!! declarations.c passes WITHOUT the overlay - the test proves nothing" >&2; exit 1
fi
echo "== malloc_usable_size, sigev_notify_function, ENODATA and sa_sigaction all usable"

# 3. Every header stands alone - C or C++ as its own contents require.
for h in $(cd "${ROOT}/include" && find . -name '*.h' | sed 's|^\./||' | sort); do
  [[ "${h}" == "bits/alltypes.h" ]] && continue          # musl drives this one; not includable alone
  if grep -qE '^\s*(namespace|template)\b' "${ROOT}/include/${h}"; then
    printf '#include <%s>\nint main(){return 0;}\n' "${h}" > "${WORK}/one.cpp"
    clang++ "${CXXFLAGS[@]}" -fsyntax-only "${WORK}/one.cpp" || { echo "!! ${h} is not self-contained" >&2; exit 1; }
  else
    printf '#include <%s>\nint main(void){return 0;}\n' "${h}" > "${WORK}/one.c"
    clang   "${CFLAGS[@]}"   -fsyntax-only "${WORK}/one.c"   || { echo "!! ${h} is not self-contained" >&2; exit 1; }
  fi
done
echo "== every header is self-contained"

# 4. ⚠ The one thing cross-compiling cannot tell you: whether it WORKS. Run natively - this is what
#    caught a missing <stdint.h> that the cross build had accepted through the PS4 headers.
cc -funwind-tables -I"${ROOT}/include" -o "${WORK}/bt" "${ROOT}/src/orbis_backtrace.c" "${ROOT}/test/backtrace_host.c"
"${WORK}/bt" >/dev/null
echo "== backtrace collects frames, bounds its buffer and formats addresses"

# 5. The crt objects present the same interface as the SDK's - section by section, symbol by symbol,
#    byte by byte. ⚠ THIS IS ALL THAT CAN BE CHECKED HERE: the objects are never linked, because this
#    is a cross build with no ld.lld on the path and nothing to run the result on. What it does prove
#    is that everything the loader reads is bit-for-bit what it reads today, and that every symbol a
#    linker resolves is in the same place with the same binding - with one documented exception per
#    object, which the check names rather than tolerates.
OBJDUMP="${OBJDUMP:-$(command -v llvm-objdump || command -v objdump || true)}"
[[ -n "${OBJDUMP}" ]] || { echo "!! no objdump - set OBJDUMP=<path>" >&2; exit 1; }
[[ -f "${TC}/lib/crt1.o" ]] || {
  echo "!! ${TC}/lib/crt1.o is missing, so there is nothing to compare against. That file is what" >&2
  echo "   crt/ replaces; a check that cannot see it would pass by saying nothing." >&2; exit 1; }
OBJDUMP="${OBJDUMP}" "${ROOT}/test/crt_abi.sh" "${CRT_OUT}" "${TC}/lib"

#    ...and it must FAIL when pointed at something that is not the thing under test, exactly as
#    sizes.c and declarations.c must. Four objects that compile and define nothing the loader wants.
mkdir -p "${WORK}/decoy"
printf 'int _start_ps4_c(void) { return 0; }\n' > "${WORK}/decoy.c"
for o in crt1 crtlib crti crtn; do
  clang --target=x86_64-pc-freebsd12-elf -fPIC -c "${WORK}/decoy.c" -o "${WORK}/decoy/${o}.o"
done
if OBJDUMP="${OBJDUMP}" "${ROOT}/test/crt_abi.sh" "${WORK}/decoy" "${TC}/lib" >/dev/null 2>&1; then
  echo "!! crt_abi.sh passes against a decoy object - the check proves nothing" >&2; exit 1
fi
echo "== the crt objects match the SDK's interface, and the check fails without them"

echo "== all checks passed"

# ---------------------------------------------------------------------------------- what is NOT checked
#
# ⚠ NOTHING ABOVE LINKS, BOOTS OR RUNS ANYTHING ON A CONSOLE, and for crt/ that gap is the whole
# remaining risk: an entry point that presents the right symbols can still be the wrong entry point.
# The link line is selected by ORBIS_CRT in cmake/ps4-openorbis.cmake and defaults to the SDK's
# object for exactly that reason. What a person with ld.lld and a console has to run to close it is
# written out at the bottom of that file, under "confirming ORBIS_CRT=own".
