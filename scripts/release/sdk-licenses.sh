#!/usr/bin/env bash
# The licence ledger for the redistributable SDK bundle, and the generator of NOTICE.md.
#
# WHY THIS EXISTS AT ALL. Until now this repository only CONSUMED the OpenOrbis SDK: a
# consumer fetched the tarball themselves, and whether that tarball carries the notices its
# own components require was upstream's problem. A bundle changes that. The moment we put
# musl's libc.a and LLVM's libc++.a inside a tarball with our name on it, MIT's "the above
# copyright notice ... shall be included in all copies" and Apache-2.0 §4(a)/(d) become
# OUR obligation, and the SDK tarball does not discharge them:
#
#   MEASURED, v0.5.4 / toolchain-llvm-18.tar.gz unpacked at ~/src/unemu-org/unemups4/data/oo_sdk
#   the whole tree contains EIGHT files whose name says "licence", and only eight:
#     LICENSE                                   GPL-3.0 (674 lines)
#     bin/{linux,macos,windows}/LICENSE.txt     LGPL-3.0, all three byte-identical
#     include/stb/LICENSE                       stb's MIT/Unlicense dual text
#     include/stb/data/herringbone/license.txt  test-data notice
#     include/stb/tests/pngsuite/PngSuite.LICENSE
#     include/SDL2/SDL_copying.h                SDL's Zlib text, as a header
#   ⚠ There is NO musl COPYRIGHT, NO LLVM LICENSE.TXT, NO FTL.TXT anywhere in the tree,
#   and upstream's own README says out loud that it knows: "The accompanying LLVM binaries
#   are licensed under the Apache 2.0 license and is owned by LLVM. Under that license,
#   redistribution is allowed." - which is true, and Apache-2.0 §4 also says the copy has to
#   carry the licence. It does not.
#
# So: this script owns a table of every component the bundle redistributes, gathers the
# licence text each one requires into `licenses/`, generates `NOTICE.md` from that same
# table, and fails offline when any of it drifts.
#
# The design is lifted, deliberately and almost verbatim, from ~/src/unemu-org/oracles's
# fetch-oracles.sh: one table that drives both the gathering and the checking, a generated
# NOTICE that `verify` refuses to let go stale, and sha256 over everything. Do not invent a
# second shape for this problem in this organisation.
#
# Usage:
#   ./sdk-licenses.sh fetch [--sdk DIR]   gather every licence text into licenses/
#   ./sdk-licenses.sh sums                rewrite licenses/SHA256SUMS from what is on disk
#   ./sdk-licenses.sh notice              regenerate NOTICE.md from the table below
#   ./sdk-licenses.sh verify              OFFLINE: the gate. licenses/ vs sums, NOTICE freshness
#   ./sdk-licenses.sh verify --upstream   re-download the url: rows and diff against the copies
#   ./sdk-licenses.sh table               print the component table, comments stripped
#
# Exit codes (the same five oracles uses, meaning the same five things):
#   0 ok · 1 LOCAL DRIFT · 2 usage · 3 UPSTREAM MOVED · 4 INCOMPLETE (an upstream was
#   unreachable, so the check did not finish and proves nothing).
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
# This script lives in TWO places and has to work in both: scripts/release/ in the checkout,
# and toolchain/ inside a cut bundle, where it is what `verify.sh` runs to check the bundle's
# own licenses/ and NOTICE.md. Resolve the root by looking for them rather than by counting
# directory levels - a hardcoded ../.. is right in one location and silently wrong in the
# other, and "silently wrong" here means verifying some checkout instead of the bundle.
# BUNDLE.txt one level up is the unambiguous marker of the bundle case; a checkout never has
# one. Everything else falls through to ../.., which is the checkout layout - including the
# very first run, before licenses/ exists at all.
if [ -f "$HERE/../BUNDLE.txt" ]; then REPO="$(cd "$HERE/.." && pwd -P)"
else                                  REPO="$(cd "$HERE/../.." && pwd -P)"; fi
LICDIR="$REPO/licenses"
NOTICE="$REPO/NOTICE.md"

# --- where the local trees are, when a row has to copy out of one ------------------------
# ⚠ OO_PS4_TOOLCHAIN ON A DEVELOPER MACHINE MAY POINT AT A SOURCE STASH, NOT AN SDK. On the
# machine this was written on it pointed at ~/src/unemu-org/oracles/openorbis, which is a
# git checkout with no lib/ at all. Every path below is probed for link.x AND lib/libc.a, so
# a stash is rejected by content rather than accepted by name.
SDK="${OO_PS4_TOOLCHAIN:-$HOME/.local/opt/openorbis}"
MESA_SRC="${ORBIS_MESA_CHECKOUT:-$REPO/../mesa-ps4}"

