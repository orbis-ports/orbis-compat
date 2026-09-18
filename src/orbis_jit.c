/* One arena of writable, executable memory, shared by every recompiler in a module.
 * See include/orbis_jit.h for what the console will and will not do; this file is how.
 *
 * ⚠ IT IS A SUBALLOCATOR AND THAT IS THE POINT, not a convenience. The two files this replaces
 * both hand a kernel object to each consumer, and both measurements of what that costs are on
 * hardware:
 *
 *   paraLLEl-RSP mapped a 64 MiB arena per module load and this port does not run a module's
 *   destructors, so nothing gave it back. 2026-08-25: four arenas, 256 MiB, and the next load got
 *   `no direct memory for 65536 KiB -> 0x80020023` - EAGAIN, the pool gone, on a console that had
 *   been running the core minutes earlier.
 *
 *   Play!'s CodeGen mapped one page per compiled basic block. A PS2 recompiler makes thousands,
 *   this console's page is 16 KiB, and Grand Theft Auto III reached its intro at 3-5 fps and then
 *   wrote to (void*)-1 when the kernel stopped handing them out.
 *
 * One arena, grown on demand, with a coalescing free list answers both. flycast's three code
 * caches, mupen64plus's two recompilers and Play!'s thousands of blocks are one kernel object.
 *
 * SPDX-License-Identifier: MIT
 */
#include <orbis_jit.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <orbis/libkernel.h>

#include <orbis_env.h>
#include "orbis_report.h"

/* FreeBSD's MAP_FIXED, which is what Sony's mapper takes: place this mapping AT the address given.
 * Spelled out because the SDK headers do not name it; the other production use of the same constant
 * is mesa-ps4's ac_orbis_drm.c, which moves buffers between buses with it. */
#define ORBIS_JIT_MAP_FIXED 0x0010

#define ORBIS_JIT_PROT_RW  (ORBIS_KERNEL_PROT_CPU_RW)                              /* 0x03 */
#define ORBIS_JIT_PROT_RWX (ORBIS_KERNEL_PROT_CPU_RW | ORBIS_KERNEL_PROT_CPU_EXEC) /* 0x07 */

/* ⚠ THE NAME IS NOT DECORATION. sceKernelVirtualQuery hands back a 32-character name for every
 * mapping, and it is the only thing that tells a reader walking this process's address space -
 * ours, or a crash dump - which range is a code arena. beetle-psx's orbis_lr_range_free already
 * prints that field on every rejection; unnamed mappings print as an empty string. */
#define ORBIS_JIT_ARENA_NAME "orbis_jit"

/* A rel32 displacement is signed 32-bit, so the reach is 2 GiB either way... */
#define ORBIS_JIT_REACH      0x80000000ull
/* ...and this is how much of it the MODULE is assumed to occupy, because the anchor below is ONE
 * address inside the module and not the module's edge. flycast declares 47 MiB of code caches and
 * swanstation 50 MiB of .bss, so a module extends tens of megabytes either side of any function in
 * it; 256 MiB is a factor of five above the largest this port has built. */
#define ORBIS_JIT_MODULE_SPAN 0x10000000ull
/* ...which leaves this much for the arena itself, on either side of the module. 1.5 GiB. Written
 * as the subtraction rather than as a number so that the search below and the re-check in
 * orbis_jit_alloc cannot drift apart - they answer the same question from opposite ends. */
#define ORBIS_JIT_WINDOW     (ORBIS_JIT_REACH - 2ull * ORBIS_JIT_MODULE_SPAN)

/* Blocks are cache-line aligned. 16 would satisfy every x86-64 emitter here; 64 additionally keeps
 * a store into one block off the line another block is executing from, which costs nothing when
 * blocks are kilobytes and matters when Play! is compiling one while the EE thread runs another.
 *
 * ⚠ AND ONE SUCH UNIT IN FRONT OF EVERY BLOCK CARRIES ITS LENGTH, because orbis_jit_free takes no
 * size - see the header for why it must not. dynarmic's own POSIX allocator does exactly this and
 * spends a whole 4096-byte page on it (block_of_code.cpp:81-102); a cache line is enough for a
 * size_t. The header is inside the arena and therefore writable and executable, so a recompiler
 * that underruns its block corrupts the NEXT block's length rather than its own code - the same
 * exposure the free list already has, and the reason a block overrun here reads as a bad free
 * rather than as a bad jump. */
