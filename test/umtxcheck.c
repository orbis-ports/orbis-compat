/* Exercise orbis-compat's sys/umtx.h on the LAPTOP.
 *
 * ⚠ WHY THIS EXISTS AT ALL, when the driver already self-tests the futex on the console. Because
 * --host-orbis does not reach this code. On Linux, futex.c takes the __linux__ arm and calls the
 * kernel's real futex; the shim is compiled only for the console, so the regression gate that
 * catches everything else here is blind to it. Every previous change to the shim was therefore
 * validated by a console run and nothing else - which is a slow way to find an arithmetic mistake.
 *
 * This links the shim directly and drives _umtx_op the way futex.c does. It cannot tell you what
 * this console's pthreads do; it CAN tell you the wait/wake pairing and the deadline arithmetic are
 * right, which is where the last defect was.
 *
 *   cc -o umtxcheck build-support/orbis/tools/umtxcheck.c -I ~/src/forks/orbis-compat/include -lpthread
 *
 * Exits non-zero and names the check that failed.
 */
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/umtx.h>

static int failures;

static void
report(bool ok, const char *what, const char *detail)
{
    printf("%-4s %s%s%s\n", ok ? "ok" : "FAIL", what, detail[0] ? " - " : "", detail);
    if (!ok)
        failures++;
}

static uint64_t
now_ns(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)t.tv_sec * 1000000000ull + (uint64_t)t.tv_nsec;
}

/* futex.c's own two calls, copied rather than included: including util/futex.h here would drag in
   Mesa's build configuration for the sake of nine lines, and these are the shapes under test. */
static int
fx_wake(uint32_t *addr, int32_t count)
{
    return _umtx_op(addr, UMTX_OP_WAKE, (uint32_t)count, NULL, NULL) == -1 ? errno : 0;
}

static int
fx_wait(uint32_t *addr, int32_t value, const struct timespec *timeout)
{
    void *uaddr = NULL, *uaddr2 = NULL;
    struct _umtx_time tmo = {._flags = UMTX_ABSTIME, ._clockid = CLOCK_MONOTONIC};

    if (timeout != NULL)
    {
        tmo._timeout = *timeout;
        uaddr        = (void *)(uintptr_t)sizeof(tmo);
        uaddr2       = (void *)&tmo;
    }

    return _umtx_op(addr, UMTX_OP_WAIT_UINT, (uint32_t)value, uaddr, uaddr2) == -1 ? errno : 0;
}

/* ---------------------------------------------------------------- untimed wait/wake */

struct blocking_probe
{
    uint32_t word;
    int ret;
    uint64_t waited_ns;
    unsigned wakeups;
    volatile bool finished;
};

static void *
blocking_waiter(void *arg)
{
    struct blocking_probe *const p = arg;
    const uint64_t t0              = now_ns();

    /* Re-checks, because a shared bucket may wake it early and that is allowed. */
    do
    {
        p->ret = fx_wait(&p->word, 0, NULL);
        p->wakeups++;
    } while (p->word == 0 && p->ret == 0);

    p->waited_ns = now_ns() - t0;
    p->finished  = true;
    return NULL;
}

