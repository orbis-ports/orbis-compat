// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* ENODATA, which this platform's errno table does not have.
 *
 * ⚠ THE NUMBERING HERE IS FreeBSD'S, NOT musl'S, and that is the whole point. The SDK ships musl's
 * headers, but bits/errno.h is FreeBSD's table: ECONNREFUSED is 61, EOPNOTSUPP is 45, ELAST is 97.
 * FreeBSD has no ENODATA at all - the STREAMS errors were never adopted - so code written against
 * Linux or against POSIX XSR finds the name missing.
 *
 * ECONNREFUSED is not a guess: it is what Mesa itself picks. src/amd/vulkan/winsys/amdgpu/
 * radv_amdgpu_cs.c does `#define ENODATA ECONNREFUSED` under DETECT_OS_FREEBSD, and this port
 * deliberately does NOT set that macro (see mesa's util/detect_os.h arm), so the in-tree definition
 * never fires and the value had to be passed on the command line as -DENODATA=61 instead. Same
 * value, same reasoning, one place.
 *
 * ⚠ WHAT THIS COSTS: any `#ifdef ENODATA` in any consumer now takes its true arm. That is the point
 * for code that wants the name, and a behaviour change for code that used the absence as a signal.
 * Nothing in this port does - checked - but it is the reason this is a header and not a -D on four
 * command lines, where it would be invisible.
 */
#ifndef _ORBIS_COMPAT_ERRNO_H
#define _ORBIS_COMPAT_ERRNO_H

#include_next <errno.h>

#ifndef ENODATA
#define ENODATA ECONNREFUSED
#endif

#endif /* _ORBIS_COMPAT_ERRNO_H */
