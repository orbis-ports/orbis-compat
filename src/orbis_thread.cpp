// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// See include/orbis_thread.h.
#include <orbis_thread.h>

#include <orbis_log.h>
#include <orbis_mem.h>

#include <execinfo.h>
#include <pthread.h>
#include <pthread_np.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <atomic>

// libkernel's own creator, which the SDK's pthread_create (0xda78) is itself a wrapper over. Calling
// it directly is what lets this file define pthread_create without recursing into itself.
extern "C" int scePthreadCreate(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *,
                                const char *);

namespace {

size_t g_defaultStack = 0;

std::atomic<unsigned long> g_created{0};
std::atomic<unsigned long> g_raised{0};

// ⚠ THE INTERPOSER USED TO LOG ONLY ON SUCCESS, AND THAT IS WHY THE ONE FAILURE THIS PORT KEEPS
// MEETING HAS NEVER BEEN NAMED FROM OUR SIDE.
//
// Three times now a run has ended with the console's own log carrying thousands of
//
//     [ScePthread/System] Internal Memory is running out.     technote 235
//
// and nothing from this file at all: the census below prints at powers of two AND only when
// scePthreadCreate returned 0, so a run whose every create fails prints exactly nothing and reads
// as "no thread was ever created again". These two counters and the report under them exist so a
// failing create is as loud as a succeeding one, and so the addresses of whoever asked for it are
// in the log rather than in a guess.
//
// ⚠ AND THE ATTR COUNTER IS THE LEADING INDICATOR, not an afterthought. scePthreadAttrInit takes an
// object out of the SAME pool a thread does, and it is asked for FIRST - twice per create through
// this file, plus once per stackOfSelf. So the pool's floor is reached on an attr before it is
// reached on a thread, and an attr failure is the earliest moment this file can say the pool is
// going. Both are reported; which one moves first is itself the measurement.
std::atomic<unsigned long> g_createFailed{0};
std::atomic<unsigned long> g_attrFailed{0};

// ⚠ POWERS OF TWO, BECAUSE THE FAILING CASE IS UNBOUNDED. Measured 2026-08-31: the console's klog
// carried 8592 of these in 133 seconds, at a flat 117 per drain - the transport's quantum, not the
// event rate, so the real rate is unknown and only bounded from below. A line per failure would put
// this port's own channel in the same state. 1, 2, 4 ... 4096 is twelve lines for four thousand
// failures and still shows the shape.
bool powerOfTwo(unsigned long n) {
  return n!=0 && (n & (n-1))==0;
  }

void reportPoolFailure(const char* what, int rc, unsigned long n) {
  if(!powerOfTwo(n) || orbis_log_enabled()==0)
    return;

  // ⚠ ADDRESSES, AND THEY ARE THE POINT. There is no symbolisation on this console; the port logs
  // each module's base as it loads and ps4/symbolise.py resolves these against the ELFs. Skipped
  // frames: this function and the interposer that called it.
  void* frames[10];
  const int nf = orbis_unwind_collect(frames,10,2);

  char   callers[10*20+1];
  size_t at = 0;
  callers[0] = '\0';
  for(int i=0; i<nf; i++) {
    const int n2 = snprintf(callers+at,sizeof(callers)-at,"%s%p",(i!=0) ? " " : "",frames[i]);
    // snprintf reports what it WOULD have written, so a truncated write must stop the loop rather
    // than advance past the end of the buffer.
    if(n2<0 || (size_t)n2>=sizeof(callers)-at)
      break;
    at += (size_t)n2;
    }

  orbis_log("thread pool FAILURE: %s returned %d - %lu %s failure(s) so far "
            "(%lu create(s) attempted, %lu raised, %lu attr failure(s), %lu create failure(s)). "
            "⚠ THIS IS THE ScePthread INTERNAL POOL, technote 235, and it backs mutexes, condvars, "
            "keys and attrs as well as threads. Callers, innermost first: %s",
            what,rc,n,what,
            g_created.load(std::memory_order_relaxed),
            g_raised.load(std::memory_order_relaxed),
            g_attrFailed.load(std::memory_order_relaxed),
            g_createFailed.load(std::memory_order_relaxed),
            callers);
  }

// The size a thread that did not choose should get. Resolved once, on first use, because the env
// file that can override it is applied by the title early but not necessarily before the first
// thread exists.
//
//   unset   -> what the MAIN thread has, which this platform sets to 2 MB
//   0       -> interpose nothing, hand every request through untouched
//   <n>     -> n KiB
std::atomic<size_t> g_floor{~size_t(0)};   // ~0 means "not resolved yet"

size_t mainThreadStack();

size_t resolveFloor() {
  size_t v = g_floor.load(std::memory_order_relaxed);
  if(v!=~size_t(0))
    return v;

  const char* e = getenv("ORBIS_THREAD_STACK");
  if(e!=nullptr) {
    const long kib = strtol(e,nullptr,10);
    v = (kib>0) ? size_t(kib)*1024 : 0;
    } else {
    v = mainThreadStack();
    }

  // A floor below the platform's own default would be a downgrade, and one below PTHREAD_STACK_MIN
  // is not a stack at all. Either means the probe failed, and doing nothing is the safe answer.
  if(v!=0 && v<size_t(PTHREAD_STACK_MIN))
    v = 0;

  g_floor.store(v,std::memory_order_relaxed);
  return v;
  }

// What a live thread actually got. pthread_attr_get_np is libkernel's (0xd88e); it fills an attr
// that must already be initialised, which is why init comes first here and not inside the caller.
size_t stackOfSelf(int* rcOut) {
  pthread_attr_t a;
  size_t         sz = 0;

  const int rcInit = pthread_attr_init(&a);
  if(rcInit!=0) {
    reportPoolFailure("pthread_attr_init",rcInit,
                      g_attrFailed.fetch_add(1,std::memory_order_relaxed)+1);
    *rcOut = rcInit;
    return 0;
    }

  const int rcGet = pthread_attr_get_np(pthread_self(),&a);
  if(rcGet==0)
    *rcOut = pthread_attr_getstacksize(&a,&sz);
  else
    *rcOut = rcGet;

  pthread_attr_destroy(&a);
  return sz;
  }

struct Result {
  size_t stack;
  int    rc;
  };

void* probeBody(void* p) {
  Result* r = static_cast<Result*>(p);
  r->stack  = stackOfSelf(&r->rc);
  return nullptr;
  }

// One thread, created the way the caller under test creates it. Returns 0 and leaves rc non-zero if
// the thread could not be started - a probe that cannot run must say so rather than report a zero.
size_t stackOfNewThread(const pthread_attr_t* attr, int* rcOut) {
  Result    r{0,0};
  pthread_t t;

  const int rcCreate = pthread_create(&t,attr,probeBody,&r);
  if(rcCreate!=0) { *rcOut = rcCreate; return 0; }

  pthread_join(t,nullptr);
  *rcOut = r.rc;
  return r.stack;
  }

// ------------------------------------------------------------------ the alternate signal stack
//
// See the block in the header for why this is carved out of the thread's own stack rather than
// allocated. What is here is the mechanism and the one thing the header cannot state: whether this
// kernel hands a new thread its creator's td_sigstk. INFERRED, NOT MEASURED - POSIX says the
// alternate stack is not inherited across pthread_create, and FreeBSD 9's kern_thr.c bzeroes the
// td_startzero..td_endzero range of a new thread, but sys/proc.h is not among the oracles this port
// has a copy of and the field could equally sit in the td_startcopy range. So the first thread to
// come through here READS BACK what it already had and prints it, and one boot settles it: an
// ss_sp equal to the main thread's leaked heap buffer means inherited, a zero or SS_DISABLE means
// not. Either way the install below is correct - it just says whether it was also necessary.
constexpr size_t kAltStackSize = 64*1024;

// Four times the alternate stack. A thread that cannot spare a quarter of itself is left alone.
constexpr size_t kAltStackMinThreadStack = 4*kAltStackSize;

std::atomic<unsigned long> g_altOk{0};
std::atomic<unsigned long> g_altFailed{0};
std::atomic<unsigned long> g_altSkipped{0};
std::atomic<int>           g_altReported{0};

struct StartArg {
  void* (*fn)(void*);
  void*   arg;
  };

// ⚠ noinline AND NOT MERELY A HINT. The 64 KiB array below must live in a frame that is created
// once, at the base of this thread, and stays until the thread body returns. Inlined into the
// trampoline it would still work; inlined into something the compiler decided to duplicate it
// might not, and the cost of being explicit is nothing.
__attribute__((noinline))
void* runOnItsOwnAltStack(void* (*fn)(void*), void* arg) {
  // ⚠ THE HANDLER ENTERS AT ss_sp + ss_size AND GROWS DOWN, so this array is the whole region the
  // signal frame will ever touch, and it sits above every frame `fn` will push.
  alignas(16) char alt[kAltStackSize];

  // ⚠ TOUCHED ONCE, BECAUSE THE KERNEL WILL WRITE HERE WHILE DELIVERING A SIGNAL AND THAT IS THE
  // ONE MOMENT A PAGE FAULT MUST NOT BE NEEDED. The frame above was made with a single `sub rsp`,
  // which skips over every page it crosses, and nothing between here and the handler reads or
  // writes the array. Four stores - the page here is 16 KiB (orbis_mmap.cpp:24) and this is 64 KiB
  // - make the region resident before it is ever handed to the kernel. INFERRED that it matters:
  // the thread stack IS mapped by scePthreadCreate, so a fault would very likely be serviced
  // normally; four stores is a cheaper way to not depend on that than an experiment would be.
  for(size_t i=0; i<sizeof(alt); i+=16*1024)
    static_cast<volatile char*>(alt)[i] = 0;
  static_cast<volatile char*>(alt)[sizeof(alt)-1] = 0;

  stack_t prev;
  prev.ss_sp = nullptr; prev.ss_size = 0; prev.ss_flags = 0;
  errno = 0;
  const int prevRc  = sigaltstack(nullptr,&prev);
  const int prevErr = errno;

  stack_t ss;
  ss.ss_sp    = alt;
  ss.ss_size  = sizeof(alt);
  ss.ss_flags = 0;
  errno = 0;
  const int rc  = sigaltstack(&ss,nullptr);
  const int err = errno;

  if(rc==0)
    g_altOk.fetch_add(1,std::memory_order_relaxed);
  else
    g_altFailed.fetch_add(1,std::memory_order_relaxed);

  // Once per process. Every thread after the first would print the same three numbers, and this
  // channel is UDP.
  if(g_altReported.exchange(1,std::memory_order_relaxed)==0 && orbis_log_enabled()!=0)
    orbis_log("thread alt stack: first worker thread - it INHERITED rc=%d errno=%d "
              "ss_sp=%p ss_size=%llu ss_flags=%d (0=nothing set, 4=SS_DISABLE, a non-null sp is "
              "the MAIN thread's buffer and means td_sigstk is copied on create); installed "
              "%llu KiB of this thread's own stack at %p rc=%d errno=%d - %s",
              prevRc,prevErr,prev.ss_sp,(unsigned long long)prev.ss_size,prev.ss_flags,
              (unsigned long long)(sizeof(alt)/1024),(void*)alt,rc,err,
              rc==0 ? "a stack overflow on a WORKER thread can now report itself"
                    : "REFUSED - worker threads still die silently on an overflow");

  void* r = fn(arg);

  // ⚠ THIS IS NOT DECORATION. Without it the call above is a candidate for a tail call, which
  // would pop this frame - and the kernel's td_sigstk still points into it - before `fn` runs.
  // The barrier keeps `alt` live across the call and forbids the tail call outright.
  __asm__ __volatile__("" :: "r"(alt) : "memory");

  // Deliberately NOT disabled again on the way out. The thread is about to stop existing and
  // td_sigstk goes with it, so there is nothing to dangle; and a crash during teardown is better
  // reported on a stack that is still mapped than on one that is not.
  return r;
  }

void* threadTrampoline(void* p) {
  StartArg* sa                = static_cast<StartArg*>(p);
  void*   (*fn)(void*)        = sa->fn;
  void*     arg               = sa->arg;
  free(sa);

  // The ground truth rather than what the creator asked for: the interposer may have raised the
  // request, refused to, or stood down entirely. pthread_attr_get_np on self is the same call the
  // probe uses, and it is measured working on this console.
  int          rc = 0;
  const size_t sz = stackOfSelf(&rc);

  if(rc==0 && sz>=kAltStackMinThreadStack)
    return runOnItsOwnAltStack(fn,arg);

  g_altSkipped.fetch_add(1,std::memory_order_relaxed);
  return fn(arg);
  }

// scePthreadCreate, plus the one thing a trampoline adds: a payload to free when the thread that
// was going to free it never starts.
int createThread(pthread_t* thread, const pthread_attr_t* attr,
                 void* (*entry)(void*), void* entryArg, StartArg* sa) {
  const int rc = scePthreadCreate(thread,attr,entry,entryArg,"orbis");
  if(rc!=0) {
    // The payload the thread that never started was going to free. Freed here whether or not
    // anything is logged - the report is diagnostics, this is correctness.
    if(sa!=nullptr)
      free(sa);
    reportPoolFailure("scePthreadCreate",rc,
                      g_createFailed.fetch_add(1,std::memory_order_relaxed)+1);
    }
  return rc;
  }

}

