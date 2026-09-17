# LICENSING — what the SDK bundle redistributes, and what is still not known about it

`NOTICE.md` is the **generated** answer: a table of components, terms and pinned upstreams
that `scripts/release/sdk-licenses.sh verify` refuses to let go stale. This file is the
**argument behind it** — what was measured, on what, and where the measurement ran out. It is
maintained by hand, the way `~/src/unemu-org/oracles/MANIFEST.md` is; `NOTICE.md` is generated
from the table in `sdk-licenses.sh`, the way that repository's `NOTICE.md` is.

**Read §5 before treating any of this as settled.** One component has no licence at all, one
ships without the source its licence asks for, and two have pins that are near enough rather
than exact. A fourth entry there is a mistake this ledger made and corrected; it is kept.

---

## 1. Why this file exists now and did not before

This repository **consumed** the OpenOrbis PS4 Toolchain. A consumer fetched
`toolchain-llvm-18.tar.gz` themselves; whether that tarball carries the notices its own
components require was upstream's business, and saying so was not evasion — it was accurate.

A bundle **redistributes**. MIT's *"The above copyright notice and this permission notice
shall be included in all copies or substantial portions of the Software"* and Apache-2.0
§4(a)/(d) bind whoever ships the copy, and from the first bundle that is this project.

⚠ **The SDK tarball does not discharge them, and this was counted rather than assumed.** The
whole unpacked tree contains **eight** files whose name says licence:

```
LICENSE                                    GPL-3.0, 674 lines   sha256 3972dc97…
bin/linux/LICENSE.txt                      LGPL-3.0, 7650 B     sha256 ea8af5e7…
bin/macos/LICENSE.txt                      byte-identical to the above
bin/windows/LICENSE.txt                    byte-identical to the above
include/stb/LICENSE                        stb's MIT/Unlicense dual text
include/stb/data/herringbone/license.txt   test-data notice
include/stb/tests/pngsuite/PngSuite.LICENSE
include/SDL2/SDL_copying.h                 SDL's Zlib text, as a header
```

There is **no musl `COPYRIGHT`**, **no LLVM `LICENSE.TXT`** and **no `FTL.TXT`** anywhere in
the tree. Upstream's own `README.md:163` says it knows: *"The accompanying LLVM binaries are
licensed under the Apache 2.0 license and is owned by LLVM. Under that license, redistribution
is allowed."* That is true, and the same licence also requires the copy to carry the licence.
It does not.

`licenses/` is that gap closed: 17 texts, each fetched from the upstream the binaries came
from, each pinned and checksummed in `licenses/SHA256SUMS`.

---

## 2. How each component was identified

Nothing below is taken from a README or a filename. Every row is a measurement over
`toolchain-llvm-18.tar.gz` (release **v0.5.4**) unpacked at
`~/src/unemu-org/unemups4/data/oo_sdk`.

### 2.1 ⚠ The asset is called `llvm-18` and its C++ runtime is **LLVM 11**

```
include/c++/v1/__config:75            #define _LIBCPP_VERSION 11000
strings lib/libc++.a                  clang version 11.0.0          (and nothing else)
strings lib/libc++abi.a               clang version 11.0.0
strings lib/libclang_rt.builtins…a    clang version 11.0.0
strings lib/libc.a                    clang version 11.0.0  AND  clang version 18.1.4
```

`libc.a` is the only archive with members from both compilers: musl was partially rebuilt
with the newer toolchain, the C++ runtime was not. The file dates agree — the `libc++*`
archives are 2021-12-20, `libc.a` is 2025-02-18.

Consequence for licensing: the LLVM texts are pinned at **`llvmorg-11.0.0`**, because that is
the release those binaries came from. Which compiler built an object does not change its
licence; which release it came from does, because each project's `LICENSE.TXT` carries a
per-release third-party appendix.

⚠ **The four LLVM texts are NOT interchangeable** — measured, at `llvmorg-11.0.0`:

```
libcxx/LICENSE.TXT        539dd7aed86e8a4f12cbdd0e6c50c189c7d74847e4fecc64ce2c6ee3a01da38b
libcxxabi/LICENSE.TXT     e2b35be49f7284a45b7baca8fc7b3ab7440e7902392b2528a457816b5bb2a15c
libunwind/LICENSE.TXT     b5efebcaca80879234098e52d1725e6d9eb8fb96a19fce625d39184b705f7b6d
compiler-rt/LICENSE.TXT   1a8f1058753f1ba890de984e48f0242a3a5c29a6a8f2ed9fd813f36985387e8d
```

Four different files. Shipping libc++'s text for compiler-rt's binary would be the wrong
notice, quietly. All four ship.

### 2.2 `include/orbis/` carries ZERO notices, and that was checked, not assumed

189 files (163 at the top level plus 26 under `_types/`). `grep -rniE
'copyright|licen[cs]e|SPDX|permission is hereby'` over the whole directory returns **eleven**
lines, and every one of them is an API name:

```
PlayReady.h:44,46,48,50,52   scePlayReadyLicenseAcq*
Pad.h:59                     scePadGetLicenseControllerInformation
_types/bgft.h:21             ORBIS_BGFT_TASK_SUB_TYPE_GAME_LICENSE
LibcInternal.h:1100,1102     void _PJP_C_Copyright();  void _PJP_CPP_Copyright();
libc.h:706,708               the same two
```

`_PJP_C_Copyright` is a **Sony symbol this header declares**, not a statement about this
header. The root `LICENSE` is the only thing that licenses these 189 files, and it therefore
has to travel with them.

### 2.3 musl's headers carry nothing either

93 headers at `include/*.h` plus `bits/ sys/ arpa/ net/ netinet/ netpacket/ scsi/`. `grep -l`
for copyright/licence/SPDX across all of them matches **one** file, and that file is
`ft2build.h`, which is FreeType's. `lib/libc.a` has 1483 `.lo` members and no notice anywhere.

The pin is **OpenOrbis' fork** (`OpenOrbis/musl`), not upstream musl, because that is the tree
these bytes came from. ⚠ It is also the only reachable one: `git.musl-libc.org` answered
`http 000` from this network, and `raw.githubusercontent.com/bminor/musl/master/COPYRIGHT` is
a 404 — a pin nobody can check is not a pin.

### 2.4 The import stubs are empty, and that is measurable

```
lib/*.so                      422 files
objdump -h lib/libkernel.so   .dynsym 0x5c10   .text 0x2654
```

`.dynsym` at 0x5c10 is 23568 bytes, 982 24-byte entries; `objdump -T` lists **981** dynamic
symbols (the first entry is the null one). `.text` at 0x2654 is 9812 bytes for all 981 of
them - ten bytes each. No symbol has a body. They are link-time import libraries
generated by `OpenOrbis/orbis-lib-gen` from `OpenOrbis/ps4libdoc`. See §5.1.

### 2.5 The packaging tools split two ways, and `bin/LICENSE.txt` covers only one of them

`strings bin/linux/create-fself` contains `github.com/OpenOrbis/create-fself/pkg/fself` and
`…/pkg/oelf` — it is a Go binary from OpenOrbis' own repository, hence GPL-3.0-only.
`bin/README.md:8` is LibOrbisPkg's: *"All code in this repository is licensed under the GNU
LGPL version 3, which can be found in LICENSE.txt."*

⚠ **So `bin/LICENSE.txt` is LibOrbisPkg's licence sitting beside three binaries it does not
cover.** `create-fself`, `create-gp4` and `readoelf` are GPL-3.0 and the LGPL text next to
them is not their notice. `NOTICE.md` separates the two rows for that reason.

### 2.6 ⚠ `include/SDL2` is not one project, it is four

Counted per file over all 87 headers:

