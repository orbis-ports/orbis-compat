// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
#include "orbis_mem.h"
#include "orbis_mmap.h"
#include "orbis_thread.h"
#include "orbis_timer.h"

#include <orbis_log.h>

#include <orbis_env.h>

#include <errno.h>
#include <pthread.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/types.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>

// Bound to the libkernel exports by asm label rather than declared by name, for the same
// reason og_ps4_stat.cpp declares its own prototypes: the SDK header disagrees with the
// SDK, and here it cannot simply be shadowed - <orbis/libkernel.h> arrives transitively
// through ps4_app.h, so a second declaration of the same C++ name is a hard error.
//
//   sceKernelAvailableFlexibleMemorySize - the installed toolchain declares it
//     `(size_t)`, by value, which cannot be a query. Upstream OpenOrbis fixed that to
//     `(size_t*)` (oracle: OpenOrbis-PS4-Toolchain include/orbis/libkernel.h:55), and
//     that is the form used here.
//   sceKernelAvailableDirectMemorySize - BOTH copies declare the last two arguments by
//     value (`(off_t,off_t,size_t,off_t,size_t)`), which is the same defect and has no
//     corrected version anywhere in the oracle. The out-pointer form below is therefore
//     INFERRED, not cited; the return code is checked and a non-zero one is printed
//     rather than trusted, so a wrong guess reports itself instead of lying.
//   sceKernelGetDirectMemorySize - documented and used verbatim by the SDK's own sample
//     (`OO samples/_common/graphics.cpp:119`); aliased alongside the other two only so
//     that all three reads come from one place.
extern "C" {
int32_t ogFlexibleAvailable(size_t* out)
        __asm__("sceKernelAvailableFlexibleMemorySize");
int32_t ogDirectAvailable(off_t start, off_t end, size_t align,
                          off_t* physOut, size_t* sizeOut)
        __asm__("sceKernelAvailableDirectMemorySize");
size_t  ogDirectTotal(void)
        __asm__("sceKernelGetDirectMemorySize");
}

