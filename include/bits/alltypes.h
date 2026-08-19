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

/* No include guard, on purpose: alltypes.h is included many times with different __NEED_ macros. */
#include_next <bits/alltypes.h>
