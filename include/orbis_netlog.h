// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
//
// UDP log transport - the channel every part of this port is actually read through.
//
// ⚠ THE IMPLEMENTATION IS IN optional/, NOT IN liborbis-compat.a, and for one concrete reason: it
// calls sceNet, so linking it obliges the consumer to add -lSceNet. A consumer that does not want a
// network logger should not inherit a library dependency for it. Add optional/orbis_netlog.cpp to
// your own target when you want it.
//
// ⚠ AND IT IS FIRE-AND-FORGET BY DESIGN. sceNetSendto cannot stall the title; a datagram that does
// not arrive is lost and nothing waits for it. That is the right trade for a log, and the wrong one
// for a line describing a process about to die - which is why orbis_log.h has a SECOND, fatal
// channel that an application points somewhere synchronous.
//
// Pairs with orbis_log.h: register a va_list wrapper of orbis_netlog through orbis_set_log() and
// every correction in this repository reports over the wire.
//
// The host receivers live beside it in scripts/ps4/: log-receiver.py (18194), logs.sh, and
// peerfilter.py. ⚠ Start the receiver BEFORE the title - the interesting lines are printed once, at
// boot, and a datagram sent to nobody is gone.
//
// Adapted from ~/src/ps4doom/platform/ps4_netlog.c (MIT, same maintainer).
//
// Deliberately not writing to stdout: on a real console stdout goes to the
// kernel debug buffer, and a write blocks the calling thread when GoldHEN's
// klog server is not being drained — seconds of stall per line inside a render
// loop. sceNetSendto is fire-and-forget and cannot stall the title.
//
// Target host/port are compile-time overridable and default to the cable link
// in backlog/docs/dev-setup.md:
//   -DNETLOG_HOST=\"192.168.100.1\" -DNETLOG_PORT=18194
//
// Link with -lSceNet (the ps4-netlog CMake target does this for you).
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Open the socket. Safe to call once, early in main(). On failure orbis_netlog()
// becomes a no-op rather than an error path the caller has to handle.
void orbis_netlog_init(void);

// printf-style; one datagram per call, truncated to 512 bytes.
void orbis_netlog(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

// Non-zero once the socket is open. It exists because klog is now a FALLBACK rather than
// a second channel: a caller has to be able to ask whether anything else is
// carrying the line before deciding to pay 8-15 ms for a klog write.
int  orbis_netlog_ready(void);

void orbis_netlog_close(void);

#ifdef __cplusplus
}
#endif