static void
check_blocking(void)
{
    struct blocking_probe p = {.word = 0};
    pthread_t th;
    char detail[256];

    if (pthread_create(&th, NULL, blocking_waiter, &p) != 0)
    {
        report(false, "wait blocks until woken", "could not start the waiter thread");
        return;
    }

    /* ⚠ THE REGISTER MUST BE SEEN TO FIRE. The watchdog's whole claim is that a thread asleep on any
     * Mesa lock shows up here; an empty register would otherwise read as "nothing is blocked",
     * which is the most misleading answer it could give. Checked WHILE the waiter is asleep, which
     * is the only moment it can be checked at all. */
    struct timespec settle = {.tv_sec = 0, .tv_nsec = 50 * 1000 * 1000};
    nanosleep(&settle, NULL);
    {
        char rdetail[192];
        uint64_t rthread = 0, rsince = 0;
        const void *raddr = NULL;
        const int rcount  = orbis_umtx_waiter_count();
        const int got     = orbis_umtx_waiter_at(0, &rthread, &raddr, &rsince);

        snprintf(rdetail, sizeof(rdetail), "count %d, slot0 thread 0x%llx on %p (expected %p)", rcount,
                 (unsigned long long)rthread, raddr, (void *)&p.word);
        report(rcount == 1 && got == 1 && rthread != 0 && raddr == (const void *)&p.word,
               "a sleeping thread appears in the waiter register", rdetail);
    }

    struct timespec nap = {.tv_sec = 0, .tv_nsec = 150 * 1000 * 1000};
    nanosleep(&nap, NULL);

    p.word              = 1;
    const int woke      = fx_wake(&p.word, 1);
    const uint64_t give = now_ns();

    while (!p.finished && now_ns() - give < 2000000000ull)
    {
        struct timespec s = {.tv_sec = 0, .tv_nsec = 5 * 1000 * 1000};
        nanosleep(&s, NULL);
    }

    if (!p.finished)
    {
        pthread_detach(th);
        report(false, "wait blocks until woken",
               "still asleep two seconds after the wake - a lost wakeup, which is the failure this "
               "shim exists to prevent");
        return;
    }
    pthread_join(th, NULL);

    {
        char rdetail[96];
        const int rcount = orbis_umtx_waiter_count();
        snprintf(rdetail, sizeof(rdetail), "count %d after the waiter returned", rcount);
        report(rcount == 0, "the waiter register empties again", rdetail);
    }

    snprintf(detail, sizeof(detail), "slept %llu us across %u wakeup(s), wait %d, wake %d",
             (unsigned long long)(p.waited_ns / 1000), p.wakeups, p.ret, woke);

    /* ⚠ FINISHING IS NOT PASSING - a waiter that never slept also finishes. */
    report(p.ret == 0 && woke == 0 && p.waited_ns >= 100000000ull, "wait blocks until woken", detail);
}

/* ---------------------------------------------------------------- the timed wait */

struct timed_probe
{
    uint32_t word;
    int ret;
    uint64_t waited_ns;
    volatile bool finished;
};

#define TIMED_MS 150

static void *
timed_waiter(void *arg)
{
    struct timed_probe *const p = arg;
    struct timespec deadline;

    clock_gettime(CLOCK_MONOTONIC, &deadline);
    deadline.tv_nsec += TIMED_MS * 1000 * 1000;
    if (deadline.tv_nsec >= 1000000000L)
    {
        deadline.tv_nsec -= 1000000000L;
        deadline.tv_sec++;
    }

    const uint64_t t0 = now_ns();
    p->ret            = fx_wait(&p->word, 0, &deadline);
    p->waited_ns      = now_ns() - t0;
    p->finished       = true;
    return NULL;
}

/* ⚠ THE CHECK THE SHIM FAILED, and it failed it silently for as long as the shim has existed.
 *
 * futex.c hands down an ABSOLUTE deadline on CLOCK_MONOTONIC. The shim passed that timespec straight
 * to pthread_cond_timedwait, which measures against CLOCK_REALTIME by default - so a deadline near
 * this machine's uptime was compared against a number near the epoch, found to be decades past, and
 * the wait returned instantly. Every timed wait became a spin.
 *
 * ⚠ BOUNDED IN BOTH DIRECTIONS, because both are real failures with opposite symptoms: too short is
 * the defect above, too long is the same arithmetic wrong the other way and would hang forever. */
