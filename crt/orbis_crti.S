/* Copyright © 2026 Mikołaj Mikołajczyk
 * SPDX-License-Identifier: MIT
 *
 * crti.o: the PROLOGUE half of the classic ELF `.init`/`.fini` pair. crt/orbis_crtn.S is the other
 * half, and neither is any use without the other.
 *
 * The convention, which is ELF's and not this platform's: the linker concatenates every `.init`
 * input section in link order into one `.init` output section, and the same for `.fini`. crti.o goes
 * FIRST on the link line and contributes the function label and the prologue; crtn.o goes LAST and
 * contributes the return. Anything a compiler emits into `.init` lands between them and becomes the
 * body. `_init` and `_fini` are therefore not functions anyone writes - they are functions the link
 * order assembles.
 *
 * The prologue is one `push %rax`. Not a frame - a frame would have to be torn down by a matching
 * `leave` in a file that cannot see it - but eight bytes of stack, so that %rsp is 16-byte aligned
 * inside the body for the reason the System V x86-64 ABI gives: %rsp is 8 past a 16-byte boundary at
 * function entry, because of the return address the call pushed. crtn.o's `pop %rax` undoes it. The
 * register is %rax because it is call-clobbered and its value on entry to `_init` means nothing.
 *
 * Measured from `objdump -s -j .init -j .fini $OO_PS4_TOOLCHAIN/lib/crti.o`: one byte in each,
 * `50`, which is `push %rax`. `objdump -t` shows the two symbols and nothing else.
 *
 * ------------------------------------------------------------------ ⚠ NOTHING IN THIS TOOLCHAIN
 *                                                                      LINKS THIS FILE
 *
 * It is here so that the SDK's `lib/` can be replaced whole, and for no other reason. Three
 * independent facts say it is unused, and any one of them is sufficient:
 *
 *   * No link recipe names it. The SDK's own `samples/hello_world/Makefile` ends its LDFLAGS with
 *     `$(TOOLCHAIN)/lib/crt1.o` and its PRX counterpart with `crtlib.o`; neither mentions crti.o or
 *     crtn.o, and neither does cmake/ps4-openorbis.cmake.
 *   * cmake/orbis-tls.ld has no `.init` or `.fini` output section, so both would be placed as orphan
 *     sections - the failure mode whose ⚠ block in that file cost a console launch.
 *   * This libc does not use the mechanism at all. `$OO_PS4_TOOLCHAIN/lib/libc.a`, member
 *     `__libc_start_main.lo`, defines `_init` and `_fini` as WEAK three-byte and one-byte stubs and
 *     CALLS `_init()` directly from `libc_start_init`. There is nothing for a `.init` section to be.
 *
 * So: assembled, compared against the original, and shipped - but if it is ever linked, that is new
 * behaviour and nobody here has seen it run.
 */

	.section .init,"ax",@progbits
	.globl _init
_init:
	pushq	%rax

	.section .fini,"ax",@progbits
	.globl _fini
_fini:
	pushq	%rax

	.section .note.GNU-stack,"",@progbits
