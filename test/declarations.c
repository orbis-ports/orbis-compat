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

/* Mesa's radv_amdgpu_cs.c, which compares a return code against -ENODATA. */
static int is_no_data(int r)
{
    return r == -ENODATA;
}

int main(void)
{
    void *p = malloc(16);
    struct sigevent ev;
    arm_event(&ev, 0);
    int n = (int)usable(p) + is_no_data(-ENODATA);
    free(p);
    return n > 0 ? 0 : 1;
}
