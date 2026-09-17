#!/usr/bin/env bash
# Cut ONE versioned, licence-complete PS4 SDK bundle: everything a stranger needs to port
# their own software, in one tarball, pointed at by one CMake toolchain file.
#
# WHAT A CONSUMER HAS TO DO TODAY, AND WHY THIS EXISTS. Fetch the OpenOrbis SDK by release
# tag; clone orbis-compat at a commit sha; build it; fetch a Mesa bundle by tag; export four
# environment variables; and copy a ~330-line GitHub composite action. That action has been
# copy-pasted into three repositories and they have drifted - measured 2026-09-17:
#
#   OpenGothic/.github/workflows/ps4.yml:52     ORBIS_COMPAT_REF 44934e4  (= HEAD)
#   RetroArch/.github/workflows/cores.yml:117   ORBIS_COMPAT_REF ab4c066  (= HEAD~1)
#   mesa-ps4/.github/workflows/release.yml:49   ORBIS_COMPAT_REF 2460acb  <- Mesa was BUILT here
#
# Between 2460acb and HEAD there are 14 commits and 362 changed lines under include/ -
# signal.h +135, sys/umtx.h +48, orbis_thread.h +56. Mesa compiled against the old headers
# and links against the new archive. After this script, a bundle is a single pinned thing and
# that class of drift cannot be expressed.
#
# =========================================================================================
# ⚠ MESA AND ORBIS-COMPAT ARE FROZEN TOGETHER IN HERE, AND mesa-ps4 DELIBERATELY DOES THE
#   OPPOSITE. Read its release.yml header first; it is not wrong, it is answering a
#   different question.
#
#   Its words: "orbis-compat IS NOT IN THE TARBALL, and that is the point of the manifest.
#   Mesa's archives carry undefined references only orbis-compat satisfies, but they are
#   resolved at the FINAL link, where -lorbis-compat is already on the line. Bundling a copy
#   would freeze the layer that changes most often inside an artifact meant to change least.
#   What ships instead is the list of names Mesa imports from it, so a consumer can assert
#   the interface without pinning the commit."
#
#   That is correct FOR MESA'S OWN ARTIFACT. mesa-ps4 publishes a component to be consumed by
#   a build that also has its own orbis-compat, and freezing one inside the other would make
#   every overlay fix require a three-hour Mesa rebuild. The looseness is the feature.
#
#   It is wrong FOR A REDISTRIBUTABLE SDK, for one reason the import list cannot reach. That
#   assertion checks that every name Mesa imports is STILL DEFINED - its own comment says so:
#   "presence, not meaning. Constants inlined at Mesa's compile time leave no symbol to check
#   at all." Between 2460acb and HEAD, `include/signal.h` grew 135 lines and `sys/umtx.h` 48.
#   A struct that changed size, a macro that changed value, a `sigevent` whose fields moved -
#   none of those move a symbol NAME, so none of them fail that gate, and all of them are
#   silent ABI breaks in a driver that compiled cleanly. mesa-ps4 can accept that risk
#   because its consumer is a build it controls. A stranger downloading one tarball cannot
#   accept it, cannot detect it, and would have no idea which half to suspect.
#
#   So: this bundle refuses to be cut unless the Mesa bundle's own manifest says it was built
#   against exactly the orbis-compat commit going in beside it. --allow-pair-mismatch exists
#   for a maintainer who knows better on a given day; it stamps BUNDLE.txt so loudly that
#   verify-sdk-bundle.sh fails on it afterwards.
# =========================================================================================
#
# Usage:
#   scripts/release/make-sdk-bundle.sh --version v1 \
#       [--sdk DIR] [--mesa DIR] [--compat DIR] [--out DIR] \
#       [--bin-hosts linux,macos] [--allow-pair-mismatch] [--allow-dirty] [--dry-run]
#
#   --version   the bundle version. Goes in the name and in BUNDLE.txt. Required.
#   --sdk       an UNPACKED OpenOrbis SDK (link.x AND lib/libc.a). Default $OO_PS4_TOOLCHAIN.
#   --mesa      an UNPACKED orbis-mesa-* bundle: the directory holding manifest.txt.
#   --compat    this repository. Default: the checkout this script lives in.
#   --out       where the tarball is written. Default $ORBIS_WORK/release, else ./dist.
#   --dry-run   stage and report, write no tarball.
#
# Exit codes: 0 ok · 1 refused (a check failed) · 2 usage.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
REPO="$(cd "$HERE/../.." && pwd -P)"

