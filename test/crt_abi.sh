#!/usr/bin/env bash
# Copyright © 2026 Mikołaj Mikołajczyk
# SPDX-License-Identifier: MIT
#
# The crt objects this repository builds, compared against the SDK's originals - section by section,
# symbol by symbol, byte by byte.
#
#   test/crt_abi.sh <our-crt-dir> <sdk-lib-dir>
#
# ⚠ THIS IS THE ONLY CHECK THAT EXISTS FOR crt/. Nothing here links, boots or runs: this machine has
# clang and objdump and no ld.lld, no llvm-ar and no console. What a compile can prove is that the
# replacement presents the same interface as the thing it replaces - the same sections under the same
# names, the same symbols with the same binding, the same bytes in the blocks the loader reads - and
# that is what this asserts. It does NOT prove the console starts the result. See build.sh's closing
# note for what a person with a toolchain and a console has to run.
#
# It is written the way test/sizes.c and test/declarations.c are written: it must FAIL when pointed
# at something that is not the thing under test, or it is asserting nothing. build.sh proves that by
# running it against a decoy object and requiring a non-zero exit.
#
# ------------------------------------------------------------------ what counts as a difference
#
# Compared:  allocatable sections (name -> size), their relocation sections, every symbol that is not
#            a file or section symbol (name -> binding + owning section), and the raw bytes of every
#            allocatable PROGBITS section present in both.
# Ignored:   .comment  (the compiler's version string - this host is not the SDK's clang 18.1.4)
#            .llvm_addrsig, .strtab, .symtab, and the empty section 0 - metadata, not interface.
#            Symbol TYPE and SIZE. The SDK's crt1.o emits its data from inline asm labels, which have
#            neither; this file emits C objects, which have both. `l .data` vs `l O .data 0x8` is the
#            same symbol in the same place, described better.
#
# Every remaining difference must appear in the EXPECT table below, with a reason. A difference that
# is not in the table fails the check. A difference in the table that has GONE is reported as a note
# rather than a failure - a deviation disappearing is an improvement, and improvements should not
# have to be scheduled.
set -uo pipefail

OURS="${1:?usage: crt_abi.sh <our-crt-dir> <sdk-lib-dir>}"
SDK="${2:?usage: crt_abi.sh <our-crt-dir> <sdk-lib-dir>}"
OBJDUMP="${OBJDUMP:-objdump}"

command -v "${OBJDUMP}" >/dev/null || { echo "!! no objdump - set OBJDUMP=<path>" >&2; exit 2; }

# ------------------------------------------------------------------ the expected differences
#
# One line per known difference, `<object> <kind> <key>  # why`. Anything else is a failure.
EXPECT=$(cat <<'EOF'
crt1.o   section .text                  the SDK's is 8 bytes and ours is 5: theirs pads the 5-byte
                                        jmp out to the next alignment boundary with a 3-byte nopl.
                                        Padding is not interface.
crt1.o   symbol  __dso_handle           ours is WEAK HIDDEN where the SDK's is LOCAL. The one
                                        deliberate symbol-table change in crt/, and the reason is in
                                        crt/orbis_crt1.c: six members of the SDK's libc++.a
                                        reference __dso_handle as GLOBAL HIDDEN UND and nothing in
                                        the SDK defines it globally, so the SDK's LOCAL definition
                                        cannot satisfy them. Weak so it yields to any TU that
                                        carries its own.
crtlib.o symbol  __dso_handle           same change, same reason.
crtlib.o symbol  __init_array_start     UND here, `g O .bss` in the SDK's object. The SDK's crtlib.c
crtlib.o symbol  __init_array_end       declares both as tentative definitions, which were COMMON
                                        under clang's pre-11 -fcommon default and became real .bss
                                        objects when that default flipped. As shipped they shadow
                                        the linker's own .init_array boundaries, leaving a one-
                                        iteration loop over a NULL function pointer. Leaving them
                                        undefined is the correction. See crt/orbis_crtlib.c.
crtlib.o section .bss                   follows from the two lines above: with nothing defined in
                                        .bss the section is not emitted at all. 0x10 bytes there.
crtlib.o section .text                  0x63 against 0x78. Different code for the same contract -
crtlib.o section .rela.text             three GOT loads against two, and no stack frame. This file
                                        is an independent implementation, not a transcription; if
                                        .text matched byte for byte that would be the surprise.
crtlib.o section .eh_frame              0x80 against 0x98, with an identical .rela.eh_frame: four
                                        FDEs either way, over functions of different lengths.
EOF
)

