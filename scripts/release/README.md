# scripts/release — cutting one SDK bundle

Four scripts, in the order they run. Each refuses rather than warns; the point of the set is
that a bundle which reaches a stranger has passed all four.

```
sdk-licenses.sh        the licence ledger. One table drives licenses/, NOTICE.md and the gate
make-sdk-bundle.sh     assembles the tarball. Refuses on a Mesa/orbis-compat mismatch
verify-sdk-bundle.sh   OFFLINE gate over a tarball or an unpacked bundle. Ships INSIDE it
bundle-gate.sh         THE PUBLICATION GATE: builds hello-world AND a real port from an
                       UNPACKED copy. RUN IN FULL 2026-09-17 - see "What is unproven" 
templates/             orbis-sdk.cmake, env.sh, bundle-README.md
hello/                 the worked example, and the gate's first stage
test/                  27 cases over the four scripts above. ~30s, no SDK, no Mesa, no network
```

`sdk-licenses.sh` is modelled on `~/src/unemu-org/oracles/fetch-oracles.sh` and uses the same
five exit codes for the same five things: `0` ok, `1` LOCAL DRIFT, `2` usage, `3` UPSTREAM
MOVED, `4` INCOMPLETE. Do not invent a second shape for this in this organisation.

---

## The command sequence, on a host with the full toolchain

Needs `clang clang++ ld.lld llvm-ar llvm-ranlib llvm-nm cmake` and, for the Mesa half, `nix`.

```sh
# 0. The four inputs, in the order they depend on each other.
export OO_PS4_TOOLCHAIN=~/.local/opt/openorbis     # an UNPACKED SDK: link.x AND lib/libc.a
cd ~/src/orbis-ports/orbis-compat
./build.sh                                          # NOT --no-check. Produces build/liborbis-compat.a
COMPAT=$(git rev-parse HEAD)

# 1. Mesa, built against THIS orbis-compat. The pairing gate refuses anything else.
cd ~/src/orbis-ports/mesa-ps4
ORBIS_COMPAT_DIR=~/src/orbis-ports/orbis-compat ./ps4/build.sh
#    ...or download an existing bundle whose manifest.txt says orbis-compat-commit=$COMPAT:
#    gh release download orbis-mesa-<sha> --repo orbis-ports/mesa-ps4 --pattern 'orbis-mesa-*.tar.gz'
#    tar -xzf orbis-mesa-<sha>.tar.gz && MESA=$PWD/orbis-mesa-<sha>

# 2. The licence ledger. Needs network once; after that it is offline forever.
cd ~/src/orbis-ports/orbis-compat
./scripts/release/sdk-licenses.sh fetch
./scripts/release/sdk-licenses.sh notice
./scripts/release/sdk-licenses.sh verify              # must say OK before anything is cut
./scripts/release/sdk-licenses.sh verify --upstream   # optional; exit 3 means a pin moved

# 3. Cut it. Refuses on a dirty tree or a mismatched pair.
./scripts/release/make-sdk-bundle.sh --version v1 --mesa "$MESA"
#    -> $ORBIS_WORK/release/orbis-sdk-v1.tar.gz  (+ .sha256), BUNDLE.txt says gate=unproven

# 4. Check what was cut, offline. Expect exit 4 INCOMPLETE here: gate=unproven is why.
./scripts/release/verify-sdk-bundle.sh "$ORBIS_WORK/release/orbis-sdk-v1.tar.gz"

# 5. THE PUBLICATION GATE. Builds hello-world AND a real port from an UNPACKED copy.
./scripts/release/bundle-gate.sh "$ORBIS_WORK/release/orbis-sdk-v1.tar.gz" \
     --port ~/src/orbis-ports/OpenGothic

# 6. Re-cut with the gate recorded, and verify THAT tarball. This one must say OK, not
#    INCOMPLETE. Step 5 stamps the unpacked copy, which necessarily breaks its SHA256SUMS
#    line - a gate result is new information, so the bundle is a new artifact.
ORBIS_BUNDLE_GATE=pass ./scripts/release/make-sdk-bundle.sh --version v1 --mesa "$MESA"
./scripts/release/verify-sdk-bundle.sh "$ORBIS_WORK/release/orbis-sdk-v1.tar.gz"
```

Only after step 6 returns `OK` is there anything worth publishing.

⚠ **Nothing in here publishes.** No `gh release create`, no push, no tag. Cutting and
publishing are separate acts on purpose: the gate sits between them, and a script that did
both would make it possible to skip it by accident.

---

## The tests

```sh
scripts/release/test/run.sh                 # 27 cases, ~30 s
scripts/release/test/run.sh --list          # the case ids
scripts/release/test/run.sh B2 B3 --keep    # one row of the pairing matrix, work tree preserved
scripts/release/test/mutations.sh           # put each defect back and require the suite to go red
```

