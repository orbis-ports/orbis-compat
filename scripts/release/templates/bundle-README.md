# orbis-sdk

Everything needed to build your own software for the PlayStation 4, in one directory, pinned
together. Unpack it and point CMake at one file.

```sh
tar -xzf orbis-sdk-<v>.tar.gz
cmake -S your-project -B build \
      -DCMAKE_TOOLCHAIN_FILE=$PWD/orbis-sdk-<v>/toolchain/orbis-sdk.cmake
cmake --build build
```

No environment variables. The bundle is relocatable: move it, rename its parent, the same
command works — `toolchain/orbis-sdk.cmake` resolves everything from its own location.

For meson, make, or the packaging scripts, `. ./env.sh` exports the four variables every
build script in `orbis-ports` reads.

⚠ **Two worked examples, and they answer different questions.** `orbis-compat/scripts/release/hello/`
proves the LAYOUT - corrected headers, the overlay under `--whole-archive`, the linker script,
`crt1.o`, `create-fself` - and touches no Mesa, so nothing else can fail and be mistaken for it.
`orbis-compat/scripts/release/triangle/` is the graphics workflow: RADV, a headless surface, a
swapchain, a pipeline compiled from SPIR-V, and a frame on the television. It needs
`glslangValidator` on the host. Both were run on a console on 2026-09-17.

`scripts/release/hello/` inside `orbis-compat/` is a complete worked example: a `CMakeLists.txt`
and one `.c` file. Copy it and start from there.

---

## What is in here

```
sdk/                  the OpenOrbis PS4 Toolchain v0.5.4: link.x, the crt objects, ~422
                      import stubs, musl's libc.a and headers, LLVM's libc++/libc++abi/
                      libunwind/compiler-rt, SDL2, FreeType, stb, and the packaging tools
orbis-compat/         the platform overlay, and build/liborbis-compat.a already built
mesa/                 RADV, EGL and GLES as static archives, and the headers they were
                      built against. manifest.txt names the commits behind them
toolchain/
  orbis-sdk.cmake     THE file. It resolves four paths and includes the real toolchain file
  orbis-tls.ld        the corrected linker script. ⚠ GPL-3.0-only, see NOTICE.md
  orbis.ini.in        the meson cross file; env.sh materialises it for this location
env.sh                the four variables, for non-CMake consumers
verify.sh             the offline gate. Run it on what you downloaded
BUNDLE.txt            every input, its sha256 and its parent commit
SHA256SUMS            every file in here
NOTICE.md             what this bundle redistributes and under what terms. GENERATED
LICENSING.md          the argument behind NOTICE.md, and what is still not known
licenses/             the actual licence texts
```

## Check it before you trust it

```sh
./orbis-sdk-<v>/verify.sh ./orbis-sdk-<v>
```

Offline. It checks every file against `SHA256SUMS`, that no file is present that `SHA256SUMS`
does not account for, that every component's licence text is there, that `NOTICE.md` has not
gone stale, that the layout is the one a build reads, and — the one that matters most — that
**this bundle's Mesa and this bundle's orbis-compat were built from the same pair**.

⚠ **It does not build anything, and it says so.** Its header lists what it provably cannot
check. Read that list once; it is short and it is the honest half.

## Is this bundle proven?

Look at `BUNDLE.txt`:

```
gate=pass        something was built from an unpacked copy of this exact tarball
gate=unproven    NOTHING has been built from it. It is not publishable and the CMake
                 toolchain file will warn you at configure time.
```

⚠ **`gate=pass` means it built, not that it ran.** Nothing in this bundle is executed on a console
by the gate; a cross-linked PS4 image cannot run on a build machine. (The two examples above HAVE
been run on hardware, by hand, on 2026-09-17 - but that is a fact about those two runs, not
something `gate=pass` asserts about the tarball you are holding.) The gate proves
the bundle produces an image and that every path on the link line came out of the bundle.

## The three things a stranger gets wrong first

1. ⚠ **`orbis-compat/include` must be searched AHEAD of `sdk/include`.** The overlay corrects
   four pthread types musl declares smaller than Sony's implementation writes — measured on
   hardware — and it works by defining musl's own `__DEFINED_<name>` guards before musl's copy
   is reached. Behind the SDK's directory it compiles, does nothing, and says nothing. The
   toolchain file gets this right; a build that adds include paths of its own can undo it.
   `orbis_compat_verify()` turns "we trust the order" into a configure-time error — call it.
2. ⚠ **In C++, `sdk/include/c++/v1` stays ahead of both.** libc++ wraps the C headers and
   `#include_next`es them. Get it wrong and you get an integer-only `std::abs` that truncates
   floats silently.
3. ⚠ **`-lc` goes last, and the overlay's archive is linked with `--whole-archive`.** Nothing
   references an interposer, so without `--whole-archive` no member is pulled in and every
   correction is silently absent from a binary that linked fine. And force-loading the overlay
   *after* libc has contributed its own `mmap.lo` is a duplicate-symbol error — which is the
   good outcome, a noisy one.

`orbis-compat/README.md` is the long version of all three, with the measurements.

## Licensing, in one paragraph

This bundle redistributes work under **GPL-3.0-only** (the SDK's own headers, linker script
and crt objects), **LGPL-3.0-only** (LibOrbisPkg's packaging tools), **MIT** (musl, Mesa, stb,
orbis-compat itself), **Apache-2.0 WITH LLVM-exception** (libc++, libc++abi, libunwind,
compiler-rt), **Apache-2.0** (the Khronos registry headers), **Zlib** (SDL, SDL_image, SDL_ttf,
zlib), **the FreeType Licence**, and **BSD-3-Clause** (one header). Every text is in
`licenses/`. `NOTICE.md` says which applies to what.

⚠ **`sdk/lib/*.so` has no licence at all.** The ~422 import stubs were generated from two
archived repositories that carry no `LICENSE` file of any kind. There is no grant to quote.
`LICENSING.md` §5.1 states this rather than papering over it, and it is the strongest argument
against this bundle existing.

⚠ **`toolchain/orbis-tls.ld` is GPL-3.0-only and it is on every link line.** It is the SDK's
own `link.x` with two patterns added, and a derivative of a GPL-3.0 file cannot be relicensed
by the deriver. It also emits bytes into the output — the four `QUAD()` values at the top of
`.text` are the ASCII of `/libexec/ld-elf.so.1`. What that implies about a binary you build
with it has **not** been established by anyone here, and neither this file nor `NOTICE.md`
pretends otherwise. If it matters to you, decide it yourself.
