// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* What the overlay is for, stated as something the compiler checks.
 * Built by build.sh against the real toolchain, and by cmake/orbis-compat.cmake at configure time.
 * No console needed: a wrong include order is a compile error here. */
#include <pthread.h>
#include <stddef.h>

/* Corrected: musl declared these smaller than Sony writes. */
_Static_assert(sizeof(pthread_mutexattr_t) >= sizeof(void *), "pthread_mutexattr_t still undersized");
_Static_assert(sizeof(pthread_condattr_t) >= sizeof(void *), "pthread_condattr_t still undersized");
_Static_assert(sizeof(pthread_barrierattr_t) >= sizeof(void *), "pthread_barrierattr_t still undersized");
_Static_assert(sizeof(pthread_spinlock_t) >= sizeof(void *), "pthread_spinlock_t still undersized");

/* Left alone: prebuilt libc++ embeds these and would disagree. See include/bits/alltypes.h. */
_Static_assert(sizeof(pthread_mutex_t) == 40, "pthread_mutex_t must keep musl's layout");
_Static_assert(sizeof(pthread_cond_t) == 48, "pthread_cond_t must keep musl's layout");
_Static_assert(sizeof(pthread_rwlock_t) == 56, "pthread_rwlock_t must keep musl's layout");

/* Four bytes, and correct at four: Sony's pthread_once writes one byte into it (measured). */
_Static_assert(sizeof(pthread_once_t) == 4, "pthread_once_t changed - re-measure before trusting it");

/* The overlay must not displace the rest of alltypes.h. */
_Static_assert(sizeof(size_t) == 8, "size_t lost");
_Static_assert(sizeof(pthread_t) == 8, "pthread_t lost");

/* The stack_t question, which is a LAYOUT question and not a size one - both orders are 24 bytes.
 * The SDK's type must stay exactly as libc++ and every prebuilt archive saw it (ss_flags at 8),
 * and the shim's type must be FreeBSD's (ss_size at 8), because the whole repair is the swap
 * between them. See include/signal.h. */
#include <signal.h>

_Static_assert(sizeof(stack_t) == 24, "stack_t is no longer 24 bytes - re-derive the shim");
_Static_assert(offsetof(stack_t, ss_flags) == 8, "the SDK's stack_t stopped being Linux-ordered");
_Static_assert(sizeof(struct __orbis_stack_bsd) == 24, "the shim's stack_t must match FreeBSD's");
_Static_assert(offsetof(struct __orbis_stack_bsd, ss_sp) == 0, "ss_sp moved");
_Static_assert(offsetof(struct __orbis_stack_bsd, ss_size) == 8, "ss_size is not where FreeBSD puts it");
_Static_assert(offsetof(struct __orbis_stack_bsd, ss_flags) == 16, "ss_flags is not where FreeBSD puts it");

/* The constants that cross the same syscall, from oracles/freebsd9/sys_sys_signal.h. */
_Static_assert(SS_ONSTACK == 0x0001, "SS_ONSTACK is not FreeBSD's");
_Static_assert(SS_DISABLE == 0x0004, "SS_DISABLE is Linux's 2 again");
_Static_assert(SA_ONSTACK == 0x0001, "SA_ONSTACK is Linux's 0x08000000 again");
_Static_assert(SA_SIGINFO == 0x0040, "SA_SIGINFO is Linux's 4 again - handlers lose siginfo");