#define ORBIS_JIT_ALIGN 64u

#define ORBIS_JIT_MAX_ARENAS 8
/* 128 MiB of arena divided by the smallest block a recompiler emits is far more than this, but a
 * free span can only exist between two live blocks, and the list coalesces on every free. Running
 * out is not fatal: the span is dropped, which leaks that block, and it is reported. */
#define ORBIS_JIT_MAX_SPANS  256

typedef struct {
   uint8_t *at;
   size_t   len;
} orbis_jit_span;

typedef struct {
   uint8_t       *base;
   size_t         size;
   off_t          phys;
   /* An arena from orbis_jit_alloc_at belongs to one caller at one address: it is never
    * suballocated, and freeing it unmaps it rather than returning a span. */
   int            dedicated;
   unsigned       nspans;
   orbis_jit_span span[ORBIS_JIT_MAX_SPANS];
} orbis_jit_arena;

/* .bss, so it is zero before any dynamic initialiser runs - and it has to be, because the first
 * caller of this file is a global constructor: RSP::JIT::CPU is constructed while the frontend's
 * MENU is reading a core's name. */
static orbis_jit_arena s_arena[ORBIS_JIT_MAX_ARENAS];
static unsigned        s_arenas;

/* -1 not asked, 0 refused, 1 granted and executed. See orbis_jit_state. */
static int s_state = -1;

/* ---------------------------------------------------------------------------- reporting */

/* Sony returns 0x8002xxxx where the low half is an errno, and two of them mean opposite things:
 * EACCES says the kernel understood the request and declined it, EINVAL says the request was
 * malformed and no policy was ever consulted. Telling them apart is the difference between "this
 * console will not do that" and "we asked wrong". */
static const char *orbis_jit_err(int32_t rc)
{
   switch ((uint32_t)rc)
   {
      case 0x80020001u: return "EPERM - not permitted";
      case 0x80020009u: return "EBADF - the flags word was not anonymous";
      case 0x8002000Cu: return "ENOMEM - out of memory or address space";
      case 0x8002000Du: return "EACCES - understood and refused on policy";
      case 0x8002000Eu: return "EFAULT - bad address argument";
      case 0x80020016u: return "EINVAL - malformed request, policy never reached";
      case 0x80020023u: return "EAGAIN - the direct memory pool is exhausted";
      default:          return "not in this file's table";
   }
}

/* ⚠ QUIET ON THE PATH THAT REPEATS. orbis_report is a SYNCHRONOUS 8-15 ms write to /data, and the
 * frontend loads and unloads a core repeatedly while the user walks the menu - two full
 * load/construct/unload cycles in 82 ms, measured 2026-09-01. A failure is still loud; success says
 * it once per arena and then stops.
 *
 * ⚠ AND orbis_env_get, NOT getenv. This file ends up inside a .prx, every .prx links its own musl
 * `environ`, and setenv() in the eboot is invisible to it - so a knob read with getenv cannot be
 * set on a console at all, and looks exactly like a knob with no effect. The predecessor this file
 * replaces reads ORBIS_EXEC_MEM_VERBOSE with getenv and has therefore never once been switched on.
 * scripts/check-env-knobs.sh exists for that mistake and would reject this file if it repeated it. */
static int s_verbose = -1;

static int orbis_jit_should_log(void)
{
   if (s_verbose < 0)
   {
      const char *const e = orbis_env_get("ORBIS_JIT_VERBOSE");
      s_verbose = (e && *e && *e != '0') ? 1 : 0;
   }
   return s_verbose;
}

/* ---------------------------------------------------------------------------- the lock */

/* ⚠ A SPIN LOCK AND NOT A pthread_mutex_t, AND THE POOL IS THE REASON. Every ScePthread object -
 * mutex, condvar, attr, thread - comes out of libkernel's internal memory, technote 235, and
 * src/orbis_thread.cpp exists because this port has watched that pool run dry and take the process
 * with it. An allocator that must work while the pool is empty cannot spend one to do it. */
static volatile int s_lock;

static void orbis_jit_lock(void)
{
   while (__atomic_exchange_n(&s_lock, 1, __ATOMIC_ACQUIRE))
      sceKernelUsleep(50);
}

static void orbis_jit_unlock(void)
{
   __atomic_store_n(&s_lock, 0, __ATOMIC_RELEASE);
}