namespace {

size_t mainThreadStack() {
  int          rc = 0;
  const size_t sz = stackOfSelf(&rc);
  return (rc==0) ? sz : 0;
  }

}

size_t orbis::threadDefaultStack() {
  return g_defaultStack;
  }

size_t orbis::threadStackFloor() {
  return resolveFloor();
  }

void orbis::threadFailures(unsigned long* createFailed, unsigned long* attrFailed) {
  if(createFailed!=nullptr) *createFailed = g_createFailed.load(std::memory_order_relaxed);
  if(attrFailed  !=nullptr) *attrFailed   = g_attrFailed.load(std::memory_order_relaxed);
  }

void orbis::threadCounts(unsigned long* created, unsigned long* raised) {
  if(created!=nullptr) *created = g_created.load(std::memory_order_relaxed);
  if(raised !=nullptr) *raised  = g_raised.load(std::memory_order_relaxed);
  }

void orbis::threadAltStacks(unsigned long* installed, unsigned long* failed,
                            unsigned long* skipped) {
  if(installed!=nullptr) *installed = g_altOk.load(std::memory_order_relaxed);
  if(failed   !=nullptr) *failed    = g_altFailed.load(std::memory_order_relaxed);
  if(skipped  !=nullptr) *skipped   = g_altSkipped.load(std::memory_order_relaxed);
  }

