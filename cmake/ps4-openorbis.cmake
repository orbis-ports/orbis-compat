# Copyright © 2026 Mikołaj Mikołajczyk
# SPDX-License-Identifier: MIT
#
# ⚠ THIS FILE LIVED IN Tempest UNTIL 2026-08-19, and only because Tempest was the first thing to
# need it. Nothing in it is about an engine: it is the target triple, the sysroot, the include order
# this SDK requires, the link line, and the wiring for this repository's own corrections. dEQP needs
# exactly the same file, and a conformance suite for a Vulkan driver should not have to clone a game
# engine to build.
#
# The three mentions of Tempest and OpenGothic that remain are EVIDENCE, not dependencies - they say
# which build proved that the include order matters and which one hit 37 truncated std::abs calls.
# The one real constraint they record is general: do not use a global link_libraries() in a tree that
# export()s targets.
#
# Consumers: Tempest, OpenGothic (through Tempest), VK-GL-CTS.
# CMake toolchain file for the PS4 (Orbis) target, driven by the OpenOrbis PS4 Toolchain.
#
#   cmake -S <dir> -B <build> -DCMAKE_TOOLCHAIN_FILE=<repo>/cmake/ps4-openorbis.cmake
#
# The SDK root is taken from -DOO_PS4_TOOLCHAIN=..., else $ENV{OO_PS4_TOOLCHAIN},
# else ~/.local/opt/openorbis. Install instructions: backlog/docs/dev-setup.md.
#
# Flags mirror the OpenOrbis sample build rules (samples/hello_world/Makefile in the
# SDK tree) — that Makefile is the only normative description of the link recipe.

if(DEFINED PS4_OPENORBIS_TOOLCHAIN_INCLUDED)
  return()
endif()
set(PS4_OPENORBIS_TOOLCHAIN_INCLUDED TRUE)

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_VERSION   1)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Tempest/upstream platform switch: Engine/system/platform.h keys off __PS4__, and
# CMake code keys off PS4 (there is no built-in platform variable for Orbis).
set(PS4 TRUE)

if(NOT OO_PS4_TOOLCHAIN)
  if(DEFINED ENV{OO_PS4_TOOLCHAIN})
    set(OO_PS4_TOOLCHAIN "$ENV{OO_PS4_TOOLCHAIN}")
  else()
    set(OO_PS4_TOOLCHAIN "$ENV{HOME}/.local/opt/openorbis")
  endif()
endif()
set(OO_PS4_TOOLCHAIN "${OO_PS4_TOOLCHAIN}" CACHE PATH "OpenOrbis PS4 Toolchain root")

if(NOT EXISTS "${OO_PS4_TOOLCHAIN}/link.x")
  message(FATAL_ERROR
    "OpenOrbis PS4 Toolchain not found at '${OO_PS4_TOOLCHAIN}'. "
    "Set -DOO_PS4_TOOLCHAIN=<path> or the OO_PS4_TOOLCHAIN environment variable. "
    "See backlog/docs/dev-setup.md.")
endif()

if(CMAKE_HOST_APPLE)
  set(OO_PS4_BINDIR "${OO_PS4_TOOLCHAIN}/bin/macos")
else()
  set(OO_PS4_BINDIR "${OO_PS4_TOOLCHAIN}/bin/linux")
endif()

# The SDK ships no compiler: libc/libc++ are prebuilt, clang comes from the host.
find_program(CMAKE_C_COMPILER   NAMES clang       REQUIRED)
find_program(CMAKE_CXX_COMPILER NAMES clang++     REQUIRED)
find_program(CMAKE_AR           NAMES llvm-ar     REQUIRED)
find_program(CMAKE_RANLIB       NAMES llvm-ranlib REQUIRED)
find_program(CMAKE_LINKER       NAMES ld.lld      REQUIRED)
find_program(CMAKE_STRIP        NAMES llvm-strip)

set(PS4_TARGET_TRIPLE "x86_64-pc-freebsd12-elf")

