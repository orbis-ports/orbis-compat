// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* sysconf(_SC_NPROCESSORS_ONLN), which this libc answers with one on a six-core console.
 *
 * ⚠ THE SECOND HALF OF THE hw.ncpu PROBLEM, and it had to be found separately because the two
 * callers ask different questions. Lightrec's recompiler asks sysctlbyname("hw.ncpu") - that is
 * sys/sysctl.h in this directory. RetroArch's cpu_features_get_core_amount() asks
 * sysconf(_SC_NPROCESSORS_ONLN) (features_cpu.c:927), and the Information tab then reports "1
 * core" on hardware with six.
 *
 * The SDK's libc DOES export sysconf and it is musl's, which answers _SC_NPROCESSORS_ONLN out of
 * sched_getaffinity - a Linux syscall this kernel does not have. So the call is well formed, the
 * answer is a refusal, and every caller's fallback for "how many CPUs" is one. Same shape as the
 * sysctl stub returning -1, same consequence: the answer that switches parallelism off rather
 * than the answer that degrades it.
 *
 * ⚠ A MACRO AND NOT A REDEFINED SYMBOL. Defining our own sysconf() would shadow libc's by link
 * order, which works until something links this archive in a different order and silently gets
 * musl's again. A macro is resolved at the call site, in the source that includes this header,
 * and leaves libc's symbol exactly where it was - orbis_sysconf forwards everything it does not
 * answer straight back to it.
 */
#ifndef _ORBIS_COMPAT_UNISTD_H
#define _ORBIS_COMPAT_UNISTD_H

#include_next <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

long orbis_sysconf(int name);

#ifdef __cplusplus
}
#endif

/* After the declaration above, so it declares orbis_sysconf and not a recursion. Any header
 * included later that redeclares sysconf will declare orbis_sysconf instead, which is the same
 * signature and the function this overlay defines. */
#define sysconf orbis_sysconf

#endif /* _ORBIS_COMPAT_UNISTD_H */
