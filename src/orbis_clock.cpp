// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// See include/orbis_clock.h for the defect, the measurement and why no clock id is used.
#ifdef __PS4__

#include <orbis_clock.h>
#include <orbis_log.h>

#include <orbis/libkernel.h>

#include <chrono>
#include <cerrno>
#include <cstdint>
#include <ctime>
#include <sys/time.h>

// NOT gettimeofday - see realtimeNow() below. This is the kernel call with no libc
// layer above it to come back through.
extern "C" int32_t sceKernelGettimeofday(struct timeval* tv);

namespace {

struct ClockBase {
  uint64_t freq   = 0;   // counter ticks per second, 0 if the SDK would not say
  uint64_t origin = 0;   // counter at first use, so tv_sec stays small and cannot overflow
  };

// Initialised on first use, not from a constructor: libc++ can call this before this
// translation unit's static initialisers have run, and a clock returning garbage during
// startup would be the same class of defect as the one being fixed.
ClockBase& clockBase() {
  static ClockBase b = [] {
    ClockBase t;
    t.freq   = sceKernelGetProcessTimeCounterFrequency();
    t.origin = sceKernelGetProcessTimeCounter();
    return t;
    }();
  return b;
  }

int monotonicNow(struct timespec* ts) {
  ClockBase& b = clockBase();
  if(b.freq==0) {
    // Documented in microseconds, and the clock every other instrument here already trusts.
    const uint64_t us = sceKernelGetProcessTime();
    ts->tv_sec  = static_cast<decltype(ts->tv_sec)> (us/1000000ull);
    ts->tv_nsec = static_cast<decltype(ts->tv_nsec)>((us%1000000ull)*1000ull);
    return 0;
    }
  const uint64_t d = sceKernelGetProcessTimeCounter()-b.origin;
  // Seconds split off first: the remainder is < freq, so the multiply is bounded by
  // freq*1e9, far inside uint64 at any plausible counter frequency.
  ts->tv_sec  = static_cast<decltype(ts->tv_sec)> (d/b.freq);
  ts->tv_nsec = static_cast<decltype(ts->tv_nsec)>((d%b.freq)*1000000000ull/b.freq);
  return 0;
  }

// ⚠ THIS MUST NOT CALL gettimeofday, AND THE FIRST VERSION DID. It cost a console crash on
// the first savegame this port ever attempted. The reasoning that put it there - "gettimeofday
// takes no clock id, so it cannot be misread the way clock_gettime's was" - is true and
// irrelevant: it takes no id because it HARDCODES one and calls the very function this file
// replaces. musl's gettimeofday.lo is, verbatim:
//
//     xor %edi,%edi          ; clockid = 0 = CLOCK_REALTIME
//     mov %r14,%rsi
//     call clock_gettime     ; <- us
//
// The kernel's crash dump named it exactly: `rip` at gettimeofday+0x19, which is that `call`,
// and a backtrace repeating until "the stack frames are too many to display".
//
// It survived five sessions because nothing here asks for the WALL clock - every timer, the
// frame pacing and the audio clock take CLOCK_MONOTONIC. The first consumer of CLOCK_REALTIME
// is a savegame header's timestamp, so the recursion was unreachable until somebody pressed save.
//
//   LEDGER: AN INTERPOSER MUST NOT CALL ANY LIBC FUNCTION THAT MIGHT BE IMPLEMENTED IN TERMS
//   OF THE ONE IT REPLACES. The check is not "does this look like a different function" but
//   "what does the archive member actually call" - one `objdump -d` would have shown it.
int realtimeNow(struct timespec* ts) {
  struct timeval tv = {};
  if(sceKernelGettimeofday(&tv)!=0)
    return -1;
  ts->tv_sec  = static_cast<decltype(ts->tv_sec)> (tv.tv_sec);
  ts->tv_nsec = static_cast<decltype(ts->tv_nsec)>(tv.tv_usec*1000);
  return 0;
  }

int  unknownId     = -1;
bool unknownIdSeen = false;

} // namespace

