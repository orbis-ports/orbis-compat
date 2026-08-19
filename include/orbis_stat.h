// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// Corrective interposition of the stat() family for PS4 builds.
//
// ---------------------------------------------------------------- the defect
//
// The OpenOrbis SDK's `struct stat` does not have the layout the PS4 kernel fills.
// libc.a does NOT translate: `fstat` is a tail jump to libkernel's `_fstat`
// (llvm-objdump: `jmp _fstat`, no body) and `stat` is not defined in libc.a at all -
// it is imported straight from libkernel.so. So whatever the kernel writes is what the
// caller reads, field for field, through the wrong declaration.
//
// BOTH LAYOUTS MEASURED, not assumed:
//
//   caller side, offsetof compiled with the PS4 toolchain against <sys/stat.h>:
//     st_dev 0(4)  st_ino 4(4)  st_mode 8(4)  st_nlink 12(2)  st_uid 16
//     st_atim 32   st_mtim 48   st_size 80    st_blocks 88    st_blksize 96
//     sizeof(struct stat) = 128
//
//   kernel side, the classic FreeBSD amd64 (pre-ino64) layout:
//     st_dev 0(4)  st_ino 4(4)  st_mode 8(2)  st_nlink 10(2)  st_uid 12  st_gid 16
//     st_rdev 20   st_atim 24   st_mtim 40    st_ctim 56      st_size 72(0x48)
//     st_blocks 80(0x50)        st_blksize 88 st_flags 92     st_gen 96
//     st_birthtim 104           sizeof = 120
//
// The shear lines up on exactly two fields, and the console measured both of them:
//
//   * st_size. The caller reads offset 0x50, which the kernel filled with st_blocks.
//     A 64857-byte SystemPack.vdf reported st_size = 128 on hardware (128 x 512 =
//     65536, the next allocation unit) and 127 under unemups4 (which computes
//     ceil(64857/512) exactly). Same field, two arithmetics.
//   * st_mode. The caller's st_mode is FOUR bytes at offset 8; the kernel wrote a
//     2-byte mode there and a 2-byte st_nlink right behind it. The console printed
//     0o300755 = (nlink 1 << 16) | 0o100755. Under unemups4, 0o300644 = (1 << 16) |
//     0o100644. Both decode exactly.
//
// ---------------------------------------------------------------- what it cost
//
// ZenKit's Mmap sizes its mapping from fstat's st_size (lib/ZenKit/src/MmapPosix.cc),
// so every .vdf was mapped 128 bytes long. Vfs::mount_disk then does
// read_string(256) for the comment and read_string(16) for the signature
// (lib/ZenKit/src/Vfs.cc:587-588), and ReadMemory::read clamps to the buffer length
// (src/Stream.cc:267) - so the comment read short, the position pinned at 128, the
// signature read returned ZERO bytes, and the error was
// `VFS disk signature not recognized: ""`. That is the console failure of 2026-08-02,
// explained end to end. mmap itself was never at fault: the probe measured
// file-backed mmap agreeing with read() byte for byte on hardware.
//
// ---------------------------------------------------------------- the authority
//
// The SDK offers none: `OrbisKernelStat` is `typedef struct stat OrbisKernelStat`
// (orbis/_types/kernel.h:177), i.e. an alias for the same wrong declaration, so
// sceKernelStat's own prototype is equally misdeclared. The kernel-side layout above
// is therefore taken from, in order of weight:
//   1. the console's own bytes - two independent fields, both decoding exactly;
//   2. unemups4's fill_sce_stat (crates/libs/src/libkernel/fs.rs:133), which writes
//      precisely this layout and which the console now corroborates field for field;
//   3. the classic FreeBSD amd64 `struct stat`, which is what a FreeBSD-9-derived
//      kernel fills.
// (3) is RECONSTRUCTED, not cited: there is no FreeBSD `sys/sys/stat.h` in
// ~/src/unemups4/oracles/freebsd9/. st_size at 0x48 is the one field the console has
// not yet been observed reading, which is why probeVdfRead() prints the raw quadwords
// at 0x48 AND 0x50 - the next console run confirms it rather than trusting this note.
//
// ---------------------------------------------------------------- how it hooks
//
// Strong definitions of stat/fstat/lstat/fstatat in this TU. `fstat` and `lstat` live
// in libc.a archive members, which the linker never extracts once the symbol is
// already defined; `stat` resolves here rather than against libkernel.so because a
// regular object file beats a shared library. Same mechanism as the open()/open64
// interposition in ps4/gapi-suite. The underlying calls are `sceKernelStat` (path) and
// `_fstat` (fd) - chosen because they are the two that BOTH libkernel.so exports and
// unemups4 binds (SYS_FSTAT names ["fstat","_fstat"], SCE_KERNEL_STAT "sceKernelStat"),
// so one binary works on both legs. sceKernelFstat is NOT used: unemups4 does not bind
// it, and a missing import is fatal there.
//
// Note lstat and fstatat were not merely wrong before, they were dead: libc.a's
// `fstatat` is a stub that sets errno 78 (ENOSYS) and returns -1, and musl's `lstat`
// is implemented on top of it.
#pragma once

#include <cstdint>
#include <cstddef>

struct stat;

namespace orbis {

// Bytes the kernel actually writes for one fd, unmodified, for diagnostics that want
// to see the raw layout rather than any interpretation of it. `out` must have room for
// kSonyStatSize. Returns the underlying call's return value.
constexpr size_t kSonyStatSize = 120;
int rawKernelFstat(int fd, void* out);

// Read a little-endian signed 64-bit field out of such a buffer.
int64_t sonyStatQword(const void* raw, size_t offset);

// Offsets used above, so the probe and the translation cannot drift apart.
constexpr size_t kSonyStatSizeOff   = 0x48;
constexpr size_t kSonyStatBlocksOff = 0x50;

}
