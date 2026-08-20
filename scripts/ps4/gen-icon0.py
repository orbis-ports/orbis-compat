#!/usr/bin/env python3
"""Generate the 512x512 sce_sys/icon0.png a PS4 package shows on the home menu.

    scripts/ps4/gen-icon0.py --out <path> [--title-id TMPS10001]

Deliberately dependency-free (stdlib zlib only) so the PS4 package build needs
nothing beyond python3. The artwork is a flat two-tone tile with a wordmark
block; its hue is derived from the title id so several installed Tempest
packages stay distinguishable on the console's home menu.
"""

from __future__ import annotations

import argparse
import struct
import zlib

SIZE = 512


def _hue_from(title_id: str) -> tuple[int, int, int]:
    h = zlib.crc32(title_id.encode("ascii")) & 0xFFFFFF
    r = 40 + (h & 0x7F)
    g = 40 + ((h >> 8) & 0x7F)
    b = 60 + ((h >> 16) & 0x7F)
    return r, g, b


def _pixels(title_id: str) -> bytearray:
    br, bg, bb = _hue_from(title_id)
    rows = bytearray()
    for y in range(SIZE):
        rows.append(0)  # PNG filter type 0 (None) for this scanline
        # vertical gradient, darker at the bottom
        f = 1.0 - 0.45 * (y / (SIZE - 1))
        base = (int(br * f), int(bg * f), int(bb * f))
        for x in range(SIZE):
            # centred wordmark block: a "T" bar plus stem
            in_bar = 96 <= y < 160 and 96 <= x < 416
            in_stem = 160 <= y < 416 and 224 <= x < 288
            if in_bar or in_stem:
                rows += bytes((235, 235, 240))
            else:
                rows += bytes(base)
    return rows


def _chunk(tag: bytes, data: bytes) -> bytes:
    return (
        struct.pack(">I", len(data))
        + tag
        + data
        + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)
    )


def write_png(path: str, title_id: str) -> None:
    ihdr = struct.pack(">IIBBBBB", SIZE, SIZE, 8, 2, 0, 0, 0)  # 8-bit truecolour
    png = b"\x89PNG\r\n\x1a\n"
    png += _chunk(b"IHDR", ihdr)
    png += _chunk(b"IDAT", zlib.compress(bytes(_pixels(title_id)), 9))
    png += _chunk(b"IEND", b"")
    with open(path, "wb") as fh:
        fh.write(png)


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--out", required=True, help="output png path")
    ap.add_argument("--title-id", default="TMPS10000", help="title id, seeds the tile colour")
    args = ap.parse_args()
    write_png(args.out, args.title_id)


if __name__ == "__main__":
    main()
