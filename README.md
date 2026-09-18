# orbis-compat

What the PlayStation 4 toolchain gets wrong, corrected once instead of four times.

**Status, 2026-08-19.** Wired into all four builds of this port and verified on hardware: the title
compiles against it, boots and plays. Everything else is checked on the laptop by `./build.sh` -
nothing here needs a console to be tested, which is deliberate.

**This repository may shrink one day.** Much of what is here corrects the SDK rather than extending
it, so it would belong upstream if anyone ever took it. ⚠ Nobody has been asked, and nothing has
been offered - §7 records where each item would belong, not an arrangement with anyone.

---

## 0. Build something and put it on a console

Everything below this section explains *why*. This section is *how*, and it is first because the
question it answers used to be answered on page four.

⚠ **THE TOOLING MOVED, 2026-09-18.** Everything a person runs or unpacks - the project generator,
the worked examples, the toolchain file, the Vulkan loader shim and the whole bundle cut - now lives
in **orbis-ports/orbis-porting-kit**. This repository is the overlay: the corrections that make the
SDK's libc behave, which is what the kit and the toolchain file compile against. It has no
user-facing surface of its own on purpose, because §7's whole plan is for it to shrink.

### 0.1 The short way: one script

```sh
git clone https://github.com/orbis-ports/orbis-porting-kit
orbis-porting-kit/scripts/orbis-new.sh --check    # what is missing, and how to fix each thing
orbis-porting-kit/scripts/orbis-new.sh mygame     # a project that builds, packages and uploads
```

`--check` changes nothing, needs no console and no network. It reports each dependency with what it
is and why it is needed, then prints the remedies as a numbered list to copy:

```
3 step(s) to do - nothing was created.

do this, in order:
  1. brew install llvm lld   # then put /opt/homebrew/opt/llvm/bin and .../lld/bin on PATH
  2. brew install cmake
  3. install the OpenOrbis SDK into ~/.local/opt/openorbis (commands above),
     then export OO_PS4_TOOLCHAIN=~/.local/opt/openorbis
```

With a name it writes `CMakeLists.txt`, `main.c`, a `build.sh` that configures, builds, packages and
optionally uploads (`./build.sh --deploy <console-ip>`), a `.gitignore` and a README. `--type cpp`
gives C++; `--type vulkan` gives RADV, a headless surface, a swapchain and a triangle, which is
§0.3's example as a starting point rather than a thing to read.

⚠ **The generated sources already avoid the three traps at the end of this section**, and say why in
a comment rather than just being correct - a fixed mistake with no reason attached is a mistake
somebody re-introduces.

The rest of §0 is what that script does, for anyone who would rather do it by hand or wants to know
what it is doing to their machine. (It touches nothing outside the paths it names.)

### 0.2 By hand

**On the host** you need: `clang`, `clang++`, `ld.lld`, `llvm-ar`, `llvm-ranlib`, `llvm-nm`, `cmake`
and `ninja` or `make`. The SDK ships **no compiler** - it is prebuilt libraries and headers, and
clang comes from the machine. On Debian/Ubuntu `apt-get install clang lld llvm cmake ninja-build`;
on macOS `brew install llvm lld cmake ninja` and put `/opt/homebrew/opt/llvm/bin` and
`/opt/homebrew/opt/lld/bin` on `PATH`, because Apple's clang carries none of them. macOS also needs
Rosetta (`softwareupdate --install-rosetta`): every tool in the SDK's `bin/macos` is an x86_64
binary.

```sh
# 1. the SDK, and the overlay that corrects it
#    v0.5.4, asset toolchain-llvm-18.tar.gz, unpacked so that link.x is at the root
export OO_PS4_TOOLCHAIN=~/.local/opt/openorbis
git clone https://github.com/orbis-ports/orbis-compat && cd orbis-compat
./build.sh                       # builds build/liborbis-compat.a, then checks it

# 2. the worked example
cmake -S <kit>/examples/hello -B /tmp/hello -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE="<kit>/cmake/ps4-openorbis.cmake"
cmake --build /tmp/hello         # -> hello, hello.oelf, eboot.bin

# 3. a package, and onto the console
scripts/ps4/make-pkg.sh --eboot /tmp/hello/eboot.bin --out-dir /tmp/hello/pkg \
      --title-id TMPS10099 --title "Orbis SDK Hello"
scripts/ps4/deploy.sh  --pkg /tmp/hello/pkg/*.pkg --name hello --host <console-ip>
```

Install it from the console's package menu and start it. `scripts/ps4/logs.sh` catches the output;
a run that worked says this and then idles, showing a black screen:

```
orbis-sdk bundle: hello
backtrace: 1 frames
malloc_usable_size(64) = 80
orbis-sdk bundle: all checks passed
```

### 0.3 The second example: a triangle on the television

`hello` proves the **layout** - corrected headers, the overlay under `--whole-archive`, the linker
script, `crt1.o`, `create-fself` - and touches no Mesa, so nothing else can fail and be mistaken for
it. The kit's `examples/triangle/` is the other half: RADV, a swapchain, a pipeline compiled from
SPIR-V, and a frame on the screen. Two examples rather than one because a single one mixing both
reports a driver problem as a layout problem and the reverse.

It needs the Mesa bundle and `glslangValidator` on the host (`apt-get install glslang-tools` /
`brew install glslang`):