/* ---------------------------------------------------------------------------- rounding */

static size_t orbis_jit_round(size_t n, size_t to)
{
   return (n + (to - 1)) & ~(size_t)(to - 1);
}

/* ---------------------------------------------------------------------------- the proof */

/* ⚠ A GRANTED PROTECTION IS NOT AN HONOURED ONE, AND THE WAY TO TELL IS TO RUN SOMETHING.
 *
 * sceKernelQueryMemoryProtection reports the protection a range was MAPPED with, not what mprotect
 * has since made it: on hardware it answered 0x03 for ranges that were executing code, at sixteen
 * base addresses out of sixteen, and using it as the check vetoed a working recompiler every time
 * (ps4-mesa-docs docs/retroarch/HANDOFF.md, 2026-08-23). The lesson is not "drop the check" - it is that on this
 * console a query API is not a measurement of the thing it names. So the check is the one
 * mesa-ps4's probe made: put six bytes of x86-64 in the buffer and call them.
 *
 *     b8 ee ff c0 00   mov eax, 0x00c0ffee
 *     c3               ret
 *
 * Self-modifying code needs no cache maintenance on x86-64, so a wrong answer here is about mapping
 * rather than coherency. A page that will not execute does not return an error - it ends the
 * process - which is why the line announcing the attempt goes out BEFORE the call, and why it is
 * better here, once, at a known point, than on the first recompiled block. */
static int s_first_stub = 1;

static int orbis_jit_verify(void *at)
{
   static const uint8_t stub[] = { 0xb8, 0xee, 0xff, 0xc0, 0x00, 0xc3 };
   uint32_t (*fn)(void);
   uint32_t got;

   memcpy(at, stub, sizeof(stub));
   /* Not a cast: an object pointer to a function pointer is not one. */
   memcpy(&fn, &at, sizeof(fn));

   if (s_first_stub || orbis_jit_should_log())
      orbis_report("jit", "calling a stub at %p to prove the promotion. If this is the last jit "
                          "line, the promotion to 0x07 was granted and not honoured, and that is "
                          "the answer rather than a crash to chase.", at);
   s_first_stub = 0;

   got = fn();
   if (got != 0x00c0ffeeu)
   {
      orbis_report("jit", "stub at %p ran and returned 0x%08x, not 0x00c0ffee - not trusting it "
                          "with recompiled code", at, (unsigned)got);
      return 0;
   }
   return 1;
}

/* ⚠ THE MODULE-WIDE VERDICT ONLY EVER GETS WORSE. One buffer the console will not run is enough to
 * make the recompiler unusable, and a later range that happens to succeed does not undo that. */
static void orbis_jit_record(int ok)
{
   s_state = (s_state < 0) ? ok : (s_state && ok);
}

/* ---------------------------------------------------------------------------- reachability */

/* The address this module's rel32 displacements are measured from. It is the address of a function
 * in THIS object, and that is exact rather than approximate: orbis_jit.o is an archive member of
 * liborbis-compat.a, pulled into whichever module references it, so a core that calls this file is
 * the module this file is in. */
static uintptr_t orbis_jit_anchor(void)
{
   return (uintptr_t)(void *)&orbis_jit_alloc;
}

/* Whether an arena at [base, base+size) is reachable from anywhere in this module. */
static int orbis_jit_near_module(const uint8_t *base, size_t size)
{
   const uintptr_t anchor = orbis_jit_anchor();
   return orbis_jit_reachable((const void *)(anchor - ORBIS_JIT_MODULE_SPAN),
                              2 * ORBIS_JIT_MODULE_SPAN, base, size);
}

int orbis_jit_reachable(const void *from, size_t len, const void *to, size_t len2)
{
   const uintptr_t a = (uintptr_t)from;
   const uintptr_t b = (uintptr_t)to;
   const uintptr_t lo = (a < b) ? a : b;
   const uintptr_t hi = ((a + len) > (b + len2)) ? (a + len) : (b + len2);

   /* Conservative on purpose: the widest displacement either range can generate into the other is
    * the distance between their outermost ends, so measuring that answers both directions at once
    * and never says yes when one of them would not fit. */
   return (hi - lo) < ORBIS_JIT_REACH;
}

/* ---------------------------------------------------------------------------- address search */

