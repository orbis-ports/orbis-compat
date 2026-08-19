// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// Which of the PS4's two memory budgets a title is out of, said in one line.
//
// A PS4 process does not have one heap, it has two pools that never help each other:
//
//   direct memory    physical, carved with sceKernelAllocateDirectMemory and mapped by
//                    hand. This is where Engine/gapi/gnm/gnmallocator.cpp takes its
//                    arenas from, and it is the pool people mean when they say "the
//                    console has 8 GB".
//   flexible memory  what plain mmap(MAP_ANON) draws from, and therefore what musl's
//                    malloc grows into: __expand_heap() is a literal
//                    mmap(0,len,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0)
//                    (disassembled from the SDK's own libc.a, expand_heap.lo), and every
//                    request at or above musl's mmap threshold becomes a mapping of its
//                    own. A separate, much smaller per-process ceiling.
//
// So `std::bad_alloc` on this platform is never a statement about how much RAM the
// console has. It is a statement about the flexible pool, and the direct pool can be
// entirely idle at the same moment - which is exactly what the world-load failure of
// 2026-08-04 looked like: the GNM allocator carved a fresh 96 MiB direct arena one
// second AFTER OpenGothic gave up with "loading error: out of memory".
//
// Nothing here allocates, and nothing here is allowed to: the OOM report runs after
// malloc has already returned null.
#pragma once

namespace orbis {

// Record the flexible/direct figures the process starts with, and print them. Call once,
// as early as possible - every later census is read against this baseline, and
// "consumed since boot" is meaningless without it.
void memCensusBaseline();

// One line naming both pools at a point the caller wants attributed. `where` is printed
// verbatim, so it should be a phase name ("world zen parsed"), not a sentence.
void memCensus(const char* where);

/// What the thread interposer has done so far, in the same shape as the memory census. Costed:
/// every raised thread reserves more address space than the platform would have given it.
void memCensusThreads(const char* where);

}
