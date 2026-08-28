// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
#pragma once

// Anchoring relative paths, because this process has no working directory at all.
//
// ------------------------------------------------------------------ what the console said
//
// probeSavePaths() on hardware, 2026-08-07 (build-ps4-logs/ps4-udp-20260807-130654.log):
//
//   getcwd()                          errno 78  = ENOSYS
//   open("save_slot_probe.sav", ...)  errno 22  = EINVAL
//   mkdir("/data/OpenGothic")         created
//   /data/OpenGothic/probe.sav        write 19 B, stat 19, read-back IDENTICAL
//   /data/probe.sav                   write 19 B, stat 19, read-back IDENTICAL
//   /data/gothic2/probe.sav           write 19 B, stat 19, read-back IDENTICAL
//   /app0/probe.sav                   errno 30  = EROFS      <- the negative control, and it
//                                                               fired with the right errno
//
// THE FIRST TWO LINES ARE THE WHOLE DESIGN. `getcwd` does not fail unhelpfully - it is
// ENOSYS, so there is no working-directory concept in this process to anchor against, and a
// relative `open` is not unanchored but INVALID: EINVAL, not ENOENT and not EACCES. This is
// stronger than the fact this project already had ("the sandbox refuses chdir",
// ps4/gapi-suite, `chdir("/app0")=-1`): there is nothing for chdir to have set.
//
// ------------------------------------------------------------------ why rewriting is safe
//
// A relative path returns EINVAL today, without exception. So redirecting every relative
// path can only turn a call that fails into one that may succeed - it cannot break anything
// that currently works, because nothing relative currently works. That argument is the
// reason this is a blanket rewrite rather than a list of filenames: a list would have to be
// kept in step with OpenGothic, and this needs no maintenance to stay correct.
//
// What it catches, beyond the six `save_slot_N.sav` sites: `FileUtil::exists`, which is a
// `stat` of the same relative name (game/utils/fileutil.cpp) and would otherwise report
// every written save as missing - a save nobody can load is worse than a save that fails.
// And CrashLog's `crash.log`, which OpenGothic writes next to the executable and which has
// therefore never been written on this platform.
//
// ------------------------------------------------------------------ where, and why there
//
// /data is the area GoldHEN gives homebrew and the probe confirms it writable; a subdirectory
// under it, so files do not land among the game's own - and the game root may be a USB stick or
// /app0, neither of which is a place to put them.
//
// ------------------------------------------------------------------ ⚠ AND NOT A FIXED NAME
//
// It used to be the literal `/data/OpenGothic/`, compiled into an overlay that three other titles
// now link. That is not a default, it is one title's name inside a shared library: RetroArch has
// been creating `/data/OpenGothic/` on every boot and opening files in it, because a relative path
// it opens - `Main Menu.png` among them - anchors there. Harmless-looking until the day two titles
// pick the same relative filename, at which point one silently reads the other's file.
//
// So the root is decided in this order, and the choice is logged once:
//
//   1. orbis_set_anchor_root(), if the application called it before its first relative path.
//      An application knows where its own data lives; nothing here can.
//   2. `/data/<TITLEID>/` from sceKernelGetAppInfo. Needs no cooperation and cannot collide,
//      because the console decides collisions by title id and so does this.
//   3. `/data/orbis-compat/`. Reached only when 1 and 2 both failed. Deliberately not a title's
//      name: a shared directory that says who made it is better than one that names a stranger.
//
// ⚠ 2 IS UNPROVEN ON THIS FIRMWARE. sceKernelGetAppInfo is declared by the SDK and this overlay
// has been caught four times by a call that exists, links, and is refused at run time. The log
// line reports what it returned whichever route wins, so one boot settles it.
//
// ⚠ THE C++ HALF IS GUARDED because the C-linkage setter below has to be reachable from C. A
// frontend written in C is exactly the kind of consumer that needs to name its own anchor, and it
// cannot include <string>.
#ifdef __cplusplus
#include <string>

namespace orbis {

// The anchor, with a trailing '/'. Decided and created on first use; the choice, the mkdir's
// result and what sceKernelGetAppInfo said are logged once.
const std::string& anchorRoot();

// Absolute paths are returned unchanged, relative ones prefixed with anchorRoot(). The
// return value points either at `path` itself or at `buf`, so a caller needs no allocation
// on the hot path - which matters because this sits inside `stat`, and `stat` is how
// OpenGothic resolves every case-insensitive path segment under the game root.
const char* anchorPath(const char* path, char* buf, size_t bufSize);

// One line, the first time anything is anchored, naming the root and the first path that
// needed it. Not per call: the menu stats four save slots every frame it is open.
void anchorSayOnce(const char* original, const char* rewritten);

}
#endif /* __cplusplus */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Sets the directory relative paths are anchored under. A trailing '/' is added if missing, and
/// the directory is created on first use like any other anchor.
///
/// ⚠ CALL IT BEFORE THE FIRST RELATIVE PATH, which in practice means before anything opens a file.
/// The anchor is decided once, on first use, because it must be cheap inside stat(); a call that
/// arrives after that decision cannot take effect and says so in the log rather than pretending.
void orbis_set_anchor_root(const char *path);

#ifdef __cplusplus
}
#endif