```sh
cmake -S <kit>/examples/triangle -B /tmp/tri -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE="<kit>/cmake/ps4-openorbis.cmake" \
      -DORBIS_MESA_SRC=<mesa-bundle> -DORBIS_MESA_BUILD=<mesa-bundle>/build-orbis
cmake --build /tmp/tri
```

Every Vulkan call is checked and named, so a failure says which step rather than showing a black
screen - which is what this console shows for a dozen unrelated reasons:

```
orbis-sdk bundle: triangle
triangle: ok   vkCreateInstance
triangle: ok   vkCreateHeadlessSurfaceEXT
triangle: GPU '...', api 1.x.y, driver 0x........
triangle: queue family 0 renders and presents
triangle: surface 1920x1080, format ..., 2..N images
triangle: ok   vkCreateGraphicsPipelines
triangle: setup complete - presenting
triangle: frame 0 presented
```

⚠ **The surface is `VK_EXT_headless_surface`, and that is the one platform-specific line in it.**
There is no window system, so there is nothing for a `VkSurfaceKHR` to attach to;
`mesa-ps4/src/vulkan/wsi/wsi_orbis.c` turns a headless present into `sceVideoOutRegisterBuffers` +
`sceVideoOutSubmitFlip`. Tempest does the same and says why (`vswapchain.cpp:264`). Everything else
in the file is ordinary Vulkan.

⚠ **Verified on hardware 2026-09-17**, along with `hello`. Two things were found by writing it, both
now fixed where they belonged rather than worked around here: `ps4-vkloader` did not carry zlib,
which RADV's shader cache imports, so a consumer that was not already linking it met eight undefined
symbols; and `mesa-ps4`'s batch table still called `wsi_orbis.c` *pending* long after it landed,
which is read as a fact about the code and cost an argument that this example could not draw at all.

⚠ **`-D` on the cmake command line does not survive into `try_compile`**, which is where this
toolchain file is read a second time. `OO_PS4_TOOLCHAIN` and `ORBIS_COMPAT_DIR` are therefore set in
the ENVIRONMENT above, not passed with `-D`. Passed with `-D` they are lost inside CMake's own
compiler test and the configure fails telling you to pass the flag you just passed.

⚠ **Returning from `main()` is reported as a crash on this console** (`CE-34878-0`), and the log
shows `SIGSYS` inside `_exit`. That is the platform, not your program: `hello` ends in an idle loop
for that reason and `optional/ps4_app.cpp` installs `ps4_idle_forever` for the same one. Close the
title from the PS button menu.

⚠ **No env file is needed for a normal run.** See §9.5 - a claim to the contrary lived in this file
until 2026-09-17 and was wrong. When a run *does* need one, the file is `/data/orbis-env.txt` - §3.3.

---

## 1. Why it exists

The SDK ships **musl's headers over Sony's implementation**, and Sony's userland is FreeBSD-derived.
The two disagree, structurally, and the port had been working around it in four separate trees -
none of which shared the fix. `Tempest/cmake/ps4-openorbis.cmake` passed only the SDK's include
directory, so **Mesa was the only component compiled against our corrected headers**, and `struct
stat` was corrected only in OpenGothic, which Mesa does not link.

## 2. What it corrects

### 2.1 Four pthread types musl declares smaller than Sony writes

`include/bits/alltypes.h`. Measured on hardware, `ORBIS_PTHREAD_LAYOUT_PROBE`, since deleted:

```
pthread_mutexattr_init    declared=4  touched [0..7]  span=8  OVERRUNS
pthread_condattr_init     declared=4  touched [0..7]  span=8  OVERRUNS
pthread_barrierattr_init  declared=4  touched [0..7]  span=8  OVERRUNS
pthread_spin_init         declared=4  touched [0..7]  span=8  OVERRUNS
pthread_once              declared=4  touched [0..0]  span=1  fits
```

Each of the four is an opaque pointer in Sony's implementation and four bytes in musl's declaration.
`ORBIS_PTHREAD_MUTEX_INITIALIZER` being `NULL` in the SDK's own `orbis/_types/pthread.h` had implied
it; the probe measured it.

⚠ **`pthread_once` was predicted to be the worst of the five and needs nothing.**
`ORBIS_PTHREAD_ONCE_INIT` is `{ NEEDS_INIT, NULL }`, which is FreeBSD's sixteen-byte
`struct pthread_once` - but the hardware wrote one byte, and FreeBSD's implementation locks the mutex
at offset 8 even uncontended. Sony's is a plain atomic flag; musl's four bytes are right. The
interposer once planned for it, and the claim that libc++abi's prebuilt four-byte flag was being
overrun, are both withdrawn.

**Why the port worked at all:** musl's other types are LARGER than Sony's. Writing eight bytes into
forty is harmless. Only types declared smaller than Sony writes can be overrun.

The one that was caught in the wild was `pthread_mutexattr_t`, in dEQP's `deMutex_create`: creating
one mutex cleared 24 bytes of an unrelated heap block 4704 bytes away, while the identical calls in
the caller's frame did no damage - same code, different frame, different victim.

### 2.2 Headers the SDK omits although the platform has them

`machine/cpu.h`, `pthread_np.h`, `sys/{cpuset,ioccom,param,sysctl}.h` - each says in its own comment
what it is and what it is not.

### 2.3 A futex that is not a spin

`sys/umtx.h`. Mesa's `simple_mtx` is built on FreeBSD's `_umtx_op`, and **the raw syscall is refused
on this console** - 454 answers ENOSYS. Every contended lock was a pure spin until this file stopped
declaring a syscall and became an implementation over pthreads. See §6.3: `libkernel.so` does export
a working `_umtx_op`, and we measured that ours is still the better one here.

