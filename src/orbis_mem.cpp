// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
#include "orbis_mem.h"
#include "orbis_mmap.h"
#include "orbis_thread.h"
#include "orbis_timer.h"

#include <orbis_log.h>

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
