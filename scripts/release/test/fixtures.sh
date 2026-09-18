#!/usr/bin/env bash
# Fixtures for scripts/release/test/run.sh. Sourced, never executed on its own.
#
# WHY SYNTHETIC INPUTS AT ALL. The three scripts under test are gated by a Mesa bundle
# (three hours to build, ~90 MB unpacked) and an OpenOrbis SDK (289 MB unpacked on this
# machine, measured 2026-09-18). Neither is needed to exercise the LOGIC: make-sdk-bundle.sh
# reads `manifest.txt` with `sed -n 's/^k=//p'` and copies directories, and
# verify-sdk-bundle.sh reads BUNDLE.txt, mesa/manifest.txt, SHA256SUMS and ONE archive. So the
# fixtures give those scripts the real SHAPE - a manifest with the real field names in the real
# order, a bundle layout with every path check 6 reaches for - and stub bytes everywhere the
# content is not read. scripts/release/README.md already recorded this technique working
# ("a complete 145-file bundle was staged, hashed, manifested and tarred (375 KB) from a
# SYNTHETIC SDK and a SYNTHETIC Mesa bundle"); this file is that stand-in written down instead
# of retyped per session.
#
# ⚠ WHAT IS NOT SYNTHETIC, AND MUST NOT BECOME SO:
#
#   build/liborbis-compat.a   the REAL archive from this checkout. verify check 5 reads it with
#                             nm and compares against the import list. Defect 1 (the
#                             `sed 's/^_//'` that truncated `_Znwm`) is only reproducible
#                             against real Itanium-ABI C++ symbols; a stub archive would make
#                             that regression test assert nothing. Measured 2026-09-18: the
#                             archive defines 85 names, 3 of them `_Z...`, and all 12 names in
#                             orbis-mesa-a4fa6b57f8bd's real import list.
#   include/                  the REAL 27-file header tree, because the include/ fingerprint is
#                             the thing under test in half these cases and a two-file stand-in
#                             would hash just as happily while proving less about ordering.
#   licenses/ NOTICE.md       come from the real checkout: make-sdk-bundle.sh takes them from
#   LICENSING.md              $REPO (itself), not from --compat, and the licence gate is a real
#                             gate that a broken ledger must still fail.

# ------------------------------------------------------------------ shared helpers
_sha256(){ if command -v sha256sum >/dev/null 2>&1; then sha256sum "$@"; else shasum -a 256 "$@"; fi; }

# ⚠ DELIBERATELY NOT A COPY OF make-sdk-bundle.sh's _include_sha256. That function is
# NUL-delimited; this one is newline-delimited. Same documented algorithm - "sha256 over the
# concatenation of '<sha256 of file>  <path>\n' for every regular file under include/, paths
# relative to the directory handed in, ordered by LC_ALL=C sort" - written a second way, so
# that case A2 comparing it against what the cut recorded is evidence rather than a tautology.
# The two can only differ for a path containing a newline, and include/ has none.
fx_include_sha256(){
  ( cd "$1" && find include -type f | LC_ALL=C sort \
      | while IFS= read -r f; do _sha256 "$f"; done | _sha256 | cut -d' ' -f1 )
}

# ------------------------------------------------------------------ a synthetic OpenOrbis SDK
#
# Every path make-sdk-bundle.sh stages or verify-sdk-bundle.sh check 6 demands, and nothing
# else. The real tree is 289 MB and 828 of its files are samples/; copying it per test case
# would cost minutes per case and prove only that `cp -a` works.
#
# ⚠ THE TWO CONTENT PROBES ARE REAL PROBES. make-sdk-bundle.sh refuses an SDK that lacks
# link.x AND lib/libc.a by name, because "a source checkout of the toolchain repository has
# link.x and no lib/ - that is a stash, not an SDK". Both files exist here for that reason; a
# fixture that omitted one would be testing the refusal, not the cut.
fx_sdk(){ # fx_sdk <dir>
  local d="$1"
  mkdir -p "$d/lib" "$d/include/orbis" "$d/include/c++/v1" "$d/src/crt" "$d/bin/macos"
  printf 'SECTIONS { . = 0x400000; }\n' > "$d/link.x"
  # >200 bytes so nothing mistakes it for a 404 stub; the licence gate never reads it, but the
  # bundle ships it and SHA256SUMS covers it.
  printf 'OpenOrbis SDK stand-in LICENSE.\n%s\n' "$(head -c 400 /dev/zero | tr '\0' 'x')" > "$d/LICENSE"
  : > "$d/lib/libc.a"; : > "$d/lib/crt1.o"; : > "$d/lib/libSceNet.so"
  printf '#pragma once\n' > "$d/include/orbis/orbis.h"
  printf '#pragma once\n' > "$d/include/c++/v1/cstdlib"
  printf 'int main(void){return 0;}\n' > "$d/src/crt/crtlib.c"
  printf 'the corresponding source for crtlib.o\n' > "$d/src/README.md"
  printf '#!/bin/sh\nexit 0\n' > "$d/bin/macos/create-fself"; chmod +x "$d/bin/macos/create-fself"
}