# --- pins. Change deliberately, then re-run `fetch` and `notice`. ------------------------
# ⚠ THE SDK ASSET IS CALLED toolchain-llvm-18 AND ITS C++ RUNTIME IS LLVM 11. Measured, not
# inferred: include/c++/v1/__config says `#define _LIBCPP_VERSION 11000`, and `strings` over
# lib/libc++.a, lib/libc++abi.a and lib/libclang_rt.builtins-x86_64.a finds "clang version
# 11.0.0" and nothing else. lib/libc.a is the one archive with BOTH "clang version 11.0.0"
# and "clang version 18.1.4" members - musl was partially rebuilt with the newer compiler,
# the C++ runtime was not. The licence texts are therefore pinned at llvmorg-11.0.0, because
# that is the release those binaries came from. (Which compiler built an object does not
# change its licence; which release it came from does, because LICENSE.TXT has a per-release
# third-party appendix and the four projects' copies are NOT identical to each other.)
LLVM_TAG="llvmorg-11.0.0"
# musl: the SDK's libc.a is OpenOrbis' FORK of musl, not upstream musl, and that is the tree
# whose COPYRIGHT has to travel. upstream git.musl-libc.org is also unreachable from some
# networks (http 000 from here), which would make a pin nobody can check.
MUSL_REPO="OpenOrbis/musl"
MUSL_COMMIT="2b0bb8ce2fa64ac255cb656785729d159a5f074c"
# SDL: include/SDL2/SDL_version.h says 2.0.9 (MAJOR 2, MINOR 0, PATCHLEVEL 9), and that is
# the pin. ⚠ The header set does not agree with itself about which release it is: 78 of the
# 87 headers say "Copyright (C) 1997-2019" while 2.0.9's own COPYING.txt says 1997-2018, and
# one header (SDL_image.h, a different project) says 1997-2020. The declared version is the
# only statement the tree makes about itself, so it is what the pin follows; the year
# disagreement is recorded in LICENSING.md rather than resolved by picking a nicer tag. The
# inline notice in each header is the authoritative copyright line either way - the Zlib
# text itself is unchanged across all of these releases.
SDL_TAG="release-2.0.9"
# SDL_image is a SEPARATE project. SDL_image.h declares 2.0.6, and libsdl-org/SDL_image has
# no reachable `release-2.0.6` tag (404 on the raw path; the tag listing stops at 2.0.5), so
# the nearest tagged release whose COPYING.txt states the terms these bytes are under is
# what ships. ⚠ The exact upstream of the shipped header is NOT established.
SDL_IMAGE_TAG="release-2.0.5"
# SDL_ttf is a THIRD project in the same directory. SDL_ttf.h declares 2.0.15 and that tag
# exists upstream. ⚠ There is no libSDL2_ttf.a in lib/ - the header ships with no library
# behind it, which is a defect of the SDK, not of this ledger.
SDL_TTF_TAG="release-2.0.15"
# FreeType: include/freetype/freetype.h says FREETYPE_MAJOR 2, FREETYPE_MINOR 5, and the
# per-file headers say "Copyright 1996-2013". VER-2-5-1 is the release that matches both.
FREETYPE_TAG="VER-2-5-1"
# FreeBSD: only for include/sys/ioccom.h in THIS repository, whose macros follow FreeBSD's.
FREEBSD_REF="release/9.0.0"
# Mesa: licences/MIT and licences/Apache-2.0 come out of the Mesa checkout the bundle was
# built from, so they follow the bundle's Mesa commit automatically. This ref is the
# fallback used by `fetch` when no checkout is present.
MESA_REF="main"
ZLIB_TAG="v1.3.1"

log(){  printf '\033[1;34m==>\033[0m %s\n' "$*"; }
warn(){ printf '\033[1;33m!!\033[0m %s\n' "$*" >&2; }
err(){  printf '\033[1;31mXX\033[0m %s\n' "$*" >&2; }

# macOS ships shasum, Linux ships sha256sum, CI is Linux and the developer machine is not.
_sha256(){ if command -v sha256sum >/dev/null 2>&1; then sha256sum "$@"; else shasum -a 256 "$@"; fi; }
_sha256c(){ if command -v sha256sum >/dev/null 2>&1; then sha256sum -c --quiet -; else shasum -a 256 -c --quiet -; fi; }
_size(){ if stat -c%s "$1" >/dev/null 2>&1; then stat -c%s "$1"; else stat -f%z "$1"; fi; }

