# The ONE file a consumer of this bundle points CMake at.
#
#   cmake -B build -DCMAKE_TOOLCHAIN_FILE=/path/to/orbis-sdk-<v>/toolchain/orbis-sdk.cmake
#
# No environment variables. No four exports. Everything this file needs is its own sibling,
# resolved from CMAKE_CURRENT_LIST_DIR, so the bundle is relocatable: unpack it anywhere,
# move it, rename its parent, and the same command still works.
#
# ⚠ THIS FILE DEFINES NOTHING ABOUT THE PLATFORM ITSELF. It resolves four paths and then
# includes orbis-compat/cmake/ps4-openorbis.cmake, which is the real toolchain file and the
# only place the flags live. That is deliberate: the include-order rules it carries
# (orbis-compat ahead of the SDK, libc++ ahead of both, -lc last on the line) are load-bearing
# and are documented at length where they are implemented. A second copy here would drift,
# and the drift would be silent - the corrections compile and do nothing when they are behind
# the SDK's directory, which is exactly the failure mode that is hardest to notice.
#
# If you want to override a path anyway - to test a newer overlay against this bundle's Mesa,
# say - set OO_PS4_TOOLCHAIN / ORBIS_COMPAT_DIR / ORBIS_MESA_SRC / ORBIS_MESA_BUILD in the
# cache or the environment BEFORE configuring; the lines below do not overwrite what is
# already set. ⚠ Doing that breaks the pairing this bundle exists to guarantee. BUNDLE.txt
# records which Mesa and which orbis-compat belong together; nothing here can stop you.

if(DEFINED ORBIS_SDK_BUNDLE_INCLUDED)
  return()
endif()
set(ORBIS_SDK_BUNDLE_INCLUDED TRUE)

get_filename_component(ORBIS_SDK_BUNDLE "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT EXISTS "${ORBIS_SDK_BUNDLE}/BUNDLE.txt")
  message(FATAL_ERROR
    "orbis-sdk.cmake expected to sit in <bundle>/toolchain/ and found no "
    "${ORBIS_SDK_BUNDLE}/BUNDLE.txt. Point CMAKE_TOOLCHAIN_FILE at the copy inside an "
    "UNPACKED bundle, not at a copy you moved somewhere else.")
endif()

# ⚠ gate=unproven means nobody has built anything from this bundle yet. A warning rather than
# an error on purpose: the person proving it has to be able to configure a build first.
file(READ "${ORBIS_SDK_BUNDLE}/BUNDLE.txt" _orbis_manifest)
if(_orbis_manifest MATCHES "pairing=MISMATCH")
  message(FATAL_ERROR
    "${ORBIS_SDK_BUNDLE}/BUNDLE.txt says pairing=MISMATCH: its Mesa was built against a "
    "different orbis-compat than the one it ships. Mesa's archives were compiled against one "
    "set of headers and will link against another. Do not build against this bundle.")
endif()
if(_orbis_manifest MATCHES "gate=unproven")
  message(WARNING
    "${ORBIS_SDK_BUNDLE}/BUNDLE.txt says gate=unproven: nothing has been built from this "
    "bundle yet, so its layout is untested. See the bundle README, 'Is this bundle proven?'.")
endif()

foreach(_v OO_PS4_TOOLCHAIN ORBIS_COMPAT_DIR ORBIS_MESA_SRC ORBIS_MESA_BUILD)
  if(NOT DEFINED ${_v} AND DEFINED ENV{${_v}})
    set(${_v} "$ENV{${_v}}")
    message(STATUS "orbis-sdk: ${_v} taken from the environment, NOT from the bundle")
  endif()
endforeach()

if(NOT DEFINED OO_PS4_TOOLCHAIN)
  set(OO_PS4_TOOLCHAIN "${ORBIS_SDK_BUNDLE}/sdk")
endif()
if(NOT DEFINED ORBIS_COMPAT_DIR)
  set(ORBIS_COMPAT_DIR "${ORBIS_SDK_BUNDLE}/orbis-compat")
endif()
if(NOT DEFINED ORBIS_MESA_SRC)
  set(ORBIS_MESA_SRC "${ORBIS_SDK_BUNDLE}/mesa")
endif()
if(NOT DEFINED ORBIS_MESA_BUILD)
  set(ORBIS_MESA_BUILD "${ORBIS_MESA_SRC}/build-orbis")
endif()

set(OO_PS4_TOOLCHAIN "${OO_PS4_TOOLCHAIN}" CACHE PATH   "OpenOrbis SDK root (this bundle's sdk/)")
set(ORBIS_COMPAT_DIR "${ORBIS_COMPAT_DIR}" CACHE PATH   "orbis-compat overlay (this bundle's orbis-compat/)")
set(ORBIS_MESA_SRC   "${ORBIS_MESA_SRC}"   CACHE PATH   "Mesa bundle root; its include/ is what RADV was built against")
set(ORBIS_MESA_BUILD "${ORBIS_MESA_BUILD}" CACHE PATH   "Mesa archives, laid out as build-orbis/src/...")

# ⚠ EXPORTED, not just set. create-fself reads OO_PS4_TOOLCHAIN from the environment when it
# runs as a POST_BUILD step, and make-pkg.sh reads it too; a CMake variable alone does not
# reach either of them.
set(ENV{OO_PS4_TOOLCHAIN} "${OO_PS4_TOOLCHAIN}")
set(ENV{ORBIS_COMPAT_DIR} "${ORBIS_COMPAT_DIR}")

include("${ORBIS_COMPAT_DIR}/cmake/ps4-openorbis.cmake")
