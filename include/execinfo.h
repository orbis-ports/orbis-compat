/* backtrace(3), which this SDK does not ship although the platform can do it.
 *
 * musl has no execinfo.h by design, so code written against glibc or FreeBSD fails to compile
 * here - and the crash handlers that would have used it are exactly what this port keeps needing.
 * The unwinder itself is present: libc++.a defines _Unwind_Backtrace, and the build already
 * compiles with -funwind-tables.
 *
 * ⚠ Addresses only. There is no symbolisation on this console; resolve them against the ELF.
 */
#ifndef _EXECINFO_H
#define _EXECINFO_H

#ifdef __cplusplus
extern "C" {
#endif

/// Fills \p buffer with up to \p size return addresses, innermost first, and returns how many.
int backtrace(void **buffer, int size);

/// Writes one address per line to \p fd. Never allocates, so it is usable after a fault.
void backtrace_symbols_fd(void *const *buffer, int size, int fd);

/// malloc()s an array of strings, as glibc does. Prefer backtrace_symbols_fd in a crash handler.
char **backtrace_symbols(void *const *buffer, int size);

/// backtrace() with control over how many of the innermost frames to drop - backtrace() skips its
/// own, a crash handler usually wants to skip the signal trampoline too. Not part of the glibc API;
/// exported because the archive exports it either way and a hidden export is worse than a named one.
int orbis_unwind_collect(void **buffer, int size, int skip);

#ifdef __cplusplus
}
#endif

#endif /* _EXECINFO_H */
