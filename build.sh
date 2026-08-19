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
mkdir -p "${OUT}"
objs=()
for src in "${ROOT}"/src/*.c;   do o="${OUT}/$(basename "${src}" .c).o";   clang   "${CFLAGS[@]}"   "${DEFS[@]+"${DEFS[@]}"}" -c "${src}" -o "${o}"; objs+=("${o}"); done
for src in "${ROOT}"/src/*.cpp; do [[ -e "${src}" ]] || continue
                                  o="${OUT}/$(basename "${src}" .cpp).o"; clang++ "${CXXFLAGS[@]}" "${DEFS[@]+"${DEFS[@]}"}" -c "${src}" -o "${o}"; objs+=("${o}"); done

rm -f "${OUT}/liborbis-compat.a"
llvm-ar rcs "${OUT}/liborbis-compat.a" "${objs[@]}" 2>/dev/null || ar rcs "${OUT}/liborbis-compat.a" "${objs[@]}"
echo "== ${OUT}/liborbis-compat.a"

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
echo "== malloc_usable_size, sigev_notify_function and ENODATA all reachable"

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

echo "== all checks passed"
