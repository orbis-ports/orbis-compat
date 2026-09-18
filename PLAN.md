<!-- Copyright © 2026 Mikołaj Mikołajczyk -->
<!-- SPDX-License-Identifier: MIT -->

# The plan, in order

**One item at a time, top to bottom.** Everything this overlay is *for* is done and confirmed on
hardware; what remains is a finite list, and the reason it is written down is that it kept growing
sideways. An idea that is not on this list goes to §Parking, not into the tree.

An item is finished when its **Done when** line is true. Not before, and not "mostly".

Status, 2026-09-17: **1-5, 7 and 9 done. 6 partly done - CI exists for the bundle and not for
`build.sh`. 8 has no date, and upstream has moved underneath it; see the note on that item.**
Items 10-12 are new, and every one of them was found by running something rather than reading it.

---

## 1. The default thread stack is 64 KiB, and that is Sony's number

**What is known, measured rather than assumed:**

```
pthread_create        libkernel.so:0xda78, beside scePthreadCreate
libc.a                pthread_create is UNDEFINED in every member - nothing here implements it
default stack         65536 bytes (klog backtrace; tcuMain prints it at startup)
what died             radv_graphics_shaders_compile, with about 72 KB of frame under it
```

64 KiB is neither libc's default nor the kernel's family default: FreeBSD's libthr uses
`THR_STACK_DEFAULT` = 2 MB on 64-bit, musl uses 128 KiB. It is **the platform's own choice, made
inside libkernel**.

⚠ **That does NOT mean only we can fix it, and the first draft of this file claimed it did.** A
wrapper in an object file overrides a definition in a shared library at static link time - measured,
not reasoned: a throwaway archive defining `pthread_create` and calling `scePthreadCreate` links
against `-lkernel` with no conflict, and `main` calls ours rather than libkernel.so:0xda78.

    link rc = 0
    0000000000000060 T pthread_create          <- ours, in the executable
    main: callq 0x60 <pthread_create>          <- and the call goes there

`pthread_create` is undefined in every member of OpenOrbis's own `libc.a` today, so **the same
wrapper belongs in their libc**, where everyone gets it instead of only those who link this overlay.
This item is a normal upstream candidate like the rest of §8, not an exception to it.

⚠ **Every thread that compiles a pipeline on this console has this cliff**, not just dEQP. The title
survives today because its compiles run on Tempest's own threads, which are created with an explicit
size. dEQP's threads are not, which is why the CTS carries a 7-line patch in `deThreadUnix.c`.

**The decision to take:** interpose `pthread_create` here - the overlay already interposes `__mmap`
and friends, and the shape is the same - so that a call with **no attributes, or attributes carrying
the platform default**, gets a stack that a shader compile fits in. A caller that asked for a size
must keep exactly what it asked for.

**Open questions to answer while doing it, not after:**
* what size? 1 MB is what the CTS patch chose; nothing has measured the actual high-water mark.
* how is "the caller did not choose" detected, given `pthread_attr_t` is Sony's opaque pointer?
  `pthread_attr_getstacksize` on a default-initialised attr is the obvious probe, and it needs
  checking on hardware rather than reasoning.
* the cost: one extra `getstacksize` per thread creation. Threads are not created in a hot loop.

**Done when:** the overlay raises it, a console run shows a pipeline compiling on a thread the
overlay sized, and the CTS's `deThreadUnix.c` patch is deleted rather than kept "just in case".
Then it goes to §8 as a `libc.a` wrapper, and leaves here.

## DONE, 2026-08-19 — except the deletion, which is not mine to make

Both open questions were answered by a probe before a line of policy was written, and both mattered:

    a fresh attr claims       65536 B     so "asked for the default" == "asked for nothing"
    the main thread has     2097152 B     the platform DOES know a sane number
    attr = NULL                65536 B
    default-init attr          65536 B     the same, so ONE policy rather than two

The policy is "a thread that did not choose gets what the main thread has", read at runtime - which
is what glibc does with RLIMIT_STACK, so it is not a number this port invented. Not configurable since 2026-09-18.

Confirmed on the console with the interposer linked in, the probe reading LIVE threads through
`pthread_attr_get_np`:

    attr = NULL             2097152 B     was 65536
    default-init attr       2097152 B     was 65536
    cost                    7936 KiB of address space, under 8 threads in the whole run
    failures                none - no `raised to 0`, no INTERPOSER IS NOT DOING ITS JOB

