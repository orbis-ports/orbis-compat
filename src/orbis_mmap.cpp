#include "orbis_mmap.h"

#include <orbis_log.h>

#include <sys/mman.h>

#include <atomic>
#include <cerrno>
#include <cstdint>
#include <cstring>

// The three musl-internal names. Declared here rather than pulled from a header because
// no header declares them: they are libc.a's own linkage between the allocator and the
// kernel, and og_ps4_mmap.h explains why intercepting them - and not `mmap` - is what
// leaves ZenKit's archive mapping untouched by construction.
extern "C" void* __mmap(void* addr, size_t len, int prot, int flags, int fd, off_t off);
extern "C" int   __munmap(void* addr, size_t len);
extern "C" int   __madvise(void* addr, size_t len, int advice);

namespace {

// 16 KiB. musl rounds every mapping to this and this is what it calls PAGE_SIZE:
// `addq $0x400f,%rbx; andq $-0x4000,%rbx` at malloc+0x5a, and the identical pair in
// __expand_heap. Not the 4 KiB a Linux intuition supplies.
constexpr uint64_t Page = 0x4000;

// The direct-memory quantum, used as both the length round-up and the alignment, exactly
// as gnmallocator.cpp does - `OO samples/_common/graphics.cpp:59,116-119`.
constexpr uint64_t DirectAlign = 0x200000;

// Design point 7, verified rather than assumed: a 2 MiB-aligned base is a 16 KiB-aligned
// base, so an offset that is a multiple of Page is an address that is page aligned and
// musl never sees an unaligned mapping.
static_assert(DirectAlign%Page==0,"direct-memory alignment no longer covers musl's page");

constexpr uint64_t CarveBytes = 128ull*1024*1024;
constexpr uint32_t MaxCarves  = 12;
constexpr uint64_t DirectCap  = uint64_t(MaxCarves)*CarveBytes;

// 128 MiB / 240 KiB (the smallest mapping musl can ask for) is 546 live blocks, and a
// free block can only exist between two used ones, so 1536 is a factor of ~1.4 above the
// worst case a carve-out can reach. Running out is not fatal - the next carve-out is
// tried and then the flexible pool, both loudly.
constexpr uint32_t MaxBlocks = 1536;

// The exact tuple musl's two allocator call sites emit, and the only one this file
// claims. PROT_READ|PROT_WRITE == 3 and MAP_PRIVATE|MAP_ANON == 0x1002 (oracle
// `freebsd9/sys_sys_mman.h:52-53,61,84`), which is `movl $3,%edx; movl $0x1002,%ecx`
// at malloc+0x70 and __expand_heap+0x5c.
constexpr int OwnedProt  = PROT_READ|PROT_WRITE;
constexpr int OwnedFlags = MAP_PRIVATE|MAP_ANON;
static_assert(OwnedProt==3 && OwnedFlags==0x1002,"the SDK's mman.h no longer agrees with the disassembly");

constexpr uint32_t LogCap = 8;

struct Block {
  uint64_t off;
  uint64_t size;
  uint32_t used;
  };

struct Carve {
  uint8_t* va;
  off_t    phys;
  uint64_t size;
  uint32_t n;
  Block    blk[MaxBlocks];
  };

// .bss, so it is zero before any dynamic initialiser runs. That matters: the first
// anonymous mapping can happen inside a static constructor, long before boot().
Carve    carve[MaxCarves];
uint32_t carves = 0;

std::atomic<uint64_t> heldBytes{0};
std::atomic<uint64_t> liveBytes{0};
std::atomic<uint64_t> peakLive{0};
std::atomic<uint64_t> cumMapped{0};
std::atomic<uint64_t> mapCalls{0};
std::atomic<uint64_t> unmapCalls{0};
std::atomic<uint64_t> fallbackCalls{0};
std::atomic<uint64_t> fallbackBytes{0};
std::atomic<uint64_t> anomalies{0};
std::atomic<uint32_t> fallbackLogged{0};
std::atomic<uint32_t> anomalyLogged{0};
std::atomic<uint32_t> bannerDone{0};

// A pthread mutex cannot be used here - see og_ps4_mmap.h, "thread safety". Nothing in
// this lock or behind it reaches an allocator.
std::atomic<int> lockFlag{0};

void lockAcq() {
  unsigned spins = 0;
  for(;;) {
    int idle = 0;
    if(lockFlag.compare_exchange_weak(idle,1,std::memory_order_acquire,std::memory_order_relaxed))
      return;
    if(++spins<64) {
      __builtin_ia32_pause();
      } else {
      // Yielding rather than spinning, because the lock is held across
      // sceKernelAllocateDirectMemory and a carve-out is not a few hundred cycles.
      spins = 0;
      sceKernelUsleep(50);
      }
    }
  }

void lockRel() {
  lockFlag.store(0,std::memory_order_release);
  }

struct Guard {
  Guard()  { lockAcq(); }
  ~Guard() { lockRel(); }
  };

uint64_t alignUp(uint64_t v, uint64_t a) {
  return (v+a-1) & ~(a-1);
  }

void banner(bool active) {
  if(bannerDone.exchange(1,std::memory_order_relaxed)!=0)
    return;
  if(active)
    orbis_log("mem mmap-direct: ACTIVE - musl's anonymous mappings are served from DIRECT "
            "memory, carve-out %llu KiB, cap %llu KiB (ORBIS_MMAP_DIRECT=1)",
            (unsigned long long)(CarveBytes/1024),(unsigned long long)(DirectCap/1024));
  else
    orbis_log("mem mmap-direct: DISABLED - musl's anonymous mappings go to FLEXIBLE memory, "
            "the pool that overflowed on 2026-08-04 (ORBIS_MMAP_DIRECT=0)");
  }

void noteLive(uint64_t n) {
  const uint64_t now = liveBytes.fetch_add(n,std::memory_order_relaxed)+n;
  uint64_t hi = peakLive.load(std::memory_order_relaxed);
  while(now>hi && !peakLive.compare_exchange_weak(hi,now,std::memory_order_relaxed))
    ;
  cumMapped.fetch_add(n,std::memory_order_relaxed);
  mapCalls.fetch_add(1,std::memory_order_relaxed);
  }

uint8_t* allocInLocked(Carve& c, uint64_t len) {
  for(uint32_t i=0; i<c.n; ++i) {
    Block& b = c.blk[i];
    if(b.used!=0 || b.size<len)
      continue;
    if(b.size==len) {
      b.used = 1;
      return c.va+b.off;
      }
    // The tail has to become a free block of its own, and if there is no slot for it the
    // block cannot be split - skip it rather than hand out more than was asked for.
    if(c.n>=MaxBlocks)
      continue;
    std::memmove(&c.blk[i+2],&c.blk[i+1],sizeof(Block)*(c.n-i-1));
    c.blk[i+1].off  = b.off+len;
    c.blk[i+1].size = b.size-len;
    c.blk[i+1].used = 0;
    b.size = len;
    b.used = 1;
    ++c.n;
    return c.va+b.off;
    }
  return nullptr;
  }

bool addCarveLocked(uint64_t need) {
  if(carves>=MaxCarves) {
    orbis_log("mem mmap-direct: REFUSED %llu KiB - all %u carve-outs taken (%llu KiB held). "
            "Further anonymous mappings fall back to flexible memory.",
            (unsigned long long)(need/1024),unsigned(MaxCarves),
            (unsigned long long)(heldBytes.load(std::memory_order_relaxed)/1024));
    return false;
    }

  const uint64_t len  = alignUp(need>CarveBytes ? need : CarveBytes,DirectAlign);
  const uint64_t held = heldBytes.load(std::memory_order_relaxed);
  if(held+len>DirectCap) {
    orbis_log("mem mmap-direct: REFUSED %llu KiB - a %llu KiB carve-out would put direct "
            "memory held at %llu KiB, past the %llu KiB cap that keeps the GNM allocator "
            "fed. Falling back to flexible memory.",
            (unsigned long long)(need/1024),(unsigned long long)(len/1024),
            (unsigned long long)((held+len)/1024),(unsigned long long)(DirectCap/1024));
    return false;
    }

  off_t         phys = 0;
  const int32_t errA = sceKernelAllocateDirectMemory(0,off_t(sceKernelGetDirectMemorySize()),
                                                     size_t(len),size_t(DirectAlign),
                                                     ORBIS_KERNEL_WB_ONION,&phys);
  if(errA!=0) {
    orbis_log("mem mmap-direct: sceKernelAllocateDirectMemory(%llu KiB, align %llu, WB_ONION) "
            "failed 0x%08x - %llu KiB already held in %u carve-out(s)",
            (unsigned long long)(len/1024),(unsigned long long)DirectAlign,unsigned(errA),
            (unsigned long long)(held/1024),unsigned(carves));
    return false;
    }

  // WB_ONION and CPU_RW, not the 0x33 gnmallocator.cpp uses: this is CPU heap and the GPU
  // has no business in it. Write-combined GARLIC would be correct for a surface and ruinous
  // for a malloc arena, which is read back constantly.
  void*         va   = nullptr;
  const int32_t errM = sceKernelMapDirectMemory(&va,size_t(len),ORBIS_KERNEL_PROT_CPU_RW,0,
                                                phys,size_t(DirectAlign));
  if(errM!=0) {
    orbis_log("mem mmap-direct: sceKernelMapDirectMemory(%llu KiB, phys 0x%llx) failed 0x%08x",
            (unsigned long long)(len/1024),(unsigned long long)phys,unsigned(errM));
    sceKernelReleaseDirectMemory(phys,size_t(len));
    return false;
    }

  // Verified, not assumed. musl hands the base straight to its chunk arithmetic, so a
  // base that is not page aligned would corrupt the heap silently rather than fail.
  if((reinterpret_cast<uintptr_t>(va) & (Page-1))!=0) {
    orbis_log("mem mmap-direct: sceKernelMapDirectMemory returned %p, which is NOT %llu-byte "
            "aligned - refusing to hand musl a base its page arithmetic cannot use",
            va,(unsigned long long)Page);
    sceKernelMunmap(va,size_t(len));
    sceKernelReleaseDirectMemory(phys,size_t(len));
    return false;
    }

  Carve& c = carve[carves];
  c.va         = reinterpret_cast<uint8_t*>(va);
  c.phys       = phys;
  c.size       = len;
  c.n          = 1;
  c.blk[0].off  = 0;
  c.blk[0].size = len;
  c.blk[0].used = 0;
  ++carves;
  heldBytes.store(held+len,std::memory_order_relaxed);

  // Ungated and loud, like gnmallocator.cpp's arena growth: direct memory is physical, so
  // this line is the moment the console actually loses the pages.
  orbis_log("mem mmap-direct: carve-out %u taken - %llu KiB at %p (phys 0x%llx), direct held "
          "%llu KiB of a %llu KiB cap",
          unsigned(carves-1),(unsigned long long)(len/1024),va,(unsigned long long)phys,
          (unsigned long long)((held+len)/1024),(unsigned long long)(DirectCap/1024));
  return true;
  }

bool freeInLocked(Carve& c, uint8_t* p, uint64_t len) {
  const uint64_t off = uint64_t(p-c.va);

  uint32_t lo = 0, hi = c.n;
  while(lo<hi) {
    const uint32_t m = (lo+hi)/2;
    if(c.blk[m].off<off)
      lo = m+1; else
      hi = m;
    }

  if(lo>=c.n || c.blk[lo].off!=off || c.blk[lo].used==0 || c.blk[lo].size!=len) {
    anomalies.fetch_add(1,std::memory_order_relaxed);
    if(anomalyLogged.fetch_add(1,std::memory_order_relaxed)<LogCap)
      orbis_log("mem mmap-direct: unmap(%p,%llu) does not name a live mapping this "
              "allocator handed out (carve-out offset %llu). musl always unmaps the exact "
              "base and length it was given, so this cannot happen - LEAKING the range "
              "rather than guessing which block it meant.",
              p,(unsigned long long)len,(unsigned long long)off);
    return false;
    }

  c.blk[lo].used = 0;
  liveBytes.fetch_sub(len,std::memory_order_relaxed);
  unmapCalls.fetch_add(1,std::memory_order_relaxed);

  // Coalesce forward first, so the index of the block being freed does not move.
  if(lo+1<c.n && c.blk[lo+1].used==0) {
    c.blk[lo].size += c.blk[lo+1].size;
    std::memmove(&c.blk[lo+1],&c.blk[lo+2],sizeof(Block)*(c.n-lo-2));
    --c.n;
    }
  if(lo>0 && c.blk[lo-1].used==0) {
    c.blk[lo-1].size += c.blk[lo].size;
    std::memmove(&c.blk[lo],&c.blk[lo+1],sizeof(Block)*(c.n-lo-1));
    --c.n;
    }
  return true;
  }

Carve* carveOfLocked(const void* p) {
  const uint8_t* q = reinterpret_cast<const uint8_t*>(p);
  for(uint32_t i=0; i<carves; ++i)
    if(q>=carve[i].va && q<carve[i].va+carve[i].size)
      return &carve[i];
  return nullptr;
  }

bool isOwnedRequest(const void* addr, int prot, int flags, int fd, off_t off) {
  return addr==nullptr && prot==OwnedProt && flags==OwnedFlags && fd<0 && off==0;
  }

void* mapOwned(size_t len) {
  const uint64_t want = alignUp(uint64_t(len),Page);
  uint8_t*       out  = nullptr;
    {
    Guard g;
    for(uint32_t i=0; i<carves && out==nullptr; ++i)
      out = allocInLocked(carve[i],want);
    if(out==nullptr && addCarveLocked(want))
      out = allocInLocked(carve[carves-1],want);
    if(out!=nullptr) {
      noteLive(want);
      } else {
      fallbackCalls.fetch_add(1,std::memory_order_relaxed);
      fallbackBytes.fetch_add(want,std::memory_order_relaxed);
      if(fallbackLogged.fetch_add(1,std::memory_order_relaxed)<LogCap)
        orbis_log("mem mmap-direct: FALLING BACK to flexible memory for %llu KiB - direct "
                "held %llu KiB in %u carve-out(s), %llu KiB live. This request is served "
                "from the pool that overflows.",
                (unsigned long long)(want/1024),
                (unsigned long long)(heldBytes.load(std::memory_order_relaxed)/1024),
                unsigned(carves),
                (unsigned long long)(liveBytes.load(std::memory_order_relaxed)/1024));
      }
    }
  if(out!=nullptr) {
    // NOT optional, and not defensive. musl is entitled to assume MAP_ANON memory is
    // zero filled, and it DOES: calloc returns a mapped chunk without clearing it at all.
    // `testb $0x1,-0x8(%rbx); je` at calloc+0x5f tests the chunk's C_INUSE bit, which is
    // 0 exactly for mmapped chunks, and that branch skips the mal0_clear that every other
    // chunk goes through. A recycled block handed back dirty would therefore make
    // calloc() return garbage, silently, at a call site with no way to notice.
    //
    // Unconditional rather than only for recycled blocks: no pinned source says whether
    // sceKernelAllocateDirectMemory hands back zeroed pages, and this is not a question
    // to answer by assumption. The cost is the one the kernel would have paid to zero an
    // anonymous mapping anyway.
    //
    // Outside the lock: the block is exclusively ours by now, and a 64 MiB memset with a
    // global lock held would serialise every other thread behind it.
    std::memset(out,0,size_t(want));
    return out;
    }
  return mmap(nullptr,len,OwnedProt,OwnedFlags,-1,0);
  }

}