```
Copyright (C) 1997-2019     78 files    SDL proper
Copyright (C) 1997-2020      1 file     SDL_image.h        SDL_IMAGE_PATCHLEVEL 6
no "Copyright (C) 1997-" line 8 files
  SDL_ttf.h                             SDL_TTF_PATCHLEVEL 15, "(C) 2001-2020 Sam Lantinga", Zlib
  SDL_vulkan.h                          "(C) 2017, Mark Callow", Zlib
  SDL_opengl_glext.h                    "(c) 2013-2014 The Khronos Group Inc.", MIT
  SDL_opengles2_gl2.h, _gl2ext.h,
  _gl2platform.h, _khrplatform.h        "(c) 2008-2009 The Khronos Group Inc.", MIT
  SDL_revision.h                        one generated line, no notice at all
include/SDL2/SDL_version.h              MAJOR 2  MINOR 0  PATCHLEVEL 9
```

So `NOTICE.md` carries four rows for this one directory, not one: `sdl2`, `sdl2-image`,
`sdl2-ttf` and `sdl2-khronos`. The seven Khronos/Callow headers carry their **complete**
licence text inline, so nothing separate has to ship for them — that row's licence-text column
is a dash meaning *inline*, which is a different thing from `sdk-stubs`' dash meaning *no
grant exists* (§5.1). `verify` prints the distinction rather than collapsing it.

⚠ **`SDL_ttf.h` ships with no library behind it.** `lib/` has `libSDL2.a`,
`libSDL2_image.a` and `libSDL2main.a` and no `libSDL2_ttf.a`. Code that includes it compiles
and does not link. That is a defect of the SDK, recorded here because a bundle inherits it.

⚠ **SDL proper's version macro and its copyright year disagree.** `SDL_version.h` declares
2.0.9, whose own upstream `COPYING.txt` says "1997-2018", while 78 of the headers say
"1997-2019" — which is 2.0.10's year. The pin follows the version macro, because that is the
only statement the tree makes about itself, and the inline notice in each header is the
authoritative copyright line either way: the Zlib text itself does not change across those
releases.

⚠ And `libsdl-org/SDL_image` has **no reachable `release-2.0.6` tag** (404 on the raw path;
the tag listing stops at 2.0.5), so `Zlib-SDL_image.txt` is taken from `release-2.0.5`: the
nearest tagged release that states the terms, **not** the exact upstream of the shipped
header.

### 2.7 FreeType's dual choice is undocumented and is not invented here

88 files under `include/freetype`, `FREETYPE_MAJOR 2` / `FREETYPE_MINOR 5`, per-file headers
"Copyright 1996-2013". 86 of the 88 name `LICENSE.TXT` — the FTL — in their boilerplate; the
two that do not are `ftchapters.h` and `config/ftmodule.h`, neither of which carries code.
⚠ **`grep -n GPL` across all 88 returns nothing.** No file in the tarball mentions GPL-2.0,
and `FTL.TXT` itself is absent.

FreeType is FTL **or** GPL-2.0 at the recipient's option. `FTL.TXT` ships because it is the
licence the shipped files actually point at. Nothing here asserts that OpenOrbis made a
choice, because nothing in their tree says they did.

### 2.8 `cmake/orbis-tls.ld` is GPL-3.0-only, and it is on every link line

This is the correction another change landed in this repository on 2026-09-17; it is recorded
here because a bundle makes it a redistribution question rather than only an in-tree one.

`cmake/orbis-tls.ld` is the SDK's own `link.x` with two match patterns added to the `.tls`
rule (`*(.tdata)` → `*(.tdata .tdata.*)`, `*(.tbss)` → `*(.tbss .tbss.*)`). The SDK is
GPL-3.0-only with no linking exception, so a derivative of it cannot be relicensed by the
deriver. The file carried `SPDX-License-Identifier: MIT` until that date and that header was
wrong. It now reads:

```
Copyright (C) the OpenOrbis contributors; modifications © 2026 Mikołaj Mikołajczyk
SPDX-License-Identifier: GPL-3.0-only
```

⚠ **`-only`, not `-or-later`, and that was checked.** `grep -n 'any later version'` over the
SDK's `LICENSE` returns three lines — 572, 574 and 640 — and all three are the GPLv3's own
boilerplate: §14 ("Public License \"or any later version\" applies to it…") and the
"How to Apply These Terms" appendix. None of them is OpenOrbis granting the option. The
repository's `README.md:161` says only "This project is licensed under the GPLv3 license".

