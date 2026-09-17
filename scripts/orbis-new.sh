#!/usr/bin/env bash
# Start a PlayStation 4 project, or find out what is stopping you.
#
#   scripts/orbis-new.sh --check                 only report what is missing, change nothing
#   scripts/orbis-new.sh mygame                  a C project in ./mygame
#   scripts/orbis-new.sh mygame --type cpp       C++ instead
#   scripts/orbis-new.sh mygame --type vulkan    C plus RADV, a swapchain and a triangle
#   scripts/orbis-new.sh mygame --dir ~/src      somewhere other than the current directory
#
# ⚠ WHY THIS EXISTS. A developer wrote, in public: "it seems that the mesa port is designed to work
# only with orbis-compat which seems to be a set of modifications for openorbis but its unclear how
# to set it up". Every word of that is accurate. The pieces are all documented and none of the
# documentation is where a person looks first, so this script is the documentation that runs.
#
# ⚠ AND THE CHECK HALF IS THE IMPORTANT HALF. Every dependency below has cost this project a
# session at some point, and most of them fail in a way that names something else: no ld.lld looks
# like a broken linker script, no Rosetta looks like a missing packaging tool, a -D that did not
# survive try_compile looks like a toolchain file demanding the flag you just gave it. What is
# missing is printed with what it is, where to get it, and where to put it.
#
# SPDX-License-Identifier: MIT
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
COMPAT="$(cd "${HERE}/.." && pwd -P)"
OS="$(uname -s)"

NAME=""; TYPE="c"; DEST="$PWD"; CHECK_ONLY=0
while [ $# -gt 0 ]; do
  case "$1" in
    --check) CHECK_ONLY=1; shift ;;
    --type)  TYPE="${2:-}"; shift 2 ;;
    --dir)   DEST="${2:-}"; shift 2 ;;
    -h|--help) sed -n '2,9p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
    -*)      echo "unknown option: $1" >&2; exit 2 ;;
    *)       NAME="$1"; shift ;;
  esac
done
case "$TYPE" in c|cpp|vulkan) ;; *) echo "--type must be c, cpp or vulkan" >&2; exit 2 ;; esac

bold(){ printf '\033[1m%s\033[0m\n' "$*"; }
ok(){   printf '  \033[1;32mok\033[0m    %s\n' "$*"; }
miss(){ printf '  \033[1;31mmissing\033[0m %s\n' "$*"; }
note(){ printf '          %s\n' "$*"; }

# ⚠ ONE REMEDY PER GROUP, NOT PER BINARY, AND THE FIRST VERSION GOT THIS WRONG. Missing llvm printed
# `brew install llvm lld` four times - once for ld.lld, llvm-ar, llvm-ranlib and llvm-nm - and then
# said "5 things missing". A newcomer reads that as five problems and one of them repeating, when it
# is one install. What is counted here is ACTIONS, because that is what the reader has to perform.
MISSING=0            # actions, not items
TODO=()              # the numbered list printed at the end

have(){ command -v "$1" >/dev/null 2>&1; }

