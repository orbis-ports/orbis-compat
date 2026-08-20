#!/usr/bin/env bash
# Build a fake-signed PS4 .pkg from an already-created eboot.bin.
#
#   scripts/ps4/make-pkg.sh --eboot <eboot.bin> --out-dir <dir> \
#       --title-id TMPS10001 --title "Tempest Hello" [--version 01.00] \
#       [--content-label TEMPESTHELLO0000] [--icon <png>] [--sdk <path>] \
#       [--extra <src>:<targ_path>]...
#
# Recipe mirrors the OpenOrbis SDK sample rule (samples/hello_world/Makefile),
# which is the only normative description of the packaging steps:
#   PkgTool.Core sfo_new / sfo_setentry  -> sce_sys/param.sfo
#   create-gp4 --content-id --files      -> pkg.gp4
#   PkgTool.Core pkg_build               -> <CONTENT_ID>.pkg
#
# PkgTool.Core is an old self-contained .NET build with two host quirks:
#   * it links libssl.so.1.1 and rejects OpenSSL 3 -> PS4_PKGTOOL_OPENSSL_LIB
#   * it has no ICU in its closure -> DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=1
# The flake devShell exports both; outside it we fall back to a /nix/store probe.
set -euo pipefail

die() { echo "make-pkg: $*" >&2; exit 1; }

EBOOT=""
OUT_DIR=""
TITLE_ID=""
TITLE=""
VERSION="01.00"
CONTENT_LABEL=""
ICON=""
SDK="${OO_PS4_TOOLCHAIN:-$HOME/.local/opt/openorbis}"
EXTRAS=()

while [[ $# -gt 0 ]]; do
  case "$1" in
    --eboot)         EBOOT="$2";         shift 2 ;;
    --out-dir)       OUT_DIR="$2";       shift 2 ;;
    --title-id)      TITLE_ID="$2";      shift 2 ;;
    --title)         TITLE="$2";         shift 2 ;;
    --version)       VERSION="$2";       shift 2 ;;
    --content-label) CONTENT_LABEL="$2"; shift 2 ;;
    --icon)          ICON="$2";          shift 2 ;;
    --sdk)           SDK="$2";           shift 2 ;;
    --extra)         EXTRAS+=("$2");     shift 2 ;;
    -h|--help)       sed -n '2,12p' "$0"; exit 0 ;;
    *)               die "unknown argument: $1" ;;
  esac
done

[[ -n "$EBOOT"    ]] || die "--eboot is required"
[[ -n "$OUT_DIR"  ]] || die "--out-dir is required"
[[ -n "$TITLE_ID" ]] || die "--title-id is required"
[[ -n "$TITLE"    ]] || die "--title is required"
[[ -f "$EBOOT"    ]] || die "eboot not found: $EBOOT"

# TITLE_ID is 4 letters + 5 digits; param.sfo and the content ID both embed it.
[[ "$TITLE_ID" =~ ^[A-Z]{4}[0-9]{5}$ ]] || die "TITLE_ID must be AAAA00000, got '$TITLE_ID'"

if [[ "$(uname -s)" == "Darwin" ]]; then
  BINDIR="$SDK/bin/macos"
else
  BINDIR="$SDK/bin/linux"
fi
PKGTOOL="$BINDIR/PkgTool.Core"
CREATE_GP4="$BINDIR/create-gp4"
[[ -x "$PKGTOOL"    ]] || die "PkgTool.Core not found at $PKGTOOL (set OO_PS4_TOOLCHAIN)"
[[ -x "$CREATE_GP4" ]] || die "create-gp4 not found at $CREATE_GP4 (set OO_PS4_TOOLCHAIN)"

# CONTENT_ID layout, copied from every OpenOrbis sample:
#   <6-char publisher>-<TITLE_ID>_00-<16-char label>
if [[ -z "$CONTENT_LABEL" ]]; then
  CONTENT_LABEL="$TITLE_ID"
fi
CONTENT_LABEL="${CONTENT_LABEL^^}"
CONTENT_LABEL="${CONTENT_LABEL//[^A-Z0-9]/}"
CONTENT_LABEL="${CONTENT_LABEL}0000000000000000"
CONTENT_LABEL="${CONTENT_LABEL:0:16}"
CONTENT_ID="IV0000-${TITLE_ID}_00-${CONTENT_LABEL}"

export DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=1
if [[ -z "${PS4_PKGTOOL_OPENSSL_LIB:-}" ]]; then
  for cand in /nix/store/*-openssl-1.1.1*/lib/libssl.so.1.1; do
    if [[ -e "$cand" ]]; then
      PS4_PKGTOOL_OPENSSL_LIB="$(dirname "$cand")"
      break
    fi
  done
fi
if [[ -n "${PS4_PKGTOOL_OPENSSL_LIB:-}" ]]; then
  export LD_LIBRARY_PATH="${PS4_PKGTOOL_OPENSSL_LIB}${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi

STAGE="$OUT_DIR/pkg-stage"
rm -rf "$STAGE"
mkdir -p "$STAGE/sce_sys" "$STAGE/sce_module"

cp -f "$EBOOT" "$STAGE/eboot.bin"

# libc / libSceFios2 are the two PRXs every OpenOrbis title ships; the loader
# resolves them from /app0/sce_module before falling back to system modules.
for prx in libc libSceFios2; do
  if [[ -f "$SDK/src/modules/$prx.prx" ]]; then
    cp -f "$SDK/src/modules/$prx.prx" "$STAGE/sce_module/$prx.prx"
  fi
done

GP4_FILES=("eboot.bin" "sce_sys/param.sfo")
for prx in libc libSceFios2; do
  [[ -f "$STAGE/sce_module/$prx.prx" ]] && GP4_FILES+=("sce_module/$prx.prx")