namespace {

std::atomic<uint64_t> flexBoot{0};
std::atomic<uint64_t> flexLow{~uint64_t(0)};
std::atomic<uint64_t> biggestRequest{0};
std::atomic<int32_t>  flexQueryErr{0};
std::atomic<int>      oomAnnounced{0};

uint64_t flexAvailable() {
  size_t  v  = 0;
  int32_t rc = ogFlexibleAvailable(&v);
  flexQueryErr.store(rc,std::memory_order_relaxed);
  if(rc!=0)
    return 0;

  // Monotone low-water mark. Every census and every refusal updates it, so a phase that
  // spikes and gives the memory back is still visible afterwards - which is the whole
  // point of recording it, since the failing allocation is the one that never appears.
  uint64_t low = flexLow.load(std::memory_order_relaxed);
  while(v<low && !flexLow.compare_exchange_weak(low,v,std::memory_order_relaxed))
    ;
  return v;
  }

uint64_t directAvailable(int32_t& rc) {
  off_t  phys = 0;
  size_t sz   = 0;
  rc = ogDirectAvailable(0,off_t(ogDirectTotal()),0x200000,&phys,&sz);
  return rc==0 ? uint64_t(sz) : 0;
  }

void report(const char* where) {
  const uint64_t flex   = flexAvailable();
  const int32_t  flexRc = flexQueryErr.load(std::memory_order_relaxed);
  const uint64_t boot   = flexBoot.load(std::memory_order_relaxed);
  const uint64_t low    = flexLow.load(std::memory_order_relaxed);

  int32_t        dRc    = 0;
  const uint64_t dAvail = directAvailable(dRc);
  const uint64_t dTotal = uint64_t(ogDirectTotal());

  if(flexRc!=0) {
    orbis_log("mem census v1 [%s]: flexible query FAILED rc=0x%08x - this build cannot see "
            "the pool musl allocates from",where,unsigned(flexRc));
    } else {
    orbis_log("mem census v1 [%s]: flexible avail %llu KiB, boot %llu KiB, low-water %llu KiB, "
            "consumed since boot %lld KiB",
            where,(unsigned long long)(flex/1024),(unsigned long long)(boot/1024),
            (unsigned long long)(low/1024),(long long)((int64_t(boot)-int64_t(flex))/1024));
    }

  if(dRc!=0)
    orbis_log("mem census v1 [%s]: direct total %llu KiB, avail query rc=0x%08x (unread)",
            where,(unsigned long long)(dTotal/1024),unsigned(dRc));
  else
    orbis_log("mem census v1 [%s]: direct total %llu KiB, avail %llu KiB",
            where,(unsigned long long)(dTotal/1024),(unsigned long long)(dAvail/1024));

  // How much of the direct figure just printed is the mmap interposer's, and which of
  // the two pools musl is actually being fed from at this phase. Without it the direct
  // numbers above cannot be attributed between the GNM arenas and the CPU heap.
  orbis::mmapDirectReport(where);
  }

// The refusal. Everything below runs with malloc already returning null, so it must not
// allocate: whatever is registered must not allocate either - Tempest's ps4_log formats into a
// 512-byte stack buffer, which is why it can be registered here at all
// and the kernel queries write into locals.
void announceOom(size_t want, size_t align) {
  // Bounded, because a title that is out of memory tends to fail every allocation from
  // then on and a flood would push the first - the only informative one - off the wire.
  if(oomAnnounced.fetch_add(1,std::memory_order_relaxed)>=8)
    return;

  const uint64_t flex   = flexAvailable();
  const int32_t  flexRc = flexQueryErr.load(std::memory_order_relaxed);
  int32_t        dRc    = 0;
  const uint64_t dAvail = directAvailable(dRc);

  orbis_log("mem OUT OF MEMORY: operator new(%llu bytes, align %llu) returned null",
          (unsigned long long)want,(unsigned long long)align);
  orbis_log("mem OUT OF MEMORY:   flexible avail %llu KiB (rc=0x%08x), boot %llu KiB, "
          "low-water %llu KiB - THIS is the pool musl's malloc grows into",
          (unsigned long long)(flex/1024),unsigned(flexRc),
          (unsigned long long)(flexBoot.load(std::memory_order_relaxed)/1024),
          (unsigned long long)(flexLow.load(std::memory_order_relaxed)/1024));
  orbis_log("mem OUT OF MEMORY:   direct total %llu KiB, avail %llu KiB (rc=0x%08x) - a "
          "large number here CANNOT serve this request, the pools are disjoint",
          (unsigned long long)(ogDirectTotal()/1024),
          (unsigned long long)(dAvail/1024),unsigned(dRc));
  orbis_log("mem OUT OF MEMORY:   largest single request this process ever made: %llu bytes",
          (unsigned long long)biggestRequest.load(std::memory_order_relaxed));
  }

void noteRequest(size_t n) {
  // Relaxed load, unconditional store on a new maximum. Racy by construction and that is
  // acceptable: this is a diagnostic watermark, not an accounting record, and the cost
  // has to stay at one relaxed load because every allocation in the process pays it.
  if(n>biggestRequest.load(std::memory_order_relaxed))
    biggestRequest.store(n,std::memory_order_relaxed);
  }

void* allocPlain(size_t n) {
  noteRequest(n);
  return std::malloc(n!=0 ? n : 1);
  }

void* allocAligned(size_t n, size_t align) {
  noteRequest(n);
  // posix_memalign, not aligned_alloc: it is what libc++'s own operator new uses on this
  // toolchain (`U posix_memalign` in libc++.a's stdlib_new_delete.cpp.o), so the pointers
  // this hands out are freed by exactly the routine that already frees libc++'s.
  void* p = nullptr;
  if(align<sizeof(void*))
    align = sizeof(void*);
  if(posix_memalign(&p,align,n!=0 ? n : 1)!=0)
    return nullptr;
  return p;
  }

}

