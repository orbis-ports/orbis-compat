# orbis-compat

What the PlayStation 4 toolchain gets wrong, corrected once instead of four times.

**Status, 2026-08-19.** Wired into all four builds of this port and verified on hardware: the title
compiles against it, boots and plays. Everything else is checked on the laptop by `./build.sh` -
nothing here needs a console to be tested, which is deliberate.

**This repository may shrink one day.** Much of what is here corrects the SDK rather than extending
it, so it would belong upstream if anyone ever took it. ⚠ Nobody has been asked, and nothing has
been offered - §7 records where each item would belong, not an arrangement with anyone.

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

## 3. Using it

Two flags, and both matter:

```
-isystem <orbis-compat>/include        ahead of the SDK's include directory
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
Tempest     cmake/ps4-openorbis.cmake                 include + archive, and refuses without them
OpenGothic  inherits Tempest's toolchain file
VK-GL-CTS   inherits Tempest's toolchain file
```

Both Mesa's `build.sh` and Tempest's toolchain file **refuse to build** if the overlay is missing.
`ORBIS_COMPAT_DIR` points them elsewhere; the default is `~/src/forks/orbis-compat`.

## 4. Layout

```
include/bits/alltypes.h     four pthread types corrected
include/execinfo.h          backtrace(3)
include/orbis_log.h         the logging hook
include/orbis_{stat,mmap,mem,paths}.h
include/machine/, sys/, pthread_np.h
src/                        orbis_backtrace.c orbis_log.c orbis_{stat,mmap,mem,paths}.cpp
test/                       sizes.c backtrace_host.c pthread_probe_host.c
cmake/orbis-compat.cmake    locate / orbis::compat / verify, for consumers that use CMake
build.sh                    produces build/liborbis-compat.a, then checks it
```

The C sources are written in four-space K&R and the C++ ones in Tempest's two-space style, because
that is what they were when they moved here. **Left alone deliberately**: reformatting a file that
moved makes the move unreadable.

## 5. Verification

`./build.sh` builds and then checks, on the laptop; `--no-check` skips the second half. The build
itself is the "does everything compile" check, so what follows is only what compiling cannot tell you:

* the four types are corrected, and `pthread_mutex_t`/`cond`/`rwlock` are **not** - and the same test
  must FAIL without the overlay, or it proves nothing
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
| `struct stat` layout | **OpenOrbis/musl** | PR #35 covers `mode_t`; verify it covers the rest |
| `machine/*`, `pthread_np.h`, `execinfo.h` | **OpenOrbis toolchain** | headers the SDK simply lacks |
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
* No licence header survey beyond this repository's own: everything here is MIT except
  `include/sys/ioccom.h`, whose macros follow FreeBSD's (BSD-3-Clause) because they encode an ABI.
* One GPU stall survives a working run - one submit in ~1200, fence stuck for four submissions, every
  address mapped. **Pre-existing**, not introduced here.
* ⚠ `__mmap`, `__munmap` and `__madvise` are **local** (`t`) in the linked title rather than global.
  Static linking is unaffected and the title runs, but there is no pre-move binary left to compare
  against - this is unverified rather than known-good.
* No CI. `./build.sh` is the whole of it, and it has to be run by hand.

## 9. Traps this cost

1. ⚠ **The package goes to `/data/pkg`, not `/data`.** Uploaded to the wrong place, the console starts
   the previously installed build and logs nothing new. **A missing answer is indistinguishable from
   an answer.**
2. ⚠ **`build.sh` copies `shims/` into `~/.cache/orbis-mesa/cross/include`.** Editing the source and
   running `ninja` directly compiles the old copy.
3. ⚠ **`meson setup` by hand loses what build.sh sets.** `PKG_CONFIG_PATH=` and `PKG_CONFIG_LIBDIR=`
   pointing at an empty directory are what stop nix's devShell supplying the HOST's libelf.
4. ⚠ **The build date does not identify a package.** `build.sh` runs meson through `nix develop`,
   which sets `SOURCE_DATE_EPOCH=315532800`, so every driver it builds reports
   `arm built Jan 1 1980 00:00:00`. Use `-Dradv-build-id=<string>`.
5. ⚠ **The port's env file is configuration, not options.** `ORBIS_3D_LINEAR=1` and `ORBIS_NO_TESS=1`
   are OFF in the driver by default and the console's env file is the only thing that turns them on.
   Cutting it down to "what this run needs" makes the title crash entering 3D.
6. ⚠ **`git checkout -- <file>` on an uncommitted file destroys it**, with no stash, no reflog and no
   dangling blob. 209 lines of OpenGothic's CMakeLists went that way and had to be reconstructed.
7. ⚠ **A stale build directory answers questions about a build that no longer exists.**
   `~/.cache/tempest-og/build-ps4` still compiles `og_ps4_mmap.cpp` and links no overlay at all; the
   title's real build directory is `~/.cache/opengothic-ps4/build`. Read `link.txt` before concluding
   anything from a build tree.