### 2.4 backtrace(3)

`execinfo.h` and `src/orbis_backtrace.c`. musl has no `execinfo.h` by design, so code written for
glibc or FreeBSD fails to compile - and crash handlers are exactly what this port keeps needing. The
unwinder is present: `libc++.a` defines `_Unwind_Backtrace` and the build already passes
`-funwind-tables`. Addresses only; resolve them against the ELF.

### 2.5 Four interposers for things the console does differently

`orbis_{stat,mmap,mem,paths}` - the stat family translating the kernel's layout into the SDK's, the
allocator's mmap path, an `operator new` that names the pool that ran out, and path anchoring for a
process with no working directory. They moved here from OpenGothic on 2026-08-19; see §6.4 for the
knot that had to be untied first.

### 2.6 A place for corrections to speak

`orbis_log.h`. Nothing is registered by default and every call is a no-op until something registers,
so a correction can report without knowing who is listening - and without including an engine header.

### 2.6b Why the mmap interposers are LOCAL in the linked image, and why that is right

`__mmap`, `__munmap` and `__madvise` come out `LOCAL HIDDEN` in the title while `backtrace` and
`pthread_create` - from the same archive, in the same link - come out `GLOBAL DEFAULT`. That looked
like something we had done wrong and was carried in this file as an open question for a week.

It is neither. **Eleven members of the SDK's `libc.a` reference those three names as
`GLOBAL HIDDEN UND`**, because musl declares its internal allocator-to-kernel linkage hidden. An
undefined *hidden* reference forces the definition that satisfies it to hidden, and a hidden symbol
is emitted LOCAL. A minimal project where nothing from `libc.a` references `__mmap` keeps it global -
which is how this was isolated rather than reasoned about.

Hidden means "not exported dynamically". For a libc-internal name that is correct, and it says
nothing about whether the interposition works: that is proven separately, by the memory census the
console prints.

### 2.7 Three names missing from headers the SDK already ships

Each of these was a patch in a consumer before it was a line here, and each is written the way that
consumer writes it - `test/declarations.c` compiles all three and must fail without the overlay.

```
malloc_usable_size      declared in <malloc.h> only. FreeBSD puts it in <stdlib.h>, and clang
                        defines __FreeBSD__ for this triple, so portable code looks there
sigev_notify_function   the FIELD exists - signal.h:144 has FreeBSD's union - only the two macros
                        every caller writes are missing
ENODATA                 absent, as on FreeBSD. Defined to ECONNREFUSED, which is what Mesa itself
                        picks for FreeBSD, and 61 in this SDK's FreeBSD-numbered table
```

⚠⚠ **`sigev_notify_function` makes code compile into a HANG, and that is measured.**
`timer_create` with `SIGEV_THREAD` **never returns** on this kernel - no error code, no silent
timer, the calling thread simply does not come back. Measured twice, the second time with our own
`pthread_create` interposer disabled, because `libc.a`'s `timer_create` calls `pthread_create` and
waits on a barrier and that confound had to be ruled out first.

What does work, same probe: a `SIGEV_NONE` timer counts down correctly, and `timer_create` refuses
`CLOCK_MONOTONIC` while accepting `CLOCK_REALTIME`. The macros stay because they are correct and
`SIGEV_SIGNAL`/`SIGEV_NONE` callers need them; the CTS's `deTimer.c` patch stays permanently. PLAN.md
§9 is the route to making the notification actually work - a userspace job here exactly as on glibc.

### 2.7b SIGEV_THREAD, delivered by a thread of ours

`timer_create` with `SIGEV_THREAD` **never returns** on this kernel - it does not fail, it does not
stay silent, the calling thread does not come back. So the overlay implements the notification the
way glibc does: in userspace, over the two things the platform does provide.

```
countdown, SIGEV_NONE     rc 0, expired              the pass-through, exercised on every boot
SIGEV_THREAD one-shot     fired 1 after 100 ms       a kernel timer underneath, for gettime
SIGEV_THREAD interval     fired 5 in 300 ms @ 50 ms  overrun 0
```

⚠ **All five timer calls are interposed, so this repository is on the path of every timer in every
consumer.** It has to be: once the handle is ours the others must understand it, and defining
`timer_create` makes musl's unreachable. Ordinary timers are created through `ktimer_create` and come
back as the plain kernel id - musl's own encoding for them - so nothing changes for a caller that
never asked for `SIGEV_THREAD`.

⚠ **The kernel's `sigevent` is not the SDK's**, and getting that wrong broke `timer_create`
port-wide for one run. musl reorders the fields (`value` at 0x00, `signo` at 0x08, `notify` at 0x0c);
the `SIGEV_*` values themselves pass through unchanged. `ORBIS_SIGEV_THREAD=0` turns the
implementation off and refuses with `ENOTSUP` instead - still better than a hang, which nobody can
handle.

The CTS's `deTimer.c` patch is **gone**: `libdeutil.a` carries `U timer_create` again, which is
upstream's own POSIX arm resolving here.

### 2.8 A thread stack a shader compile fits in

`scePthreadCreate` gives every thread **64 KiB**. A dEQP worker died inside
`radv_graphics_shaders_compile` with about 72 KB of frame under it, and **every thread that compiles
a pipeline on this console has the same cliff** - the title only survives it because Tempest sizes
its own threads.