/* ⚠ MAP_FIXED ON THIS KERNEL REPLACES WHATEVER IS THERE. There is no MAP_FIXED_NOREPLACE, so the
 * Linux arm's "ask for the address and check what came back" protects nothing: by the time we could
 * check, the frontend's heap would already be gone. Every fixed mapping therefore asks first.
 *
 * ⚠ sceKernelVirtualQuery IS CALLED WITH flags=0, AND A NONZERO RETURN MEANS NOTHING IS MAPPED
 * THERE. That is the only value with an established meaning in this workshop - mesa-ps4's
 * ac_orbis_drm.c - and beetle-psx's orbis_lr_range_free reasons at length about why guessing
 * flags=1 ("the first mapping at or above this address") would be the dangerous kind of wrong: a
 * guess that makes the call fail reports every address as free, MAP_FIXED lands on the heap, and
 * the failure is a corrupted process rather than a refusal.
 *
 * ⚠ WHAT IS NEW HERE IS THE SKIP. That file walks the whole range one 16 KiB granule at a time -
 * about eighteen thousand calls per content load - and throws away what the call returns. The info
 * block carries the containing mapping's start and end, so a granule that is TAKEN answers for the
 * whole mapping and the walk resumes past it. The window below is 1.75 GiB, which a granule-at-a-
 * time walk would cost 114688 calls to cross; with the skip it costs one call per mapping in the
 * way plus one per granule of the span finally chosen.
 *
 * The end field is read defensively: if it is not greater than the address asked about - which is
 * what a wrong guess about that struct would look like - the walk falls back to one granule, so a
 * misread costs speed and never correctness. */
static uint8_t *orbis_jit_find_free(uintptr_t lo, uintptr_t hi, size_t len)
{
   const uintptr_t granule = ORBIS_JIT_GRANULE;
   uintptr_t       p       = (lo + (granule - 1)) & ~(granule - 1);
   uintptr_t       run     = p;

   while (p + granule <= hi)
   {
      OrbisKernelVirtualQueryInfo info;
      memset(&info, 0, sizeof(info));

      if (sceKernelVirtualQuery((const void *)p, 0, &info, sizeof(info)) == 0)
      {
         const uintptr_t end = (uintptr_t)info.unk02;
         p   = (end > p) ? ((end + (granule - 1)) & ~(granule - 1)) : (p + granule);
         run = p;
         continue;
      }

      p += granule;
      if (p - run >= len)
         return (uint8_t *)run;
   }
   return NULL;
}

/* ---------------------------------------------------------------------------- arena creation */

/* Map `len` bytes of direct memory read-write, promote them, and prove they run. `at` is NULL to
 * let the kernel choose or an address to insist on. */
static int orbis_jit_map(orbis_jit_arena *a, void *at, size_t len)
{
   off_t   phys = 0;
   void   *got  = at;
   int32_t rc;

   rc = sceKernelAllocateDirectMemory(0, (off_t)sceKernelGetDirectMemorySize(), len,
                                      ORBIS_JIT_GRANULE, ORBIS_KERNEL_WB_ONION, &phys);
   if (rc != 0)
   {
      orbis_report("jit", "no direct memory for %lu KiB -> 0x%08x (%s)",
                   (unsigned long)(len / 1024), (unsigned)rc, orbis_jit_err(rc));
      return 0;
   }

   /* ⚠ READ-WRITE AT MAP TIME, EXECUTE ONLY AFTERWARDS. Asking sceKernelMapDirectMemory for
    * READ|EXECUTE up front returns 0x8002000d, EACCES - understood and declined on policy. The
    * policy lives at MAP time, not at PROTECT time. Anyone who tries the direct form first
    * concludes a recompiler is impossible on this console. */
   rc = sceKernelMapNamedDirectMemory(&got, len, ORBIS_JIT_PROT_RW,
                                      at ? ORBIS_JIT_MAP_FIXED : 0,
                                      phys, ORBIS_JIT_GRANULE, ORBIS_JIT_ARENA_NAME);
   if (rc != 0 || !got || (at && got != at))
   {
      orbis_report("jit", "could not map %lu KiB at %p -> 0x%08x (%s), landed at %p",
                   (unsigned long)(len / 1024), at, (unsigned)rc, orbis_jit_err(rc), got);
      if (rc == 0 && got)
         sceKernelMunmap(got, len);
      sceKernelReleaseDirectMemory(phys, len);
      return 0;
   }

   rc = sceKernelMprotect(got, len, ORBIS_JIT_PROT_RWX);
   if (rc != 0)
   {
      orbis_report("jit", "%lu KiB at %p would not take execute -> 0x%08x (%s). The recompiler "
                          "has nowhere to write and must not be used.",
                   (unsigned long)(len / 1024), got, (unsigned)rc, orbis_jit_err(rc));
      sceKernelMunmap(got, len);
      sceKernelReleaseDirectMemory(phys, len);
      orbis_jit_record(0);
      return 0;
   }

   if (!orbis_jit_verify(got))
   {
      sceKernelMunmap(got, len);
      sceKernelReleaseDirectMemory(phys, len);
      orbis_jit_record(0);
      return 0;
   }

   a->base = (uint8_t *)got;
   a->size = len;
   a->phys = phys;
   orbis_jit_record(1);
   return 1;
}

