// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// SIGEV_THREAD, delivered by a thread of ours. See include/orbis_timer.h and PLAN.md §9.
//
// ⚠ WHY: timer_create with SIGEV_THREAD NEVER RETURNS on this kernel - measured twice on
// 2026-08-19, the second time with our own pthread_create interposer disabled so the platform could
// not be blamed for something this repository did. The title never reached its menu, and the klog
// carried no fault because nothing faulted.
//
// SIGEV_THREAD IS A USERSPACE CONSTRUCTION EVERYWHERE. Linux does not call user functions from the
// kernel either; glibc runs a helper thread. This is the same construction over the two things the
// probe proved this platform does provide: a SIGEV_NONE timer that counts down, and threads.
//
// ---------------------------------------------------------------- why all five, and why ktimer_*
//
// Once the handle a caller holds is ours, the other four calls have to understand it - so all five
// are interposed. And interposing timer_create makes musl's own unreachable, so the pass-through for
// ordinary timers goes to ktimer_*, which libkernel exports (ktimer_create at 0xd730) and which is
// what musl's wrappers call anyway.
//
// ⚠ THE PASS-THROUGH USES musl's OWN HANDLE ENCODING, which is the one thing here that had to be
// read out of a disassembly rather than a header. musl's timer_settime decodes:
//
//     testq %rdi,%rdi ; jns <plain> ; movl 0xa0(%rdi,%rdi),%edi ; andl $0x7fffffff,%edi
//
// - a non-thread timer_t IS the kernel id, sign bit clear. Ours are pointers into a static table,
// which the range check below distinguishes without guessing, and which never reach musl anyway.
#include <orbis_timer.h>

#include <orbis_log.h>

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <atomic>
#include <cstdint>

// ⚠ THE KERNEL'S sigevent IS NOT THE SDK'S, AND THIS COST ONE CONSOLE RUN. Passing the public
// struct sigevent straight to ktimer_create returns EINVAL for every notification type - including
// SIGEV_NONE, which had worked minutes earlier through musl's own wrapper. musl translates, and the
// translation is a REORDERING rather than a renumbering. Read out of its disassembly:
//
//     movl (%rsi),%eax        ; notify  <- SDK offset 0
//     movq 0x8(%r14),%rcx     ; value   <- SDK offset 8
//     movq %rcx,(%rsi)        ; ksev+0x00 = value        (8 bytes)
//     movl 0x4(%r14),%ecx     ; signo   <- SDK offset 4
//     movl %ecx,0x8(%rsi)     ; ksev+0x08 = signo
//     movl %eax,0xc(%rsi)     ; ksev+0x0c = notify       - value UNCHANGED, musl's own numbering
//     movl $0x0,0x10(%rsi)    ; ksev+0x10 = 0
//
// So SIGEV_NONE stays 1 and SIGEV_SIGNAL stays 0; only the field order differs. The first attempt
// here assumed FreeBSD's numbering was the difference and was wrong about which half of the problem
// it was - the log said EINVAL for BOTH types, which a renumbering alone would not explain.
struct KSigevent {
  uint64_t value;      // 0x00 - sigev_value, as one quadword
  int32_t  signo;      // 0x08
  int32_t  notify;     // 0x0c - musl's numbering, passed through
  int32_t  zero;       // 0x10
  int32_t  pad;        // musl leaves this uninitialised; zeroing costs nothing and reads better
  };

// libkernel's raw timer layer, under musl's wrappers. Nothing declares them.
extern "C" {
int ktimer_create(clockid_t, KSigevent*, int*);
int ktimer_settime(int, int, const struct itimerspec*, struct itimerspec*);
int ktimer_gettime(int, struct itimerspec*);
int ktimer_getoverrun(int);
int ktimer_delete(int);
}