# ===========================================================================================
# THE COMPONENT TABLE. One row per thing the bundle redistributes. `#` lines are prose and
# are stripped by table_rows. Columns, `|`-separated, trimmed:
#
#   1 id        short key, also the NOTICE.md row order
#   2 paths     where it lands INSIDE the bundle (comma-separated; informational + checked
#               by verify-sdk-bundle.sh, which reads this same table)
#   3 spdx      SPDX identifier, or a plain-English string when no clean SPDX id applies
#   4 licfile   the file under licenses/ whose text MUST travel with the binary, or `-`
#   5 source    how licenses/<licfile> is obtained:
#                 sdk:<path>   copy out of the unpacked OpenOrbis SDK
#                 repo:<path>  copy out of this repository
#                 mesa:<path>  copy out of the Mesa checkout (falls back to url on `fetch`)
#                 url:<URL>    download at the pin (the only rows `verify --upstream` checks)
#                 -            no separate text needed; the note says why
#   6 origin    upstream project URL
#   7 pin       the ref/tag/commit/version that identifies WHICH upstream this came from
#   8 note      one line of prose. No `|` in it.
# ===========================================================================================
components(){ cat <<EOF
# --- the SDK's own GPL-3.0 core --------------------------------------------------------
# link.x, the crt objects and everything under include/orbis/ are OpenOrbis' own work. The
# repository LICENSE is the GPLv3 text and its README says "This project is licensed under
# the GPLv3 license". Nothing in that repository says "or any later version" as a grant and
# nothing grants a linking exception, hence GPL-3.0-only.
#
# ⚠ ZERO of the 189 files under include/orbis/ carries a copyright or licence notice. That
# was checked by grep for copyright/licence/SPDX/"permission is hereby" across the whole
# directory: eleven lines match and every one of them is an API name, not a notice -
# scePlayReadyLicenseAcq*, scePadGetLicenseControllerInformation,
# ORBIS_BGFT_TASK_SUB_TYPE_GAME_LICENSE, and the declaration \`void _PJP_C_Copyright();\`,
# which is a Sony symbol this header declares, not a statement about this header. The root
# LICENSE is therefore the ONLY thing that licenses them, and it has to travel.
sdk-core      | sdk/link.x, sdk/include/orbis/**            | GPL-3.0-only | GPL-3.0.txt  | sdk:LICENSE                   | https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain | v0.5.4 | The SDK's own headers and linker script. No per-file notices anywhere; the root LICENSE is all there is.
# ⚠ THE CRT OBJECTS ARE NOT ONE COMPONENT AND SIX OF THE SEVEN ARE NOT GPL. This row used to
# say all of them were GPL-3.0; that was wrong, and the evidence that corrects it is in the
# objects themselves. objdump -t reports each one's STT_FILE - the source it was compiled
# from - and the exported symbols agree:
#
#   crt1.o     crt1.c      _start _start_ps4_c __ps4Argv
#   crt_dyn.o  crt_dyn.c   _start _start_ps4_c __ps4Argv
#   rcrt1.o    rcrt1.c     _start _start_c __dls2
#   Scrt1.o    Scrt1.c     _start _start_c
#   crti.o     (none)      _init _fini            assembled from .s, so no STT_FILE
#   crtn.o     (none)      -                      the same
#   crtlib.o   crtlib.c    module_start module_stop _init _fini __init_array_{start,end}
#
# OpenOrbis/musl (MIT) carries crt/ps4/{crt1.c,crt_dyn.c,crti.s,crtn.s} and crt/{rcrt1.c,
# Scrt1.c}; its crt/ps4/crt1.c defines __ps4Argv and _start_ps4_c, the same two symbols the
# shipped crt1.o exports. The toolchain repository's src/crt/ contains ONLY build.bat,
# build.sh, crtlib.c and crtlib.S - no crt1.c, no crt_dyn.c. And src/README.md says it
# outright: "Games/apps CRT is handled by musl already, however we have a crtlib stub here
# for PRXs to use, crtlib.o."
#
# So six objects are musl's, MIT, and their notice is the same missing musl COPYRIGHT the
# libc row is about - which makes that finding broader, not narrower. One object, crtlib.o,
# is the toolchain repository's own and is GPL-3.0-only.
#
# ⚠ AND crtlib.o IS THE ONLY GPL-3.0 OBJECT THAT CAN REACH A CONSUMER'S OUTPUT, on a path
# most consumers never take. cmake/ps4-openorbis.cmake puts exactly one crt object on an
# executable's link line - crt1.o, and its comment says why none of the other four is there -
# so an eboot.bin carries no GPL-3.0 crt at all. crtlib.o is a MODULE entry point and belongs
# on a .prx/.sprx link line. Its corresponding source IS in the tarball at src/crt/crtlib.c,
# which is why the bundle keeps that directory and why GPL-3.0 §6 is satisfied for it.
sdk-crt-musl  | sdk/lib/crt1.o, sdk/lib/crti.o, sdk/lib/crtn.o, sdk/lib/crt_dyn.o, sdk/lib/rcrt1.o, sdk/lib/Scrt1.o | MIT | MIT-musl.txt | url:https://raw.githubusercontent.com/${MUSL_REPO}/${MUSL_COMMIT}/COPYRIGHT | https://github.com/${MUSL_REPO} | ${MUSL_COMMIT} | Six of the seven startup objects, built from musl's crt/ps4 and crt/. Identified by each object's STT_FILE and exported symbols, not by its filename.
sdk-crt-lib   | sdk/lib/crtlib.o, sdk/src/crt/**  | GPL-3.0-only | GPL-3.0.txt | sdk:LICENSE | https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain | v0.5.4 | The PRX module stub, and the ONLY GPL-3.0 crt object. Its source ships beside it at sdk/src/crt/crtlib.c. It is not on an executable's link line.
# --- musl ------------------------------------------------------------------------------
# 1483 members in lib/libc.a; 93 headers at include/*.h plus bits/ sys/ arpa/ net/ netinet/
# netpacket/ scsi/. Not one of them carries a notice - grep for copyright/licence/SPDX over
# all of them returns exactly one file, and that file is ft2build.h, which is FreeType's.
musl          | sdk/lib/libc.a, sdk/include/*.h, sdk/include/bits/**, sdk/include/sys/**, sdk/include/arpa/**, sdk/include/net/**, sdk/include/netinet/**, sdk/include/netpacket/**, sdk/include/scsi/** | MIT | MIT-musl.txt | url:https://raw.githubusercontent.com/${MUSL_REPO}/${MUSL_COMMIT}/COPYRIGHT | https://github.com/${MUSL_REPO} | ${MUSL_COMMIT} | The libc and its headers. OpenOrbis' fork rather than upstream musl - that is the tree these bytes came from.
# --- LLVM runtimes ---------------------------------------------------------------------
# Four separate LICENSE.TXT files, NOT one: their sha256 differ, because each carries its
# own "Copyrights and Licenses for Third Party Software" appendix. Shipping libcxx's text
# for compiler-rt's binary would be the wrong notice, quietly.
llvm-libcxx   | sdk/lib/libc++.a, sdk/lib/libc++experimental.a, sdk/include/c++/v1/** | Apache-2.0 WITH LLVM-exception | LLVM-libcxx-LICENSE.TXT   | url:https://raw.githubusercontent.com/llvm/llvm-project/${LLVM_TAG}/libcxx/LICENSE.TXT      | https://github.com/llvm/llvm-project | ${LLVM_TAG} | libc++ and its 180 headers. _LIBCPP_VERSION 11000 is the measurement that picks the pin.
llvm-libcxxabi| sdk/lib/libc++abi.a                        | Apache-2.0 WITH LLVM-exception | LLVM-libcxxabi-LICENSE.TXT | url:https://raw.githubusercontent.com/llvm/llvm-project/${LLVM_TAG}/libcxxabi/LICENSE.TXT   | https://github.com/llvm/llvm-project | ${LLVM_TAG} | 21 members. Its LICENSE.TXT is not byte-identical to libc++'s.
llvm-libunwind| sdk/lib/libunwind.a                        | Apache-2.0 WITH LLVM-exception | LLVM-libunwind-LICENSE.TXT | url:https://raw.githubusercontent.com/llvm/llvm-project/${LLVM_TAG}/libunwind/LICENSE.TXT   | https://github.com/llvm/llvm-project | ${LLVM_TAG} | Also present INSIDE libc++.a as libunwind.cpp.o and Unwind-sjlj.c.o - the same code, twice.
llvm-builtins | sdk/lib/libclang_rt.builtins-x86_64.a      | Apache-2.0 WITH LLVM-exception | LLVM-compiler-rt-LICENSE.TXT | url:https://raw.githubusercontent.com/llvm/llvm-project/${LLVM_TAG}/compiler-rt/LICENSE.TXT | https://github.com/llvm/llvm-project | ${LLVM_TAG} | 159 members. compiler-rt's own text, which differs from the other three.
# --- the generated import stubs --------------------------------------------------------
# ⚠ THE ONE ROW WITH NO ANSWER. 422 .so files, and they are empty by construction:
# libkernel.so has .dynsym 0x5c10 (982 entries; objdump -T lists 981 symbols) against .text
# 0x2654 = 9812 bytes, ten bytes per symbol - no symbol has a real body. They were produced by OpenOrbis/orbis-lib-gen from OpenOrbis/ps4libdoc, and BOTH of
# those repositories are archived with NO LICENSE file of any kind. There is no notice to
# ship because there is no grant to quote. LICENSING.md states this as unresolved.
sdk-stubs     | sdk/lib/*.so                               | UNRESOLVED - see LICENSING.md | - | - | https://github.com/OpenOrbis/ps4libdoc | archived, no LICENSE | ~422 generated import libraries with empty bodies. Their generator and their input data carry no licence at all.
# --- the packaging tools ---------------------------------------------------------------
sdk-oo-tools  | sdk/bin/*/create-fself*, sdk/bin/*/create-gp4*, sdk/bin/*/readoelf* | GPL-3.0-only | GPL-3.0.txt | sdk:LICENSE | https://github.com/OpenOrbis/create-fself | v0.5.4 | Go binaries; \`strings\` finds github.com/OpenOrbis/create-fself/pkg/fself inside create-fself. ⚠ Only bin/LICENSE.txt sits beside them and that is the LGPL, which is LibOrbisPkg's, not theirs.
sdk-pkgtool   | sdk/bin/*/PkgTool.Core*, sdk/bin/*/LibOrbisPkg*.dll, sdk/bin/*/PkgEditor.exe | LGPL-3.0-only | LGPL-3.0.txt | sdk:bin/linux/LICENSE.txt | https://github.com/maxton/LibOrbisPkg | bundled with v0.5.4 | bin/README.md: "All code in this repository is licensed under the GNU LGPL version 3". This one IS correctly noticed upstream.
# --- third-party headers and libraries the SDK carries ---------------------------------
sdl2          | sdk/include/SDL2/**, sdk/lib/libSDL2.a, sdk/lib/libSDL2main.a | Zlib | Zlib-SDL.txt | url:https://raw.githubusercontent.com/libsdl-org/SDL/${SDL_TAG}/COPYING.txt | https://github.com/libsdl-org/SDL | ${SDL_TAG} | 86 headers plus two archives. The headers carry the Zlib text inline; the ARCHIVES carry nothing, which is why a separate file ships.
sdl2-image    | sdk/include/SDL2/SDL_image.h, sdk/lib/libSDL2_image.a | Zlib | Zlib-SDL_image.txt | url:https://raw.githubusercontent.com/libsdl-org/SDL_image/${SDL_IMAGE_TAG}/COPYING.txt | https://github.com/libsdl-org/SDL_image | ${SDL_IMAGE_TAG} | A separate project that sits in the same include directory. Its header declares 2.0.6 and upstream has no reachable tag of that name - see LICENSING.md.
sdl2-ttf      | sdk/include/SDL2/SDL_ttf.h                 | Zlib | Zlib-SDL_ttf.txt | url:https://raw.githubusercontent.com/libsdl-org/SDL_ttf/${SDL_TTF_TAG}/COPYING.txt | https://github.com/libsdl-org/SDL_ttf | ${SDL_TTF_TAG} | A third project in the same directory, © 2001-2020. ⚠ Header only - there is no libSDL2_ttf.a in the SDK's lib/, so nothing links against it.
# ⚠ include/SDL2 IS NOT ONE PROJECT, and counting it as one was the mistake this row corrects.
# Eight of its 87 files carry no "Copyright (C) 1997-" line at all: SDL_revision.h (generated,
# one line, no notice) and seven Khronos headers - SDL_opengl_glext.h, the four
# SDL_opengles2_* files and SDL_vulkan.h. The Khronos ones are MIT, © 2008-2014 The Khronos
# Group Inc.; SDL_vulkan.h is Zlib, © 2017 Mark Callow. All of them carry the FULL licence
# text inline, so nothing separate has to ship for them - which is why this row's licfile is
# a plain dash and says "inline" - a different situation from sdk-stubs, where no text exists.
sdl2-khronos  | sdk/include/SDL2/SDL_opengl_glext.h, sdk/include/SDL2/SDL_opengles2_*.h, sdk/include/SDL2/SDL_vulkan.h | MIT (Khronos) and Zlib (SDL_vulkan.h) | - | - | https://github.com/KhronosGroup/OpenGL-Registry | shipped in include/SDL2 | inline - each file carries its own complete licence text, so no separate file is required.
# ⚠ THE DUAL CHOICE IS UNDOCUMENTED AND STAYS THAT WAY. FreeType is FTL or GPLv2 at the
# recipient's option, and 86 of the 88 shipped headers name only "LICENSE.TXT" - the FTL -
# in their boilerplate. No file in the tarball mentions GPLv2 at all, and FTL.TXT itself is
# absent. We ship FTL.TXT because that is the licence the shipped files actually point at;
# we do NOT assert that OpenOrbis made a choice, because nothing in the tree says they did.
freetype      | sdk/include/freetype/**, sdk/include/ft2build.h | FTL (dual FTL OR GPL-2.0, choice undocumented) | FTL.TXT | url:https://raw.githubusercontent.com/freetype/freetype/${FREETYPE_TAG}/docs/FTL.TXT | https://github.com/freetype/freetype | ${FREETYPE_TAG} | 88 headers, FREETYPE_MAJOR 2 / MINOR 5, "Copyright 1996-2013". FTL.TXT is NOT in the SDK tarball.
stb           | sdk/include/stb/**                         | MIT OR Unlicense | MIT-stb.txt | sdk:include/stb/LICENSE | https://github.com/nothings/stb | bundled with v0.5.4 | The only third-party component in the tarball whose notice was already present.
# --- this repository -------------------------------------------------------------------
orbis-compat  | orbis-compat/include/**, orbis-compat/src/**, orbis-compat/optional/**, orbis-compat/vkloader/**, orbis-compat/cmake/*.cmake, orbis-compat/scripts/**, orbis-compat/test/**, orbis-compat/build/liborbis-compat.a | MIT | MIT-orbis-compat.txt | repo:LICENSE | https://github.com/orbis-ports/orbis-compat | see BUNDLE.txt | The overlay, minus the two files below which are NOT MIT. Since 2026-09-18 the cmake/*.cmake and vkloader/** staged under this path come from https://github.com/orbis-ports/orbis-porting-kit - same MIT terms, same author, different repository; the bundle keeps the old layout because toolchain/orbis-sdk.cmake names it.
# This repository's OWN crt, which exists so that a consumer need not link the SDK's at all.
# ⚠ IT IS SELECTED, NOT SUBSTITUTED: ps4-openorbis.cmake takes ORBIS_CRT=sdk (the default) or
# ORBIS_CRT=own, and only the second reaches orbis-compat/build/crt/crt1.o. A bundle carries
# both sets and this row describes the second; sdk-crt-musl and sdk-crt-lib describe the
# first. Which one a given binary was linked with is a property of that build, not of the
# bundle, and nothing here can tell you after the fact - read the build's own link.txt.
compat-crt    | orbis-compat/crt/**, orbis-compat/build/crt/*.o | MIT | MIT-orbis-compat.txt | repo:LICENSE | https://github.com/orbis-ports/orbis-compat | see BUNDLE.txt | orbis-compat's own startup objects, written here rather than taken from either upstream. Absent from a bundle cut before they existed.
# ⚠ THIS FILE IS GPL-3.0-only AND IT IS ON THE LINK LINE OF EVERY EXECUTABLE THE BUNDLE
# BUILDS. cmake/orbis-tls.ld is the SDK's link.x with two match patterns added to the .tls
# rule; a derivative of a GPL-3.0 file cannot be relicensed by the deriver. It carried an
# MIT header until 2026-09-17 and that header was wrong. Its own header now records the
# whole check, including that the four QUAD() values at the top of .text are the ASCII of
# "/libexec/ld-elf.so.1" and land in every eboot.bin linked through it - a fact a reader
# needs, NOT a conclusion about what a built binary's licence then is. Nobody has
# established that, and neither this table nor NOTICE.md pretends to.
compat-tls-ld | toolchain/orbis-tls.ld, orbis-compat/cmake/orbis-tls.ld | GPL-3.0-only | GPL-3.0.txt | sdk:LICENSE | https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain | v0.5.4 link.x | Derived from the SDK's link.x. Two substantive lines differ; the rest of the diff is its own comment.
compat-ioccom | orbis-compat/include/sys/ioccom.h          | BSD-3-Clause | BSD-3-Clause-FreeBSD.txt | url:https://raw.githubusercontent.com/freebsd/freebsd-src/${FREEBSD_REF}/COPYRIGHT | https://github.com/freebsd/freebsd-src | ${FREEBSD_REF} | Its macros follow FreeBSD's sys/sys/ioccom.h because they encode an ABI and cannot be written differently and still work.
# --- Mesa and what it drags in ---------------------------------------------------------
mesa          | mesa/build-orbis/**, mesa/include/**        | MIT | MIT-mesa.txt | mesa:licenses/MIT | https://gitlab.freedesktop.org/mesa/mesa | see BUNDLE.txt | RADV plus EGL/GLES and the gallium target, and the headers they were built against. Per-file SPDX headers throughout.
mesa-khronos  | mesa/include/vulkan/**, mesa/include/EGL/**, mesa/include/KHR/**, mesa/include/GLES*/** | Apache-2.0 | Apache-2.0-Khronos.txt | mesa:licenses/Apache-2.0 | https://github.com/KhronosGroup | shipped in the Mesa tree | The registry headers Mesa vendors. ⚠ These are the ones a mismatched copy silently breaks - see the bundle README.
mesa-zlib     | mesa/build-orbis/subprojects/zlib-*/libz.a  | Zlib | Zlib-zlib.txt | url:https://raw.githubusercontent.com/madler/zlib/${ZLIB_TAG}/LICENSE | https://github.com/madler/zlib | ${ZLIB_TAG} | Mesa's meson subproject, pinned by subprojects/zlib.wrap to 1.3.1. Mesa's shader disk cache needs it.
EOF
}

