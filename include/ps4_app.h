// The console's log channel and its termination policy. Opt-in: optional/, not the archive.
//
// ⚠ THIS IS POLICY, WHICH IS WHY IT IS NOT IN liborbis-compat.a. It decides that a dying process
// idles rather than returns, and it obliges the link to carry -lSceNet. The overlay's corrections
// declare a hook (orbis_log.h) and this registers into it; nothing in src/ knows this file exists.
//
// Logging is a tee over two independent channels:
//   * orbis_netlog() - UDP datagram to the dev host, fire-and-forget, primary
//     (orbis_netlog.h, received by scripts/ps4/logs.sh)
//   * klog     - sceKernelDebugOutText, what GoldHEN's klog server relays; the
//     only channel that says anything when netlog never came up
//
// ~/src/ps4doom/platform/ps4_netlog.c:76-82 measured that a console write
// blocks the calling thread for seconds when nothing drains the debug channel,
// so the klog half is only used for the bounded set of lines outside a render
// loop. ps4_log_frame() is the UDP-only variant for per-frame lines.
//
// Termination: returning from main() on a retail console tears the process down
// outside the system's expected path and pops the error dialog, which reads as a
// crash (CE-34878-0). ~/src/ps4doom/platform/doomgeneric_ps4.c:350 never leaves its
// tick loop - so a demo ends with ps4_idle_forever() instead of a return.
//
// ---------------------------------------------------------------- the run config
//
// An automated run wants the opposite: return, so the process ends and the harness
// gets an exit code. That used to be -DPS4_DEMO_AUTOEXIT=ON, i.e. a SECOND BINARY.
// That was retired: the emulator leg and the console leg now run bytes built once,
// and what differs between them is what the HOST puts next to the executable.
//
//   /app0/ps4-run.cfg   optional, plain text, one `key=value` per line, `#` comments
//     autoexit=1        ps4_idle_forever() returns instead of idling
//     frame-klog=1      ps4_log_frame() also writes klog, not netlog alone
//
// With no file - which is every .pkg launch, because nothing puts one in the package
// (scripts/ps4/make-pkg.sh ships only the files it is told to) - both are 0 and the
// behaviour is the console behaviour: idle forever, per-frame lines over UDP only.
// scripts/ps4/run-tests.sh writes the file into the build directory before it starts
// unemups4, which union-mounts the executable's directory onto /app0.
//
// This is deliberately not runtime detection. Nothing here asks what it is running
// on; it reads a file the launcher chose to leave, and a console launch that left
// the same file would behave the same way. A binary that sniffed for an emulator
// would make every conformance result a statement about the sniff.
#pragma once

#include <orbis_netlog.h>

// Announced in the first log line so the installed package is never in doubt.
// The value is fixed at configure time by optional/CMakeLists.txt; __DATE__ is
// only the fallback, and it is frozen to 1980 inside the reproducible-build
// devShell (SOURCE_DATE_EPOCH), which is exactly why it cannot be the source.
#ifndef PS4_APP_BUILD_STAMP
#define PS4_APP_BUILD_STAMP __DATE__ " " __TIME__
#endif
#define PS4_APP_STAMP PS4_APP_BUILD_STAMP

#ifdef __cplusplus
extern "C" {
#endif

// klog + netlog "alive" stamp, and the one read of /app0/ps4-run.cfg. First
// statement of main(), before anything else is constructed - ps4_log_frame() and
// ps4_idle_forever() both answer out of what this read.
void ps4_app_init(const char* app, const char* stamp);

// The two run-config answers, for a caller that wants to say what it is doing. Both
// are 0 until ps4_app_init() has run, which is the console default either way.
int ps4_run_autoexit(void);
int ps4_run_frame_klog(void);

// Change the "[<app>]" tag without announcing anything. ps4/runner switches it to the
// scene's own tag around each scene, so a scene's lines in the one log it shares with
// fourteen others are the lines that scene's standalone demo would have written.
void ps4_app_set_tag(const char* app);

// Tagged "[<app>] ..." line on both channels. Bounded call sites only.
void ps4_log(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

// Same, UDP only - safe to call from a render loop. Also klog when the host asked
// for it (`frame-klog=1`), which is what an emulator run needs: nothing there drains
// a UDP socket, so klog is the only channel the harness reads, and an emulated klog
// has none of the back-pressure that bans it from a console render loop.
void ps4_log_frame(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

// Pre-formatted line, UDP only, no tag added - the shape a backend's own log sink wants. See the
// implementation for why nothing on this branch calls it.
void ps4_log_net_line(const char* line);

// THE FATAL PATH, and it exists because klog came off the normal path. A line that
// describes a process about to die cannot go out over UDP alone: `sceNetSendto` hands the
// datagram to a network stack that needs the process to survive long enough for it to leave,
// and the console's own klog said this exact crash was `App Crash reason=0x4` while our
// handler's line never arrived at all.
//
// So this writes `sceKernelDebugOutText` FIRST and unconditionally - a synchronous kernel
// call that has already returned by the time it returns - and then the netlog, which is the
// nicer channel to read and the one that may not make it. Use it in crash handlers and
// nowhere else; everything else wants the cheap path (klog is 8-15 ms a line on this
// console, measured).
void ps4_log_fatal(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

// Hold the process alive with a slow heartbeat instead of returning from main().
// Returns immediately when the host asked for it (`autoexit=1`), and only then.
void ps4_idle_forever(const char* what);

#ifdef __cplusplus
}
#endif
