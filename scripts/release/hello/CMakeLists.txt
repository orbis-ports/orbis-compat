# The smallest thing that exercises every part of the bundle's layout, and nothing else.
#
#   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<bundle>/toolchain/orbis-sdk.cmake
#   cmake --build build
#
# It is also the worked example a stranger copies: this file plus hello.c is the complete
# answer to "how do I build my own thing for the PS4 with this".
#
# ⚠ IT DELIBERATELY DOES NOT USE MESA. A hello-world that linked RADV would fail for reasons
# that have nothing to do with whether the bundle is laid out correctly, and the gate needs
# one stage that isolates layout from everything else. The real port in stage 5 of
# bundle-gate.sh is what covers Mesa, C++ and libc++ include ordering.
cmake_minimum_required(VERSION 3.16)

# ⚠ THE GUARD IS BELOW project(), AND IT USED TO BE ABOVE IT, WHERE IT COULD ONLY EVER FIRE.
# CMake reads CMAKE_TOOLCHAIN_FILE when project() runs, not when the cache is populated, so
# before that line nothing the toolchain file sets exists yet. ORBIS_COMPAT_DIR was therefore
# undefined at the top of this file even on a perfectly correct invocation, and the check
# refused every configure with a message telling the caller to pass exactly the flag they had
# just passed:
#
#     cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<bundle>/toolchain/orbis-sdk.cmake
#     CMake Error at CMakeLists.txt:16 (message):
#       configure with -DCMAKE_TOOLCHAIN_FILE=<bundle>/toolchain/orbis-sdk.cmake.
#
# Found by bundle-gate.sh stage 3 on 2026-09-17, on the first run of either script. The gate
# was passing the flag correctly; this file could not see it yet.
project(orbis_hello C)

# Now it means something: the toolchain file has been read, so this is a real statement about
# whether the PS4 toolchain is in effect rather than about line ordering. PS4 is set by
# ps4-openorbis.cmake, which orbis-sdk.cmake includes; ORBIS_COMPAT_DIR is how that file found
# the overlay. Either one missing means this is a host build wearing the wrong name.
if(NOT DEFINED ORBIS_COMPAT_DIR OR NOT PS4)
  message(FATAL_ERROR
    "configure with -DCMAKE_TOOLCHAIN_FILE=<bundle>/toolchain/orbis-sdk.cmake. "
    "Without it nothing here targets the PS4 and a 'successful' build means nothing.")
endif()

add_executable(hello hello.c)

# ⚠ NOTHING LINKS orbis::compat HERE, AND THAT IS NOT AN OVERSIGHT. The toolchain file already
# puts the archive on EVERY executable link line, force-loaded:
#
#   ps4-openorbis.cmake:
#     set(CMAKE_EXE_LINKER_FLAGS_INIT
#         "... -Wl,--whole-archive ${ORBIS_COMPAT_LIBRARY} -Wl,--no-whole-archive")
#
# Adding target_link_libraries(hello PRIVATE orbis::compat) on top of that put it there a
# SECOND time, and --whole-archive twice means every member is loaded twice. Measured by
# bundle-gate.sh stage 3 on 2026-09-17 - CMakeFiles/hello.dir/link.txt carried both:
#
#   25 -Wl,--whole-archive   26 .../liborbis-compat.a   27 -Wl,--no-whole-archive
#   28 -Wl,--whole-archive   29 .../liborbis-compat.a   30 -Wl,--no-whole-archive
#
#   ld.lld: error: duplicate symbol: clock_gettime
#   >>> defined at orbis_clock.cpp
#   >>>            orbis_clock.o:(clock_gettime) in archive .../liborbis-compat.a
#   >>> defined at orbis_clock.cpp
#   >>>            orbis_clock.o:(.text+0x30) in archive .../liborbis-compat.a
#
# The same object, in the same archive, twice. ps4-openorbis.cmake's own comment says where
# the archive belongs and why it is EXE_LINKER_FLAGS rather than a link_libraries(): a global
# link_libraries() puts it on every target including exported ones, which CMake refuses to
# export. This file just has to not repeat what the toolchain file already did.
#
# The reason it must be force-loaded at all still holds and is worth keeping written down:
# this program references not one symbol from the overlay, so without --whole-archive the
# linker would pull in no member at all and the interposers - the whole reason the overlay
# exists - would be silently absent from an image that linked fine.

# Compiles the overlay's type assertions with THIS project's real flags, at configure time.
# It is what turns "we trust the include order" into an error you get in seconds rather than
# a pthread_mutexattr_t overrun you get on hardware. See orbis-compat README §3.1.
orbis_compat_verify()

# eboot.bin, via create-fself out of the bundle's sdk/bin/<host>.
if(COMMAND ps4_create_eboot)
  ps4_create_eboot(hello)
endif()
