// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* Corrected pthread type sizes for the PlayStation 4, ahead of the toolchain's own header.
 *
 * The SDK ships musl's declarations over Sony's implementation, and Sony's is FreeBSD-derived:
 * every one of these types is an opaque pointer there, while musl stores state inline. Where
 * musl's type is LARGER the mismatch is harmless - Sony writes eight bytes into forty. The four
 * below are the ones musl declares SMALLER than Sony writes, so each is a live overrun.
 *
 * musl guards every typedef with __DEFINED_<name>, so defining one here suppresses its own. The
 * toolchain's file still supplies the other 152 types; this is not a copy of it.
 *
 * ⚠ pthread_mutex_t, pthread_cond_t and pthread_rwlock_t are DELIBERATELY NOT corrected, and this
 * is where we part company with OpenOrbis/musl PR #29. std::mutex embeds pthread_mutex_t, and the
 * toolchain's libc++.a and libc++abi.a are prebuilt against musl's 40-byte version. Shrinking it
 * here would silently disagree with every archive we did not build. Oversized is safe; that is the
 * whole reason this port has worked so far.
 *
 * pthread_once_t is not corrected because it does not need to be: MEASURED on hardware, Sony's
 * pthread_once writes ONE byte into it. Four is enough, musl is right, and the interposer this
 * repository once planned for it is not needed. See README section 3.
 */

#if defined(__NEED_pthread_mutexattr_t) && !defined(__DEFINED_pthread_mutexattr_t)
typedef struct { void *__opaque; } pthread_mutexattr_t;
#define __DEFINED_pthread_mutexattr_t
#endif

#if defined(__NEED_pthread_condattr_t) && !defined(__DEFINED_pthread_condattr_t)
typedef struct { void *__opaque; } pthread_condattr_t;
#define __DEFINED_pthread_condattr_t
#endif

#if defined(__NEED_pthread_barrierattr_t) && !defined(__DEFINED_pthread_barrierattr_t)
typedef struct { void *__opaque; } pthread_barrierattr_t;
#define __DEFINED_pthread_barrierattr_t
#endif

#if defined(__NEED_pthread_spinlock_t) && !defined(__DEFINED_pthread_spinlock_t)
typedef void *pthread_spinlock_t;
#define __DEFINED_pthread_spinlock_t
#endif

/* ⚠ wchar_t IS SIXTEEN BITS IN THIS SDK'S C HEADERS AND THIRTY-TWO EVERYWHERE ELSE, and that is a
 * live data corruption rather than a papercut.
 *
 * The SDK's own bits/alltypes.h says `typedef unsigned short wchar_t` for C. clang targeting
 * x86_64-pc-freebsd12-elf says `__WCHAR_TYPE__ int`, `__WCHAR_WIDTH__ 32` - measured with
 * `clang --target=... -dM -E -` - and in C++ wchar_t is a keyword, so the compiler wins there
 * whatever a header says. The prebuilt libc++.a agrees with the compiler.
 *
 * So a C translation unit and a C++ translation unit in one program disagreed about the width of
 * the same type, and the SDK's libc.a sided with neither consistently: its wide functions were
 * compiled against the 16-bit typedef and walk two-byte elements - wmemcpy copies with
 * `movw (%r8,%rdx,2)`, wcslen tests `cmpw $0,2(%rdi,%rax)`. Every std::wstring a C++ port builds
 * therefore went through a libc reading half of each character. First victim on record: a static
 * initializer in sonic3air's librmx FileIO.cpp, SIGSEGV in wmemcpy BEFORE main(), with klog as the
 * only witness.
 *
 * Correcting the typedef alone would make C agree with C++ and leave both wrong against libc.a, so
 * this arrives together with src/orbis_wchar32.c, which replaces those libc.a members whole. The
 * header and the implementation are one change and must not be separated.
 *
 * ⚠ NOT GUARDED BY __NEED_wchar_t, unlike everything above it. musl only defines wchar_t when a
 * header asks for it, but the damage is done by code that gets the type from the COMPILER (all of
 * C++) meeting code that got it from the header. Defining it here whenever C is being compiled is
 * what makes the two agree; __DEFINED_wchar_t then suppresses musl's, exactly as above.
 */
#if !defined(__cplusplus) && !defined(__DEFINED_wchar_t)
typedef __WCHAR_TYPE__ wchar_t;
#define __DEFINED_wchar_t
#endif

/* No include guard, on purpose: alltypes.h is included many times with different __NEED_ macros. */
#include_next <bits/alltypes.h>