void orbis::memCensusBaseline() {
  size_t  v  = 0;
  int32_t rc = ogFlexibleAvailable(&v);
  flexQueryErr.store(rc,std::memory_order_relaxed);
  if(rc==0) {
    flexBoot.store(v,std::memory_order_relaxed);
    flexLow.store(v,std::memory_order_relaxed);
    }
  report("boot");

  // The thread probe rides here because this is the one place the overlay is already given control
  // at boot, with a logger registered and before the threads that matter exist. It answers a
  // question about memory too: what every thread on this platform gets by default. See
  // include/orbis_thread.h.
  threadStackProbe();
  timerProbe();
  internalMemoryProbe();
  }

void orbis::memCensusThreads(const char* where) {
  unsigned long created = 0, raised = 0;
  threadCounts(&created,&raised);
  unsigned long altOk = 0, altFailed = 0, altSkipped = 0;
  threadAltStacks(&altOk,&altFailed,&altSkipped);
  orbis_log("thread census [%s]: %lu created, %lu raised to %llu KiB - %llu KiB of address space "
            "the platform's default would not have reserved; %lu on an alternate signal stack, "
            "%lu refused, %lu too small to spare one - %llu KiB, taken from those threads' own "
            "stacks rather than allocated",
            where,created,raised,
            (unsigned long long)(threadStackFloor()/1024),
            (unsigned long long)(raised*(threadStackFloor()>65536 ? threadStackFloor()-65536 : 0)/1024),
            altOk,altFailed,altSkipped,
            (unsigned long long)(altOk*threadAltStackSize()/1024));
  }

void orbis::memCensus(const char* where) {
  report(where);
  }

// ------------------------------------------------------------ replaceable allocation
//
// libc++ ships these as WEAK symbols (`W _Znwm` in stdlib_new_delete.cpp.o), so a strong
// definition in a source of the executable simply wins - the same linker rule the stat
// interposition rests on, and the reason this file has to be a source of the target and
// not a library member. The matching `operator delete`s are deliberately NOT replaced:
// libc++'s call free(), and everything below produces free-able pointers.
//
// Why replace them at all, when libc++'s own version already throws std::bad_alloc: it
// throws it without saying how many bytes were asked for, out of which pool, or how much
// of that pool was left - so the failure arrives at the catch site as the word "out of
// memory" and nothing else. That is precisely how the world-load failure of 2026-08-04
// reached the log.

void* operator new(std::size_t n) {
  if(void* p = allocPlain(n))
    return p;
  announceOom(n,0);
  throw std::bad_alloc();
  }

void* operator new[](std::size_t n) {
  if(void* p = allocPlain(n))
    return p;
  announceOom(n,0);
  throw std::bad_alloc();
  }

void* operator new(std::size_t n, std::align_val_t a) {
  if(void* p = allocAligned(n,static_cast<size_t>(a)))
    return p;
  announceOom(n,static_cast<size_t>(a));
  throw std::bad_alloc();
  }

void* operator new[](std::size_t n, std::align_val_t a) {
  if(void* p = allocAligned(n,static_cast<size_t>(a)))
    return p;
  announceOom(n,static_cast<size_t>(a));
  throw std::bad_alloc();
  }

void* operator new(std::size_t n, const std::nothrow_t&) noexcept {
  void* p = allocPlain(n);
  if(p==nullptr)
    announceOom(n,0);
  return p;
  }

void* operator new[](std::size_t n, const std::nothrow_t&) noexcept {
  void* p = allocPlain(n);
  if(p==nullptr)
    announceOom(n,0);
  return p;
  }

void* operator new(std::size_t n, std::align_val_t a, const std::nothrow_t&) noexcept {
  void* p = allocAligned(n,static_cast<size_t>(a));
  if(p==nullptr)
    announceOom(n,static_cast<size_t>(a));
  return p;
  }

void* operator new[](std::size_t n, std::align_val_t a, const std::nothrow_t&) noexcept {
  void* p = allocAligned(n,static_cast<size_t>(a));
  if(p==nullptr)
    announceOom(n,static_cast<size_t>(a));
  return p;
  }


// ------------------------------------------------------- libkernel's internal memory, per primitive
//
// See the block in include/orbis_mem.h for why this exists.

