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

#ifdef __cplusplus
}
#endif

#endif /* _ORBIS_LOG_H */