⚠ **Still open: delete the CTS's `deThreadUnix.c` patch.** The overlay covers it now, but VK-GL-CTS
is uncommitted - it belongs to §5, with the rest of the consumer wiring.

---

## 2. Does `SIGEV_THREAD` deliver on this kernel?

`include/signal.h` adds `sigev_notify_function` and `sigev_notify_attributes`, and that makes
portable timer code **compile**. It says nothing about whether the notification arrives.

```
timer_create          real: 0x1a1 bytes in libc.a, calling ktimer_create
SIGEV_THREAD          defined as 2 in signal.h:160
does it fire          UNMEASURED
```

**The probe:** create a timer with `SIGEV_THREAD`, arm it for 100 ms, wait 1 s, report whether the
function ran. Twenty lines, one console run, and it can ride along with any other package.

**Done when:** the answer is in the handoff, and either the CTS's `deTimer.c` patch is deleted (it
fires) or the patch gains a comment citing this measurement (it does not).

## DONE, 2026-08-19 — and the answer is worse than either option allowed for

```
SIGEV_NONE countdown      WORKS - 100 ms armed, read at 250 ms, nothing left
CLOCK_MONOTONIC           REFUSED by timer_create; CLOCK_REALTIME is what answered
SIGEV_THREAD              timer_create NEVER RETURNS
```

Not an error code, not a timer that stays silent - **the calling thread does not come back.** The
title never reached its menu and the klog carried no fault, because nothing faulted.

⚠ **The first run had a confound this repository introduced the same day.** `libc.a`'s
`timer_create` calls `pthread_create` and then waits on a barrier, and `pthread_create` is §1's
interposer now. A second run with `ORBIS_THREAD_STACK=0` hung identically - and re-measured the raw
64 KiB default on the way, proving the disable knob really disables. **The platform is the cause;
the interposer is exonerated.** Blaming Sony before ruling that out would have been guessing.

The probe now sits behind `ORBIS_TIMER_PROBE=1`, off by default. ⚠ It should have been built that way
from the start: it was written to measure something known to be unsupported, on the title's boot
path, with no way out.

`include/signal.h`'s macros STAY - they are correct, they name fields the struct really has, and
SIGEV_SIGNAL/SIGEV_NONE callers need them. Their comment now says plainly that SIGEV_THREAD compiles
into a hang.

⚠ **Still open: the CTS's `deTimer.c` patch stays permanently and should gain a comment citing this**
- part of §5, in an uncommitted tree.

---

## 3. What of `struct stat` is left after OpenOrbis/musl PR #35?

`orbis_stat` translates the kernel's layout into the SDK's. PR #35 upstream covers `mode_t`. Whether
it covers the rest has been **written down as a question twice and never answered**.

**The work:** field by field, compare what the interposer corrects against what the PR changes. Some
of the interposer may already be dead weight; the rest is what to send upstream next.

**Done when:** README §7's `struct stat` row names the specific fields that still need us, or the
interposer shrinks.

## DONE, 2026-08-19 — and the answer is "all of it, and none of it can be shipped"

**ONE TYPEDEF EXPLAINS THE WHOLE SHEAR.** FreeBSD's `mode_t` is `uint16_t`; musl's is
`unsigned int`. Dumping the record layout with the real toolchain, once as shipped and once with
`mode_t` narrowed through the same `__DEFINED_` mechanism the pthread types use:

```
                as shipped     mode_t = u16     the kernel, measured on the console
st_mode              8              8                  8
st_nlink            12             10                 10
st_uid              16             12                 12
st_size             80             72 (0x48)          72 (0x48)
st_blocks           88             80 (0x50)          80 (0x50)
st_birthtim        112            104                104
sizeof             128            120                120
```

Field for field. Every other difference is alignment following that one width.

⚠ **AND IT CANNOT BE SHIPPED HERE.** The prebuilt `libc++.a` reads `st_size` at the WIDE offset and
is right today - the console's own boot probe reads `Speech1.vdf` twice, through our interposer and
through `std::filesystem`, and gets the same 722595072 with `ec=0`. Narrowing `mode_t` would move the
field under prebuilt code that cannot be recompiled. **The same reason three pthread types were
deliberately left alone in §2.1 of the README.**