VERSION=""
SDK="${OO_PS4_TOOLCHAIN:-$HOME/.local/opt/openorbis}"
MESA=""
COMPAT="$REPO"
OUT="${ORBIS_WORK:-}"
BIN_HOSTS="linux,macos"
ALLOW_MISMATCH=0
ALLOW_DIRTY=0
DRYRUN=0

log(){  printf '\033[1;34m==>\033[0m %s\n' "$*"; }
warn(){ printf '\033[1;33m!!\033[0m %s\n' "$*" >&2; }
die(){  printf '\033[1;31mXX\033[0m %s\n' "$*" >&2; exit 1; }
_sha256(){ if command -v sha256sum >/dev/null 2>&1; then sha256sum "$@"; else shasum -a 256 "$@"; fi; }
_size(){ if stat -c%s "$1" >/dev/null 2>&1; then stat -c%s "$1"; else stat -f%z "$1"; fi; }

while [ $# -gt 0 ]; do
  case "$1" in
    --version)  VERSION="$2"; shift 2 ;;
    --sdk)      SDK="$2"; shift 2 ;;
    --mesa)     MESA="$2"; shift 2 ;;
    --compat)   COMPAT="$2"; shift 2 ;;
    --out)      OUT="$2"; shift 2 ;;
    --bin-hosts) BIN_HOSTS="$2"; shift 2 ;;
    --allow-pair-mismatch) ALLOW_MISMATCH=1; shift ;;
    --allow-dirty) ALLOW_DIRTY=1; shift ;;
    --dry-run)  DRYRUN=1; shift ;;
    -h|--help)  sed -n '1,60p' "$0"; exit 0 ;;
    *) echo "unknown argument: $1" >&2; exit 2 ;;
  esac
done
[ -n "$VERSION" ] || { echo "usage: $0 --version <v> [--sdk DIR] [--mesa DIR] ..." >&2; exit 2; }
[ -n "$OUT" ] && OUT="$OUT/release" || OUT="$PWD/dist"

# -----------------------------------------------------------------------------------------
# 1. The inputs, checked BY CONTENT. A directory that is present but is not the thing fails
#    hundreds of files later with an error naming nothing - which is the same reasoning
#    orbis-env.sh gives for probing link.x rather than the directory.
# -----------------------------------------------------------------------------------------
log "inputs"
[ -f "$SDK/link.x" ] && [ -f "$SDK/lib/libc.a" ] || die \
  "SDK at '$SDK' has no link.x AND lib/libc.a. A source checkout of the toolchain repository
   has link.x and no lib/ - that is a stash, not an SDK. OO_PS4_TOOLCHAIN on a developer
   machine is quite likely pointing at one. Unpack toolchain-llvm-18.tar.gz instead."
[ -f "$COMPAT/cmake/ps4-openorbis.cmake" ] || die "'$COMPAT' is not an orbis-compat checkout"
[ -f "$COMPAT/build/liborbis-compat.a" ] || die \
  "no '$COMPAT/build/liborbis-compat.a'. Run ./build.sh first - and do NOT pass --no-check:
   a silently wrong overlay is exactly what a bundle would then distribute."
[ -n "$MESA" ] || die "--mesa <unpacked orbis-mesa bundle> is required (the directory with manifest.txt)"
[ -f "$MESA/manifest.txt" ] || die "'$MESA' has no manifest.txt - it cannot name its own parents"

