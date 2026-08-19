/* FreeBSD's cpuset_t, for a platform that is a FreeBSD kernel with a musl libc.
 *
 * Mesa's u_thread.c does `#define cpu_set_t cpuset_t` under __FreeBSD__ and then uses CPU_ZERO /
 * CPU_SET / CPU_ISSET and pthread_setaffinity_np. musl HAS all of that - but spelled cpu_set_t and
 * hidden behind _GNU_SOURCE, and turning _GNU_SOURCE on breaks OpenOrbis' own sched.h and alltypes.h
 * (measured: 24 errors). So the type is declared HERE, matching musl's layout, with the macros the
 * three call sites need.
 *
 * `struct cpu_set_t` is the tag musl's pthread.h already references unconditionally
 * (OpenOrbis include/pthread.h:226), so this is the same type that pthread_setaffinity_np expects and
 * not a parallel one.
 *
 * ⚠ THE MACROS ARE FUNCTIONAL BUT THE AFFINITY IS NOT WIRED. Mesa uses this to pin shader-compile
 * threads; it is an optimisation and Mesa runs without it. Making it real is scePthreadSetaffinity and
 * belongs with the rest of the PS4 arm, not in a build shim. */
#pragma once
#include <stddef.h>

#ifndef _GNU_SOURCE
struct cpu_set_t { unsigned long __bits[128/sizeof(long)]; };
#define __CPU_BITS(i)  ((i)/(8*sizeof(unsigned long)))
#define __CPU_MASK(i)  (1UL << ((i)%(8*sizeof(unsigned long))))
#define CPU_SETSIZE    1024
#define CPU_ZERO(s)    do { for(size_t __i=0;__i<sizeof((s)->__bits)/sizeof((s)->__bits[0]);++__i) (s)->__bits[__i]=0; } while(0)
#define CPU_SET(i,s)   ((s)->__bits[__CPU_BITS(i)] |= __CPU_MASK(i))
#define CPU_CLR(i,s)   ((s)->__bits[__CPU_BITS(i)] &= ~__CPU_MASK(i))
#define CPU_ISSET(i,s) (((s)->__bits[__CPU_BITS(i)] & __CPU_MASK(i)) != 0)
#endif

typedef struct cpu_set_t cpuset_t;
