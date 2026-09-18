// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
/* 32-bit wchar_t wide-character functions for the OpenOrbis SDK - they replace the SDK libc.a's.
 *
 * ⚠ MOVED HERE 2026-09-18 from a game's build directory (orbis-ports/sonic3air,
 * Oxygen/sonic3air/build/_ps4/orbis/). Nothing about a 16-bit wchar_t is that game's; every C++
 * port that touches std::wstring meets it, and the first one did so as a SIGSEGV before main().
 * It arrives together with the typedef correction in include/bits/alltypes.h - the header and
 * these definitions are one change and separating them leaves C and C++ disagreeing again.
 *
 * ⚠ WHY THIS FILE EXISTS. The SDK's libc.a (musl) was compiled with a 16-bit wchar_t: its C headers
 * say `typedef unsigned short wchar_t` (include/bits/alltypes.h, C only) and every wide function in
 * the archive walks 2-byte elements - wmemcpy copies with `movw (%r8,%rdx,2)`, wcslen tests
 * `cmpw $0,2(%rdi,%rax)`, mbrtowc stores `movw %ax,(%r14)`. But C++ has wchar_t as a keyword, and
 * clang's x86_64-pc-freebsd12-elf target makes it 32 bits - and so does the SDK's prebuilt
 * libc++.a. So every std::wstring the engine builds (paths are std::wstring throughout) goes through
 * a libc that reads half of each character. The first victim was librmx's FileIO.cpp static
 * initializer in the Sonic 3 A.I.R. port: SIGSEGV in wmemcpy before main(), with klog as the only
 * witness.
 *
 * ⚠ THE FIX IS DEFINITION ORDER, as in orbis_abort_report.c: this file is an OBJECT on the link line,
 * so every symbol below is defined before the linker looks at -lc and the libc.a members that
 * define these names are never extracted. Each libc.a member is replaced WHOLE - every global it
 * defines is defined here - so no member can be pulled in for a sibling symbol and bring its own
 * 16-bit copy along. (--allow-multiple-definition, which the Mesa link needs, would otherwise pick
 * the first definition silently instead of failing.)
 *
 * ⚠ AND IT CHANGES WHAT libc's OWN 16-bit CALLERS SEE. A libc.a member that is NOT replaced and calls
 * mbtowc(&wc, ...) with a 2-byte `wc` on its stack would now get 4 bytes written there. regcomp,
 * regexec (Mesa's driconf uses them) and vfscanf do exactly that; CMakeLists.txt links renamed copies
 * of those three members that call the SDK's original 16-bit mbtowc/mbrtowc under another name, and
 * fails the build if any other such member (fnmatch, getopt, iconv, the wide stdio family) ends up
 * in the executable.
 *
 * ⚠ C CODE IN THIS PORT STILL SEES A 16-bit wchar_t through the SDK headers. That is why this file
 * never includes <wchar.h>, <stdlib.h>, <uchar.h> or <inttypes.h> (their prototypes would disagree
 * with the definitions) and spells the type w32_t. SDL is the only C consumer of wide functions in
 * the link, and SDL_config_orbis.h keeps it on its own implementations for that reason.
 *
 * Semantics: always UTF-8 (musl's "C.UTF-8"), whatever setlocale says; MB_CUR_MAX is 4.
 * The code is musl 1.2.5 (src/multibyte, src/string, src/ctype, src/locale) adapted to that, except
 * wcsto*, vswprintf/swprintf and wcsftime, which musl builds on its internal FILE machinery and
 * which are re-implemented here on top of the narrow strto*, snprintf and strftime.
 *
 * Not supported by vswprintf: positional arguments (%1$d) - it fails with EINVAL. Nothing in libc++,
 * the engine or SDL uses them.
 *
 * Host test: -DORBIS_WCHAR32_TEST renames every public function to t_<name> (see W32() below), so
 * the file can be compiled for Linux and compared against glibc.
 *
 * ----------------------------------------------------------------------------------------------
 * Derived from musl libc (https://musl.libc.org), which carries this license:
 *
 * Copyright © 2005-2020 Rich Felker, et al.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ----------------------------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 */
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <wctype.h>

#ifdef ORBIS_WCHAR32_TEST
#define W32(name) t_##name
#else
#define W32(name) name
#endif

typedef __WCHAR_TYPE__ w32_t;                                        /* int: what C++ and libc++ use */
typedef unsigned w32_wint_t;
typedef struct { unsigned __opaque1, __opaque2; } w32_mbstate_t;     /* musl's layout, 8 bytes */
typedef locale_t w32_locale_t;

_Static_assert(sizeof(w32_t) == 4, "orbis_wchar32.c needs the compiler's wchar_t to be 32-bit");
_Static_assert(sizeof(w32_mbstate_t) == 8, "mbstate_t layout");

#define W32_WEOF       0xffffffffU
#define W32_MB_LEN_MAX 4

/* From <stdlib.h>, which cannot be included here (see above). */
void *malloc(size_t);
void free(void *);
long strtol(const char *, char **, int);
long long strtoll(const char *, char **, int);
unsigned long strtoul(const char *, char **, int);
unsigned long long strtoull(const char *, char **, int);
double strtod(const char *, char **);
float strtof(const char *, char **);
long double strtold(const char *, char **);

/* ---------------------------------------------------------------- prototypes */

size_t W32(__ctype_get_mb_cur_max)(void);
w32_wint_t W32(btowc)(int);
int W32(wctob)(w32_wint_t);
size_t W32(mbrtowc)(w32_t *restrict, const char *restrict, size_t, w32_mbstate_t *restrict);
size_t W32(mbrlen)(const char *restrict, size_t, w32_mbstate_t *restrict);
int W32(mbtowc)(w32_t *restrict, const char *restrict, size_t);
int W32(mblen)(const char *, size_t);
int W32(mbsinit)(const w32_mbstate_t *);
size_t W32(mbsrtowcs)(w32_t *restrict, const char **restrict, size_t, w32_mbstate_t *restrict);
size_t W32(mbsnrtowcs)(w32_t *restrict, const char **restrict, size_t, size_t, w32_mbstate_t *restrict);
size_t W32(mbstowcs)(w32_t *restrict, const char *restrict, size_t);
size_t W32(wcrtomb)(char *restrict, w32_t, w32_mbstate_t *restrict);
int W32(wctomb)(char *, w32_t);
size_t W32(wcsrtombs)(char *restrict, const w32_t **restrict, size_t, w32_mbstate_t *restrict);
size_t W32(wcsnrtombs)(char *restrict, const w32_t **restrict, size_t, size_t, w32_mbstate_t *restrict);
size_t W32(wcstombs)(char *restrict, const w32_t *restrict, size_t);
size_t W32(c16rtomb)(char *restrict, uint16_t, w32_mbstate_t *restrict);
size_t W32(c32rtomb)(char *restrict, uint32_t, w32_mbstate_t *restrict);
size_t W32(mbrtoc16)(uint16_t *restrict, const char *restrict, size_t, w32_mbstate_t *restrict);
size_t W32(mbrtoc32)(uint32_t *restrict, const char *restrict, size_t, w32_mbstate_t *restrict);