COMPAT_SHA="$(git -C "$COMPAT" rev-parse HEAD 2>/dev/null || echo unknown)"
COMPAT_DIRTY=no
if ! git -C "$COMPAT" diff --quiet HEAD 2>/dev/null; then COMPAT_DIRTY=yes; fi
if [ "$COMPAT_DIRTY" = yes ] && [ "$ALLOW_DIRTY" -eq 0 ]; then
  die "the orbis-compat working tree is dirty. A bundle whose orbis-compat commit does not
   describe its bytes is a bundle nobody can reproduce. Commit, or pass --allow-dirty and
   accept that BUNDLE.txt will say so."
fi

mesa_field(){ sed -n "s/^$1=//p" "$MESA/manifest.txt" | head -1; }
MESA_BUNDLE="$(mesa_field bundle)"
MESA_COMMIT="$(mesa_field mesa-commit)"
MESA_VERSION="$(mesa_field mesa-version)"
MESA_BUILT_AGAINST="$(mesa_field orbis-compat-commit)"
MESA_SDK_TAG="$(mesa_field openorbis-sdk-tag)"
MESA_LINKPROBE="$(mesa_field gllinkprobe)"

printf '   %-26s %s\n' "SDK"                  "$SDK"
printf '   %-26s %s (%s, dirty=%s)\n' "orbis-compat" "$COMPAT" "${COMPAT_SHA:0:12}" "$COMPAT_DIRTY"
printf '   %-26s %s (mesa %s @ %s)\n' "Mesa bundle" "$MESA_BUNDLE" "$MESA_VERSION" "${MESA_COMMIT:0:12}"
printf '   %-26s %s\n' "Mesa built against"   "${MESA_BUILT_AGAINST:0:12}"
printf '   %-26s %s\n' "Mesa link probe"      "${MESA_LINKPROBE:-unrecorded}"

# -----------------------------------------------------------------------------------------
# 2. THE PAIRING GATE. See the header. This is the one check that makes a bundle different
#    in kind from the four env vars it replaces.
# -----------------------------------------------------------------------------------------
PAIR=ok
# ⚠ WHAT MESA COMPILES AGAINST IS include/, AND THIS USED TO COMPARE COMMIT IDS. Any commit to
# this repository invalidated the pair - a typo fixed in a README, a comment rewritten, a change
# to THIS SCRIPT - because the test was sha equality rather than anything Mesa can observe.
#
# Measured 2026-09-17: a bundle was refused over cc75949..4f61d6f, three commits touching
# LICENSING.md, build.sh, crt/orbis_crt1.c and make-sdk-bundle.sh. `git diff -- include/` across
# that range is EMPTY. Under the old rule a release meant freezing this repository, rebuilding
# Mesa, and cutting the bundle with not one commit in between - which is not a process anybody
# would follow twice.
#
# So: the shas are still recorded, and they are still what BUNDLE.txt reports, because provenance
# is worth having. What REFUSES is a difference in include/.
#
# ⚠ AND WHEN THE COMPARISON CANNOT BE MADE, THE OLD RULE STILL APPLIES. A shallow clone resolves
# the old commit object without its tree, so there is nothing to diff; refusing on the sha is then
# the honest answer, and the message says which case it is. The workflow fetches full history so
# that this is the rare path rather than the normal one.
HEADERS_DIFFER=unknown
if [ "$MESA_BUILT_AGAINST" = "$COMPAT_SHA" ]; then
  HEADERS_DIFFER=no
elif git -C "$COMPAT" cat-file -e "${MESA_BUILT_AGAINST}^{tree}" 2>/dev/null; then
  if git -C "$COMPAT" diff --quiet "$MESA_BUILT_AGAINST" "$COMPAT_SHA" -- include/ 2>/dev/null; then
    HEADERS_DIFFER=no
  else
    HEADERS_DIFFER=yes
  fi