# ------------------------------------------------------------------ a synthetic orbis-compat
#
# A REAL git repository, because make-sdk-bundle.sh asks git four questions about it -
# rev-parse HEAD, diff --quiet HEAD, cat-file -e <sha>^{tree}, diff --quiet A B -- include/ -
# and the pairing matrix is exactly the matrix of answers to the last two. The real checkout
# cannot serve: it is dirty right now, its history is fixed, and a test that had to commit to
# it would be a test nobody dares run.
#
# Three commits, and each one exists to make one row of the matrix reachable:
#   A  the base.
#   B  README.md only - `git diff A B -- include/` is EMPTY. This is the shape that cost a
#      rebuild per README typo under the old commit-id rule (make-sdk-bundle.sh measured it
#      2026-09-17 over cc75949..4f61d6f) and the shape pairing-basis=include-identical exists
#      to accept.
#   C  include/orbis_prefix.h changed - the pair that must still be refused.
# Writes "$d/.fx-shas" as three lines A B C.
fx_compat(){ # fx_compat <dir> <real-repo>
  local d="$1" repo="$2"
  mkdir -p "$d"
  cp -a "$repo/include" "$d/include"
  cp -a "$repo/cmake"   "$d/cmake"
  cp -a "$repo/scripts" "$d/scripts"
  cp -a "$repo/optional" "$d/optional" 2>/dev/null || true
  cp -a "$repo/crt" "$d/crt" 2>/dev/null || true
  cp -a "$repo/LICENSE" "$repo/README.md" "$d/"
  mkdir -p "$d/build"
  # ⚠ THE REAL ARCHIVE. See the header: check 5 and defect 1 are unreachable without it.
  cp -a "$repo/build/liborbis-compat.a" "$d/build/"
  ( cd "$d"
    git init -q .
    git config user.email test@example.invalid
    git config user.name  "release test"
    git config commit.gpgsign false
    # build/ is normally ignored; the bundle needs it staged, and the pairing gate needs a
    # clean `git diff --quiet HEAD`, so it is committed here on purpose.
    git add -A -f >/dev/null
    git commit -qm "A: the base tree"
    git rev-parse HEAD > .fx-a
    printf '\na typo fixed in prose.\n' >> README.md
    git add -A -f >/dev/null; git commit -qm "B: README only - include/ untouched"
    git rev-parse HEAD > .fx-b
    printf '\n/* a header change, which is what must refuse */\n' >> include/orbis_prefix.h
    git add -A -f >/dev/null; git commit -qm "C: include/ changed"
    git rev-parse HEAD > .fx-c
    { cat .fx-a; cat .fx-b; cat .fx-c; } > .fx-shas
    rm -f .fx-a .fx-b .fx-c
    git checkout -q "$(sed -n 1p .fx-shas)" )
}

# ⚠ -f AND `git clean`, BOTH. A plain `git checkout <sha>` while already detached at that sha
# keeps local modifications - which is correct git behaviour and exactly wrong here: a case that
# deliberately dirtied the tree would leave every case after it running against a dirty fixture,
# and the pairing gate would then refuse cuts for a reason the case never asked about. Found
# 2026-09-18 by the first run of this suite: D1 dirtied include/, and E2/E3 were still carrying
# it three cases later.
fx_compat_at(){ # fx_compat_at <dir> <a|b|c>  -> detached at that commit, tree clean
  local d="$1" n
  case "$2" in a) n=1 ;; b) n=2 ;; c) n=3 ;; *) return 2 ;; esac
  ( cd "$d" && git checkout -q -f "$(sed -n "${n}p" .fx-shas)" && git clean -qfd -e .fx-shas )
}
fx_compat_sha(){ # fx_compat_sha <dir> <a|b|c>
  local n; case "$2" in a) n=1 ;; b) n=2 ;; c) n=3 ;; *) return 2 ;; esac
  sed -n "${n}p" "$1/.fx-shas"
}

