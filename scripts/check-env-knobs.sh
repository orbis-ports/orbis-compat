#!/usr/bin/env bash
# Copyright © 2026 Mikołaj Mikołajczyk
# SPDX-License-Identifier: MIT
#
# Fails if anything in src/ reads a knob with getenv() instead of orbis_env_get().
#
#   scripts/check-env-knobs.sh [<dir>]       # default: the repository's src/
#   scripts/check-env-knobs.sh --self-test   # prove the check fails when it should
#
# ⚠ THE FAILURE MODE IS SILENT BY CONSTRUCTION - A SWITCH THAT CANNOT BE THROWN LOOKS EXACTLY LIKE A
# SWITCH WITH NO EFFECT. That is the whole reason this file exists rather than a line in a review
# checklist. A knob read through getenv() in a .prx returns NULL on a console no matter what the
# operator wrote, the code takes its default, the run comes back clean, and the result reads as a
# measurement of the feature being switched.
#
# MEASURED 2026-09-17: two of this overlay's three switches - ORBIS_THREAD_STACK and
# ORBIS_SIGEV_THREAD - read getenv, so the A/B method every number in README §2 and §6 rests on
# worked on the laptop and had never once worked on hardware. ORBIS_INTERNAL_MEM_PROBE always went
# through orbis_env_get, which is exactly why nobody noticed: one of the three did what it said.
#
# The cause is not style, it is linkage, and it is not fixable by being careful. The SDK's libc.a is
# a real static musl - 1481 objects, getenv and setenv as defined text rather than stubs into a
# shared libc module - so an executable and every .prx it loads each link their own copy with its own
# `environ`. MEASURED 2026-08-23: ORBIS_NCPU=1 was written into /data/retroarch-env.txt, the console
# relaunched, and the core still started 5 recompiler workers. The line was applied - to the eboot's
# environ, which is not the environ the core reads. src/orbis_env.cpp answers from well-known FILES
# for that reason; /data/orbis-env.txt is the generic path a program nobody has heard of can use.
#
# ------------------------------------------------------------------ what counts as an offence
#
#   getenv("ORBIS_...")   anywhere in the tree, INCLUDING src/orbis_env.cpp. A hard-coded knob name
#                         is the bug this exists to catch, and the implementation has no business
#                         carrying one either.
#   getenv(anything)      everywhere except inside orbis_env_get(). "The equivalent with a variable"
#                         is the same defect written differently - `const char* e = getenv(k)` reads
#                         the same empty environ - and a check that only matched the string literal
#                         would be trivially walked around by the next person in a hurry.
#
# ⚠ THE EXEMPTION IS A FUNCTION, NOT A FILE. src/orbis_env.cpp legitimately touches the real
# environment - orbis_env_get() tries it FIRST, so a value genuinely set in this image still beats a
# file - but that is one call in one function. Skipping the whole file would make the one place most
# likely to grow a hard-coded ORBIS_ name the one place nothing looks at. Everything outside that
# function's braces is checked like any other file.
#
# ⚠ AND IT IS WRITTEN THE WAY test/crt_abi.sh IS WRITTEN: it must FAIL when pointed at something
# that is not the thing under test, or it is asserting nothing. --self-test builds six small files
# whose correct verdicts are known - including an orbis_env.cpp that carries a SECOND getenv outside
# the exempt function - and requires the check to agree with all six. build.sh and CI run it.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# ------------------------------------------------------------------ comment- and string-aware scan
#
# ⚠ COMMENTS ARE STRIPPED BEFORE ANYTHING IS MATCHED, and that is not tidiness: this repository
# argues in its comments, and the two files that were FIXED on 2026-09-17 both now say "⚠
# orbis_env_get, NOT getenv" in prose. A grep over raw text would fail on the fix and pass on the
# defect. Strings survive the strip, because `getenv("ORBIS_` is the thing being looked for.
#
# Offences are printed as `file:line: rule: text`; the exit status is 0 clean, 1 offences found.
scan() {
  local dir="$1" found=0 f
  while IFS= read -r f; do
    # Blank out comments in place (spaces, so columns and line numbers survive), then decide per line
    # whether it sits inside orbis_env_get's braces. Braces inside string literals are not counted:
    # one of those would open a block that never closes, and the exemption would run to end of file.
    local hits
    hits="$(awk '
      # ⚠ WRITTEN FOR mawk AS WELL AS FOR macOS awk, and the two constructs below are why. `(^|X)`
      # puts an anchor inside an alternation, which POSIX leaves undefined and not every awk takes,
      # so the two halves are separate matches; and a brace is a bracket expression rather than
      # \{, which some awks read as an interval and warn about. CI runs mawk; this laptop does not.
      function calls(s, p) { return (s ~ ("^" p)) || (s ~ ("[^A-Za-z0-9_]" p)) }
      BEGIN {
        SQ = sprintf("%c", 39); inblock = 0; state = 0; depth = 0; seen = 0
        ANY = "getenv[ \t]*\\("                 # getenv(anything) - the variable case included
        LIT = "getenv[ \t]*\\([ \t]*\"ORBIS_"   # getenv("ORBIS_...")
      }
      {
        out = ""; i = 1; n = length($0); instr = 0; inchr = 0
        while (i <= n) {
          c = substr($0, i, 1); d = substr($0, i, 2)
          if (inblock)     { if (d == "*/") { inblock = 0; out = out "  "; i += 2 }
                             else           { out = out " ";  i += 1 }; continue }
          if (instr || inchr) {
            out = out c
            if (c == "\\") { out = out substr($0, i + 1, 1); i += 2; continue }
            if (instr && c == "\"") instr = 0
            if (inchr && c == SQ)   inchr = 0
            i += 1; continue
          }
          if (d == "//") break                    # to end of line
          if (d == "/*") { inblock = 1; out = out "  "; i += 2; continue }
          if (c == "\"") { instr = 1; out = out c; i += 1; continue }
          if (c == SQ)   { inchr = 1; out = out c; i += 1; continue }
          out = out c; i += 1
        }

        # Is this line inside the exempt function? state 0 = not yet reached, 1 = inside,
        # 2 = past it, so a SECOND definition of the same name gets no exemption.
        #
        # ⚠ ONLY IN THE FILE THAT DEFINES IT (envimpl), and that is not belt and braces. Every other
        # file CALLS orbis_env_get, and a call is written `if (const char* v = orbis_env_get(name))
        # {` - which contains the name and an opening brace and is therefore indistinguishable from
        # a definition by shape alone. Matching it anywhere would silently exempt the rest of that
        # block, in exactly the files this check exists to police.
        exempt = 0
        if (envimpl && state == 0 && calls(out, "orbis_env_get[ \t]*\\(") && out ~ /[{]/) {
          state = 1; depth = 0; seen = 0
        }
        if (state == 1) {
          exempt = 1
          bc = out; gsub(/"[^"]*"/, "", bc)
          opens  = gsub(/[{]/, "", bc)
          closes = gsub(/[}]/, "", bc)
          if (opens > 0) seen = 1
          depth += opens - closes
          if (seen && depth <= 0) state = 2
        }

        # ⚠ NO gensub HERE. It is a gawk extension and neither macOS awk nor Ubuntu mawk has it;
        # the check would print an error instead of an offence on every host that matters.
        text = $0; sub(/^[ \t]+/, "", text)
        if (calls(out, LIT))
          printf "%d: knob name hard-coded into getenv: %s\n", FNR, text
        else if (!exempt && calls(out, ANY))
          printf "%d: getenv outside orbis_env_get: %s\n", FNR, text
      }
    ' envimpl="$([[ "$(basename "$f")" == orbis_env.cpp ]] && echo 1 || echo 0)" "$f")" || true
    if [[ -n "${hits}" ]]; then
      found=1
      while IFS= read -r h; do echo "!! ${f#"${dir}"/}:${h}"; done <<<"${hits}"
    fi
  done < <(find "$dir" \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \) | sort)
  return "${found}"
}

