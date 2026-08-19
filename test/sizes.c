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
