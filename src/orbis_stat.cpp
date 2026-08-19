// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
#include "orbis_stat.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <cstddef>

// The two entry points that libkernel.so exports AND unemups4 binds. Declared here with
// a void* buffer rather than through <orbis/libkernel.h>, whose OrbisKernelStat is a
// typedef of the very `struct stat` this file exists to work around.
extern "C" int sceKernelStat(const char* path, void* st);
extern "C" int _fstat(int fd, void* st);

// DECLARED, NOT INCLUDED. og_ps4_paths.h needs <string>, and libc++'s <string> on this SDK
// reaches <orbis/libkernel.h>, whose sceKernelStat is declared against the OrbisKernelStat
// typedef of the very `struct stat` this file exists to work around - so including it here
// makes the two declarations conflict. One prototype costs nothing and keeps that out.
namespace orbis {
const char* anchorPath(const char* path, char* buf, size_t bufSize);
}

namespace {

// Kernel-side field offsets. See og_ps4_stat.h for how they were established and which
// of them the console has already confirmed.
constexpr size_t kDev     = 0x00;  // uint32
constexpr size_t kIno     = 0x04;  // uint32
constexpr size_t kMode    = 0x08;  // uint16
constexpr size_t kNlink   = 0x0a;  // uint16
constexpr size_t kUid     = 0x0c;  // uint32
constexpr size_t kGid     = 0x10;  // uint32
constexpr size_t kRdev    = 0x14;  // uint32
constexpr size_t kAtim    = 0x18;  // timespec{int64 sec, int64 nsec}
constexpr size_t kMtim    = 0x28;
constexpr size_t kCtim    = 0x38;
constexpr size_t kSize    = 0x48;  // int64
constexpr size_t kBlocks  = 0x50;  // int64
constexpr size_t kBlksize = 0x58;  // uint32
constexpr size_t kFlags   = 0x5c;  // uint32
constexpr size_t kGen     = 0x60;  // uint32
constexpr size_t kBirth   = 0x68;

// memcpy rather than a reinterpret_cast to a packed struct: the buffer's alignment is
// the caller's business and an unaligned load is not worth the risk for a cold path.
template<class T>
static T fieldAt(const unsigned char* raw, size_t off) {
  T v = T();
  std::memcpy(&v,raw+off,sizeof(T));
  return v;
  }

static void readTimespec(const unsigned char* raw, size_t off, struct timespec& out) {
  out.tv_sec  = static_cast<decltype(out.tv_sec)>(fieldAt<int64_t>(raw,off));
  out.tv_nsec = static_cast<decltype(out.tv_nsec)>(fieldAt<int64_t>(raw,off+8));
  }

// The whole point of this file: kernel bytes in, caller's struct stat out, by NAME on
// the caller's side so a future SDK that fixes its declaration keeps working.
static void translate(const unsigned char* raw, struct stat* out) {
  std::memset(out,0,sizeof(*out));
  out->st_dev     = static_cast<decltype(out->st_dev)>    (fieldAt<uint32_t>(raw,kDev));
  out->st_ino     = static_cast<decltype(out->st_ino)>    (fieldAt<uint32_t>(raw,kIno));
  out->st_mode    = static_cast<decltype(out->st_mode)>   (fieldAt<uint16_t>(raw,kMode));
  out->st_nlink   = static_cast<decltype(out->st_nlink)>  (fieldAt<uint16_t>(raw,kNlink));
  out->st_uid     = static_cast<decltype(out->st_uid)>    (fieldAt<uint32_t>(raw,kUid));
  out->st_gid     = static_cast<decltype(out->st_gid)>    (fieldAt<uint32_t>(raw,kGid));
  out->st_rdev    = static_cast<decltype(out->st_rdev)>   (fieldAt<uint32_t>(raw,kRdev));
  out->st_size    = static_cast<decltype(out->st_size)>   (fieldAt<int64_t> (raw,kSize));
  out->st_blocks  = static_cast<decltype(out->st_blocks)> (fieldAt<int64_t> (raw,kBlocks));
  out->st_blksize = static_cast<decltype(out->st_blksize)>(fieldAt<uint32_t>(raw,kBlksize));
  out->st_flags   = fieldAt<uint32_t>(raw,kFlags);
  out->st_gen     = fieldAt<uint32_t>(raw,kGen);
  readTimespec(raw,kAtim, out->st_atim);
  readTimespec(raw,kMtim, out->st_mtim);
  readTimespec(raw,kCtim, out->st_ctim);
  readTimespec(raw,kBirth,out->st_birthtim);

  // ---------------------------------------------------------------- the second writer
  //
  // THE SDK DISAGREES WITH ITSELF, and translating to the public header alone is not
  // enough. Two kinds of consumer read this buffer:
  //
  //   * code compiled from source against the SDK's <sys/stat.h> - ZenKit, Tempest,
  //     OpenGothic - which reads st_size at 0x50. The field assignments above serve it.
  //   * code PREBUILT INTO THE SDK, above all libc++'s <filesystem> in libc++.a, which
  //     was compiled against the TRUE kernel layout and reads st_size at 0x48.
  //
  // Measured, on the emulator, with the assignments above and nothing else:
  //     stat()                 rc=0 st_size=722595072 st_mode=0100644   <- correct
  //     filesystem::file_size  0 (no error)                             <- WRONG
  //     filesystem::is_regular 1                                        <- correct
  // is_regular_file is right because st_mode sits at offset 8 in BOTH layouts (the
  // 4-byte read of a 2-byte field still yields S_IFREG|0644 in the low half). file_size
  // is 0 because offset 0x48 in the public layout is st_ctim.tv_nsec (offsetof measured:
  // st_ctim = 0x40), which is 0 for these files.
  //
  // That single zero is what broke the port: Resources::detectVdf admits an archive only
  // `if(std::filesystem::file_size(ar.name)>0)` (game/resources.cpp:269), so all 15
  // archives were dropped silently, nothing was ever mounted, and every resource lookup
  // returned nullptr - reported as `failed to open resource: font_old_20_white.fnt`.
  //
  // So the size is written at BOTH offsets. The cost is st_ctim.tv_nsec, which becomes
  // the file size instead of a nanosecond count; nothing in this tree reads it, and a
  // wrong ctime nanosecond is a far smaller lie than a zero file size. st_ctim.tv_sec is
  // untouched, so ctime itself still works.
  static_assert(offsetof(struct stat,st_size)!=kSize,
                "the SDK header now agrees with the kernel - delete this block");
  static_assert(offsetof(struct stat,st_ctim)+8==kSize,
                "0x48 is no longer st_ctim.tv_nsec - re-measure before clobbering it");
  const int64_t trueSize = fieldAt<int64_t>(raw,kSize);
  std::memcpy(reinterpret_cast<unsigned char*>(out)+kSize,&trueSize,sizeof(trueSize));
  }

// 256 rather than 120: the buffer the kernel writes into must be at least as large as
// whatever it decides to write, and overshooting a stack buffer costs nothing.
constexpr size_t kScratch = 256;

}

