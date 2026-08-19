// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* Stub. Mesa asks sysctl for the CPU count, the process name and the memory size in four places
   (u_cpu_detect.c, u_process.c, os_misc.c, u_thread.c). Declared so those compile; a PS4 arm should
   answer them from sceKernel* instead, and returning -1 makes Mesa take its own fallbacks. */
#pragma once
#include <sys/types.h>
#define CTL_HW      6
#define HW_NCPU     3
#define HW_PHYSMEM  5
#define HW_USERMEM  6
#define CTL_KERN    1
#define KERN_PROC   14
#define KERN_PROC_PATHNAME 12
/* u_process.c asks the kernel for its own argv through this. The sysctl above always fails, so the
   struct is never filled and the caller takes its documented false path - which is why a size is
   enough and the real layout is not needed. */
#define KERN_PROC_ARGS     7
#define KERN_PROC_ARGV     1
#ifdef __cplusplus
extern "C" {
#endif
static inline int sysctl(const int* name, unsigned namelen, void* old, size_t* oldlen,
                         const void* new_, size_t newlen) {
  (void)name; (void)namelen; (void)old; (void)oldlen; (void)new_; (void)newlen;
  return -1;
  }
static inline int sysctlbyname(const char* name, void* old, size_t* oldlen,
                              const void* new_, size_t newlen) {
  (void)name; (void)old; (void)oldlen; (void)new_; (void)newlen;
  return -1;
  }
#ifdef __cplusplus
}
#endif
