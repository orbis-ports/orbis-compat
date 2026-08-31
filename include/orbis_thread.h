// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* What this platform gives a thread that did not ask for anything.
 *
 * ⚠ THE DEFAULT THREAD STACK HERE IS 65536 BYTES, and a Vulkan pipeline compile does not fit in it:
 * a dEQP worker died inside radv_graphics_shaders_compile with about 72 KB of frame under it. The
 * number is neither libc's nor the kernel family's - FreeBSD's libthr uses 2 MB on 64-bit and musl
 * 128 KiB - because pthread_create is UNDEFINED in every member of the SDK's libc.a and comes from
 * libkernel.so:0xd a78, so the default is Sony's.
 *
 * MEASURED ON THE CONSOLE, 2026-08-19, all four with rc 0:
 *
 *     a freshly pthread_attr_init'd attr claims      65536 B
 *     the MAIN thread, created by the loader       2097152 B
 *     a thread created with attr = NULL              65536 B
 *     a thread created with a default-init attr      65536 B
 *     PTHREAD_STACK_MIN                                2048
 *
 * ⚠ THE MAIN THREAD GETS 2 MB - which is exactly FreeBSD's THR_STACK_DEFAULT for 64-bit. So the
 * platform knows a sane number and applies it to the first thread; scePthreadCreate then hands every
 * other thread a thirty-second of it.
 *
 * ⚠ A FRESH attr REPORTS THE DEFAULT, so "the caller asked for 65536" and "the caller asked for
 * nothing" are indistinguishable. The policy below overrides both, because the consequence is
 * one-directional: a caller that genuinely wanted 64 KiB gets more stack and still works, while the
 * opposite mistake is a crash in a shader compile. Both creation shapes give the same number, so
 * there is one policy rather than two.
 *
 * THE POLICY: a thread that did not choose gets what the MAIN thread has - measured at runtime, not
 * hardcoded, so it follows the platform rather than a number someone picked. ORBIS_THREAD_STACK
 * overrides it: a size in KiB, or 0 to interpose nothing.
 */
#ifndef _ORBIS_THREAD_H
#define _ORBIS_THREAD_H

#include <stddef.h>

namespace orbis {

/// Runs the probe and reports through orbis_log. Cheap, and safe to call before the threads that
/// matter exist: it creates two of its own, joins them, and touches nothing else.
///
/// A no-op when no logger is registered - there is no point measuring for nobody.
void threadStackProbe();

/// The default stack size the probe observed, or 0 before it has run.
size_t threadDefaultStack();

/// What the interposer gives a thread that did not choose, in bytes; 0 when it is disabled.
size_t threadStackFloor();

/// How many threads have been created through the interposer, and how many it raised. Reported
/// beside the memory census, because at this size the answer is also a memory number.
void threadCounts(unsigned long *created, unsigned long *raised);

/* ⚠ AND HOW MANY DID NOT HAPPEN, WHICH IS THE HALF THIS FILE USED TO LOSE. The census above prints
 * at powers of two and only when the create SUCCEEDED, so a process whose pool has run out prints
 * nothing at all and the log reads as though nothing had asked for a thread again. It has read that
 * way three times in this port: 2026-08-28 (mupen64plus-next), 2026-08-28 (Play!) and 2026-08-31
 * (melonDS DS), each with thousands of `[ScePthread/System] Internal Memory is running out` in the
 * console's own klog and not one line from here.
 *
 * ⚠ AND THE ATTR COUNT IS THE ONE TO READ FIRST. scePthreadAttrInit draws on the SAME pool and is
 * asked for before the thread is, so it fails first. If attrs are failing and creates are not, the
 * pool is going but is not yet empty - which is the only window in which anything can be measured.
 * If NEITHER moves while the console logs the pool running out, the objects being leaked are not
 * threads and not attrs: they are mutexes, condvars or keys, and the leak is somewhere this file
 * cannot see - which is a narrow, useful answer rather than an absence. */
void threadFailures(unsigned long *createFailed, unsigned long *attrFailed);

/* ------------------------------------------------------------------ the alternate signal stack
 *
 * ⚠ AN ALTERNATE SIGNAL STACK IS PER THREAD, AND UNTIL NOW ONLY THE MAIN THREAD HAD ONE.
 * orbis::installCrashHandlers() installs the SIGSEGV disposition, which FreeBSD keeps in the
 * PROCESS, and one 64 KiB alternate stack, which FreeBSD keeps in td_sigstk, per THREAD. So a
 * worker that overflowed its stack still faulted while the kernel tried to push a signal frame
 * onto the stack that was already exhausted, and the process died with nothing written down -
 * exactly the silence the main-thread fix was meant to end. A core's own thread is the one that
 * matters: melonDS's renderer and flycast's emulator thread are where a deep recursion lives.
 *
 * THE STACK COMES OUT OF THE THREAD'S OWN STACK, AND THAT IS THE WHOLE MEMORY ARGUMENT.
 * The interposer above already raises every thread to 2048 KiB. The trampoline declares a
 * 64 KiB array in the OUTERMOST frame it owns and hands the kernel that, so:
 *
 *   * NOTHING IS ALLOCATED. No malloc during thread start, no pool to size, nothing served out of
 *     the flexible pool this console measures at ~417 MiB, and nothing to compete with the direct
 *     memory carve-outs orbis_mmap.cpp hands musl. The 64 KiB was already reserved by the
 *     scePthreadCreate the line above it - a thread that used to have 2048 KiB of usable stack
 *     now has 1984 KiB.
 *   * NOTHING LEAKS. A per-thread heap buffer would need a free path, and the only free path for a
 *     thread that exits without joining is a TSD destructor - machinery this port has not verified
 *     runs on this kernel. A stack array is reclaimed by the thread's own exit, for free.
 *   * IT IS AT THE FAR END FROM THE GUARD PAGE. The overflow that this exists to report happens at
 *     the LOW end of the thread stack; the array sits above every frame the thread body will ever
 *     push, so the one region the handler needs is the one region the overflow cannot have reached.
 *     FreeBSD computes the handler's entry as ss_sp + ss_size and grows DOWN, which keeps it inside
 *     the array.
 *
 * ⚠ IT IS SKIPPED FOR A THREAD WITH LESS THAN 256 KiB OF STACK, and that case is real: the
 * interposer stands down entirely under ORBIS_THREAD_STACK=0, and a caller may ask for a small
 * stack on purpose. Taking a quarter of a stack to report on the other three quarters is a worse
 * trade than reporting nothing, so below that threshold the trampoline gets out of the way.
 */

/// How many threads installed an alternate signal stack of their own, how many were refused by the
/// kernel, and how many were passed over for having too little stack to spare.
void threadAltStacks(unsigned long *installed, unsigned long *failed, unsigned long *skipped);

/// The size of the alternate signal stack each thread gets, in bytes.
size_t threadAltStackSize();

}

#endif /* _ORBIS_THREAD_H */
