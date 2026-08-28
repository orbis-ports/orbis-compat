// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
#include "orbis_paths.h"

#include <orbis_log.h>

#include <orbis/libkernel.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstdarg>
#include <cstring>
#include <cstdint>


namespace {

bool s_rootDecided = false;
std::string s_rootOverride;

// ⚠ VALIDATED, NOT TRUSTED. This becomes a directory name under /data, so a field of garbage
// would create a directory of garbage - or, with a '/' in it, somewhere else entirely. A real
// title id is nine characters of A-Z0-9; anything else is treated as "the call did not answer".
bool titleIdLooksReal(const char* id) {
  size_t n = 0;
  for(; id[n]!='\0'; ++n) {
    if(n>=9)
      return false;
    const char c = id[n];
    if(!((c>='A'&&c<='Z') || (c>='a'&&c<='z') || (c>='0'&&c<='9')))
      return false;
    }
  return n>0;
  }

// The console's own answer to "which title am I".
//
// ⚠ TitleId IS A FIXED 10-BYTE FIELD AND IS NOT PROMISED TO BE TERMINATED, so it is copied bounded
// and terminated here rather than handed to a string function as it stands.
std::string titleIdRoot() {
  OrbisAppInfo  info = {};
  const int32_t rc   = sceKernelGetAppInfo(getpid(), &info);
  char          id[10] = {};
  std::memcpy(id, info.TitleId, sizeof(info.TitleId) - 1);

  if(rc!=0) {
    orbis_log("paths: sceKernelGetAppInfo returned 0x%08x - no title id, so the anchor cannot be "
            "named after this title",unsigned(rc));
    return std::string();
    }
  if(!titleIdLooksReal(id)) {
    orbis_log("paths: sceKernelGetAppInfo succeeded but the title id is not nine characters of "
            "A-Z0-9 - ignored rather than made into a directory name");
    return std::string();
    }
  orbis_log("paths: title id is '%s'",id);
  return std::string("/data/") + id + "/";
  }

}

extern "C" void orbis_set_anchor_root(const char* path) {
  if(path==nullptr || path[0]!='/') {
    orbis_log("paths: orbis_set_anchor_root('%s') ignored - the anchor must be an absolute path",
            path==nullptr ? "(null)" : path);
    return;
    }
  if(s_rootDecided) {
    // Not applied, and not silent. Whatever has already been anchored went somewhere else, so a
    // quiet late override would leave a process using two roots and no record of it.
    orbis_log("paths: orbis_set_anchor_root('%s') arrived AFTER the anchor was already decided as "
            "'%s' and is ignored. Call it before anything opens a file",path,
            orbis::anchorRoot().c_str());
    return;
    }
  s_rootOverride = path;
  if(s_rootOverride.back()!='/')
    s_rootOverride += '/';
  }

namespace orbis {

const std::string& anchorRoot() {
  static std::string root = [](){
    const char* how = "set by the application";
    std::string r   = s_rootOverride;
    if(r.empty()) {
      r   = titleIdRoot();
      how = "this title's id";
      }
    if(r.empty()) {
      // ⚠ NOT ANOTHER TITLE'S NAME. This used to be '/data/OpenGothic/' unconditionally, which
      // meant every other consumer of this overlay wrote into a directory belonging to a game it
      // has nothing to do with.
      r   = "/data/orbis-compat/";
      how = "the fallback - the application named no anchor and the title id was not available";
      }
    s_rootDecided = true;

    std::string dir = r.substr(0,r.size()-1);
    // 0777 rather than something tighter: this is a homebrew title's own directory on a dev
    // console and a umask nobody here controls decides the rest anyway.
    if(mkdir(dir.c_str(),0777)==0)
      orbis_log("paths: anchor '%s' created (%s)",r.c_str(),how);
    else if(errno==EEXIST)
      orbis_log("paths: anchor '%s' (%s)",r.c_str(),how);
    else
      // Not fatal and not silent. If this failed, every anchored path below will fail too and
      // the errno here is the one that explains all of them.
      orbis_log("paths: anchor '%s' (%s) could not be created, errno %d - every relative path will "
              "fail, and this is why",r.c_str(),how,errno);
    return r;
    }();
  return root;
  }

void anchorSayOnce(const char* original, const char* rewritten) {
  static bool said = false;
  if(said)
    return;
  said = true;
  orbis_log("paths: relative paths are anchored - '%s' -> '%s'. This process has NO working "
          "directory (getcwd = ENOSYS) and a relative open returns EINVAL, so without this "
          "nothing relative can be opened at all",original,rewritten);
  }

const char* anchorPath(const char* path, char* buf, size_t bufSize) {
  if(path==nullptr || path[0]=='/' || path[0]=='\0')
    return path;

  const std::string& root = anchorRoot();
  const size_t       n    = std::strlen(path);
  if(root.size()+n+1 > bufSize) {
    // Longer than the caller's buffer. Returned unchanged rather than truncated: an EINVAL
    // from an unanchored path is a failure that names itself, and a rewrite to a DIFFERENT
    // file is the kind of wrong answer this project refuses to produce quietly.
    orbis_log("paths: '%s' is too long to anchor into %zu B - passed through unchanged, so it "
            "will fail with EINVAL",path,bufSize);
    return path;
    }
  std::memcpy(buf,root.data(),root.size());
  std::memcpy(buf+root.size(),path,n+1);
  anchorSayOnce(path,buf);
  return buf;
  }

}

