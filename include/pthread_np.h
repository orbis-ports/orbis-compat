// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* FreeBSD's non-portable pthread extras. Present so meson detects HAVE_PTHREAD_NP_H and u_thread.c
   gets cpuset_t; the affinity calls themselves are declared, not implemented.

   pthread_attr_get_np is declared because libkernel EXPORTS it (0xd88e) and nothing declares it:
   musl has no such function, so a caller that wants to ask a RUNNING thread about its stack - which
   is the only way to learn what this platform's default really is - has no prototype to call. */
#pragma once
#include <pthread.h>
#include <sys/cpuset.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Fills an already-initialised attr with the attributes of a live thread. FreeBSD's spelling of
/// glibc's pthread_getattr_np, and the one this platform exports.
int pthread_attr_get_np(pthread_t, pthread_attr_t *);

#ifdef __cplusplus
}
#endif
