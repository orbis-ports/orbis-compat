// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// See include/orbis_timer.h.
#include <orbis_timer.h>

#include <orbis_log.h>

#include <signal.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <atomic>

namespace {

std::atomic<int> g_fired{0};

void onTimer(union sigval) {
  g_fired.fetch_add(1,std::memory_order_relaxed);
  }

void sleepMs(long ms) {
  struct timespec ts;
  ts.tv_sec  = ms/1000;
  ts.tv_nsec = (ms%1000)*1000000L;
  nanosleep(&ts,nullptr);
  }

// ⚠ WHICH CLOCK DOES timer_create ACCEPT? The umtx work found this platform's condition variables
// measure their deadlines against CLOCK_REALTIME whatever they are asked for, so the clock is not a
// detail to assume. Try MONOTONIC, fall back to REALTIME, and say which one answered.
int createOn(struct sigevent* ev, timer_t* t, const char** clockName) {
  if(timer_create(CLOCK_MONOTONIC,ev,t)==0) { *clockName = "CLOCK_MONOTONIC"; return 0; }
  const int e1 = errno;
  if(timer_create(CLOCK_REALTIME,ev,t)==0)  { *clockName = "CLOCK_REALTIME";  return 0; }
  *clockName = "neither";
  return e1;
  }

// Question 1: does a timer run AT ALL? SIGEV_NONE asks for no notification, so the answer depends on
// nothing but the kernel's timer machinery - and timer_gettime says how much is left. A timer armed
// for 100 ms and read after 250 ms should have nothing left.
void probeCountdown() {
  struct sigevent ev;
  memset(&ev,0,sizeof(ev));
  ev.sigev_notify = SIGEV_NONE;

  timer_t     t     = nullptr;
  const char* clock = "";
  const int   rc    = createOn(&ev,&t,&clock);
  if(rc!=0) {
    orbis_log("timer probe: SIGEV_NONE timer_create FAILED errno %d - this platform has no usable "
              "POSIX timer at all, and the rest of this probe would be measuring nothing",rc);
    return;
    }

  struct itimerspec its;
  memset(&its,0,sizeof(its));
  its.it_value.tv_nsec = 100*1000000L;

  const int rcSet = timer_settime(t,0,&its,nullptr);
  sleepMs(250);

  struct itimerspec left;
  memset(&left,0,sizeof(left));
  const int rcGet = timer_gettime(t,&left);
  const int over  = timer_getoverrun(t);

  orbis_log("timer probe: countdown on %s - settime rc %d, gettime rc %d, %lld.%09ld s left, "
            "overrun %d",clock,rcSet,rcGet,
            (long long)left.it_value.tv_sec,(long)left.it_value.tv_nsec,over);

  if(rcGet==0 && left.it_value.tv_sec==0 && left.it_value.tv_nsec==0)
    orbis_log("timer probe: VERDICT the kernel's timers RUN - a 100 ms timer read at 250 ms has "
              "expired");
  else
    orbis_log("timer probe: VERDICT the kernel's timers DO NOT run, or do not count down where this "
              "can see it - anything below is inconclusive");

  timer_delete(t);
  }

// Question 2: does SIGEV_THREAD deliver? This is the one dEQP needs, and the one the header's macros
// only made compilable.
void probeThreadNotify() {
  g_fired.store(0,std::memory_order_relaxed);

  struct sigevent ev;
  memset(&ev,0,sizeof(ev));
  ev.sigev_notify            = SIGEV_THREAD;
  ev.sigev_notify_function   = onTimer;
  ev.sigev_notify_attributes = nullptr;
  ev.sigev_value.sival_ptr   = nullptr;

  timer_t     t     = nullptr;
  const char* clock = "";
  const int   rc    = createOn(&ev,&t,&clock);
  if(rc!=0) {
    orbis_log("timer probe: SIGEV_THREAD timer_create FAILED errno %d - the notification type is "
              "refused at creation, so the CTS's deTimer.c patch stays and this is WHY",rc);
    return;
    }

  struct itimerspec its;
  memset(&its,0,sizeof(its));
  its.it_value.tv_nsec = 100*1000000L;
  const int rcSet = timer_settime(t,0,&its,nullptr);

  // At most a second, in short naps, so a platform that delivers late is not recorded as silent.
  int waited = 0;
  while(g_fired.load(std::memory_order_relaxed)==0 && waited<1000) {
    sleepMs(50);
    waited += 50;
    }

  const int fired = g_fired.load(std::memory_order_relaxed);
  orbis_log("timer probe: SIGEV_THREAD on %s - settime rc %d, fired %d time(s) after %d ms",
            clock,rcSet,fired,waited);

  if(fired>0)
    orbis_log("timer probe: VERDICT SIGEV_THREAD DELIVERS - dEQP's POSIX timer arm works here, and "
              "the CTS's deTimer.c patch can go");
  else
    orbis_log("timer probe: VERDICT SIGEV_THREAD IS SILENT - it is accepted at creation and never "
              "calls back, which is the worst shape: code that compiles and hangs. The CTS's "
              "deTimer.c patch STAYS, and signal.h's macros are for compilation only");

  timer_delete(t);
  }

}

void orbis::timerProbe() {
  if(!orbis_log_enabled())
    return;

  probeCountdown();

  // ⚠ ANSWERED, AT THE COST OF ONE BLACK SCREEN, 2026-08-19 16:34:34. timer_create with
  // SIGEV_THREAD DOES NOT RETURN on this kernel. Not an error, not a silent timer: the calling
  // thread never comes back, the title never reaches its menu, and the klog records no fault of any
  // kind because nothing faulted. The log stops mid-probe, one line after the countdown verdict.
  //
  // So it stays behind a knob, off by default. The answer is already in the tree - nobody needs to
  // re-run it, and anybody who does gets a hung console rather than a measurement.
  const char* e = getenv("ORBIS_TIMER_PROBE");
  if(e!=nullptr && e[0]=='1') {
    orbis_log("timer probe: ORBIS_TIMER_PROBE=1 - about to call timer_create(SIGEV_THREAD), which "
              "HUNG this title on 2026-08-19. If this is the last line you see, that is why");
    probeThreadNotify();
    } else {
    orbis_log("timer probe: SIGEV_THREAD not attempted - measured 2026-08-19, timer_create with it "
              "NEVER RETURNS here. ORBIS_TIMER_PROBE=1 to try anyway");
    }
  }