w32_t *W32(wmemchr)(const w32_t *, w32_t, size_t);
int W32(wmemcmp)(const w32_t *, const w32_t *, size_t);
w32_t *W32(wmemcpy)(w32_t *restrict, const w32_t *restrict, size_t);
w32_t *W32(wmemmove)(w32_t *, const w32_t *, size_t);
w32_t *W32(wmemset)(w32_t *, w32_t, size_t);
size_t W32(wcslen)(const w32_t *);
size_t W32(wcsnlen)(const w32_t *, size_t);
w32_t *W32(wcscpy)(w32_t *restrict, const w32_t *restrict);
w32_t *W32(wcsncpy)(w32_t *restrict, const w32_t *restrict, size_t);
w32_t *W32(wcpcpy)(w32_t *restrict, const w32_t *restrict);
w32_t *W32(wcpncpy)(w32_t *restrict, const w32_t *restrict, size_t);
w32_t *W32(wcscat)(w32_t *restrict, const w32_t *restrict);
w32_t *W32(wcsncat)(w32_t *restrict, const w32_t *restrict, size_t);
int W32(wcscmp)(const w32_t *, const w32_t *);
int W32(wcsncmp)(const w32_t *, const w32_t *, size_t);
int W32(wcscasecmp)(const w32_t *, const w32_t *);
int W32(wcsncasecmp)(const w32_t *, const w32_t *, size_t);
int W32(wcscasecmp_l)(const w32_t *, const w32_t *, w32_locale_t);
int W32(wcsncasecmp_l)(const w32_t *, const w32_t *, size_t, w32_locale_t);
w32_t *W32(wcschr)(const w32_t *, w32_t);
w32_t *W32(wcsrchr)(const w32_t *, w32_t);
size_t W32(wcsspn)(const w32_t *, const w32_t *);
size_t W32(wcscspn)(const w32_t *, const w32_t *);
w32_t *W32(wcspbrk)(const w32_t *, const w32_t *);
w32_t *W32(wcsstr)(const w32_t *restrict, const w32_t *restrict);
w32_t *W32(wcswcs)(const w32_t *, const w32_t *);
w32_t *W32(wcstok)(w32_t *restrict, const w32_t *restrict, w32_t **restrict);
w32_t *W32(wcsdup)(const w32_t *);
int W32(wcscoll)(const w32_t *, const w32_t *);
int W32(__wcscoll_l)(const w32_t *, const w32_t *, w32_locale_t);
int W32(wcscoll_l)(const w32_t *, const w32_t *, w32_locale_t);
size_t W32(wcsxfrm)(w32_t *restrict, const w32_t *restrict, size_t);
size_t W32(__wcsxfrm_l)(w32_t *restrict, const w32_t *restrict, size_t, w32_locale_t);
size_t W32(wcsxfrm_l)(w32_t *restrict, const w32_t *restrict, size_t, w32_locale_t);

int W32(iswspace)(w32_wint_t);
int W32(__iswspace_l)(w32_wint_t, w32_locale_t);
int W32(iswspace_l)(w32_wint_t, w32_locale_t);
int W32(wcwidth)(w32_t);
int W32(wcswidth)(const w32_t *, size_t);

long W32(wcstol)(const w32_t *restrict, w32_t **restrict, int);
long long W32(wcstoll)(const w32_t *restrict, w32_t **restrict, int);
unsigned long W32(wcstoul)(const w32_t *restrict, w32_t **restrict, int);
unsigned long long W32(wcstoull)(const w32_t *restrict, w32_t **restrict, int);
intmax_t W32(wcstoimax)(const w32_t *restrict, w32_t **restrict, int);
uintmax_t W32(wcstoumax)(const w32_t *restrict, w32_t **restrict, int);
float W32(wcstof)(const w32_t *restrict, w32_t **restrict);
double W32(wcstod)(const w32_t *restrict, w32_t **restrict);
long double W32(wcstold)(const w32_t *restrict, w32_t **restrict);

int W32(vswprintf)(w32_t *restrict, size_t, const w32_t *restrict, va_list);
int W32(swprintf)(w32_t *restrict, size_t, const w32_t *restrict, ...);

size_t W32(wcsftime)(w32_t *restrict, size_t, const w32_t *restrict, const struct tm *restrict);
size_t W32(__wcsftime_l)(w32_t *restrict, size_t, const w32_t *restrict, const struct tm *restrict, w32_locale_t);
size_t W32(wcsftime_l)(w32_t *restrict, size_t, const w32_t *restrict, const struct tm *restrict, w32_locale_t);


/* ---------------------------------------------------------------- UTF-8 state tables (musl src/multibyte/internal.[ch]) */

/* Upper 6 state bits are a negative integer offset to bound-check next byte */
/*    equivalent to: ( (b-0x80) | (b+offset) ) & ~0x3f      */
#define OOB(c,b) (((((b)>>3)-0x10)|(((b)>>3)+((int32_t)(c)>>26))) & ~7)

/* Interval [a,b). Either a must be 80 or b must be c0, lower 3 bits clear. */
#define R(a,b) ((uint32_t)((a==0x80 ? 0x40u-b : 0u-a) << 23))

#define SA 0xc2u
#define SB 0xf4u

#define C(x) ( x<2 ? -1 : ( R(0x80,0xc0) | x ) )
#define D(x) C((x+16))
#define E(x) ( ( x==0 ? R(0xa0,0xc0) : \
                 x==0xd ? R(0x80,0xa0) : \
                 R(0x80,0xc0) ) \
             | ( R(0x80,0xc0) >> 6 ) \
             | x )
#define F(x) ( ( x>=5 ? 0 : \
                 x==0 ? R(0x90,0xc0) : \
                 x==4 ? R(0x80,0x90) : \
                 R(0x80,0xc0) ) \
             | ( R(0x80,0xc0) >> 6 ) \
             | ( R(0x80,0xc0) >> 12 ) \
             | x )

static const uint32_t bittab[] = {
	              C(0x2),C(0x3),C(0x4),C(0x5),C(0x6),C(0x7),
	C(0x8),C(0x9),C(0xa),C(0xb),C(0xc),C(0xd),C(0xe),C(0xf),
	D(0x0),D(0x1),D(0x2),D(0x3),D(0x4),D(0x5),D(0x6),D(0x7),
	D(0x8),D(0x9),D(0xa),D(0xb),D(0xc),D(0xd),D(0xe),D(0xf),
	E(0x0),E(0x1),E(0x2),E(0x3),E(0x4),E(0x5),E(0x6),E(0x7),
	E(0x8),E(0x9),E(0xa),E(0xb),E(0xc),E(0xd),E(0xe),E(0xf),
	F(0x0),F(0x1),F(0x2),F(0x3),F(0x4)
};

#undef C
#undef D
#undef E
#undef F


/* ---------------------------------------------------------------- multibyte (musl src/multibyte, UTF-8 only) */

size_t W32(__ctype_get_mb_cur_max)(void)
{
	return 4;
}

w32_wint_t W32(btowc)(int c)
{
	int b = (unsigned char)c;
	return (unsigned)b<128U ? (w32_wint_t)b : W32_WEOF;
}

