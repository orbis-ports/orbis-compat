#!/usr/bin/env bash
# Source this to get the four variables every build script in orbis-ports reads, pointed at
# THIS bundle:
#
#   . /path/to/orbis-sdk-<v>/env.sh
#
# A CMake consumer does not need it - toolchain/orbis-sdk.cmake resolves everything from its
# own location. This is for meson, for make, and for the packaging scripts.
#
# ⚠ IT OVERWRITES WHAT IS ALREADY IN THE ENVIRONMENT, and that is the point. A developer
# machine very often has OO_PS4_TOOLCHAIN pointing at some other SDK - on the machine this was
# written on it pointed at a source checkout with no lib/ at all - and a bundle that politely
# deferred to that would be a bundle that silently was not used. Pass --keep to defer instead.

_orbis_keep=0
[ "${1:-}" = "--keep" ] && _orbis_keep=1

if [ -n "${BASH_SOURCE[0]:-}" ]; then _orbis_self="${BASH_SOURCE[0]}"; else _orbis_self="$0"; fi
ORBIS_SDK_BUNDLE="$(cd "$(dirname "$_orbis_self")" && pwd -P)"
export ORBIS_SDK_BUNDLE

if [ ! -f "$ORBIS_SDK_BUNDLE/BUNDLE.txt" ]; then
  echo "!! $ORBIS_SDK_BUNDLE has no BUNDLE.txt - this is not an unpacked orbis-sdk bundle" >&2
  return 1 2>/dev/null || exit 1
fi

_orbis_set(){ # name value
  if [ "$_orbis_keep" = 1 ] && [ -n "${!1:-}" ]; then
    echo "   $1 kept as ${!1}"
  else
    export "$1=$2"; echo "   $1=$2"
  fi
}

echo "== $(sed -n 's/^bundle=//p' "$ORBIS_SDK_BUNDLE/BUNDLE.txt")"
_orbis_set OO_PS4_TOOLCHAIN "$ORBIS_SDK_BUNDLE/sdk"
_orbis_set ORBIS_COMPAT_DIR "$ORBIS_SDK_BUNDLE/orbis-compat"
_orbis_set ORBIS_MESA_SRC   "$ORBIS_SDK_BUNDLE/mesa"
_orbis_set ORBIS_MESA_BUILD "$ORBIS_SDK_BUNDLE/mesa/build-orbis"

# ⚠ orbis-env.sh RE-DERIVES ORBIS_MESA_* AND WILL OVERWRITE THESE. It probes for a mesa-ps4
# CHECKOUT (it looks for src/amd/vulkan) and sets ORBIS_MESA_BUILD from what it finds. The
# bundle's Mesa is a build output, not a checkout, so that probe finds nothing and clears the
# variables. Every consumer that sources orbis-env.sh must re-apply these two afterwards -
# which is exactly what OpenGothic's ps4/build.sh already does, and its composite action
# carries the same warning. These two copies exist so the value survives the round trip.
export ORBIS_SDK_MESA_SRC="$ORBIS_SDK_BUNDLE/mesa"
export ORBIS_SDK_MESA_BUILD="$ORBIS_SDK_BUNDLE/mesa/build-orbis"

# The packaging tools, per host, the same way cmake/ps4-openorbis.cmake picks them.
case "$(uname -s)" in
  Darwin) ORBIS_PKG_TOOLS="$ORBIS_SDK_BUNDLE/sdk/bin/macos" ;;
  *)      ORBIS_PKG_TOOLS="$ORBIS_SDK_BUNDLE/sdk/bin/linux" ;;
esac
export ORBIS_PKG_TOOLS
if [ -d "$ORBIS_PKG_TOOLS" ]; then
  echo "   ORBIS_PKG_TOOLS=$ORBIS_PKG_TOOLS"
else
  echo "!! this bundle carries no bin/ for $(uname -s) - it was cut with --bin-hosts excluding it." >&2
  echo "   create-fself, create-gp4 and PkgTool.Core are absent; you can compile but not package." >&2
fi

# The meson cross file, materialised for this bundle's actual location. It ships as a template
# with __BUNDLE__ in it rather than as a finished file because the bundle is relocatable and a
# cross file with absolute paths baked in at release time would be wrong the moment it moved.
if [ -f "$ORBIS_SDK_BUNDLE/toolchain/orbis.ini.in" ]; then
  sed "s|__BUNDLE__|$ORBIS_SDK_BUNDLE|g" \
      "$ORBIS_SDK_BUNDLE/toolchain/orbis.ini.in" > "$ORBIS_SDK_BUNDLE/toolchain/orbis.ini"
  export ORBIS_MESON_CROSS="$ORBIS_SDK_BUNDLE/toolchain/orbis.ini"
  echo "   ORBIS_MESON_CROSS=$ORBIS_MESON_CROSS"
fi

export ORBIS_CMAKE_TOOLCHAIN="$ORBIS_SDK_BUNDLE/toolchain/orbis-sdk.cmake"
echo "   ORBIS_CMAKE_TOOLCHAIN=$ORBIS_CMAKE_TOOLCHAIN"

unset _orbis_keep _orbis_self
unset -f _orbis_set
