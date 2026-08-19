// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
//
// A place for this overlay's corrections to say something, without knowing who is listening.
//
// The interposers here run before, beneath and after whatever the application is: a stat() that
// translates the kernel's layout, an operator new that reports which pool ran out. When they have
// something to say they must not reach into an engine to say it - that is what tied them to
// Tempest's ps4_log and made them unmovable.
//
// Nothing is registered by default and the calls are cheap no-ops until something registers. An
// application that wants the output calls orbis_set_log once, early; one that does not, does not.
#ifndef _ORBIS_LOG_H
#define _ORBIS_LOG_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/// The shape of a logger: printf, as far as these call sites are concerned.
typedef void (*orbis_log_fn)(const char *fmt, va_list ap);

/// Installs the logger, or removes it with nullptr. Not thread-safe against concurrent logging,
/// which is deliberate: it is meant to be called once before the threads exist.
void orbis_set_log(orbis_log_fn fn);

/// Writes a line through whatever was registered. A no-op if nothing was.
void orbis_log(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/// True when a logger is installed, for call sites that would otherwise build a message for nobody.
int orbis_log_enabled(void);

/// ⚠ A SECOND CHANNEL, FOR LINES THAT DESCRIBE A PROCESS ABOUT TO STOP EXISTING. The normal logger
/// may be a datagram, and a datagram from a process the kernel is about to kill can fail to leave
/// the machine at all. A crash handler needs a channel that has already written before it returns.
/// Registered separately because only the application knows which of its channels that is.
void orbis_set_log_fatal(orbis_log_fn fn);

/// Writes through the fatal logger, or through the ordinary one if none was registered, or nowhere.
void orbis_log_fatal(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/// What to do once a fatal event has been reported. The overlay reports; it does not decide whether
/// to idle for a debugger, exit, or return - that is the application's policy, and on this console
/// the choice matters (a title that exits gives the user a console error dialog and nothing else).
/// `what` names the event: "terminate" or "signal". Returning from it means the overlay exits.
typedef void (*orbis_fatal_action_fn)(const char *what);
void orbis_set_fatal_action(orbis_fatal_action_fn fn);
void orbis_fatal_action(const char *what);

#ifdef __cplusplus
}
#endif

#endif /* _ORBIS_LOG_H */