So the message for OpenOrbis is not "fix mode_t" but **"fix mode_t and rebuild libc++ in the same
release"** - a mode_t change alone silently breaks `std::filesystem`, and silently is the word.

**WHAT STAYS OURS EVEN AFTER THAT, and it is not about layout at all.** Fresh disassembly of the
SDK's `libc.a`:

```
fstatat.lo   movl $0x4e,(%rax); movl $-1,%eax; ret    errno 78 = ENOSYS. A stub.
lstat.lo     jmp fstatat                              so lstat is ENOSYS too
fstat.lo     jmp _fstat                               forwards to libkernel, no translation
stat         not in libc.a at all                     comes straight from libkernel
```

`lstat` and `fstatat` are not misdeclared, they are **absent**. No typedef revives them; those two
implementations stay ours until OpenOrbis writes them.

---

## 4. Two things in the title that are not about the title

`OpenGothic/ps4/og_ps4_boot.cpp` is 960 lines. Most of it is Gothic - VDF archives, save paths, the
data inventory. Two pieces are not:

```
probeCtype             a question about this musl, not about this game
installCrashHandlers   ~80 lines; its only outside dependency is ps4_log, which is a hook now
```

The crash handlers belong here in particular: **this overlay is what supplies `backtrace()`**, and
every consumer of this SDK wants a stack trace on a pad, not just one game.

**Move without editing.** A move that also changes code cannot be bisected. One console run of the
title afterwards, to show the handlers still install (`sigaction rc=0`) and the ctype verdict is
unchanged.

**Done when:** both are here, OpenGothic includes them from the overlay, and a run shows the same
two lines it showed before the move.

## DONE, 2026-08-19 — the two lines came back identical

```
boot: ctype probe: tolower('A')=97('a') ... isspace(' ')=1 locale=C
boot: ctype probe - VERDICT: case folding works
boot: crash handlers installed (... sigaction rc=0, sigaltstack rc=-1 - NO alt stack ...)
```

Character for character what they were before the move, which is the whole test. The title reached
3D and played. `og_ps4_boot.cpp` lost 159 lines and gained a note saying where they went.

⚠ **A FOURTH SDK DEFECT FELL OUT OF IT.** `sa.sa_sigaction` does not compile as the SDK defines it:
`signal.h:136` names the union member `__sa_sigaction`, `:142` defines the macro as
`__sa_handler.sa_sigaction` - one underscore pair short. The moved code carried a hand-written
workaround; the overlay corrects the macro instead, so the POSIX spelling works for every consumer.
`test/declarations.c` has the negative control.

**Two seams the move needed, and neither was invented for it:** `orbis_log_fatal` (the ordinary
channel here is UDP, and a datagram from a process the kernel is about to kill may never leave) and
`orbis_fatal_action` - **the overlay reports, the application decides what happens next**. On this
console that choice is visible to the user: returning from `main()` is reported as CE-34878-0.

---

## 5. Point the consumers at what already exists

⚠ **Blocked until the three forks are committed.** Tempest, OpenGothic and VK-GL-CTS carry their
half of the wiring uncommitted, and 209 lines were lost that way once already.

```
Tempest/cmake/ps4-openorbis.cmake   include(cmake/orbis-compat.cmake); orbis_compat_target();
                                    orbis_compat_verify(); -include orbis_prefix.h for stdlib.h
VK-GL-CTS deMemory.c                DONE 2026-08-19 - reverted to upstream, overlay declares it
VK-GL-CTS deThreadUnix.c            DONE 2026-08-19 - reverted to upstream, overlay raises the stack
VK-GL-CTS deTimer.c                 KEPT, and its comment now cites the measurement rather than
                                    claiming the struct lacks a field it has
```

The CTS half is done and verified by building: `make debase dethread deutil` compiles upstream's own
sources against the overlay, and `libdebase.a` still carries `U malloc_usable_size`. Its patch set is
six files now, and two of them stopped being patches at all.

⚠ Its `build-umtx/` tree - 9164 files, 2.3 GB, including a built `deqp-vk` and two eboot pairs from
the umtx A/B - was STAGED in git. Removed from the index, kept on disk, `build*/` added to
`.gitignore`. Deleting it from disk would cost a full dEQP rebuild and save nothing that matters.