fi

if [ "$HEADERS_DIFFER" = no ] && [ "$MESA_BUILT_AGAINST" != "$COMPAT_SHA" ]; then
  warn "Mesa was built against orbis-compat ${MESA_BUILT_AGAINST:0:12}, this bundle ships ${COMPAT_SHA:0:12}"
  log  "include/ is identical across those commits - the pair is coherent"
fi

if [ "$HEADERS_DIFFER" != no ]; then
  PAIR="MISMATCH"
  warn "Mesa was built against orbis-compat ${MESA_BUILT_AGAINST:0:12}"
  warn "this bundle would ship orbis-compat   ${COMPAT_SHA:0:12}"
  # ⚠ rev-parse --verify IS NOT ENOUGH, AND THIS PRINTED BLANKS THE FIRST TIME IT FIRED IN CI:
  #
  #     !! between them: ? commits,  under include/
  #
  # A CI checkout is shallow. The old commit OBJECT can resolve while its ancestry is absent, so
  # --verify passes and rev-list has no range to walk. The drift figure is the most useful line
  # this warning has, and it is needed exactly when the warning fires - so compute it first and
  # say plainly when it cannot be computed, rather than printing a question mark and an empty gap.
  _n="$(git -C "$COMPAT" rev-list --count "$MESA_BUILT_AGAINST..$COMPAT_SHA" 2>/dev/null || true)"
  if [ -n "$_n" ]; then
    _d="$(git -C "$COMPAT" diff --shortstat "$MESA_BUILT_AGAINST..$COMPAT_SHA" -- include/ 2>/dev/null | tr -d '\n')"
    warn "between them: $_n commits,${_d:- no change} under include/"
  else
    warn "between them: cannot say - ${MESA_BUILT_AGAINST:0:12} is not reachable in this checkout"
    warn "  (a shallow clone resolves the object but not its ancestry; git fetch --unshallow to see)"
  fi
  if [ "$HEADERS_DIFFER" = unknown ]; then
    warn "include/ could not be compared - ${MESA_BUILT_AGAINST:0:12} has no tree in this clone."
    warn "  Refusing on the commit id, which is the old and stricter rule. git fetch --unshallow."
  fi
  [ "$ALLOW_MISMATCH" -eq 1 ] || die \
"REFUSED. Mesa compiled against one set of headers and would link against a different
   archive. The import-list assertion the sibling repositories carry does NOT catch this -
   its own comment says it checks 'presence, not meaning. Constants inlined at Mesa's compile
   time leave no symbol to check at all.' A struct that changed size does not change a symbol
   name. Rebuild the Mesa bundle against ${COMPAT_SHA:0:12}, or pass --allow-pair-mismatch
   and accept that verify-sdk-bundle.sh will then FAIL on the result by design."
  warn "--allow-pair-mismatch given; BUNDLE.txt will record pairing=MISMATCH and verify WILL fail"
fi
[ "$PAIR" = ok ] && log "pairing ok: Mesa and orbis-compat are the same pair"

# -----------------------------------------------------------------------------------------
# 3. Licences BEFORE files. If the ledger cannot be satisfied there is nothing to ship.
# -----------------------------------------------------------------------------------------
log "licence gate"
OO_PS4_TOOLCHAIN="$SDK" "$HERE/sdk-licenses.sh" verify \
  || die "the licence ledger does not verify. Run scripts/release/sdk-licenses.sh fetch,
   then sums, then notice. Nothing ships until NOTICE.md matches the table that generates it."

