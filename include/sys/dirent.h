/* SPDX-License-Identifier: MIT */
#ifndef _ORBIS_SYS_DIRENT_H
#define _ORBIS_SYS_DIRENT_H

/*
 * <sys/dirent.h> - the BSD spelling, which this target asks for and the SDK does not have.
 *
 * ⚠ THE SAME HOLE AS <sys/endian.h>, AND FOUND THE SAME WAY. clang invoked as
 * --target=x86_64-pc-freebsd12-elf predefines __FreeBSD__ 12, so portable code takes its FreeBSD
 * arm; FreeBSD ships both <dirent.h> and <sys/dirent.h> and this SDK ships only the first.
 *
 * MEASURED 2026-09-18: libretro-common's vfs_implementation.c, compiled for a libretro core -
 *
 *     vfs_implementation.c:62:12: fatal error: 'sys/dirent.h' file not found
 *        62 | #  include <sys/dirent.h>
 *
 * On FreeBSD the split is real: <sys/dirent.h> declares `struct dirent` and the DT_* constants,
 * and <dirent.h> adds the DIR API on top. Here the SDK's <dirent.h> already carries both halves,
 * so this forwards to it rather than restating a layout - two declarations of one struct is how a
 * port gets a readdir that reads the wrong fields.
 */

#include <dirent.h>

#endif /* _ORBIS_SYS_DIRENT_H */