int W32(wctob)(w32_wint_t c)
{
	if (c < 128U) return c;
	return EOF;
}

size_t W32(mbrtowc)(w32_t *restrict wc, const char *restrict src, size_t n, w32_mbstate_t *restrict st)
{
	static unsigned internal_state;
	unsigned c;
	const unsigned char *s = (const void *)src;
	const size_t N = n;
	w32_t dummy;

	if (!st) st = (void *)&internal_state;
	c = *(unsigned *)st;

	if (!s) {
		if (c) goto ilseq;
		return 0;
	} else if (!wc) wc = &dummy;

	if (!n) return -2;
	if (!c) {
		if (*s < 0x80) return !!(*wc = *s);
		if (*s-SA > SB-SA) goto ilseq;
		c = bittab[*s++-SA]; n--;
	}

	if (n) {
		if (OOB(c,*s)) goto ilseq;
loop:
		c = c<<6 | (*s++-0x80); n--;
		if (!(c&(1U<<31))) {
			*(unsigned *)st = 0;
			*wc = c;
			return N-n;
		}
		if (n) {
			if (*s-0x80u >= 0x40) goto ilseq;
			goto loop;
		}
	}

	*(unsigned *)st = c;
	return -2;
ilseq:
	*(unsigned *)st = 0;
	errno = EILSEQ;
	return -1;
}

size_t W32(mbrlen)(const char *restrict s, size_t n, w32_mbstate_t *restrict st)
{
	static unsigned internal;
	return W32(mbrtowc)(0, s, n, st ? st : (w32_mbstate_t *)(void *)&internal);
}

int W32(mbtowc)(w32_t *restrict wc, const char *restrict src, size_t n)
{
	unsigned c;
	const unsigned char *s = (const void *)src;
	w32_t dummy;

	if (!s) return 0;
	if (!n) goto ilseq;
	if (!wc) wc = &dummy;

	if (*s < 0x80) return !!(*wc = *s);
	if (*s-SA > SB-SA) goto ilseq;
	c = bittab[*s++-SA];

	/* Avoid excessive checks against n: If shifting the state n-1
	 * times does not clear the high bit, then the value of n is
	 * insufficient to read a character */
	if (n<4 && ((c<<(6*n-6)) & (1U<<31))) goto ilseq;

	if (OOB(c,*s)) goto ilseq;
	c = c<<6 | (*s++-0x80);
	if (!(c&(1U<<31))) {
		*wc = c;
		return 2;
	}

	if (*s-0x80u >= 0x40) goto ilseq;
	c = c<<6 | (*s++-0x80);
	if (!(c&(1U<<31))) {
		*wc = c;
		return 3;
	}

	if (*s-0x80u >= 0x40) goto ilseq;
	*wc = c<<6 | (*s++-0x80);
	return 4;

ilseq:
	errno = EILSEQ;
	return -1;
}

int W32(mblen)(const char *s, size_t n)
{
	return W32(mbtowc)(0, s, n);
}

int W32(mbsinit)(const w32_mbstate_t *st)
{
	return !st || !*(unsigned *)st;
}

size_t W32(mbsrtowcs)(w32_t *restrict ws, const char **restrict src, size_t wn, w32_mbstate_t *restrict st)
{
	const unsigned char *s = (const void *)*src;
	size_t wn0 = wn;
	unsigned c = 0;

	if (st && (c = *(unsigned *)st)) {
		if (ws) {
			*(unsigned *)st = 0;
			goto resume;
		} else {
			goto resume0;
		}
	}

	/* (musl's word-at-a-time ASCII fast paths are left out) */
	if (!ws) for (;;) {
		if (*s-1u < 0x7f) {
			s++;
			wn--;
			continue;
		}
		if (*s-SA > SB-SA) break;
		c = bittab[*s++-SA];
resume0:
		if (OOB(c,*s)) { s--; break; }
		s++;
		if (c&(1U<<25)) {
			if (*s-0x80u >= 0x40) { s-=2; break; }
			s++;
			if (c&(1U<<19)) {
				if (*s-0x80u >= 0x40) { s-=3; break; }
				s++;
			}
		}
		wn--;
		c = 0;
	} else for (;;) {
		if (!wn) {
			*src = (const void *)s;
			return wn0;
		}
		if (*s-1u < 0x7f) {
			*ws++ = *s++;
			wn--;
			continue;
		}
		if (*s-SA > SB-SA) break;
		c = bittab[*s++-SA];
resume:
		if (OOB(c,*s)) { s--; break; }
		c = (c<<6) | (*s++-0x80);
		if (c&(1U<<31)) {
			if (*s-0x80u >= 0x40) { s-=2; break; }
			c = (c<<6) | (*s++-0x80);
			if (c&(1U<<31)) {
				if (*s-0x80u >= 0x40) { s-=3; break; }
				c = (c<<6) | (*s++-0x80);
			}
		}
		*ws++ = c;
		wn--;
		c = 0;
	}

	if (!c && !*s) {
		if (ws) {
			*ws = 0;
			*src = 0;
		}
		return wn0-wn;
	}
	errno = EILSEQ;
	if (ws) *src = (const void *)s;
	return -1;
}

size_t W32(mbsnrtowcs)(w32_t *restrict wcs, const char **restrict src, size_t n, size_t wn, w32_mbstate_t *restrict st)
{
	size_t l, cnt=0, n2;
	w32_t *ws, wbuf[256];
	const char *s = *src;
	const char *tmp_s;

	if (!wcs) ws = wbuf, wn = sizeof wbuf / sizeof *wbuf;
	else ws = wcs;

	/* making sure output buffer size is at most n/4 will ensure
	 * that mbsrtowcs never reads more than n input bytes. thus
	 * we can use mbsrtowcs as long as it's practical.. */

	while ( s && wn && ( (n2=n/4)>=wn || n2>32 ) ) {
		if (n2>=wn) n2=wn;
		tmp_s = s;
		l = W32(mbsrtowcs)(ws, &s, n2, st);
		if (!(l+1)) {
			cnt = l;
			wn = 0;
			break;
		}
		if (ws != wbuf) {
			ws += l;
			wn -= l;
		}
		n = s ? n - (s - tmp_s) : 0;
		cnt += l;
	}
	if (s) while (wn && n) {
		l = W32(mbrtowc)(ws, s, n, st);
		if (l+2<=2) {
			if (!(l+1)) {
				cnt = l;
				break;
			}
			if (!l) {
				s = 0;
				break;
			}
			/* have to roll back partial character */
			*(unsigned *)st = 0;
			break;
		}
		s += l; n -= l;
		/* safe - this loop runs fewer than sizeof(wbuf)/8 times */
		ws++; wn--;
		cnt++;
	}
	if (wcs) *src = s;
	return cnt;
}

size_t W32(mbstowcs)(w32_t *restrict ws, const char *restrict s, size_t wn)
{
	return W32(mbsrtowcs)(ws, (void*)&s, wn, 0);
}

