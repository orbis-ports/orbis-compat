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
// `/data/OpenGothic/`. /data is the area GoldHEN gives homebrew and the probe confirms it
// writable; the subdirectory is ours, so saves do not land among the game's own files - and
// the game root may be a USB stick or /app0, neither of which is a place to put them.
#include <string>

namespace orbis {

// The anchor, with a trailing '/'. Created on first use; the mkdir's result is logged once.
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