size_t orbis::threadAltStackSize() {
  return kAltStackSize;
  }

// ------------------------------------------------------------------ the interposer
//
// ⚠ THIS DEFINITION WINS OVER libkernel's, and that is a linker rule rather than a trick: a symbol
// defined in an object file is preferred to one from a shared library. Measured on a throwaway
// project before it was relied on here - the link succeeds, and the caller's call lands on this
// function rather than on libkernel.so:0xda78.
//
// ⚠ IT MUST BE PULLED IN BY --whole-archive. Nothing REFERENCES it: every caller already has
// pthread_create resolved from libkernel unless this object is in the link, and an archive member
// nobody references is an archive member the linker drops.
extern "C" int pthread_create(pthread_t* thread, const pthread_attr_t* attr,
                              void* (*start)(void*), void* arg) {
  g_created.fetch_add(1,std::memory_order_relaxed);

  // ⚠ THE TRAMPOLINE IS INSTALLED FIRST AND FOR EVERY PATH BELOW, INCLUDING THE ONE THAT RAISES
  // NOTHING. The stack floor and the alternate signal stack are separate questions: a caller that
  // chose its own 8 MB stack wants no interference with the size and still wants an overflow on
  // that thread to be reportable. The only reason to skip is a stack too small to spare 64 KiB,
  // and that is decided inside the trampoline where the real size is knowable.
  //
  // Two pointers do not fit in one, so the payload is malloc'd and freed by the thread itself on
  // its first instruction - or by createThread() when the thread never starts. If the malloc
  // fails, the caller's start routine is used unchanged: an interposer that cannot do its extra
  // job must still do the original one.
  StartArg* sa                = static_cast<StartArg*>(malloc(sizeof(StartArg)));
  void*   (*entry)(void*)     = start;
  void*     entryArg          = arg;
  if(sa!=nullptr) {
    sa->fn   = start;
    sa->arg  = arg;
    entry    = &threadTrampoline;
    entryArg = sa;
    }

  const size_t floor = resolveFloor();
  if(floor==0)
    return createThread(thread,attr,entry,entryArg,sa);

  // What did the caller ask for? A fresh attr reports the platform default here (65536, measured),
  // so this cannot distinguish "asked for the default" from "asked for nothing" - see the header for
  // why overriding both is the right call.
  size_t want = 0;
  if(attr!=nullptr && pthread_attr_getstacksize(attr,&want)!=0)
    want = 0;

  if(want>=floor)
    return createThread(thread,attr,entry,entryArg,sa);

  // Raise it. The caller's attr is const and may be reused for other threads, so the change goes on
  // a copy that lives exactly as long as this call.
  pthread_attr_t raised;
  const int rcAttr = pthread_attr_init(&raised);
  if(rcAttr!=0) {
    reportPoolFailure("pthread_attr_init",rcAttr,
                      g_attrFailed.fetch_add(1,std::memory_order_relaxed)+1);
    return createThread(thread,attr,entry,entryArg,sa);
    }

  if(attr!=nullptr) {
    // Carry across what the caller DID choose. Anything not copied here is left at this platform's
    // default, which is what the caller would have got had it passed no attr at all.
    int    detach = 0;
    size_t guard  = 0;
    if(pthread_attr_getdetachstate(attr,&detach)==0) pthread_attr_setdetachstate(&raised,detach);
    if(pthread_attr_getguardsize(attr,&guard)==0)    pthread_attr_setguardsize(&raised,guard);
    }

  const int rcSet = pthread_attr_setstacksize(&raised,floor);
  const int rc    = (rcSet==0) ? createThread(thread,&raised,entry,entryArg,sa)
                               : createThread(thread,attr,entry,entryArg,sa);
  if(rcSet==0 && rc==0)
    g_raised.fetch_add(1,std::memory_order_relaxed);

  pthread_attr_destroy(&raised);

  // Nothing in a consumer calls the census after boot, so the interposer reports itself - at powers
  // of two, which is enough to see the shape of a run (1, 2, 4 ... 64) without becoming the log.
  const unsigned long n = g_created.load(std::memory_order_relaxed);
  if(rc==0 && (n & (n-1))==0)
    orbis::memCensusThreads("live");

  return rc;
  }