size_t W32(wcrtomb)(char *restrict s, w32_t wc, w32_mbstate_t *restrict st)
{
	(void)st;
	if (!s) return 1;
	if ((unsigned)wc < 0x80) {
		*s = wc;
		return 1;
	} else if ((unsigned)wc < 0x800) {
		*s++ = 0xc0 | (wc>>6);
		*s = 0x80 | (wc&0x3f);
		return 2;
	} else if ((unsigned)wc < 0xd800 || (unsigned)wc-0xe000 < 0x2000) {
		*s++ = 0xe0 | (wc>>12);
		*s++ = 0x80 | ((wc>>6)&0x3f);
		*s = 0x80 | (wc&0x3f);
		return 3;
	} else if ((unsigned)wc-0x10000 < 0x100000) {
		*s++ = 0xf0 | (wc>>18);
		*s++ = 0x80 | ((wc>>12)&0x3f);
		*s++ = 0x80 | ((wc>>6)&0x3f);
		*s = 0x80 | (wc&0x3f);
		return 4;
	}
	errno = EILSEQ;
	return -1;
}

int W32(wctomb)(char *s, w32_t wc)
{
	if (!s) return 0;
	return W32(wcrtomb)(s, wc, 0);
}

size_t W32(wcsrtombs)(char *restrict s, const w32_t **restrict ws, size_t n, w32_mbstate_t *restrict st)
{
	const w32_t *ws2;
	char buf[4];
	size_t N = n, l;
	(void)st;
	if (!s) {
		for (n=0, ws2=*ws; *ws2; ws2++) {
			if ((unsigned)*ws2 >= 0x80u) {
				l = W32(wcrtomb)(buf, *ws2, 0);
				if (!(l+1)) return -1;
				n += l;
			} else n++;
		}
		return n;
	}
	while (n>=4) {
		if ((unsigned)**ws-1u >= 0x7fu) {
			if (!**ws) {
				*s = 0;
				*ws = 0;
				return N-n;
			}
			l = W32(wcrtomb)(s, **ws, 0);
			if (!(l+1)) return -1;
			s += l;
			n -= l;
		} else {
			*s++ = **ws;
			n--;
		}
		(*ws)++;
	}
	while (n) {
		if ((unsigned)**ws-1u >= 0x7fu) {
			if (!**ws) {
				*s = 0;
				*ws = 0;
				return N-n;
			}
			l = W32(wcrtomb)(buf, **ws, 0);
			if (!(l+1)) return -1;
			if (l>n) return N-n;
			W32(wcrtomb)(s, **ws, 0);
			s += l;
			n -= l;
		} else {
			*s++ = **ws;
			n--;
		}
		(*ws)++;
	}
	return N;
}

size_t W32(wcsnrtombs)(char *restrict dst, const w32_t **restrict wcs, size_t wn, size_t n, w32_mbstate_t *restrict st)
{
	const w32_t *ws = *wcs;
	size_t cnt = 0;
	(void)st;
	if (!dst) n=0;
	while (ws && wn) {
		char tmp[W32_MB_LEN_MAX];
		size_t l = W32(wcrtomb)(n<W32_MB_LEN_MAX ? tmp : dst, *ws, 0);
		if (l==(size_t)-1) {
			cnt = -1;
			break;
		}
		if (dst) {
			if (n<W32_MB_LEN_MAX) {
				if (l>n) break;
				memcpy(dst, tmp, l);
			}
			dst += l;
			n -= l;
		}
		if (!*ws) {
			ws = 0;
			break;
		}
		ws++;
		wn--;
		cnt += l;
	}
	if (dst) *wcs = ws;
	return cnt;
}

size_t W32(wcstombs)(char *restrict s, const w32_t *restrict ws, size_t n)
{
	const w32_t *p = ws;
	return W32(wcsrtombs)(s, &p, n, 0);
}

size_t W32(c16rtomb)(char *restrict s, uint16_t c16, w32_mbstate_t *restrict ps)
{
	static unsigned internal_state;
	if (!ps) ps = (void *)&internal_state;
	unsigned *x = (unsigned *)ps;
	w32_t wc;

	if (!s) {
		if (*x) goto ilseq;
		return 1;
	}

	if (!*x && c16 - 0xd800u < 0x400) {
		*x = (c16 - 0xd7c0) << 10;
		return 0;
	}

	if (*x) {
		if (c16 - 0xdc00u >= 0x400) goto ilseq;
		else wc = *x + c16 - 0xdc00;
		*x = 0;
	} else {
		wc = c16;
	}
	return W32(wcrtomb)(s, wc, 0);

ilseq:
	*x = 0;
	errno = EILSEQ;
	return -1;
}

size_t W32(c32rtomb)(char *restrict s, uint32_t c32, w32_mbstate_t *restrict ps)
{
	return W32(wcrtomb)(s, c32, ps);
}

size_t W32(mbrtoc16)(uint16_t *restrict pc16, const char *restrict s, size_t n, w32_mbstate_t *restrict ps)
{
	static unsigned internal_state;
	if (!ps) ps = (void *)&internal_state;
	unsigned *pending = (unsigned *)ps;

	if (!s) return W32(mbrtoc16)(0, "", 1, ps);

	/* mbrtowc states for partial UTF-8 characters have the high bit set;
	 * we use nonzero states without high bit for pending surrogates. */
	if ((int)*pending > 0) {
		if (pc16) *pc16 = *pending;
		*pending = 0;
		return -3;
	}

	w32_t wc;
	size_t ret = W32(mbrtowc)(&wc, s, n, ps);
	if (ret <= 4) {
		if (wc >= 0x10000) {
			*pending = (wc & 0x3ff) + 0xdc00;
			wc = 0xd7c0 + (wc >> 10);
		}
		if (pc16) *pc16 = wc;
	}
	return ret;
}

size_t W32(mbrtoc32)(uint32_t *restrict pc32, const char *restrict s, size_t n, w32_mbstate_t *restrict ps)
{
	static unsigned internal_state;
	if (!ps) ps = (void *)&internal_state;
	if (!s) return W32(mbrtoc32)(0, "", 1, ps);
	w32_t wc;
	size_t ret = W32(mbrtowc)(&wc, s, n, ps);
	if (ret <= 4 && pc32) *pc32 = wc;
	return ret;
}


/* ---------------------------------------------------------------- string (musl src/string) */

w32_t *W32(wmemchr)(const w32_t *s, w32_t c, size_t n)
{
	for (; n && *s != c; n--, s++);
	return n ? (w32_t *)s : 0;
}

int W32(wmemcmp)(const w32_t *l, const w32_t *r, size_t n)
{
	for (; n && *l==*r; n--, l++, r++);
	return n ? (*l < *r ? -1 : *l > *r) : 0;
}

w32_t *W32(wmemcpy)(w32_t *restrict d, const w32_t *restrict s, size_t n)
{
	w32_t *a = d;
	while (n--) *d++ = *s++;
	return a;
}

w32_t *W32(wmemmove)(w32_t *d, const w32_t *s, size_t n)
{
	w32_t *d0 = d;
	if (d == s) return d;
	if ((uintptr_t)d-(uintptr_t)s < n * sizeof *d)
		while (n--) d[n] = s[n];
	else
		while (n--) *d++ = *s++;
	return d0;
}