void orbis::mmapDirectReport(const char* where) {
#if ORBIS_MMAP_DIRECT
  orbis_log("mem mmap-direct [%s]: ACTIVE - direct held %llu KiB in %u carve-out(s), live "
          "%llu KiB, peak live %llu KiB",
          where,(unsigned long long)(heldBytes.load(std::memory_order_relaxed)/1024),
          unsigned(carves),
          (unsigned long long)(liveBytes.load(std::memory_order_relaxed)/1024),
          (unsigned long long)(peakLive.load(std::memory_order_relaxed)/1024));
  // cumulative vs held is the proof that munmap comes back: a cumulative figure far above
  // the held figure can only mean blocks were reused.
  orbis_log("mem mmap-direct [%s]: %llu map / %llu unmap calls, %llu KiB mapped cumulatively, "
          "%llu fallback(s) to flexible for %llu KiB, %llu anomaly(s)",
          where,(unsigned long long)mapCalls.load(std::memory_order_relaxed),
          (unsigned long long)unmapCalls.load(std::memory_order_relaxed),
          (unsigned long long)(cumMapped.load(std::memory_order_relaxed)/1024),
          (unsigned long long)fallbackCalls.load(std::memory_order_relaxed),
          (unsigned long long)(fallbackBytes.load(std::memory_order_relaxed)/1024),
          (unsigned long long)anomalies.load(std::memory_order_relaxed));
#else
  orbis_log("mem mmap-direct [%s]: DISABLED at build time - every anonymous mapping goes to "
          "flexible memory (ORBIS_MMAP_DIRECT=0)",where);
#endif
  }