# -isysroot/-isystem instead of CMAKE_SYSROOT: the SDK is not a real sysroot layout
# (no usr/ prefix), so CMake's sysroot handling would not find the headers.
# _BSD_SOURCE: the SDK libc is musl, which hides POSIX declarations (nanosleep,
# and the <math.h> spillover libc++ <cmath> expects) when __STRICT_ANSI__ is set —
# which -std=c++NN always sets. Without it libc++ <atomic>/<cmath> do not compile.
#
# The include directories are NOT part of the common flags: C++ needs libc++'s directory
# ahead of the C one, and that ordering is load-bearing (see below).
set(PS4_COMMON_FLAGS
  "--target=${PS4_TARGET_TRIPLE} -fPIC -funwind-tables -D__PS4__ -DPS4 -D__ORBIS__ -D_BSD_SOURCE=1 -isysroot ${OO_PS4_TOOLCHAIN}")

# ⚠ THE OVERLAY GOES AHEAD OF THE SDK, AND MUST. orbis-compat corrects declarations this SDK gets
# wrong - four pthread types musl declares smaller than Sony writes, measured on hardware - and it
# does so by defining musl's own __DEFINED_<name> guards before musl's bits/alltypes.h is reached.
# Behind the SDK's directory it would compile, do nothing, and say nothing.
#
# It also supplies headers the SDK omits although the platform has them: execinfo.h, machine/cpu.h,
# sys/{cpuset,ioccom,param,sysctl}.h, pthread_np.h.
# The overlay ships the module that knows all of this, so the paths are no longer spliced by hand
# here. orbis_compat_locate() finds it and explains in its FATAL_ERROR why it matters;
# orbis_compat_target() defines orbis::compat with --whole-archive already on it.
if (NOT ORBIS_COMPAT_DIR)
	if (DEFINED ENV{ORBIS_COMPAT_DIR})
		set(ORBIS_COMPAT_DIR "$ENV{ORBIS_COMPAT_DIR}")
	else ()
		set(ORBIS_COMPAT_DIR "$ENV{HOME}/src-ps4/orbis-compat")
	endif ()
endif ()

if (NOT EXISTS "${ORBIS_COMPAT_DIR}/cmake/orbis-compat.cmake")
	message(FATAL_ERROR
		"orbis-compat not found at '${ORBIS_COMPAT_DIR}'. It carries the corrections this SDK "
		"needs; building without it is building against declarations known to be wrong. Set "
		"-DORBIS_COMPAT_DIR=<path> or the ORBIS_COMPAT_DIR environment variable.")
endif ()

include("${ORBIS_COMPAT_DIR}/cmake/orbis-compat.cmake")
orbis_compat_locate()

# ⚠ NO "the archive must already exist" CHECK ANY MORE, and its absence is deliberate: the overlay
# has to be buildable WITH this toolchain file, and it cannot be if configuring demands the archive
# it is about to produce. The link line names the path; a missing archive is a link error naming
# itself, which is a better failure than a configure-time refusal that also blocks the fix.

set(CMAKE_C_FLAGS_INIT   "${PS4_COMMON_FLAGS} -isystem ${ORBIS_COMPAT_DIR}/include -isystem ${OO_PS4_TOOLCHAIN}/include")