table_rows(){ components | grep -v '^[[:space:]]*#' | grep -v '^[[:space:]]*$'; }

# field N of a row, trimmed
fld(){ awk -F'|' -v n="$2" '{gsub(/^[ \t]+|[ \t]+$/,"",$n); print $n}' <<<"$1"; }

# ===========================================================================================
# fetch
# ===========================================================================================

check_sdk(){ # a stash with no lib/ is NOT an SDK, and saying so by name saves an hour
  [ -f "$SDK/link.x" ] && [ -f "$SDK/lib/libc.a" ] && return 0
  err "OO_PS4_TOOLCHAIN=$SDK is not an unpacked OpenOrbis SDK"
  warn "  wanted link.x AND lib/libc.a there. A source checkout of the toolchain repository"
  warn "  has link.x and no lib/ - that is a stash, not an SDK, and rows sourced sdk: cannot"
  warn "  be gathered from it. Unpack toolchain-llvm-18.tar.gz and point OO_PS4_TOOLCHAIN at it."
  return 1
}

fetch_url(){ # url dest
  local url="$1" dest="$2" tmp code sz
  tmp="$(mktemp)"
  code=$(curl -sL -o "$tmp" -w '%{http_code}' --max-time 60 "$url" 2>/dev/null || echo 000)
  sz=$(_size "$tmp" 2>/dev/null || echo 0)
  # A licence text under 200 bytes is a 404 page or a redirect stub, never a licence.
  if [ "$code" != "200" ] || [ "${sz:-0}" -lt 200 ]; then
    rm -f "$tmp"; warn "FAILED $(basename "$dest") (http=$code size=$sz) <- $url"; return 1
  fi
  mv "$tmp" "$dest"; printf '   %-30s %7s bytes  <- url\n' "$(basename "$dest")" "$sz"
}