/* How big an arena to take when one is needed. Never smaller than the request that triggered it. */
static size_t orbis_jit_arena_size(size_t atleast)
{
   /* ⚠ 16 MiB, AND SMALL ON PURPOSE. The predecessor took 64 and a console that had been running
    * the core died on the fourth load with the pool gone (see the file header). This is one arena
    * for the whole module rather than one per consumer, and another is taken on demand, so the
    * number is a first guess and not a ceiling. ORBIS_JIT_ARENA_MB moves it without a rebuild. */
   size_t      mb = 16;
   const char *e  = orbis_env_get("ORBIS_JIT_ARENA_MB");

   if (e && *e)
   {
      const long v = strtol(e, NULL, 10);
      if (v > 0 && v <= 512)
         mb = (size_t)v;
   }

   {
      const size_t want = mb * 1024u * 1024u;
      return orbis_jit_round((atleast > want) ? atleast : want, ORBIS_JIT_GRANULE);
   }
}

/* Take a new arena. `flags` carries ORBIS_JIT_NEAR_TEXT, which chooses the address by search. */
static orbis_jit_arena *orbis_jit_grow(size_t atleast, unsigned flags)
{
   orbis_jit_arena *a;
   size_t           len;

   if (s_arenas >= ORBIS_JIT_MAX_ARENAS)
   {
      orbis_report("jit", "all %d arena slots are in use and %lu KiB more was asked for",
                   ORBIS_JIT_MAX_ARENAS, (unsigned long)(atleast / 1024));
      return NULL;
   }

   a   = &s_arena[s_arenas];
   len = orbis_jit_arena_size(atleast);

   if (flags & ORBIS_JIT_NEAR_TEXT)
   {
      const uintptr_t anchor = orbis_jit_anchor();
      uint8_t        *at;

      /* Above the module first. A module is loaded low in its own region and the address space
       * above it is where every arena this port has ever been given by the kernel landed, so the
       * search usually ends on its first free granule; below is the fallback for a module that has
       * something parked above it. */
      at = orbis_jit_find_free(anchor + ORBIS_JIT_MODULE_SPAN, anchor + ORBIS_JIT_WINDOW, len);
      if (!at)
         at = orbis_jit_find_free(anchor - ORBIS_JIT_WINDOW, anchor - ORBIS_JIT_MODULE_SPAN, len);

      if (!at)
      {
         orbis_report("jit", "no free %lu KiB within rel32 reach of this module (anchor %p). A "
                             "buffer outside it would not run slower, it would jump somewhere "
                             "arbitrary, so this is a refusal rather than a fallback.",
                      (unsigned long)(len / 1024), (void *)anchor);
         return NULL;
      }

      if (!orbis_jit_map(a, at, len))
         return NULL;
   }
   else if (!orbis_jit_map(a, NULL, len))
      return NULL;

   /* Explicit rather than relying on .bss: freeing a dedicated arena compacts the table, so a slot
    * can be reused by a later grow. */
   a->dedicated   = 0;
   a->nspans      = 1;
   a->span[0].at  = a->base;
   a->span[0].len = a->size;
   s_arenas++;

   if (s_arenas == 1 || orbis_jit_should_log())
      orbis_report("jit", "arena %u: %lu KiB at %p%s", s_arenas, (unsigned long)(len / 1024),
                   (void *)a->base,
                   (flags & ORBIS_JIT_NEAR_TEXT) ? ", within rel32 reach of this module" : "");
   return a;
}