done

if [[ -n "$ICON" && -f "$ICON" ]]; then
  cp -f "$ICON" "$STAGE/sce_sys/icon0.png"
  GP4_FILES+=("sce_sys/icon0.png")
fi

EXTRA_DIRS=()
for spec in ${EXTRAS+"${EXTRAS[@]}"}; do
  src="${spec%%:*}"
  targ="${spec#*:}"
  [[ -f "$src" ]] || die "extra file not found: $src"
  mkdir -p "$STAGE/$(dirname "$targ")"
  cp -f "$src" "$STAGE/$targ"
  GP4_FILES+=("$targ")
  # Every ancestor directory of the target, because of the .gp4 quirk below.
  d="$(dirname "$targ")"
  while [[ "$d" != "." && "$d" != "/" ]]; do
    EXTRA_DIRS+=("$d")
    d="$(dirname "$d")"
  done
done

# create-gp4 emits a FIXED <rootdir>: sce_sys (+about), sce_module and assets with five
# hard-coded children. LibOrbisPkg's PfsProperties.BuildFSTree then resolves every file's
# targ_path against that tree, and a file in a directory the tree does not declare dies in
# FindDir with "Sequence contains no elements" - so --extra has only ever worked for a
# target at the package root or inside one of those seven directories. ps4/gapi-suite is
# the first caller to pass --extra at all (it ships /app0/shader and /app0/assets/gapi, the
# paths Tests/tests/gapi's own bodies open), and it needs two directories create-gp4 does
# not know about.
#
# So the rootdir is REBUILT here from the union of what create-gp4 declared and what the
# extras need, rather than appended to: a second top-level <dir targ_name="assets"> would
# be a different node from the first, and FindDir would keep finding the one without the
# new child. Runs only when there are extras, so a package without them is byte-identical
# to what this script produced before.
gp4_patch_rootdir() {
  local gp4="$1"; shift
  [[ "$#" -gt 0 ]] || return 0
  local tmp; tmp="$(mktemp)"
  {
    sed -n '1,/<rootdir>/p' "$gp4"
    {
      # Directories create-gp4 declared, as full paths.
      awk '
        /<rootdir>/ { inr=1; next }
        /<\/rootdir>/ { inr=0 }
        !inr { next }
        {
          line=$0
          if(match(line,/targ_name="[^"]*"/)) {
            name=substr(line,RSTART+11,RLENGTH-12)
            path=(depth>0 ? stack[depth] "/" name : name)
            print path
            if(line ~ /\/>[[:space:]]*$/) next
            stack[++depth]=path
            next
          }
          if(line ~ /<\/dir>/) depth--
        }' "$gp4"
      printf '%s\n' "$@"
    } | LC_ALL=C sort -u | awk -F/ '
        {
          # Close deeper-or-equal levels, then open this one. Paths arrive sorted, so a
          # parent always precedes its children.
          while(open>0 && substr($0,1,length(cur[open])+1)!=(cur[open] "/")) {
            printf("%s</dir>\n",ind(open)); open--
          }
          cur[++open]=$0
          printf("%s<dir targ_name=\"%s\">\n",ind(open),$NF)
        }
        END { while(open>0) { printf("%s</dir>\n",ind(open)); open-- } }
        function ind(n,  s,i) { s="\t\t"; for(i=1;i<n;i++) s=s "\t"; return s }'
    sed -n '/<\/rootdir>/,$p' "$gp4"
  } > "$tmp"
  mv -f "$tmp" "$gp4"
}

SFO="$STAGE/sce_sys/param.sfo"
"$PKGTOOL" sfo_new "$SFO" > /dev/null
sfo_set() { "$PKGTOOL" sfo_setentry "$SFO" "$1" --type "$2" --maxsize "$3" --value "$4" > /dev/null; }
sfo_set APP_TYPE           Integer 4   1
sfo_set APP_VER            Utf8    8   "$VERSION"
sfo_set ATTRIBUTE          Integer 4   0
sfo_set CATEGORY           Utf8    4   gd
sfo_set CONTENT_ID         Utf8    48  "$CONTENT_ID"
sfo_set DOWNLOAD_DATA_SIZE Integer 4   0
sfo_set SYSTEM_VER         Integer 4   0
sfo_set TITLE              Utf8    128 "$TITLE"
sfo_set TITLE_ID           Utf8    12  "$TITLE_ID"
sfo_set VERSION            Utf8    8   "$VERSION"

# create-gp4 records orig_path verbatim, so it must run with the stage as cwd.
(
  cd "$STAGE"
  "$CREATE_GP4" -out pkg.gp4 --content-id="$CONTENT_ID" --files "${GP4_FILES[*]}" > /dev/null
  gp4_patch_rootdir pkg.gp4 ${EXTRA_DIRS+"${EXTRA_DIRS[@]}"}
  "$PKGTOOL" pkg_build pkg.gp4 . > pkg-build.log 2>&1 || {
    cat pkg-build.log >&2
    exit 1
  }
)

PKG="$STAGE/$CONTENT_ID.pkg"
[[ -f "$PKG" ]] || {
  sed -n '1,40p' "$STAGE/pkg-build.log" >&2 2>/dev/null || true
  die "PkgTool.Core produced no $CONTENT_ID.pkg"
}

mv -f "$PKG" "$OUT_DIR/$CONTENT_ID.pkg"
ln -sf "$CONTENT_ID.pkg" "$OUT_DIR/$TITLE_ID.pkg"
echo "pkg: $OUT_DIR/$CONTENT_ID.pkg ($(du -h "$OUT_DIR/$CONTENT_ID.pkg" | cut -f1))"
