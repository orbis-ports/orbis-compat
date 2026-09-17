// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
//
// The parameter blocks the PlayStation 4 loader reads out of an executable or a module, as
// structures - so that the offsets are a compiler-checked fact rather than a hand-counted one.
//
// ⚠ EVERY LAYOUT HERE WAS MEASURED, NOT COPIED. The method, so anyone can repeat it:
//
//     objdump -h  $OO_PS4_TOOLCHAIN/lib/crt1.o     which sections exist, and how big
//     objdump -t  $OO_PS4_TOOLCHAIN/lib/crt1.o     which symbol sits at which offset in them
//     objdump -r  $OO_PS4_TOOLCHAIN/lib/crt1.o     which offset holds a pointer, and to what
//     objdump -s -j <section> ...                  the bytes themselves
//
// A relocation record IS the field list: `.data.sce_process_param+0x38 -> _sceLibcParam` says, with
// no ambiguity, that offset 0x38 of the process parameter block is a pointer to the libc parameter
// block. Fields with no relocation and no non-zero bytes are unknown and are named `unknown_<off>`
// here - naming them anything else would be inventing a fact.
//
// The four crt objects the SDK ships for executables - crt1.o, Scrt1.o, rcrt1.o, crt_dyn.o - carry
// BYTE-IDENTICAL copies of all five blocks below, which is what makes the unknown constants
// trustworthy as constants: they are not a property of one object's build.
//
// Sizes, from the section headers of $OO_PS4_TOOLCHAIN/lib/crt1.o (SDK build: "clang version
// 18.1.4", per its .comment):
//
//     .data.sce_process_param                     0x50
//     .data.rel.ro._sceLibcParam                  0x90
//     .data.rel.ro._sceKernelMemParam             0x30
//     .data.rel.ro._sceKernelFsParam              0x10
//     .data.rel.ro._sceLibcMallocReplace          0x70
//     .data.rel.ro._sceLibcNewReplace             0x70
//     .data.rel.ro._sceLibcMallocReplaceForTls    0x38
//
// and for a module, from $OO_PS4_TOOLCHAIN/lib/crtlib.o:
//
//     .data.sce_module_param                      0x18
//
// ⚠ THE SECTION NAMES ARE THE INTERFACE, not the symbol names. Every symbol naming one of these
// blocks is LOCAL in the SDK's objects, so nothing resolves them by name; the loader finds them
// because cmake/orbis-tls.ld (and the SDK's link.x it descends from) KEEPs `.data.sce_process_param`
// and `.data.sce_module_param` into their own page-aligned output sections. Rename a struct here
// and nothing breaks. Rename a section and the console gets an executable with no parameters.
#ifndef ORBIS_SCE_PARAMS_H
#define ORBIS_SCE_PARAMS_H

#include <stddef.h>
#include <stdint.h>

// ---------------------------------------------------------------- libc parameters
//
// 0x10 of header followed by sixteen pointer slots. Five of the sixteen are identified, by the five
// relocations `.data.rel.ro._sceLibcParam` carries; the other eleven are zero in every SDK object.
struct orbis_libc_param {
    uint64_t size;                       // 0x00  0x90
    uint32_t unknown_08;                 // 0x08  0x0000000c in every SDK crt object
    uint32_t unknown_0c;                 // 0x0c  0x00000001 in every SDK crt object
    const size_t   *heap_size;           // 0x10  -> sceLibcHeapSize
    const void     *unknown_18;          // 0x18
    const uint32_t *heap_extended_alloc; // 0x20  -> sceLibcHeapExtendedAlloc
    const void     *unknown_28;          // 0x28
    const void     *malloc_replace;      // 0x30  -> _sceLibcMallocReplace
    const void     *new_replace;         // 0x38  -> _sceLibcNewReplace
    const void     *unknown_40[4];       // 0x40 0x48 0x50 0x58
    const void     *malloc_replace_for_tls; // 0x60  -> _sceLibcMallocReplaceForTls
    const void     *unknown_68[5];       // 0x68 0x70 0x78 0x80 0x88
};

// ---------------------------------------------------------------- allocator replacement tables
//
// Three blocks of the same shape: a size, a version, and a table of function pointers that is
// entirely NULL in the SDK's objects - i.e. "no replacement installed". Their sizes and versions are
// what distinguishes them, and both are measured:
//
//     _sceLibcMallocReplace        size 0x70   version 1   12 slots
//     _sceLibcNewReplace           size 0x70   version 2   12 slots
//     _sceLibcMallocReplaceForTls  size 0x38   version 1    5 slots
//
// ⚠ The tables are reproduced all-NULL deliberately. An allocator replacement is a policy this
// repository already has a considered position on - src/orbis_mem.cpp interposes at the libc level
// instead - and installing one here would put it on the path of every consumer with no way to opt
// out. See §2.5 of the README.
struct orbis_malloc_replace {
    uint64_t    size;                    // 0x00  0x70
    uint64_t    version;                 // 0x08  1
    const void *entry[12];               // 0x10..0x68
};

struct orbis_new_replace {
    uint64_t    size;                    // 0x00  0x70
    uint64_t    version;                 // 0x08  2
    const void *entry[12];               // 0x10..0x68
};

struct orbis_malloc_replace_for_tls {
    uint64_t    size;                    // 0x00  0x38
    uint64_t    version;                 // 0x08  1
    const void *entry[5];                // 0x10..0x30
};