Where that number comes from, and where it sits:

```
                 main thread    other threads
Linux / glibc        8 MiB          8 MiB       both from RLIMIT_STACK (measured on the laptop)
FreeBSD / libthr     8 MiB          2 MiB       THR_STACK_DEFAULT = sizeof(void*)/4 MB
musl                 8 MiB        128 KiB       small on purpose; musl assumes small frames
PS4, as shipped      2 MiB         64 KiB       measured on the console
PS4, with this       2 MiB          2 MiB
```

`pthread_create` is **undefined in every member of the SDK's `libc.a`** and comes from
`libkernel.so:0xda78`, so the 64 KiB is Sony's own choice rather than the libc's.

**The policy: a thread that did not choose gets what the main thread has** - read at runtime, not
hardcoded, so it follows the platform instead of a number someone picked. That is also exactly what
glibc does with `RLIMIT_STACK`. `ORBIS_THREAD_STACK=<KiB>` overrides it; `0` interposes nothing, so
an A/B needs no rebuild.

⚠ **A fresh `pthread_attr_t` reports 65536 here**, so "asked for the default" and "asked for
nothing" are indistinguishable and both are overridden. The consequence is one-directional: a caller
that genuinely wanted 64 KiB gets more stack and still works, while the opposite mistake is a crash
inside a shader compile.

Measured on the console with the interposer in place - the probe asks `pthread_attr_get_np` about
**live threads**, so these are what the kernel gave, not what the overlay intended:

```
attr = NULL              2097152 B      was 65536
default-init attr        2097152 B      was 65536
cost                     31744 KiB of address space at 16 threads, the most a run has reached
```

A failure at any step - `pthread_attr_init`, `setstacksize`, or a floor below `PTHREAD_STACK_MIN` -
hands the original attr through untouched. **A broken interposer must behave like an absent one.**

### 2.9 A prefix header, because the SDK's own headers are not self-contained

The headers under `<orbis/>` name `size_t` and the `stdint` types without including them. The port
has been passing `-include stdlib.h` everywhere to cover it. `orbis_prefix.h` replaces that, and the
swap was measured over all 189 of them:

```
prefix                       orbis/ headers that fail to compile alone
none                         26
-include stdlib.h            16      <- what the port passes today
-include orbis_prefix.h       7
```

Better coverage from a smaller injection - two type headers instead of a whole libc one, which
matters because `stdlib.h` in every TU at global scope is exactly how this port once got an
integer-only `std::abs`.

**The remaining seven are SDK bugs**, not missing includes, and are the shortest upstream list here:
`JpegEnc.h:17` says `OrbisJpgEncOutputInfo` for `OrbisJpegEncOutputInfo`; `SysCore.h:27` names an
undefined `OrbisAppInfo`; `libc.h:352` and `LibcInternal.h:432` redeclare clang builtins
(`__sync_fetch_and_add_16` and neighbours); `Font.h`, `FontFt.h` and `Usbd.h` round it out. No prefix
reaches those, and shadowing a header to correct a typo inside it would hide the bug rather than fix
it.

## 3. Using it

Three flags, and all of them matter:

```
-isystem <orbis-compat>/include        ahead of the SDK's include directory
-include orbis_prefix.h                in place of the -include stdlib.h this port used to pass
<orbis-compat>/build/liborbis-compat.a with --whole-archive
```

⚠ **The include directory must come first.** `bits/alltypes.h` works by defining musl's own
`__DEFINED_<name>` guards before musl's copy is reached. Behind the SDK's directory it compiles, does
nothing, and says nothing.

⚠ **In C++, libc++'s directory stays ahead of both.** Tempest's toolchain file spends thirty lines on
why: libc++ wraps the C headers and `#include_next`es them, and the wrong order yields an
integer-only `std::abs` that truncates floats silently - 37 call sites in OpenGothic. The overlay goes
between libc++ and the SDK's C headers.

⚠ **`--whole-archive` is not tidiness.** Nothing references `backtrace()` until something crashes, and
nothing references an interposer at all - interposition works by the linker preferring a defined
symbol, and a symbol nobody references never pulls its archive member in.

⚠ **libc goes last on the link line.** The interposers define `__mmap`, `__munmap` and `__madvise`,
and the SDK's `libc.a` defines them too. Force-loading the overlay *after* libc has contributed its
own `mmap.lo` is a **duplicate-symbol error**, not a silent preference - which is the good outcome, a
noisy one. In CMake, that means `-lc` belongs in `CMAKE_<LANG>_STANDARD_LIBRARIES`, not in
`CMAKE_EXE_LINKER_FLAGS`.

An application that wants the corrections to log calls `orbis_set_log` once, early.

### 3.1 From CMake

`cmake/orbis-compat.cmake` carries the three of those that a build system can hold:

```cmake
include(${ORBIS_COMPAT_DIR}/cmake/orbis-compat.cmake)
orbis_compat_locate()     # finds it, or FATAL_ERRORs saying why it matters
orbis_compat_target()     # orbis::compat - the archive, with --whole-archive already on it
orbis_compat_verify()     # compiles the type assertions with THIS project's real flags
```

⚠ **The include path is the one part a target cannot carry.** CMake puts `CMAKE_<LANG>_FLAGS` on the
command line ahead of any target's `-isystem`, so an `INTERFACE_INCLUDE_DIRECTORIES` would land
behind the SDK - where the corrections compile and do nothing. It stays in the toolchain file, and
`orbis_compat_verify()` is what turns "we trust the order" into a configure-time error.

