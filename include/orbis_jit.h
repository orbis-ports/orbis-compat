// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
//
// One arena of writable, executable memory, shared by every recompiler in a module.
//
// This replaces two files that between them already specified it from both ends:
// RetroArch/ps4/orbis_exec_mem.c, which PROMOTES pages a core already owns, and
// beetle-psx-libretro/ps4/orbis_lightrec_mem.c, which MAPS a buffer up front. Both shapes are
// here, and so is the third thing neither of them had: suballocation, so that N recompilers in one
// module cost one kernel arena rather than N.
//
// ⚠ THERE IS NO W^X ON THIS PLATFORM, AND IT IS NOT A CHOICE THIS FILE MAKES. The kernel's own
// answer to W^X is the sceKernelJit* family - CreateSharedMemory, CreateAliasOfSharedMemory,
// MapSharedMemory - which hands out a writable alias and an executable alias of the same physical
// pages. Measured on the SDK, 2026-09-18: those four symbols are exported by libkernel_sys.so,
// libkernel_jvm.so, libkernel_ps2emu.so and libkernel_psmkit.so, and by NONE of them from
// lib/libkernel.so, which is the stub `-lkernel` resolves against on every link line this port
// makes (orbis-porting-kit/cmake/ps4-openorbis.cmake:346). Referencing one is a link error, not a
// runtime refusal. The SDK's own declarations say the same thing a second way: they are spelled
// `void sceKernelJitCreateSharedMemory();` with no parameters, so nobody here has ever called one.
//
// What IS granted is map read-write, then promote with sceKernelMprotect. Asking
// sceKernelMapDirectMemory for READ|EXECUTE at map time returns 0x8002000d, EACCES - understood and
// declined, not malformed - so the policy lives at MAP time and not at PROTECT time. Everything
// this file hands out is therefore RWX for its whole lifetime.
//
// ⚠ SO THERE IS NO RW-TO-RX TOGGLE HERE, AND A CONSUMER THAT HAS ONE MUST TURN IT OFF. dynarmic's
// BlockOfCode::EnableWriting/DisableWriting mprotect the whole arena around every emitted block,
// and paraLLEl-RSP's commit_execute sets PROT_EXEC ALONE on a range it will write to again. Both
// are compiled out on platforms that ask them to be - dynarmic's is DYNARMIC_ENABLE_NO_EXECUTE_
// SUPPORT, OFF by default - and they must be, because the promotion above is granted once, at a
// moment chosen by policy, and a request to narrow it invites a refusal that cannot be undone.
//
// ⚠ AND THERE IS NO RESERVE-THEN-COMMIT. Executable pages come from direct memory, which hands out
// PHYSICAL pages at allocation time; there is no PROT_NONE reservation to grow into. A consumer
// built around reserving a gigabyte and committing as it fills - paraLLEl-RSP's jit_allocator,
// dynarmic's Windows arm - has to ask for what it will really use. On a console with no swap the
// reservation size simply fails.
//
// ⚠ AND A GRANTED PROTECTION IS NOT AN HONOURED ONE. sceKernelQueryMemoryProtection reports the
// protection a range was MAPPED with rather than what mprotect has since made it - it answered 0x03
// for ranges that were executing code, at sixteen base addresses out of sixteen
// (ps4-mesa-docs docs/retroarch/HANDOFF.md, 2026-08-23). So every range this file hands out has had six bytes of
// x86-64 written into it and CALLED before the caller sees it.
#ifndef ORBIS_JIT_H
#define ORBIS_JIT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/// The unit sceKernelMprotect and sceKernelMapDirectMemory work in. 16 KiB, not the 4096 a Linux
/// intuition supplies, and the difference is not cosmetic: swanstation's 4096-byte JIT guard page
/// was rounded up to this and the recompiler's first write landed 12 KiB inside its own guard.
/// Every length and address below is rounded to it, outwards.
#define ORBIS_JIT_GRANULE 0x4000u

/// ⚠ THE ALLOCATION MUST BE REACHABLE BY rel32 FROM THIS MODULE'S TEXT. x86-64 emitters write a
/// 32-bit displacement for calls and jumps leaving generated code - Xbyak, new_dynarec's emit_call
/// (0xe8 + disp32, assem_x64.c), dolphin's x64Emitter - so a buffer more than 2 GiB from the code
/// it calls is not slower, it is wrong. flycast measured it as an abort 0.2 s into a run:
///
///     libc++abi: terminating with uncaught exception of type Xbyak::Error: offset is too big
///
/// with an mmap'd cache at 0x899200000 against a module at 0x800870000. Upstream asserts this on
/// every emit and libretro builds with -DNDEBUG, which deletes all of them, so out of range does
/// not trip an assertion - it writes a truncated displacement and jumps somewhere arbitrary.
///
/// With this flag the arena is placed by address search and MAP_FIXED inside the window around this
/// module, and a request that cannot be placed there returns NULL. It does NOT fall back to letting
/// the kernel choose: a far pointer is worse than no pointer, and that is what the flycast abort
/// above is.
#define ORBIS_JIT_NEAR_TEXT 0x1u