namespace {

// ⚠ A FIXED TABLE, AND A SMALL ONE: every entry costs a thread. Bounded means timer_create can fail
// cleanly rather than this file allocating without limit on a console with no swap.
constexpr unsigned kMaxTimers = 8;

struct Slot {
  bool            used  = false;
  int             under = -1;                    // the SIGEV_NONE timer, for gettime/getoverrun
  void          (*fn)(union sigval) = nullptr;
  union sigval    val{};
  pthread_t       thread{};
  pthread_mutex_t lock  = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
  bool            armed = false;
  bool            stop  = false;
  struct timespec value{};                       // absolute deadline
  struct timespec interval{};
  };

Slot            g_slot[kMaxTimers];
pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
std::atomic<unsigned long> g_fired{0};

bool enabled() {
  const char* e = getenv("ORBIS_SIGEV_THREAD");
  return !(e!=nullptr && e[0]=='0');
  }

// Ours exactly when it points into the table. No tag bits: the comparison is against an array this
// file owns, so a kernel id can never be mistaken for one of ours.
Slot* slotOf(timer_t t) {
  Slot* s = static_cast<Slot*>(t);
  return (s>=&g_slot[0] && s<&g_slot[kMaxTimers] && s->used) ? s : nullptr;
  }

int idOf(timer_t t)      { return int(reinterpret_cast<intptr_t>(t)); }
timer_t handleOf(int id) { return reinterpret_cast<timer_t>(intptr_t(id)); }

bool isZero(const struct timespec& t) { return t.tv_sec==0 && t.tv_nsec==0; }

void addTo(struct timespec& t, const struct timespec& d) {
  t.tv_sec  += d.tv_sec;
  t.tv_nsec += d.tv_nsec;
  if(t.tv_nsec>=1000000000L) { t.tv_sec += 1; t.tv_nsec -= 1000000000L; }
  }

// ⚠ CLOCK_REALTIME, AND NOT AS A PREFERENCE. timer_create refuses CLOCK_MONOTONIC here (measured),
// and this platform's condition variables measure their deadlines against CLOCK_REALTIME whatever
// they are asked for (measured earlier, in the umtx work). Both point the same way, so the wait
// below and the timer underneath agree about what time it is.
void nowRealtime(struct timespec& t) { clock_gettime(CLOCK_REALTIME,&t); }

void* body(void* arg) {
  Slot* s = static_cast<Slot*>(arg);
  pthread_mutex_lock(&s->lock);
  for(;;) {
    while(!s->armed && !s->stop)
      pthread_cond_wait(&s->cond,&s->lock);
    if(s->stop)
      break;

    // Absolute deadline, so a re-arm or a spurious wake just recomputes and nothing drifts.
    const int rc = pthread_cond_timedwait(&s->cond,&s->lock,&s->value);
    if(s->stop)
      break;
    if(rc!=ETIMEDOUT)
      continue;

    struct timespec now;
    nowRealtime(now);
    if(now.tv_sec<s->value.tv_sec ||
       (now.tv_sec==s->value.tv_sec && now.tv_nsec<s->value.tv_nsec))
      continue;

    void (*fn)(union sigval) = s->fn;
    const union sigval val   = s->val;
    if(isZero(s->interval))
      s->armed = false;
    else
      addTo(s->value,s->interval);

    // ⚠ THE CALLBACK RUNS WITHOUT THE LOCK. It is the caller's code: it may re-arm the timer, delete
    // it, or block for as long as it likes, and holding the lock across that is a deadlock waiting
    // for a reason to happen.
    pthread_mutex_unlock(&s->lock);
    if(fn!=nullptr) {
      fn(val);
      g_fired.fetch_add(1,std::memory_order_relaxed);
      }
    pthread_mutex_lock(&s->lock);
    }
  pthread_mutex_unlock(&s->lock);
  return nullptr;
  }

}

void orbis::timerCounts(unsigned* live, unsigned long* fired) {
  if(live!=nullptr) {
    unsigned n = 0;
    pthread_mutex_lock(&g_lock);
    for(unsigned i=0; i<kMaxTimers; ++i)
      if(g_slot[i].used) ++n;
    pthread_mutex_unlock(&g_lock);
    *live = n;
    }
  if(fired!=nullptr)
    *fired = g_fired.load(std::memory_order_relaxed);
  }

// ------------------------------------------------------------------ the five

namespace {

// The one place the SDK's sigevent becomes the kernel's. See the table above the declarations.
void toKernel(const struct sigevent* ev, KSigevent& k) {
  memset(&k,0,sizeof(k));
  if(ev==nullptr)
    return;
  memcpy(&k.value,&ev->sigev_value,sizeof(k.value));
  k.signo  = ev->sigev_signo;
  k.notify = ev->sigev_notify;
  }

}

