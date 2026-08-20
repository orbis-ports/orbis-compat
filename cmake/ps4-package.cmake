# PS4 .pkg packaging for OpenOrbis targets. Included by cmake/ps4-openorbis.cmake.
#
#   ps4_create_pkg(<target>
#     TITLE_ID      TMPS10001            # AAAA00000; TMPS1xxxx is this fork's range
#     TITLE         "Tempest Hello"
#     [VERSION      01.00]
#     [CONTENT_LABEL TEMPESTHELLO]       # <=16 chars, [A-Z0-9], padded with zeros
#     [EXTRA_FILES  <src>:<targ_path> ...])
#
# Appends POST_BUILD steps that turn the target's eboot.bin (produced by
# ps4_create_eboot) into <build>/IV0000-<TITLE_ID>_00-<LABEL>.pkg plus a
# <TITLE_ID>.pkg symlink. The actual SDK tool sequence lives in
# scripts/ps4/make-pkg.sh — that script is also usable standalone.
#
# Set -DPS4_BUILD_PKG=OFF to build only the ELF/eboot (emulator runs need no pkg).

if(DEFINED PS4_PACKAGE_CMAKE_INCLUDED)
  return()
endif()
set(PS4_PACKAGE_CMAKE_INCLUDED TRUE)

option(PS4_BUILD_PKG "Build a .pkg for every PS4 executable target" ON)

get_filename_component(PS4_PACKAGE_SCRIPT_DIR "${CMAKE_CURRENT_LIST_DIR}/../scripts/ps4" ABSOLUTE)

function(ps4_create_pkg target)
  if(NOT PS4_BUILD_PKG)
    return()
  endif()

  cmake_parse_arguments(ARG "" "TITLE_ID;TITLE;VERSION;CONTENT_LABEL" "EXTRA_FILES" ${ARGN})
  if(NOT ARG_TITLE_ID)
    message(FATAL_ERROR "ps4_create_pkg(${target}): TITLE_ID is required")
  endif()
  if(NOT ARG_TITLE)
    message(FATAL_ERROR "ps4_create_pkg(${target}): TITLE is required")
  endif()
  if(NOT ARG_VERSION)
    set(ARG_VERSION "01.00")
  endif()
  if(NOT ARG_CONTENT_LABEL)
    set(ARG_CONTENT_LABEL "${ARG_TITLE_ID}")
  endif()

  find_program(PS4_PKGTOOL NAMES PkgTool.Core PATHS "${OO_PS4_BINDIR}" NO_DEFAULT_PATH)
  if(NOT PS4_PKGTOOL)
    message(WARNING
      "PkgTool.Core not found in ${OO_PS4_BINDIR}; skipping .pkg for ${target}. "
      "The ELF and eboot.bin still build.")
    return()
  endif()

  # icon0.png depends only on TITLE_ID, so generate it once at configure time
  # rather than re-running the pure-python encoder on every build.
  set(_icon "")
  find_program(PS4_PYTHON3 NAMES python3 python)
  if(PS4_PYTHON3)
    set(_icon "${CMAKE_CURRENT_BINARY_DIR}/pkg-assets/icon0.png")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pkg-assets")
    execute_process(
      COMMAND "${PS4_PYTHON3}" "${PS4_PACKAGE_SCRIPT_DIR}/gen-icon0.py"
              --out "${_icon}" --title-id "${ARG_TITLE_ID}"
      RESULT_VARIABLE _icon_rc
      OUTPUT_QUIET)
    if(NOT _icon_rc EQUAL 0)
      message(WARNING "gen-icon0.py failed (${_icon_rc}); packaging ${target} without icon0.png")
      set(_icon "")
    endif()
  else()
    message(STATUS "python3 not found; packaging ${target} without icon0.png")
  endif()

  set(_args
    --eboot   "$<TARGET_FILE_DIR:${target}>/eboot.bin"
    --out-dir "$<TARGET_FILE_DIR:${target}>"
    --title-id "${ARG_TITLE_ID}"
    --title    "${ARG_TITLE}"
    --version  "${ARG_VERSION}"
    --content-label "${ARG_CONTENT_LABEL}"
    --sdk      "${OO_PS4_TOOLCHAIN}")
  if(_icon)
    list(APPEND _args --icon "${_icon}")
  endif()
  foreach(_extra IN LISTS ARG_EXTRA_FILES)
    list(APPEND _args --extra "${_extra}")
  endforeach()

  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND "${PS4_PACKAGE_SCRIPT_DIR}/make-pkg.sh" ${_args}
    VERBATIM
    COMMENT "pkg: ${target} -> IV0000-${ARG_TITLE_ID}_00-*.pkg")
endfunction()
