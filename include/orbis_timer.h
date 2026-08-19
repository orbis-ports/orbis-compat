// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* Does SIGEV_THREAD deliver on this kernel?
 *
 * include/signal.h adds sigev_notify_function and sigev_notify_attributes, because the SDK omits the
 * two macros while shipping FreeBSD's struct sigevent that already has the fields. That makes
 * portable timer code COMPILE. It says nothing about whether the notification arrives, and code that
 * compiles into a timer which never fires is worse than code that does not build - the failure moves
 * from the build log to a hang nobody attributes.
 *
 * dEQP's deTimer.c is the caller that matters: upstream's POSIX arm uses SIGEV_THREAD, our CTS fork
 * cuts that arm out, and the patch cannot be retired on the strength of a header alone.
 *
 * ANSWERED, 2026-08-19, at the cost of two black screens:
 *
 *     SIGEV_NONE countdown     WORKS - 100 ms armed, read at 250 ms, nothing left
 *     CLOCK_MONOTONIC          REFUSED by timer_create; CLOCK_REALTIME is what answered
 *     SIGEV_THREAD             timer_create NEVER RETURNS - not an error, not a silent timer
 *     with our interposer off  hangs identically, so it is the platform and not our pthread_create
 *
 * The second run mattered: libc.a's timer_create calls pthread_create and waits on a barrier, and
 * since the same day that pthread_create is ours. A confound introduced by this repository had to be
 * ruled out before the platform could be blamed. It was ruled out.
 *
 * THE PROBE IS TWO QUESTIONS, not one, because "nothing happened" has three causes and they need
 * separating: the timer never ran, the timer ran but SIGEV_THREAD is not implemented, or the clock
 * this platform accepts is not the one asked for.
 *
 * ---------------------------------------------------------------- and then made to work
 *
 * ⚠ THIS OVERLAY INTERPOSES ALL FIVE TIMER CALLS, WHICH IS A BIGGER COMMITMENT THAN IT LOOKS.
 *
 * SIGEV_THREAD is a userspace construction everywhere - Linux does not call user functions from the
 * kernel either, glibc runs a helper thread - and both ingredients exist here: a SIGEV_NONE timer
 * that counts down, and threads. Two facts decide the shape, and both were found by writing it:
 *
 *   1. THE HANDLE IS musl's, AND ITS ENCODING IS INTERNAL. timer_settime decodes timer_t as
 *          testq %rdi,%rdi ; jns <plain-id> ; movl 0xa0(%rdi,%rdi),%edi ; andl $0x7fffffff,%edi
 *      A SIGEV_THREAD timer_t is a POINTER with the sign bit set, and the kernel id sits at offset
 *      0xa0 inside musl's own thread structure. Mimicking that would bind this repository to a
 *      layout an SDK update can move silently. So the handles here are OURS - pointers into a static
 *      table - and all five calls are interposed so nothing else ever has to decode one.
 *
 *   2. DEFINING timer_create MAKES musl's UNREACHABLE, so ordinary timers must be created here too.
 *      They go to ktimer_create and come back as the plain kernel id, which IS musl's own encoding
 *      for a non-thread timer - the `jns` arm above.
 *
 * ⚠ SO THIS FILE IS ON THE PATH OF EVERY TIMER IN EVERY CONSUMER: Mesa, the title, the CTS. The
 * pass-through is four lines, and timerProbe()'s countdown exercises it on every boot - which is the
 * only reason that is acceptable. If timers ever stop working port-wide, look here first.
 *
 * ⚠ NOT PROMISED: the notification runs on an ordinary thread of ours, one per timer - which is what
 * SIGEV_THREAD means, not a signal context. Overrun counting comes from the kernel timer underneath.
 * ORBIS_SIGEV_THREAD=0 makes timer_create refuse SIGEV_THREAD with ENOTSUP instead, which is still
 * better than the platform's answer: a caller can handle a refusal and cannot handle a hang.
 *
 * ---------------------------------------------------------------- and it works, measured
 *
 *     countdown, SIGEV_NONE     rc 0, expired               the pass-through, on every boot
 *     SIGEV_THREAD one-shot     fired 1 after 100 ms        kernel timer 3 underneath
 *     SIGEV_THREAD interval     fired 5 in 300 ms @ 50 ms   overrun 0
 *     carried after the probe   0 timers, 6 notifications   both torn down cleanly
 *
 * ⚠ THE INTERVAL TEST IS THE ONE THAT MATTERED. dEQP's deTimer.c is a periodic watchdog, and the
 * first verdict this probe printed said the patch could go on the strength of a SINGLE shot. It was
 * wrong to say so; repeating is a different code path and now has its own measurement. The patch is
 * gone from our CTS fork, and libdeutil.a carries `U timer_create` again - upstream's own POSIX arm,
 * resolving here.
 */
#ifndef _ORBIS_TIMER_H
#define _ORBIS_TIMER_H

namespace orbis {

/// Runs both questions and reports through orbis_log. Bounded: it waits at most a second, installs
/// no signal handler, and deletes every timer it creates. A no-op when no logger is registered.
void timerProbe();

/// How many SIGEV_THREAD timers this overlay is carrying, and how many notifications it has
/// delivered. For a consumer that wants to know its watchdog is alive rather than merely created.
void timerCounts(unsigned *live, unsigned long *fired);


}

#endif /* _ORBIS_TIMER_H */