⚠ `-lc` stays in `CMAKE_<LANG>_STANDARD_LIBRARIES`. Ahead of the overlay it is a
`duplicate symbol: __mmap` link error - measured, on a throwaway project, not predicted.

**Done when:** the toolchain file has no hand-spliced overlay paths left, and all four builds pass.

## DONE, 2026-08-19

Tempest's toolchain file includes the module, calls `orbis_compat_locate()`, and passes
`-include orbis_prefix.h` in place of `-include stdlib.h`. OpenGothic calls `orbis_compat_verify()`
after `project()`. A **from-scratch** configure and build of the title: `Performing Test
ORBIS_COMPAT_TYPES_CORRECTED - Success`, then zero errors.

⚠ **THE FIRST BUILD OF THIS CHANGE WAS GREEN AND TESTED NOTHING.** `CMAKE_<LANG>_FLAGS_INIT` only
applies at FIRST configure, and the existing build directory had its flags cached from August, so
`flags.make` still said `-include stdlib.h`. Checking the generated flags rather than the exit code
is what caught it. A fresh directory needed `-DCMAKE_POLICY_VERSION_MINIMUM=3.5`, because doctest's
`cmake_minimum_required` predates CMake 4 - **worth knowing that the working build directory could
not be reproduced from scratch without that flag.**

⚠ **`link_libraries(orbis::compat)` DOES NOT WORK IN THIS TREE.** Tempest export()s targets
(spirv-cross among them) and CMake refuses to export a target whose link interface names something
outside the export set. The archive goes in `CMAKE_EXE_LINKER_FLAGS` instead, which reaches exactly
what needs it. Recorded in the module's own comment so the next person does not rediscover it.

---

## 6. CI

`./build.sh` is the whole of the verification and a human has to remember to run it. It needs the
OpenOrbis toolchain, which is the only interesting part of describing the job.

**Done when:** a push runs build.sh and its checks, and a broken header fails the run.

## PARTLY DONE, 2026-09-17 — and the half that exists is the other half

`.github/workflows/sdk-bundle.yml` cuts a redistributable bundle and runs the offline verify and
the publication gate over it. That is real CI and it is **not** what this item asked for: nothing
runs `./build.sh` on a push, so a broken header still reaches a person rather than a red build.

⚠ The gate found six defects on its first real run, all in the scripts that check a bundle rather
than in a bundle - a `sed 's/^_//'` that truncated every C++ symbol name, a dirty-tree refusal that
deadlocked the gate against itself, a CMake guard placed above `project()` where it could only ever
fire, an archive force-loaded twice, a `grep -q` under `set -o pipefail` that reported the overlay
absent from an image containing 16 of its symbols, and a port build sharing a cache between runs.
Each is recorded where it was found. **What remains for this item is the cheap half**: one job that
runs `./build.sh`.

---

## 7. Retire the `__mmap` binding question

`__mmap`, `__munmap` and `__madvise` are local (`t`) in the linked title. No pre-move binary exists
to compare against, so this is **unverified rather than suspicious**. Either establish what it should
be - a linker-visibility question, answerable on the laptop with a two-object test - or strike the
line from README §8 and stop carrying it.

**Done when:** it is answered or gone. Not carried a third time.

## DONE, 2026-08-19 — answered, and the answer is "correct as it stands"

```
libc.a   11 members reference __mmap/__munmap/__madvise as GLOBAL HIDDEN UND
```

musl declares its internal allocator-to-kernel linkage hidden. An undefined *hidden* reference forces
the definition that satisfies it to hidden, and a hidden symbol is emitted LOCAL. Nothing we did.

Isolated rather than reasoned about: in the SAME link, `backtrace` and `pthread_create` are
`GLOBAL DEFAULT`, and in a minimal project where nothing from `libc.a` references `__mmap`, ours
stays global. Two controls, one conclusion.

The line is struck from README §8 and the finding written into §2.6b, so it is not carried a third
time.

---

## 8. Upstream, headers first

The point of this repository is to shrink. §7 of the README maps every item to where it belongs.

