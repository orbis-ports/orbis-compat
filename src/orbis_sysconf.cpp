// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
// See include/unistd.h for why this exists.
#include <sys/sysctl.h>

// ⚠ THE MACRO FROM OUR OWN unistd.h MUST NOT REACH THE FORWARDING CALL BELOW, or this function
// calls itself. Undefined here and the real symbol declared by hand, which is the one place in
// the overlay that wants libc's sysconf rather than ours.
#include <unistd.h>
#undef sysconf

extern "C" long sysconf(int);

extern "C" long orbis_sysconf(int name) {
#if defined(__ORBIS__) || defined(__PS4__)
  // Both spellings of the question. _SC_NPROCESSORS_CONF is "configured" and _SC_NPROCESSORS_ONLN
  // is "online"; this console does not offline cores, so they are the same number.
  if (name == _SC_NPROCESSORS_ONLN || name == _SC_NPROCESSORS_CONF) {
    size_t len = sizeof(int);
    int    n   = 0;
    if (sysctlbyname("hw.ncpu", &n, &len, nullptr, 0) == 0 && n > 0)
      return n;
    return 1;
    }
#endif
  return sysconf(name);
  }