Who does this today:

```
Mesa        build-support/orbis/cross/orbis.ini.in    include only - it builds a static archive
OpenGothic  ps4-openorbis.cmake (the kit's)           include + archive + both opt-in targets
VK-GL-CTS   ps4-openorbis.cmake (the kit's)           include + archive
RetroArch   Makefile.orbis, flags transcribed         include + archive + the kit's vkloader
Tempest     nothing of its own - the title configures it
```

Both Mesa's `build.sh` and the toolchain file **refuse to build** if the overlay is missing.
`ORBIS_COMPAT_DIR` points them elsewhere; the default is `~/src-ps4/orbis-compat`.

### 3.2 The two opt-in targets

The archive is not everything here. Two directories declare targets a consumer adds **by name**,
because they are policy rather than correction:

```cmake
add_subdirectory("${ORBIS_COMPAT_DIR}/vkloader" ps4-vkloader)  # the Vulkan C ABI over RADV
add_subdirectory("${ORBIS_COMPAT_DIR}/optional" ps4-common)    # ps4-app: the klog/netlog tee
```

⚠ **Neither is in `liborbis-compat.a`, and that is the whole point.** Consumers link the archive
with `--whole-archive`, so a member arrives whether it was wanted or not. `ps4-app` obliges the link
to carry `-lSceNet` and decides that a dying process idles rather than returns; the CTS and Mesa
should not inherit either for having asked for a working `mmap`. `ps4-app` registers itself into
`orbis_log.h`'s hooks, which is how `src/` gets a channel while knowing nothing about it.

### 3.3 Setting a knob on a console: `/data/orbis-env.txt`

Every `ORBIS_*` switch here is meant to be flipped without rebuilding, and on this console the only
way to do that is a file. **`/data/orbis-env.txt` is that file** - `KEY=VALUE` per line, `#`
comments, whitespace trimmed on both sides of the `=`. `src/orbis_env.cpp` reads it and
`orbis_env_get()` answers out of it; the process environment still wins, so a value genuinely
`setenv()`ed beats the file.

⚠ **`setenv()` in one image is invisible to another here, which is why a file exists at all.** The
SDK's `libc.a` is a real static musl - `getenv`/`setenv` are defined text, not stubs into a shared
libc - so an executable and every `.prx` it loads each link their own `environ`. Measured
2026-08-23: `ORBIS_NCPU=1` was applied by the frontend and never reached the core, which still
reported five recompiler workers. A loadable module can only be reached through the file, and it
must read the file *itself*, in its own image.

⚠ **One name, because the reader cannot know who loaded it.** Three product paths -
`/data/tempest-env.txt`, `/data/retroarch-env.txt`, `/data/retroarch-glcore-env.txt` - are still
read after the generic one, so a per-product file overrides it. All three are **deprecated as of
2026-09-18** and stay only for packages already flashed; they go once a released OpenGothic and a
released RetroArch both write the generic name. Write the generic name.

```
lftp -p 2121 <console> -e "cd /data; put orbis-env.txt; bye"
```

`OpenGothic/ps4/tempest-env.example.txt` is the normative description of the format and of the
driver knobs; it names `/data/orbis-env.txt` as the destination.

## 4. Layout

```
include/bits/alltypes.h     four pthread types corrected
include/execinfo.h          backtrace(3)
include/orbis_log.h         the logging hook
include/orbis_prefix.h      the -include prefix, replacing -include stdlib.h
include/orbis_thread.h      the thread-stack floor, and the probe that measured it
include/orbis_boot.h        the ctype probe and the crash handlers, out of a game that wrote them
include/{errno,signal,stdlib}.h   three names the SDK's own headers leave out
include/orbis_{stat,mmap,mem,paths,timer,netlog,clock}.h
include/ps4_app.h           the console log channel's API - implemented in optional/, not src/
include/machine/, sys/, pthread_np.h
src/                        THE ARCHIVE. orbis_backtrace.c orbis_log.c orbis_boot.cpp
                            orbis_{stat,mmap,mem,paths,thread,timer,sigev,clock}.cpp
optional/                   NOT the archive - policy, added by name. orbis_netlog.cpp ps4_app.cpp
                            orbis_bigheap.c orbis_thread_atexit_stub.c, and a CMakeLists that
                            declares ps4-netlog / ps4-app from the first two ONLY
vkloader/                   the Vulkan C ABI: vkloader.c, 771 weak thunks, gen.py, a CMakeLists
cmake/                      ps4-openorbis.cmake (the toolchain file), ps4-package.cmake,
                            orbis-compat.cmake - locate / orbis::compat / verify,
                            orbis-tls.ld (GPL-3.0-only, see §8), orbis.ini.in (the meson cross file)
crt/                        an MIT C runtime: crt1, crtlib, crti, crtn, and orbis_sce_params.h
                            holding the loader's parameter blocks as compiler-checked structs.
                            ORBIS_CRT=sdk|own selects; see §6.5
licenses/  NOTICE.md  LICENSING.md
                            the licence ledger for the redistributable bundle - one row per
                            component, with the text every one of them requires to travel
scripts/release/            sdk-licenses.sh ONLY - it WRITES licenses/, NOTICE.md and LICENSING.md,
                            so it stayed where the files it writes are. The cut, the verify, the
                            gate, their 27 tests and the two worked examples moved to the porting
                            kit on 2026-09-18; the cut calls this script where it lives and ships
                            it in the bundle
.github/workflows/          build.yml (./build.sh and its checks on every push, with a negative
                            control) and the ORBIS_* knob check. The bundle workflow moved with the
                            scripts it runs
                            orbis-tls.ld - the linker script. ⚠ GPL-3.0-only, not MIT: it is the
                            SDK's own link.x, corrected. §8 and the file's own header say why
(the tooling)               cmake/, vkloader/ and scripts/orbis-new.sh moved to the porting kit on
                            2026-09-18 and are gone from here. scripts/ps4/orbis-env.sh resolves the
                            toolchain file through ORBIS_KIT_DIR, falling back to this repository
                            for bundles and pins older than that day
scripts/ps4/                make-pkg.sh gen-icon0.py log-receiver.py logs.sh peerfilter.py
test/                       sizes.c declarations.c backtrace_host.c pthread_probe_host.c
                            umtxcheck.c crt_abi.sh (the crt's section/symbol comparison)
build.sh                    produces build/liborbis-compat.a from src/ ONLY, then checks it
```

