// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// See include/orbis_thread.h.
#include <orbis_thread.h>

#include <orbis_log.h>
#include <orbis_mem.h>

#include <pthread.h>
#include <pthread_np.h>
#include <limits.h>
#include <stdlib.h>
#include <stddef.h>

#include <atomic>

// libkernel's own creator, which the SDK's pthread_create (0xda78) is itself a wrapper over. Calling
// it directly is what lets this file define pthread_create without recursing into itself.
extern "C" int scePthreadCreate(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *,
                                const char *);

namespace {

size_t g_defaultStack = 0;

std::atomic<unsigned long> g_created{0};
std::atomic<unsigned long> g_raised{0};

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
  if(rcInit!=0) { *rcOut = rcInit; return 0; }

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

void orbis::threadCounts(unsigned long* created, unsigned long* raised) {
  if(created!=nullptr) *created = g_created.load(std::memory_order_relaxed);
  if(raised !=nullptr) *raised  = g_raised.load(std::memory_order_relaxed);
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

  const size_t floor = resolveFloor();
  if(floor==0)
    return scePthreadCreate(thread,attr,start,arg,"orbis");

  // What did the caller ask for? A fresh attr reports the platform default here (65536, measured),
  // so this cannot distinguish "asked for the default" from "asked for nothing" - see the header for
  // why overriding both is the right call.
  size_t want = 0;
  if(attr!=nullptr && pthread_attr_getstacksize(attr,&want)!=0)
    want = 0;

  if(want>=floor)
    return scePthreadCreate(thread,attr,start,arg,"orbis");

  // Raise it. The caller's attr is const and may be reused for other threads, so the change goes on
  // a copy that lives exactly as long as this call.
  pthread_attr_t raised;
  if(pthread_attr_init(&raised)!=0)
    return scePthreadCreate(thread,attr,start,arg,"orbis");

  if(attr!=nullptr) {
    // Carry across what the caller DID choose. Anything not copied here is left at this platform's
    // default, which is what the caller would have got had it passed no attr at all.
    int    detach = 0;
    size_t guard  = 0;
    if(pthread_attr_getdetachstate(attr,&detach)==0) pthread_attr_setdetachstate(&raised,detach);
    if(pthread_attr_getguardsize(attr,&guard)==0)    pthread_attr_setguardsize(&raised,guard);
    }

  const int rcSet = pthread_attr_setstacksize(&raised,floor);
  const int rc    = (rcSet==0) ? scePthreadCreate(thread,&raised,start,arg,"orbis")
                               : scePthreadCreate(thread,attr,start,arg,"orbis");
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