need_group() {       # need_group "<label>" "<remedy>" <binary>...
  local label="$1" remedy="$2"; shift 2
  local absent=() t
  for t in "$@"; do have "$t" || absent+=("$t"); done
  if [ ${#absent[@]} -eq 0 ]; then ok "$label"; return; fi
  MISSING=$((MISSING+1))
  miss "$label - not found: ${absent[*]}"
  note "$remedy"
  TODO+=("$remedy")
}

need_tool() {               # need_tool <binary> <what it is> <how to get it>
  if have "$1"; then ok "$1"
  else
    MISSING=$((MISSING+1)); miss "$1 - $2"; note "$3"; TODO+=("$3")
  fi
}

# ---------------------------------------------------------------------------- the check
bold "host tools"
# ⚠ THE SDK SHIPS NO COMPILER. It is prebuilt libraries and headers; clang comes from the machine.
# On macOS none of these are in Apple's clang, which is the single most common way this fails.
if [ "$OS" = Darwin ]; then
  GET_LLVM="brew install llvm lld   # then put /opt/homebrew/opt/llvm/bin and .../lld/bin on PATH"
else
  GET_LLVM="sudo apt-get install -y clang lld llvm      (or your distribution's equivalent)"
fi
need_group "clang and the LLVM binutils" "$GET_LLVM" \
  clang clang++ ld.lld llvm-ar llvm-ranlib llvm-nm
need_tool cmake "the build system every example here uses" \
  "$([ "$OS" = Darwin ] && echo 'brew install cmake' || echo 'sudo apt-get install -y cmake')"
if ! command -v ninja >/dev/null 2>&1 && ! command -v make >/dev/null 2>&1; then
  _r="$([ "$OS" = Darwin ] && echo 'brew install ninja' || echo 'sudo apt-get install -y ninja-build')"
  MISSING=$((MISSING+1)); miss "ninja or make - something has to run the build"
  note "$_r"; TODO+=("$_r")
else
  ok "$(command -v ninja >/dev/null 2>&1 && echo ninja || echo make)"
fi

if [ "$TYPE" = vulkan ]; then
  need_tool glslangValidator "compiles GLSL to SPIR-V at build time; the console loads no shader files" \
    "$([ "$OS" = Darwin ] && echo 'brew install glslang' || echo 'sudo apt-get install -y glslang-tools')"
fi

# ⚠ macOS ONLY: every tool in the SDK's bin/macos is an x86_64 Mach-O binary. Without Rosetta they
# die with "bad CPU type in executable", which reads as a corrupt download rather than a missing
# translator - and it happens AFTER a successful build, at packaging.
if [ "$OS" = Darwin ]; then
  bold "rosetta (macOS only)"
  if /usr/bin/pgrep -q oahd 2>/dev/null || [ -d /Library/Apple/usr/share/rosetta ]; then
    ok "Rosetta 2 present - the SDK's macOS tools are x86_64 and need it"
  else
    MISSING=$((MISSING+1)); miss "Rosetta 2 - create-fself, create-gp4 and PkgTool.Core are x86_64"
    note "softwareupdate --install-rosetta --agree-to-license"
    TODO+=("softwareupdate --install-rosetta --agree-to-license")
  fi
fi

bold "the SDK"
SDK="${OO_PS4_TOOLCHAIN:-$HOME/.local/opt/openorbis}"
SDK_OK=1
if [ ! -f "$SDK/link.x" ]; then
  MISSING=$((MISSING+1)); SDK_OK=0
  miss "OpenOrbis SDK at $SDK"
  note "Download the release asset and unpack it so link.x lands at the root:"
  note "  gh release download v0.5.4 --repo OpenOrbis/OpenOrbis-PS4-Toolchain \\"
  note "     --pattern 'toolchain-llvm-18.tar.gz'"
  note "  mkdir -p $SDK /tmp/oo && tar -xzf toolchain-llvm-18.tar.gz -C /tmp/oo"
  note "  root=\$(dirname \"\$(find /tmp/oo -name link.x -print -quit)\"); cp -a \"\$root\"/. $SDK/"
  note "Then: export OO_PS4_TOOLCHAIN=$SDK"
  TODO+=("install the OpenOrbis SDK into $SDK (commands above), then export OO_PS4_TOOLCHAIN=$SDK")
else
  ok "SDK at $SDK"
  # Named individually, because "the SDK is broken" is not a diagnosis. Each of these has been
  # absent at least once and each fails hundreds of files later than the moment it is needed.
  for f in include/stdlib.h lib/libc.a lib/crt1.o; do
    [ -e "$SDK/$f" ] || { MISSING=$((MISSING+1)); SDK_OK=0; miss "$SDK/$f"; }
  done
  BINDIR="$SDK/bin/$([ "$OS" = Darwin ] && echo macos || echo linux)"
  # ⚠ THE macOS BINARY IS SPELLED create-fself-macos. Looking only for the Linux name found nothing
  # on a Mac, and because that was a warning rather than an error the build produced no eboot and
  # failed much later, in whatever consumed it.
  if [ -x "$BINDIR/create-fself" ] || [ -x "$BINDIR/create-fself-macos" ]; then ok "packaging tools in $BINDIR"
  else MISSING=$((MISSING+1)); miss "create-fself in $BINDIR"; note "the SDK unpack above is incomplete"; fi
fi

bold "the overlay"
if [ -f "$COMPAT/build/liborbis-compat.a" ]; then
  ok "liborbis-compat.a"
elif [ "$SDK_OK" = 1 ]; then
  MISSING=$((MISSING+1)); miss "$COMPAT/build/liborbis-compat.a"
  note "Build it - it needs the SDK above and nothing else:"
  note "  ( cd $COMPAT && OO_PS4_TOOLCHAIN=$SDK ./build.sh )"
  TODO+=("( cd $COMPAT && OO_PS4_TOOLCHAIN=$SDK ./build.sh )")
else
  MISSING=$((MISSING+1)); miss "$COMPAT/build/liborbis-compat.a - build the SDK first"
  TODO+=("( cd $COMPAT && ./build.sh )   # after the SDK above is in place")
fi

if [ "$TYPE" = vulkan ]; then
  bold "mesa (only for --type vulkan)"
  MSRC="${ORBIS_MESA_SRC:-}"; MBUILD="${ORBIS_MESA_BUILD:-}"
  if [ -n "$MSRC" ] && [ -f "$MSRC/include/vulkan/vulkan.h" ] \
     && [ -n "$MBUILD" ] && [ -f "$MBUILD/src/amd/vulkan/libvulkan_radeon.a" ]; then
    ok "Mesa bundle at $MSRC"
  else
    MISSING=$((MISSING+1)); miss "the Mesa bundle (ORBIS_MESA_SRC / ORBIS_MESA_BUILD)"
    note "One asset carries the archives AND the headers they were built against - take both from"
    note "the same tarball, because a mismatched vulkan_core.h is a struct-layout bug that compiles"
    note "cleanly and fails at run time:"
    note "  gh release download <orbis-mesa-tag> --repo orbis-ports/mesa-ps4 \\"
    note "     --pattern 'orbis-mesa-*.tar.gz'"
    note "  tar -xzf orbis-mesa-*.tar.gz -C ~/.local/opt"
    note "  export ORBIS_MESA_SRC=~/.local/opt/orbis-mesa-<sha>"
    note "  export ORBIS_MESA_BUILD=\$ORBIS_MESA_SRC/build-orbis"
    TODO+=("download the Mesa bundle (commands above) and export ORBIS_MESA_SRC / ORBIS_MESA_BUILD)")
  fi
fi

echo
if [ "$MISSING" -ne 0 ]; then
  # ⚠ A NUMBERED LIST, IN ORDER, AT THE END. The per-item notes above explain WHY each thing is
  # needed; this is the part a person copies. Reading a diagnosis and then having to reassemble the
  # commands out of it is the step where people give up, and giving up here is the whole problem
  # this script was written for.
  printf '\033[1;31m%s\033[0m\n' "$MISSING step(s) to do - nothing was created."
  echo
  bold "do this, in order:"
  i=1
  for t in "${TODO[@]}"; do printf '  %d. %s\n' "$i" "$t"; i=$((i+1)); done
  echo
  echo "  then run this again:  $0 ${NAME:-<name>}${TYPE:+ --type $TYPE}"
  echo
  echo "None of it needs a console, and none of it touches anything outside the paths named above."
  exit 1
fi
printf '\033[1;32m%s\033[0m\n' "everything needed is present."
[ "$CHECK_ONLY" = 1 ] && exit 0
[ -n "$NAME" ] || { echo; echo "Give it a project name:  $0 <name> [--type c|cpp|vulkan]"; exit 0; }

# ---------------------------------------------------------------------------- scaffold
PROJ="$DEST/$NAME"
[ -e "$PROJ" ] && { echo "$PROJ already exists - refusing to write into it" >&2; exit 1; }
mkdir -p "$PROJ"

# A title id must be nine characters, four letters then five digits, and unique per title on the
# console: two packages sharing one overwrite each other. This is a placeholder that says so.
TITLEID="TMPS$(printf '%05d' $(( (RANDOM % 89999) + 10000 )))"
SRC="main.c"; LANGS="C"; [ "$TYPE" = cpp ] && { SRC="main.cpp"; LANGS="CXX"; }

cat > "$PROJ/CMakeLists.txt" <<CMAKE
cmake_minimum_required(VERSION 3.16)

# ⚠ THE GUARD IS BELOW project(), AND IT HAS TO BE. CMake reads CMAKE_TOOLCHAIN_FILE when project()
# runs, so nothing the toolchain file sets exists before that line - a check above it refuses every
# correct invocation and tells you to pass the flag you just passed.
project($NAME $LANGS)

if(NOT DEFINED ORBIS_COMPAT_DIR OR NOT PS4)
  message(FATAL_ERROR "configure with -DCMAKE_TOOLCHAIN_FILE=<orbis-compat>/cmake/ps4-openorbis.cmake")
endif()

add_executable($NAME $SRC)
CMAKE

if [ "$TYPE" = vulkan ]; then
  cp "$COMPAT/scripts/release/triangle/tri.vert" "$COMPAT/scripts/release/triangle/tri.frag" "$PROJ/"
  cat >> "$PROJ/CMakeLists.txt" <<'CMAKE'

# Shaders are compiled to SPIR-V here and embedded, so the console reads no files.
find_program(GLSLANG NAMES glslangValidator glslang REQUIRED)
function(orbis_spv stem)
  add_custom_command(
    OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/tri_${stem}_spv.h"
    COMMAND "${GLSLANG}" -V "${CMAKE_CURRENT_LIST_DIR}/tri.${stem}"
            --vn "tri_${stem}_spv" -o "${CMAKE_CURRENT_BINARY_DIR}/tri_${stem}_spv.h"
    DEPENDS "${CMAKE_CURRENT_LIST_DIR}/tri.${stem}")
endfunction()
orbis_spv(vert)
orbis_spv(frag)
target_sources(PROJECT_NAME_PLACEHOLDER PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}/tri_vert_spv.h" "${CMAKE_CURRENT_BINARY_DIR}/tri_frag_spv.h")
target_include_directories(PROJECT_NAME_PLACEHOLDER PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# ps4-vkloader is the Vulkan C ABI. This console has no Vulkan loader and cannot have one: the eboot
# is statically linked, so there is no libvulkan.so to dlopen. The shim defines the entry points a
# normal program calls and forwards each through RADV's ICD door.
add_subdirectory("${ORBIS_COMPAT_DIR}/vkloader" ps4-vkloader)
target_link_libraries(PROJECT_NAME_PLACEHOLDER PRIVATE ps4-vkloader)
CMAKE
  sed -i.bak "s/PROJECT_NAME_PLACEHOLDER/$NAME/g" "$PROJ/CMakeLists.txt" && rm -f "$PROJ/CMakeLists.txt.bak"
fi

cat >> "$PROJ/CMakeLists.txt" <<CMAKE

# ⚠ DO NOT LINK orbis::compat HERE. The toolchain file already puts liborbis-compat.a on every
# executable link line with --whole-archive. Adding it again loads every member twice and every
# symbol in it becomes a duplicate.

# Compiles the overlay's type assertions with THIS project's real flags, at configure time. It is
# what turns "we trust the include order" into an error you get in seconds rather than a
# pthread_mutexattr_t overrun you get on hardware.
orbis_compat_verify()

ps4_create_eboot($NAME)
ps4_create_pkg($NAME TITLE_ID $TITLEID TITLE "$NAME")
CMAKE

if [ "$TYPE" = vulkan ]; then
  sed -e "s/^int main(void)/int main(void)/" "$COMPAT/scripts/release/triangle/triangle.c" > "$PROJ/main.c"
else
  EXT_OPEN=""; EXT_CLOSE=""
  [ "$TYPE" = cpp ] && { EXT_OPEN='extern "C" {'; EXT_CLOSE='}'; }
  cat > "$PROJ/$SRC" <<SRCF
/* $NAME - a PlayStation 4 program.
 *
 * Built with the OpenOrbis SDK plus orbis-compat, which corrects what that SDK gets wrong. See the
 * overlay's README section 0 for the whole workflow, and section 2 for what is being corrected.
 */
#include <orbis_prefix.h>

#include <stdio.h>
#include <unistd.h>

$EXT_OPEN
#include <orbis/libkernel.h>
$EXT_CLOSE

int main(void)
{
    /* sceKernelDebugOutText is the console's own channel and needs no library of ours - it is the
     * one thing that still reports when everything else has gone wrong. scripts/ps4/logs.sh
     * catches it. */
    sceKernelDebugOutText(0, "$NAME: alive\\n");
    printf("$NAME: alive\\n");
    fflush(stdout);

    /* your program here */

    /* ⚠ DO NOT RETURN FROM main(). On this console that is reported as CE-34878-0 and reads exactly
     * like a crash: the log shows SIGSYS inside _exit, because the kernel refuses the syscall
     * underneath it. Idle instead, and close the title from the PS button menu. */
    sceKernelDebugOutText(0, "$NAME: done - idling\\n");
    for (;;)
        sleep(1);
}
SRCF
fi

cat > "$PROJ/build.sh" <<'BUILDSH'
#!/usr/bin/env bash
# Configure, build, package and (optionally) upload.
#
#   ./build.sh                       build and package
#   ./build.sh --deploy <console-ip> and put it on the console
set -euo pipefail
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"

# ⚠ THE ENVIRONMENT, NOT -D. CMake does not carry -D cache entries into try_compile, which is where
# the toolchain file is read a second time - so -DOO_PS4_TOOLCHAIN=... is lost inside CMake's own
# compiler test and the configure fails asking for the flag you just gave it. Exported variables
# survive, because the environment does.
export OO_PS4_TOOLCHAIN="${OO_PS4_TOOLCHAIN:-$HOME/.local/opt/openorbis}"
export ORBIS_COMPAT_DIR="${ORBIS_COMPAT_DIR:-__COMPAT__}"

DEPLOY=""
[ "${1:-}" = "--deploy" ] && DEPLOY="${2:-}"

CM=(-DCMAKE_TOOLCHAIN_FILE="$ORBIS_COMPAT_DIR/cmake/ps4-openorbis.cmake" -DPS4_BUILD_PKG=ON)
[ -n "${ORBIS_MESA_SRC:-}" ]   && CM+=(-DORBIS_MESA_SRC="$ORBIS_MESA_SRC")
[ -n "${ORBIS_MESA_BUILD:-}" ] && CM+=(-DORBIS_MESA_BUILD="$ORBIS_MESA_BUILD")

cmake -S "$HERE" -B "$HERE/build" "${CM[@]}"
cmake --build "$HERE/build"

pkg="$(find "$HERE/build" -name '*.pkg' -print -quit)"
[ -n "$pkg" ] && echo "== package: $pkg"

if [ -n "$DEPLOY" ] && [ -n "$pkg" ]; then
  "$ORBIS_COMPAT_DIR/scripts/ps4/deploy.sh" --pkg "$pkg" --name "$(basename "$HERE")" --host "$DEPLOY"
fi
BUILDSH
sed -i.bak "s|__COMPAT__|$COMPAT|" "$PROJ/build.sh" && rm -f "$PROJ/build.sh.bak"
chmod +x "$PROJ/build.sh"

printf 'build/\n*.pkg\n*.oelf\neboot.bin\n' > "$PROJ/.gitignore"

cat > "$PROJ/README.md" <<RDM
# $NAME

A PlayStation 4 program, built with the OpenOrbis SDK and [orbis-compat]($COMPAT).

\`\`\`sh
./build.sh                            # build and package
./build.sh --deploy 192.168.1.50      # and put it on the console
\`\`\`

Then install it from the console's package menu. \`$COMPAT/scripts/ps4/logs.sh\` catches the output.

Title id is \`$TITLEID\`, generated at random. ⚠ **Change it if you publish anything**: two packages
sharing a title id overwrite each other on the console.

Three things about this platform that are not obvious, all of them in the generated sources:

* **Do not return from \`main()\`.** It is reported as \`CE-34878-0\` and looks like a crash.
* **Pass the toolchain paths in the environment, not with \`-D\`.** CMake drops \`-D\` entries inside
  \`try_compile\`, which is where the toolchain file is read a second time.
* **Do not link \`orbis::compat\` yourself.** The toolchain file already force-loads it.

\`$COMPAT/README.md\` section 0 is the full workflow; section 2 is what the overlay corrects and why.
RDM

echo
bold "created $PROJ"
note "$(cd "$PROJ" && ls)"
echo
echo "  cd $PROJ && ./build.sh"
[ "$TYPE" = vulkan ] && echo "  (set ORBIS_MESA_SRC and ORBIS_MESA_BUILD first - see --check)"
