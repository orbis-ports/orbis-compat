// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* sysctl for a kernel that does not answer it, and the one query worth answering.
 *
 * Mesa asks sysctl for the CPU count, the process name and the memory size in four places
 * (u_cpu_detect.c, u_process.c, os_misc.c, u_thread.c) and Lightrec's recompiler asks for the
 * CPU count in a fifth. This kernel has no sysctl; the declarations exist so those compile, and
 * everything except hw.ncpu still returns -1 so the caller takes its own documented fallback.
 *
 * ---------------------------------------------------------------- why hw.ncpu is not a stub
 *
 * ⚠ A STUB THAT RETURNS -1 IS NOT NEUTRAL HERE. Every caller's fallback for "how many CPUs" is
 * ONE, and one is the answer that switches off parallelism rather than the answer that
 * degrades it. Measured 2026-08-23: Beetle PSX HW's recompiler reported
 *
 *     [Lightrec]: Threaded recompiler started with 1 workers.
 *
 * on a console with six, because recompiler.c's __FreeBSD__ arm reads
 * `sysctlbyname("hw.ncpu", ...) ? 1 : count` and the stub made it take the `1`. Mesa's
 * u_cpu_detect.c reaches the same conclusion by the same route, for every title.
 *
 * ⚠ AND THE NUMBER IS SIX, NOT A GUESS AT SEVEN. sceKernelGetCpumode() exists and reportedly
 * distinguishes a six-core mode from a seven-core one, but this SDK ships no constants for its
 * return values and this port has never established the mapping. Six is what a homebrew process
 * on this console gets, it is the figure the rest of this workshop already works to, and the
 * difference between six and seven is a rounding error beside the difference between six and
 * one. Guessing at the seventh would be the fourth time a value was taken from a call whose
 * meaning was assumed rather than measured. ORBIS_NCPU overrides it for anyone who wants to
 * establish the mapping, and a run that sets it is an experiment rather than a configuration.
 */
#pragma once
#include <sys/types.h>
#include <stdlib.h>
#define CTL_HW      6
#define HW_NCPU     3
#define HW_PHYSMEM  5
#define HW_USERMEM  6
#define CTL_KERN    1
#define KERN_PROC   14
#define KERN_PROC_PATHNAME 12
/* u_process.c asks the kernel for its own argv through this. That sysctl still fails, so the
   struct is never filled and the caller takes its documented false path - which is why a size is
   enough and the real layout is not needed. */
#define KERN_PROC_ARGS     7
#define KERN_PROC_ARGV     1
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__ORBIS__) || defined(__PS4__)
static inline int orbis_hw_ncpu(void) {
  const char* const e = getenv("ORBIS_NCPU");
  if (e && e[0]) {
    const int n = atoi(e);
    if (n > 0)
      return n;
    }
  return 6;
  }

/* Both spellings, because the two callers that matter use different ones: Lightrec asks
   by name and Mesa builds a {CTL_HW, HW_NCPU} mib. Answering only one would leave the
   other on its one-CPU fallback and the fix would look half-applied. */
static inline int orbis_sysctl_ncpu(void* old, size_t* oldlen) {
  int n;
  if (!old || !oldlen || *oldlen < sizeof(int))
    return -1;
  n = orbis_hw_ncpu();
  __builtin_memcpy(old, &n, sizeof(n));
  *oldlen = sizeof(n);
  return 0;
  }
#endif

static inline int sysctl(const int* name, unsigned namelen, void* old, size_t* oldlen,
                         const void* new_, size_t newlen) {
  (void)new_; (void)newlen;
#if defined(__ORBIS__) || defined(__PS4__)
  if (name && namelen >= 2 && name[0] == CTL_HW && name[1] == HW_NCPU)
    return orbis_sysctl_ncpu(old, oldlen);
#endif
  (void)name; (void)namelen; (void)old; (void)oldlen;
  return -1;
  }
static inline int sysctlbyname(const char* name, void* old, size_t* oldlen,
                              const void* new_, size_t newlen) {
  (void)new_; (void)newlen;
#if defined(__ORBIS__) || defined(__PS4__)
  if (name && __builtin_strcmp(name, "hw.ncpu") == 0)
    return orbis_sysctl_ncpu(old, oldlen);
#endif
  (void)name; (void)old; (void)oldlen;
  return -1;
  }
#ifdef __cplusplus
}
#endif