The C sources are written in four-space K&R and the C++ ones in Tempest's two-space style, because
that is what they were when they moved here. **Left alone deliberately**: reformatting a file that
moved makes the move unreadable.

## 5. Verification

`./build.sh` builds and then checks, on the laptop; `--no-check` skips the second half. The build
itself is the "does everything compile" check, so what follows is only what compiling cannot tell you:

* the four types are corrected, and `pthread_mutex_t`/`cond`/`rwlock` are **not** - and the same test
  must FAIL without the overlay, or it proves nothing
* `malloc_usable_size`, `sigev_notify_function` and `ENODATA` are all reachable the way their
  consumers reach them - with the same negative control
* every header is self-contained, compiled alone, C or C++ as its contents require
* `backtrace` collects frames, bounds its buffer and formats addresses - **run natively**, with a
  positive control that deliberately overruns

`test/pthread_probe_host.c` is kept but no longer run: it mirrors the console probe that measured the
four types, and that probe was deleted once it had answered. It is a record of the method, and the
starting point if `pthread_condattr_t` in Mesa's `cnd_monotonic.c` ever needs the same treatment.

⚠ **The host test earned its place immediately**: it caught a missing `<stdint.h>` that the cross
build had accepted through the PS4 headers. *A file that cross-compiles is not evidence that it is
correct.*

## 6. Decisions that are not obvious

### 6.1 Resize, or interpose? Ask what libc++ already embeds

The toolchain's `libc++.a` and `libc++abi.a` are prebuilt against musl's declarations, which splits
the undersized types in two:

```
                        in a prebuilt libc++ object?     therefore
pthread_mutexattr_t     no                               RESIZE in the header
pthread_condattr_t      no                               RESIZE
pthread_barrierattr_t   no                               RESIZE
pthread_spinlock_t      no                               RESIZE
pthread_once_t          yes - and it needs nothing        LEAVE ALONE (§2.1)
```

⚠ **This is where we differ from OpenOrbis/musl PR #29, and it should be said when anything is
sent.** #29 turns `pthread_mutex_t` into an 8-byte pointer, which is correct about Sony and fatal
here: `std::mutex` embeds it, so every prebuilt libc++ object would disagree with every object we
compile. Oversized is what has kept this port alive. A musl PR can be bolder than an overlay - but
only if libc++ is rebuilt in the same release, and that is probably why #29 has sat since 2022.

### 6.2 The syscall table is refused wholesale

The driver's own startup check:

```
raw syscall check: syscall(SYS_getpid)=-1 errno=78, getpid()=136
                   - raw syscalls are refused, so _umtx_op may exist behind libkernel
```

Even `getpid`. So ENOSYS on 454 was never evidence about `_umtx_op` in particular.

### 6.3 libkernel's `_umtx_op` works, and ours is still better here

Four rungs, 2026-08-19: a WAIT whose expected value does not match returns instantly **with no
timeout** - so it reads the word rather than sleeping blindly - while the same call with a matching
value and a 150 ms deadline slept 149906 us and returned ETIMEDOUT.

⚠ **The mismatch case returns 0, not EWOULDBLOCK.** That is not Linux's convention, and the probe's
own verdict string called it "not a comparison, so not an implementation" - wrong. Neither rung
settles it alone.

Then the A/B decided it. Same console, same 49 `object_management.multithreaded_*` cases, same
arguments, driver told apart per binary by whether `_umtx_op` is undefined:

```
A  libkernel        15 tests, 14 passed,  63 devices   ABORTED - VK_ERROR_OUT_OF_HOST_MEMORY
B  shims/sys/umtx.h 49 tests, 49 passed, 179 devices   completed, 100%
```

Run A does not hang - which is real, and is what the probe predicted - but it does not finish, and B
finishes the same list on the same hardware. `-DORBIS_UMTX_LIBKERNEL=1` selects Sony's, off by
default. One run each; the mechanism behind the exhaustion is not established.

### 6.4 The four interposers were one knot

`orbis_stat` looks self-contained by its `#include` list and calls `Ps4Og::anchorPath` from
`orbis_paths`, which called `ps4_log` from the Tempest fork - the only thing the other three took
from it either. Moving stat alone put an object with an unresolved symbol into the archive, and
`--whole-archive` then broke CMake's compiler test in **every** project using the toolchain file,
before any of their own code was reached.