# -----------------------------------------------------------------------------------------
# 4. Stage.
#
#    WHAT IS PRUNED OUT OF THE SDK, AND WHY. The unpacked tarball is ~1.1 GB, and 828 of its
#    files are samples/, none of which a consumer of THIS bundle compiles. Dropped:
#    samples/ docs/ tests/ extra/ logo.png Dockerfile CHANGELOG.md CONTRIBUTING.md
#    src/modules/ (prebuilt .prx demos and Visual Studio projects), and every bin/<host> not
#    named in --bin-hosts.
#    ⚠ KEPT ALTHOUGH IT LOOKS DROPPABLE: src/crt/. Four files - build.bat, build.sh, crtlib.c,
#    crtlib.S - and they are the corresponding source for crtlib.o, the ONE crt object in this
#    SDK that is GPL-3.0 (the other six are musl's and MIT; LICENSING.md §2.9 has the objdump
#    that separates them). Pruning it as "just build scripts" turns a compliant redistribution
#    into a non-compliant one under GPL-3.0 §6. src/README.md goes with it because it is the
#    sentence that says which object the directory belongs to.
# -----------------------------------------------------------------------------------------
NAME="orbis-sdk-${VERSION}"
STAGE="$(mktemp -d)/$NAME"
trap 'rm -rf "$(dirname "$STAGE")"' EXIT
log "staging $NAME"
mkdir -p "$STAGE"/{sdk,orbis-compat,mesa,toolchain,licenses}

# --- the SDK ---
cp -a "$SDK/link.x" "$SDK/LICENSE" "$STAGE/sdk/"
cp -a "$SDK/include" "$STAGE/sdk/include"
mkdir -p "$STAGE/sdk/lib"
# Everything in lib/ except nothing: the archives, the crt objects and all ~422 import stubs.
# The stubs are what make -lSceNet and friends resolve, and their licence is unresolved (see
# NOTICE.md) - they are shipped with that stated, not quietly.
cp -a "$SDK/lib/." "$STAGE/sdk/lib/"
mkdir -p "$STAGE/sdk/src"
[ -d "$SDK/src/crt" ] && cp -a "$SDK/src/crt" "$STAGE/sdk/src/crt"
[ -f "$SDK/src/README.md" ] && cp -a "$SDK/src/README.md" "$STAGE/sdk/src/"
IFS=, read -r -a _hosts <<< "$BIN_HOSTS"
for h in "${_hosts[@]}"; do
  [ -d "$SDK/bin/$h" ] || { warn "no bin/$h in the SDK - skipped"; continue; }
  mkdir -p "$STAGE/sdk/bin/$h"; cp -a "$SDK/bin/$h/." "$STAGE/sdk/bin/$h/"
done

# --- orbis-compat: exactly what ORBIS_COMPAT_DIR has to be for a consumer ---
# ⚠ THE CHECKOUT IS NEEDED AS WELL AS THE ARCHIVE. A consumer's CMakeLists compiles
# optional/ps4_app.cpp and optional/orbis_netlog.cpp out of it, adds vkloader/ by name,
# includes cmake/orbis-compat.cmake, links against cmake/orbis-tls.ld and packages through
# scripts/ps4/make-pkg.sh. Shipping only build/liborbis-compat.a would break all five.
# crt/ is in this list because ORBIS_CRT=own reaches for build/crt/crt1.o and a consumer who
# selects it must be able to rebuild those objects; src/ and test/ are here because the MIT
# terms are easier to honour with the source present than by pointing at a repository.
for d in include cmake optional vkloader scripts test src crt; do
  [ -d "$COMPAT/$d" ] && cp -a "$COMPAT/$d" "$STAGE/orbis-compat/$d"
done
cp -a "$COMPAT/LICENSE" "$COMPAT/README.md" "$STAGE/orbis-compat/"
[ -f "$COMPAT/build.sh" ] && cp -a "$COMPAT/build.sh" "$STAGE/orbis-compat/"
mkdir -p "$STAGE/orbis-compat/build"
cp -a "$COMPAT/build/liborbis-compat.a" "$STAGE/orbis-compat/build/"
# ⚠ THE BUILT crt OBJECTS, WHEN THEY EXIST. ORBIS_CRT defaults to `sdk` and most builds will
# never look here; a consumer that sets ORBIS_CRT=own gets a link error naming the path it
# wanted if they are absent, which is the failure ps4-openorbis.cmake deliberately chose over
# a configure-time refusal. Copying them when they are there costs five files.
[ -d "$COMPAT/build/crt" ] && cp -a "$COMPAT/build/crt" "$STAGE/orbis-compat/build/crt"

