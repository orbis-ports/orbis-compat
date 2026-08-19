// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* Two things every consumer of this SDK needs, moved out of a game that happened to write them first.
 *
 * They arrived here from OpenGothic's ps4/og_ps4_boot.cpp, which is 960 lines of which most is about
 * Gothic - VDF archives, save paths, a data inventory. These two are not:
 *
 *   probeCtype()           asks whether this musl folds case. Nothing to do with any one title.
 *   installCrashHandlers() a title that dies quietly is a bug report with no content, and THIS
 *                          overlay is what supplies backtrace(3) - so the natural home for the
 *                          handler that will one day print a stack is here, beside it.
 *
 * ⚠ THE OVERLAY REPORTS; IT DOES NOT DECIDE WHAT HAPPENS NEXT. After a fatal event it calls
 * orbis_fatal_action(), which the application registers - idling for a debugger and exiting are both
 * legitimate and only the application knows which it wants. On this console the choice is visible to
 * the user: a process that exits gets a system error dialog, one that idles keeps the screen.
 */
#ifndef _ORBIS_BOOT_H
#define _ORBIS_BOOT_H

namespace orbis {

/// Reports through orbis_log whether std::tolower/toupper fold ASCII, and says what breaks if not.
void probeCtype();

/// Installs a std::terminate handler and SA_SIGINFO handlers for SIGSEGV/BUS/FPE/ILL/ABRT, on an
/// alternate stack so that a stack overflow can still report itself. Reports what it managed.
void installCrashHandlers();

}

#endif /* _ORBIS_BOOT_H */
