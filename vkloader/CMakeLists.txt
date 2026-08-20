# ps4-vkloader — the Vulkan C ABI, for a console that has no Vulkan loader.
#
# ⚠ DECLARED HERE, BESIDE THE SOURCES, SINCE 2026-08-20. This file was the last shard of the loader
# left in the Tempest fork after vkloader.c/vkthunks.c/gen.py moved; the engine only ever required
# that a target NAMED ps4-vkloader exist (Engine/CMakeLists.txt), never that Tempest declare it.
# A consumer adds this directory:
#
#     add_subdirectory("${ORBIS_COMPAT_DIR}/vkloader" ps4-vkloader)
#
# WHAT THIS IS. Tempest's Vulkan backend calls vkCreateInstance, vkCmdDraw and ~130 more global symbols, and
# every other platform satisfies them with the Khronos loader. There is none here and there cannot be one: the
# eboot is statically linked, there is no libvulkan.so to dlopen and no icd.d to read. What exists is
# orbis-mesa's libvulkan_radeon.a, which exports exactly three public symbols:
#
#     vk_icdGetInstanceProcAddr  vk_icdNegotiateLoaderICDInterfaceVersion  vk_icdGetPhysicalDeviceProcAddr
#
# `nm -g --defined-only libvulkan_radeon.a | grep -c ' T vk[A-Z]'` is 0. So this library defines the entry
# points the engine calls and forwards each one through the ICD's own door — the same path ps4/radv's harness
# walks by hand, made linkable so that the engine's UNMODIFIED call sites work. The Vulkan backend is
# upstream-owned (Engine/gapi/vulkan/, 38 files); converting it to VK_NO_PROTOTYPES plus a dispatch table would
# touch all of them and fight every merge from Try/Tempest.
#
# THE THUNKS ARE GENERATED, not written. gen.py reads the undefined vk* symbols out of the engine's own object
# files and the prototypes out of the same vulkan_core.h the driver was compiled against. Re-running it must
# leave `git diff` empty — same discipline as orbis-mesa/tools/shaders/gen.py.

cmake_minimum_required(VERSION 3.16)

set(ORBIS_MESA_BUILD "$ENV{HOME}/.cache/orbis-mesa/mesa/build-orbis"
    CACHE PATH "orbis-mesa's cross build directory (where build.sh leaves libvulkan_radeon.a)")
set(ORBIS_MESA_SRC "$ENV{HOME}/.cache/orbis-mesa/mesa"
    CACHE PATH "the patched Mesa checkout, for the vulkan headers the driver was built against")

set(RADV_ARCHIVE "${ORBIS_MESA_BUILD}/src/amd/vulkan/libvulkan_radeon.a")
if(NOT EXISTS "${RADV_ARCHIVE}")
  message(FATAL_ERROR
    "no RADV archive at ${RADV_ARCHIVE}\n"
    "Build it first:  ~/src/mesa-ps4/build-support/orbis/build.sh\n"
    "Or point -DORBIS_MESA_BUILD=<dir> at an existing build-orbis.")
endif()
if(NOT EXISTS "${ORBIS_MESA_SRC}/include/vulkan/vulkan.h")
  message(FATAL_ERROR "no vulkan headers at ${ORBIS_MESA_SRC}/include - is ORBIS_MESA_SRC right?")
endif()

# One weak thunk per entry point vulkan_core.h declares, generated once and shared, so nothing is
# generated per project. The per-project vkloader.c/vkthunks.c/gen.py were deleted from Tempest on
# 2026-08-20 after the title ran on the console without them.
add_library(ps4-vkloader STATIC
  ${CMAKE_CURRENT_LIST_DIR}/vkloader.c
  ${CMAKE_CURRENT_LIST_DIR}/vkthunks.c)
target_include_directories(ps4-vkloader PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

# PUBLIC: the consumer compiles against these headers too, and they must be the ones the driver was built
# against - a mismatched vulkan_core.h is a struct-layout bug that compiles cleanly.
#
# ⚠ NO VERSION NUMBER IS REPEATED HERE. This comment used to say "RADV advertises 1.3.358"; the header
# had moved to 359 and nobody noticed, because the claim is derived, not chosen: RADV_API_VERSION_1_3 is
# VK_MAKE_VERSION(1, 3, VK_HEADER_VERSION) (radv_instance.h), and Liverpool takes the 1.3 arm rather than
# the 1.4 one because it is GFX7. Read it there; a copy here only rots.
target_include_directories(ps4-vkloader PUBLIC "${ORBIS_MESA_SRC}/include")

# --whole-archive: a driver linked into a title must have all of itself
# present, because nothing later will dlopen the members the linker decided were unreachable. Only this
# archive — meson merges addrlib and amd_common into it and also emits them separately, and adding those
# produces a thousand duplicate-symbol errors that look like a broken driver.
#
# INTERFACE rather than PRIVATE: a static library cannot swallow its own dependencies, so the flags have to
# reach the final link of whatever executable uses the loader.
# -lSceVideoOut alongside -lSceGnmDriver: the driver's WSI arm opens video-out, registers the scan-out buffers and
# flips, so it is a dependency of the ARCHIVE and not of any particular title. orbis-mesa's tools/linkprobe.sh
# carries the same flag, which is what makes a missing one fail there instead of here.
target_link_libraries(ps4-vkloader INTERFACE
  -Wl,--whole-archive "${RADV_ARCHIVE}" -Wl,--no-whole-archive
  -lSceGnmDriver
  -lSceVideoOut)