# Include order for C++: libc++ FIRST, the SDK's C headers second. This is not cosmetic.
#
# libc++ ships C-header wrappers of its own — include/c++/v1/{math.h,stdlib.h,string.h,…} —
# each of which `#include_next`es the platform header and then adds the C++ overloads the
# standard requires. The float `abs` overloads live in libc++'s <stdlib.h>; libc++'s
# <math.h> includes it; <cmath> then does an unconditional `using ::abs;` to pull all of
# them into namespace std. That chain only works if libc++'s directory is searched before
# the C one — otherwise `#include <stdlib.h>` from inside libc++ lands on musl's header,
# whose only `abs` is `int abs(int)`.
#
# With the C directory first, as it once was, the chain broke twice over:
#   * <cmath> failed outright — `using ::abs;` with no ::abs declared anywhere;
#   * the workaround for that, `-include stdlib.h`, made musl's `int abs(int)` visible in
#     every TU at global scope. <cmath> then compiled, but std::abs was the INTEGER
#     overload and nothing else, so `std::abs(x)` on a float silently truncated to int and
#     converted back. clang says so (-Wabsolute-value, on by default) but it is only a
#     warning: cross-building OpenGothic hit 37 such call sites. Codegen is the proof —
#     wrong order emits cvttss2si/cvtsi2ss, this one emits andps.
#
# A PREFIX IS STILL NEEDED, for the OTHER, load-bearing reason: the SDK's own headers are not
# self-contained — orbis/Net.h and friends name size_t without including <stddef.h> — and every TU
# that touches them relies on the prefix having declared it. Dropping it entirely fails on
# `unknown type name 'size_t'` in any TU that includes orbis/Net.h.
#
# ⚠ BUT IT IS NO LONGER stdlib.h. orbis-compat ships `orbis_prefix.h`, which is <stddef.h> and
# <stdint.h> and nothing else. Measured across all 189 of the SDK's orbis/ headers compiled alone:
#
#     no prefix                26 fail
#     -include stdlib.h        16 fail      <- what this file used to pass
#     -include orbis_prefix.h   7 fail
#
# Better coverage from a far smaller injection - two type headers instead of a whole libc one, which
# matters precisely because of the paragraph above: a libc header in every TU at global scope is how
# the std::abs truncation happened in the first place. (The remaining seven are SDK bugs - a typo'd
# type name, an undefined type, clang builtins redeclared - and no prefix reaches those.)
#
# ⚠ And orbis-compat goes BETWEEN them, not first: libc++'s directory has to stay ahead for the
# reason above, while the overlay only has to be ahead of the SDK's C headers, which it now is.
set(CMAKE_CXX_FLAGS_INIT "${PS4_COMMON_FLAGS} -isystem ${OO_PS4_TOOLCHAIN}/include/c++/v1 -isystem ${ORBIS_COMPAT_DIR}/include -isystem ${OO_PS4_TOOLCHAIN}/include -include orbis_prefix.h")
set(CMAKE_ASM_FLAGS_INIT "${PS4_COMMON_FLAGS} -isystem ${ORBIS_COMPAT_DIR}/include -isystem ${OO_PS4_TOOLCHAIN}/include")


# …and check it, because the failure mode above is silent. A one-off compile of a TU that
# takes std::abs of a float: it must resolve to an overload RETURNING float. execute_process
# rather than check_cxx_source_compiles because this is a toolchain file — try_compile
# re-enters it, and the compiler is not yet known to CMake the first time through.
if(NOT PS4_ABS_OVERLOAD_CHECKED AND NOT CMAKE_IN_TRY_COMPILE)
  set(PS4_ABS_CHECK_SRC "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/ps4-abs-check.cpp")
  file(WRITE "${PS4_ABS_CHECK_SRC}"
    "#include <cmath>\n"
    "#include <type_traits>\n"
    "static_assert(std::is_same<decltype(std::abs(-1.5f)), float>::value,\n"
    "              \"std::abs(float) binds to the integer overload\");\n"
    "static_assert(std::is_same<decltype(std::abs(-1.5)), double>::value,\n"
    "              \"std::abs(double) binds to the integer overload\");\n")
  separate_arguments(PS4_ABS_CHECK_FLAGS NATIVE_COMMAND "${CMAKE_CXX_FLAGS_INIT}")
  execute_process(
    COMMAND "${CMAKE_CXX_COMPILER}" ${PS4_ABS_CHECK_FLAGS} -std=c++17 -Werror=absolute-value
            -fsyntax-only "${PS4_ABS_CHECK_SRC}"
    RESULT_VARIABLE PS4_ABS_CHECK_RESULT
    OUTPUT_VARIABLE PS4_ABS_CHECK_OUT
    ERROR_VARIABLE  PS4_ABS_CHECK_OUT)
  if(NOT PS4_ABS_CHECK_RESULT EQUAL 0)
    message(FATAL_ERROR
      "PS4 toolchain: std::abs on a floating-point argument does not resolve to a "
      "floating-point overload. Every float abs() in every PS4 TU would truncate to int. "
      "Check the C++ include order in cmake/ps4-openorbis.cmake (libc++ must come first) "
      "and the SDK at '${OO_PS4_TOOLCHAIN}'.\n${PS4_ABS_CHECK_OUT}")
  endif()
  set(PS4_ABS_OVERLOAD_CHECKED TRUE CACHE INTERNAL "std::abs(float) resolves correctly")
endif()

