// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// See include/orbis_boot.h.
//
// ⚠ MOVED VERBATIM from OpenGothic ps4/og_ps4_boot.cpp on 2026-08-19, except for three renames a
// move out of an engine cannot avoid: ps4_log -> orbis_log, ps4_log_fatal -> orbis_log_fatal, and
// ps4_idle_forever -> orbis_fatal_action. The comments are the originals and say what they measured.
#include <orbis_boot.h>

#include <execinfo.h>
#include <orbis_log.h>

#include <clocale>
#include <cctype>
#include <cerrno>
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
  // ⚠ `info` IS NOT TRUSTED TO BE A POINTER, AND THAT IS NOT PARANOIA - IT WAS A SMALL INTEGER
  // FOR THE WHOLE LIFE OF THIS FILE. The SDK's SA_SIGINFO was Linux's 4 while this kernel is
  // FreeBSD's, which reads 4 as SA_RESETHAND - so the handler was installed one-shot and WITHOUT
  // siginfo, and arrived with FreeBSD's original (int sig, int code, struct sigcontext *scp).
  // `code` was 2. The null check below passed, `info->si_code` faulted inside the SIGSEGV handler
  // with `reentered` already set, and the process _Exit(2)ed in silence. The absence is the proof:
  // "crash handlers installed" appears in log after log and "fatal: signal" in none of them.
  //
  // include/signal.h now corrects the flag values, so this should be a real siginfo_t. The guard
  // stays because a handler that dies is worse than a handler that says less: anything below a
  // page cannot be a valid pointer, and on this platform a page is 16 KiB.
  const bool info_is_pointer = (reinterpret_cast<uintptr_t>(info) >= 0x4000u);
  if(info_is_pointer)
    orbis_log_fatal("fatal: signal %d - %s, si_code %d, fault address %p",
                  sig,signalName(sig),info->si_code,info->si_addr);
  else
    orbis_log_fatal("fatal: signal %d - %s, and NO siginfo: the second argument was %p, which is "
                  "too small to be a pointer. The kernel called this handler with FreeBSD's "
                  "original (sig, code, scp) signature, so SA_SIGINFO did not reach it - check the "
                  "SA_* values in orbis-compat/include/signal.h against this SDK's bits/signal.h",
                  sig,signalName(sig),(void*)info);
  // ⚠ THE THIRD ARGUMENT CARRIES THE FAULTING INSTRUCTION AND WAS BEING THROWN AWAY, WHICH COSTS
  // A DIAGNOSIS EVERY TIME THE HANDLER ACTUALLY WORKS. Once this handler catches a signal the
  // process does not die, so the kernel writes NO dump - no registers, no rip, no module list.
  // The very first crash it caught (a null dereference in swanstation) could be described as
  // "si_code 1, address 0" and located no further than that.
  //
  // ⚠ AND THE LAYOUT CANNOT BE TAKEN FROM THE SDK's HEADERS. Code compiled here has already
  // failed on `no member named 'mc_rip' in 'mcontext_t'` - the SDK ships musl's ucontext while the
  // kernel passes FreeBSD's - so the offset of rip is not something to look up, it is something to
  // measure. Hence a raw dump: the words are printed and whichever one lands inside a loaded
  // module's text range IS the instruction pointer. Replace this with a named field the moment
  // that offset is known and written down.
  // ⚠ A BACKTRACE, BECAUSE THE CONTEXT DUMP DID NOT CONTAIN THE INSTRUCTION POINTER.
  // Thirty-two words of the third argument were printed on hardware and not one of them fell
  // inside the faulting module's .text - rip sits further in than that, and its offset cannot be
  // taken from this SDK's headers because they describe musl's mcontext while the kernel passes
  // FreeBSD's. Frame pointers are a shorter road to the same answer: the return addresses ARE in
  // the module, so subtracting the base gives a function name.
  //
  // ⚠ AND THE MODULE BASE IS NOT IN THIS DUMP EITHER, BY DESIGN - the kernel writes no crash dump
  // once this handler survives the signal. libretro-common/dynamic/dylib.c prints each core's
  // retro_run address at load for exactly this purpose; subtract its address in the .elf.
  {
    void *frames[24];
    const int n = backtrace(frames, (int)(sizeof(frames) / sizeof(frames[0])));
    if(n > 0)
      for(int i = 0; i < n; i += 4)
      {
        const int r = (n - i) < 4 ? (n - i) : 4;
        orbis_log_fatal("fatal: frame[%02d..%02d] %016llx %016llx %016llx %016llx",
                      i, i + r - 1,
                      (unsigned long long)(r > 0 ? (uintptr_t)frames[i + 0] : 0),
                      (unsigned long long)(r > 1 ? (uintptr_t)frames[i + 1] : 0),
                      (unsigned long long)(r > 2 ? (uintptr_t)frames[i + 2] : 0),
                      (unsigned long long)(r > 3 ? (uintptr_t)frames[i + 3] : 0));
      }
    else
      orbis_log_fatal("fatal: backtrace returned %d frames", n);
  }

  // ⚠ THE FIELDS ARE NAMED NOW, AND THE OFFSET IS FOUND AT RUN TIME RATHER THAN COMPILED IN.
  //
  // The layout cannot come from this SDK's headers: they describe musl's mcontext while the
  // kernel passes FreeBSD's, and code compiled here has already failed on `no member named
  // 'mc_rip' in 'mcontext_t'`. It also cannot be a constant measured once, because the only
  // measurement available was a raw dump read by hand - and reading sixteen lines of hex to find
  // rip cost three separate diagnoses on 2026-08-30 alone.
  //
  // So the handler calibrates itself against something it can recognise. On FreeBSD/amd64 a user
  // thread's selectors are fixed: mc_cs is 0x43 and mc_ss is 0x3b, and in the mcontext they sit
  // exactly three words apart. Finding that pair fixes the base of every other field. Measured on
  // hardware 2026-08-30: mc_ss landed at ctx[32], which puts the mcontext at ctx[8] and mc_rip at
  // ctx[28] - the same offset that had been read out of a raw dump by eye, now derived instead.
  //
  // ⚠ AND IT IS CHECKED AGAINST A SECOND, INDEPENDENT ANCHOR BEFORE ANYTHING IS PRINTED UNDER A
  // FIELD NAME. siginfo already carries the faulting address, so mc_addr must equal si_addr. A
  // named line that is confidently wrong is worse than the hex it replaced: this port has spent
  // whole cycles on addresses that turned out to be a different frame than the one claimed. If
  // either check fails, the raw dump is printed and says why.
  if (uctx != nullptr)
  {
    // Field order of FreeBSD's amd64 mcontext_t, in 8-byte words from the start of the struct.
    enum : int {
      MC_TRAPNO = 16,   // uint32 trapno, then uint16 fs, gs - one word
      MC_ADDR   = 17,
      MC_FLAGS  = 18,   // uint32 flags, then uint16 es, ds
      MC_ERR    = 19,
      MC_RIP    = 20,
      MC_CS     = 21,
      MC_RFLAGS = 22,
      MC_RSP    = 23,
      MC_SS     = 24
    };
    const uint64_t *w = reinterpret_cast<const uint64_t*>(uctx);
    const int words   = 96;               // far past mc_ss for any plausible prefix
    int mc            = -1;

    for(int i = MC_CS; i + (MC_SS - MC_CS) < words; i++)
      if(w[i] == 0x43u && w[i + (MC_SS - MC_CS)] == 0x3bu) { mc = i - MC_CS; break; }

    const bool addr_agrees = (mc >= 0) && info_is_pointer
                          && (w[mc + MC_ADDR] == (uint64_t)(uintptr_t)info->si_addr);

    if(mc >= 0 && (addr_agrees || !info_is_pointer))
    {
      orbis_log_fatal("fatal: rip %016llx  rsp %016llx  addr %016llx",
                    (unsigned long long)w[mc + MC_RIP],
                    (unsigned long long)w[mc + MC_RSP],
                    (unsigned long long)w[mc + MC_ADDR]);
      orbis_log_fatal("fatal: trapno %u  err %llu  rflags %016llx  (mcontext at ctx[%d])",
                    (unsigned)(w[mc + MC_TRAPNO] & 0xffffffffu),
                    (unsigned long long)w[mc + MC_ERR],
                    (unsigned long long)w[mc + MC_RFLAGS], mc);
      // ⚠ SUBTRACT THE MODULE BASE FROM rip TO GET A FUNCTION. The kernel writes no dump once
      // this handler survives the signal, so nothing else prints where the module landed -
      // libretro-common/dynamic/dylib.c logs each core's retro_run address at load for exactly
      // this, and its address in the .elf is the other half of the subtraction.
    }
    else
    {
      if(mc < 0)
        orbis_log_fatal("fatal: no mc_cs/mc_ss pair (0x43, 0x3b) in the first %d words of the "
                      "context - the layout is not FreeBSD's amd64 mcontext, raw words follow",
                      words);
      else
        orbis_log_fatal("fatal: found a context at ctx[%d] but mc_addr %016llx does not match "
                      "si_addr %p - not trusting the field names, raw words follow",
                      mc, (unsigned long long)w[mc + MC_ADDR], info_is_pointer ? info->si_addr : nullptr);

      for(int base = 0; base < 64; base += 4)
        orbis_log_fatal("fatal: ctx[%02d..%02d] %016llx %016llx %016llx %016llx",
                      base, base + 3,
                      (unsigned long long)w[base + 0], (unsigned long long)w[base + 1],
                      (unsigned long long)w[base + 2], (unsigned long long)w[base + 3]);
    }
  }

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