// Overrides libkernel.so's export for every reference inside this executable - the same
// mechanism orbis_stat.cpp uses against `stat` and orbis_mmap.cpp against `mmap`. libc++ is
// linked statically here, so its steady_clock and high_resolution_clock resolve to this.
/* ⚠ COUNTED, BECAUSE IT IS THE LAST THING LEFT ON A PATH WHERE EVERYTHING ELSE HAS BEEN CLEARED.
 *
 * The graphics driver loses ~146 bytes of libkernel's internal memory for every syncobj wait that
 * expires, ~81 times a frame. Measured and excluded so far: every sceKernel and sceVideoOut call the
 * driver makes (submits, flips, mprotect, direct memory, usleep - all flat across leaking and
 * non-leaking windows), the log sink (budgeted to 8 lines, loss unchanged), and this overlay's futex
 * shim (0 cond-timedwaits and 0 timeouts in every window). What remains on the failing path is one
 * mutex round trip and TWO CALLS TO THIS FUNCTION - orbis_wait_begin reads the clock, orbis_wait_continue
 * reads it again - and 146/2 is 73, which is the size of a small fixed record.
 *
 * That is a coincidence until it is measured, which is what these counters are for: if the leak is
 * here, the monotonic count runs at about twice the driver's expiring-wait count and the bytes divide
 * evenly by it. Relaxed atomics on a path this hot are two instructions and no ordering. */
static unsigned long long clkMonotonic;
static unsigned long long clkRealtime;

extern "C" void orbis_clock_counts(unsigned long long* monotonic, unsigned long long* realtime) {
  if(monotonic!=nullptr) *monotonic = __atomic_load_n(&clkMonotonic,__ATOMIC_RELAXED);
  if(realtime !=nullptr) *realtime  = __atomic_load_n(&clkRealtime, __ATOMIC_RELAXED);
  }

extern "C" int clock_gettime(clockid_t id, struct timespec* ts) {
  if(ts==nullptr) {
    errno = EFAULT;
    return -1;
    }
  switch(id) {
    case CLOCK_MONOTONIC:
    case CLOCK_MONOTONIC_RAW:
    case CLOCK_MONOTONIC_COARSE:
    case CLOCK_BOOTTIME:
      __atomic_fetch_add(&clkMonotonic,1ull,__ATOMIC_RELAXED);
      return monotonicNow(ts);
    case CLOCK_REALTIME:
    case CLOCK_REALTIME_COARSE:
      __atomic_fetch_add(&clkRealtime,1ull,__ATOMIC_RELAXED);
      return realtimeNow(ts);
    default:
      // The CPU-time clocks. Nothing here asks for them, and EINVAL would fail a caller for a
      // reason unrelated to what it wanted. A monotonic answer is wrong in the same direction
      // for everyone and is SAID OUT LOUD below - the difference between a fallback and a lie.
      if(!unknownIdSeen) {
        unknownIdSeen = true;
        unknownId     = int(id);
        }
      return monotonicNow(ts);
    }
  }

