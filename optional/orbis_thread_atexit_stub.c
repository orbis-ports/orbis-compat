// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
//
// __cxa_thread_atexit_impl, which this libc does not have - AS A STUB THAT LEAKS.
//
// ⚠ THIS FILE IS NOT IN liborbis-compat.a AND MUST NOT BE, for the reason the comment below gives:
// it is not a correct implementation and a driver must not get it by accident.
//
// libc++ calls it to register destructors for thread_local objects with non-trivial destruction.
// OpenOrbis's musl does not provide it. OpenGothic has never needed it because nothing in it
// declares such an object; dEQP does.
//
// ⚠ THIS STUB LEAKS, AND SAYS SO RATHER THAN PRETENDING. A correct implementation keeps a per-thread
// list and runs it at thread exit. This one registers nothing, so thread_local destructors never
// run - which for a test binary means memory a finishing thread would have freed stays until the
// process exits. Acceptable for a test run; not acceptable in a driver, and that is why a consumer
// has to ask for this file by name.
//
// The real fix belongs in OpenOrbis's musl. See PLAN.md §8.
int __cxa_thread_atexit_impl(void (*destructor)(void *), void *object, void *dso_handle);

int
__cxa_thread_atexit_impl(void (*destructor)(void *), void *object, void *dso_handle)
{
    (void)destructor;
    (void)object;
    (void)dso_handle;
    return 0;
}