// ---------------------------------------------------------------- kernel parameters
//
// Both are size-only in every SDK object: no relocations, no non-zero bytes past offset 0. They are
// still emitted, because the process parameter block points at them and a null pointer there is a
// different statement from "a block that asks for nothing".
struct orbis_kernel_mem_param {
    uint64_t    size;                    // 0x00  0x30
    const void *unknown_08[5];           // 0x08..0x28
};

struct orbis_kernel_fs_param {
    uint64_t    size;                    // 0x00  0x10
    const void *unknown_08;              // 0x08
};

// ---------------------------------------------------------------- process parameters
//
// What the loader reads out of `.data.sce_process_param` for an EXECUTABLE.
//
// The magic is the four ASCII bytes 4F 52 42 49 - "ORBI" - which read as 0x4942524f as a
// little-endian word. The three pointers at 0x38/0x40/0x48 are exactly the three relocations the
// section carries, and `entry_count` is 3, which is the same three.
//
// ⚠ `main_thread_stack_size` IS NOT A FIELD HERE, and the four unknown slots at 0x18..0x30 are where
// one would sit on other Sony targets. They are zero in the SDK's object and this repository has a
// measurement that says the loader's own default is what applies: README §2.8 measured the main
// thread at 2 MiB on the console while every other thread got 64 KiB. Putting a guess in one of
// those slots would be changing a number that is already known to be right.
#define ORBIS_PROCESS_PARAM_MAGIC 0x4942524fu   // "ORBI", little-endian

struct orbis_process_param {
    uint64_t    size;                    // 0x00  0x50
    uint32_t    magic;                   // 0x08  "ORBI"
    uint32_t    entry_count;             // 0x0c  3
    uint32_t    sdk_version;             // 0x10  0x04508101
    uint32_t    unknown_14;              // 0x14
    const void *unknown_18[4];           // 0x18 0x20 0x28 0x30
    const struct orbis_libc_param       *libc_param;        // 0x38
    const struct orbis_kernel_mem_param *kernel_mem_param;  // 0x40
    const struct orbis_kernel_fs_param  *kernel_fs_param;   // 0x48
};

// ---------------------------------------------------------------- module parameters
//
// What the loader reads out of `.data.sce_module_param` for a MODULE (.prx/.sprx). A different
// block from the one above, in a different section, with a different magic - 0x13c13f4bf, which is
// 33 bits wide and therefore a quad, not the four-byte ASCII tag an executable carries.
//
// From `objdump -s -j .data.sce_module_param $OO_PS4_TOOLCHAIN/lib/crtlib.o`:
//
//     0000 18000000 00000000 bff4133c 01000000
//     0010 51000001 00000000
struct orbis_module_param {
    uint64_t size;                       // 0x00  0x18
    uint64_t magic;                      // 0x08  0x13c13f4bf
    uint64_t sdk_version;                // 0x10  0x1000051
};

#define ORBIS_MODULE_PARAM_MAGIC 0x13c13f4bfULL

// ---------------------------------------------------------------- the offsets, checked
//
// Everything above is a claim about a binary layout, so the compiler is made to check it. A field
// reordered by an edit becomes a build failure here rather than a console that refuses to start.
_Static_assert(sizeof(struct orbis_libc_param) == 0x90, "_sceLibcParam is not 0x90 bytes");
_Static_assert(offsetof(struct orbis_libc_param, heap_size) == 0x10, "heap_size moved off 0x10");
_Static_assert(offsetof(struct orbis_libc_param, heap_extended_alloc) == 0x20, "heap_extended_alloc moved off 0x20");
_Static_assert(offsetof(struct orbis_libc_param, malloc_replace) == 0x30, "malloc_replace moved off 0x30");
_Static_assert(offsetof(struct orbis_libc_param, new_replace) == 0x38, "new_replace moved off 0x38");
_Static_assert(offsetof(struct orbis_libc_param, malloc_replace_for_tls) == 0x60, "malloc_replace_for_tls moved off 0x60");

_Static_assert(sizeof(struct orbis_malloc_replace) == 0x70, "_sceLibcMallocReplace is not 0x70 bytes");
_Static_assert(sizeof(struct orbis_new_replace) == 0x70, "_sceLibcNewReplace is not 0x70 bytes");
_Static_assert(sizeof(struct orbis_malloc_replace_for_tls) == 0x38, "_sceLibcMallocReplaceForTls is not 0x38 bytes");
_Static_assert(sizeof(struct orbis_kernel_mem_param) == 0x30, "_sceKernelMemParam is not 0x30 bytes");
_Static_assert(sizeof(struct orbis_kernel_fs_param) == 0x10, "_sceKernelFsParam is not 0x10 bytes");

_Static_assert(sizeof(struct orbis_process_param) == 0x50, "sce_process_param is not 0x50 bytes");
_Static_assert(offsetof(struct orbis_process_param, magic) == 0x08, "the ORBI magic moved off 0x08");
_Static_assert(offsetof(struct orbis_process_param, sdk_version) == 0x10, "sdk_version moved off 0x10");
_Static_assert(offsetof(struct orbis_process_param, libc_param) == 0x38, "libc_param moved off 0x38");
_Static_assert(offsetof(struct orbis_process_param, kernel_mem_param) == 0x40, "kernel_mem_param moved off 0x40");
_Static_assert(offsetof(struct orbis_process_param, kernel_fs_param) == 0x48, "kernel_fs_param moved off 0x48");

_Static_assert(sizeof(struct orbis_module_param) == 0x18, "sce_module_param is not 0x18 bytes");

#endif  // ORBIS_SCE_PARAMS_H