w32_t *W32(wmemset)(w32_t *d, w32_t c, size_t n)
{
	w32_t *ret = d;
	while (n--) *d++ = c;
	return ret;
}

size_t W32(wcslen)(const w32_t *s)
{
	const w32_t *a;
	for (a=s; *s; s++);
	return s-a;
}

size_t W32(wcsnlen)(const w32_t *s, size_t n)
{
	const w32_t *z = W32(wmemchr)(s, 0, n);
	if (z) n = z-s;
	return n;
}

w32_t *W32(wcscpy)(w32_t *restrict d, const w32_t *restrict s)
{
	w32_t *a = d;
	while ((*d++ = *s++));
	return a;
}

w32_t *W32(wcsncpy)(w32_t *restrict d, const w32_t *restrict s, size_t n)
{
	w32_t *a = d;
	while (n && *s) n--, *d++ = *s++;
	W32(wmemset)(d, 0, n);
	return a;
}

w32_t *W32(wcpcpy)(w32_t *restrict d, const w32_t *restrict s)
{
	return W32(wcscpy)(d, s) + W32(wcslen)(s);
}

w32_t *W32(wcpncpy)(w32_t *restrict d, const w32_t *restrict s, size_t n)
{
	return W32(wcsncpy)(d, s, n) + W32(wcsnlen)(s, n);
}

w32_t *W32(wcscat)(w32_t *restrict dest, const w32_t *restrict src)
{
	W32(wcscpy)(dest + W32(wcslen)(dest), src);
	return dest;
}

w32_t *W32(wcsncat)(w32_t *restrict d, const w32_t *restrict s, size_t n)
{
	w32_t *a = d;
	d += W32(wcslen)(d);
	while (n && *s) n--, *d++ = *s++;
	*d++ = 0;
	return a;
}

int W32(wcscmp)(const w32_t *l, const w32_t *r)
{
	for (; *l==*r && *l && *r; l++, r++);
	return *l < *r ? -1 : *l > *r;
}

int W32(wcsncmp)(const w32_t *l, const w32_t *r, size_t n)
{
	for (; n && *l==*r && *l && *r; n--, l++, r++);
	return n ? (*l < *r ? -1 : *l > *r) : 0;
}

int W32(wcsncasecmp)(const w32_t *l, const w32_t *r, size_t n)
{
	if (!n--) return 0;
	for (; *l && *r && n && (*l == *r || towlower(*l) == towlower(*r)); l++, r++, n--);
	return towlower(*l) - towlower(*r);
}

int W32(wcscasecmp)(const w32_t *l, const w32_t *r)
{
	return W32(wcsncasecmp)(l, r, -1);
}

int W32(wcscasecmp_l)(const w32_t *l, const w32_t *r, w32_locale_t locale)
{
	(void)locale;
	return W32(wcscasecmp)(l, r);
}

int W32(wcsncasecmp_l)(const w32_t *l, const w32_t *r, size_t n, w32_locale_t locale)
{
	(void)locale;
	return W32(wcsncasecmp)(l, r, n);
}

w32_t *W32(wcschr)(const w32_t *s, w32_t c)
{
	if (!c) return (w32_t *)s + W32(wcslen)(s);
	for (; *s && *s != c; s++);
	return *s ? (w32_t *)s : 0;
}

w32_t *W32(wcsrchr)(const w32_t *s, w32_t c)
{
	const w32_t *p;
	for (p=s+W32(wcslen)(s); p>=s && *p!=c; p--);
	return p>=s ? (w32_t *)p : 0;
}

size_t W32(wcsspn)(const w32_t *s, const w32_t *c)
{
	const w32_t *a;
	for (a=s; *s && W32(wcschr)(c, *s); s++);
	return s-a;
}

size_t W32(wcscspn)(const w32_t *s, const w32_t *c)
{
	const w32_t *a;
	if (!c[0]) return W32(wcslen)(s);
	if (!c[1]) return (s=W32(wcschr)(a=s, *c)) ? (size_t)(s-a) : W32(wcslen)(a);
	for (a=s; *s && !W32(wcschr)(c, *s); s++);
	return s-a;
}

w32_t *W32(wcspbrk)(const w32_t *s, const w32_t *b)
{
	s += W32(wcscspn)(s, b);
	return *s ? (w32_t *)s : NULL;
}

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

static w32_t *twoway_wcsstr(const w32_t *h, const w32_t *n)
{
	const w32_t *z;
	size_t l, ip, jp, k, p, ms, p0, mem, mem0;

	/* Computing length of needle */
	for (l=0; n[l] && h[l]; l++);
	if (n[l]) return 0; /* hit the end of h */

	/* Compute maximal suffix */
	ip = -1; jp = 0; k = p = 1;
	while (jp+k<l) {
		if (n[ip+k] == n[jp+k]) {
			if (k == p) {
				jp += p;
				k = 1;
			} else k++;
		} else if (n[ip+k] > n[jp+k]) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else {
			ip = jp++;
			k = p = 1;
		}
	}
	ms = ip;
	p0 = p;

	/* And with the opposite comparison */
	ip = -1; jp = 0; k = p = 1;
	while (jp+k<l) {
		if (n[ip+k] == n[jp+k]) {
			if (k == p) {
				jp += p;
				k = 1;
			} else k++;
		} else if (n[ip+k] < n[jp+k]) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else {
			ip = jp++;
			k = p = 1;
		}
	}
	if (ip+1 > ms+1) ms = ip;
	else p = p0;

	/* Periodic needle? */
	if (W32(wmemcmp)(n, n+p, ms+1)) {
		mem0 = 0;
		p = MAX(ms, l-ms-1) + 1;
	} else mem0 = l-p;
	mem = 0;

	/* Initialize incremental end-of-haystack pointer */
	z = h;

	/* Search loop */
	for (;;) {
		/* Update incremental end-of-haystack pointer */
		if ((size_t)(z-h) < l) {
			/* Fast estimate for MIN(l,63) */
			size_t grow = l | 63;
			const w32_t *z2 = W32(wmemchr)(z, 0, grow);
			if (z2) {
				z = z2;
				if ((size_t)(z-h) < l) return 0;
			} else z += grow;
		}

		/* Compare right half */
		for (k=MAX(ms+1,mem); n[k] && n[k] == h[k]; k++);
		if (n[k]) {
			h += k-ms;
			mem = 0;
			continue;
		}
		/* Compare left half */
		for (k=ms+1; k>mem && n[k-1] == h[k-1]; k--);
		if (k <= mem) return (w32_t *)h;
		h += p;
		mem = mem0;
	}
}

w32_t *W32(wcsstr)(const w32_t *restrict h, const w32_t *restrict n)
{
	/* Return immediately on empty needle or haystack */
	if (!n[0]) return (w32_t *)h;
	if (!h[0]) return 0;

	/* Use faster algorithms for short needles */
	const w32_t *hh = W32(wcschr)(h, *n);
	if (!hh || !n[1]) return (w32_t *)hh;
	if (!hh[1]) return 0;

	return twoway_wcsstr(hh, n);
}

