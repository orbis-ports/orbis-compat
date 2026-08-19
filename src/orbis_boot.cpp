// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// See include/orbis_boot.h.
//
// ⚠ MOVED VERBATIM from OpenGothic ps4/og_ps4_boot.cpp on 2026-08-19, except for three renames a
// move out of an engine cannot avoid: ps4_log -> orbis_log, ps4_log_fatal -> orbis_log_fatal, and
// ps4_idle_forever -> orbis_fatal_action. The comments are the originals and say what they measured.
#include <orbis_boot.h>

#include <orbis_log.h>

#include <clocale>
#include <cctype>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <system_error>

namespace {

[[noreturn]] static void ps4TerminateHandler() {
  char msg[512] = "std::terminate with no active exception";
  if(std::exception_ptr p = std::current_exception()) {
    try {
      std::rethrow_exception(p);
      }
    catch(const std::system_error& e) {
      std::snprintf(msg,sizeof(msg),"std::system_error(%s)",e.what());
      }
    catch(const std::runtime_error& e) {
      std::snprintf(msg,sizeof(msg),"std::runtime_error(%s)",e.what());
      }
    catch(const std::logic_error& e) {
      std::snprintf(msg,sizeof(msg),"std::logic_error(%s)",e.what());
      }
    catch(const std::bad_alloc& e) {
      std::snprintf(msg,sizeof(msg),"std::bad_alloc(%s)",e.what());
      }
    catch(const std::exception& e) {
      // GothicNotFoundException lands here: OpenGothic derives it from std::runtime_error
      // via Tempest, and CrashLog's own handler abort()s on it by name without printing.
      std::snprintf(msg,sizeof(msg),"%s","std::exception(");
      std::snprintf(msg+strlen(msg),sizeof(msg)-strlen(msg),"%s)",e.what());
      }
    catch(...) {
      std::snprintf(msg,sizeof(msg),"%s","a non-std exception");
      }
    }
  // The fatal channel, for the reason it exists: this line describes a process that is about to
  // stop existing, and a datagram needs to survive long enough to be sent.
  orbis_log_fatal("fatal: unhandled exception - %s",msg);
  orbis_fatal_action("terminate");
  // Only reached when the host asked for autoexit; a console never gets here.
  _Exit(1);
  }

static const char* signalName(int sig) {
  switch(sig) {
    case SIGSEGV: return "SIGSEGV (bad address)";
    case SIGBUS:  return "SIGBUS (unaligned or unmapped)";
    case SIGFPE:  return "SIGFPE";
    case SIGILL:  return "SIGILL (illegal instruction)";
    case SIGABRT: return "SIGABRT";
    }
  return "?";
  }

// siginfo, not just the number. The FAULTING ADDRESS is what separates the three things this
// project can plausibly die of, and the bare signal number separates none of them:
//
//   si_addr just below a thread's stack        a stack overflow
//   si_addr at the instruction pointer         an illegal instruction - a CPU this binary was
//                                              compiled for does not have
//   si_addr anywhere else                      an ordinary bad pointer
//
// The console's own klog reported this crash as `App Crash reason=0x4` with
// `[gpudump] This is NOT Gpu crash`, and OUR handler printed nothing at all - so the first
// question is whether the handler runs, and the second is what it can say when it does.
static void ps4SignalAction(int sig, siginfo_t* info, void* uctx) {
  (void)uctx;
  // Not async-signal-safe, and deliberately so: a homebrew title that dies quietly is
  // a bug report with no content. Re-raising would give the console the CE-34878-0
  // dialog and nothing else.
  static volatile int reentered = 0;
  if(reentered!=0)
    _Exit(2);
  reentered = 1;
  // orbis_log_fatal and not orbis_log: the ordinary channel is whatever the application chose,
  // and in this port's case it writes UDP only - a datagram from a process the kernel is about to
  // kill may never leave the machine. The fatal one is registered precisely to write synchronously
  // somewhere that survives.
  orbis_log_fatal("fatal: signal %d - %s, si_code %d, fault address %p",
                sig,signalName(sig),
                info!=nullptr ? info->si_code : 0,
                info!=nullptr ? info->si_addr : nullptr);
  orbis_fatal_action("signal");
  _Exit(2);
  }


}

// ------------------------------------------------------------------ ctype probe

