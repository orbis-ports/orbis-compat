# The second worked example: a triangle on the television, out of the bundle.
#
#   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<bundle>/toolchain/orbis-sdk.cmake
#   cmake --build build
#
# ⚠ THIS ONE NEEDS MESA, AND hello/ DELIBERATELY DOES NOT. That split is the point of having two:
# hello answers "is the bundle laid out correctly", with nothing else able to fail; this answers
# "does the graphics stack come up and reach the screen". A single example mixing both would
# report a driver problem as a layout problem and a layout problem as a driver problem.
#
# ⚠ AND IT NEEDS glslangValidator ON THE HOST. The SDK has no shader compiler and neither does the
# bundle - SPIR-V is built here and embedded, so the console loads no files. Debian/Ubuntu:
# `apt-get install glslang-tools`; macOS: `brew install glslang`.
cmake_minimum_required(VERSION 3.16)

# Same ordering rule as hello/CMakeLists.txt: CMake reads CMAKE_TOOLCHAIN_FILE when project() runs,
# so nothing it sets exists before this line and a guard above it can only ever fire.
project(orbis_triangle C)

if(NOT DEFINED ORBIS_COMPAT_DIR OR NOT PS4)
  message(FATAL_ERROR
    "configure with -DCMAKE_TOOLCHAIN_FILE=<bundle>/toolchain/orbis-sdk.cmake. "
    "Without it nothing here targets the PS4 and a 'successful' build means nothing.")
endif()

# ORBIS_MESA_SRC is what ps4-vkloader compiles against, and it must be the headers the driver in
# this bundle was built with. A mismatched vulkan_core.h is a struct-layout bug that compiles
# cleanly and fails at run time - mesa-ps4's release workflow ships headers and archives as one
# asset for exactly this reason.
if(NOT ORBIS_MESA_SRC)
  if(DEFINED ENV{ORBIS_MESA_SRC})
    set(ORBIS_MESA_SRC "$ENV{ORBIS_MESA_SRC}")
  else()
    message(FATAL_ERROR
      "ORBIS_MESA_SRC is not set. It is the unpacked Mesa bundle - the directory holding "
      "include/ and build-orbis/. From an SDK bundle, env.sh exports it.")
  endif()
endif()

find_program(GLSLANG NAMES glslangValidator glslang REQUIRED)

# ---------------------------------------------------------------------------- shaders
#
# glslangValidator -V produces SPIR-V; --vn emits it as a C array in a header, which is how it gets
# into the image without the console reading a file. Two shaders, two headers, both generated into
# the build tree so the source directory stays clean.
function(orbis_spv stem)
  add_custom_command(
    OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/tri_${stem}_spv.h"
    COMMAND "${GLSLANG}" -V "${CMAKE_CURRENT_LIST_DIR}/tri.${stem}"
            --vn "tri_${stem}_spv" -o "${CMAKE_CURRENT_BINARY_DIR}/tri_${stem}_spv.h"
    DEPENDS "${CMAKE_CURRENT_LIST_DIR}/tri.${stem}"
    COMMENT "glslang: tri.${stem} -> tri_${stem}_spv.h")
endfunction()
orbis_spv(vert)
orbis_spv(frag)

add_executable(triangle
  triangle.c
  "${CMAKE_CURRENT_BINARY_DIR}/tri_vert_spv.h"
  "${CMAKE_CURRENT_BINARY_DIR}/tri_frag_spv.h")
target_include_directories(triangle PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# ps4-vkloader is the Vulkan C ABI. This console has no Vulkan loader and cannot have one: the eboot
# is statically linked, there is no libvulkan.so to dlopen and no icd.d to read. libvulkan_radeon.a
# exports three ICD symbols and nothing named vkSomething; the loader shim defines the ~130 entry
# points a normal program calls and forwards each through the ICD's door. See vkloader/CMakeLists.txt.
add_subdirectory("${ORBIS_COMPAT_DIR}/vkloader" ps4-vkloader)
target_link_libraries(triangle PRIVATE ps4-vkloader)

# The overlay's archive is already on every executable link line, put there by the toolchain file
# with --whole-archive. Do not add it again here: hello/CMakeLists.txt records what happens when
# something does - every member loads twice and every symbol in it is a duplicate.

# Compiles the overlay's type assertions with THIS project's real flags, at configure time.
orbis_compat_verify()

if(COMMAND ps4_create_eboot)
  ps4_create_eboot(triangle)
endif()