```
1. the seven broken <orbis/> headers   their own PR - typo'd type names, an undefined type,
                                       clang builtins redeclared. Nothing controversial.
2. the three missing names             malloc_usable_size, sigev_notify_function, ENODATA
2b. the pthread_create wrapper         §1's default-stack fix, once it has run on hardware. libc.a
                                       has no definition today, so this is an addition, not a change
3. the four pthread sizes              measured; extends musl PR #29, corrects fewer types
4. the six SDK-gap headers             machine/*, pthread_np.h, sys/*
5. everything else                     only once 1-4 have landed and the shape is agreed
```

⚠ **Anything sent upstream has to be measured, not inferred.** Every number in README §2 and §6 came
off the console; that is the bar, and it is the reason these are worth their maintainers' time.

## ⚠ UPSTREAM HAS MOVED UNDERNEATH THIS ITEM, 2026-09-17

Three things were true when this list was written and are not now. None of them is a reason to drop
the item; all three change who to send what, and to whom.

* **The `mode_t` diagnosis landed without us.** OpenOrbis/OpenOrbis-PS4-Toolchain PR #278
  (red-prig, merged 2025-07-21) narrows `OrbisKernelMode` to `uint16_t`, and PLAN §3's field table
  matches the result field for field. It corrects only the Orbis-namespaced type, so `stat()` still
  reads the wrong layout and the interposer stays - but the argument for it no longer has to be
  made. ⚠ And it is in the v0.5.4 **tag** only: the published **asset** is the v0.5.3 tree, so
  everyone who pins the asset, which is everyone, does not have it.
* **OpenOrbis/musl PR #35 is open, unreviewed, and lands on §1 and §6.3.** It rewrites `__wait.c`
  onto libkernel's `_umtx_op` - the arm README §6.3 measured as 15-tests-aborted against 49/49 for
  ours - and its author wrote *"I didn't try rebuilding libc++"* and asked for someone to verify the
  definitions. That is an invitation, addressed to the only person who has the measurements.
* **The toolchain is being replaced.** Its maintainer has said since 2024-12 that a BSD libc +
  latest LLVM toolchain is in progress, privately (issue #262). Patches against the musl tree are
  patches against a tree with a successor. ⚠ Nothing public shows the new one; the only artefact is
  `kiwidoggie/llvm-project` @ `release/19.x`, last pushed 2025-07-28.

**What this changes:** item 1 of the list above (the seven broken headers) is still worth sending as
written. Everything touching musl should be a question first - on their Discord, where the offer of
access was made - rather than a PR against a tree nobody upstream is building on any more.

---

## 9. Make SIGEV_THREAD actually work, in userspace, the way glibc does

**Only after §8.** This is the expensive route, written down so it stops being an idea and starts
being a decision someone can take later.

`SIGEV_THREAD` is a userspace construction on glibc too - the kernel does not deliver callbacks, a
helper thread does. Here the ingredients are all present and measured:

```
a SIGEV_NONE timer     counts down correctly            proven, §2
threads                work, and now with a real stack  proven, §1
CLOCK_REALTIME         is the clock timer_create takes  proven, §2
```

**The shape:** interpose `timer_create`; when `sigev_notify == SIGEV_THREAD`, create the underlying
timer with `SIGEV_NONE`, keep the callback and its `sigval` in a record, and run a helper thread that
sleeps to the expiry and calls back - repeating on `it_interval`. Because the returned handle is then
ours rather than the kernel's, `timer_settime`, `timer_gettime`, `timer_delete` and
`timer_getoverrun` have to be interposed with it and mapped through.

**What it buys:** dEQP's upstream POSIX timer arm works, so the `deTimer.c` patch disappears instead
of being explained; and every consumer of this SDK gets a notification type the platform advertises
in its own header and does not implement.

**What it costs:** five interposed functions and a thread per timer, to replace a patch that is five
lines in one consumer. ⚠ That trade is the reason this is item 9 and not item 3 - and the reason it
must not start until the cheap items are finished.

**The cheap alternative, if this is never done:** interpose `timer_create` alone and have it return
`-1`/`ENOTSUP` for `SIGEV_THREAD`. Twenty lines, and it turns an unrecoverable hang into a failure
that portable code already handles - dEQP throws `NotSupportedError` and moves on. **A hang cannot be
handled by anybody.**

## DONE, 2026-08-19 — built, and the second attempt was the one that worked

```
countdown, SIGEV_NONE     rc 0, expired               the pass-through
SIGEV_THREAD one-shot     fired 1 after 100 ms
SIGEV_THREAD interval     fired 5 in 300 ms @ 50 ms   overrun 0
```

All five calls are interposed over `ktimer_*`. Handles for SIGEV_THREAD are pointers into a static
table of eight, recognised by a range check rather than a tag bit, so musl's internal encoding (a
pointer with the sign bit set, kernel id at offset 0xa0 of its thread structure) never has to be
imitated. Ordinary timers go to `ktimer_create` and come back as the plain kernel id, which IS musl's
encoding for them.