w32_t *W32(wcswcs)(const w32_t *haystack, const w32_t *needle)
{
	return W32(wcsstr)(haystack, needle);
}

w32_t *W32(wcstok)(w32_t *restrict s, const w32_t *restrict sep, w32_t **restrict p)
{
	if (!s && !(s = *p)) return NULL;
	s += W32(wcsspn)(s, sep);
	if (!*s) return *p = 0;
	*p = s + W32(wcscspn)(s, sep);
	if (**p) *(*p)++ = 0;
	else *p = 0;
	return s;
}

w32_t *W32(wcsdup)(const w32_t *s)
{
	size_t l = W32(wcslen)(s);
	w32_t *d = malloc((l+1)*sizeof(w32_t));
	if (!d) return NULL;
	return W32(wmemcpy)(d, s, l+1);
}


/* ---------------------------------------------------------------- locale (musl src/locale: code-point collation) */

int W32(__wcscoll_l)(const w32_t *l, const w32_t *r, w32_locale_t locale)
{
	(void)locale;
	return W32(wcscmp)(l, r);
}

int W32(wcscoll_l)(const w32_t *l, const w32_t *r, w32_locale_t locale)
{
	return W32(__wcscoll_l)(l, r, locale);
}

int W32(wcscoll)(const w32_t *l, const w32_t *r)
{
	return W32(__wcscoll_l)(l, r, 0);
}

size_t W32(__wcsxfrm_l)(w32_t *restrict dest, const w32_t *restrict src, size_t n, w32_locale_t loc)
{
	size_t l = W32(wcslen)(src);
	(void)loc;
	if (l < n) {
		W32(wmemcpy)(dest, src, l+1);
	} else if (n) {
		W32(wmemcpy)(dest, src, n-1);
		dest[n-1] = 0;
	}
	return l;
}

size_t W32(wcsxfrm_l)(w32_t *restrict dest, const w32_t *restrict src, size_t n, w32_locale_t loc)
{
	return W32(__wcsxfrm_l)(dest, src, n, loc);
}

size_t W32(wcsxfrm)(w32_t *restrict dest, const w32_t *restrict src, size_t n)
{
	return W32(__wcsxfrm_l)(dest, src, n, 0);
}


/* ---------------------------------------------------------------- ctype (musl src/ctype)
 *
 * Only the functions that touch wchar_t MEMORY or take a wchar_t (not wint_t) argument. The SDK's
 * isw*()/tow*()/wctype()/wctrans() take wint_t, which is 32-bit on both sides, and their code uses
 * the full value (checked in the disassembly: no 16-bit truncation, tables reach 0x2ffff) - they stay.
 * iswspace is the exception: musl implements it as wcschr() over a static wchar_t table, and in the
 * SDK that table is 16-bit. */

int W32(iswspace)(w32_wint_t wc)
{
	/* Our definition of whitespace is the Unicode White_Space property,
	 * minus non-breaking spaces (U+00A0, U+2007, and U+202F) and script-
	 * specific characters with non-blank glyphs (U+1680 and U+180E). */
	static const w32_t spaces[] = {
		' ', '\t', '\n', '\r', 11, 12,  0x0085,
		0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005,
		0x2006, 0x2008, 0x2009, 0x200a,
		0x2028, 0x2029, 0x205f, 0x3000, 0
	};
	return wc && W32(wcschr)(spaces, (w32_t)wc);
}

int W32(__iswspace_l)(w32_wint_t c, w32_locale_t l)
{
	(void)l;
	return W32(iswspace)(c);
}

int W32(iswspace_l)(w32_wint_t c, w32_locale_t l)
{
	(void)l;
	return W32(iswspace)(c);
}

#include "orbis_wchar32_width.h"

int W32(wcwidth)(w32_t wc)
{
	if ((unsigned)wc < 0xffU)
		return ((wc+1) & 0x7f) >= 0x21 ? 1 : wc ? -1 : 0;
	if ((wc & 0xfffeffffU) < 0xfffe) {
		if ((w32_nonspacing_table[w32_nonspacing_table[wc>>8]*32+((wc&255)>>3)]>>(wc&7))&1)
			return 0;
		if ((w32_wide_table[w32_wide_table[wc>>8]*32+((wc&255)>>3)]>>(wc&7))&1)
			return 2;
		return 1;
	}
	if ((wc & 0xfffe) == 0xfffe)
		return -1;
	if (wc-0x20000U < 0x20000)
		return 2;
	if (wc == 0xe0001 || wc-0xe0020U < 0x5f || wc-0xe0100U < 0xef)
		return 0;
	return 1;
}

int W32(wcswidth)(const w32_t *wcs, size_t n)
{
	int l=0, k=0;
	for (; n-- && *wcs && (k = W32(wcwidth)(*wcs)) >= 0; l+=k, wcs++);
	return (k < 0) ? k : l;
}


/* ---------------------------------------------------------------- wcsto* (on top of the narrow strto*)
 *
 * musl's versions feed the wide string to __intscan/__floatscan through a fake FILE and map every
 * non-ASCII character to '@', which no number can contain. The same result comes from copying the
 * leading ASCII run into a narrow buffer and handing it to strto*: the parse stops at the same
 * place, and the end pointer maps back one character per byte. */

static const w32_t *w32_skip_space(const w32_t *t)
{
	while (W32(iswspace)((w32_wint_t)*t)) t++;
	return t;
}

/* The ASCII prefix of t as a C string, in stack[] when it fits, else on the heap. */
static char *w32_ascii_prefix(const w32_t *t, char *stack, size_t cap)
{
	size_t k = 0, i;
	char *b = stack;
	while (t[k] > 0 && t[k] < 0x80) k++;
	if (k >= cap) {
		b = malloc(k + 1);
		if (!b) {
			b = stack;
			k = cap - 1;
		}
	}
	for (i = 0; i < k; i++) b[i] = (char)t[i];
	b[k] = 0;
	return b;
}

static void w32_release(char *b, char *stack)
{
	if (b != stack) {
		int e = errno;
		free(b);
		errno = e;
	}
}

#define W32_STRTO_INT(name, type, narrow) \
type W32(name)(const w32_t *restrict s, w32_t **restrict p, int base) \
{ \
	char stack[128], *e; \
	const w32_t *t = w32_skip_space(s); \
	char *b = w32_ascii_prefix(t, stack, sizeof stack); \
	e = b; \
	type y = narrow(b, &e, base); \
	if (p) *p = (w32_t *)(e != b ? t + (e - b) : s); \
	w32_release(b, stack); \
	return y; \
}

#define W32_STRTO_FLT(name, type, narrow) \
type W32(name)(const w32_t *restrict s, w32_t **restrict p) \
{ \
	char stack[128], *e; \
	const w32_t *t = w32_skip_space(s); \
	char *b = w32_ascii_prefix(t, stack, sizeof stack); \
	e = b; \
	type y = narrow(b, &e); \
	if (p) *p = (w32_t *)(e != b ? t + (e - b) : s); \
	w32_release(b, stack); \
	return y; \
}

