// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* FreeBSD's _umtx_op, which is the kernel primitive Mesa's FreeBSD futex is written against.
 *
 * ⚠ THE SYSCALL IS BLOCKED; THE PLATFORM IS NOT MISSING IT. Measured on the console, not assumed:
 * syscall 454 is the right number - OpenOrbis's bits/syscall.h defines SYS__umtx_op as 454, and the
 * preprocessor confirms the call site sees 454 - and the kernel answers ENOSYS (78) to both WAIT and
 * WAKE. Sony's sandbox does not expose it through the syscall table.
 *
 * ⚠ BUT libkernel.so EXPORTS `_umtx_op` AS A SYMBOL, at 0xd366, and it WORKS. Measured 2026-08-19,
 * four rungs (orbis_selftest_umtx_symbol): a WAIT whose expected value does not match memory returns
 * immediately even with NO timeout - so it compares rather than sleeping blindly - and the same call
 * with a matching value and a 150 ms deadline slept 149906 us and returned ETIMEDOUT. Same function,
 * opposite behaviour, difference only in the contents of the word. That is a real compare-and-wait.
 *
 * ⚠ AND THIS FILE IS WHY NOBODY NOTICED. The definition below is `static inline`, so it shadows the
 * libkernel symbol for every translation unit that includes the header. The measurement that said
 * "no _umtx_op" was of the syscall, and after this file existed there was no longer a way to reach
 * the other one by accident.
 *
 * A word of warning about reading such a probe: the mismatch case returns 0, not EWOULDBLOCK. That
 * is not Linux's futex convention and it looked like a stub at first glance. What settles it is the
 * pair of results above, not either one alone.
 *
 * ⚠ SO THIS IMPLEMENTATION MAY BE REDUNDANT. OpenOrbis/musl PR #35 calls the libkernel symbol and
 * reports it working; that PR and this file disagreed only because they were talking about different
 * mechanisms. Deleting the definition below would let futex.c reach Sony's own primitive with no
 * change to Mesa at all. What has not been established is how it behaves under real contention,
 * which is what dEQP-VK.api.object_management.multithreaded_* exercises - build with
 * -DORBIS_UMTX_LIBKERNEL=1 to try it.
 *
 * What that cost, before anyone noticed: futex_wait returned an error INSTANTLY instead of sleeping,
 * so every contended simple_mtx in Mesa became a pure spin. That was invisible for the life of this
 * port because nothing it ran was contended - OpenGothic's own locks are barely contested and the
 * driver's are held for microseconds. The first genuinely multithreaded thing to run here,
 * dEQP-VK.api.object_management.multithreaded_per_thread_device, hung: measured over 180 seconds
 * with not one byte written to the result file or the driver's log.
 *
 * So this file stops declaring a syscall and starts BEING the implementation. Mesa's futex.c is
 * untouched; it still calls _umtx_op, and _umtx_op is now a small wait/wake built on the pthreads
 * this console does have.
 *
 * ⚠ WHAT THIS IS NOT. A futex proper is address-keyed with no bookkeeping and no allocation; this is
 * a fixed table of mutex+condvar pairs that addresses HASH onto, so two unrelated addresses can
 * share a pair and wake each other spuriously. That is correct - futex waiters must re-check their
 * condition anyway, and Mesa's do - but it is slower under heavy contention than the real thing.
 * Sixty-four pairs is enough that collisions are rare and small enough to cost nothing.
 *
 * ⚠ AND THE TABLE IS PRIVATE TO ONE TRANSLATION UNIT, which is the one thing about this file that
 * can be got wrong silently. It is `static`, so a second includer would get its OWN buckets - and a
 * waiter and a waker in different tables never meet. That failure has no symptom except the hang
 * this file was written to remove. The sentinel below is deliberately not static so that a second
 * includer fails at LINK time with a duplicate symbol instead.
 */
#pragma once
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

#define UMTX_OP_WAIT               2
#define UMTX_OP_WAKE               3
#define UMTX_OP_WAIT_UINT          11
#define UMTX_OP_WAIT_UINT_PRIVATE  15
#define UMTX_OP_WAKE_PRIVATE       16

