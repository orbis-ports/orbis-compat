/* The measuring machinery of the console probe, run here so that a console run is not spent finding
 * out that the probe itself is wrong.
 *
 * Same algorithm as orbis_selftest_pthread_layout() in mesa-ps4's ac_orbis_drm.c, which is where it
 * runs on hardware and which is temporary. On a host whose headers and libc agree, every initialiser
 * must report `fits`; that is the negative control. What it proves is that a write IS detected, that
 * the span is computed correctly, and that the guard bytes either side are watched.
 */
#include <pthread.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define SPAN 128

static int fail;

static int probe_spin_init(void *o) { return pthread_spin_init((pthread_spinlock_t *) o, 0); }
static void once_body(void) {}
static int probe_once(void *o) { return pthread_once((pthread_once_t *) o, once_body); }

/* Deliberately writes past the object, to prove the probe notices. */
static int probe_overrun(void *o) { memset(o, 0x11, sizeof(int) * 4); return 0; }

static void extent(const char *name, int (*init)(void *), size_t declared, size_t zero_prefix, int expect_fits)
{
    unsigned char slab[3 * SPAN], before[3 * SPAN];
    memset(slab, 0xA5, sizeof slab);
    unsigned char *obj = slab + SPAN;
    if (zero_prefix) memset(obj, 0, zero_prefix);
    memcpy(before, slab, sizeof slab);

    int rc = init(obj);

    ptrdiff_t first = -1, last = -1;
    for (size_t i = 0; i < sizeof slab; i++) {
        if (slab[i] == before[i]) continue;
        ptrdiff_t off = (ptrdiff_t) i - (ptrdiff_t) SPAN;
        if (first < 0) first = off;
        last = off;
    }

    int fits = (first >= 0 && (size_t) last < declared);
    printf("  %-24s rc=%d declared=%zu touched=[%td..%td] %s\n", name, rc, declared, first, last,
           first < 0 ? "wrote nothing" : (fits ? "fits" : "OVERRUNS"));

    if (first < 0) { printf("FAIL: %s wrote nothing - the probe cannot see writes\n", name); fail = 1; }
    else if (fits != expect_fits) { printf("FAIL: %s verdict is not what this host must give\n", name); fail = 1; }
}

int main(void)
{
    printf("negative control - on this host the headers and the libc agree:\n");
    extent("pthread_mutexattr_init", (int (*)(void *)) pthread_mutexattr_init, sizeof(pthread_mutexattr_t), 0, 1);
    extent("pthread_condattr_init", (int (*)(void *)) pthread_condattr_init, sizeof(pthread_condattr_t), 0, 1);
    extent("pthread_barrierattr_init", (int (*)(void *)) pthread_barrierattr_init, sizeof(pthread_barrierattr_t), 0, 1);
    extent("pthread_spin_init", probe_spin_init, sizeof(pthread_spinlock_t), 0, 1);
    extent("pthread_once", probe_once, sizeof(pthread_once_t), 4, 1);

    printf("positive control - a deliberate overrun must be reported as one:\n");
    extent("(sixteen bytes into four)", probe_overrun, sizeof(int), 0, 0);

    return fail;
}