# --- Mesa: the bundle verbatim, manifest included ---
# ⚠ COPIED WHOLE AND UNCHANGED, manifest.txt and all. That manifest is the only record of
# which orbis-compat Mesa saw, and this bundle's own pairing check reads it. Rewriting it
# here would destroy the evidence the check rests on.
cp -a "$MESA/." "$STAGE/mesa/"

# --- licences ---
cp -a "$REPO/licenses/." "$STAGE/licenses/"
cp -a "$REPO/NOTICE.md" "$STAGE/NOTICE.md"
cp -a "$REPO/LICENSING.md" "$STAGE/LICENSING.md"

# --- the one file a consumer points CMake at, plus the meson equivalent ---
cp -a "$HERE/templates/orbis-sdk.cmake" "$STAGE/toolchain/orbis-sdk.cmake"
cp -a "$COMPAT/cmake/orbis-tls.ld"      "$STAGE/toolchain/orbis-tls.ld"
# The meson cross file, substituted for THIS bundle's layout. The template is the repository's
# own cmake/orbis.ini.in, so the two stay one file and cannot drift apart.
mkdir -p "$STAGE/toolchain/cross"
sed -e 's|@OO_PS4_TOOLCHAIN@|__BUNDLE__/sdk|g' \
    -e 's|@ORBIS_COMPAT@|__BUNDLE__/orbis-compat|g' \
    -e 's|@ORBIS_CROSS@|__BUNDLE__/toolchain/cross|g' \
    "$COMPAT/cmake/orbis.ini.in" > "$STAGE/toolchain/orbis.ini.in"
mkdir -p "$STAGE/toolchain/cross/lib/pkgconfig"
: > "$STAGE/toolchain/cross/lib/pkgconfig/.keep"
cp -a "$HERE/templates/env.sh"          "$STAGE/env.sh"
cp -a "$HERE/templates/bundle-README.md" "$STAGE/README.md"
# The offline gate travels INSIDE the bundle, so a consumer can check what they downloaded
# without cloning anything.
cp -a "$HERE/verify-sdk-bundle.sh"      "$STAGE/verify.sh"
cp -a "$HERE/sdk-licenses.sh"           "$STAGE/toolchain/sdk-licenses.sh"
chmod +x "$STAGE/env.sh" "$STAGE/verify.sh"

