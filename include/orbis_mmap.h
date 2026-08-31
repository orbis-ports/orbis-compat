// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// musl's anonymous mappings, served out of direct memory instead of flexible memory.
//
// ------------------------------------------------------------------ why this exists
//
// A PS4 process has two disjoint budgets (og_ps4_mem.h). Everything malloc grows into
// is FLEXIBLE memory, and on this console that pool gave the title 427008 KiB at the
// first instruction of boot(). Loading NEWWORLD.ZEN wants more than 405 MiB of it and
// was still climbing when it ran out. At that exact instant DIRECT memory had
// 4601856 KiB free and had not moved since startup - the healthy pool was not the pool
// that failed (console run 2026-08-04 15:50).
//
// So the memory exists; malloc simply cannot reach it. This file makes it reachable,
// entirely inside the eboot, without a line of upstream OpenGothic or Tempest change.
//
// --------------------------------------------------- what is interposed, and why THAT
//
// Not `mmap`. Three internal names, all of which musl's allocator - and ONLY musl's
// allocator - routes through. Measured by disassembling the SDK's own libc.a, which is
// the allocator this title actually links:
//
//   mmap.lo      `__mmap`    is one instruction: `jmp mmap`  (R_X86_64_PLT32 mmap-0x4)
//   munmap.lo    `__munmap`  is one instruction: `jmp munmap`
//   madvise.lo   `__madvise` is one instruction: `jmp madvise`
//
// and the callers:
//
//   expand_heap.lo  U __mmap                            heap growth
//   malloc.lo       U __mmap __munmap __madvise __mremap large allocations
//   __map_file.lo   U __mmap                            locale/timezone, FILE-BACKED
//   __tz.lo, locale_map.lo, dcngettext.lo  U __munmap    the unmap half of the above
//
// Nothing else in libc.a, libc++.a, libc++abi.a, libunwind.a or the crt objects
// references any of the three (checked with llvm-nm over all four archives). Everything
// that is NOT musl's allocator - ZenKit's archive mmap, `__init_tls`, `catopen` - calls
// plain `mmap`/`munmap` and never touches this file at all. That is the answer to "what
// must be forwarded untouched", and it is answered by the symbol graph rather than by a
// runtime test: the archive mount path that hung the console is not reachable
// from here.
//
// The archive rule does the rest. `mmap.lo` defines `__mmap`, a WEAK `mmap64` and a WEAK
// `__vm_wait`, and nothing anywhere references either weak symbol - so mmap.lo is pulled
// out of libc.a for exactly one reason: to satisfy `__mmap`. Define `__mmap` in a source
// of the executable and mmap.lo is never pulled, so there is no duplicate symbol and
// libkernel's real `mmap` stays reachable BY NAME. munmap.lo and madvise.lo are the same
// shape. The forward path is therefore a call to the very function musl was jumping to,
// with identical semantics - not a re-implementation over sceKernelMmap, whose contract
// is undocumented in the pinned oracles and whose misreading would take the title down.
//
// If a future SDK makes some other member define these names, the link fails with a
// duplicate symbol. That is the correct failure and it happens at build time.
//
// ------------------------------------------------------------------ what is taken
//
// Only the shape musl's two allocator call sites actually emit, spelled exactly:
//
//   addr == nullptr, prot == PROT_READ|PROT_WRITE (3), flags == MAP_PRIVATE|MAP_ANON
//   (0x1002) with no other bit set, fd < 0, offset == 0
//
// measured at malloc+0x6b..0x83 and __expand_heap+0x5c..0x6a - both push exactly
// `movl $3,%edx; movl $0x1002,%ecx; movl $-1,%r8d; xorl %r9d,%r9d` with a zero addr.
// Anything else - a file-backed mapping, MAP_FIXED, an address hint, PROT_EXEC - is
// forwarded to the real `mmap` unexamined. Conservative by construction: the filter
// admits one exact tuple rather than excluding the cases we happened to think of.
//
// ------------------------------------------------ granularity: suballocate, not carve
//
// Direct memory carves at 2 MiB (`OO samples/_common/graphics.cpp:59,116-119`, the same
// quantum gnmallocator.cpp uses). musl mmaps every large allocation individually, so one
// carve-out per mapping is not an option, and the choice the design had was a
// suballocator over a few large carve-outs versus a size threshold that leaves small
// mappings on flexible memory.
//
// SUBALLOCATOR, and the measurement decides it rather than taste: musl's MMAP_THRESHOLD
// is 0x38000 = 224 KiB (`cmpq $0x38001,%rbx` at malloc+0x51), and every mapping is then
// rounded up to a 16 KiB page (`addq $0x400f; andq $-0x4000`, so PAGE_SIZE is 16 KiB on
// this platform - measured twice, the same constant appears in __expand_heap). There are
// therefore NO small anonymous mappings to send anywhere: the smallest one musl can ask
// for is 240 KiB. A size threshold would have to sit above 224 KiB, which would leave
// the entire large-allocation stream - the 247 MiB the .zen parse costs - on the pool
// that overflows, i.e. it would decline to fix the bug. Meanwhile a 2 MiB carve per
// 240 KiB mapping wastes 88% and burns a kernel object per malloc.
//
// So: 128 MiB carve-outs, first-fit from an address-sorted block list per carve-out,
// 16 KiB granularity (a multiple of which every carve base already is, since 2 MiB is a
// multiple of 16 KiB - static_asserted rather than assumed, design point 7).
//
// ---------------------------------------------------------------- munmap and reuse
//
// musl always unmaps EXACTLY what it was given. malloc's free path recomputes the base
// from the chunk header (`addq %rax,%rsi; subq %rax,%rdi; jmp __munmap` at free+0x20)
// and realloc's does the same, so an unmap is always (exact base, exact length) of a
// mapping this file handed out. Blocks are freed on that exact match and coalesced with
// both neighbours; a partial or unaligned unmap inside a carve-out is a case that cannot
// happen, so it is reported loudly and the block is LEAKED rather than guessed at.
// Handing an address back on a guess is the one outcome worse than losing it.
//
// A recycled block is ZEROED before it is handed back, and that is a correctness
// requirement rather than hygiene. musl's calloc skips clearing entirely for a mapped
// chunk - `testb $0x1,-0x8(%rbx); je` at calloc+0x5f tests C_INUSE, which is 0 exactly
// for mmapped chunks, and that branch returns the pointer without going near mal0_clear -
// because a real MAP_ANON mapping arrives zero filled from the kernel. Hand back a dirty
// block and calloc() returns garbage at a call site that has no way to notice. The zeroing
// is unconditional: no pinned source says whether sceKernelAllocateDirectMemory returns
// zeroed pages, and that is not a question to settle by assuming the convenient answer.
//
// `__mremap` needs nothing: madvise.lo's neighbour mremap.lo is a hard stub that sets
// errno 78 (ENOSYS) and returns -1 unconditionally, so realloc of a mapped chunk always
// degrades to malloc + memcpy + __munmap, which this file already handles.
//
// `__madvise` DOES need something. musl's __bin_chunk releases the interior of a large
// freed span with `__madvise(a, b-a, 4)` - MADV_DONTNEED, oracle
// `freebsd9/sys_sys_mman.h:123` - at __bin_chunk+0x651..0x675. On a direct-memory
// mapping that call has no defined meaning in any pinned source: the pages are physical
// by construction and cannot be reclaimed by advice. It is a no-op INSIDE our carve-outs
// (returning success, which is what musl expects and does not check) and forwarded
// everywhere else, rather than left to find out what Sony's kernel does to a physically
// backed mapping that has just been told nobody needs it.
//
// ------------------------------------------------- coexistence with the GNM allocator
//
// gnmallocator.cpp takes direct memory too, and grows at runtime. Its own hard ceiling
// is arithmetic, not a guess: gnmdevice.cpp:44-45 asks for a 16 MiB onion and a 96 MiB
// garlic arena, and gnmallocator.h:120 caps each bus at `MaxArenasPerBus = 4` carve-outs
// sized like the first, so it can never hold more than 4*16 + 4*96 = 448 MiB unless a
// single allocation exceeds an arena.
//
// This file is capped at 1536 MiB. Measured direct-memory free at the instant of the OOM
// was 4601856 KiB (4.39 GiB), so the cap leaves 2.89 GiB - six and a half times the GNM
// allocator's entire ceiling - and it is a cap on a pool that is grown ON DEMAND, one
// 128 MiB carve-out at a time, so a title that needs 300 MiB holds 384 MiB and not
// 1.5 GiB. Every carve-out prints its size and physical address when it is taken.
//
// Direct memory is physical, so a carve-out costs its full size the moment it is taken -
// there is no lazy population to hide behind (design point 6). That is the reason the
// reservation grows on demand instead of being taken up front, and the reason the cap is
// stated against a measured free figure rather than against the pool total.
//
// Carve-outs are never released. musl never returns heap-expansion mappings, so the
// first carve-out can never become empty anyway, and releasing a later one would buy
// direct memory that nothing measured is short of while adding an unmap/release path
// that, if it were ever wrong, would hand out an address whose pages are no longer ours.
// The high-water figures in the report are what a future decision to reclaim would be
// sized against.
//
// ------------------------------------------------------------------ thread safety
//
// malloc is called from every thread and the world load runs two async builders, so the
// block lists are under a lock - and it is a hand-rolled spin/yield lock rather than a
// pthread mutex for a specific reason: `pthread_mutex_lock` on this platform comes from
// libkernel, and a statically initialised mutex on a FreeBSD-derived libthr allocates on
// first lock. That allocation would arrive inside malloc, while malloc's own lock is
// held, through the very function it is trying to call. A lock that can call malloc
// cannot be the lock that malloc's mmap sits behind.
//
// For the same reason nothing on any path here allocates: no std::vector, no std::string,
// fixed-capacity arrays in .bss. `orbis_log` is safe to call from inside it - it formats
// into a 512-byte stack buffer and musl's vsnprintf/vfprintf reference no allocator
// (only vasprintf.lo does).
//
// ------------------------------------------------------------------ turning it off
//
// ORBIS_MMAP_DIRECT=0 at build time (build.sh --define ORBIS_MMAP_DIRECT=0) restores the
// pre-2026-08-04 behaviour EXACTLY, and exactly is meant
// literally: the disabled path is `return mmap(...)`, which is the one instruction
// musl's `__mmap` was executing before this file existed. Which path is active is named
// in the log at the first anonymous mapping and again in every census line, so a capture
// can never be ambiguous about which build produced it.
#pragma once

