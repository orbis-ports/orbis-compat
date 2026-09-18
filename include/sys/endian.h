/* SPDX-License-Identifier: MIT */
#ifndef _ORBIS_SYS_ENDIAN_H
#define _ORBIS_SYS_ENDIAN_H

/*
 * <sys/endian.h> - the BSD spelling, which this target asks for and the SDK does not have.
 *
 * ⚠ WHY ANYTHING NEEDS THIS. clang invoked as --target=x86_64-pc-freebsd12-elf predefines
 * __FreeBSD__ 12 (measured: `clang --target=... -dM -E -` prints `#define __FreeBSD__ 12`), so every
 * portable library that has a FreeBSD arm takes it and includes <sys/endian.h>. The SDK ships the
 * glibc spelling, <endian.h>, and nothing else - so the arm that was chosen BECAUSE the target says
 * FreeBSD then fails with "file not found".
 *
 * Two things have already paid for that hole:
 *
 *   * SDL2. orbis-ports/SDL carries one commit, d2f6ea6ff "Skip sys/endian.h on the PlayStation 4",
 *     which adds && !defined(__ORBIS__) to SDL_endian.h's FreeBSD arm. ⚠ AND THAT PATCH IS WEAKER
 *     THAN IT LOOKS: __ORBIS__ is defined by the kit's toolchain file, not by the SDK or by clang,
 *     so anyone compiling SDL with the SDK and their own build system still hits the same error.
 *   * Mesa. u_endian.h fell past its <endian.h> arm into the FreeBSD one and reached for a
 *     <machine/endian.h> nothing ships.
 *
 * With this header both stop being special cases: the arm the target selects simply works.
 *
 * ------------------------------------------------------------------ what is here, and what is not
 *
 * The SDK's <endian.h> already defines BYTE_ORDER, LITTLE_ENDIAN, BIG_ENDIAN, PDP_ENDIAN and the
 * whole htobeNN, beNNtoh, htoleNN and leNNtoh family, so this includes it rather than restating any
 * of it - two definitions of one byte order is how a port ends up with one of them wrong.
 *
 * What FreeBSD adds on top, and this supplies:
 *
 *   bswap16/32/64   the same swaps musl declares with two leading underscores
 *   be16enc/dec     unaligned load/store helpers. FreeBSD documents them as working on unaligned
 *   le16enc/dec     addresses, and these do: they go through unsigned char, so they are correct
 *   (and 32/64)     for any alignment and for any aliasing, which a uint32_t* cast is not.
 */

#include <stdint.h>
#include <endian.h>

/* FreeBSD's spelling of the swaps musl already provides as __bswapNN. */
#define bswap16(x) __bswap16(x)
#define bswap32(x) __bswap32(x)
#define bswap64(x) __bswap64(x)

static __inline uint16_t be16dec(const void *_p)
{
	const unsigned char *_c = (const unsigned char *)_p;
	return (uint16_t)((uint16_t)_c[0] << 8 | (uint16_t)_c[1]);
}

static __inline uint32_t be32dec(const void *_p)
{
	const unsigned char *_c = (const unsigned char *)_p;
	return (uint32_t)_c[0] << 24 | (uint32_t)_c[1] << 16 |
	       (uint32_t)_c[2] << 8  | (uint32_t)_c[3];
}

static __inline uint64_t be64dec(const void *_p)
{
	const unsigned char *_c = (const unsigned char *)_p;
	return (uint64_t)be32dec(_c) << 32 | be32dec(_c + 4);
}

static __inline uint16_t le16dec(const void *_p)
{
	const unsigned char *_c = (const unsigned char *)_p;
	return (uint16_t)((uint16_t)_c[1] << 8 | (uint16_t)_c[0]);
}

static __inline uint32_t le32dec(const void *_p)
{
	const unsigned char *_c = (const unsigned char *)_p;
	return (uint32_t)_c[3] << 24 | (uint32_t)_c[2] << 16 |
	       (uint32_t)_c[1] << 8  | (uint32_t)_c[0];
}

static __inline uint64_t le64dec(const void *_p)
{
	const unsigned char *_c = (const unsigned char *)_p;
	return (uint64_t)le32dec(_c + 4) << 32 | le32dec(_c);
}

static __inline void be16enc(void *_p, uint16_t _v)
{
	unsigned char *_c = (unsigned char *)_p;
	_c[0] = (unsigned char)((_v >> 8) & 0xff);
	_c[1] = (unsigned char)(_v & 0xff);
}

static __inline void be32enc(void *_p, uint32_t _v)
{
	unsigned char *_c = (unsigned char *)_p;
	_c[0] = (unsigned char)((_v >> 24) & 0xff);
	_c[1] = (unsigned char)((_v >> 16) & 0xff);
	_c[2] = (unsigned char)((_v >> 8) & 0xff);
	_c[3] = (unsigned char)(_v & 0xff);
}

static __inline void be64enc(void *_p, uint64_t _v)
{
	unsigned char *_c = (unsigned char *)_p;
	be32enc(_c, (uint32_t)(_v >> 32));
	be32enc(_c + 4, (uint32_t)(_v & 0xffffffffU));
}

static __inline void le16enc(void *_p, uint16_t _v)
{
	unsigned char *_c = (unsigned char *)_p;
	_c[0] = (unsigned char)(_v & 0xff);
	_c[1] = (unsigned char)((_v >> 8) & 0xff);
}

static __inline void le32enc(void *_p, uint32_t _v)
{
	unsigned char *_c = (unsigned char *)_p;
	_c[0] = (unsigned char)(_v & 0xff);
	_c[1] = (unsigned char)((_v >> 8) & 0xff);
	_c[2] = (unsigned char)((_v >> 16) & 0xff);
	_c[3] = (unsigned char)((_v >> 24) & 0xff);
}

static __inline void le64enc(void *_p, uint64_t _v)
{
	unsigned char *_c = (unsigned char *)_p;
	le32enc(_c, (uint32_t)(_v & 0xffffffffU));
	le32enc(_c + 4, (uint32_t)(_v >> 32));
}

#endif /* _ORBIS_SYS_ENDIAN_H */