# ⚠ --script POINTS AT THIS OVERLAY'S SCRIPT, NOT THE SDK's. cmake/orbis-tls.ld is the SDK's link.x
# with one rule corrected; its header says what and why. Short version: the SDK matches *(.tdata) and
# not *(.tdata.*), so anything built with -fdata-sections - which is most modern C, Mesa included -
# leaves its thread-locals as orphan sections, lld places them ahead of .data.rel.ro, and the
# ALIGN(0x4000) that starts the RW segment stops starting anything. The console then refuses the
# executable with "segment #1 is not page aligned", which reaches the user as "Cannot start the
# application" and appears in no log the title can write.
#
# --no-rosegment: modern ld.lld emits a separate, non-page-aligned read-only PT_LOAD.
# The PS4 loader rounds segment sizes to 0x1000 and maps raw p_vaddr, so that extra
# segment overlaps the text segment and the image is rejected.
set(PS4_LINK_FLAGS
  "-nostdlib -fuse-ld=lld -pie -Wl,-m,elf_x86_64 -Wl,--script=${ORBIS_COMPAT_DIR}/cmake/orbis-tls.ld -Wl,--eh-frame-hdr -Wl,--no-rosegment -L${OO_PS4_TOOLCHAIN}/lib")

# ⚠ orbis-compat with --whole-archive, and both halves matter. Nothing references backtrace() until
# something crashes, so a linker that keeps only what is referenced drops it; and this has to be
# appended HERE rather than set earlier, because this line is the one that reaches CMake - an
# EXE_LINKER_FLAGS_INIT set further up is simply overwritten by it.
# ⚠ THE ARCHIVE GOES ON THE EXECUTABLE LINK LINE, NOT THROUGH link_libraries(orbis::compat).
# Tempest's dependency graph export()s targets (spirv-cross among them), and CMake refuses to export
# a target whose link interface names something outside the export set - "requires target
# orbis_compat that is not in any export set". A global link_libraries() puts it on EVERY target,
# including those. EXE_LINKER_FLAGS reaches exactly what needs it: executables.
#
# orbis_compat_locate() above set ORBIS_COMPAT_LIBRARY, so the path is still not spliced by hand.
# --whole-archive because nothing REFERENCES an interposer: backtrace, __mmap and the rest exist to
# be preferred over the SDK's definitions, and a linker pulls an archive member in only when
# something already wants it.
set(CMAKE_EXE_LINKER_FLAGS_INIT
  "${PS4_LINK_FLAGS} -Wl,--whole-archive ${ORBIS_COMPAT_LIBRARY} -Wl,--no-whole-archive")

# crt1.o goes last, after the SDK libs — the order the SDK's own link rule uses.
set(CMAKE_C_STANDARD_LIBRARIES   "-lc -lkernel -lc++ ${OO_PS4_TOOLCHAIN}/lib/crt1.o")
set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES}")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_SHARED_LIBRARY_C_FLAGS   "")
set(CMAKE_SHARED_LIBRARY_CXX_FLAGS "")

set(CMAKE_FIND_ROOT_PATH "${OO_PS4_TOOLCHAIN}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Packaging. create-fself turns the ELF into an Orbis ELF (OELF) and the eboot.bin
# a console expects; unemups4 loads the plain ELF directly, so the eboot is only
# needed for the hardware path. PAID 0x3800000000000011 is the value every
# OpenOrbis sample uses for a homebrew eboot.
if(NOT COMMAND ps4_create_eboot)
  function(ps4_create_eboot target)
    find_program(PS4_CREATE_FSELF NAMES create-fself PATHS "${OO_PS4_BINDIR}" NO_DEFAULT_PATH)
    if(NOT PS4_CREATE_FSELF)
      message(WARNING "create-fself not found in ${OO_PS4_BINDIR}; skipping eboot for ${target}")
      return()
    endif()
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E env "OO_PS4_TOOLCHAIN=${OO_PS4_TOOLCHAIN}"
              "${PS4_CREATE_FSELF}"
              -in=$<TARGET_FILE:${target}>
              -out=$<TARGET_FILE_DIR:${target}>/${target}.oelf
              --eboot $<TARGET_FILE_DIR:${target}>/eboot.bin
              --paid 0x3800000000000011
      COMMENT "create-fself: ${target} -> eboot.bin")
  endfunction()
endif()

# ps4_create_pkg(): eboot.bin -> installable .pkg (the console path).
include("${CMAKE_CURRENT_LIST_DIR}/ps4-package.cmake")