namespace orbis {

int rawKernelFstat(int fd, void* out) {
  std::memset(out,0,kSonyStatSize);
  return _fstat(fd,out);
  }

int64_t sonyStatQword(const void* raw, size_t offset) {
  return fieldAt<int64_t>(static_cast<const unsigned char*>(raw),offset);
  }

}

// ------------------------------------------------------------------ the interposition

extern "C" int fstat(int fd, struct stat* out) {
  if(out==nullptr) {
    errno = EFAULT;
    return -1;
    }
  unsigned char raw[kScratch] = {};
  const int rc = _fstat(fd,raw);
  if(rc!=0)
    return rc;
  translate(raw,out);
  return 0;
  }

extern "C" int stat(const char* path, struct stat* out) {
  if(path==nullptr || out==nullptr) {
    errno = EFAULT;
    return -1;
    }
  // ANCHORED, for the same reason `open` is (og_ps4_paths.h): this process has no working
  // directory - getcwd is ENOSYS on hardware - so a relative path names nothing. It matters HERE
  // and not only in open() because OpenGothic's FileUtil::exists is a `stat` of the relative save
  // name, so an anchor that covered only opens would write saves the load menu reports as missing.
  char          abuf[512] = {};
  const char*   apath     = orbis::anchorPath(path,abuf,sizeof(abuf));
  unsigned char raw[kScratch] = {};
  const int rc = sceKernelStat(apath,raw);
  if(rc!=0)
    return rc;
  translate(raw,out);
  return 0;
  }

// No symlinks exist on the filesystems a PS4 title reads, so following one is a
// distinction without a difference - and the alternative is what is there today, which
// is libc.a's fstatat stub returning ENOSYS unconditionally.
extern "C" int lstat(const char* path, struct stat* out) {
  return stat(path,out);
  }

// Only the two forms that occur in practice: an absolute path (fd ignored) and a
// path relative to AT_FDCWD. Anything else keeps the ENOSYS the SDK already returned,
// rather than silently answering about the wrong file.
extern "C" int fstatat(int fd, const char* path, struct stat* out, int flag) {
  (void)flag;
  if(path==nullptr || out==nullptr) {
    errno = EFAULT;
    return -1;
    }
  if(path[0]=='/' || fd==AT_FDCWD)
    return stat(path,out);
  errno = ENOSYS;
  return -1;
  }