// ---------------------------------------------------------------- the open interposer
//
// The mechanism is ps4/gapi-suite's, proven on this console, and its reasoning is repeated
// here in short because this is a second copy of it rather than a shared one: OpenOrbis'
// musl is statically linked and reaches the kernel through exactly one path for a path-based
// open -
//
//   Tempest RFile/WFile -> fopen (libc.a fopen.lo) -> open (libc.a open.lo) -> _open
//
// - and a strong definition in a translation unit the linker takes FIRST means open.lo is
// never pulled in, because nothing is left undefined for it to resolve. So this IS the
// title's open, and it delegates to `_open`, the same libkernel import open.lo itself calls.
// Delegating rather than reimplementing against sceKernelOpen is what keeps the errno
// contract exact: sceKernelOpen reports SCE codes and every musl caller reads errno.
extern "C" int _open(const char* path, int flags, int mode);

extern "C" int open(const char* path, int flags, ...) {
  // musl reads the mode argument only when the flags say one was pushed. Mirrored rather than
  // always-read: va_arg on an argument the caller did not pass is undefined.
  int  mode      = 0;
  bool wantsMode = (flags & O_CREAT)!=0;
#ifdef O_TMPFILE
  wantsMode = wantsMode || (flags & O_TMPFILE)==O_TMPFILE;
#endif
  if(wantsMode) {
    va_list ap;
    va_start(ap,flags);
    mode = va_arg(ap,int);
    va_end(ap);
    }

  char      buf[512] = {};
  const int fd = _open(orbis::anchorPath(path,buf,sizeof(buf)),flags,mode);

#ifdef O_CLOEXEC
  // open.lo does this too, and it is cheap insurance against being a subtly different open()
  // than the one this binary would otherwise have had.
  if(fd>=0 && (flags & O_CLOEXEC)!=0)
    ::fcntl(fd,F_SETFD,FD_CLOEXEC);
#endif
  return fd;
  }

// Defined for exactly one reason: `open64` is a weak alias inside open.lo. Leaving it
// undefined would make the linker pull that member in to satisfy it, and open.lo's strong
// `open` would then collide with the one above.
extern "C" int open64(const char* path, int flags, ...) __attribute__((alias("open")));

// ---------------------------------------------------------------- unlink and rename
//
// FOUND BY A FILE THAT SHOULD NOT HAVE EXISTED. probeSavePaths() creates a probe file, checks it
// and unlinks it - and `/data/OpenGothic/save_slot_probe.sav` was still there when the console's
// filesystem was listed over FTP. The create had been anchored and the unlink had not, so it was
// asked to delete a relative path and got EINVAL, silently, exactly as every relative call does
// on this platform.
//
// That is the whole argument for interposing these two as well: an anchor that covers only the
// calls somebody remembered leaves the rest failing invisibly, and here the evidence was a
// leftover file rather than a log line. A title that overwrites a save slot, deletes one, or
// writes to a temporary and renames it would each hit this.
//
// These delegate to sceKernelUnlink / sceKernelRename rather than to a `_unlink` import, because
// musl reaches those through a raw syscall and there is no lower symbol to hand off to. The
// consequence is stated rather than hidden: the SCE calls report an SCE error code instead of
// setting errno, so errno is set here from a nonzero return - imprecisely, but a caller that
// checks only success/failure (which both of musl's do) cannot tell, and the alternative is a
// call that keeps failing for no reason anyone can see.
extern "C" int32_t sceKernelUnlink(const char*);
extern "C" int32_t sceKernelRename(const char* from, const char* to);

extern "C" int unlink(const char* path) {
  char        buf[512] = {};
  const int32_t rc = sceKernelUnlink(orbis::anchorPath(path,buf,sizeof(buf)));
  if(rc!=0) {
    errno = EIO;
    return -1;
    }
  return 0;
  }

extern "C" int rename(const char* from, const char* to) {
  char        fbuf[512] = {};
  char        tbuf[512] = {};
  // Both ends anchored, and both from the same root: a rename with one relative and one absolute
  // end would cross filesystems on a platform where that is not a rename at all.
  const char* f = orbis::anchorPath(from,fbuf,sizeof(fbuf));
  const char* t = orbis::anchorPath(to,tbuf,sizeof(tbuf));
  const int32_t rc = sceKernelRename(f,t);
  if(rc!=0) {
    errno = EIO;
    return -1;
    }
  return 0;
  }