# -----------------------------------------------------------------------------------------
# 5. BUNDLE.txt - the manifest. Same key=value shape as mesa-ps4's manifest.txt, on purpose:
#    every tool in this organisation already parses that shape with `sed -n 's/^k=//p'`.
# -----------------------------------------------------------------------------------------
log "manifest"
SDK_TAG="${MESA_SDK_TAG:-unknown}"
{
  echo "bundle=$NAME"
  echo "version=$VERSION"
  echo "built=$(date -u '+%Y-%m-%dT%H:%M:%SZ')"
  echo "built-by=$(uname -srm)"
  echo "generator=scripts/release/make-sdk-bundle.sh"
  echo
  echo "# --- inputs, each pinned by what identifies it ---"
  echo "openorbis-sdk-tag=$SDK_TAG"
  echo "openorbis-sdk-asset=toolchain-llvm-18.tar.gz"
  echo "openorbis-sdk-linkx-sha256=$(_sha256 "$SDK/link.x" | cut -d' ' -f1)"
  echo "openorbis-sdk-libc-sha256=$(_sha256 "$SDK/lib/libc.a" | cut -d' ' -f1)"
  echo "orbis-compat-commit=$COMPAT_SHA"
  echo "orbis-compat-dirty=$COMPAT_DIRTY"
  echo "orbis-compat-archive-sha256=$(_sha256 "$COMPAT/build/liborbis-compat.a" | cut -d' ' -f1)"
  echo "mesa-bundle=$MESA_BUNDLE"
  echo "mesa-commit=$MESA_COMMIT"
  echo "mesa-version=$MESA_VERSION"
  echo "mesa-built-against-orbis-compat=$MESA_BUILT_AGAINST"
  echo "mesa-gllinkprobe=${MESA_LINKPROBE:-unrecorded}"
  echo
  echo "# --- the pairing. See make-sdk-bundle.sh's header for why this bundle freezes the"
  echo "# two halves together while mesa-ps4's own artifact deliberately does not. ---"
  echo "pairing=$PAIR"
  echo
  echo "# --- the publication gate. UNPROVEN until bundle-gate.sh has run against an UNPACKED"
  echo "# copy of this tarball on a host with ld.lld, llvm-ar, llvm-ranlib and llvm-nm. A"
  echo "# bundle carrying gate=unproven is not publishable. ---"
  echo "gate=${ORBIS_BUNDLE_GATE:-unproven}"
  echo
  echo "# --- what Mesa imports from orbis-compat, carried through from the Mesa bundle's own"
  echo "# manifest so the interface assertion still works inside this one. ---"
  echo "[orbis-compat-imports]"
  sed -n '/^\[orbis-compat-imports\]/,$p' "$MESA/manifest.txt" | tail -n +2 | sed '/^[[:space:]]*$/d'
} > "$STAGE/BUNDLE.txt"

# -----------------------------------------------------------------------------------------
# 6. SHA256SUMS over every file in the bundle. This is what makes "a component's sha256 does
#    not match" detectable at all, and it is written last so it covers everything above it.
# -----------------------------------------------------------------------------------------
log "sha256 over the staged tree"
# ⚠ NUL-DELIMITED, because the SDK ships filenames with spaces in them - extra/ alone has
# "OpenOrbis PS4 SELF Project.zip". A `read -r` loop over a newline list would still be
# correct here, but xargs -0 is what keeps this honest if the prune list ever shrinks.
( cd "$STAGE" && find . -type f ! -name SHA256SUMS -print0 | LC_ALL=C sort -z \
    | while IFS= read -r -d '' f; do _sha256 "$f"; done ) > "$STAGE/SHA256SUMS"
printf '   %s files\n' "$(grep -c . "$STAGE/SHA256SUMS" || true)"

# -----------------------------------------------------------------------------------------
# 7. Tar.
# -----------------------------------------------------------------------------------------
if [ "$DRYRUN" -eq 1 ]; then
  log "DRY RUN - staged at $STAGE, no tarball written"
  du -sh "$STAGE"/* 2>/dev/null | sed 's/^/   /'
  echo
  cat "$STAGE/BUNDLE.txt"
  trap - EXIT
  echo
  warn "stage left at $STAGE (remove it yourself)"
  exit 0
fi

mkdir -p "$OUT"
TARBALL="$OUT/${NAME}.tar.gz"
log "writing $TARBALL"
tar -C "$(dirname "$STAGE")" -czf "$TARBALL" "$NAME"
printf '   %s bytes\n' "$(_size "$TARBALL")"
_sha256 "$TARBALL" | sed "s|$OUT/||" > "$TARBALL.sha256"
cat "$TARBALL.sha256" | sed 's/^/   /'
echo
cat "$STAGE/BUNDLE.txt"
echo
log "NEXT, and it is not optional:"
echo "   1. scripts/release/verify-sdk-bundle.sh $TARBALL"
echo "   2. scripts/release/bundle-gate.sh       $TARBALL   # needs ld.lld + llvm-ar + llvm-nm"
echo "   The bundle is NOT publishable until (2) passes - BUNDLE.txt still says gate=unproven."
