# Copyright © 2026 Mikołaj Mikołajczyk
# SPDX-License-Identifier: MIT
#
# include() this from a PlayStation 4 toolchain file or from a project that has one.
#
#   include(${ORBIS_COMPAT_DIR}/cmake/orbis-compat.cmake)
#   orbis_compat_locate()                       # finds the overlay, or FATAL_ERRORs
#   ...                                         # toolchain file: put the include path FIRST, by hand
#   orbis_compat_target()                       # defines orbis::compat, for the link line
#   orbis_compat_verify()                       # IN THE PROJECT, not the toolchain file - see below
#
# ⚠ WHERE EACH ONE BELONGS, learned by wiring a real project rather than by design:
#
#   locate()   toolchain file. It is only variables.
#   verify()   THE PROJECT, after project(). It compiles a test program, and while a toolchain file
#              is being read there is no working compiler to compile it with.
#   target()   either - but ⚠ do NOT follow it with a global link_libraries(orbis::compat) in a tree
#              that export()s targets. CMake refuses: "requires target orbis_compat that is not in
#              any export set", because link_libraries() reaches every target including exported
#              ones. Put the archive in CMAKE_EXE_LINKER_FLAGS instead, which reaches exactly what
#              needs it - executables - or link the target per-executable.
#
# ⚠ THE INCLUDE PATH CANNOT COME FROM THE TARGET, and this is the whole reason this file is three
# functions rather than one imported library. CMake puts CMAKE_<LANG>_FLAGS on the command line
# BEFORE a target's own -isystem entries, so a toolchain file that names the SDK's include directory
# wins over any INTERFACE_INCLUDE_DIRECTORIES this module could set. The overlay's bits/alltypes.h
# works by defining musl's __DEFINED_<name> guards before musl's copy is reached; reached second it
# compiles, does nothing, and says nothing. So the toolchain file places
#
#     -isystem ${ORBIS_COMPAT_INCLUDE_DIR}
#
# ahead of the SDK itself, and orbis_compat_verify() is what stops that from being a matter of trust.

# ------------------------------------------------------------------ locate
#
# In order: an explicit -DORBIS_COMPAT_DIR, the environment, then the directory this file is in.
function(orbis_compat_locate)
  if (NOT ORBIS_COMPAT_DIR)
    if (DEFINED ENV{ORBIS_COMPAT_DIR})
      set(ORBIS_COMPAT_DIR "$ENV{ORBIS_COMPAT_DIR}")
    else()
      get_filename_component(ORBIS_COMPAT_DIR "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    endif()
  endif()

  if (NOT EXISTS "${ORBIS_COMPAT_DIR}/include/bits/alltypes.h")
    message(FATAL_ERROR
      "orbis-compat not found at '${ORBIS_COMPAT_DIR}'. It carries the corrections this SDK needs - "
      "four pthread types it declares smaller than the kernel writes. Pass -DORBIS_COMPAT_DIR=<path> "
      "or set the ORBIS_COMPAT_DIR environment variable.")
  endif()

  set(ORBIS_COMPAT_DIR         "${ORBIS_COMPAT_DIR}"                            PARENT_SCOPE)
  set(ORBIS_COMPAT_INCLUDE_DIR "${ORBIS_COMPAT_DIR}/include"                    PARENT_SCOPE)
  set(ORBIS_COMPAT_LIBRARY     "${ORBIS_COMPAT_DIR}/build/liborbis-compat.a"    PARENT_SCOPE)
endfunction()

# ------------------------------------------------------------------ the link side
#
# ⚠ --whole-archive, and it is not optional. Nothing REFERENCES an interposer: __mmap, backtrace and
# the rest are there to be preferred over the SDK's definitions of the same names, and a linker
# pulls an archive member in only when something already wants it. Without --whole-archive the
# archive links cleanly and contributes nothing.
#
# ⚠ Called BEFORE the archive is built, this is still correct: the path is a link option, not an
# imported location, so CMake does not require it to exist at configure time. That is deliberate -
# the overlay's own build must be able to run with this same toolchain file.
#
# ⚠ libc GOES LAST, and the linker says so out loud when it does not. The interposers define __mmap,
# __munmap and __madvise, which the SDK's libc.a also defines; force-loading ours after libc has
# already contributed mmap.lo is a duplicate-symbol error, not a silent preference. Put -lc in
# CMAKE_<LANG>_STANDARD_LIBRARIES (where CMake emits it at the end of the line) rather than in
# CMAKE_EXE_LINKER_FLAGS. Measured: the same project links with the archive ahead of -lc and fails
# with 'duplicate symbol: __mmap' behind it.
function(orbis_compat_target)
  if (TARGET orbis_compat)
    return()
  endif()
  orbis_compat_locate()

  add_library(orbis_compat INTERFACE)
  add_library(orbis::compat ALIAS orbis_compat)
  target_link_options(orbis_compat INTERFACE
    "-Wl,--whole-archive" "${ORBIS_COMPAT_LIBRARY}" "-Wl,--no-whole-archive")

  # For the include path see the note at the top: it belongs to the toolchain file. It is exported
  # here so a consumer can splice it in without recomputing the path.
  set(ORBIS_COMPAT_INCLUDE_DIR "${ORBIS_COMPAT_INCLUDE_DIR}" PARENT_SCOPE)
  set(ORBIS_COMPAT_LIBRARY     "${ORBIS_COMPAT_LIBRARY}"     PARENT_SCOPE)
endfunction()

# ------------------------------------------------------------------ prove it arrived
#
# Compiles the same assertions test/sizes.c makes, with the flags this project will really use. It
# fails when the overlay's include directory is missing from them, or sits behind the SDK's - the
# one mistake that otherwise produces a working build against declarations known to be wrong.
function(orbis_compat_verify)
  orbis_compat_locate()
  include(CheckCSourceCompiles)
  check_c_source_compiles("
#include <pthread.h>
_Static_assert(sizeof(pthread_mutexattr_t)   >= sizeof(void *), \"undersized\");
_Static_assert(sizeof(pthread_condattr_t)    >= sizeof(void *), \"undersized\");
_Static_assert(sizeof(pthread_barrierattr_t) >= sizeof(void *), \"undersized\");
_Static_assert(sizeof(pthread_spinlock_t)    >= sizeof(void *), \"undersized\");
_Static_assert(sizeof(pthread_mutex_t) == 40, \"libc++'s layout moved\");
int main(void) { return 0; }
" ORBIS_COMPAT_TYPES_CORRECTED)

  if (NOT ORBIS_COMPAT_TYPES_CORRECTED)
    message(FATAL_ERROR
      "orbis-compat is present but its headers are NOT reaching the compiler. Put "
      "'-isystem ${ORBIS_COMPAT_INCLUDE_DIR}' in CMAKE_C_FLAGS_INIT and CMAKE_CXX_FLAGS_INIT AHEAD "
      "of the SDK's own include directory. Behind it, the corrections compile and do nothing.")
  endif()
endfunction()