#include <cstddef>

#ifndef ORBIS_MMAP_DIRECT
// On. A knob whose default is the configuration nobody would deliberately choose is how
// this project has already lost two console runs (conventions.md, "Defaults that every
// deliberate target overrides"): the fix has to be what a build gets without asking.
#define ORBIS_MMAP_DIRECT 1

/* ⚠ HOW MUCH OF musl's ALLOCATION TRAFFIC REACHES libkernel. A request the carve-outs can serve costs
   no syscall; one they cannot falls through to the platform's mmap/munmap, and those are the only
   libkernel calls the frontend's and zink's allocations make. The frame ledger put 9073 of 9280 bytes
   a frame in segments above the graphics driver where no counted call is made, and this path is the
   one that was never counted. Any pointer may be null. */
extern "C" void orbis_mmap_counts(unsigned long long* maps, unsigned long long* maps_carved,
                                  unsigned long long* maps_fell, unsigned long long* unmaps,
                                  unsigned long long* unmaps_carved, unsigned long long* unmaps_fell);

#endif

namespace orbis {

// One line of interposer state, appended to a census. `where` is printed verbatim and
// should be the same phase name the census used. Allocates nothing and is safe to call
// after malloc has started returning null.
void mmapDirectReport(const char* where);

// What a single address is, in this file's terms. Filled by mmapDirectDescribe below.
//
// `index` numbers the carve-out the way the "carve-out N taken" banner does, so a fault
// line and a startup line can be read against each other. The block fields describe the
// suballocated block the address falls in - and `blockUsed` is the field worth crossing a
// room for: an address inside a FREE block is a use-after-free, stated rather than
// inferred, which is otherwise one of the hardest things on this platform to prove.
struct MmapDirectSpan {
  unsigned           index;      // 1-based, as the banner prints it
  const void*        base;       // carve-out base
  unsigned long long size;       // carve-out size in bytes
  unsigned long long offset;     // addr - base
  bool               blockFound;
  bool               blockUsed;  // false == the address is inside memory that was FREED
  unsigned long long blockOff;   // block start, as an offset into the carve-out
  unsigned long long blockSize;
  };

// True if `addr` falls inside a carve-out this file handed out, with `out` filled in.
//
// ⚠ SAFE TO CALL FROM A SIGNAL HANDLER, AND THAT IS THE ONLY REASON IT EXISTS SEPARATELY
// FROM mmapDirectReport. It takes no lock, allocates nothing, and calls nothing that
// might: see the comment on the definition for why reading the table unlocked is sound
// and what it can and cannot get wrong.
bool mmapDirectDescribe(const void* addr, MmapDirectSpan& out);

}
