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
 * ⚠ THIS MAKES THE CODE COMPILE. IT DOES NOT PROVE THE CONSOLE DELIVERS. timer_create is real here -
 * 0x1a1 bytes in libc.a, calling ktimer_create - but whether SIGEV_THREAD spawns anything on this
 * kernel is UNMEASURED. Until it is, a caller that switches to the POSIX timer on the strength of
 * this header may get a timer that never fires, which is worse than not compiling. The CTS patch
 * stays where it is.
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