// Does case folding work on this platform?
//
// This is not a general health check, it is aimed at one specific failure. ZenKit's whole
// VFS lookup is case-insensitive by way of std::tolower: `icompare` ORDERS the child set
// with `tolower(a) < tolower(b)` and `iequals` VERIFIES the hit with `tolower(a) ==
// tolower(b)` (lib/ZenKit/src/Misc.cc:11-21). OpenGothic asks for `font_old_20_white.fnt`
// in lower case; every entry in Textures.vdf is upper case (`FONT_OLD_20_WHITE.FNT`,
// verified by parsing the catalog directly). If std::tolower were the identity here, both
// functions silently become case-SENSITIVE, the set is ordered by raw bytes, the binary
// search for a lowercase key misses an uppercase entry, and Vfs::find returns nullptr -
// which is exactly, and only, what `implLoadFont` reports as
// `failed to open resource: font_old_20_white.fnt` (game/resources.cpp:769-771: the
// message is emitted when `entry == nullptr`, so it is a LOOKUP failure and open_read()
// is never reached - no mmap, no paging, no read is involved).
//
// std::tolower answers out of the C locale's ctype tables, and nothing in this title ever
// calls setlocale(). That is fine on glibc, whose default tables are populated; it is a
// question worth asking of the SDK's musl. The probe is three calls and it runs at boot,
// so unlike the font path itself - which needs seven presented frames to reach - it is
// observable on BOTH legs.
void orbis::probeCtype() {
  const int la = std::tolower('A'), lz = std::tolower('Z');
  const int ua = std::toupper('a'), uz = std::toupper('z');
  const char* loc = setlocale(LC_ALL,nullptr);
  orbis_log("boot: ctype probe: tolower('A')=%d('%c') tolower('Z')=%d('%c') "
          "toupper('a')=%d('%c') toupper('z')=%d('%c') isspace(' ')=%d locale=%s",
          la,(la>=32&&la<127)?char(la):'?', lz,(lz>=32&&lz<127)?char(lz):'?',
          ua,(ua>=32&&ua<127)?char(ua):'?', uz,(uz>=32&&uz<127)?char(uz):'?',
          std::isspace(' ') ? 1 : 0, loc!=nullptr ? loc : "(null)");

  const bool folds = (la=='a' && lz=='z' && ua=='A' && uz=='Z');
  if(folds) {
    orbis_log("boot: ctype probe - VERDICT: case folding works. ZenKit's case-insensitive "
            "VFS lookup is sound; a failed resource lookup is NOT a folding problem.");
    } else {
    orbis_log("boot: ctype probe - VERDICT: CASE FOLDING IS BROKEN. std::tolower is not "
            "folding ASCII, so ZenKit's icompare/iequals are case-SENSITIVE. Every "
            "lower-case lookup into an upper-case VDFS catalog - which is every font, "
            "every texture, every resource OpenGothic names in lower case - misses.");
    }
  }

void orbis::installCrashHandlers() {
  std::set_terminate(&ps4TerminateHandler);

  // AN ALTERNATE STACK, and without it a stack overflow is UNREPORTABLE: the handler needs
  // stack to run and the overflow is precisely the absence of it, so the fault recurses into
  // the kernel and the process dies with nothing said. That was the leading hypothesis for
  // this crash until klog named it otherwise, and it costs 64 KiB to remove from the list of
  // things a silence can mean. Leaked on purpose - it has to outlive everything.
  static stack_t alt = {};
  alt.ss_size  = 64*1024;
  alt.ss_sp    = new char[alt.ss_size];
  alt.ss_flags = 0;
  const int altRc = sigaltstack(&alt,nullptr);

  struct sigaction sa = {};
  // `sa.sa_sigaction` DOES NOT COMPILE AS THE SDK DEFINES IT - signal.h:136 names the union member
  // `__sa_sigaction` while :142 defines the macro as `__sa_handler.sa_sigaction`, one underscore
  // pair short. The code that lived here reached through the real member by hand; this repository's
  // own signal.h corrects the macro instead, so the POSIX spelling works and every other consumer
  // of the SDK gets the same repair.
  sa.sa_sigaction = &ps4SignalAction;
  sa.sa_flags     = SA_SIGINFO | SA_ONSTACK;
  sigemptyset(&sa.sa_mask);
  const int segvRc = sigaction(SIGSEGV,&sa,nullptr);
  sigaction(SIGBUS, &sa,nullptr);
  sigaction(SIGFPE, &sa,nullptr);
  sigaction(SIGILL, &sa,nullptr);
  sigaction(SIGABRT,&sa,nullptr);

  orbis_log("boot: crash handlers installed (terminate + SIGSEGV/SIGBUS/SIGFPE/SIGILL/SIGABRT, "
          "sigaction rc=%d, sigaltstack rc=%d %s)",segvRc,altRc,
          altRc==0 ? "- a stack overflow can now report itself"
                   : "- NO alt stack, so a stack overflow will still die silently");
  }

