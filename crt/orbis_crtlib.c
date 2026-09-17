// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
//
// crtlib.o for the PlayStation 4: the entry and exit of a MODULE (.prx / .sprx), as opposed to an
// executable. crt/orbis_crt1.c is the executable's side of the same job, and the two are mutually
// exclusive - an image carries `.data.sce_process_param` or `.data.sce_module_param`, never both.
//
// ⚠ THIS IS THE ONE OBJECT WHOSE GPL-3.0 PROVENANCE IS NOT IN DOUBT. Unlike crt1.o, its source IS in
// the OpenOrbis toolchain repository - `src/crt/crtlib.c` and `src/crt/crtlib.S` - in a tree licensed
// GPL-3.0 with no linking exception and no per-file headers. It is 1.9 KB of object that links
// statically into every PRX built with that SDK. That is the copy this file exists to replace.
//
// Written from the interface, not from that source: section names, symbol names, visibilities and
// the `.data.sce_module_param` bytes, all measured here with objdump - see crt/orbis_sce_params.h
// for the method and crt/orbis_crt1.c for the provenance argument in full.
//
// ------------------------------------------------------------------ the module contract, measured
//
// From `objdump -h -t -d $OO_PS4_TOOLCHAIN/lib/crtlib.o`:
//
//     module_start   g F .text  .hidden   0x4b   (int64_t argc, const void *argp) -> int32_t
//     module_stop    g F .text  .hidden   0x10   same signature, returns 0
//     _init          g F .text            0x08   returns 0
//     _fini          g F .text            0x08   returns 0
//
// `module_start` and `module_stop` are what the loader calls; they are HIDDEN, which is not a
// contradiction - the loader finds a module's entry through the module parameter block and the
// dynamic tables, not by a dynamic symbol lookup. `_init` and `_fini` are the opposite: default
// visibility, because libc's `libc_start_init` calls `_init` by name, and because a module that also
// links libc needs the strong definition to beat libc's own weak `dummy` stub.
//
// ------------------------------------------------------------------ what this fixes on the way past
//
// ⚠⚠ THE SDK's crtlib.o CALLS A NULL POINTER AT MODULE ENTRY, and the mechanism is a compiler
// default that changed under it. Its `crtlib.c` declares
//
//     void (*__init_array_start[])(void);
//     void (*__init_array_end[])(void);
//
// which in C are TENTATIVE DEFINITIONS, not declarations. Under `-fcommon` - clang's default until
// clang 11 - each became a COMMON symbol, and a linker-defined symbol beats a common one, so the
// linker's real `.init_array` boundaries won and the loop below worked. Under `-fno-common`, the
// default since clang 11, each becomes an ordinary definition in `.bss`. `objdump -t` on the shipped
// object (built by "clang version 18.1.4", per its `.comment`) shows exactly that:
//
//     0000000000000000 g     O .bss  0000000000000008 __init_array_start
//     0000000000000008 g     O .bss  0000000000000008 __init_array_end
//
// two adjacent eight-byte zeroed slots. The walk from `&__init_array_start` to `&__init_array_end`
// is then exactly one iteration long and the one function pointer it calls is the zero in the first
// slot. Every constructor in the module is skipped and the module entry jumps to address 0.
//
// ⚠ NOT CONFIRMED ON HARDWARE. This is read off the object; no PRX was linked or run to see it fail,
// because this machine has no ld.lld and no console. What IS confirmed is the object's symbol table,
// which is the whole of the argument. `extern` below is the correction: it leaves both names
// undefined so the linker's own `__init_array_start`/`__init_array_end` are what the walk uses.
//
// ⚠ AND IT CHANGES THE SYMBOL TABLE, DELIBERATELY. Against the SDK's object, this one has those two
// names as UND rather than as `.bss` definitions, and `.bss` is therefore empty here and 0x10 bytes
// there. test/crt_abi.sh lists that as an EXPECTED difference rather than tolerating it silently.
#include "orbis_sce_params.h"

// ---------------------------------------------------------------- the module parameter block
//
// KEEPt by cmake/orbis-tls.ld into its own ALIGN(0x4000) output section, which is how the loader
// finds it. Bytes reproduced from `objdump -s -j .data.sce_module_param crtlib.o`; the magic is a
// 33-bit value and therefore a quad, not the four ASCII bytes an executable's block carries.
//
// ⚠ THE SYMBOL IS CALLED `_sceProcessParam` IN THE SHIPPED OBJECT, and that is a misnomer this file
// keeps. It is a module parameter block, in the module parameter section, and it is LOCAL - so the
// name reaches nothing and changing it would break nothing. It is kept so that a symbol-table diff
// against the original has one fewer line of noise in it; `objdump -t crtlib.o` is where anyone
// checking this will look first.
__attribute__((section(".data.sce_module_param"), used, aligned(8)))
static const struct orbis_module_param _sceProcessParam = {
    .size        = sizeof(struct orbis_module_param),
    .magic       = ORBIS_MODULE_PARAM_MAGIC,
    .sdk_version = 0x1000051,
};

// ---------------------------------------------------------------- .data
//
// Plain `.data` for the reason crt/orbis_crt1.c spells out: cmake/orbis-tls.ld matches `*(.data)` and
// not `*(.data.*)`, so a per-object data section here would be an orphan.
//
// `__dso_handle` is weak and hidden rather than local - the same single deviation crt1 makes, for the
// same measured reason: six members of the SDK's libc++.a reference it as GLOBAL HIDDEN UND and
// nothing in the SDK defines it globally. `_sceLibc` is carried verbatim: local, zero, referenced by
// nothing in the SDK.
#define ORBIS_CRT_DATA __attribute__((section(".data"), used))

ORBIS_CRT_DATA __attribute__((visibility("hidden"), weak)) void *__dso_handle = 0;
static ORBIS_CRT_DATA void *_sceLibc = 0;

// ---------------------------------------------------------------- the initialiser array
//
// Left UNDEFINED on purpose - see the ⚠⚠ above. ld.lld supplies both names as the boundaries of the
// `.init_array` output section; when a module has no `.init_array` at all it supplies them equal,
// and the loop below then does nothing, which is the correct behaviour for that case too.
extern void (*__init_array_start[])(void);
extern void (*__init_array_end[])(void);

// ---------------------------------------------------------------- entry and exit
//
// The loader's two calls. Hidden, matching the shipped object; `used` because nothing in this
// translation unit references them.
//
// ⚠ `module_stop` DOES NOT WALK `.fini_array`, and that matches the SDK. Adding it would be inventing
// a teardown contract rather than reproducing one, and a destructor that runs where the original ran
// none is a behaviour change that could only be discovered on hardware - which is not available here.
// If a module ever needs it, measure what the loader does with the return value first.
__attribute__((visibility("hidden"), used))
int32_t module_start(int64_t argc, const void *argp)
{
    (void)argc;
    (void)argp;

    for (void (**fn)(void) = __init_array_start; fn != __init_array_end; ++fn)
        (*fn)();

    return 0;
}

__attribute__((visibility("hidden"), used))
int32_t module_stop(int64_t argc, const void *argp)
{
    (void)argc;
    (void)argp;
    return 0;
}

// Default visibility, non-weak, returning 0 - as the shipped object has them. libc's own `_init` and
// `_fini` are WEAK stubs inside `__libc_start_main.lo`, so these win wherever both are linked, and a
// module that does not link libc still resolves them.
int32_t _init(void)
{
    return 0;
}

int32_t _fini(void)
{
    return 0;
}