Untied by `orbis_log.h`: Tempest's `ps4_log` was split into a `va_list` form, `ps4_app_init`
registers it, and the interposers call the overlay's hook. 29 call sites renamed, four includes
repointed, no logic touched.

## 7. Where each item would belong

⚠ **This is attribution, not a plan, and nobody upstream has been contacted about any of it.** The
table says which project a correction is properly a fix TO - useful for deciding whether something
is our workaround or their bug, which is a different question from whether it will ever be sent.

| item | belongs to | note |
|---|---|---|
| the four pthread sizes | **OpenOrbis/musl** | measured; extends PR #29 but corrects fewer types, see §6.1 |
| `struct stat` layout | **OpenOrbis, and partly done** ⚠ | `mode_t` is the WHOLE defect - narrowing it to FreeBSD's `uint16_t` makes the struct match the kernel field for field (measured). ⚠ But it must ship WITH a rebuilt `libc++.a`, which reads `st_size` at the wide offset and is correct today; alone it breaks `std::filesystem` silently. ⚠ **AND UPSTREAM REACHED THE SAME DIAGNOSIS INDEPENDENTLY**: PR #278 (red-prig, merged 2025-07-21) narrows `OrbisKernelMode` to `uint16_t` in `include/orbis/_types/kernel.h`, and the field table in PLAN.md §3 matches the resulting struct field for field. Two things it does NOT settle: it corrects only the Orbis-namespaced type, so `stat()`/`fstat()` still read the wrong layout and this overlay's interposer is still required; and it is in the v0.5.4 **tag** while the v0.5.4 **release asset** is the v0.5.3 tree - `kernel.h` in the published tarball is byte-identical to v0.5.3 and still says `typedef mode_t OrbisKernelMode`. Anyone pinning the asset, which is everyone, does not have the fix |
| `lstat`, `fstatat` | **stays ours** | not misdeclared - ABSENT. `libc.a`'s `fstatat` sets errno 78 and returns -1, and `lstat` tail-jumps into it. No typedef revives them |
| `machine/*`, `pthread_np.h`, `execinfo.h` | **OpenOrbis toolchain** | headers the SDK simply lacks |
| `malloc_usable_size`, `sigev_notify_function`, `ENODATA` | **OpenOrbis/musl** | three names missing from headers that ship; each is one line |
| the `pthread_create` stack floor | **OpenOrbis/musl** | `libc.a` has no `pthread_create` at all today, so this is an addition rather than a change. §2.8 has the cross-platform table anyone writing it up would need |
| SIGEV_THREAD delivery | **OpenOrbis/musl** | musl's own SIGEV_THREAD path hangs here; ours is the userspace construction glibc uses. ⚠ Send the sigevent-reordering note with it - that is where an implementer will lose an afternoon |
| the seven broken `<orbis/>` headers | **OpenOrbis toolchain** | real bugs, listed in §2.8. `orbis_prefix.h` itself stays until the other 19 are self-contained |
| `sys/{cpuset,ioccom,param,sysctl}.h` | **OpenOrbis toolchain** | the same, for the FreeBSD side of the platform. `ioccom.h` is BSD-3-Clause and should be sent as FreeBSD's file, not as ours |
| `_umtx_op` | **OpenOrbis/musl** | with §6.2 and §6.3, which change the story |
| mmap, heap, no-cwd interposers | **OpenOrbis toolchain**, if they want them |
| `orbis_log.h` | **stays ours** | an overlay concern, not a libc one |
| the corrections themselves | **deleted** | each exists only until its upstream item lands |

⚠ **Anything sent upstream has to be measured, not inferred.** Every number in §2 and §6 came off the
console; that is the bar.

## 8. Known gaps

* ⚠ **Nothing here is committed anywhere but this repository.** The four forks it serves still carry
  their side of the wiring as uncommitted changes.
* ⚠ **This repository is not uniformly MIT, and said it was until 2026-09-17.** Two files are not:
  `include/sys/ioccom.h`, whose macros follow FreeBSD's (BSD-3-Clause) because they encode an ABI,
  and `cmake/orbis-tls.ld`, which is the OpenOrbis SDK's own `link.x` (v0.5.4, asset
  `toolchain-llvm-18.tar.gz`) with two match patterns added - the toolchain is **GPL-3.0 with no
  linking exception**, so that file is `GPL-3.0-only` and carried an MIT header it had no right to.
  Its own comment records the provenance. Everything else here is MIT, and `LICENSE` now says so
  with those two named. The licence survey went no further than this repository and the SDK tree it
  derives from: every other tracked file was diffed against the same-named file under the SDK and
  none of them is a copy (the largest verbatim overlap outside `orbis-tls.ld` is 12 lines of musl's
  `__NEED_`/`__DEFINED_` guard idiom in `include/bits/alltypes.h`).
* One GPU stall survives a working run - one submit in ~1200, fence stuck for four submissions, every
  address mapped. **Pre-existing**, not introduced here.
* CI exists now and covers one thing only: `.github/workflows/sdk-bundle.yml` cuts and verifies
  the redistributable bundle. `./build.sh` is still the whole of the overlay's own checking and is
  still run by hand. ⚠ And the bundle's publication gate has run exactly once, on macOS - every
  finding it made on that run is recorded in the scripts it made them about.

## 9. Traps this cost

1. ⚠ **The package goes to `/data/pkg`, not `/data`.** Uploaded to the wrong place, the console starts
   the previously installed build and logs nothing new. **A missing answer is indistinguishable from
   an answer.**
