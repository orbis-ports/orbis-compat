// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
//
// An unlimited, on-demand libc heap - FOR A CONSUMER THAT ASKS FOR IT, AND ONLY THAT ONE.
//
// ⚠ THIS FILE IS NOT IN liborbis-compat.a AND MUST NOT BE. Everything in src/ is linked into every
// consumer with --whole-archive; this is a POLICY, not a correction, and one that competes for the
// same memory the driver wants. A consumer adds this source to its own target deliberately.
//
// ------------------------------------------------------------------ why it exists
//
// libSceLibcInternal takes its heap size from a symbol the APPLICATION defines. dEQP is a 107 MB
// binary whose static initialisers build the whole test hierarchy BEFORE main is entered, and it
// exhausts the default before the first line of main runs. What that looked like:
//
//     ERROR: Unable to create output XML writer to file '/data/deqp-results.qpa'
//
// which reads like a file problem and is not one - the file had been opened successfully a line
// earlier. qpXmlWriter_createFileWriter does nothing but deCalloc a small struct, so what failed
// was a TINY ALLOCATION. The heap was already gone.
//
// SIZE_MAX with extended allocation is the SDK's own idiom for "grow on demand", not a number
// picked to be large.
//
// ⚠ AND IT COMPETES WITH THE DRIVER. RADV's arena is direct memory taken in one lump at device
// creation; a libc heap that has already grown into that memory makes the arena's ladder fall to a
// smaller rung, or fail. If the driver reports a small arena in a binary that links this file, this
// is the first place to look. OpenGothic deliberately does NOT link it.
//
// ⚠ AND THEY HAVE TO BE EXPORTED, WHICH THE FIRST ATTEMPT WAS NOT. Defining them was not enough:
// the build compiles with -fvisibility=hidden, so both landed as LOCAL symbols - nm showed a
// lowercase 'd' - and libSceLibcInternal looks them up BY NAME at load time and found nothing. The
// run failed identically, which is the worst kind of no-change: the fix was right and invisible.
#include <stddef.h>
#include <stdint.h>

__attribute__((visibility("default"))) size_t       sceLibcHeapSize          = SIZE_MAX;
__attribute__((visibility("default"))) unsigned int sceLibcHeapExtendedAlloc = 1;
