/* FreeBSD's non-portable pthread extras. Present so meson detects HAVE_PTHREAD_NP_H and u_thread.c
   gets cpuset_t; the affinity calls themselves are declared, not implemented. */
#pragma once
#include <pthread.h>
#include <sys/cpuset.h>