fetch_all(){
  local rc=0 need_sdk=0
  mkdir -p "$LICDIR"
  while IFS= read -r row; do
    local lf src
    lf="$(fld "$row" 4)"; src="$(fld "$row" 5)"
    [ "$lf" = "-" ] && continue
    [ -f "$LICDIR/$lf" ] && [ "${FORCE:-0}" != "1" ] && { printf '   %-30s present\n' "$lf"; continue; }
    case "$src" in
      sdk:*)  need_sdk=1 ;;
    esac
  done < <(table_rows)
  if [ "$need_sdk" = 1 ] && ! check_sdk; then rc=1; fi

  while IFS= read -r row; do
    local id lf src p
    id="$(fld "$row" 1)"; lf="$(fld "$row" 4)"; src="$(fld "$row" 5)"
    [ "$lf" = "-" ] && continue
    if [ -f "$LICDIR/$lf" ] && [ "${FORCE:-0}" != "1" ]; then continue; fi
    case "$src" in
      sdk:*)
        p="${src#sdk:}"
        if [ -f "$SDK/$p" ]; then cp "$SDK/$p" "$LICDIR/$lf"
          printf '   %-30s %7s bytes  <- sdk:%s\n' "$lf" "$(_size "$LICDIR/$lf")" "$p"
        else warn "MISSING $lf: no $SDK/$p ($id)"; rc=1; fi ;;
      repo:*)
        p="${src#repo:}"
        if [ -f "$REPO/$p" ]; then cp "$REPO/$p" "$LICDIR/$lf"
          printf '   %-30s %7s bytes  <- repo:%s\n' "$lf" "$(_size "$LICDIR/$lf")" "$p"
        else warn "MISSING $lf: no $REPO/$p ($id)"; rc=1; fi ;;
      mesa:*)
        p="${src#mesa:}"
        if [ -f "$MESA_SRC/$p" ]; then cp "$MESA_SRC/$p" "$LICDIR/$lf"
          printf '   %-30s %7s bytes  <- mesa:%s\n' "$lf" "$(_size "$LICDIR/$lf")" "$p"
        else
          warn "no Mesa checkout at $MESA_SRC - falling back to $MESA_REF over the network for $lf"
          fetch_url "https://gitlab.freedesktop.org/mesa/mesa/-/raw/${MESA_REF}/$p" "$LICDIR/$lf" || rc=1
        fi ;;
      url:*)  fetch_url "${src#url:}" "$LICDIR/$lf" || rc=1 ;;
      *)      warn "row $id has licfile $lf and source '$src' - cannot gather it"; rc=1 ;;
    esac
  done < <(table_rows)
  [ "$rc" -eq 0 ] || warn "some licence texts were not gathered - licenses/ is INCOMPLETE"
  return $rc
}