expected() {                    # expected <object> <kind> <key>
  grep -qE "^$1[[:space:]]+$2[[:space:]]+$3([[:space:]]|$)" <<<"${EXPECT}"
}

fail=0
note() { printf '   .. %s\n' "$*"; }
bad()  { printf '   !! %s\n' "$*"; fail=1; }

# ------------------------------------------------------------------ readers
#
# `objdump -h` prints a four-line preamble and then "Idx Name Size VMA Type"; take name and size, and
# drop the metadata sections named above. `objdump -t` prints "SYMBOL TABLE:" and then one row per
# symbol: sixteen hex digits, a space, SEVEN columns of flags, a space, the owning section, a TAB,
# the size, and the name - optionally preceded by ".hidden". The flag columns are fixed (binutils
# order): 0 is l/g/u, 1 is w for weak, 5 is d for a debugging symbol, 6 is F/f/O. So a file symbol is
# column 6 == "f" and a section symbol is column 5 == "d", and neither is interface.
#
# Visibility IS interface and is kept. Symbol TYPE and SIZE are not, and are dropped - see the note
# at the top of this file.
sections() {
  "${OBJDUMP}" -h "$1" | awk '$2 ~ /^\./ && $3 ~ /^[0-9a-f]+$/ {printf "%s %s\n", $2, $3}' |
    grep -vE '^\.(comment|llvm_addrsig|strtab|symtab) ' | sort
}

symbols() {
  "${OBJDUMP}" -t "$1" | awk -F'\t' '
    /^SYMBOL TABLE:/ { on = 1; next }
    !on || NF < 2    { next }
    {
      flags = substr($1, 18, 7)
      sec   = substr($1, 26)
      if (substr(flags, 7, 1) == "f") next          # the source-file symbol
      if (substr(flags, 6, 1) == "d") next          # a section symbol
      bind = substr(flags, 1, 1)
      if (bind == " ") bind = (substr(flags, 2, 1) == "w") ? "w" : "-"
      n = split($2, f, " ")
      name = f[n]
      vis  = (n > 1 && f[n-1] == ".hidden") ? "hidden" : "default"
      printf "%s %s %s %s\n", name, bind, vis, sec
    }' | sort -u
}

# `objdump -s -j` prints a four-line preamble then the hex dump; an absent section prints nothing.
bytes() { "${OBJDUMP}" -s -j "$2" "$1" 2>/dev/null | sed -n '/^Contents of section/,$p'; }

