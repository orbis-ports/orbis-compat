#!/usr/bin/env python3
"""Generate a consumer's Vulkan thunks from its own undefined symbols.

    python3 vkloader/gen.py --objs <that consumer's object directories>

WHY GENERATED. The set of Vulkan entry points a program calls is a property of THAT program, not a list anyone
should maintain by hand: the call sites usually live in upstream-owned code, and any merge can add one. A
hand-written list would rot, and it would rot into a link error in a file nobody edited. So the list is READ
from the objects the engine just produced, and the signatures are READ from the same vulkan_core.h the driver
was compiled against - which also means a header/driver mismatch cannot silently produce a wrong ABI here.

The generated file is COMMITTED, and re-running this must leave `git diff` empty. Same discipline as
mesa-ps4's build-support/orbis/tools/shaders/gen.py, for the same reason: the PS4 build has no code-generation step, and a
generated file that is not reproducible is a fork nobody declared.
"""

import argparse
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

# Hand-written in vkloader.c, and they have to be: these are the ones the ICD answers with a NULL instance
# because they are called before an instance exists, plus vkCreateInstance, which additionally records the
# handle every later resolve needs, plus vkEnumerateInstanceLayerProperties, which is answered locally because
# layers are a loader mechanism and there is no loader.
HAND_WRITTEN = {
    "vkCreateInstance",
    "vkEnumerateInstanceExtensionProperties",
    "vkEnumerateInstanceLayerProperties",
    "vkGetInstanceProcAddr",
}

PREAMBLE = """/* GENERATED - do not edit. Regenerate with:
 *
 *     python3 <orbis-compat>/vkloader/gen.py --objs <this project's object dirs> --out <this file>
 *
 * One forwarding thunk per Vulkan entry point THIS project references, over the RADV ICD. There is no Vulkan
 * loader on this console and no shared object, so something has to define the vk* symbols a program calls and
 * forward them through vkGetInstanceProcAddr; vkloader.c is that something and this is the generated half.
 *
 * The list is read from this project's own objects, so it cannot rot into a link error in a file nobody edited.
 * Re-running the command above must leave `git diff` empty.
 *
 * THE POINTERS ARE RESOLVED ON FIRST CALL, per thunk. A race between two threads calling the same entry point
 * for the first time resolves it twice and stores the same value twice, which is benign - the alternative is a
 * mutex on every Vulkan call in the program, to protect a write that cannot tear.
 *
 * vkloader.h is orbis-compat's, reached through the include directory the build adds for it -
 * ORBIS_VKLOADER_DIR in a CMake consumer. It is not a file of this repository.
 *
 * SPDX-License-Identifier: MIT
 */

#include "vkloader.h"
"""


def undefined_vk_symbols(obj_dirs):
    """Every undefined vk* symbol in the engine's objects - the exact set the linker will demand."""
    objs = []
    for d in obj_dirs:
        for root, _dirs, files in os.walk(d):
            objs += [os.path.join(root, f) for f in files if f.endswith((".o", ".obj"))]
    if not objs:
        sys.exit("no object files under %s - build the engine first" % ", ".join(obj_dirs))

    out = subprocess.run(["nm", "--undefined-only", "--no-demangle"] + objs,
                         check=True, capture_output=True, text=True).stdout
    names = set()
    for line in out.splitlines():
        # "                 U vkCreateDevice"
        m = re.match(r"\s*[Uw]\s+(vk[A-Z]\w*)\s*$", line)
        if m:
            names.add(m.group(1))
    return names, len(objs)


def split_params(text):
    """Split a parameter list on commas, ignoring any nested in parentheses or brackets."""
    parts, depth, cur = [], 0, ""
    for ch in text:
        if ch in "([":
            depth += 1
        elif ch in ")]":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += ch
    if cur.strip():
        parts.append(cur)
    return [p.strip() for p in parts]


def param_name(decl):
    """The identifier a parameter declaration binds - what the thunk passes on.

    Vulkan commands take only scalars, handles and pointers, so the name is the last identifier before an
    optional array suffix: `const float blendConstants[4]` -> blendConstants.
    """
    decl = re.sub(r"\[[^\]]*\]\s*$", "", decl).strip()
    m = re.search(r"(\w+)\s*$", decl)
    if not m:
        sys.exit("cannot find the parameter name in %r" % decl)
    return m.group(1)


