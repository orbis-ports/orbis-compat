// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
//
// ---------------------------------------------------------------- the defect
//
// ⚠ ON THIS CONSOLE, setenv() IN ONE IMAGE IS INVISIBLE TO ANOTHER, AND EVERY KNOB THIS
// WORKSHOP HAS DEPENDS ON setenv().
//
// The SDK's libc.a is a REAL STATIC MUSL ARCHIVE - 1481 objects, with getenv and setenv as
// defined text, not stubs into a shared libc module. So an executable and every .prx it loads
// each link their own copy, and each copy has its own `environ`. The frontend's env-file reader
// setenv()s into the eboot's; a core asking getenv() reads the core's, which nothing ever wrote.
//
// MEASURED 2026-08-23: ORBIS_NCPU=1 was written into /data/retroarch-env.txt to isolate
// Lightrec's recompiler worker count, the console was relaunched, and the core still reported
// "Threaded recompiler started with 5 workers". Not a parsing bug and not a stale build - the
// line was applied, to the wrong image's environ.
//
// ⚠ AND IT EXPLAINS WHY NOBODY HIT IT BEFORE. Every knob this workshop has ever set -
// ORBIS_3D_LINEAR, ORBIS_NO_TESS, MESA_LOG_FILE, RADV_DEBUG, all of tempest-env.example.txt -
// is read by Mesa, and Mesa is linked INTO the executable. The first knob that had to reach a
// loadable module was the first one to fail, and it failed silently, looking exactly like a
// knob with no reader. tempest-env.example.txt already warns about that shape: "the run comes
// back clean and reads as a measurement."
//
// ---------------------------------------------------------------- what this does
//
// orbis_env_get() answers from the process's own environment first - so anything genuinely
// set still wins - and falls back to the env FILES, parsed here, in this image.
//
// ⚠ THE FILE LIST NAMES A FRONTEND, and that is a seam rather than a design. The overlay
// should not know what "retroarch" is; the honest mechanism is the loader handing its module
// the values it applied, and libretro has no channel for that. Until something better exists
// the list mirrors exactly what RetroArch's own platform_orbis.c applies, in the same order,
// so a module and its loader cannot disagree about what the operator asked for.
//
// ⚠ AND THE SEAM IS NOW BEING CLOSED FROM THE PRODUCTS' SIDE, 2026-09-18. Both consumers read the
// generic /data/orbis-env.txt themselves - OpenGothic's game/main.cpp and RetroArch's
// platform_orbis.c, both before their own file - and both shipped example files now tell the
// operator to write that name. The three product paths below are deprecated from today and stay
// only for packages already flashed; the delete condition is spelled out at each of them.
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

// Bounded on purpose: these files are hand-written experiment configuration, and a file long
// enough to overflow this is a file somebody should look at rather than one this should grow for.
constexpr int   kMaxEntries = 64;
constexpr int   kMaxLine    = 512;

struct Entry {
  char key[64];
  char val[256];
  };

Entry g_entry[kMaxEntries];
int   g_count  = 0;
bool  g_loaded = false;

char* trim_front(char* s) {
  while (*s == ' ' || *s == '\t')
    s++;
  return s;
  }

void trim_back(char* s) {
  char* end = s + std::strlen(s);
  while (end > s) {
    const char c = end[-1];
    if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
      break;
    *--end = '\0';
    }
  }

void store(const char* key, const char* val) {
  // A later file overrides an earlier one, which is the order the frontend applies them in.
  for (int i = 0; i < g_count; i++) {
    if (std::strcmp(g_entry[i].key, key) == 0) {
      std::snprintf(g_entry[i].val, sizeof(g_entry[i].val), "%s", val);
      return;
      }
    }
  if (g_count >= kMaxEntries)
    return;
  std::snprintf(g_entry[g_count].key, sizeof(g_entry[g_count].key), "%s", key);
  std::snprintf(g_entry[g_count].val, sizeof(g_entry[g_count].val), "%s", val);
  g_count++;
  }