/* The timed variant. futex.c uses _umtx_time with UMTX_ABSTIME for futex_wait's deadline; the layout
   and the flag are FreeBSD's stable ABI, and this implementation honours both. */
#define UMTX_ABSTIME               0x01

struct _umtx_time {
  struct timespec _timeout;
  uint32_t        _flags;
  uint32_t        _clockid;
  };

#ifdef __cplusplus
extern "C" {
#endif

#define ORBIS_UMTX_BUCKETS 64

struct orbis_umtx_bucket {
  pthread_mutex_t lock;
  pthread_cond_t  cond;
  };

static struct orbis_umtx_bucket orbis_umtx_bucket[ORBIS_UMTX_BUCKETS];
static pthread_once_t orbis_umtx_once = PTHREAD_ONCE_INIT;

/* See the note at the top: this exists only so that a second includer collides at link time. The
   ODR complaint a linter raises here is precisely what it is for. */
int orbis_umtx_single_translation_unit; /* NOLINT(misc-definitions-in-headers) */

/* ⚠ WHICH CLOCK DOES pthread_cond_timedwait MEASURE ITS DEADLINE AGAINST? The first version of this
 * file assumed the answer and was wrong.
 *
 * futex.c hands down an ABSOLUTE deadline on CLOCK_MONOTONIC (_umtx_time._clockid says so). POSIX
 * says a condition variable made with default attributes uses CLOCK_REALTIME. Passing the timespec
 * straight through therefore compared a value near this console's UPTIME - a few thousand seconds -
 * against a value near the epoch. Every timed wait's deadline was decades in the past, so it
 * returned ETIMEDOUT instantly and the caller spun. That is the same failure this file exists to
 * remove, on the other path.
 *
 * ⚠ AND pthread_condattr_setclock IS NOT THE FIX HERE, however much it looks like one. It is
 * declared and exported, but `pthread_condattr_t` on this console is `struct { unsigned __attr; }`
 * - FOUR BYTES - which is the exact shape of pthread_mutexattr_t, whose implementation writes past
 * it and quietly destroyed an unrelated 24 bytes of somebody else's stack frame. Setting a clock
 * through a type that small buys a correct deadline at the price of a corruption that lands
 * somewhere else entirely.
 *
 * So the clock is MEASURED instead of declared, and the deadline is converted rather than trusted.
 * That needs no attribute type at all. */
static int orbis_umtx_cond_clock = CLOCK_REALTIME;

/* ⚠ AND THE VERDICT HAS TO BE VISIBLE, which the first version of this got wrong. It decided
 * between two clocks and told nobody, so when the timed self-test failed on the console there was
 * no way to tell a wrong conversion from a wrong decision. A mechanism whose decision cannot be
 * read is not diagnosable. These two are NOT static for the same reason as the sentinel above: one
 * translation unit owns this header, and the driver reads them through an extern declaration. */
static int64_t orbis_umtx_probe_ms;

int orbis_umtx_cond_clock_probed(void);
int64_t orbis_umtx_cond_probe_ms(void);

/* ⚠ THE ONE PLACE EVERY CONTENDED LOCK IN MESA PASSES THROUGH, and that is why the register lives
 * here rather than at the call sites.
 *
 * The arm's watchdog can name a deadlock among the arm's own eight locks, and it did its job: it
 * reported "nothing has progressed for 28 periods and NO lock of this driver is held - whatever is
 * blocking is outside the arm". True, and as far as that instrument can see. Mesa has hundreds of
 * other simple_mtx instances and wrapping them one by one is not a plan.
 *
 * But every simple_mtx that actually BLOCKS calls futex_wait, and on this console futex_wait is this
 * file. So a thread that is stuck on any lock anywhere in Mesa is, by construction, inside the WAIT
 * arm below. Recording who is in there costs two atomics on a path that is already going to sleep.
 *
 * ⚠ WHAT THIS STILL CANNOT SEE, so that an empty register is read correctly: pthread_mutex_lock on
 * Sony's own mutexes (u_mutex, mtx_t, anything not simple_mtx), condition variables waited on
 * directly, and blocking system calls. If the dump comes back EMPTY while nothing progresses, the
 * block is in one of those - which is a different answer, and a narrower one than "outside the arm".
 */
#define ORBIS_UMTX_WAITERS 64

struct orbis_umtx_waiter {
  uint64_t    thread;    /* 0 when the slot is free */
  const void *addr;
  uint64_t    since_ns;  /* wall clock, because CLOCK_MONOTONIC is CPU time in some processes here */
  };

int  orbis_umtx_waiter_count(void);
int  orbis_umtx_waiter_at(int i, uint64_t *thread, const void **addr, uint64_t *since_ns);
uint64_t orbis_umtx_now_ns(void);

/* ⚠ THE PROBE IS BOUNDED IN BOTH OUTCOMES, which is why the deadline is built on MONOTONIC and not
 * on REALTIME. A monotonic deadline 20 ms out is either 20 ms away (the cond is monotonic) or
 * decades in the past (the cond is realtime, whose numbers are far larger) - a wait of 20 ms or a
 * wait of none. The other direction would have to sit through the difference between the two
 * clocks, which on this console is about fifty-five years.
 *
 * ⚠ AND IT IS TIMED ON CLOCK_REALTIME EVEN THOUGH THE DEADLINE IS MONOTONIC, which is not an
 * inconsistency but the point. In the CTS process on this console, CLOCK_MONOTONIC is per-thread
 * CPU TIME - measured, three runs, while the same id is a wall clock in the title's process. Timing
 * a wait with it returns ~0 for a thread that slept perfectly well, so the probe would read every
 * outcome as "no wait" and always answer CLOCK_REALTIME. CLOCK_REALTIME measured as a wall clock in
 * both processes. */
static inline void orbis_umtx_probe_cond_clock(void) {
  struct orbis_umtx_bucket *const b = &orbis_umtx_bucket[0];
  struct timespec deadline, t0, t1;

  clock_gettime(CLOCK_MONOTONIC, &deadline);
  deadline.tv_nsec += 20 * 1000 * 1000;
  if (deadline.tv_nsec >= 1000000000L) {
    deadline.tv_nsec -= 1000000000L;
    deadline.tv_sec++;
    }

  clock_gettime(CLOCK_REALTIME, &t0);
  pthread_mutex_lock(&b->lock);
  /* Nothing can signal this - the once-guard means no waiter exists yet - so it must time out. */
  pthread_cond_timedwait(&b->cond, &b->lock, &deadline);
  pthread_mutex_unlock(&b->lock);
  clock_gettime(CLOCK_REALTIME, &t1);

  const int64_t elapsed_ms =
    (int64_t)(t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_nsec - t0.tv_nsec) / 1000000;

  /* ⚠ HALF THE INTERVAL, not the whole one: a wait that returns early by a millisecond is still a
   * monotonic wait, and a realtime one returns in microseconds. The two outcomes are not close. */
  orbis_umtx_cond_clock  = (elapsed_ms >= 10) ? CLOCK_MONOTONIC : CLOCK_REALTIME;
  orbis_umtx_probe_ms    = elapsed_ms;
  }

static inline void orbis_umtx_init(void) {
  for (int i = 0; i < ORBIS_UMTX_BUCKETS; i++) {
    pthread_mutex_init(&orbis_umtx_bucket[i].lock, NULL);
    pthread_cond_init(&orbis_umtx_bucket[i].cond, NULL);
    }
  orbis_umtx_probe_cond_clock();
  }

/* What the probe decided, for anything that wants to report it. Reading either one forces the probe
   first, so a caller cannot print a default that was never measured. */
static inline int orbis_umtx_cond_clock_id(void) {
  pthread_once(&orbis_umtx_once, orbis_umtx_init);
  return orbis_umtx_cond_clock;
  }

int orbis_umtx_cond_clock_probed(void) { /* NOLINT(misc-definitions-in-headers) */
  return orbis_umtx_cond_clock_id();
  }

int64_t orbis_umtx_cond_probe_ms(void) { /* NOLINT(misc-definitions-in-headers) */
  pthread_once(&orbis_umtx_once, orbis_umtx_init);
  return orbis_umtx_probe_ms;
  }

static struct orbis_umtx_waiter orbis_umtx_waiters[ORBIS_UMTX_WAITERS];

/* ⚠ WHAT THIS SHIM COSTS, COUNTED - BECAUSE IT IS NOW A SUSPECT AND NOT MERELY A MECHANISM.
 *
 * Measured on hardware 2026-08-31: libkernel's internal memory drains by ~146 bytes for every syncobj
 * wait in the graphics driver that expires, ~81 times a frame, until the pool is empty and the process
 * dies. The driver's own census cleared every sceKernel and sceVideoOut call it makes - submits, flips,
 * mprotect, direct memory, usleep are all flat across leaking and non-leaking windows - and the log
 * sink was cleared too, by budgeting the warning to 8 lines while the loss stayed identical.
 *
 * ⚠ WHICH LEAVES THE CALLS THE DRIVER DOES NOT KNOW IT MAKES. That wait's only libkernel-facing work
 * is simple_mtx, and on this console simple_mtx's contended path is futex_wait, and futex_wait is THIS
 * FILE - a pthread_cond_timedwait on one of the buckets above. So the question "what does the failing
 * wait allocate" becomes "does scePthreadCondTimedwait give back what it takes when it TIMES OUT",
 * which is exactly the shape of leak that survives a correct destroy: allocated on entry, released on
 * the signalled return, and never on the expired one.
 *
 * These four counters are how that stops being a hypothesis. If timedwait timeouts arrive at the same
 * rate as the driver's syncobj timeouts, this file is the consumer and the fix is ours - either
 * ORBIS_UMTX_LIBKERNEL=1 to use Sony's own _umtx_op, or a wait that does not use a deadline. If they
 * are flat while the driver's climb, this file is cleared and the allocation is inside libkernel's own
 * syncobj-free path, which no counter here can reach.
 *
 * Relaxed atomics: these are read once every five seconds by a report, and an instrument that
 * synchronises the thing it measures is not measuring it. */
static unsigned long long orbis_umtx_n_wait;
static unsigned long long orbis_umtx_n_timedwait;
static unsigned long long orbis_umtx_n_timeout;
static unsigned long long orbis_umtx_n_lock;

static inline void orbis_umtx_bump(unsigned long long *c) {
  __atomic_fetch_add(c, 1ull, __ATOMIC_RELAXED);
  }

/* Not static, for the same reason as the sentinel: one translation unit owns the table, and the
   graphics driver reads these through a weak extern so it can print them beside the bytes lost. */
void orbis_umtx_stats(unsigned long long *waits, unsigned long long *timedwaits, /* NOLINT(misc-definitions-in-headers) */
                      unsigned long long *timeouts, unsigned long long *locks) {
  if (waits     !=NULL) *waits     = __atomic_load_n(&orbis_umtx_n_wait,    __ATOMIC_RELAXED);
  if (timedwaits!=NULL) *timedwaits= __atomic_load_n(&orbis_umtx_n_timedwait,__ATOMIC_RELAXED);
  if (timeouts  !=NULL) *timeouts  = __atomic_load_n(&orbis_umtx_n_timeout, __ATOMIC_RELAXED);
  if (locks     !=NULL) *locks     = __atomic_load_n(&orbis_umtx_n_lock,    __ATOMIC_RELAXED);
  }

uint64_t orbis_umtx_now_ns(void) { /* NOLINT(misc-definitions-in-headers) */
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return (uint64_t)t.tv_sec * 1000000000ull + (uint64_t)t.tv_nsec;
  }

/* Lock-free: this runs on the way into a sleep, and taking a lock to record a lock is how an
   instrument becomes the defect it was built to find. */
static inline struct orbis_umtx_waiter *orbis_umtx_waiter_claim(const void *addr) {
  const uint64_t me = (uint64_t)(uintptr_t)pthread_self();
  for (int i = 0; i < ORBIS_UMTX_WAITERS; i++) {
    uint64_t expected = 0;
    if (__atomic_compare_exchange_n(&orbis_umtx_waiters[i].thread, &expected, me, 0,
                                    __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)) {
      orbis_umtx_waiters[i].addr     = addr;
      orbis_umtx_waiters[i].since_ns = orbis_umtx_now_ns();
      return &orbis_umtx_waiters[i];
      }
    }
  /* More concurrent sleepers than slots. The dump says how many it can show, so this is visible
     as a truncation rather than as an absence. */
  return NULL;
  }

static inline void orbis_umtx_waiter_release(struct orbis_umtx_waiter *w) {
  if (w != NULL)
    __atomic_store_n(&w->thread, (uint64_t)0, __ATOMIC_RELEASE);
  }

int orbis_umtx_waiter_count(void) { /* NOLINT(misc-definitions-in-headers) */
  int n = 0;
  for (int i = 0; i < ORBIS_UMTX_WAITERS; i++)
    if (__atomic_load_n(&orbis_umtx_waiters[i].thread, __ATOMIC_ACQUIRE) != 0)
      n++;
  return n;
  }

/* Index over OCCUPIED slots, so a caller can walk 0..count-1 without knowing the table's shape.
   Returns 0 when i is past the end - the set can shrink between the count and the walk, and a
   racing reader of a debug register is not worth a lock. */
int orbis_umtx_waiter_at(int i, uint64_t *thread, const void **addr, /* NOLINT(misc-definitions-in-headers) */
                         uint64_t *since_ns) {
  int seen = 0;
  for (int k = 0; k < ORBIS_UMTX_WAITERS; k++) {
    const uint64_t t = __atomic_load_n(&orbis_umtx_waiters[k].thread, __ATOMIC_ACQUIRE);
    if (t == 0)
      continue;
    if (seen++ != i)
      continue;
    if (thread)   *thread   = t;
    if (addr)     *addr     = orbis_umtx_waiters[k].addr;
    if (since_ns) *since_ns = orbis_umtx_waiters[k].since_ns;
    return 1;
    }
  return 0;
  }

/* Whatever clock the caller named, expressed on whatever clock the cond turned out to use. Routed
   through a RELATIVE interval because that is the one form both clocks agree on. */
static inline void orbis_umtx_deadline(const struct _umtx_time *t, struct timespec *out) {
  struct timespec rel, base;

  if ((t->_flags & UMTX_ABSTIME) == 0) {
    /* Already an interval. FreeBSD allows this shape; futex.c never uses it, but a shim that
       answers only its current caller is a shim that breaks on the next one. */
    rel = t->_timeout;
    } else {
    struct timespec now;
    if (clock_gettime((clockid_t)t->_clockid, &now) != 0)
      clock_gettime(CLOCK_MONOTONIC, &now);

    rel.tv_sec  = t->_timeout.tv_sec - now.tv_sec;
    rel.tv_nsec = t->_timeout.tv_nsec - now.tv_nsec;
    if (rel.tv_nsec < 0) {
      rel.tv_nsec += 1000000000L;
      rel.tv_sec--;
      }
    if (rel.tv_sec < 0) {          /* the deadline has already passed */
      rel.tv_sec  = 0;
      rel.tv_nsec = 0;
      }
    }

  clock_gettime((clockid_t)orbis_umtx_cond_clock, &base);
  out->tv_sec  = base.tv_sec + rel.tv_sec;
  out->tv_nsec = base.tv_nsec + rel.tv_nsec;
  if (out->tv_nsec >= 1000000000L) {
    out->tv_nsec -= 1000000000L;
    out->tv_sec++;
    }
  }

/* The address, not its contents. Shifted past the low bits because these are 4-byte aligned words
   and the bottom two bits are always zero. */
static inline struct orbis_umtx_bucket *orbis_umtx_bucket_for(const void *addr) {
  const uintptr_t v = (uintptr_t)addr >> 2;
  return &orbis_umtx_bucket[(v ^ (v >> 8)) % ORBIS_UMTX_BUCKETS];
  }

/* ⚠ THE RACE THIS HAS TO GET RIGHT, and it is the only subtle thing here. A waiter must not sleep
   through a wakeup that arrives between its check of the word and its wait. Holding the bucket lock
   across BOTH the comparison and the wait closes that window, because a waker takes the same lock
   before signalling - so a wake that comes after the value changed cannot slip past a waiter that
   has already read the old value. */
#ifdef ORBIS_UMTX_LIBKERNEL
/* Sony's own, reached by name. Everything above this point is bypassed. */
extern int _umtx_op(void* obj, int op, unsigned long val, void* uaddr, void* uaddr2);
#else
static inline int _umtx_op(void* obj, int op, unsigned long val, void* uaddr, void* uaddr2) {
  pthread_once(&orbis_umtx_once, orbis_umtx_init);

  switch (op) {
    case UMTX_OP_WAIT:
    case UMTX_OP_WAIT_UINT:
    case UMTX_OP_WAIT_UINT_PRIVATE: {
      struct orbis_umtx_bucket *const b = orbis_umtx_bucket_for(obj);
      const volatile uint32_t *const word = (const volatile uint32_t *)obj;
      int ret = 0;

      orbis_umtx_bump(&orbis_umtx_n_wait);
      orbis_umtx_bump(&orbis_umtx_n_lock);
      pthread_mutex_lock(&b->lock);
      if (*word != (uint32_t)val) {
        /* Already changed - FreeBSD returns EWOULDBLOCK for this and Mesa treats any error as
           "re-check and carry on", so the value matters less than not sleeping. */
        pthread_mutex_unlock(&b->lock);
        errno = EWOULDBLOCK;
        return -1;
        }

      /* Registered around the sleep ONLY - a caller that returned above never slept, and listing it
         as a waiter would make the dump lie in the direction that matters most. */
      struct orbis_umtx_waiter *const reg = orbis_umtx_waiter_claim(obj);

      if (uaddr2 != NULL) {
        const struct _umtx_time *const t = (const struct _umtx_time *)uaddr2;
        struct timespec deadline;
        /* ⚠ NOT PASSED STRAIGHT THROUGH, and the comment that used to say it was is what made this
           wrong. The caller's clock and the cond's clock are different numbers for the same instant;
           see orbis_umtx_deadline. */
        orbis_umtx_deadline(t, &deadline);
        orbis_umtx_bump(&orbis_umtx_n_timedwait);
        ret = pthread_cond_timedwait(&b->cond, &b->lock, &deadline);
        if (ret == ETIMEDOUT)
          orbis_umtx_bump(&orbis_umtx_n_timeout);
        } else {
        ret = pthread_cond_wait(&b->cond, &b->lock);
        }

      orbis_umtx_waiter_release(reg);
      pthread_mutex_unlock(&b->lock);

      if (ret != 0) {
        errno = ret;
        return -1;
        }
      return 0;
      }

    case UMTX_OP_WAKE:
    case UMTX_OP_WAKE_PRIVATE: {
      struct orbis_umtx_bucket *const b = orbis_umtx_bucket_for(obj);

      /* ⚠ BROADCAST EVEN WHEN ASKED FOR ONE. Addresses share buckets, so signalling one waiter could
         wake the wrong sleeper and leave the right one asleep - a lost wakeup, which is the exact
         failure this file exists to remove. Waking everyone is correct and merely wasteful. */
      orbis_umtx_bump(&orbis_umtx_n_lock);
      pthread_mutex_lock(&b->lock);
      pthread_cond_broadcast(&b->cond);
      pthread_mutex_unlock(&b->lock);
      (void)val;
      return 0;
      }

    default:
      /* Anything else is a FreeBSD facility nothing in Mesa asks for on this path. Refusing loudly
         beats pretending: a silent success here would be a lock that never blocks. */
      errno = ENOSYS;
      return -1;
    }

  (void)uaddr;
  }
#endif /* ORBIS_UMTX_LIBKERNEL */

#ifdef __cplusplus
}
#endif