# ===========================================================================================
# sums / notice / verify
# ===========================================================================================

licfiles(){ table_rows | awk -F'|' '{gsub(/^[ \t]+|[ \t]+$/,"",$4); if ($4 != "-") print $4}' | LC_ALL=C sort -u; }

write_sums(){
  log "writing licenses/SHA256SUMS"
  local f missing=0
  while read -r f; do [ -f "$LICDIR/$f" ] || { warn "licence text MISSING, not recorded: $f"; missing=1; }; done < <(licfiles)
  ( cd "$LICDIR" && while read -r f; do [ -f "$f" ] && _sha256 "$f"; done < <(licfiles) ) > "$LICDIR/SHA256SUMS"
  printf '   %s entries\n' "$(grep -c . "$LICDIR/SHA256SUMS" || true)"
  [ "$missing" -eq 0 ] || { err "licenses/ is incomplete - run 'fetch' before committing sums"; return 1; }
}

gen_notice(){
  local today; today="$(date -u '+%Y-%m-%d')"
  cat <<'HEAD'
# NOTICE — everything the orbis-sdk bundle redistributes, and under what terms

**Generated by `scripts/release/sdk-licenses.sh notice` — do not edit by hand.**
`scripts/release/sdk-licenses.sh verify` fails when this file is stale, so it cannot drift
from the table that produced it.

This file exists because of one change of role. Until the first bundle, this repository only
**consumed** the OpenOrbis PS4 Toolchain: the consumer fetched that tarball themselves, and
whether it carried the notices its own components require was upstream's business. A bundle
**redistributes**, and MIT's *"The above copyright notice … shall be included in all copies
or substantial portions of the Software"* and Apache-2.0 §4(a)/(d) then bind whoever ships
the copy. That is us.

⚠ **The SDK tarball does not discharge them.** Measured over `toolchain-llvm-18.tar.gz`
(release v0.5.4) unpacked: the entire tree contains **eight** files whose name says licence —
the root `LICENSE` (GPL-3.0), three byte-identical `bin/*/LICENSE.txt` (LGPL-3.0),
`include/stb/LICENSE`, `include/stb/data/herringbone/license.txt`,
`include/stb/tests/pngsuite/PngSuite.LICENSE`, and `include/SDL2/SDL_copying.h`. There is no
musl `COPYRIGHT`, no LLVM `LICENSE.TXT` and no `FTL.TXT` anywhere in it. Upstream's own
README says it knows: *"The accompanying LLVM binaries are licensed under the Apache 2.0
license and is owned by LLVM. Under that license, redistribution is allowed."* — true, and
the same licence also requires the copy to carry the licence. It does not.

`licenses/` in this repository, and `licenses/` in every bundle cut from it, is that gap
closed. Every file named below is the real upstream text, gathered at the pin recorded
beside it and checksummed in `licenses/SHA256SUMS`.

**This notice is not a licence.** Each component keeps the terms it arrived under. Taking a
file out of a bundle means complying with *that file's* licence, not with this repository's
`LICENSE`.

HEAD
  printf '## The bundle, component by component\n\n'
  printf '| Component | In the bundle | Terms | Licence text shipped | Upstream | Pinned at |\n'
  printf '|---|---|---|---|---|---|\n'
  local row id paths spdx lf origin pin
  while IFS= read -r row; do
    id="$(fld "$row" 1)"; paths="$(fld "$row" 2)"; spdx="$(fld "$row" 3)"
    lf="$(fld "$row" 4)"; origin="$(fld "$row" 6)"; pin="$(fld "$row" 7)"
    [ "$lf" = "-" ] && lf="— (none exists; see below)" || lf="\`licenses/$lf\`"
    printf '| `%s` | %s | **%s** | %s | %s | `%s` |\n' \
      "$id" "$(sed 's/, /`, `/g; s/^/`/; s/$/`/' <<<"$paths")" "$spdx" "$lf" "$origin" "$pin"
  done < <(table_rows)

  printf '\n## Why each row is what it is\n\n'
  while IFS= read -r row; do
    id="$(fld "$row" 1)"
    printf '* **`%s`** — %s\n' "$id" "$(fld "$row" 8)"
  done < <(table_rows)

  cat <<'TAIL'

## The three things this notice does NOT establish

1. ⚠ **`sdk/lib/*.so` has no licence, and no amount of care produces one.** The ~422 import
   libraries are empty by construction — `libkernel.so` carries `.dynsym` 0x5c10 for roughly
   945 symbols against `.text` 0x2654, so no symbol has a body. They were generated by
   `OpenOrbis/orbis-lib-gen` from `OpenOrbis/ps4libdoc`, and **both of those repositories are
   archived with no `LICENSE` file at all**. There is no grant to quote and therefore no
   notice to ship. A bundle containing them is shipping files whose redistribution terms
   nobody has stated. That is recorded, not resolved.
2. ⚠ **`create-fself`, `create-gp4` and `readoelf` ship without corresponding source.** They
   are GPL-3.0-only Go binaries from OpenOrbis' own repositories — `strings` finds
   `github.com/OpenOrbis/create-fself/pkg/fself` inside the first — and the tarball carries
   no source for any of them. GPL-3.0 §6 wants the source with the object or a written offer
   in its place. `NOTICE.md` records the upstream repository; that is a pointer, not an offer.
   (The crt objects are **not** in this category: six of the seven are musl's and MIT, and the
   one that is GPL-3.0, `crtlib.o`, has its source shipped beside it at `sdk/src/crt/crtlib.c`.
   `LICENSING.md` §2.9 has the measurement that separates them.)
3. ⚠ **FreeType's dual choice is undocumented and this notice does not invent one.**
   FreeType is FTL **or** GPL-2.0 at the recipient's option. 86 of the 88 shipped headers
   point at `LICENSE.TXT` — the FTL — and no file in the tarball mentions GPL-2.0. `FTL.TXT`
   is shipped because it is the licence the shipped files actually name. Nothing here says
   OpenOrbis made a choice, because nothing in their tree says they did.

## Regenerating

```sh
scripts/release/sdk-licenses.sh fetch     # gather the texts at their pins (network)
scripts/release/sdk-licenses.sh sums      # rewrite licenses/SHA256SUMS
scripts/release/sdk-licenses.sh notice    # rewrite this file
scripts/release/sdk-licenses.sh verify    # offline: the gate CI runs
```
TAIL
  printf '\nGenerated %s.\n' "$today"
}