// Same grammar as RetroArch's frontend_orbis_apply_env_file: KEY=VALUE, '#' comments, blank
// lines ignored, whitespace trimmed on BOTH sides of the '=' - the format's own documentation
// warns that a trailing space "would otherwise read as a different experiment".
void load_file(const char* path) {
  std::FILE* f = std::fopen(path, "r");
  if (!f)
    return;

  char line[kMaxLine];
  while (std::fgets(line, sizeof(line), f)) {
    char* key = trim_front(line);
    if (*key == '#' || *key == '\n' || *key == '\r' || *key == '\0')
      continue;

    char* eq = std::strchr(key, '=');
    if (!eq)
      continue;
    *eq = '\0';

    char* val = trim_front(eq + 1);
    trim_back(key);
    trim_back(val);
    if (*key)
      store(key, val);
    }

  std::fclose(f);
  }

void load_once() {
  if (g_loaded)
    return;
  g_loaded = true;
  // ⚠ A GENERIC FILE FIRST, BECAUSE THE LIST BELOW ONLY KNOWS THREE CONSUMERS BY NAME. Every
  // switch this overlay has - ORBIS_THREAD_STACK, ORBIS_SIGEV_THREAD, ORBIS_UMTX_LIBKERNEL - is
  // meant to be flipped on a console without rebuilding, and that is how every measurement in the
  // README was made. A consumer this file has never heard of could not do it at all: it would read
  // three paths belonging to other programs and find nothing.
  //
  // Found by running the bundle's own hello example on hardware, 2026-09-17. It crashed with
  // SIGSYS inside pthread_create and there was no way to turn the thread interposer off to see
  // whose fault it was - the one question an A/B answers in one run.
  //
  // First, so a program with a file of its own still overrides this one.
  load_file("/data/orbis-env.txt");

  // ⚠ THE THREE BELOW ARE DEPRECATED AS OF 2026-09-18 AND MUST NOT BE DELETED YET. Both products
  // were moved onto the generic name above on that date - OpenGothic reads it first in
  // game/main.cpp, RetroArch applies it first in frontend_orbis_init, and OpenGothic's
  // ps4/tempest-env.example.txt (the format's normative description, which RetroArch's own comment
  // points at) now tells the operator to write /data/orbis-env.txt. None of that reaches a console
  // that already has a .pkg on it: the packages in people's hands read the old names, and the
  // operator's file sits on /data where no reinstall touches it.
  //
  // DELETE WHEN, AND NOT BEFORE: a RELEASED OpenGothic package and a RELEASED RetroArch package
  // both write and read the generic file. Until both exist, dropping a line here turns an
  // operator's existing knob into a silent no-op - the exact failure this whole file was written
  // for (measured 2026-08-23: ORBIS_NCPU=1 applied to the eboot, never reached the core, and the
  // run came back looking like a measurement).
  load_file("/data/tempest-env.txt");    // deprecated 2026-09-18 - OpenGothic packages before that
  load_file("/data/retroarch-env.txt");  // deprecated 2026-09-18 - RetroArch packages before that
  // ⚠ THE DESKTOP-GL EBOOT HAD A FILE OF ITS OWN, and this list not knowing about it was worth a
  // wasted console run: the frontend applied it but a module reading through orbis_env_get would
  // have missed it entirely. Last, so it overrides the shared file, which was the order the
  // frontend applied them in.
  //
  // ⚠ THAT EBOOT IS GONE, and this path is the last thing in the workshop that still names it:
  // checked 2026-09-18, `grep -rn glcore-env` over the RetroArch ps4-support tree hits nothing, and
  // ps4/build-cores.sh records that RTRG00001 and /data/retroarch-glcore/ were both retired. It is
  // kept on the same terms as the two above - a console that ran the desktop-GL package may still
  // have the file - and it goes at the same time they do.
  load_file("/data/retroarch-glcore-env.txt");  // deprecated 2026-09-18 - product itself retired
  }

}  // namespace

extern "C" const char* orbis_env_get(const char* name) {
  if (!name || !*name)
    return nullptr;

  // The real environment first: a value somebody set in this image beats a file, and in the
  // executable this path answers everything before a file is ever opened.
  if (const char* v = std::getenv(name))
    if (*v)
      return v;

  load_once();
  for (int i = 0; i < g_count; i++)
    if (std::strcmp(g_entry[i].key, name) == 0)
      return g_entry[i].val;

  return nullptr;
  }