extern "C" int timer_create(clockid_t clk, struct sigevent* ev, timer_t* out) {
  if(ev==nullptr || ev->sigev_notify!=SIGEV_THREAD) {
    KSigevent k;
    toKernel(ev,k);
    int id = -1;
    const int rc = ktimer_create(clk,(ev!=nullptr) ? &k : nullptr,&id);
    if(rc==0 && out!=nullptr)
      *out = handleOf(id);
    return rc;
    }

  if(!enabled()) {
    // Refusing is not what the platform does - the platform hangs. A caller can handle this.
    errno = ENOTSUP;
    return -1;
    }

  pthread_mutex_lock(&g_lock);
  Slot* s = nullptr;
  for(unsigned i=0; i<kMaxTimers && s==nullptr; ++i)
    if(!g_slot[i].used) s = &g_slot[i];
  if(s!=nullptr) s->used = true;
  pthread_mutex_unlock(&g_lock);

  if(s==nullptr) { errno = EAGAIN; return -1; }

  // The timer underneath asks for NO notification - the one mode that works here - and exists so
  // gettime and getoverrun keep answering out of the kernel rather than out of our arithmetic.
  KSigevent none;
  memset(&none,0,sizeof(none));
  none.notify = SIGEV_NONE;      // musl's numbering, which is what the kernel is given

  int id = -1;
  if(ktimer_create(clk,&none,&id)!=0) {
    pthread_mutex_lock(&g_lock); s->used = false; pthread_mutex_unlock(&g_lock);
    return -1;
    }

  s->under = id;
  s->fn    = ev->sigev_notify_function;
  s->val   = ev->sigev_value;
  s->armed = false;
  s->stop  = false;
  memset(&s->value,0,sizeof(s->value));
  memset(&s->interval,0,sizeof(s->interval));

  if(pthread_create(&s->thread,nullptr,body,s)!=0) {
    ktimer_delete(id);
    pthread_mutex_lock(&g_lock); s->used = false; pthread_mutex_unlock(&g_lock);
    errno = EAGAIN;
    return -1;
    }

  if(out!=nullptr)
    *out = static_cast<timer_t>(s);
  orbis_log("timer: SIGEV_THREAD timer created - kernel timer %d underneath, notification on a "
            "thread of ours (PLAN.md 9)",id);
  return 0;
  }

extern "C" int timer_settime(timer_t t, int flags, const struct itimerspec* in,
                             struct itimerspec* old) {
  Slot* s = slotOf(t);
  if(s==nullptr)
    return ktimer_settime(idOf(t),flags,in,old);

  // The kernel timer is armed identically, so timer_gettime keeps telling the truth about it.
  ktimer_settime(s->under,flags,in,old);

  pthread_mutex_lock(&s->lock);
  if(in==nullptr || isZero(in->it_value)) {
    s->armed = false;
    } else {
    struct timespec deadline{};
    if((flags & TIMER_ABSTIME)!=0) {
      deadline = in->it_value;
      } else {
      nowRealtime(deadline);
      addTo(deadline,in->it_value);
      }
    s->value    = deadline;
    s->interval = in->it_interval;
    s->armed    = true;
    }
  pthread_cond_signal(&s->cond);
  pthread_mutex_unlock(&s->lock);
  return 0;
  }

extern "C" int timer_gettime(timer_t t, struct itimerspec* out) {
  Slot* s = slotOf(t);
  return ktimer_gettime((s!=nullptr) ? s->under : idOf(t), out);
  }

extern "C" int timer_getoverrun(timer_t t) {
  Slot* s = slotOf(t);
  return ktimer_getoverrun((s!=nullptr) ? s->under : idOf(t));
  }

extern "C" int timer_delete(timer_t t) {
  Slot* s = slotOf(t);
  if(s==nullptr)
    return ktimer_delete(idOf(t));

  pthread_mutex_lock(&s->lock);
  s->stop  = true;
  s->armed = false;
  pthread_cond_signal(&s->cond);
  pthread_mutex_unlock(&s->lock);
  pthread_join(s->thread,nullptr);

  const int rc = ktimer_delete(s->under);
  pthread_mutex_lock(&g_lock);
  s->used  = false;
  s->under = -1;
  s->fn    = nullptr;
  pthread_mutex_unlock(&g_lock);
  return rc;
  }