// FreeBSD's errno numbers, not Linux's - oracles/freebsd9/sys_sys_errno.h. The two tables agree
// up to 34 and diverge from 35 (EAGAIN there, EDEADLK on Linux), so a number under 35 read through
// the wrong table happens to be right; anything above it is not. Only the ones sigaltstack can
// return are named, plus ENOSYS, because "this symbol exists and refuses" and "this kernel does
// not implement the call" lead to opposite conclusions.
static const char* bsdErrnoName(int e) {
  switch(e) {
    case 0:  return "no error";
    case 1:  return "EPERM - already running on the alternate stack";
    case 12: return "ENOMEM - ss_size below MINSIGSTKSZ";
    case 14: return "EFAULT - the kernel could not read the stack_t we passed";
    case 22: return "EINVAL - a bit set in ss_flags that is not SS_DISABLE";
    case 78: return "ENOSYS - the kernel does not implement this call at all";
    }
  return "not in this file's table - decode against FreeBSD's errno.h, NOT Linux's";
  }

void orbis::installCrashHandlers() {
  std::set_terminate(&ps4TerminateHandler);

  // AN ALTERNATE STACK, and without it a stack overflow is UNREPORTABLE: the handler needs
  // stack to run and the overflow is precisely the absence of it, so the fault recurses into
  // the kernel and the process dies with nothing said. That was the leading hypothesis for
  // this crash until klog named it otherwise, and it costs 64 KiB to remove from the list of
  // things a silence can mean. Leaked on purpose - it has to outlive everything.
  //
  // ⚠ WHY THIS CALL RETURNED -1 FOR MONTHS, AND WHY SA_ONSTACK WAS NOT THE ANSWER. SA_ONSTACK
  // WAS wrong - Linux's 0x08000000 where this kernel wants 0x0001 - and correcting it changed
  // nothing here, because the SDK also declares stack_t in Linux's field order while the kernel
  // reads FreeBSD's. include/signal.h now carries the swap and the whole derivation; what is left
  // in this function is the instrumentation that proves it on the console rather than on paper.
  //
  // ⚠ THE BUFFER IS HEAP, NOT A STATIC ARRAY, AND THAT IS NOT INCIDENTAL. A large static array
  // on this console is a page-permission question of its own (ps4/orbis_exec_mem.c in the
  // RetroArch tree is thirty lines on the subject) and there is no reason to put the one
  // allocation that has to work during a crash into the one region that needs promoting. `alt`
  // itself is 24 bytes of .bss and has demonstrably always been writable: the boot line printed
  // its return code rather than faulting on the store.
  static stack_t alt = {};
  alt.ss_size  = 64*1024;
  alt.ss_sp    = new char[alt.ss_size];
  alt.ss_flags = 0;
  errno = 0;
  const int altRc  = sigaltstack(&alt,nullptr);
  const int altErr = errno;

  // ⚠ THE LEGACY PROBE, so that ONE reboot decides between two explanations instead of one.
  // If the corrected layout is accepted, this never runs. If it is refused, the same 64 KiB is
  // offered again in the SDK's byte order - exactly what the old code sent - and the two errnos
  // side by side say whether the layout was the problem or whether this kernel refuses an
  // alternate stack however it is asked. Only reached when the first attempt already failed, so
  // it cannot un-install a working stack.
  int legacyRc = 0, legacyErr = 0;
  if(altRc != 0) {
    static stack_t legacy = {};
    legacy.ss_size  = 64*1024;
    legacy.ss_sp    = alt.ss_sp;
    legacy.ss_flags = 0;
    errno = 0;
    legacyRc  = __orbis_sigaltstack_kernel(
                  reinterpret_cast<const struct __orbis_stack_bsd*>(&legacy),nullptr);
    legacyErr = errno;
    }

  // ⚠ THE READBACK IS THE EVIDENCE, AND IT ANSWERS THE LAYOUT QUESTION WHETHER OR NOT THE INSTALL
  // WORKED. sigaltstack(NULL,&oss) makes the kernel WRITE twenty-four bytes, and they are printed
  // raw - decoded by nobody - alongside the FreeBSD reading of them. Both outcomes are diagnostic:
  //
  //   installed, FreeBSD order   w0 = the buffer   w1 = 0000000000010000   w2 = 0 or 1 (SS_ONSTACK)
  //   refused,   FreeBSD order   w0 = 0            w1 = 0                 w2 = 4 (SS_DISABLE)
  //   refused,   Linux order     w0 = 0            w1 = 2 (Linux's        w2 = 0
  //                                                     SS_DISABLE)
  //
  // so the position of the one non-zero word names the layout even when nothing is installed.
  // Called through the raw symbol rather than the shim precisely so that the shim's own
  // assumption is not what formats the answer.
  union { struct __orbis_stack_bsd s; unsigned long long w[3]; } back;
  back.w[0] = back.w[1] = back.w[2] = 0;
  errno = 0;
  const int backRc  = __orbis_sigaltstack_kernel(nullptr,&back.s);
  const int backErr = errno;

  const bool bsdOrder = (back.w[2] <= 7ull) && (back.w[1] == 0ull || back.w[1] == 64ull*1024ull);
  const bool linOrder = (back.w[1] <= 7ull) && (back.w[2] == 0ull || back.w[2] == 64ull*1024ull);
  const char* order   = (back.w[0]==0ull && back.w[1]==0ull && back.w[2]==0ull)
                          ? "all three words zero - inconclusive, and also not what FreeBSD does "
                            "for a thread with no alternate stack (it sets SS_DISABLE)"
                      : (bsdOrder && !linOrder) ? "FreeBSD field order confirmed (sp, size, flags)"
                      : (linOrder && !bsdOrder) ? "⚠ LINUX field order - the shim in "
                            "orbis-compat/include/signal.h is backwards and must be reverted"
                      : "neither order fits - read the raw words by hand";

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
          "sigaction rc=%d, sigaltstack rc=%d errno=%d %s %s)",segvRc,altRc,altErr,
          bsdErrnoName(altErr),
          altRc==0 ? "- a stack overflow on THIS thread can now report itself"
                   : "- NO alt stack, so a stack overflow will still die silently");
  orbis_log("boot: sigaltstack readback rc=%d errno=%d - raw %016llx %016llx %016llx - "
          "as FreeBSD stack_t: ss_sp=%016llx ss_size=%llu ss_flags=%d - %s",
          backRc,backErr,back.w[0],back.w[1],back.w[2],
          (unsigned long long)(uintptr_t)back.s.ss_sp,
          (unsigned long long)back.s.ss_size,back.s.ss_flags,order);
  if(altRc != 0)
    orbis_log("boot: sigaltstack legacy-layout probe (the SDK's own field order, i.e. what this "
            "port sent before today) rc=%d errno=%d %s - if BOTH orders fail with the same errno "
            "the layout was never the problem and this kernel is refusing the call itself",
            legacyRc,legacyErr,bsdErrnoName(legacyErr));

  // ⚠ AND IT COVERS THIS THREAD ONLY. FreeBSD keeps the alternate stack in td_sigstk, per THREAD,
  // while sigaction's disposition is per PROCESS - so a core running on a worker thread still
  // overflows onto a stack that is already exhausted, whatever this call returned. Not fixed here
  // because nothing has yet observed the working case; the honest next step is one more 64 KiB and
  // one more sigaltstack from inside orbis_thread's start routine, once the console has confirmed
  // that the call succeeds at all.
  }