compare() {                     # compare <object-name>
  local obj="$1" ours="${OURS}/$1" sdk="${SDK}/$1"
  printf '== %s\n' "${obj}"
  [[ -f "${ours}" ]] || { bad "${ours} does not exist"; return; }
  [[ -f "${sdk}"  ]] || { bad "${sdk} does not exist - is OO_PS4_TOOLCHAIN the SDK?"; return; }

  # ---- sections
  local a b
  a="$(sections "${ours}")"; b="$(sections "${sdk}")"
  while read -r name size; do
    [[ -z "${name}" ]] && continue
    local theirs; theirs="$(awk -v n="${name}" '$1==n{print $2}' <<<"${b}")"
    if [[ -z "${theirs}" ]]; then
      expected "${obj}" section "${name}" && note "section ${name}: only in ours (expected)" \
                                          || bad  "section ${name}: only in ours"
    elif [[ "${theirs}" != "${size}" ]]; then
      expected "${obj}" section "${name}" && note "section ${name}: ${size} vs ${theirs} (expected)" \
                                          || bad  "section ${name}: ${size} vs ${theirs}"
    else
      printf '   section %-42s %s\n' "${name}" "${size}"
    fi
  done <<<"${a}"
  while read -r name size; do
    [[ -z "${name}" ]] && continue
    grep -qE "^${name} " <<<"${a}" && continue
    expected "${obj}" section "${name}" && note "section ${name}: only in the SDK's, ${size} (expected)" \
                                        || bad  "section ${name}: only in the SDK's, ${size}"
  done <<<"${b}"

  # ---- symbols
  a="$(symbols "${ours}")"; b="$(symbols "${sdk}")"
  local name rest theirs
  while read -r name rest; do
    [[ -z "${name}" ]] && continue
    theirs="$(awk -v n="${name}" '$1==n{$1="";print substr($0,2)}' <<<"${b}")"
    if [[ -z "${theirs}" ]]; then
      expected "${obj}" symbol "${name}" && note "symbol ${name}: only in ours (expected)" \
                                         || bad  "symbol ${name}: only in ours"
    elif [[ "${theirs}" != "${rest}" ]]; then
      expected "${obj}" symbol "${name}" && note "symbol ${name}: [${rest}] vs [${theirs}] (expected)" \
                                         || bad  "symbol ${name}: [${rest}] vs [${theirs}]"
    else
      printf '   symbol  %-42s %s\n' "${name}" "${rest}"
    fi
  done <<<"${a}"
  while read -r name rest; do
    [[ -z "${name}" ]] && continue
    grep -qE "^${name} " <<<"${a}" && continue
    expected "${obj}" symbol "${name}" && note "symbol ${name}: only in the SDK's, ${rest} (expected)" \
                                       || bad  "symbol ${name}: only in the SDK's, ${rest}"
  done <<<"${b}"

  # ---- bytes, for every allocatable PROGBITS section both objects have. ⚠ The blocks the loader
  #      reads are DATA, and that is where a byte difference would be a behaviour difference: a wrong
  #      size word, a wrong magic, a heap knob quietly changed. A .text difference is expected and is
  #      covered by the section-size comparison above.
  local sec
  a="$(sections "${ours}")"; b="$(sections "${sdk}")"
  for sec in $(awk '$1 ~ /^\.(text|init|fini|data|rodata|eh_frame)/ {print $1}' <<<"${b}"); do
    grep -qE "^${sec} " <<<"${a}" || continue
    if [[ "$(bytes "${ours}" "${sec}")" == "$(bytes "${sdk}" "${sec}")" ]]; then
      printf '   bytes   %-42s identical\n' "${sec}"
    else
      expected "${obj}" section "${sec}" && note "bytes ${sec}: differ (expected - the sizes do)" \
                                         || bad  "bytes ${sec}: differ"
    fi
  done

  # ---- relocations, which are the field list of every block the loader reads
  if [[ "$("${OBJDUMP}" -r "${ours}" | tail -n +4 | sort)" == "$("${OBJDUMP}" -r "${sdk}" | tail -n +4 | sort)" ]]; then
    printf '   relocations                                        identical\n'
  else
    expected "${obj}" section .rela.text && note "relocations differ (expected - .text does)" \
                                         || bad  "relocations differ"
  fi
}

compare crt1.o
compare crtlib.o
compare crti.o
compare crtn.o

# ------------------------------------------------------------------ the stack pointer at entry
#
# ⚠ THE ENTRY PATH MAY NOT TOUCH %rsp, and that is checked on the artifact rather than trusted from
# the source. The loader picks the stack pointer it enters `_start` with; the SDK's crt reaches
# __libc_start_main through two jmps and no push, so libc sees exactly the alignment the loader
# chose. A compiler that decided to build a frame in _start_ps4_c - clang does, at -O0, and did here
# until -fomit-frame-pointer was added - would shift %rsp under libc for the life of the process, and
# the symptom would be an unaligned movaps a long way from this file. clang's musttail cannot express
# it: it requires both signatures to have the same parameter count, and these are 1 and 6.
printf '== the entry path\n'
for sec in .text .text._start_ps4_c; do
  if "${OBJDUMP}" -d -j "${sec}" "${OURS}/crt1.o" 2>/dev/null | tail -n +5 | grep -qE '%rsp|push|pop|call'; then
    bad "${sec} of crt1.o writes the stack pointer - see the note above"
  else
    printf '   %-20s no push, no pop, no call, no write to %%rsp\n' "${sec}"
  fi
done

if [[ ${fail} -ne 0 ]]; then
  printf '!! crt objects do not match the SDK'"'"'s interface\n' >&2
  exit 1
fi
printf '== crt objects match the SDK'"'"'s interface, with the differences listed above and only those\n'
