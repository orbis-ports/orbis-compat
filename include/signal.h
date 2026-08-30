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

/* ⚠⚠ AND stack_t's LAYOUT IS LINUX'S TOO, WHICH IS WHY sigaltstack RETURNS -1 ON THIS CONSOLE.
 *
 * The SA_* values above were the first half of this defect and fixing them did NOT make
 * sigaltstack succeed, which is what sent the search here. Same header, same provenance,
 * same mistake one field over.
 *
 *   include/bits/signal.h:91      struct sigaltstack { void *ss_sp; int ss_flags; size_t ss_size; };
 *   FreeBSD 9 sys/sys/signal.h    typedef struct sigaltstack {
 *     :358-365, the base Orbis OS      char *ss_sp; __size_t ss_size; int ss_flags; } stack_t;
 *     is built on
 *
 * Twenty-four bytes either way, with ss_size and ss_flags EXCHANGED. So a caller that fills the
 * SDK's struct the obvious way - ss_sp = buffer, ss_size = 65536, ss_flags = 0 - hands the kernel
 *
 *     ss_size  = 0            (read out of the SDK's ss_flags plus its padding)
 *     ss_flags = 0x00010000   (the low half of the SDK's ss_size)
 *
 * and FreeBSD's kern_sigaltstack rejects any bit outside SS_DISABLE with EINVAL, in a test that
 * runs BEFORE it ever looks at ss_size. INFERRED, not measured: the order of those two checks is
 * read from FreeBSD's kern_sig.c, which is not among the oracles this port has a copy of. The
 * struct layout, SS_*, SA_* and the errno numbers below ARE measured, from
 * oracles/freebsd9/sys_sys_signal.h and sys_sys_errno.h.
 *
 * SS_DISABLE is wrong the same way: 2 is Linux's, 0x0004 is what this kernel tests for. SS_ONSTACK
 * is 1 on both and needs nothing.
 *
 * ⚠ SIGSTKSZ IS ALSO LINUX'S AND IS DELIBERATELY LEFT ALONE. The SDK says 8192; FreeBSD 9 says
 * MINSIGSTKSZ + 32768 = 34816. Nothing rejects the smaller number - the kernel enforces only
 * MINSIGSTKSZ, which the SDK happens to have right at 2048 - so correcting it would only grow
 * every `char buf[SIGSTKSZ]` in every consumer, on a console whose .bss is already a subject of
 * its own. Anyone who wants the recommended size should ask for it by number and say why.
 *
 * The repair is a translating shim rather than a redeclared struct: `stack_t` is already typedef'd
 * by the SDK header above and libc++ and every prebuilt archive were compiled against it, so the
 * type stays exactly as it was and only the twenty-four bytes that cross the syscall boundary are
 * reordered. A function-LIKE macro, so that `struct sigaltstack` as a type name still means what
 * it says - an object-like macro would silently rewrite the tag as well.
 */
#undef  SS_DISABLE
#define SS_DISABLE 0x0004

struct __orbis_stack_bsd {
        void         *ss_sp;
        __SIZE_TYPE__ ss_size;
        int           ss_flags;
        int           __orbis_pad;
};

/* The real symbol, which comes from libkernel.so - the SDK's own libc.a compiles sigaltstack.c to
 * an object with an EMPTY .text and no symbols at all, so there is no libc wrapper in between and
 * nothing else to blame for the return code. `errno` is meaningful here for a reason worth writing
 * down: libc.a's __errno_location is a single `jmp __error`, so the application's errno IS
 * libkernel's errno rather than a musl-side copy that nothing ever writes. */
extern int __orbis_sigaltstack_kernel(const struct __orbis_stack_bsd *__ss,
                                      struct __orbis_stack_bsd *__oss) __asm__("sigaltstack");

static __inline int __orbis_sigaltstack(const stack_t *__ss, stack_t *__oss)
{
        struct __orbis_stack_bsd __in;
        struct __orbis_stack_bsd __out;
        int __rc;

        __in.ss_sp = 0; __in.ss_size = 0; __in.ss_flags = 0; __in.__orbis_pad = 0;
        __out = __in;

        if (__ss != 0) {
                __in.ss_sp    = __ss->ss_sp;
                __in.ss_size  = __ss->ss_size;
                __in.ss_flags = __ss->ss_flags;
        }

        __rc = __orbis_sigaltstack_kernel(__ss  != 0 ? &__in  : 0,
                                          __oss != 0 ? &__out : 0);

        if (__oss != 0) {
                __oss->ss_sp    = __out.ss_sp;
                __oss->ss_size  = __out.ss_size;
                __oss->ss_flags = __out.ss_flags;
        }
        return __rc;
}

#define sigaltstack(ss, oss) __orbis_sigaltstack((ss), (oss))

#ifndef sigev_notify_function
#define sigev_notify_function   _sigev_un._sigev_thread._function
#endif
#ifndef sigev_notify_attributes
#define sigev_notify_attributes _sigev_un._sigev_thread._attribute
#endif

#endif /* _ORBIS_COMPAT_SIGNAL_H */