They need `build/liborbis-compat.a` and nothing else — no SDK, no Mesa bundle, no network, no
console. `scripts/release/test/fixtures.sh` stands up a synthetic OpenOrbis SDK and a synthetic
unpacked Mesa bundle (the real `manifest.txt` shape, the real import names) plus a throwaway
three-commit orbis-compat repository, and the cases drive the real `make-sdk-bundle.sh` and
`verify-sdk-bundle.sh` end to end against them. A cut takes ~1.3 s that way against minutes for
the real 289 MB tree, which is what makes a 27-case matrix affordable at all.

⚠ **They assert the three verdicts by name.** `OK` is exit 0, `INCOMPLETE` is exit 4, `FAILED`
is exit 1, and no case accepts "non-zero". Conflating the first two is the defect that made this
gate refuse every bundle it exists to fix.

⚠ **`mutations.sh` is what makes the suite worth running.** Green means twenty-seven assertions
held; it does not mean any of them *could* have failed, and a check that cannot fail is the exact
shape of five of the six defects below. That script reinstates each defect in turn, requires the
matching case to go red, restores every file and compares it byte for byte. `build.sh` already
holds `test/crt_abi.sh` to the same standard with its decoy object.

What the suite cannot reach, stated rather than faked: `bundle-gate.sh` stages 3–6 need a cross
toolchain and, for stage 5, a three-hour port build. Stages 0, 1 and 1b run for real (1b is where
the `INCOMPLETE` deadlock lived); the two defects past that point are covered by evaluating the
real source line under the real shell options (the stage-4 `llvm-nm` count) and by a
decoy-checked read of the real stage-5 invocation. `.github/workflows/release-scripts-test.yml`
runs both scripts on every pull request that touches `scripts/release/`, with `--no-skips`,
because a runner that just installed `llvm` has no honest reason to skip anything.

---

## Run in full on 2026-09-17, and what it found

Every stage of `bundle-gate.sh` has now executed on macOS against a real bundle: SDK v0.5.4 asset,
`liborbis-compat.a`, `orbis-mesa-a4fa6b57f8bd`, OpenGothic as the port. It ended `GATE PASSED`, and
the two `eboot.bin` it produced carry the Orbis SELF magic `4f15 3d1d`.

**It took ten runs, and the first six failed in this file's own machinery rather than in a bundle:**

| what | where |
|---|---|
| `sed 's/^_//'` truncated every Itanium-ABI C++ name, so `_Znwm` could never match | `verify-sdk-bundle.sh` |
| a dirty tree was `FAILED` not `INCOMPLETE`, so the gate refused every bundle it exists to fix | `verify-sdk-bundle.sh` |
| the hello guard sat above `project()`, where nothing the toolchain file sets exists yet | `hello/CMakeLists.txt` |
| the overlay archive was force-loaded twice, duplicating every symbol in it | `hello/CMakeLists.txt` |
| `grep -q` under `set -o pipefail` - SIGPIPE made a matching grep report "not found" | `bundle-gate.sh` |
| the port built in `~/.cache`, shared between runs, so run N used run N-1's deleted sysroot | `bundle-gate.sh` |

The last is README §9 trap 7 happening inside the script written to catch it. Each fix carries the
measurement that produced it.

⚠ **And the bundle was then verified on a console**, which the gate cannot do: `hello` ran to
`all checks passed`, and `triangle` presented frames. That is outside this script's reach and is
recorded in orbis-compat's README §0.

## What is still unproven, and exactly what would prove it

⚠ **`bundle-gate.sh` has never executed past its own stage 0.** It was written on a host with
`/usr/bin/clang` (Apple clang 21.0.0) and `/usr/bin/objdump` and **without** `ld.lld`,
`llvm-ar`, `llvm-ranlib`, `llvm-nm` or `cmake`. Stage 0 exists to detect exactly that, and it
did — it named all five and exited 4. Everything after it is **reasoned from the build files
it drives, not observed**: no cross link has completed, no `link.txt` has been read, no
`eboot.bin` has been produced.

`make-sdk-bundle.sh` and `verify-sdk-bundle.sh` are in a different position. Both ran end to
end, repeatedly, and both refused the things they are meant to refuse — but against synthetic
inputs. Read the two lists below as one claim each, not as one claim about all four scripts.

What WAS exercised on that host, and is therefore evidence:

```
sdk-licenses.sh fetch/sums/notice/verify   all four ran. 17 licence texts gathered from four
                                           kinds of source, 22 components, verify OK, and
                                           NOTICE.md regenerates byte-identically
the SDK audit                              every figure in LICENSING.md §2 was measured on the
                                           unpacked tarball, not taken from a table - and one
                                           row of that table turned out to be wrong (§2.9)
hello/hello.c, cross syntax-only           clang --target=x86_64-pc-freebsd12-elf -fsyntax-only
                                           against the real SDK + the overlay: clean, rc=0
the include-order assertion, three ways    overlay AHEAD of the SDK:  PASSES
                                           SDK alone:                 FAILS, "4 == 8"
                                           overlay BEHIND the SDK:    FAILS, "4 == 8"
                                           - the third is the SILENT case, and it is not silent
make-sdk-bundle.sh, a real cut             a complete 145-file bundle was staged, hashed,
                                           manifested and tarred (375 KB) from a SYNTHETIC SDK
                                           and a SYNTHETIC Mesa bundle. Every code path ran;
                                           what did not run is the copy of the real 1.1 GB tree
verify-sdk-bundle.sh, all six checks       run against that tarball. Check 5 (the import list)
                                           really did read a real archive with Apple's nm and
                                           match all nine recorded names
the pairing gate, both directions          a deliberately mismatched pair was REFUSED by
                                           make-sdk-bundle.sh, which printed the real drift -
                                           "14 commits, 8 files changed, 362 insertions(+),
                                           6 deletions(-) under include/" - and, when forced
                                           through with --allow-pair-mismatch, FAILED in
                                           verify-sdk-bundle.sh exactly as its header promises
four tamper cases, each detected twice     a modified licence text, a deleted one, a hand-edited
                                           NOTICE.md and an unaccounted extra file were each
                                           caught by SHA256SUMS and again by the ledger
env.sh from an unpacked bundle             set all four variables, materialised the meson cross
                                           file with bundle-relative paths, and deferred
                                           correctly under --keep
bundle-gate.sh, ALL stages                 run in full 2026-09-17 against a REAL SDK, a REAL
                                           Mesa bundle and OpenGothic. GATE PASSED. Stage 0's
                                           missing-tool path was exercised earlier on the same
                                           host, before llvm and lld were installed
```

⚠ **WITHDRAWN 2026-09-17: the cut has been done from the real tree.** The paragraph below
described a 17-file stand-in and is kept because its reasoning is still the right reasoning for
any future stand-in - but the bundle that passed the gate was cut from the real SDK, the real
`liborbis-compat.a` and the real `orbis-mesa-a4fa6b57f8bd`, and came to 87.9 MB.

⚠ **A synthetic SDK is not the SDK.** The cut above used a 17-file stand-in with the right
names in the right places. It proves the script's logic; it does not prove that copying
`lib/` with its 422 `.so` files, `include/` with its 189 `orbis/` headers and two 74 MB
`PkgTool.Core` binaries produces a usable tree. Only a real cut does that.

What was NOT exercised, and the exact command that would exercise it:

| unproven | the command |
|---|---|
| `build.sh` produces a real `liborbis-compat.a` | `OO_PS4_TOOLCHAIN=<sdk> ./build.sh` on a host with `ld.lld llvm-ar llvm-ranlib` |
| a real bundle is cut from real inputs | `./scripts/release/make-sdk-bundle.sh --version v1 --mesa <unpacked orbis-mesa-*>` |
| the import-list check (verify step 5) | any of the above, then `./scripts/release/verify-sdk-bundle.sh <tarball>` on a host with `llvm-nm` |
| hello-world CONFIGURES, LINKS and produces an `eboot.bin` | `./scripts/release/bundle-gate.sh <tarball>` |
| the link line names only bundle paths | the same run — it is stage 4 of that script |
| `--whole-archive` actually pulled the overlay in | the same run — stage 4's `llvm-nm` check on the linked image |
| a real port builds from the unpacked bundle | `./scripts/release/bundle-gate.sh <tarball> --port ~/src/orbis-ports/OpenGothic` |
| `.github/workflows/sdk-bundle.yml` | push the branch, or `gh workflow run sdk-bundle.yml` |

⚠ **Expect the first real run of `bundle-gate.sh` to fail, and expect it to fail in stage 5.**
The most likely cause is named in that script's own comment: `ps4/build.sh` sources
`orbis-env.sh`, which probes for a mesa-ps4 **checkout** by looking for `src/amd/vulkan` and
overwrites `ORBIS_MESA_BUILD` when it does not find one. The bundle's `mesa/` is a build
output, not a checkout. `env.sh` exports `ORBIS_SDK_MESA_SRC`/`ORBIS_SDK_MESA_BUILD` for a
consumer to re-apply afterwards, which is what OpenGothic's `ps4/build.sh` already does — but
that path has not been walked with a bundle at the other end of it.

---

## Why the bundle freezes Mesa and orbis-compat together

`mesa-ps4/.github/workflows/release.yml` deliberately does the opposite for its own artifact,
and its header says why. That reasoning is correct there and wrong here; the long form is in
`make-sdk-bundle.sh`'s header, and the short form is:

The import-list assertion the three sibling repositories carry checks that every name Mesa
imports is **still defined**. Its own comment states the limit: *"presence, not meaning.
Constants inlined at Mesa's compile time leave no symbol to check at all."* Between the
orbis-compat Mesa was last built against and HEAD there are 14 commits and 362 changed lines
under `include/` — `signal.h` +135, `sys/umtx.h` +48, `orbis_thread.h` +56. A struct that
changed size does not change a symbol name. mesa-ps4 can accept that because its consumer is a
build it controls; a stranger with one tarball cannot accept it, cannot detect it, and would
have no idea which half to suspect.
