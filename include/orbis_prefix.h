// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* What every translation unit needs before it can include the SDK's own headers.
 *
 * The headers under <orbis/> name size_t, uint32_t and friends without including <stddef.h> or
 * <stdint.h> themselves. Compiled alone, 26 of the SDK's 189 orbis/ headers fail for that reason;
 * in a real translation unit they work only because something earlier happened to declare the type.
 * The port has been papering over this with `-include stdlib.h` on every command line.
 *
 * ⚠ THIS IS STRICTLY BETTER THAN -include stdlib.h, AND IT WAS MEASURED, not reasoned about:
 *
 *     prefix                   orbis/ headers that fail to compile alone (of 189)
 *     none                     26
 *     -include stdlib.h        16     <- what the port passes today
 *     -include orbis_prefix.h   7
 *
 * It fixes more and injects less. `stdlib.h` drags an entire libc header into global scope in every
 * TU, which is how this port once ended up with an integer-only std::abs truncating floats at 37
 * call sites - see the thirty lines Tempest's toolchain file spends on include order. Two type
 * headers cannot do that.
 *
 * THE REMAINING SEVEN ARE SDK BUGS, not missing includes, and each is a one-line fix upstream:
 *
 *     orbis/JpegEnc.h:17       says OrbisJpgEncOutputInfo; the type is OrbisJpegEncOutputInfo
 *     orbis/SysCore.h:27       names OrbisAppInfo, which nothing defines
 *     orbis/libc.h:352         redeclares clang builtins (__sync_fetch_and_add_16 and neighbours)
 *     orbis/LibcInternal.h:432 the same redeclarations
 *     orbis/Font.h, FontFt.h, Usbd.h
 *
 * No prefix can reach those, and this header deliberately does not try: shadowing an SDK header to
 * correct a typo inside it would hide the bug instead of fixing it.
 */
#ifndef _ORBIS_PREFIX_H
#define _ORBIS_PREFIX_H

#include <stddef.h>
#include <stdint.h>

#endif /* _ORBIS_PREFIX_H */