verify_offline(){
  local rc=0
  log "licence texts vs licenses/SHA256SUMS (offline)"
  if [ ! -f "$LICDIR/SHA256SUMS" ]; then
    err "no licenses/SHA256SUMS - run 'fetch' then 'sums'"; return 1
  fi
  if ( cd "$LICDIR" && _sha256c < SHA256SUMS ); then
    printf '   %s licence texts OK\n' "$(grep -c . "$LICDIR/SHA256SUMS" || true)"
  else
    err "LOCAL DRIFT: a licence text does not match its recorded sha256"; rc=1
  fi

  log "every component that needs a text has one"
  local row id lf n=0
  while IFS= read -r row; do
    id="$(fld "$row" 1)"; lf="$(fld "$row" 4)"
    if [ "$lf" = "-" ]; then
      # ⚠ TWO DIFFERENT SITUATIONS SHARE THIS COLUMN and conflating them would be the whole
      # point missed: `inline` means the complete text is inside the shipped files, so nothing
      # separate is needed; anything else means no grant exists to quote. The note says which.
      printf '   %-14s no separate text: %s\n' "$id" "$(fld "$row" 8)"
      continue
    fi
    if [ ! -f "$LICDIR/$lf" ]; then
      err "MISSING licence text for component '$id': licenses/$lf"; rc=1
    elif [ "$(_size "$LICDIR/$lf")" -lt 200 ]; then
      err "licenses/$lf is $( _size "$LICDIR/$lf") bytes - that is not a licence text ($id)"; rc=1
    else n=$((n+1)); fi
  done < <(table_rows)
  printf '   %s components have their text present\n' "$n"

  # An UNRECORDED file in licenses/ is drift too: it means something was put there by hand.
  local extra
  extra="$(comm -23 <(cd "$LICDIR" && find . -maxdepth 1 -type f ! -name SHA256SUMS | sed 's|^\./||' | LC_ALL=C sort) \
                    <(licfiles) || true)"
  if [ -n "$extra" ]; then
    err "LOCAL DRIFT: file(s) in licenses/ that no component row accounts for:"
    printf '     %s\n' $extra; rc=1
  fi

  log "NOTICE.md is generated - checking it is current"
  # The trailing "Generated <date>." line is the only part that legitimately moves, so it is
  # excluded from the comparison; everything above it is table-derived and must not drift.
  if [ -f "$NOTICE" ] && diff -q <(gen_notice | sed '$d;/^Generated /d') <(sed '$d;/^Generated /d' "$NOTICE") >/dev/null; then
    printf '   NOTICE.md up to date\n'
  else
    err "NOTICE.md is stale or missing - run 'sdk-licenses.sh notice'"; rc=1
  fi
  return $rc
}

