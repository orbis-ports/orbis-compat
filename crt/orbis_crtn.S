/* Copyright © 2026 Mikołaj Mikołajczyk
 * SPDX-License-Identifier: MIT
 *
 * crtn.o: the EPILOGUE half of the `.init`/`.fini` pair crt/orbis_crti.S opens. That file carries the
 * whole explanation, including the ⚠ that nothing in this toolchain links either of them.
 *
 * Two bytes in each section: `pop %rax` to undo the alignment push crti.o made, then `ret`. No
 * symbols at all - the labels live in crti.o, and these bytes are simply the tail of the same
 * function once the linker has concatenated the pieces.
 *
 * Measured from `objdump -s -j .init -j .fini $OO_PS4_TOOLCHAIN/lib/crtn.o`: `58c3` in each, and an
 * empty symbol table.
 */

	.section .init,"ax",@progbits
	popq	%rax
	retq

	.section .fini,"ax",@progbits
	popq	%rax
	retq

	.section .note.GNU-stack,"",@progbits