W32_STRTO_INT(wcstol,   long,               strtol)
W32_STRTO_INT(wcstoll,  long long,          strtoll)
W32_STRTO_INT(wcstoul,  unsigned long,      strtoul)
W32_STRTO_INT(wcstoull, unsigned long long, strtoull)
W32_STRTO_FLT(wcstof,   float,              strtof)
W32_STRTO_FLT(wcstod,   double,             strtod)
W32_STRTO_FLT(wcstold,  long double,        strtold)

intmax_t W32(wcstoimax)(const w32_t *restrict s, w32_t **restrict p, int base)
{
	return W32(wcstoll)(s, p, base);
}

uintmax_t W32(wcstoumax)(const w32_t *restrict s, w32_t **restrict p, int base)
{
	return W32(wcstoull)(s, p, base);
}


/* ---------------------------------------------------------------- vswprintf / swprintf
 *
 * Self-contained: literal text is copied wide, %c %lc %s %ls are produced here (as musl's
 * vfwprintf does: %c goes through btowc, %s through mbtowc, width and precision count wide
 * characters, padding is spaces), and every numeric conversion is formatted by the narrow
 * snprintf - its output is ASCII, one byte per wide character. Like musl: output is truncated to
 * n-1 characters and terminated, and the result is -1 when it did not fit. */

struct w32_out {
	w32_t *s;
	size_t cap;     /* characters that may be stored, excluding the terminator */
	size_t len;     /* characters produced so far, stored or not */
};

static void w32_put(struct w32_out *o, w32_t c)
{
	if (o->len < o->cap) o->s[o->len] = c;
	o->len++;
}

static void w32_pad(struct w32_out *o, size_t k)
{
	while (k--) w32_put(o, ' ');
}

/* Reads a decimal field; -1 on overflow past INT_MAX. */
static int w32_getint(const w32_t **s)
{
	int i;
	for (i=0; (unsigned)**s-'0' < 10; (*s)++) {
		if ((unsigned)i > INT_MAX/10U || **s-'0' > INT_MAX-10*i) i = -1;
		else if (i >= 0) i = 10*i + (**s-'0');
	}
	return i;
}

enum { W32_LEN_NONE, W32_LEN_HH, W32_LEN_H, W32_LEN_L, W32_LEN_LL, W32_LEN_BIGL, W32_LEN_J, W32_LEN_Z, W32_LEN_T };