2. ⚠ **WITHDRAWN, 2026-09-17: there is no `shims/` any more.** It said `build.sh` copies `shims/`
   into `~/.cache/orbis-mesa/cross/include`, so editing the source and running `ninja` compiled the
   old copy. Those seven headers moved into this repository and `mesa-ps4/build-support/orbis/build.sh`
   says so at line 68 - *"The seven headers that used to be copied from ${ROOT}/shims are GONE FROM
   THIS TREE"*. The directory does not exist. The general shape survives as trap 7.
3. ⚠ **`meson setup` by hand loses what build.sh sets.** `PKG_CONFIG_PATH=` and `PKG_CONFIG_LIBDIR=`
   pointing at an empty directory are what stop nix's devShell supplying the HOST's libelf.
4. ⚠ **The build date does not identify a package.** `build.sh` runs meson through `nix develop`,
   which sets `SOURCE_DATE_EPOCH=315532800`, so every driver it builds reports
   `arm built Jan 1 1980 00:00:00`. Use `-Dradv-build-id=<string>`.
5. ⚠ **This trap is WITHDRAWN, and it was wrong in the direction that matters for anyone who is
   handed a package.** It used to say that `ORBIS_3D_LINEAR=1` and `ORBIS_NO_TESS=1` are off in the
   driver by default, that the console's env file is the only thing that turns them on, and that a
   run without it crashes entering 3D. That was true once. The defaults were moved INTO the driver
   and the entry was not updated. Read off `mesa-ps4` on 2026-09-17:

   ```c
   /* ac_surface.c, under HAVE_ORBIS_PLATFORM */
   const bool off = linear3d != NULL && linear3d[0] == '0' && linear3d[1] == '\0';
   if (!off) { mode = RADEON_SURF_MODE_LINEAR_ALIGNED; ... }
   ```

   Unset means `off` is false, so linear IS applied; `ORBIS_NO_TESS` reads the same way round.
   `OpenGothic/ps4/tempest-env.example.txt` has said so for a while - *"A NORMAL RUN NEEDS NO FILE AT
   ALL. The driver ships the configuration it was tested in"* - and `ac_surface.c`'s own comment gives
   the reason: *"Anyone who downloads this driver gets the configuration it was tested in."*

   **So a package runs on a console with no env file, which is the only thing a stranger who
   downloaded a release can have.** The file turns a diagnostic ON or a default OFF, for one run.

   The trap worth keeping is the opposite one, and it is in that example file rather than here: a
   knob LEFT BEHIND applies to every later run, including a different title's - a watermark left in
   on 2026-09-01 froze RetroArch's menu for 1.5 s every 768 frames. Take a knob out when its run is
   done.
6. ⚠ **`git checkout -- <file>` on an uncommitted file destroys it**, with no stash, no reflog and no
   dangling blob. 209 lines of OpenGothic's CMakeLists went that way and had to be reconstructed.
7. ⚠ **A stale build directory answers questions about a build that no longer exists.**
   `~/.cache/tempest-og/build-ps4` still compiles `og_ps4_mmap.cpp` and links no overlay at all; the
   title's real build directory is `~/.cache/opengothic-ps4/build`. Read `link.txt` before concluding
   anything from a build tree.

---

## The build contract every repository in orbis-ports shares

Each repository has one entry point, in the same place, taking the same two things from the
environment. Clone the repositories next to each other and nothing needs setting at all:

    ~/somewhere/
      orbis-compat/          <- this one
      mesa-ps4/              ps4/build.sh
      VK-GL-CTS/             ps4/build.sh
      OpenGothic/            ps4/build.sh
      RetroArch/             ps4/build-core.sh

    ORBIS_COMPAT_DIR   this checkout.   Searched: the variable, then ../orbis-compat, then
                       ~/src-ps4/orbis-compat. Refused loudly, by name, if none of them is one.
    OO_PS4_TOOLCHAIN   the OpenOrbis SDK. Default ~/.local/opt/openorbis, checked by link.x
                       rather than by the directory - a toolchain missing its linker script fails
                       hundreds of files later with an error that names nothing.

    ORBIS_WORK         where builds write. Default ~/.cache/orbis-ports; never the checkout.
    ORBIS_JOBS         parallelism. Default nproc.

`scripts/ps4/orbis-env.sh` is what resolves and verifies all four; every entry point sources it.

⚠ **Finding orbis-compat cannot itself be shared** - that is the chicken-and-egg of a shared
prologue - so each entry point carries six lines of path search before it can source anything. Copy
that block verbatim rather than inventing a variant; everything after it lives in one file precisely
so the five copies cannot drift.

### Deploying

    scripts/ps4/deploy.sh --pkg <file> --name <short> [--also <local>:<remote>]...

Packages go to `/data/pkg/<name>-YYYYMMDD.pkg`, which is the only directory this console installs
from, and dated because it keeps every package ever installed and an undated name cannot be pointed
at one build. Uploads are verified by reading back - sizes for packages, byte for byte for anything
under a megabyte - and the script ends by saying whether the next step is **INSTALL + RUN** or
**RUN, no install**. An old package under the same title id will otherwise run instead, silently.

⚠ **Size does not prove freshness.** Three CTS packages built on three different days were all
exactly 109117440 bytes. This checks that what arrived is what was sent; whether what was sent is
what you meant is answered by the build script, which prints the driver it linked and when it was
built.
