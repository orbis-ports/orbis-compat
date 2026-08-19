// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* malloc_usable_size, where a FreeBSD program looks for it.
 *
 * This libc is musl, which declares it in <malloc.h> - and the SDK does too, malloc.h:19. FreeBSD
 * puts it in <stdlib.h>, so portable code that gates on __FreeBSD__ (which clang defines for this
 * triple) includes <stdlib.h>, finds nothing, and fails. dEQP's deMemory.c is exactly that shape.
 *
 * Declaring it in both places costs nothing: it is the same function in the same libc, and a second
 * identical declaration is not a redefinition.
 */
#ifndef _ORBIS_COMPAT_STDLIB_H
#define _ORBIS_COMPAT_STDLIB_H

#include_next <stdlib.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t malloc_usable_size(void *);

#ifdef __cplusplus
}
#endif

#endif /* _ORBIS_COMPAT_STDLIB_H */