int W32(vswprintf)(w32_t *restrict s, size_t n, const w32_t *restrict fmt, va_list ap)
{
	struct w32_out o;
	const w32_t *f = fmt;
	va_list aq;
	int result = -1;

	if (!n) return -1;
	o.s = s;
	o.cap = n - 1;
	o.len = 0;
	va_copy(aq, ap);

	while (*f) {
		if (o.len > INT_MAX) goto overflow;
		if (*f != '%') {
			w32_put(&o, *f++);
			continue;
		}
		if (f[1] == '%') {
			w32_put(&o, '%');
			f += 2;
			continue;
		}
		f++;

		/* Positional arguments are not supported. */
		{
			const w32_t *q = f;
			while ((unsigned)*q-'0' < 10) q++;
			if (q != f && *q == '$') goto inval;
		}

		/* flags */
		char flags[8];
		int nflags = 0, left = 0;
		for (;;) {
			w32_t c = *f;
			if (c != '-' && c != '+' && c != ' ' && c != '#' && c != '0' && c != '\'') break;
			if (c == '-') left = 1;
			if (nflags < (int)sizeof flags - 1 && !memchr(flags, (int)c, (size_t)nflags)) flags[nflags++] = (char)c;
			f++;
		}
		flags[nflags] = 0;

		/* width */
		int w;
		if (*f == '*') {
			w = va_arg(aq, int);
			f++;
			if (w < 0) {
				if (w == INT_MIN) goto overflow;
				w = -w;
				if (!left) {
					left = 1;
					if (nflags < (int)sizeof flags - 1) flags[nflags++] = '-', flags[nflags] = 0;
				}
			}
		} else if ((w = w32_getint(&f)) < 0) goto overflow;

		/* precision: p < 0 means none */
		int p = -1;
		if (*f == '.' && f[1] == '*') {
			p = va_arg(aq, int);
			if (p < 0) p = -1;
			f += 2;
		} else if (*f == '.') {
			f++;
			if ((p = w32_getint(&f)) < 0) goto overflow;
		}

		/* length */
		int len = W32_LEN_NONE;
		switch (*f) {
		case 'h': f++; if (*f == 'h') f++, len = W32_LEN_HH; else len = W32_LEN_H; break;
		case 'l': f++; if (*f == 'l') f++, len = W32_LEN_LL; else len = W32_LEN_L; break;
		case 'L': f++; len = W32_LEN_BIGL; break;
		case 'q': f++; len = W32_LEN_LL; break;
		case 'j': f++; len = W32_LEN_J; break;
		case 'z': f++; len = W32_LEN_Z; break;
		case 't': f++; len = W32_LEN_T; break;
		}

		w32_t conv = *f++;
		if (conv == 'c' && len == W32_LEN_L) conv = 'C';
		if (conv == 's' && len == W32_LEN_L) conv = 'S';

		char cf[32], nb[256], *out = nb;
		int k = 0;

		switch (conv) {
		case 'n': {
			void *ptr = va_arg(aq, void *);
			switch (len) {
			case W32_LEN_NONE: *(int *)ptr = (int)o.len; break;
			case W32_LEN_HH: *(signed char *)ptr = (signed char)o.len; break;
			case W32_LEN_H: *(short *)ptr = (short)o.len; break;
			case W32_LEN_L: *(long *)ptr = (long)o.len; break;
			case W32_LEN_LL: *(long long *)ptr = (long long)o.len; break;
			case W32_LEN_J: *(intmax_t *)ptr = (intmax_t)o.len; break;
			case W32_LEN_Z: *(size_t *)ptr = o.len; break;
			case W32_LEN_T: *(ptrdiff_t *)ptr = (ptrdiff_t)o.len; break;
			default: goto inval;
			}
			continue;
		}
		case 'c':
		case 'C': {
			w32_t c = conv == 'C' ? (w32_t)va_arg(aq, w32_wint_t) : (w32_t)W32(btowc)(va_arg(aq, int));
			if (w < 1) w = 1;
			if (!left) w32_pad(&o, w-1);
			w32_put(&o, c);
			if (left) w32_pad(&o, w-1);
			continue;
		}
		case 'S': {
			static const w32_t null_ws[] = { '(', 'n', 'u', 'l', 'l', ')', 0 };
			const w32_t *a = va_arg(aq, const w32_t *);
			size_t l;
			if (!a) a = null_ws;
			l = W32(wcsnlen)(a, p < 0 ? (size_t)INT_MAX : (size_t)p);
			if (p < 0 && a[l]) goto overflow;
			if ((size_t)w > l && !left) w32_pad(&o, w-l);
			for (size_t i = 0; i < l; i++) w32_put(&o, a[i]);
			if ((size_t)w > l && left) w32_pad(&o, w-l);
			continue;
		}
		case 's': {
			const char *a = va_arg(aq, const char *), *bs;	/* a: walked again below */
			size_t l = 0;
			int i = 0;
			w32_t wc;
			if (!a) a = "(null)";
			for (bs = a; l < (p < 0 ? (size_t)INT_MAX : (size_t)p) && (i = W32(mbtowc)(&wc, bs, W32_MB_LEN_MAX)) > 0; bs += i, l++);
			if (i < 0) goto fail;
			if (p < 0 && *bs) goto overflow;
			if ((size_t)w > l && !left) w32_pad(&o, w-l);
			for (size_t j = 0; j < l; j++) {
				i = W32(mbtowc)(&wc, a, W32_MB_LEN_MAX);
				a += i;
				w32_put(&o, wc);
			}
			if ((size_t)w > l && left) w32_pad(&o, w-l);
			continue;
		}
		case 'd': case 'i': {
			intmax_t v;
			switch (len) {
			case W32_LEN_HH: v = (signed char)va_arg(aq, int); break;
			case W32_LEN_H: v = (short)va_arg(aq, int); break;
			case W32_LEN_NONE: v = va_arg(aq, int); break;
			case W32_LEN_L: v = va_arg(aq, long); break;
			case W32_LEN_LL: v = va_arg(aq, long long); break;
			case W32_LEN_J: v = va_arg(aq, intmax_t); break;
			case W32_LEN_Z: v = (intmax_t)(ptrdiff_t)va_arg(aq, size_t); break;
			case W32_LEN_T: v = va_arg(aq, ptrdiff_t); break;
			default: goto inval;
			}
			snprintf(cf, sizeof cf, "%%%s*.*j%c", flags, (char)conv);
			k = snprintf(nb, sizeof nb, cf, w, p, v);
			if (k >= (int)sizeof nb) {
				if (!(out = malloc((size_t)k + 1))) goto fail;
				k = snprintf(out, (size_t)k + 1, cf, w, p, v);
			}
			break;
		}
		case 'o': case 'u': case 'x': case 'X': {
			uintmax_t v;
			switch (len) {
			case W32_LEN_HH: v = (unsigned char)va_arg(aq, int); break;
			case W32_LEN_H: v = (unsigned short)va_arg(aq, int); break;
			case W32_LEN_NONE: v = va_arg(aq, unsigned); break;
			case W32_LEN_L: v = va_arg(aq, unsigned long); break;
			case W32_LEN_LL: v = va_arg(aq, unsigned long long); break;
			case W32_LEN_J: v = va_arg(aq, uintmax_t); break;
			case W32_LEN_Z: v = va_arg(aq, size_t); break;
			case W32_LEN_T: v = (uintmax_t)(size_t)va_arg(aq, ptrdiff_t); break;
			default: goto inval;
			}
			snprintf(cf, sizeof cf, "%%%s*.*j%c", flags, (char)conv);
			k = snprintf(nb, sizeof nb, cf, w, p, v);
			if (k >= (int)sizeof nb) {
				if (!(out = malloc((size_t)k + 1))) goto fail;
				k = snprintf(out, (size_t)k + 1, cf, w, p, v);
			}
			break;
		}
		case 'p': {
			void *v = va_arg(aq, void *);
			snprintf(cf, sizeof cf, "%%%s*p", flags);
			k = snprintf(nb, sizeof nb, cf, w, v);
			if (k >= (int)sizeof nb) {
				if (!(out = malloc((size_t)k + 1))) goto fail;
				k = snprintf(out, (size_t)k + 1, cf, w, v);
			}
			break;
		}
		case 'a': case 'A': case 'e': case 'E': case 'f': case 'F': case 'g': case 'G':
			if (len == W32_LEN_BIGL) {
				long double v = va_arg(aq, long double);
				snprintf(cf, sizeof cf, "%%%s*.*L%c", flags, (char)conv);
				k = snprintf(nb, sizeof nb, cf, w, p, v);
				if (k >= (int)sizeof nb) {
					if (!(out = malloc((size_t)k + 1))) goto fail;
					k = snprintf(out, (size_t)k + 1, cf, w, p, v);
				}
			} else if (len == W32_LEN_NONE || len == W32_LEN_L) {
				double v = va_arg(aq, double);
				snprintf(cf, sizeof cf, "%%%s*.*%c", flags, (char)conv);
				k = snprintf(nb, sizeof nb, cf, w, p, v);
				if (k >= (int)sizeof nb) {
					if (!(out = malloc((size_t)k + 1))) goto fail;
					k = snprintf(out, (size_t)k + 1, cf, w, p, v);
				}
			} else goto inval;
			break;
		default:
			goto inval;
		}

		if (k < 0) {
			if (out != nb) free(out);
			goto fail;
		}
		for (int i = 0; i < k; i++) w32_put(&o, (unsigned char)out[i]);
		if (out != nb) free(out);
	}

	if (o.len > INT_MAX) goto overflow;
	result = (o.len >= n) ? -1 : (int)o.len;
	goto done;

inval:
	errno = EINVAL;
	goto fail;
overflow:
	errno = EOVERFLOW;
fail:
	result = -1;
done:
	s[o.len < o.cap ? o.len : o.cap] = 0;
	va_end(aq);
	return result;
}

int W32(swprintf)(w32_t *restrict s, size_t n, const w32_t *restrict fmt, ...)
{
	int r;
	va_list ap;
	va_start(ap, fmt);
	r = W32(vswprintf)(s, n, fmt, ap);
	va_end(ap);
	return r;
}


/* ---------------------------------------------------------------- wcsftime (on top of strftime) */

size_t W32(__wcsftime_l)(w32_t *restrict s, size_t n, const w32_t *restrict f, const struct tm *restrict tm, w32_locale_t loc)
{
	char fstack[256], ostack[512], *nf = fstack, *out = ostack;
	size_t fl, cap, k, wl, r = 0;
	(void)loc;

	if (!n) return 0;
	s[0] = 0;
	fl = W32(wcstombs)(0, f, 0);
	if (fl == (size_t)-1) return 0;
	if (fl >= sizeof fstack && !(nf = malloc(fl + 1))) return 0;
	W32(wcstombs)(nf, f, fl + 1);

	/* n-1 wide characters need at most 4*(n-1) bytes, plus the terminator */
	cap = n > (1U << 20) ? (4U << 20) + 1 : 4*n + 1;
	if (cap > sizeof ostack && !(out = malloc(cap))) goto done;
	k = strftime(out, cap, nf, tm);
	if (k) {
		wl = W32(mbstowcs)(0, out, 0);
		if (wl != (size_t)-1 && wl < n) {
			W32(mbstowcs)(s, out, n);
			r = wl;
		}
	}
done:
	if (out != ostack) free(out);
	if (nf != fstack) free(nf);
	return r;
}

size_t W32(wcsftime_l)(w32_t *restrict s, size_t n, const w32_t *restrict f, const struct tm *restrict tm, w32_locale_t loc)
{
	return W32(__wcsftime_l)(s, n, f, tm, loc);
}

size_t W32(wcsftime)(w32_t *restrict s, size_t n, const w32_t *restrict f, const struct tm *restrict tm)
{
	return W32(__wcsftime_l)(s, n, f, tm, 0);
}
