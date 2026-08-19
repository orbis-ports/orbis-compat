// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* The two sigevent accessors FreeBSD defines and this SDK omits.
 *
 * The SDK's struct sigevent IS FreeBSD's - signal.h:144 has the union, with the thread-notification
 * function and its pthread_attr_t sitting in _sigev_un._sigev_thread. What is missing is the pair of
 * macros every caller actually writes, so portable code that fills in a SIGEV_THREAD event fails to
 * compile against a structure that could hold it perfectly well. dEQP's deTimer.c is one such caller
 * and carries a patch in our CTS fork for exactly this reason.
 *
 * ⚠⚠ THESE MACROS MAKE CODE COMPILE INTO A HANG, AND THAT IS MEASURED, NOT FEARED.
 * timer_create with sigev_notify = SIGEV_THREAD NEVER RETURNS on this kernel. Not an error code,
 * not a timer that stays silent: the calling thread does not come back, the title never reaches its
 * menu, and the klog records no fault because nothing faults. Measured 2026-08-19, twice, and the
 * second time with our own pthread_create interposer DISABLED - libc.a's timer_create calls
 * pthread_create and then waits on a barrier, so that had to be ruled out before blaming Sony.
 *
 * The macros stay because they are CORRECT - they name fields the SDK's own struct sigevent really
 * has, and code using SIGEV_SIGNAL or SIGEV_NONE needs them. What is broken is the delivery of one
 * notification type, and no header can fix that.
 *
 * WHAT WORKS, same probe, same run: a SIGEV_NONE timer counts down properly - 100 ms armed, read at
 * 250 ms, nothing left. And timer_create REFUSES CLOCK_MONOTONIC; CLOCK_REALTIME is what answered.
 *
 * dEQP's deTimer.c patch in our CTS fork therefore stays permanently, with this citation. PLAN.md §9
 * is the route to making SIGEV_THREAD actually work, which is a userspace job here exactly as it is
 * on glibc.
 */
#ifndef _ORBIS_COMPAT_SIGNAL_H
#define _ORBIS_COMPAT_SIGNAL_H

#include_next <signal.h>

#ifndef sigev_notify_function
#define sigev_notify_function   _sigev_un._sigev_thread._function
#endif
#ifndef sigev_notify_attributes
#define sigev_notify_attributes _sigev_un._sigev_thread._attribute
#endif

#endif /* _ORBIS_COMPAT_SIGNAL_H */