def prototypes(header):
    """{name: (return type, [parameter declarations])} for every VKAPI_CALL prototype in the header."""
    text = open(header, encoding="utf-8", errors="replace").read()
    protos = {}
    for m in re.finditer(r"VKAPI_ATTR\s+(.+?)\s+VKAPI_CALL\s+(vk\w+)\s*\(([^;]*?)\)\s*;", text, re.S):
        ret, name, params = m.group(1).strip(), m.group(2), m.group(3).strip()
        params = " ".join(params.split())
        if params in ("void", ""):
            protos[name] = (ret, [])
        else:
            protos[name] = (ret, split_params(params))
    return protos


def emit(name, ret, params):
    args = ", ".join(param_name(p) for p in params)
    decl = ", ".join(params) if params else "void"
    ret_stmt = "return " if ret != "void" else ""
    return (
        # ⚠ WEAK, so that a consumer which implements an entry point itself simply wins. Two strong
        # definitions are a duplicate-symbol error and would need a per-consumer exclusion list;
        # weak plus strong is the linker resolving it for us. The CTS relies on this: its
        # orbisCtsRuntime.c defines vkEnumerateInstanceVersion.
        "__attribute__((weak)) VKAPI_ATTR %s VKAPI_CALL\n"
        "%s(%s)\n"
        "{\n"
        "  static PFN_%s pfn;\n"
        "  if(pfn == NULL) {\n"
        "    pfn = (PFN_%s)vkl_resolve(\"%s\");\n"
        "    if(pfn == NULL)\n"
        "      vkl_missing(\"%s\");\n"
        "    }\n"
        "  %spfn(%s);\n"
        "}\n" % (ret, name, decl, name, name, name, name, ret_stmt, args)
    )


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--objs", nargs="*", default=[],
                    help="directories holding the consumer's object files (walked recursively)")
    # ⚠ THE SHARED TABLE. With --all the list is the HEADER's, not any one consumer's: every entry
    # point the driver could implement gets a weak thunk, once, and no consumer generates anything.
    # The reason this is affordable here is that libvulkan_radeon.a defines ZERO vk* symbols - it is
    # reached entirely through vk_icdGetInstanceProcAddr - so a complete table collides with nothing.
    ap.add_argument("--all", action="store_true",
                    help="emit a weak thunk for every entry point in the header, ignoring --objs")
    ap.add_argument("--header",
                    default=os.path.expanduser("~/.cache/orbis-mesa/mesa/include/vulkan/vulkan_core.h"),
                    help="the vulkan_core.h the driver was built against")
    ap.add_argument("--out", default=os.path.join(HERE, "vkthunks.c"))
    # ⚠ A CONSUMER MAY ALREADY IMPLEMENT AN ENTRY POINT ITSELF, and then a thunk for it is a
    # duplicate symbol at link time rather than a spare function. The CTS does exactly this:
    # orbisCtsRuntime.c hand-writes vkEnumerateInstanceVersion. It has to be named rather than
    # detected, because at generation time that file is not compiled yet.
    ap.add_argument("--exclude", nargs="*", default=[],
                    help="entry points the consumer defines itself; a thunk would collide")
    args = ap.parse_args()

    protos = prototypes(args.header)
    if args.all:
        wanted, n_objs = set(protos), 0
    else:
        if not args.objs:
            sys.exit("--objs is required without --all")
        wanted, n_objs = undefined_vk_symbols(args.objs)

    todo = sorted(wanted - HAND_WRITTEN - set(args.exclude))
    unknown = [n for n in todo if n not in protos]
    if unknown:
        # Not a warning. A symbol the engine calls and the header does not declare means the two are out of
        # step, and guessing a signature here would be an ABI bug that links.
        sys.exit("not declared in %s: %s" % (args.header, ", ".join(unknown)))

    body = [PREAMBLE]
    for n in todo:
        ret, params = protos[n]
        body.append("\n" + emit(n, ret, params))
    open(args.out, "w").write("".join(body))

    print("%d object file(s) -> %d vk* symbols %s, %d hand-written, %d excluded, %d thunks -> %s"
          % (n_objs, len(wanted), "declared" if args.all else "referenced",
             len(wanted & HAND_WRITTEN), len(wanted & set(args.exclude)),
             len(todo), os.path.relpath(args.out)))


if __name__ == "__main__":
    main()