/* ---------------------------------------------------------------------------- the free list */

static void *orbis_jit_take(orbis_jit_arena *a, size_t len)
{
   unsigned i;

   for (i = 0; i < a->nspans; i++)
   {
      uint8_t *at;

      if (a->span[i].len < len)
         continue;

      at = a->span[i].at;
      if (a->span[i].len == len)
      {
         memmove(&a->span[i], &a->span[i + 1],
                 (size_t)(a->nspans - i - 1) * sizeof(a->span[0]));
         a->nspans--;
      }
      else
      {
         a->span[i].at  += len;
         a->span[i].len -= len;
      }
      return at;
   }
   return NULL;
}

/* Put a block back and coalesce with its neighbours. The list is kept sorted by address, which is
 * what makes coalescing a look at index-1 and index+1 rather than a scan - and coalescing is not
 * optional here: Play! frees and reallocates one span per invalidated basic block, thousands of
 * times a session, and a list that only ever splits fragments into slivers nothing fits in. */
static void orbis_jit_give(orbis_jit_arena *a, uint8_t *at, size_t len)
{
   unsigned i = 0;

   while (i < a->nspans && a->span[i].at < at)
      i++;

   if (a->nspans >= ORBIS_JIT_MAX_SPANS)
   {
      orbis_report("jit", "no free-list slot for %lu KiB at %p - that block is leaked until the "
                          "arena is released", (unsigned long)(len / 1024), (void *)at);
      return;
   }

   memmove(&a->span[i + 1], &a->span[i], (size_t)(a->nspans - i) * sizeof(a->span[0]));
   a->span[i].at  = at;
   a->span[i].len = len;
   a->nspans++;

   if ((i + 1) < a->nspans && a->span[i].at + a->span[i].len == a->span[i + 1].at)
   {
      a->span[i].len += a->span[i + 1].len;
      memmove(&a->span[i + 1], &a->span[i + 2],
              (size_t)(a->nspans - i - 2) * sizeof(a->span[0]));
      a->nspans--;
   }
   if (i > 0 && a->span[i - 1].at + a->span[i - 1].len == a->span[i].at)
   {
      a->span[i - 1].len += a->span[i].len;
      memmove(&a->span[i], &a->span[i + 1],
              (size_t)(a->nspans - i - 1) * sizeof(a->span[0]));
      a->nspans--;
   }
}

/* ---------------------------------------------------------------------------- the interface */

void *orbis_jit_alloc(size_t size, unsigned flags)
{
   const size_t len = orbis_jit_round(size ? size : 1, ORBIS_JIT_ALIGN) + ORBIS_JIT_ALIGN;
   void        *at  = NULL;
   unsigned     i;

   orbis_jit_lock();

   for (i = 0; i < s_arenas && !at; i++)
   {
      if (s_arena[i].dedicated)
         continue;
      /* ⚠ AN ARENA THAT IS NOT REACHABLE DOES NOT SERVE A REACHABLE REQUEST. Arenas are taken with
       * and without ORBIS_JIT_NEAR_TEXT by different consumers in the same module, so the flag has
       * to be re-checked against the arena rather than remembered from the request that created
       * it. Measuring is exact and cheaper than a flag. */
      if ((flags & ORBIS_JIT_NEAR_TEXT) &&
          !orbis_jit_near_module(s_arena[i].base, s_arena[i].size))
         continue;
      at = orbis_jit_take(&s_arena[i], len);
   }

   if (!at)
   {
      orbis_jit_arena *a = orbis_jit_grow(len, flags);
      if (a)
         at = orbis_jit_take(a, len);
   }

   if (at)
   {
      memcpy(at, &len, sizeof(len));
      at = (uint8_t *)at + ORBIS_JIT_ALIGN;
   }

   orbis_jit_unlock();
   return at;
}