⚠ **THE FIRST ATTEMPT BROKE timer_create FOR THE WHOLE PORT**, exactly the risk this item was
declined over an hour earlier. Passing the SDK's public `struct sigevent` to `ktimer_create` returns
EINVAL for every notification type. musl translates, and the translation is a REORDERING, not a
renumbering - read out of its disassembly rather than guessed the second time:

```
ksev+0x00 = value    (SDK offset 8)      ksev+0x0c = notify   (SDK offset 0, UNCHANGED)
ksev+0x08 = signo    (SDK offset 4)      ksev+0x10 = 0
```

My first hypothesis was FreeBSD-vs-musl numbering, and the log already refuted it: EINVAL for BOTH
types, which a renumbering alone cannot explain. **The boot probe caught the regression in one run
and the title still played** - which is the only reason putting this repository on the path of every
timer in the port is defensible.

⚠ **AND THE PROBE'S FIRST VERDICT WAS WRONG.** It said the CTS patch could go after one shot. dEQP's
deTimer.c is a PERIODIC watchdog; repeating is a different code path. The interval control was added
before anything was deleted, and only then did the patch go.

---

## 10. The env-file list names its consumers, and cannot keep doing that

`src/orbis_env.cpp` reads `/data/orbis-env.txt` and then three paths belonging to named products.
The file's own header calls it *"a seam rather than a design"* and it is right. The constraint
underneath is real and is not going away: `setenv()` in one image is invisible to another here,
because the SDK's `libc.a` is a real static musl and every `.prx` links its own `environ` - measured
2026-08-23, `ORBIS_NCPU=1` applied to the eboot and never reached the core.

So a well-known path is the only channel between images; an `orbis_env_add_file()` API cannot work,
because nobody calls it inside a module before its first read. `/data/orbis-env.txt` (added
2026-09-17) is that path.

**Done when:** the three product paths are gone, because Tempest and RetroArch write the generic one.
Until then they stay for compatibility and are marked as such.

**2026-09-18 - both products now read the generic name; the deletion waits on releases.** OpenGothic
(`game/main.cpp`) and RetroArch (`frontend/drivers/platform_orbis.c`) both apply `/data/orbis-env.txt`
FIRST and their own file after it, which keeps the frontend's order and `orbis_env.cpp`'s identical,
and `OpenGothic/ps4/tempest-env*.txt` - the normative description of the format - now names the
generic path as the destination. The three paths here are marked deprecated with that date. They can
only go once a RELEASED package of each product carries the change: the operator's file sits on
`/data`, no reinstall touches it, and dropping a read early turns an existing knob into a silent
no-op - the same failure this section exists for. `/data/retroarch-glcore-env.txt` needs no release
at all: the desktop-GL eboot is retired (RTRG00001 and `/data/retroarch-glcore/` are gone,
`ps4/build-cores.sh`) and nothing in the RetroArch tree has written or read that path since.

## 11. `ORBIS_*` knobs that could not be set on a console

⚠ **Two of this overlay's three switches read `getenv` and not `orbis_env_get`, so the A/B method
every measurement in the README depends on worked only on the laptop.** Found 2026-09-17 while
trying to bisect a crash; both now go through the file-aware path. `ORBIS_INTERNAL_MEM_PROBE` always
did, which is why nobody noticed.

**Done when:** a check fails if a new `getenv("ORBIS_...")` appears in `src/`. The failure mode is
silent by construction - a switch that cannot be thrown looks exactly like a switch with no effect.

## 12. `crtlib.o`, and the one licence question left