# ------------------------------------------------------------------ the check checks itself
#
# Six files, six known verdicts. Two of them are called orbis_env.cpp and only one is meant to pass;
# the sixth is the call-shaped-like-a-definition that an earlier draft of this check got wrong.
self_test() {
  local work rc fails=0
  work="$(mktemp -d)"; trap 'rm -rf "${work}"' RETURN

  mkdir -p "${work}/literal" "${work}/variable" "${work}/prose" "${work}/impl" "${work}/impl_bad" \
           "${work}/caller"
  cat >"${work}/literal/a.cpp" <<'EOF'
void f(void) { const char* v = getenv("ORBIS_THREAD_STACK"); (void)v; }
EOF
  cat >"${work}/variable/b.cpp" <<'EOF'
const char* f(const char* k) { return std::getenv(k); }
EOF
  cat >"${work}/prose/c.cpp" <<'EOF'
// ⚠ orbis_env_get, NOT getenv - setenv() in one image is invisible to another here.
/* getenv("ORBIS_NCPU") in a block comment is prose too. */
const char* f(void) { return orbis_env_get("ORBIS_NCPU"); }
EOF
  cat >"${work}/impl/orbis_env.cpp" <<'EOF'
extern "C" const char* orbis_env_get(const char* name) {
  if (const char* v = std::getenv(name))
    return v;
  return nullptr;
  }
EOF
  cat >"${work}/impl_bad/orbis_env.cpp" <<'EOF'
extern "C" const char* orbis_env_get(const char* name) {
  if (const char* v = std::getenv(name))
    return v;
  return nullptr;
  }
void later(void) { const char* v = getenv(g_key); (void)v; }
EOF

  # ⚠ The shape that made the exemption file-scoped: a CALL that opens a block reads exactly like a
  # definition, and an earlier draft exempted everything up to the closing brace because of it.
  cat >"${work}/caller/orbis_thread.cpp" <<'EOF'
void f(const char* name, const char* k) {
  if (const char* v = orbis_env_get(name)) {
    const char* w = getenv(k);
    (void)v; (void)w;
    }
  }
EOF

  expect() {                                    # expect <0|1> <dir> <what it is>
    rc=0; scan "${work}/$2" >/dev/null 2>&1 || rc=$?
    if [[ "${rc}" -ne "$1" ]]; then
      echo "!! self-test: $3 should exit $1 and exited ${rc}" >&2; fails=1
    fi
  }
  expect 1 literal   'getenv("ORBIS_...")'
  expect 1 variable  'getenv with a variable'
  expect 0 prose     'getenv named in comments only'
  expect 0 impl      "orbis_env_get's own call"
  expect 1 impl_bad  'a second getenv in orbis_env.cpp, outside the exempt function'
  expect 1 caller    'a CALL to orbis_env_get opening a block, in a file that does not define it'

  [[ "${fails}" -eq 0 ]] || { echo "!! the check does not agree with its own known cases" >&2; exit 1; }
  echo "== check-env-knobs self-test: 6/6, including the exemption's own file and a call that looks like it"
}

TARGET="${ROOT}/src"
case "${1:-}" in
  --self-test) self_test; exit 0 ;;
  "")          ;;
  -*)          echo "unknown argument: $1" >&2; exit 2 ;;
  *)           TARGET="$1" ;;
esac

[[ -d "${TARGET}" ]] || { echo "!! no such directory: ${TARGET}" >&2; exit 2; }

if ! scan "${TARGET}"; then
  cat >&2 <<'EOF'
!!
!! Use orbis_env_get() from <orbis_env.h>. getenv() reads THIS image's environ, and on this console
!! every .prx links its own - so the knob above cannot be set from outside the image that contains
!! it, and a run with it "off" is indistinguishable from a run with it on. PLAN.md §11.
EOF
  exit 1
fi
echo "== every ORBIS_* knob in ${TARGET#"${ROOT}"/} is read through orbis_env_get"
