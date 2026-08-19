<!-- Copyright © 2026 Mikołaj Mikołajczyk -->
<!-- SPDX-License-Identifier: MIT -->

# The plan, in order

**One item at a time, top to bottom.** Everything this overlay is *for* is done and confirmed on
hardware; what remains is a finite list, and the reason it is written down is that it kept growing
sideways. An idea that is not on this list goes to §Parking, not into the tree.

An item is finished when its **Done when** line is true. Not before, and not "mostly".

Status: **1 is done bar one deletion that is not mine to make; 2 is next.**

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
is what glibc does with RLIMIT_STACK, so it is not a number this port invented. `ORBIS_THREAD_STACK`
overrides; `0` disables, so an A/B needs no rebuild.

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

⚠ **Until then the patch stays.** A timer that never fires is worse than one that does not build.

---

## 3. What of `struct stat` is left after OpenOrbis/musl PR #35?

`orbis_stat` translates the kernel's layout into the SDK's. PR #35 upstream covers `mode_t`. Whether
it covers the rest has been **written down as a question twice and never answered**.

**The work:** field by field, compare what the interposer corrects against what the PR changes. Some
of the interposer may already be dead weight; the rest is what to send upstream next.

**Done when:** README §7's `struct stat` row names the specific fields that still need us, or the
interposer shrinks.

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

---

## 5. Point the consumers at what already exists

⚠ **Blocked until the three forks are committed.** Tempest, OpenGothic and VK-GL-CTS carry their
half of the wiring uncommitted, and 209 lines were lost that way once already.

```
Tempest/cmake/ps4-openorbis.cmake   include(cmake/orbis-compat.cmake); orbis_compat_target();
                                    orbis_compat_verify(); -include orbis_prefix.h for stdlib.h
VK-GL-CTS deMemory.c                DELETE - include/stdlib.h covers it
```

⚠ `-lc` stays in `CMAKE_<LANG>_STANDARD_LIBRARIES`. Ahead of the overlay it is a
`duplicate symbol: __mmap` link error - measured, on a throwaway project, not predicted.

**Done when:** the toolchain file has no hand-spliced overlay paths left, and all four builds pass.

---

## 6. CI

`./build.sh` is the whole of the verification and a human has to remember to run it. It needs the
OpenOrbis toolchain, which is the only interesting part of describing the job.

**Done when:** a push runs build.sh and its checks, and a broken header fails the run.

---

## 7. Retire the `__mmap` binding question

`__mmap`, `__munmap` and `__madvise` are local (`t`) in the linked title. No pre-move binary exists
to compare against, so this is **unverified rather than suspicious**. Either establish what it should
be - a linker-visibility question, answerable on the laptop with a two-object test - or strike the
line from README §8 and stop carrying it.

**Done when:** it is answered or gone. Not carried a third time.

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

---

## Parking

Ideas that are real but **not** to be started before §8 is done. Written here so they stop
interrupting.

* interposing `getcwd` so a relative path means something, instead of `orbis_paths` anchoring
* the GPU stall, one submit in ~1200 - pre-existing, agreed to leave, and not an overlay concern
* `__PS4__` -> `__ORBIS__` across the port (27 files) - not this repository's call alone
