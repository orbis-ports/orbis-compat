# The two OPT-IN targets of optional/, for a consumer that wants the console's log channel.
#
#   ps4-netlog   UDP transport only (orbis_netlog.cpp). Obliges the link to carry -lSceNet.
#   ps4-app      the klog/netlog tee, the /app0/ps4-run.cfg reader and the termination policy
#                (ps4_app.cpp). Registers itself into orbis_log.h's hooks, which is how the
#                overlay's own corrections - stat(), the mmap translation, the crash handlers -
#                get a channel without knowing anything about this file.
#
#   add_subdirectory("${ORBIS_COMPAT_DIR}/optional" ps4-common)
#   target_link_libraries(<app> PRIVATE ps4-app)
#
# ⚠ NOTHING ELSE IN optional/ IS A TARGET HERE, and that is the point of the directory. orbis_bigheap.c
# and orbis_thread_atexit_stub.c are POLICY a consumer adopts by naming the source file: an unlimited
# heap that competes with the driver's arena, and a thread_local stub that leaks by design. They must
# not arrive because somebody linked a logger.
#
# ⚠ AND NEITHER TARGET IS IN liborbis-compat.a. Consumers link that archive with --whole-archive, so a
# member is present whether it was wanted or not; -lSceNet is not something the CTS or Mesa should
# inherit for having asked for a working mmap.
#
# Host receivers: ../scripts/ps4/log-receiver.py and ../scripts/ps4/logs.sh (udp, 18194).
cmake_minimum_required(VERSION 3.16)
project(orbis-compat-optional CXX)

if(NOT PS4)
  message(FATAL_ERROR "optional/ builds only with cmake/ps4-openorbis.cmake")
endif()

set(PS4_NETLOG_HOST "192.168.100.1" CACHE STRING "dev host that receives netlog datagrams")
set(PS4_NETLOG_PORT "18194"         CACHE STRING "udp port scripts/ps4/logs.sh binds")

add_library(ps4-netlog STATIC orbis_netlog.cpp)
target_include_directories(ps4-netlog PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../include")
target_compile_definitions(ps4-netlog PRIVATE
  NETLOG_HOST="${PS4_NETLOG_HOST}"
  NETLOG_PORT=${PS4_NETLOG_PORT})
target_link_libraries(ps4-netlog PUBLIC -lSceNet)

# Build stamp. It exists to answer "is the console running the package I just installed?" from the
# first log line.
#
# ⚠ THE REV DESCRIBES THE CONSUMER, NOT THE OVERLAY. It used to be `git describe` of the directory
# holding ps4_app.cpp, which was the Tempest fork; that directory is orbis-compat now, and an overlay
# revision would answer a question nobody asks. What is in doubt after an install is which build of
# the TITLE is running, so the default is the top-level project. Override with -DPS4_APP_STAMP_REPO
# when a consumer knows better.
set(PS4_APP_STAMP_REPO "${CMAKE_SOURCE_DIR}" CACHE PATH
    "repository whose revision goes into the build stamp; the consuming project by default")

# The DATE half is fixed the first time a build directory is configured and is sticky afterwards. A
# stamp that followed the wall clock would recompile ps4_app.cpp on every configure and hand two runs
# different bytes for a difference nobody could point at. A build that did not change does not get a
# new stamp; delete the directory to get one. (The REV half is not cached: a source that moved SHOULD
# produce different bytes.)
#
# `date` and not string(TIMESTAMP): the latter honours SOURCE_DATE_EPOCH, which the reproducible-build
# devShell pins to 1980.
if(NOT DEFINED PS4_APP_BUILD_DATE_STICKY)
  execute_process(
    COMMAND date -u "+%Y-%m-%d %H:%M"
    OUTPUT_VARIABLE PS4_APP_BUILD_DATE_NOW
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)
  if(NOT PS4_APP_BUILD_DATE_NOW)
    string(TIMESTAMP PS4_APP_BUILD_DATE_NOW "%Y-%m-%d %H:%M" UTC)
  endif()
  set(PS4_APP_BUILD_DATE_STICKY "${PS4_APP_BUILD_DATE_NOW}" CACHE INTERNAL
      "build-stamp date, fixed at the first configure of this directory")
endif()
set(PS4_APP_BUILD_DATE "${PS4_APP_BUILD_DATE_STICKY}")
execute_process(
  COMMAND git -C "${PS4_APP_STAMP_REPO}" describe --always --dirty
  OUTPUT_VARIABLE PS4_APP_BUILD_REV
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET)
if(NOT PS4_APP_BUILD_REV)
  set(PS4_APP_BUILD_REV "no-git")
endif()

add_library(ps4-app STATIC ps4_app.cpp)
target_include_directories(ps4-app PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../include")
target_link_libraries(ps4-app PUBLIC ps4-netlog)
target_compile_definitions(ps4-app PUBLIC
  PS4_APP_BUILD_STAMP="${PS4_APP_BUILD_DATE} ${PS4_APP_BUILD_REV}")