`crt/` is an MIT C runtime and `ORBIS_CRT` selects it. The reason it exists shrank on inspection:
`crt1.o`, `crti.o`, `crtn.o` and `crt_dyn.o` are built from `OpenOrbis/musl` (`crt/ps4/crt1.c`,
`arch/ps4/crt_arch.h`), which is **MIT** - so they were never the problem. Only `crtlib.o`, from the
toolchain repository's `src/crt/crtlib.c`, is GPL-3.0 with no per-file header and no linking
exception, and it reaches only `.prx` modules.

⚠ **And ours fixes a real defect, not just a licence one**: the SDK's `crtlib.o` resolves
`__init_array_start`/`__init_array_end` into its own `.bss` - measured on a linked module,
`0xc030`/`0xc038` inside `.bss` against a real `.init_array` at `0x4000` - so `module_start` walks
one zeroed entry and calls through NULL. Static constructors in a `.prx` never run. Cause: tentative
definitions that were COMMON under `-fcommon` and are ordinary objects under clang 18.

**Done when:** OpenOrbis answers whether `src/crt/crtlib.c` was meant to be GPL-3.0 and whether they
would add a linking exception. One message. The `.init_array` defect should be sent either way.

---

## 13. This repository has two poles, and one of them is a porting kit

Measured 2026-09-18, by file:

| bucket | what | lines | direction |
|---|---|---|---|
| core | `orbis_env` 185, `orbis_log` 62 | 247 | substrate; both poles call it |
| compat | `include/` (27 files), `orbis_stat` 197, `orbis_sigev` 325, `orbis_clock` 234, `orbis_timer` 196, `orbis_thread` 506, `orbis_sysconf` 27, `crt/` | ~1700 | **shrinks** - §8 maps each item upstream |
| kit | `vkloader/` 9418, `cmake/` + `scripts/` 1594, `orbis_boot` 552, `orbis_mem` 463, `orbis_paths` 259, `ps4_app` 294, `orbis_netlog` 85, `orbis_bigheap` 38 | ~12700 | **grows** |

The kit bucket is already the majority of this repository by volume, under a name that says
"compat", and the README opens with *"This repository may shrink one day."* Those two sentences
cannot both stay true of one repository.

⚠ **It does not cut cleanly in two.** `orbis_thread`, `orbis_sigev` and `orbis_mem` call
`orbis_env_get`; those three plus `orbis_mmap` and `orbis_clock` call `orbis_log`. So env and log
are not kit material, they are the substrate under both - a two-way cut would point that dependency
backwards and fail halfway. `orbis_mmap` (508) sits on the line: part musl gap, part this console's
memory policy. Read it before assigning it.

**`orbis-ports/orbis-porting-kit` exists, private**, and carries `vkloader/` and `cmake/` as
VERBATIM COPIES, not moves - every port keeps building against this repository exactly as before,
and a wrong boundary can be abandoned without touching them. `vkloader/` went first because it is
the cleanest cut in the tree: it includes `<orbis/libkernel.h>` and Mesa's headers and no overlay
API at all.

Proven there on 2026-09-18: the triangle example builds with the kit's own toolchain file and
vkloader; and **OpenGothic, unmodified, at its own pinned commit, builds and packages** against a
`composed/` tree that symlinks this repository's checkout EXCEPT `cmake/` and `vkloader/`, which
point into the kit. By absence rather than by shadowing, so anything reaching back for the
overlay's copies fails instead of quietly succeeding. Result: `IV0000-TMPS10021_00-TEMPESTOPENGOTHI.pkg`,
51 MB, and the port's checkout came back clean.

**Done when:** this repository stops shipping `vkloader/` and `cmake/`, and the copies in the kit
become the only ones. That is the expiry date on `scripts/copied-files.txt` over there; until then
its `check-copies.sh` compares byte for byte against the checkout each build used, so drift is a red
build rather than a discovery. The disease being prevented is measured: the `orbis-toolchain`
composite action lived in three repositories and had reached 317 lines against 332 with different
cache keys before anyone read them side by side.

**Not yet moved, and why:** `orbis_boot`, `orbis_paths`, `ps4_app`, `bigheap` and `netlog` all call
`orbis_log`/`orbis_env_get`, so they wait until the core bucket is named. `scripts/release/` belongs
to the kit by rights but was hardened with 27 tests the same week; forking it the next day would
trade a real safeguard for a tidy diagram.