`LICENSE` at the repository root names it, and `include/sys/ioccom.h` (BSD-3-Clause), as the
only two exceptions to this repository's MIT. Those two are the whole exception list.

⚠ **The file emits bytes into the output, not only into the build.** The four `QUAD()` values
at the top of `.text` are the ASCII of `/libexec/ld-elf.so.1` and land in every `eboot.bin`
linked through it. That is recorded as a fact a reader needs — **not** as a conclusion about
what a built binary's licence then is. Nobody here has established that, and neither this
file nor `NOTICE.md` pretends to.

### 2.9 ⚠ Six of the seven crt objects are MIT, not GPL — and the objects say so themselves

This is the one row of the original audit that was wrong, and correcting it needed no guesswork:
each object records the source it was compiled from in its `STT_FILE` entry, and the symbols it
exports corroborate it.

```
objdump -t <sdk>/lib/<obj>          STT_FILE     global symbols
  crt1.o                            crt1.c       _start _start_ps4_c __ps4Argv
  crt_dyn.o                         crt_dyn.c    _start _start_ps4_c __ps4Argv
  rcrt1.o                           rcrt1.c      _start _start_c __dls2
  Scrt1.o                           Scrt1.c      _start _start_c
  crti.o                            (none)       _init _fini            assembled from .s
  crtn.o                            (none)       —                      the same
  crtlib.o                          crtlib.c     module_start module_stop _init _fini
                                                 __init_array_start __init_array_end
```

Three corroborating facts, each re-checkable:

1. **`OpenOrbis/musl` (MIT) has `crt/ps4/` containing exactly `crt1.c crt_dyn.c crti.s
   crtn.s`**, and `crt/` at its root containing `rcrt1.c` and `Scrt1.c`. Its
   `crt/ps4/crt1.c` defines `__ps4Argv` and `_start_ps4_c` — the same two symbols the shipped
   `crt1.o` exports.
