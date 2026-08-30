// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* FIONREAD and its neighbours, with FreeBSD's values.
 *
 * ⚠ TWO SEPARATE PROBLEMS, AND THE SECOND IS THE DANGEROUS ONE.
 *
 * The SDK's <sys/ioctl.h> declares ioctl() and defines none of the FION* requests at all, so any
 * portable code that polls a socket for pending bytes fails to compile - measured on
 * gambatte-libretro's net_serial.cpp, which is a plain `use of undeclared identifier 'FIONREAD'`.
 *
 * ⚠ AND THE OBVIOUS FIX WOULD HAVE BEEN WRONG. The rest of this SDK is musl, whose bits/ioctl.h
 * carries LINUX request numbers - FIONREAD is 0x541B there. This kernel is FreeBSD-derived and
 * encodes its requests differently: direction, length and a two-byte group in the number itself.
 * Handing a Linux request number to a FreeBSD ioctl does not fail cleanly; it names a different
 * request or none.
 *
 * ⚠ AND NOTE WHAT THE SDK DOES ELSEWHERE, because the mmap constants are the counter-example and
 * mistaking them for a parallel case has already cost this port a wasted task. <sys/mman.h> is
 * musl's there too and does carry Linux's MAP_ANON 0x20 - but it ends with
 * `#include <bits/mman.h>`, and that header #undefs MAP_SHARED, MAP_PRIVATE, MAP_FIXED, MAP_ANON
 * and the PROT_* set and gives them FreeBSD's values, so MAP_PRIVATE|MAP_ANON really is 0x1002
 * and orbis_mmap.cpp's static_assert on it passes. The SDK corrected itself for mman. It does
 * NOT for FION*, which it simply never defines - hence this file, and hence the derivation
 * below rather than a copy.
 *
 * The values below are FreeBSD's own, derived rather than copied so the derivation can be checked:
 *
 *     _IOC(inout, group, num, len) = inout | ((len & 0x1fff) << 16) | (group << 8) | num
 *     IOC_OUT  = 0x40000000   (kernel writes to userland)
 *     IOC_IN   = 0x80000000   (kernel reads from userland)
 *     group 'f' = 0x66
 *
 *     FIONREAD = _IOR('f', 127, int) = 0x40000000 | (4 << 16) | (0x66 << 8) | 127 = 0x4004667f
 *     FIONBIO  = _IOW('f', 126, int) = 0x80000000 | (4 << 16) | (0x66 << 8) | 126 = 0x8004667e
 *     FIOASYNC = _IOW('f', 125, int) = 0x80000000 | (4 << 16) | (0x66 << 8) | 125 = 0x8004667d
 *
 * ⚠ UNVERIFIED AGAINST THE KERNEL. Nothing in this workshop has yet called ioctl() on this
 * console. The numbers are FreeBSD's and the derivation is shown, which is as far as reading can
 * take it; the first caller that gets an unexpected answer should suspect this file first.
 */
#ifndef _ORBIS_COMPAT_SYS_IOCTL_H
#define _ORBIS_COMPAT_SYS_IOCTL_H

#include_next <sys/ioctl.h>

#ifndef IOC_VOID
#define IOC_VOID  0x20000000U
#define IOC_OUT   0x40000000U
#define IOC_IN    0x80000000U
#define IOC_INOUT (IOC_IN | IOC_OUT)
#endif

#ifndef _IOC
#define _IOC(inout, group, num, len) \
   ((unsigned long)((inout) | (((len) & 0x1fff) << 16) | ((group) << 8) | (num)))
#define _IO(g, n)      _IOC(IOC_VOID,  (g), (n), 0)
#define _IOR(g, n, t)  _IOC(IOC_OUT,   (g), (n), sizeof(t))
#define _IOW(g, n, t)  _IOC(IOC_IN,    (g), (n), sizeof(t))
#define _IOWR(g, n, t) _IOC(IOC_INOUT, (g), (n), sizeof(t))
#endif

#ifndef FIONREAD
#define FIONREAD _IOR('f', 127, int)
#endif
#ifndef FIONBIO
#define FIONBIO  _IOW('f', 126, int)
#endif
#ifndef FIOASYNC
#define FIOASYNC _IOW('f', 125, int)
#endif

#endif /* _ORBIS_COMPAT_SYS_IOCTL_H */
