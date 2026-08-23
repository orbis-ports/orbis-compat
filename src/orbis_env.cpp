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
  load_file("/data/tempest-env.txt");
  load_file("/data/retroarch-env.txt");
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