verify_upstream(){ # 0 ok, 3 moved, 4 unreachable
  local moved=0 unreachable=0 ok=0 tmp row lf src url up loc
  tmp="$(mktemp)"
  log "re-downloading the url: rows at their pins and diffing"
  while IFS= read -r row; do
    lf="$(fld "$row" 4)"; src="$(fld "$row" 5)"
    case "$src" in url:*) url="${src#url:}" ;; *) continue ;; esac
    [ -f "$LICDIR/$lf" ] || continue
    if ! curl -sfL -o "$tmp" --max-time 60 "$url" 2>/dev/null; then
      warn "UNREACHABLE $lf <- $url"; unreachable=$((unreachable+1)); continue
    fi
    up="$(_sha256 "$tmp" | cut -d' ' -f1)"
    loc="$(_sha256 "$LICDIR/$lf" | cut -d' ' -f1)"
    if [ "$up" = "$loc" ]; then ok=$((ok+1)); printf '   %-30s identical to upstream\n' "$lf"
    else
      err "UPSTREAM MOVED  $lf"
      printf '     committed  %s\n     upstream   %s\n     %s\n' "$loc" "$up" "$url"
      moved=$((moved+1))
    fi
  done < <(table_rows)
  rm -f "$tmp"
  printf '   %s texts identical to upstream at the pin\n' "$ok"
  [ "$moved" -gt 0 ] && return 3
  [ "$unreachable" -gt 0 ] && return 4
  return 0
}

verify(){
  local upstream=0
  case "${1:-}" in
    --upstream) upstream=1 ;;
    "") ;;
    *) echo "usage: $0 verify [--upstream]" >&2; exit 2 ;;
  esac
  local rc=0
  verify_offline || rc=1
  if [ "$upstream" -eq 1 ]; then
    if [ "$rc" -ne 0 ]; then
      err "LOCAL DRIFT found - fix that first; an upstream diff on drifted copies means nothing"
      return 1
    fi
    verify_upstream || return $?
  fi
  [ "$rc" -eq 0 ] && log "OK"
  return $rc
}

case "${1:-verify}" in
  fetch)  shift; while [ $# -gt 0 ]; do case "$1" in --sdk) SDK="$2"; shift 2;; --force) FORCE=1; shift;; *) echo "usage: $0 fetch [--sdk DIR] [--force]" >&2; exit 2;; esac; done
          log "gathering licence texts into licenses/"; fetch_all && write_sums ;;
  sums)   write_sums ;;
  notice) gen_notice > "$NOTICE"; log "NOTICE.md regenerated" ;;
  table)  table_rows ;;
  verify) shift; verify "${1:-}" ;;
  *) echo "usage: $0 [fetch|sums|notice|table|verify [--upstream]]" >&2; exit 2 ;;
esac