static void
check_timed(void)
{
    struct timed_probe p = {.word = 0};
    pthread_t th;
    char detail[256];

    if (pthread_create(&th, NULL, timed_waiter, &p) != 0)
    {
        report(false, "a timed wait expires when it should", "could not start the waiter thread");
        return;
    }

    const uint64_t t0 = now_ns();
    while (!p.finished && now_ns() - t0 < 2000000000ull)
    {
        struct timespec s = {.tv_sec = 0, .tv_nsec = 5 * 1000 * 1000};
        nanosleep(&s, NULL);
    }

    if (!p.finished)
    {
        pthread_detach(th);
        report(false, "a timed wait expires when it should",
               "still asleep two seconds after a 150 ms deadline - the deadline landed on a clock "
               "whose numbers are larger, so it is in the far future");
        return;
    }
    pthread_join(th, NULL);

    const uint64_t ms = p.waited_ns / 1000000;
    snprintf(detail, sizeof(detail), "returned after %llu ms (deadline %d ms), ret %d, ETIMEDOUT %d",
             (unsigned long long)ms, TIMED_MS, p.ret, ETIMEDOUT);

    report(ms >= TIMED_MS * 2 / 3 && ms < 1000 && p.ret == ETIMEDOUT,
           "a timed wait expires when it should", detail);
}

/* ---------------------------------------------------------------- the cheap ones */

static void
check_wake_idle(void)
{
    uint32_t word = 0;
    char detail[64];
    const int r = fx_wake(&word, 1);
    snprintf(detail, sizeof(detail), "returned %d", r);
    report(r == 0, "wake on an address with no waiters", detail);
}

static void
check_value_mismatch(void)
{
    uint32_t word = 7;
    char detail[128];

    /* The word does not hold the expected value, so the wait must not sleep - it must say so. */
    const uint64_t t0 = now_ns();
    const int r       = fx_wait(&word, 0, NULL);
    const uint64_t us = (now_ns() - t0) / 1000;

    snprintf(detail, sizeof(detail), "returned %d after %llu us (EWOULDBLOCK is %d)", r,
             (unsigned long long)us, EWOULDBLOCK);
    report(r == EWOULDBLOCK && us < 50000, "wait on a changed word returns instead of sleeping", detail);
}

static void
check_relative_timeout(void)
{
    /* ⚠ THE SHAPE futex.c NEVER USES. FreeBSD allows a RELATIVE _umtx_time - no UMTX_ABSTIME - and a
     * shim that answers only its current caller breaks on the next one. This is the only check here
     * that tests something nothing in Mesa asks for today. */
    uint32_t word = 0;
    struct _umtx_time tmo = {._timeout = {.tv_sec = 0, .tv_nsec = TIMED_MS * 1000 * 1000},
                             ._flags   = 0,
                             ._clockid = CLOCK_MONOTONIC};
    char detail[160];

    const uint64_t t0 = now_ns();
    const int r = _umtx_op(&word, UMTX_OP_WAIT_UINT, 0, (void *)(uintptr_t)sizeof(tmo), &tmo);
    const uint64_t ms = (now_ns() - t0) / 1000000;

    snprintf(detail, sizeof(detail), "returned %d/errno %d after %llu ms (interval %d ms)", r,
             r == -1 ? errno : 0, (unsigned long long)ms, TIMED_MS);
    report(ms >= TIMED_MS * 2 / 3 && ms < 1000, "a RELATIVE timeout is honoured as an interval", detail);
}

int
main(void)
{
    printf("umtxcheck: the console's futex shim, driven the way futex.c drives it\n\n");

    check_wake_idle();
    check_value_mismatch();
    check_blocking();
    check_timed();
    check_relative_timeout();

    /* Printed last and never asserted on: which clock the condition variables turned out to use is a
       property of the platform, not a pass or a fail. On this laptop it is realtime; if it is ever
       monotonic the conversion becomes a no-op and everything above still holds. */
    printf("\ncondition variables measure their deadlines against %s\n",
           orbis_umtx_cond_clock_id() == CLOCK_MONOTONIC ? "CLOCK_MONOTONIC" : "CLOCK_REALTIME");

    printf("\n%s\n", failures == 0 ? "umtxcheck: all checks passed"
                                   : "umtxcheck: FAILURES above");
    return failures == 0 ? 0 : 1;
}