/// Suballocate `size` bytes of writable, executable memory. Returns NULL if the console refuses,
/// which callers must treat as "clamp to the interpreter" rather than as a transient error.
///
/// The arena behind this is grown on demand and shared by every caller in the module, which is the
/// point: paraLLEl-RSP mapped an arena per module load and leaked it, and 2026-08-25 on hardware
/// the fourth load got `no direct memory for 65536 KiB -> 0x80020023`, EAGAIN, on a console that
/// had been running the same core minutes earlier.
void *orbis_jit_alloc(size_t size, unsigned flags);

/// Map a whole arena at exactly `addr`, for a caller that chooses its own layout - Lightrec's
/// custom map, which places the code buffer relative to its emulated address space.
///
/// ⚠ MAP_FIXED ON THIS KERNEL REPLACES WHATEVER IS THERE and there is no MAP_FIXED_NOREPLACE, so
/// this asks first and returns NULL if anything is in the way. "Ask for it and check what came
/// back" would already have unmapped the frontend's heap by the time it could check.
void *orbis_jit_alloc_at(void *addr, size_t size);

/// Give a block back - from either allocator; this file knows which. Quiet about an address it
/// never handed out: a destructor running for a construction that failed is normal.
///
/// ⚠ NO LENGTH ARGUMENT, AND THAT IS A REQUIREMENT RATHER THAN A CONVENIENCE. The seam every x86-64
/// consumer in this organisation reaches through is Xbyak::Allocator, whose signature is
/// `void free(uint8_t*)` with no size - dynarmic's own POSIX allocator wastes a page in front of
/// every allocation to store one (block_of_code.cpp:81-102). So the length lives with the block
/// here and callers do not have to carry it.
void orbis_jit_free(void *addr);

/// Promote pages the CALLER already owns - a code cache declared as a static array, which is how
/// mupen64plus, flycast and melonDS all carry theirs. Returns 1 if code can be written and run
/// there, 0 if it cannot.
///
/// ⚠ THIS IS ALSO HOW A LARGE .bss BECOMES WRITABLE, which is a separate platform fact that looks
/// like a different bug every time. swanstation faulted with si_code 2, SEGV_ACCERR - mapped but
/// not permitted - writing the first element of a static array 49 MB into its own .bss; the module
/// loader does not hand out writable pages for all of a 50 MB .bss. One promotion answers both.
///
/// Idempotent per range, and that is load-bearing rather than an optimisation: the proof below
/// writes six bytes at the start of the range, so a second call for pages already promoted would
/// scribble on whatever the recompiler has since generated there. Overlapping ranges are merged -
/// melonDS asks twice with the same length and bases 224 KiB apart (0x800e14000 and 0x800e4c000,
/// measured 2026-09-01), which containment misses both ways.
int orbis_jit_protect(void *addr, size_t len);

/// -1 nothing has asked yet, 0 the console refused, 1 there is executable memory.
///
/// ⚠ THREE VALUES AND NOT TWO. "Not asked" and "refused" lead to opposite decisions about the
/// recompiler, and a caller chooses its CPU mode before the recompiler has been near a buffer;
/// collapsing them turns the first frame of every run into a silent fall back to the interpreter.
/// This is the whole module's verdict - the AND of every range asked about - and it only ever gets
/// worse, because one buffer the console will not run makes the recompiler unusable whatever a
/// later range says.
int orbis_jit_state(void);

/// Whether a rel32 from anywhere in [from, from+len) can reach anywhere in [to, to+len2), for a
/// caller that wants to check rather than to abort on Xbyak's behalf.
int orbis_jit_reachable(const void *from, size_t len, const void *to, size_t len2);

/// Unmap every arena and release its physical pages. Nothing in this port calls it: module
/// destructors do not run here, which is why the leak above was measured in the first place. It
/// exists for a host that does unload cleanly, and for the tests.
void orbis_jit_release_all(void);

#ifdef __cplusplus
}
#endif

#endif /* ORBIS_JIT_H */
