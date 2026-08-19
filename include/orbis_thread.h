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

}

#endif /* _ORBIS_THREAD_H */