namespace orbis {

void clockProbe() {
  static bool done = false;
  if(done || orbis_log_enabled()==0)
    return;
  done = true;

  ClockBase& b = clockBase();
  if(b.freq!=0)
    orbis_log("clock: CLOCK_MONOTONIC served from sceKernelGetProcessTimeCounter, %llu tick(s)/s "
              "(%.3f MHz); CLOCK_REALTIME from sceKernelGettimeofday, not from gettimeofday",
              (unsigned long long)b.freq, double(b.freq)/1000000.0);
  else
    orbis_log("clock: sceKernelGetProcessTimeCounterFrequency returned 0 - CLOCK_MONOTONIC falls "
              "back to sceKernelGetProcessTime at microsecond resolution");

  // ⚠ THE REFERENCE IS THE COUNTER, not sceKernelGetProcessTime. An earlier version used process
  // time and could not tell a clock that runs slow from one that measures CPU time - the two give
  // the same ratio in a busy loop and need different fixes. Process time is a SUBJECT here.
  const uint64_t freq = sceKernelGetProcessTimeCounterFrequency();

  struct Window {
    const char* name;
    double      procRatio   = 0.0;
    double      steadyRatio = 0.0;
    double      hiresRatio  = 0.0;
    };
  Window win[2] = {{"busy"},{"asleep"}};

  for(unsigned k=0; k<2; ++k) {
    const uint64_t c0 = sceKernelGetProcessTimeCounter();
    const uint64_t p0 = sceKernelGetProcessTime();
    const auto     s0 = std::chrono::steady_clock::now();
    const auto     h0 = std::chrono::high_resolution_clock::now();

    if(k==0) {
      // ~20 ms, spun against the counter itself so the window's length does not depend on any
      // of the clocks under test.
      const uint64_t want = (freq!=0) ? (freq/50) : 0;
      if(want!=0)
        while(sceKernelGetProcessTimeCounter()-c0 < want) {}
      else
        for(volatile uint64_t i=0; i<20000000; ++i) {}
      } else {
      sceKernelUsleep(20000);
      }

    const uint64_t c1 = sceKernelGetProcessTimeCounter();
    const uint64_t p1 = sceKernelGetProcessTime();
    const auto     s1 = std::chrono::steady_clock::now();
    const auto     h1 = std::chrono::high_resolution_clock::now();

    const int64_t realUs   = (freq!=0) ? int64_t((c1-c0)*1000000ull/freq) : 0;
    const int64_t procUs   = int64_t(p1-p0);
    const int64_t steadyUs = std::chrono::duration_cast<std::chrono::microseconds>(s1-s0).count();
    const int64_t hiresUs  = std::chrono::duration_cast<std::chrono::microseconds>(h1-h0).count();

    win[k].procRatio   = realUs>0 ? double(procUs)  /double(realUs) : 0.0;
    win[k].steadyRatio = realUs>0 ? double(steadyUs)/double(realUs) : 0.0;
    win[k].hiresRatio  = realUs>0 ? double(hiresUs) /double(realUs) : 0.0;

    orbis_log("clock probe [%s]: counter %lld us (ref) | process-time %lld us | steady_clock %lld us "
              "| high_resolution_clock %lld us", win[k].name, (long long)realUs, (long long)procUs,
              (long long)steadyUs, (long long)hiresUs);
    }

  // Ratios on their own line: "19998 vs 20000" and "2993 vs 20000" are the same two numbers to
  // anyone skimming a 2 MB log and completely different findings.
  for(unsigned k=0; k<2; ++k)
    orbis_log("clock probe [%s]: proc/ref %.4f steady/ref %.4f hires/ref %.4f  (1.0 = agrees with "
              "the counter; ~0.0 while asleep = it is a CPU-time clock)", win[k].name,
              win[k].procRatio, win[k].steadyRatio, win[k].hiresRatio);

  if(win[0].steadyRatio>0.9 && win[1].steadyRatio<0.1)
    orbis_log("clock probe: VERDICT THE INTERPOSER IS NOT IN THIS BINARY - steady_clock tracks the "
              "counter while busy and stops dead while asleep, which is CLOCK_VIRTUAL");
  else if(win[0].steadyRatio>0.9 && win[1].steadyRatio>0.9)
    orbis_log("clock probe: VERDICT the monotonic clock is WALL TIME - it agrees with the counter "
              "busy and asleep alike, which is what this file exists to guarantee");
  else
    orbis_log("clock probe: VERDICT INCONCLUSIVE - neither window gave a clean ratio");

  if(unknownIdSeen)
    orbis_log("clock probe: clock id %d was requested and answered with the monotonic clock - it is "
              "a CPU-time clock and nothing should be asking for it", unknownId);
  }

} // namespace orbis

#endif
