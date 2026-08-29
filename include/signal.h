// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* The two sigevent accessors FreeBSD defines and this SDK omits.
 *
 * The SDK's struct sigevent IS FreeBSD's - signal.h:144 has the union, with the thread-notification
 * function and its pthread_attr_t sitting in _sigev_un._sigev_thread. What is missing is the pair of
 * macros every caller actually writes, so portable code that fills in a SIGEV_THREAD event fails to
 * compile against a structure that could hold it perfectly well. dEQP's deTimer.c is one such caller.
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
 * ⚠ AND THE CTS FORK DOES NOT PATCH deTimer.c, which this comment claimed twice until 2026-08-21.
 * It does not need to: nothing in deqp-vk calls deTimer_*, so the linker drops the whole object and
 * the SIGEV_THREAD arm never reaches the binary (measured - deTimer_* absent from a non-stripped
 * deqp-vk whose deMutex_create and deThread_create are both present). Anything that DOES arm a timer
 * here will hang, and will need its own arm; this header is where the reason is written down.
 *
 * PLAN.md §9 is the route to making SIGEV_THREAD actually work, which is a userspace job here
 * exactly as it is on glibc.
 */
#ifndef _ORBIS_COMPAT_SIGNAL_H
#define _ORBIS_COMPAT_SIGNAL_H

#include_next <signal.h>

/* ⚠ sa_sigaction DOES NOT COMPILE AS THE SDK DEFINES IT, and this is a typo rather than a design.
   signal.h:136 names the union member `__sa_sigaction`; signal.h:142 defines the macro as
   `__sa_handler.sa_sigaction` - one underscore pair short. Every caller that installs a SA_SIGINFO
   handler therefore has to reach through the real member by hand, which is portable code's business
   to not have to know. Redefined here to what the struct actually contains. */
#undef sa_sigaction
#define sa_sigaction __sa_handler.__sa_sigaction

/* ⚠⚠ THE SA_* FLAG VALUES IN THIS SDK ARE LINUX'S, AND THE KERNEL THEY ARE SENT TO IS FreeBSD'S.
 *
 * include/bits/signal.h defines SA_SIGINFO as 4, SA_ONSTACK as 0x08000000 and SA_RESTART as
 * 0x10000000 - the Linux numbers. This kernel is a FreeBSD derivative and reads the same field as:
 *
 *     SA_ONSTACK 0x0001   SA_RESTART 0x0002   SA_RESETHAND 0x0004
 *     SA_NOCLDSTOP 0x0008 SA_NODEFER 0x0010   SA_NOCLDWAIT 0x0020   SA_SIGINFO 0x0040
 *
 * ⚠ SO `sa_flags = SA_SIGINFO` ASKS FOR SA_RESETHAND. The handler is installed as a ONE-SHOT and
 * WITHOUT siginfo, which is exactly what was measured on hardware: a SIGSEGV arrived as
 *
 *     rdi = 11 (the signal)      rsi = 2      rdx = a valid stack address
 *
 * - FreeBSD's ORIGINAL signature, void (*)(int sig, int code, struct sigcontext *scp), with a small
 * integer where a siginfo_t* was expected. Play!'s CEeExecutor::HandleException read si_addr from
 * it (`movq 0x18(%rsi), %rsi`), faulted on address 0x1a, and took the process down.
 *
 * ⚠⚠ AND THIS PORT'S OWN CRASH REPORTER HAS BEEN DYING OF IT SINCE THE DAY IT WAS WRITTEN.
 * src/orbis_boot.cpp installs ps4SignalAction with SA_SIGINFO|SA_ONSTACK and reads info->si_code
 * first thing. A `code` of 2 is not null, so the null check passes and the read faults - inside the
 * SIGSEGV handler, with `reentered` already 1, which goes straight to _Exit(2). The evidence is the
 * absence: "crash handlers installed ... sigaction rc=0" appears in log after log, and
 * "fatal: signal ..." appears in NONE of them, ever.
 *
 * ⚠ SA_ONSTACK IS WRONG THE OTHER WAY and is why the alt stack never took either: 0x08000000 is
 * not a flag this kernel knows, so the request was silently dropped and a stack overflow still
 * died in silence - which the boot log has been reporting as "NO alt stack" all along, blaming
 * sigaltstack's return code rather than the flag.
 *
 * This is the same family as libc++'s ETIMEDOUT being Linux's 110 on a FreeBSD target: headers
 * assembled for one kernel, shipped against another. The numbers below are FreeBSD's, which is
 * what the kernel on the other side of the syscall actually parses. */
#undef  SA_ONSTACK
#define SA_ONSTACK   0x0001
#undef  SA_RESTART
#define SA_RESTART   0x0002
#undef  SA_RESETHAND
#define SA_RESETHAND 0x0004
#undef  SA_NOCLDSTOP
#define SA_NOCLDSTOP 0x0008
#undef  SA_NODEFER
#define SA_NODEFER   0x0010
#undef  SA_NOCLDWAIT
#define SA_NOCLDWAIT 0x0020
#undef  SA_SIGINFO
#define SA_SIGINFO   0x0040
#undef  SA_ONESHOT
#define SA_ONESHOT   SA_RESETHAND
#undef  SA_NOMASK
#define SA_NOMASK    SA_NODEFER

#ifndef sigev_notify_function
#define sigev_notify_function   _sigev_un._sigev_thread._function
#endif
#ifndef sigev_notify_attributes
#define sigev_notify_attributes _sigev_un._sigev_thread._attribute
#endif

#endif /* _ORBIS_COMPAT_SIGNAL_H */
