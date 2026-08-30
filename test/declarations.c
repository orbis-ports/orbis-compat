// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* The three names this overlay adds to headers the SDK already ships.
 *
 * Each one is here because a real consumer failed without it, and each is written the way that
 * consumer writes it - through <stdlib.h> rather than <malloc.h>, through the macro rather than the
 * union member. Compiled against the overlay it passes; compiled without it, it must not.
 */
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

/* dEQP's deMemory.c, which reaches for it through <stdlib.h> because clang says __FreeBSD__. */
static size_t usable(void *p)
{
    return malloc_usable_size(p);
}

/* dEQP's deTimer.c, filling in a SIGEV_THREAD event. See the warning in include/signal.h: this is a
   compile-time fact and says nothing about whether the console delivers the notification. */
static void arm_event(struct sigevent *ev, void (*fn)(union sigval))
{
    ev->sigev_notify          = SIGEV_THREAD;
    ev->sigev_notify_function = fn;
}

/* Any caller installing a SA_SIGINFO handler. The SDK's own macro is one underscore pair short of
   the union member it names, so this is a compile error without the overlay. */
static void on_signal(int sig, siginfo_t *info, void *uctx)
{
    (void)sig; (void)info; (void)uctx;
}

static int install(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = on_signal;
    sa.sa_flags     = SA_SIGINFO;
    return sigaction(SIGSEGV, &sa, 0);
}

/* Mesa's radv_amdgpu_cs.c, which compares a return code against -ENODATA. */
static int is_no_data(int r)
{
    return r == -ENODATA;
}

/* sigaltstack, which the overlay replaces with a layout-translating shim. Written the way a caller
 * writes it - by the POSIX name, with the tag still usable as a type - because the shim is a
 * function-LIKE macro and an object-like one would have rewritten `struct sigaltstack` too. */
static int alt_stack(void *buf, size_t n)
{
    struct sigaltstack ss;
    ss.ss_sp    = buf;
    ss.ss_size  = n;
    ss.ss_flags = 0;
    return sigaltstack(&ss, (stack_t *)0);
}

int main(void)
{
    void *p = malloc(16);
    struct sigevent ev;
    arm_event(&ev, 0);
    int n = (int)usable(p) + is_no_data(-ENODATA) + (install() == 0 ? 0 : 0)
          + (alt_stack(p, 16) == 0 ? 0 : 0);
    free(p);
    return n > 0 ? 0 : 1;
}
