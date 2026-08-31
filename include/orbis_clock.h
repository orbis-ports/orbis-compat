// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// Corrective interposition of clock_gettime() for PS4 builds.
//
// ---------------------------------------------------------------- the defect
//
// The SDK ships the LINUX clock-id table while `clock_gettime` is libkernel.so's
// entry point into a FreeBSD-derived kernel, and the two tables disagree:
//
//     id   OpenOrbis <time.h>          FreeBSD kernel
//     0    CLOCK_REALTIME              CLOCK_REALTIME
//     1    CLOCK_MONOTONIC             CLOCK_VIRTUAL      <- process USER cpu time
//     2    CLOCK_PROCESS_CPUTIME_ID    CLOCK_PROF
//     4    CLOCK_MONOTONIC_RAW         CLOCK_MONOTONIC
//
// So libc++ asks for CLOCK_MONOTONIC, writes a 1, and is answered with CPU time.
//
// MEASURED (console, 2026-08-06): sceKernelGetProcessTime 20000 us against
// steady_clock 2993 us - 14.96%, and 0.1496 is not a slow clock but a different
// quantity: the loop spent ~85% of its wall time in the kernel, where CLOCK_VIRTUAL
// does not advance. A third, independent witness agreed - the host's own UDP log
// timestamps put 64 frames at 110.4 ms each.
//
// WHAT IT COSTS WHEN ABSENT: anything that spins until a deadline multiplies its
// target by 1/0.1496 = 6.68. In the title that was Application::sleep's busy-spin
// turning a 16 ms menu frame into 108 ms. In the driver it is os_time_get_nano()
// (via timespec_get(TIME_MONOTONIC) -> clock_gettime), util/futex.c's deadlines, and
// VK_KHR_calibrated_timestamps - which is a Vulkan API surface the CTS tests.
//
// ⚠ THE FIX USES NO CLOCK ID AT ALL, deliberately. Translating 1 -> 4 would be a
// second guess layered on the first. sceKernelGetProcessTimeCounter and its stated
// frequency are a counter: monotonic and correctly scaled without an id namespace.
// Which FreeBSD id is really monotonic is left open rather than guessed - one wrong
// memory call has frozen this console before, and a curiosity is not worth that.
#ifndef _ORBIS_CLOCK_H
#define _ORBIS_CLOCK_H

namespace orbis {

/// Reports the clock's basis and then FALSIFIES it on hardware: two ~20 ms windows,
/// one busy and one asleep, with the counter as reference. A CPU-time clock scores
/// ~1.0 busy and ~0.0 asleep whatever its name says, so the pair separates "runs
/// slow" from "measures the wrong quantity" - which need different fixes.
///
/// Runs once; later calls return immediately. Costs ~40 ms and five log lines, and
/// is a no-op when no logger is registered. Safe to call from a frame loop.
void clockProbe();

} // namespace orbis

/* How many times clock_gettime has answered each family. See the block in src/orbis_clock.cpp: this
   is the last uncleared call on the graphics driver's leaking path, and these two numbers are how the
   driver's report can divide bytes lost by clock reads instead of by a guess. Not in a namespace -
   the consumer is C. */
extern "C" void orbis_clock_counts(unsigned long long* monotonic, unsigned long long* realtime);


#endif