namespace {

// ⚠ NOT IN ANY SDK HEADER, SO THE SIGNATURE IS COVERED RATHER THAN GUESSED. libkernel.so exports it
// (llvm-nm --dynamic, 2026-08-31) and orbis/libkernel.h does not declare it. Calling it with a real
// pointer to a zeroed scratch satisfies both plausible Sony shapes - `int f(size_t*)` writes through
// the pointer and returns 0, `size_t f(void)` ignores it and returns the size - and rsi/rdx are given
// defined values so a third shape taking a length cannot be handed rubbish. Weak: a toolchain that
// cannot resolve it yields a probe that says so instead of a link that fails.
extern "C" unsigned long long sceKernelInternalMemoryGetAvailableSize(void*, unsigned long long,
                                                                     unsigned long long)
    __attribute__((weak));

extern "C" int sceKernelUsleep(unsigned int);

unsigned long long internalFree() {
  if(&sceKernelInternalMemoryGetAvailableSize==nullptr)
    return 0;
  unsigned long long scratch[8] = {0,0,0,0,0,0,0,0};
  const unsigned long long ret = sceKernelInternalMemoryGetAvailableSize(scratch,sizeof(scratch),0);
  return (scratch[0]!=0) ? scratch[0] : ret;
  }

constexpr int kProbeIterations = 2000;

// ⚠ THE CONTROL IS NOT OPTIONAL. Something else in the process may be spending the pool while this
// runs, and without an empty loop measured the same way every candidate would inherit that drift and
// the smallest ones would be indistinguishable from it.
void measureN(const char* what, void (*body)(), int n) {
  const unsigned long long before = internalFree();
  for(int i=0; i<n; i++)
    body();
  const unsigned long long after = internalFree();

  const long long lost = (long long)before - (long long)after;
  orbis_log("internal memory probe: %-26s %lld bytes over %d call(s) = %lld bytes each "
            "(%llu free after)",
            what,lost,n,lost/n,after);
  }

void measure(const char* what, void (*body)()) {
  measureN(what,body,kProbeIterations);
  }

void bodyNothing() {
  // Not empty: an empty body is a loop the optimiser deletes, and a control that did not run is
  // worse than no control at all.
  static volatile int sink;
  sink = sink + 1;
  }

void bodyClockMonotonic() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC,&ts);
  }

void bodyClockRealtime() {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME,&ts);
  }

pthread_mutex_t g_probeMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  g_probeCond  = PTHREAD_COND_INITIALIZER;

void bodyMutexLock() {
  pthread_mutex_lock(&g_probeMutex);
  pthread_mutex_unlock(&g_probeMutex);
  }

// ⚠ THE ONE THIS PROBE WAS BUILT FOR, and the deadline is the epoch on purpose. A cond made with
// default attributes may measure against CLOCK_REALTIME or CLOCK_MONOTONIC and this port has measured
// BOTH answers in different processes; {0,0} is in the past on either, so the wait cannot block
// whichever it turns out to be. A deadline built from the wrong clock would either return instantly
// or hang for fifty-five years, and a probe must not be able to do the second.
void bodyCondTimedwaitExpired() {
  struct timespec epoch;
  epoch.tv_sec  = 0;
  epoch.tv_nsec = 0;
  pthread_mutex_lock(&g_probeMutex);
  pthread_cond_timedwait(&g_probeCond,&g_probeMutex,&epoch);
  pthread_mutex_unlock(&g_probeMutex);
  }

void bodyUsleep() {
  sceKernelUsleep(1);
  }

/* ⚠ THE TWO THE LEDGER POINTED AT, AND NEITHER HAS EVER BEEN WEIGHED.
 *
 * The frame ledger put 9073 of 9280 bytes a frame in the two segments above the graphics driver - the
 * frontend between frames, and the draw/translate/record path - with every segment inside the driver
 * costing nothing. Bytes cannot leave libkernel's internal pool without a libkernel call, and the only
 * libkernel calls those segments make that this port has never counted are the ones musl's allocator
 * makes through orbis_mmap.cpp when a request cannot be served from a carve-out.
 *
 * ⚠ SIZED ABOVE musl's mmap THRESHOLD ON PURPOSE. A small malloc is served from the heap and touches
 * no syscall at all, which would measure nothing and prove nothing; 96 KiB is what a per-frame buffer
 * looks like and it takes the path under test. The raw mmap/munmap pair is measured separately and at
 * a lower count, because it is the heavier of the two and 500 is already far more than the meter's
 * 32-byte resolution needs at any plausible per-call cost. */
