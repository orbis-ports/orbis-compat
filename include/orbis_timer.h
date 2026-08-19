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
 */
#ifndef _ORBIS_TIMER_H
#define _ORBIS_TIMER_H

namespace orbis {

/// Runs both questions and reports through orbis_log. Bounded: it waits at most a second, installs
/// no signal handler, and deletes every timer it creates. A no-op when no logger is registered.
void timerProbe();

}

#endif /* _ORBIS_TIMER_H */