void *orbis_jit_alloc_at(void *addr, size_t size)
{
   const size_t len = orbis_jit_round(size ? size : 1, ORBIS_JIT_GRANULE);
   void        *at  = NULL;

   if (!addr || ((uintptr_t)addr & (ORBIS_JIT_GRANULE - 1)))
   {
      orbis_report("jit", "%p is not a %u-byte aligned address, which sceKernelMapDirectMemory "
                          "rejects as malformed rather than refusing on policy",
                   addr, ORBIS_JIT_GRANULE);
      return NULL;
   }

   orbis_jit_lock();

   if (s_arenas >= ORBIS_JIT_MAX_ARENAS)
      orbis_report("jit", "all %d arena slots are in use", ORBIS_JIT_MAX_ARENAS);
   else if (!orbis_jit_find_free((uintptr_t)addr, (uintptr_t)addr + len, len))
      orbis_report("jit", "%p is already mapped, and MAP_FIXED here would replace whatever is "
                          "there rather than fail", addr);
   else if (orbis_jit_map(&s_arena[s_arenas], addr, len))
   {
      s_arena[s_arenas].dedicated = 1;
      at = s_arena[s_arenas].base;
      s_arenas++;
   }

   orbis_jit_unlock();
   return at;
}

void orbis_jit_free(void *addr)
{
   unsigned i;

   if (!addr)
      return;

   orbis_jit_lock();

   for (i = 0; i < s_arenas; i++)
   {
      uint8_t *const at = (uint8_t *)addr;

      if (at < s_arena[i].base || at >= s_arena[i].base + s_arena[i].size)
         continue;

      if (s_arena[i].dedicated)
      {
         if (at != s_arena[i].base)
         {
            /* A dedicated arena is one block at one address, so an interior pointer is not a free
             * of it - unmapping on that would take the caller's whole code buffer away. */
            orbis_report("jit", "%p is inside the dedicated arena at %p but is not its base",
                         addr, (void *)s_arena[i].base);
            break;
         }
         /* ⚠ munmap ALONE WOULD LEAVE THE PHYSICAL PAGES ALLOCATED. Direct memory is released by
          * the offset the allocator returned and unmapped by the address, which are two handles for
          * one allocation - and a caller that only kept the address cannot give the memory back.
          * That is the whole reason this file owns the arena table. */
         sceKernelMunmap(s_arena[i].base, s_arena[i].size);
         sceKernelReleaseDirectMemory(s_arena[i].phys, s_arena[i].size);
         memmove(&s_arena[i], &s_arena[i + 1],
                 (size_t)(s_arenas - i - 1) * sizeof(s_arena[0]));
         s_arenas--;
         memset(&s_arena[s_arenas], 0, sizeof(s_arena[0]));
      }
      else
      {
         uint8_t *const blk = at - ORBIS_JIT_ALIGN;
         size_t         len = 0;

         memcpy(&len, blk, sizeof(len));
         /* A length that is not a multiple of the unit, or that runs past the end of the arena, is
          * a header something has written over - a block overrun, or a free of a pointer this file
          * did not hand out. Dropping the block leaks it; trusting the number would put an
          * arbitrary span on the free list and hand it to the next recompiler. */
         if (len < 2 * ORBIS_JIT_ALIGN || (len & (ORBIS_JIT_ALIGN - 1)) ||
             blk + len > s_arena[i].base + s_arena[i].size)
            orbis_report("jit", "the block header in front of %p reads %lu, which is not a length "
                                "this file wrote - leaking it rather than handing that span out "
                                "again", addr, (unsigned long)len);
         else
            orbis_jit_give(&s_arena[i], blk, len);
      }
      break;
   }

   orbis_jit_unlock();
}

void orbis_jit_release_all(void)
{
   unsigned i;

   orbis_jit_lock();
   for (i = 0; i < s_arenas; i++)
   {
      sceKernelMunmap(s_arena[i].base, s_arena[i].size);
      sceKernelReleaseDirectMemory(s_arena[i].phys, s_arena[i].size);
   }
   memset(s_arena, 0, sizeof(s_arena));
   s_arenas = 0;
   orbis_jit_unlock();
}

/* ---------------------------------------------------------------------------- promotion */

/* ⚠ ONE CACHED ANSWER WAS RIGHT FOR ONE BUFFER AND SILENTLY WRONG FOR THREE. The predecessor was
 * written for mupen64plus, which has a single code buffer, so "asked twice means asked about the
 * same pages" held and the first answer was returned for every later call. flycast has THREE
 * in-module caches - SH4_TCB 11 MiB, ARM7_TCB 4 MiB and the AICA DSP's 32 KiB - and each is a
 * different range that has to be mprotected on its own; the second and third calls returned 1
 * without touching those pages, and the first write into them would have faulted with SEGV_ACCERR.
 *
 * ⚠ AND OVERLAP, NOT CONTAINMENT. Measured 2026-09-01: melonDS asks twice with the SAME length and
 * bases 224 KiB apart - 0x800e14000 and 0x800e4c000, 131072 KiB each. Neither contains the other,
 * so a containment test missed both ways and the two alternated forever - mprotect, stub, log,
 * mprotect, stub, log - scribbling six bytes over melonDS's generated code during gameplay.
 * Sixteen slots is one more than this port's hungriest core needs. */
