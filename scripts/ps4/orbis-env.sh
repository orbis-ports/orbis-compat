# Shared prologue for every PS4 build entry point in orbis-ports. Source it; do not run it.
#
#   . "${ORBIS_COMPAT}/scripts/ps4/orbis-env.sh"
#
# It resolves and VERIFIES the two paths every project in this organisation needs, and exports
# them so anything further down - cmake, meson, a toolchain file - sees the same values:
#
#   ORBIS_COMPAT_DIR   this overlay: the toolchain file, the libc and pthread shims, the Vulkan
#                      loader shim, packaging and console logging
#   OO_PS4_TOOLCHAIN   the OpenOrbis SDK
#
# ⚠ FINDING THIS FILE CANNOT ITSELF BE SHARED, which is why each entry point carries six lines of
# path search before it can source this one. That is the chicken-and-egg of a shared prologue and
# there is no way around it; what CAN be shared is everything after, which is why the verification,
# the error messages and the helpers live here rather than being copied five times and drifting.
# Copy the search block verbatim from ps4/build.sh in any repository of the org.
#
# SPDX-License-Identifier: MIT

# ---------------------------------------------------------------- the overlay
#
# The caller has already found it - that is how this file got sourced - but say so out loud, because
# a caller that found the WRONG one is the failure this project has paid for most. On 2026-08-22 two
# CTS build directories were configured against a BACKUP checkout of the driver and would have
# packaged the previous day's work with nothing anywhere announcing it.
ORBIS_COMPAT_DIR="${ORBIS_COMPAT_DIR:?orbis-env.sh sourced without ORBIS_COMPAT_DIR set}"

if [[ ! -f "${ORBIS_COMPAT_DIR}/cmake/ps4-openorbis.cmake" ]]; then
  echo "!! ${ORBIS_COMPAT_DIR} is not an orbis-compat checkout - cmake/ps4-openorbis.cmake is missing." >&2
  echo "   Clone https://github.com/orbis-ports/orbis-compat next to this repository, or set" >&2
  echo "   ORBIS_COMPAT_DIR to where it already is." >&2
  return 1 2>/dev/null || exit 1
fi
ORBIS_COMPAT_DIR="$(cd "${ORBIS_COMPAT_DIR}" && pwd -P)"
export ORBIS_COMPAT_DIR

# ---------------------------------------------------------------- the SDK
#
# ⚠ link.x rather than the directory, and rather than the compiler. A toolchain that is present but
# missing its linker script fails hundreds of files later, in the link, with an error that names
# nothing useful.
OO_PS4_TOOLCHAIN="${OO_PS4_TOOLCHAIN:-${HOME}/.local/opt/openorbis}"
if [[ ! -f "${OO_PS4_TOOLCHAIN}/link.x" ]]; then
  echo "!! no OpenOrbis toolchain at ${OO_PS4_TOOLCHAIN} - link.x is missing." >&2
  echo "   Install it, or set OO_PS4_TOOLCHAIN to where it is." >&2
  return 1 2>/dev/null || exit 1
fi
OO_PS4_TOOLCHAIN="$(cd "${OO_PS4_TOOLCHAIN}" && pwd -P)"
# ⚠ EXPORTED, NOT JUST SET, and create-fself is why: it reads OO_PS4_TOOLCHAIN from the environment
# and refuses to run without it, whatever it was passed on the command line. RetroArch's core
# builder carried that line and that reason itself until the two became shared; the reason belongs
# with the export.
export OO_PS4_TOOLCHAIN

# ---------------------------------------------------------------- what every entry point shares
#
# Out-of-tree by default, and under the user's cache rather than the repository: a build that writes
# into the checkout makes `git status` useless and eventually gets committed by accident.
ORBIS_WORK="${ORBIS_WORK:-${XDG_CACHE_HOME:-${HOME}/.cache}/orbis-ports}"
export ORBIS_WORK

ORBIS_JOBS="${ORBIS_JOBS:-$(nproc 2>/dev/null || echo 4)}"
export ORBIS_JOBS

# ---------------------------------------------------------------- the driver
#
# ⚠ THREE DIFFERENT DEFAULTS POINTED AT THREE DIFFERENT MESA CHECKOUTS, and two of them were stale.
# The CTS fork's build directories named ~/src/mesa-ps4 (a backup since 2026-08-21) and
# vkloader/CMakeLists.txt still defaults to ~/.cache/orbis-mesa/mesa/build-orbis, left over from the
# patch-queue era. Both held a driver a day older than the tree being worked in, and nothing
# anywhere said so - a title or a test built against either would have been evidence about changes
# it did not contain.
#
# So the driver is resolved HERE, once, the same way as everything else, and every consumer is
# passed the answer explicitly rather than falling back to a default of its own.
for _m in "${ORBIS_MESA_DIR:-}" "${ORBIS_COMPAT_DIR}/../mesa-ps4" "${HOME}/src-ps4/mesa-ps4"; do
  [[ -n "$_m" && -d "$_m/src/amd/vulkan" ]] && { ORBIS_MESA_DIR="$(cd "$_m" && pwd -P)"; break; }
done
export ORBIS_MESA_DIR="${ORBIS_MESA_DIR:-}"
export ORBIS_MESA_BUILD="${ORBIS_MESA_DIR:+${ORBIS_MESA_DIR}/build-orbis}"
export ORBIS_RADV_ARCHIVE="${ORBIS_MESA_BUILD:+${ORBIS_MESA_BUILD}/src/amd/vulkan/libvulkan_radeon.a}"

# ⚠ SAY WHICH DRIVER, AND WHEN IT WAS BUILT. A path is not enough: every one of the three stale
# defaults above LOOKED right. The date is what separates them.
orbis_announce_driver() {
  [[ -n "${ORBIS_RADV_ARCHIVE}" && -f "${ORBIS_RADV_ARCHIVE}" ]] || {
    echo "!! no RADV archive at ${ORBIS_RADV_ARCHIVE:-<no mesa-ps4 checkout found>}" >&2
    echo "   Build it first with mesa-ps4/ps4/build.sh, or set ORBIS_MESA_DIR." >&2
    exit 1
  }
  echo "== driver: ${ORBIS_RADV_ARCHIVE}"
  echo "==         built $(date -r "${ORBIS_RADV_ARCHIVE}" '+%Y-%m-%d %H:%M:%S'), $(stat -c %s "${ORBIS_RADV_ARCHIVE}") bytes"
}

orbis_die()  { echo "!! $*" >&2; exit 1; }
orbis_note() { echo "== $*"; }

# The toolchain file every CMake consumer passes, named once so a rename is one edit.
ORBIS_CMAKE_TOOLCHAIN="${ORBIS_COMPAT_DIR}/cmake/ps4-openorbis.cmake"
export ORBIS_CMAKE_TOOLCHAIN
