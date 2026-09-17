/* hello-world for the orbis-sdk bundle, and the first half of the publication gate.
 *
 * It is not a greeting. Every line below touches one thing the bundle has to have got right,
 * and each says which. If this compiles and links, the bundle's LAYOUT is correct; if it does
 * not, nothing further is worth trying.
 *
 * ⚠ IT IS NOT RUN. Nothing here executes on this host or on a console during the gate - a
 * cross-linked PS4 image cannot run on the build machine. What the gate proves is that the
 * image was produced, and that every path on the link line came out of the bundle. The
 * orbis-compat README makes the same distinction about its own tests and is worth repeating:
 * "A file that cross-compiles is not evidence that it is correct."                          */

#include <orbis_prefix.h>   /* the -include prefix; here as a plain include so this file also
                               compiles for a reader who passes no -include at all         */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>    /* ENODATA, which is absent here as on FreeBSD; the overlay defines it */
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <execinfo.h>       /* the SDK does not ship this. musl has no execinfo.h by design,
                               so this include alone proves orbis-compat/include is being
                               searched AHEAD of sdk/include rather than behind it        */

#include <orbis/libkernel.h>

/* Compile-time assertions, so a wrong include ORDER is a compiler error rather than a
 * pthread_mutexattr_t overrun somebody finds on hardware six weeks later.
 *
 * ⚠ THE OVERLAY COMPILES AND DOES NOTHING WHEN IT IS BEHIND THE SDK'S DIRECTORY. It works by
 * defining musl's own __DEFINED_<name> guards before musl's bits/alltypes.h is reached, so
 * the failure mode of getting this wrong is not an error - it is silence. These four are the
 * four types the port measured on hardware as declared smaller than Sony writes. */
_Static_assert(sizeof(pthread_mutexattr_t)   == sizeof(void *), "orbis-compat is not ahead of the SDK's include directory");
_Static_assert(sizeof(pthread_condattr_t)    == sizeof(void *), "orbis-compat is not ahead of the SDK's include directory");
_Static_assert(sizeof(pthread_barrierattr_t) == sizeof(void *), "orbis-compat is not ahead of the SDK's include directory");
_Static_assert(sizeof(pthread_spinlock_t)    == sizeof(void *), "orbis-compat is not ahead of the SDK's include directory");

/* pthread_once_t is deliberately NOT asserted to have changed: it was predicted to be the
 * worst of the five and needs nothing. The hardware wrote one byte; musl's four are right. */
_Static_assert(sizeof(pthread_once_t) == 4, "pthread_once_t should be untouched");

/* Three names the SDK's own headers leave out, each of which was a patch in a consumer before
 * it was a line in the overlay. Naming them here is what makes their absence a build failure
 * of the BUNDLE rather than a surprise in somebody's port. */
#ifndef ENODATA
#  error "ENODATA is absent - orbis-compat/include/errno.h is not on the include path"
#endif
#ifndef sigev_notify_function
#  error "sigev_notify_function is absent - orbis-compat/include/signal.h is not on the include path"
#endif

static void *worker(void *arg)
{
    /* The thread-stack floor. scePthreadCreate gives every thread 64 KiB and a shader compile
     * does not fit in it; the overlay's policy is that a thread which did not choose gets what
     * the main thread has. Nothing is asserted about the NUMBER here - it is read at runtime
     * on the console, not compiled in - only that the interposer is linked and callable. */
    (void)arg;
    return NULL;
}

int main(void)
{
    void *frames[8];
    pthread_t t;
    size_t n;

    /* sceKernelDebugOutText is the console's own channel and needs no library of ours. */
    sceKernelDebugOutText(0, "orbis-sdk bundle: hello\n");
    puts("orbis-sdk bundle: hello");

    /* backtrace(3): in the overlay's archive, declared by a header musl does not have. It is
     * referenced by nothing in a normal program, which is precisely why the archive must be
     * linked with --whole-archive - and calling it here does NOT test that, because a call
     * creates the reference. The gate's llvm-nm check on the linked image is what tests it. */
    n = (size_t)backtrace(frames, 8);
    printf("backtrace: %zu frames\n", n);

    /* malloc_usable_size through <stdlib.h>, which is where FreeBSD puts it and where portable
     * code therefore looks; the SDK declares it in <malloc.h> only. */
    {
        void *p = malloc(64);
        printf("malloc_usable_size(64) = %zu\n", p ? malloc_usable_size(p) : (size_t)0);
        free(p);
    }

    if (pthread_create(&t, NULL, worker, NULL) == 0)
        pthread_join(t, NULL);

    sceKernelDebugOutText(0, "orbis-sdk bundle: all checks passed\n");
    puts("orbis-sdk bundle: all checks passed");

    /* ⚠ THIS DOES NOT RETURN, AND RETURNING IS THE BUG IT REPLACES. Every check above had already
     * passed the first time this ran on hardware, and the console still reported a crash:
     *
     *     # signal: 12 (SIGSYS)
     *     # rip: 00000008000028bc        <- libkernel.sprx
     *     # BrF: 000000000045a330        <- .plt slot 56 in this binary = _exit@plt
     *
     * main() returned 0, musl called exit(), exit() called libkernel's _exit, and the kernel
     * refused the syscall underneath it. optional/ps4_app.cpp names the same thing from the other
     * side: "returning from main() is reported as CE-34878-0 and reads exactly like a crash", which
     * is why ps4_app_init() installs ps4_idle_forever as the fatal action.
     *
     * This example deliberately does NOT link ps4_app - README section 3.2: a consumer that only
     * wanted a working mmap should not inherit -lSceNet - so it idles on its own. A worked example
     * that ends in a crash report teaches the wrong thing to whoever copies it first.
     *
     * Close it from the PS button menu. The line above is the success marker; a run that reaches it
     * has passed, whatever the console does with the process afterwards. */
    for (;;)
        sleep(1);
}
