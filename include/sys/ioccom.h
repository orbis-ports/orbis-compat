// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* FreeBSD's ioctl encoding macros. OpenOrbis' musl does not ship them and Mesa's vendored
   drm-uapi/drm.h needs them for every _IOWR in the amdgpu ABI - 100 of this build's 109 errors.
   These are the plain FreeBSD definitions; nothing here calls an ioctl on this platform, the macros
   only have to expand to the same numbers the ABI headers were written against.

   ⚠ THIS IS THE ONE FILE HERE THAT IS NOT ORIGINAL. The macros below follow FreeBSD's
   sys/sys/ioccom.h, which is BSD-3-Clause; they encode an ABI and cannot be written differently and
   still work. The rest of this repository is MIT. */
#pragma once
#define IOCPARM_SHIFT   13
#define IOCPARM_MASK    ((1 << IOCPARM_SHIFT) - 1)
#define IOCPARM_LEN(x)  (((x) >> 16) & IOCPARM_MASK)
#define IOCBASECMD(x)   ((x) & ~(IOCPARM_MASK << 16))
#define IOCGROUP(x)     (((x) >> 8) & 0xff)
#define IOCPARM_MAX     (1 << IOCPARM_SHIFT)
#define IOC_VOID        0x20000000UL
#define IOC_OUT         0x40000000UL
#define IOC_IN          0x80000000UL
#define IOC_INOUT       (IOC_IN|IOC_OUT)
#define IOC_DIRMASK     (IOC_VOID|IOC_OUT|IOC_IN)
#define _IOC(inout,group,num,len) \
   ((unsigned long)((inout) | (((len) & IOCPARM_MASK) << 16) | ((group) << 8) | (num)))
#define _IO(g,n)        _IOC(IOC_VOID,  (g), (n), 0)
#define _IOWINT(g,n)    _IOC(IOC_VOID,  (g), (n), sizeof(int))
#define _IOR(g,n,t)     _IOC(IOC_OUT,   (g), (n), sizeof(t))
#define _IOW(g,n,t)     _IOC(IOC_IN,    (g), (n), sizeof(t))
#define _IOWR(g,n,t)    _IOC(IOC_INOUT, (g), (n), sizeof(t))