void orbis::threadStackProbe() {
  if(!orbis_log_enabled())
    return;

  // 1. What a fresh attr CLAIMS, before anyone sets anything on it. If this equals the size threads
  //    really get, then "the caller chose the default" and "the caller chose nothing" look identical
  //    to an interposer, and the policy has to be built around that.
  pthread_attr_t fresh;
  size_t         claimed = 0;
  int            rcFresh = pthread_attr_init(&fresh);
  int            rcClaim = (rcFresh==0) ? pthread_attr_getstacksize(&fresh,&claimed) : rcFresh;

  // 2. What the calling thread - main, created by the loader rather than by us - actually has.
  int          rcSelf = 0;
  const size_t self   = stackOfSelf(&rcSelf);

  // 3. What a thread created with NO attributes gets. This is the number the plan calls the default.
  int          rcNull = 0;
  const size_t viaNull = stackOfNewThread(nullptr,&rcNull);

  // 4. What a thread created with a default-INITIALISED attr gets - dEQP's shape, and the one that
  //    died. If 3 and 4 differ, the interposer has two cases to handle rather than one.
  int          rcAttr = 0;
  const size_t viaAttr = (rcFresh==0) ? stackOfNewThread(&fresh,&rcAttr) : 0;

  if(rcFresh==0)
    pthread_attr_destroy(&fresh);

  g_defaultStack = viaNull;

  orbis_log("thread probe: fresh attr claims %llu B (rc %d/%d), main thread has %llu B (rc %d)",
            (unsigned long long)claimed,rcFresh,rcClaim,
            (unsigned long long)self,rcSelf);
  orbis_log("thread probe: attr=NULL gives %llu B (rc %d), default-init attr gives %llu B (rc %d), "
            "PTHREAD_STACK_MIN %d",
            (unsigned long long)viaNull,rcNull,
            (unsigned long long)viaAttr,rcAttr,
            (int)PTHREAD_STACK_MIN);

  // ⚠ With the interposer active, 3 and 4 above measure the POLICY rather than the platform: the
  // probe's own threads go through pthread_create like everyone else's. That makes them a self-check
  // - they should now report the floor - and the platform's raw 65536 is recorded in the header,
  // measured before the interposer existed.
  const size_t floor = resolveFloor();
  orbis_log("thread probe: floor is %llu KiB (%s)",
            (unsigned long long)(floor/1024),
            (floor==0) ? "interposer DISABLED, ORBIS_THREAD_STACK=0"
                       : "ORBIS_THREAD_STACK, or the main thread's own size");

  // The verdict, stated so the log answers the question instead of only carrying the numbers.
  if(viaNull!=0 && viaNull<256*1024)
    orbis_log("thread probe: VERDICT threads get %llu KiB - a pipeline compile wants ~72 KB of "
              "frame, so every thread created without an explicit size is one deep call from the "
              "guard page. THE INTERPOSER IS NOT DOING ITS JOB.",
              (unsigned long long)(viaNull/1024));
  else if(viaNull!=0)
    orbis_log("thread probe: VERDICT threads get %llu KiB - the platform's own 64 KiB has been "
              "raised, and a shader compile fits",(unsigned long long)(viaNull/1024));
  else
    orbis_log("thread probe: VERDICT INCONCLUSIVE - the probe could not read a thread's stack "
              "(rc %d); pthread_attr_get_np may not do what its export suggests",rcNull);
  }
