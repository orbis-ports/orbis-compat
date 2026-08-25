# The linker script every PS4 executable this overlay supports is linked with.
#
# The OpenOrbis SDK's own link.x with ONE correction, in the .tls rule below. It lives here rather
# than beside the SDK for the same reason bits/alltypes.h and errno.h do: the SDK is upstream and this
# is a correction to it, and a consumer that finds the overlay must not also have to know about this.
#
# ⚠ IT IS THE SECOND NON-PAGE-ALIGNED-SEGMENT BUG THIS OVERLAY CARRIES A FIX FOR. The first is
# -Wl,--no-rosegment in ps4-openorbis.cmake, whose comment describes the same shape from a different
# cause: ld.lld emits a segment the PS4 loader will not accept, and the loader's diagnosis names the
# segment rather than the reason. Anyone reading either fix should know the other exists.
#
# SPDX-License-Identifier: MIT
SECTIONS
{
	# Combine text, rodata, and eh_frame stuff
	# In case you'll ever need to get the base address of eboot.bin, you can use the &__text_start symbol.
	# No need to go through libkernel for that.
	.text : ALIGN(0x4000) {
		__text_start = .;
		# "/libexec/ld-elf.so.1"
		QUAD(0x6365786562696C2F);
		QUAD(0x2E666C652D646C2F);
		QUAD(0x00000000312E6F73);
		QUAD(0x0000000000000000);

		# original .text
		*(.text .text.*)
	}

	.rodata : ALIGN(0x10) {
		*(.rodata .rodata.*)
	}

    # Since we lack a proper POSIX dladdr() on a PS4, we define these global symbols
    # For libunwind's bare metal compilation mode.
    # That's way better than going through libkernel... ;-;
	.eh_frame : {
	   __eh_frame_start = .;
	   KEEP(*(.eh_frame))
	   __eh_frame_end = .;
	}

	.eh_frame_hdr : {
	   __eh_frame_hdr_start = .;
	   KEEP(*(.eh_frame_hdr))
	   __eh_frame_hdr_end = .;
	}

	.data.rel.ro : ALIGN(0x4000) {
	   __data_relro_start = .;	
	   KEEP(*(.data.rel.ro .data.rel.ro.*))
	   __data_relro_start = .;
	}
	
	.init_array : {
		*(.init_array);
	}
	
	.dynamic : {
		*(.dynamic);
	}

	# ⚠ .tdata.* AND .tbss.* ADDED - THE SDK's COPY MATCHES ONLY THE BARE NAMES, AND THAT IS A BUG THAT
	# COSTS A CONSOLE LAUNCH. Anything built with -fdata-sections (Mesa is, and so is most modern C)
	# puts a thread-local into its own section: `__thread void *_mesa_glapi_tls_Dispatch` becomes
	# .tdata._mesa_glapi_tls_Dispatch, which matches NO rule in this script. lld then places it as an
	# orphan, by its own heuristic, and it chose to put it AHEAD of .data.rel.ro - so the
	# `.data.rel.ro : ALIGN(0x4000)` below, whose whole job is to start the RW segment on a page
	# boundary, no longer starts anything. The second PT_LOAD came out at vaddr 0x1c3d858 and the
	# console's loader refused the binary outright:
	#
	#     [rtld] ERROR scan_phdr:697: segment #1 of "/app0/eboot.bin" is not page aligned.
	#     [rtld] ERROR exec_self_imgact:992: found illegal segment header in /app0/eboot.bin.
	#     [Syscore App] processSpawn() error: 0x80aa001a
	#
	# which reaches the user as "Cannot start the application" with nothing else to go on. A Vulkan-only
	# title never hit it because it has no initialised TLS at all - .tdata is empty and the orphan does
	# not exist. The GL frontend's glapi dispatch is the first thing in this port to have one.
	#
	.tls : {
		*(.tdata .tdata.*);
		*(.tbss .tbss.*);
	}

	# Align .got to 0x4000 if .data.rel.ro doesn't exist
	.got : ALIGN(SIZEOF(.data.rel.ro) > 0 ? 8 : 0x4000) {
		*(.got)
	}

	# Align .got.plt to 0x4000 if .got doesn't exist
	.got.plt : ALIGN((SIZEOF(.got) > 0 || SIZEOF(.data.rel.ro) > 0) ? 8 : 0x4000) {
		*(.got.plt)
	}

	.data.sce_process_param : ALIGN(0x4000) {
		KEEP(*(.data.sce_process_param))
	}

	.data.sce_module_param : ALIGN(0x4000) {
		KEEP(*(.data.sce_module_param))
	}

	.data : {
		*(.data)
	}

	.bss : {
		*(.bss .bss.*)
	}

	# Force .got.plt to appear, because SPRX requires a valid .got.plt.
	/DISCARD/ : {
		QUAD(_GLOBAL_OFFSET_TABLE_)
	}
}