void bodyMallocLarge() {
  void* p = malloc(96*1024);
  if(p!=nullptr) {
    // Touched, so the allocator cannot hand back something it never had to back.
    *static_cast<volatile char*>(p) = 1;
    free(p);
    }
  }

void bodyMmapPair() {
  void* p = mmap(nullptr,64*1024,PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE,-1,0);
  if(p!=MAP_FAILED && p!=nullptr) {
    *static_cast<volatile char*>(p) = 1;
    munmap(p,64*1024);
    }
  }

}

void orbis::internalMemoryProbe() {
  if(orbis_log_enabled()==0)
    return;

  const char* e = orbis_env_get("ORBIS_INTERNAL_MEM_PROBE");
  if(e==nullptr || *e=='\0' || *e=='0')
    return;

  if(&sceKernelInternalMemoryGetAvailableSize==nullptr) {
    orbis_log("internal memory probe: sceKernelInternalMemoryGetAvailableSize did not resolve - "
              "nothing here can be measured");
    return;
    }

  orbis_log("internal memory probe: %llu bytes free before any candidate runs. Each line below is "
            "one primitive run %d times in isolation; the control must read ~0 or every other line "
            "is inheriting somebody else's drift.",
            internalFree(),kProbeIterations);

  measure("control (nothing)",       &bodyNothing);
  measure("clock_gettime MONOTONIC", &bodyClockMonotonic);
  measure("clock_gettime REALTIME",  &bodyClockRealtime);
  measure("pthread_mutex lock+unlock",&bodyMutexLock);
  measure("sceKernelUsleep(1)",      &bodyUsleep);
  measure("cond_timedwait EXPIRED",  &bodyCondTimedwaitExpired);
  measure("malloc+free 96 KiB",      &bodyMallocLarge);
  measureN("mmap+munmap 64 KiB",     &bodyMmapPair, 500);

  {
    unsigned long long m=0,mc=0,mf=0,u=0,uc=0,uf=0;
    orbis_mmap_counts(&m,&mc,&mf,&u,&uc,&uf);
    orbis_log("internal memory probe: mmap traffic so far: %llu map(s) (%llu from a carve-out, %llu "
              "fell through to libkernel), %llu unmap(s) (%llu carve-out, %llu fell through). The "
              "fall-throughs are the ones that can cost internal memory.",
              m,mc,mf,u,uc,uf);
    }

  orbis_log("internal memory probe: done. The frame ledger puts 9073 of 9280 bytes a frame above the "
            "graphics driver, and bytes cannot leave this pool without a libkernel call - so if the "
            "malloc or mmap line above is non-zero, that is the consumer and it is reached from every "
            "allocation the frontend and zink make. If every line is 0, the call is one this overlay "
            "does not make either, and the remaining route is instrumenting zink itself.");
  }


/* ⚠ A C ENTRY POINT, BECAUSE THE ONE PLACE THAT CAN CALL THIS IS C AND THE PROBE WAS DEAD WITHOUT IT.
 *
 * The first version of this probe was hung on orbis::memCensusBaseline(), on my claim that it "rides
 * at boot, so no frontend change is needed". memCensusBaseline() is defined in this file and CALLED
 * FROM NOWHERE - not by this overlay, not by the frontend, not by anything in ps4/. The probe was
 * shipped, the knob was set correctly, and it printed nothing, because it was never reached.
 *
 * ⚠ AND THE CALL SITE IS NOT FREE TO CHOOSE. The knob lives in an env FILE, and on this platform
 * setenv() in one image is invisible to another - the frontend parses those files and setenv()s them
 * at frontend/drivers/platform_orbis.c:472-484. Anything that reads getenv before that line sees an
 * empty environment, so a boot-time constructor here would be just as silent as dead code was. */
extern "C" void orbis_internal_memory_probe(void) {
  orbis::internalMemoryProbe();
  }