**The kit's own backlog, ranked by what the ports measured**, not by what seemed likely:

1. one published `setup-orbis` and a `workflow_call` workflow - deletes the three drifted copies
2. one version string in place of three pins (SDK tag, overlay sha, Mesa release)
3. fold sonic3air's runtime shims in - `orbis_wchar32.c` 1515, `orbis_cxa_guard.c` 329,
   `orbis_thread_atexit.c` 109 against this repository's 29-line **stub**, `orbis_cv_fix.cpp` 115,
   `orbis_abort_report.c` 172. None of it is about Sonic and all of it lives in a game's build
   directory today.
4. **`orbis_jit`** - an executable-memory arena. 9 of the 30 core patches in RetroArch's
   `ps4/core-patches/` are this one gap (`executable memory for the recompiler`, `for the rsp jit`,
   `for the lightrec code buffer`, `code caches out of text`, `one arena for compiled code`,
   `give the jit code buffer a platform`), beetle-psx carries `orbis_lightrec_mem`, and Panda3DS,
   dynarmic and 3dsTrident are waiting in the org untouched. This is the largest single gap the
   ports have found, and it is not audio or input.
5. the SDL2 orbis backend - video 888, joystick 534, audio 326 - lives inside **sonic3air's
   vendored SDL tree**, which is a plain directory rather than a submodule. Most engines a stranger
   arrives with are SDL2, so this is the widest lever in the whole plan and it is currently sitting
   in one game.

   ⚠ **`orbis-ports/SDL` was not that lever and is archived as of 2026-09-18.** It held exactly one
   commit ahead of upstream - d2f6ea6ff, `&& !defined(__ORBIS__)` on SDL_endian.h's FreeBSD arm -
   and **nothing consumed it**: Panda3DS's submodule points at `libsdl-org/SDL`, and sonic3air
   vendors an UNPATCHED copy that worked because of a private four-line `sys/endian.h` shim in its
   own build directory. Three answers to one question, none aware of the others. `include/sys/endian.h`
   here is the fourth and the last: it is a superset of the shim, the shim is deleted, and the patch
   was never needed by anyone who had this header. Archived rather than deleted because the token
   here cannot delete repositories - `gh auth refresh -h github.com -s delete_repo` first if that is
   what you want.

⚠ **A kit is read by people who did not write it.** `cmake/orbis-tls.ld` is GPL-3.0-only with no
linking exception and is on every consumer's link line; a linker script directs the linker rather
than being linked in, but the question will be asked and the answer has to be written down rather
than inferred. And `__PS4__` vs `__ORBIS__` (Parking) has to be settled before strangers pin either
one.

---

## Parking

Ideas that are real but **not** to be started before §8 is done. Written here so they stop
interrupting.

* interposing `getcwd` so a relative path means something, instead of `orbis_paths` anchoring
* ⚠ `sigaltstack` returns -1 on this platform, so **a stack overflow still dies silently** - the
  alternate stack the crash handler allocates is never installed. Now that the handler lives here
  and threads have 2 MB (§1), finding out whether `sigaltstack` is unimplemented or merely refused
  as called is an overlay question. It has been printed in every run for weeks and read by nobody.
* the GPU stall, one submit in ~1200 - pre-existing, agreed to leave, and not an overlay concern
* ~~`__PS4__` -> `__ORBIS__` across the port (27 files)~~ **ANSWERED 2026-09-18, and it needs no
  rename.** The toolchain file defines `__ORBIS__`, `PS4` and `__PS4__`, so both spellings work and
  always did. Which is *right* is settled by the SDK's own `include/SDL2/SDL_platform.h` - upstream
  SDL: `#if defined(__ORBIS__) || defined(PS4)` / `#undef __PS4__` / `#define __PS4__ 1`. So
  `__ORBIS__` and `PS4` are the inputs an SDK sets and `__PS4__` is SDL's derived output; Sony's own
  SDK sets `__ORBIS__` (and `__PROSPERO__` for PS5), which means an engine already ported to that
  SDK compiles here unchanged. Rule, now in the kit's README and beside the flags: write new code
  and anything upstreamable against `__ORBIS__`, rename nothing, and let the 24 files that test
  `__PS4__` be corrected when they are touched for another reason.