2. **The toolchain repository's `src/crt/` contains ONLY `build.bat build.sh crtlib.c
   crtlib.S`.** There is no `crt1.c` and no `crt_dyn.c` there.
3. **`src/README.md` says it outright:** *"Games/apps CRT is handled by musl already, however
   we have a crtlib stub here for PRXs to use, `crtlib.o`."*

So `crt1.o crti.o crtn.o crt_dyn.o rcrt1.o Scrt1.o` are **musl's, MIT**, and `crtlib.o` alone
is **GPL-3.0-only**. `NOTICE.md` has two rows, `sdk-crt-musl` and `sdk-crt-lib`.

⚠ **This makes the missing-notice finding broader, not narrower.** musl's absent `COPYRIGHT`
(§2.3) now covers six startup objects as well as `libc.a` and 93 headers.

⚠ **`crtlib.o` is the only GPL-3.0 object that can reach a consumer's output, and most
consumers never touch it.** `cmake/ps4-openorbis.cmake` puts exactly one crt object on an
executable's link line — `crt1.o` — and its own comment gives four separate reasons why none of
the other four is there. An `eboot.bin` therefore carries **no GPL-3.0 crt**. `crtlib.o` is a
module entry point and belongs on a `.prx`/`.sprx` link line.

**And GPL-3.0 §6 is satisfied for it**, which is the other half of the correction: its
corresponding source ships in the same tarball at `src/crt/crtlib.c`. The bundle keeps
`sdk/src/crt/` for exactly that reason, and would be non-compliant without it.

### 2.9b The `compat-crt` row, and why there are two crt rows and not one

This repository now carries its own MIT startup objects under `crt/` — `orbis_crt1.c`,
`orbis_crti.S`, `orbis_crtlib.c`, `orbis_crtn.S`, `orbis_sce_params.h` — and
`cmake/ps4-openorbis.cmake` selects between the two sets:

```cmake
ORBIS_CRT=sdk    (the default)   ${OO_PS4_TOOLCHAIN}/lib/crt1.o
ORBIS_CRT=own                    ${ORBIS_COMPAT_DIR}/build/crt/crt1.o
```

⚠ **A bundle carries both sets, and the manifest describes both.** Which one a given binary was
linked with is a property of **that build**, not of the bundle, and nothing in the ledger can
tell you after the fact — read that build's own `link.txt`. `sdk-crt-musl` and `sdk-crt-lib`
cover the SDK's set; `compat-crt` covers this repository's. A bundle cut before `crt/` existed
simply has no files matching that row's paths, which is why the row does not assert they are
there.

⚠ **`ORBIS_CRT=own` does not remove the GPL question, it narrows it.** The default is still
`sdk`, and even under `own` the substitution is `crt1.o` only — `crtlib.o` for a `.prx` link
remains the SDK's GPL-3.0 object unless something replaces that too.

---

## 3. The bundle's own two-way licence split

Two licences are in play and they do not overlap.

* Files written **for** this repository — everything under `scripts/`, `src/`, `optional/`,
  `vkloader/`, `test/`, `cmake/*.cmake`, `include/` except `sys/ioccom.h`, and this file — are
  **MIT**, © 2026 Mikołaj Mikołajczyk (`LICENSE`).
* **That licence does not extend to anything the bundle redistributes.** Every third-party
  file keeps the terms it arrived under, with its headers intact. Taking a file out of a
  bundle means complying with *that file's* licence, not with `LICENSE`.

The two in-repository exceptions are `cmake/orbis-tls.ld` (GPL-3.0-only, §2.8) and
`include/sys/ioccom.h` (BSD-3-Clause — its macros follow FreeBSD's because they encode an ABI
and cannot be written differently and still work).

---

## 4. Where the texts come from

`licenses/` is populated by `scripts/release/sdk-licenses.sh fetch`, which takes each text
from one of four places and records the sha256 of all of them in `licenses/SHA256SUMS`:

| how | which texts |
|---|---|
| `sdk:` — copied out of the unpacked SDK | `GPL-3.0.txt` (root `LICENSE`), `LGPL-3.0.txt` (`bin/linux/LICENSE.txt`), `MIT-stb.txt` |
| `repo:` — copied out of this repository | `MIT-orbis-compat.txt` |
| `mesa:` — copied out of the Mesa checkout | `MIT-mesa.txt`, `Apache-2.0-Khronos.txt` |
| `url:` — downloaded at the pin | the LLVM four, musl, SDL, SDL_image, SDL_ttf, FreeType, FreeBSD, zlib |

⚠ **`MIT-mesa.txt` and `Apache-2.0-Khronos.txt` are SPDX templates with `<year> <copyright
holders>` placeholders**, because that is the form Mesa ships them in (`licenses/MIT`,
`licenses/Apache-2.0`, kernel-style with a `License-Text:` section). Mesa's **per-file SPDX
headers** are the authoritative copyright notices and they travel inside the archives'
sources; these two files are the licence text those headers refer to. That is the same
arrangement Mesa itself distributes under.

`sdk-licenses.sh verify --upstream` re-downloads the `url:` rows and diffs them, so nobody has
to take this repository's word that the committed texts are upstream's texts. Exit 3 means
UPSTREAM MOVED — the pin is supposed to be immutable, so that is itself a finding.

---

## 5. ⚠ Open obligations and unresolvable provenance

These four are the reason this file is worth reading. None of them is fixed by the machinery;
all four are recorded so that a reader knows where the ledger stops.

### 5.1 `sdk/lib/*.so` — no licence exists to comply with

422 generated import libraries, empty by construction (§2.4). Their generator
(`OpenOrbis/orbis-lib-gen`) and their input data (`OpenOrbis/ps4libdoc`) are **both archived
with no `LICENSE` file of any kind**. There is no grant to quote, so there is no notice to
ship, so there is nothing `verify` can check. A bundle containing them is shipping files whose
redistribution terms nobody has stated.

**This is the single strongest argument for not publishing a bundle at all**, and it is put
here rather than buried. Three things would change it, in decreasing order of realism: ask
OpenOrbis to put a licence on `ps4libdoc`; regenerate the stubs from a source whose terms are
known; or ship the bundle without `lib/*.so` and have the consumer fetch the SDK for them —
which is most of what the bundle exists to avoid. **Nobody has been asked.**

### 5.2 ~~Six of the seven GPL-3.0 crt objects ship without corresponding source~~ — CLOSED

**This entry was wrong and is kept rather than deleted, because a ledger that quietly removes
its own mistakes is worth less than one that records them.** It claimed all seven crt objects
were GPL-3.0 and that six of them shipped sourceless. §2.9 has the measurement that refutes it:
six of the seven are musl's and **MIT**, and the one that is GPL-3.0 — `crtlib.o` — has its
corresponding source shipped beside it at `src/crt/crtlib.c`. GPL-3.0 §6 is satisfied.

What survives from it is one instruction: ⚠ **the bundle must keep `sdk/src/crt/`.** Pruning it
as "just build scripts" would turn a compliant redistribution into a non-compliant one, and it
is 4 files. `make-sdk-bundle.sh` keeps it and says why at the point where it does.

### 5.3 `create-fself`, `create-gp4`, `readoelf` — right licence, wrong neighbour

GPL-3.0-only (§2.5), shipping in a directory whose only `LICENSE.txt` is the LGPL — which is
LibOrbisPkg's licence, not theirs. Half-fixed for the bundle: they get their own `NOTICE.md`
row pointing at `licenses/GPL-3.0.txt`, so the right text now travels with them.

⚠ **What is NOT fixed is the source.** They are compiled Go binaries and the tarball carries no
source for any of the three. GPL-3.0 §6 wants the source with the object or a written offer
valid for three years in its place. `NOTICE.md` records the upstream repository
(`OpenOrbis/create-fself`), and **that is a pointer, not an offer.** A project that
redistributes these binaries should either satisfy itself that the upstream repository holds
the corresponding source at the revision it ships, or state the offer properly. Neither has
been done. **This is now the only open GPL §6 obligation in the bundle** — §5.2 having turned
out not to be one.

### 5.4 The exact upstream of two SDL components is not established

`SDL_image.h` declares 2.0.6 and upstream has no reachable tag of that name (§2.6); its
licence text is taken from `release-2.0.5`. SDL proper's headers' copyright year disagrees
with the release its own version macro declares. Neither affects **which** licence applies —
Zlib, in both cases, and the text is identical across those releases — but a pin that is
"near enough" is recorded as near enough rather than as exact.

Related, and smaller: `SDL_ttf.h` ships with no `libSDL2_ttf.a` behind it, so that header is
a compile-time promise the SDK cannot keep. It is in the ledger because it is redistributed,
not because anything can link against it.

---

## 6. Running it

```sh
scripts/release/sdk-licenses.sh fetch     # gather the texts at their pins (needs network)
scripts/release/sdk-licenses.sh sums      # rewrite licenses/SHA256SUMS
scripts/release/sdk-licenses.sh notice    # regenerate NOTICE.md from the table
scripts/release/sdk-licenses.sh verify    # OFFLINE: the gate CI runs
scripts/release/sdk-licenses.sh verify --upstream   # re-download and diff. Exit 3 = a pin moved.
```

`fetch` needs `OO_PS4_TOOLCHAIN` pointing at an **unpacked SDK**. ⚠ It is checked by content —
`link.x` **and** `lib/libc.a` — because a source checkout of the toolchain repository has
`link.x` and no `lib/` at all, and on at least one developer machine that is exactly what
`OO_PS4_TOOLCHAIN` points at.

The exit codes are the five `~/src/unemu-org/oracles/fetch-oracles.sh` uses, meaning the same
five things: `0` ok, `1` LOCAL DRIFT, `2` usage, `3` UPSTREAM MOVED, `4` INCOMPLETE.