// ------------------------------------------------------------------ the interposition

extern "C" void* __mmap(void* addr, size_t len, int prot, int flags, int fd, off_t off) {
  if(!isOwnedRequest(addr,prot,flags,fd,off))
    return mmap(addr,len,prot,flags,fd,off);
  banner(ORBIS_MMAP_DIRECT!=0);
#if ORBIS_MMAP_DIRECT
  return mapOwned(len);
#else
  // Byte for byte what musl's own __mmap did: mmap.lo is `jmp mmap` and nothing else.
  return mmap(addr,len,prot,flags,fd,off);
#endif
  }

extern "C" int __munmap(void* addr, size_t len) {
    {
    Guard g;
    if(Carve* c = carveOfLocked(addr)) {
      if(freeInLocked(*c,reinterpret_cast<uint8_t*>(addr),alignUp(uint64_t(len),Page)))
        return 0;
      errno = EINVAL;
      return -1;
      }
    }
  return munmap(addr,len);
  }

// MADV_DONTNEED against physically backed direct memory has no meaning any pinned source
// defines, and musl asks for it on the interior of every large freed span. Answered with
// success - which is what musl expects and never checks - instead of finding out.
extern "C" int __madvise(void* addr, size_t len, int advice) {
    {
    Guard g;
    if(carveOfLocked(addr)!=nullptr)
      return 0;
    }
  return madvise(addr,len,advice);
  }