# ------------------------------------------------------------------ a synthetic Mesa bundle
#
# The manifest is the real one from orbis-mesa-a4fa6b57f8bd (read out of a stage left on this
# machine 2026-09-18) with two changes: the orbis-compat-commit is whichever the case needs,
# and orbis-compat-include-sha256 - the field mesa-ps4's release.yml started writing 2026-09-18
# - is present, absent or wrong on demand. The FIELD ORDER MATTERS: both scripts read the
# import list as "everything after [orbis-compat-imports]", so every key=value has to come
# before it.
#
# The five archives and include/vulkan exist because verify check 6 asserts them by name and by
# glob; their bytes are never read by anything under test.
fx_mesa(){ # fx_mesa <dir> <orbis-compat-commit> <include-sha|none> [imports-file]
  local d="$1" commit="$2" incsha="$3" imports="${4:-}"
  mkdir -p "$d/include/vulkan" \
           "$d/build-orbis/src/amd/vulkan" "$d/build-orbis/src/egl" \
           "$d/build-orbis/src/gallium/targets/dri" "$d/build-orbis/subprojects/zlib-1.3.1"
  printf '#pragma once\n' > "$d/include/vulkan/vulkan.h"
  : > "$d/build-orbis/src/amd/vulkan/libvulkan_radeon.a"
  : > "$d/build-orbis/src/egl/libEGL.a"
  : > "$d/build-orbis/src/gallium/targets/dri/libgallium-26.3.0-devel.a"
  : > "$d/build-orbis/subprojects/zlib-1.3.1/libz.a"
  {
    echo "bundle=orbis-mesa-a4fa6b57f8bd"
    echo "mesa-commit=a4fa6b57f8bddfb86d4adaba3260fa39002c6430"
    echo "mesa-version=26.3.0-devel"
    echo "mesa-ref=orbis-mesa-a4fa6b57f8bd"
    echo "orbis-compat-commit=$commit"
    echo "openorbis-sdk-tag=v0.5.4"
    echo "gallium-archive=build-orbis/src/gallium/targets/dri/libgallium-26.3.0-devel.a"
    echo "gllinkprobe=pass"
    echo "built=2026-09-18T00:00:00Z"
    [ "$incsha" = none ] || echo "orbis-compat-include-sha256=$incsha"
    echo "[orbis-compat-imports]"
    if [ -n "$imports" ]; then cat "$imports"; else fx_imports_default; fi
  } > "$d/manifest.txt"
}

# The real list orbis-mesa-a4fa6b57f8bd records, verbatim. All twelve are defined by this
# checkout's build/liborbis-compat.a (checked 2026-09-18 with /usr/bin/nm), and the first three
# are the Itanium-ABI names defect 1 truncated.
fx_imports_default(){
  cat <<'EOF'
_Znam
_Znwm
_ZnwmRKSt9nothrow_t
clock_gettime
fstat
fstatat
open
orbis_sysconf
pthread_create
rename
stat
unlink
EOF
}

# ------------------------------------------------------------------ post-cut tampering
#
# ⚠ A TAMPER CASE THAT DOES NOT REFRESH SHA256SUMS TESTS CHECK 1 AND NOTHING ELSE. Every edit
# to an unpacked bundle breaks that file's recorded hash, verify fails at check 1 with rc=1,
# and a case aiming at check 4b would "pass" without check 4b ever having an opinion. That is
# the same conflation defect 2 shipped through, one level down. So: cases that aim at a later
# check re-hash first, and exactly one case (C5) leaves SHA256SUMS alone on purpose, to prove
# check 1 still bites.
#
# Byte-for-byte the generation in make-sdk-bundle.sh section 6, including the NUL delimiter.
fx_resum(){ # fx_resum <unpacked-bundle-dir>
  ( cd "$1" && find . -type f ! -name SHA256SUMS -print0 | LC_ALL=C sort -z \
      | while IFS= read -r -d '' f; do _sha256 "$f"; done ) > "$1/SHA256SUMS"
}

# Unpack a cut tarball into a fresh directory and echo the bundle root.
fx_unpack(){ # fx_unpack <tarball> <into-dir>
  mkdir -p "$2"; tar -xzf "$1" -C "$2"
  dirname "$(find "$2" -maxdepth 2 -name BUNDLE.txt -print -quit)"
}