#define ORBIS_JIT_MAX_PROMOTED 16
static struct { uintptr_t base, end; int ok; } s_promoted[ORBIS_JIT_MAX_PROMOTED];
static unsigned s_promoted_n;

int orbis_jit_protect(void *addr, size_t len)
{
   uintptr_t base = (uintptr_t)addr;
   uintptr_t end  = base + len;
   int       overlap = -1;
   int       ok;
   int32_t   rc;
   unsigned  i;

   /* ⚠ ROUNDED OUTWARDS, NOT INWARDS. sceKernelMprotect takes 16 KiB units and a caller's array may
    * be no better than 4 KiB aligned - mupen64plus's is declared ALIGN(4096) - so rounding the base
    * UP would leave the first pages of the buffer unpromoted, and the recompiler writes its first
    * block exactly there. Rounding down covers whatever else shares that granule, which is other
    * .bss of this module and no more dangerous for being executable than the buffer beside it. A
    * caller that would rather not share aligns its array to ORBIS_JIT_GRANULE, as flycast's three
    * caches do. */
   base &= ~(uintptr_t)(ORBIS_JIT_GRANULE - 1);
   end   = (end + (ORBIS_JIT_GRANULE - 1)) & ~(uintptr_t)(ORBIS_JIT_GRANULE - 1);

   orbis_jit_lock();

   for (i = 0; i < s_promoted_n; i++)
      if (base >= s_promoted[i].base && end <= s_promoted[i].end)
      {
         ok = s_promoted[i].ok;
         orbis_jit_unlock();
         return ok;
      }

   for (i = 0; i < s_promoted_n; i++)
      if (base < s_promoted[i].end && end > s_promoted[i].base)
      {
         overlap = (int)i;
         break;
      }

   rc = sceKernelMprotect((void *)base, (size_t)(end - base), ORBIS_JIT_PROT_RWX);
   if (rc != 0)
   {
      orbis_report("jit", "%p (%lu KiB) would not take execute - sceKernelMprotect returned "
                          "0x%08x (%s). The recompiler has nowhere to write and must not be used.",
                   (void *)base, (unsigned long)((end - base) / 1024), (unsigned)rc,
                   orbis_jit_err(rc));
      ok = 0;
   }
   else if (overlap >= 0)
   {
      /* The pages next door were proven; these are the same mapping extended. Take that verdict
       * rather than writing a stub over code that is already there, and widen the entry to the
       * union so the pair stops alternating. */
      ok = s_promoted[overlap].ok;
      if (base < s_promoted[overlap].base)
         s_promoted[overlap].base = base;
      if (end > s_promoted[overlap].end)
         s_promoted[overlap].end = end;
      orbis_jit_unlock();
      return ok;
   }
   else
   {
      const int say = s_first_stub || orbis_jit_should_log();
      ok = orbis_jit_verify(addr) ? 1 : 0;
      if (say || !ok)
         orbis_report("jit", "%lu KiB at %p is %s", (unsigned long)(len / 1024), addr,
                      ok ? "writable and executable" : "NOT executable");
   }

   orbis_jit_record(ok);

   if (s_promoted_n < ORBIS_JIT_MAX_PROMOTED)
   {
      s_promoted[s_promoted_n].base = base;
      s_promoted[s_promoted_n].end  = end;
      s_promoted[s_promoted_n].ok   = ok;
      s_promoted_n++;
   }
   else
      orbis_report("jit", "no slot to record %p - a repeat request for these pages will re-run the "
                          "stub check and overwrite generated code", (void *)base);

   orbis_jit_unlock();
   return ok;
}

/* What the promotion decided, for callers that must choose a CPU mode before the recompiler has
 * been anywhere near its buffer. -1 until something has asked; a caller reading -1 has asked too
 * early and should treat it as "unknown" rather than as "no". */
int orbis_jit_state(void)
{
   return s_state;
}
