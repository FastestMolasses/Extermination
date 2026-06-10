#!/usr/bin/env python3
"""Resident-CLUT recovery from a captured PCSX2 GS local-memory image.

Original tooling for the Extermination decompilation project. Reads only the
user's own PCSX2 capture (a save-state GS freeze blob) and the user's locally
extracted disc data; redistributes nothing. All PNG output lands under a
git-ignored directory (``scratch/``).

WHY THIS EXISTS
---------------
Extermination's PSMT8 textures sample a 256-entry CLUT, but the palettes are
NOT shipped as GS upload packets (see tools/vram_residency.py: zero CLUT GS
uploads disc-wide) and brute-force coherence scoring could not recover the
binding (see tools/clut_bruteforce.py / docs/FINDINGS.md). The palettes are raw
1024-byte ``[R G B A]·256`` blobs scattered through the asset files, bound to
textures at runtime by engine code we have not yet matched.

A live PCSX2 capture sidesteps the whole problem: the engine has already
uploaded the *resident* CLUTs into GS local memory (VRAM). We read them
straight out of the captured 4 MB, validate them as palettes, un-swizzle the
CSM1 layout, and -- the payoff -- cross-reference each resident CLUT against
the disc-wide 1024-byte blob pool. A match tells us WHICH on-disc blob is the
palette for a texture, recovering the offline binding rule.

GS FREEZE BLOB LAYOUT
---------------------
The capture's ``gs.bin`` is a PCSX2 GS *freeze* blob (a save-state component),
NOT the GS-dump packet format (no 0xFFFFFFFF magic). It is a small fixed-size
header, the 4 MB GS local memory, then **84 trailing state bytes**. So the
local-memory base is ``len(gs.bin) - 0x400000 - 84`` (= 425 for the v9 freeze
format), NOT ``len - 0x400000`` (= 509) as this tool originally assumed.
Proven 2026-06-09 by byte-exact 8 KB-page anchoring of a known disc texture
upload simulated through the PSMCT32 swizzle (see tools/clut_pair.py and
docs/FINDINGS.md); the old reading skewed every read by 84 bytes (21 CLUT
entries). VRAM word 0 lives at the base; a CLUT buffer pointer ``CBP`` (in
256-byte GS blocks) maps to file offset ``base + CBP * 256``.

CSM1 CLUT SWIZZLE
-----------------
A CLUT uploaded in CSM1 mode (the common case here: CSM=0 in the TEX0 register)
is stored in VRAM through the GS's PSMCT32 page swizzle, NOT linearly. To
recover the linear ``[R G B A]·256`` palette that the disc blobs hold, we apply
the inverse PSMCT32-page block reorder for a 16x16 CLUT region. See
``csm1_unswizzle_clut``.
"""
from __future__ import annotations

import argparse
import hashlib
import sys
from dataclasses import dataclass
from pathlib import Path

_HERE = Path(__file__).resolve().parent
if str(_HERE) not in sys.path:
    sys.path.insert(0, str(_HERE))

from clut import psmct8_csm1_swizzle, write_png_rgba  # noqa: E402


GS_LOCALMEM_SIZE = 0x400000        # 4 MB GS local memory
CLUT_BYTES = 1024                  # 256 entries * 4 bytes (PSMCT32)
VRAM_TRAILER = 84                  # freeze state bytes AFTER the 4 MB (v9 .p2s)


# ---------------------------------------------------------------------------
# Locate the 4 MB GS local memory inside a PCSX2 GS freeze blob.
# ---------------------------------------------------------------------------
def localmem_base(gs_bytes: bytes) -> int:
    """Return the byte offset of VRAM word 0 inside a GS freeze blob.

    In the .p2s v9 freeze the 4 MB local memory is followed by 84 trailing
    state bytes, so the base is ``len - 0x400000 - 84`` (see module
    docstring for the byte-exact proof). The pre-2026-06-09 ``len -
    0x400000`` reading was off by +84 bytes.
    """
    if len(gs_bytes) < GS_LOCALMEM_SIZE + VRAM_TRAILER:
        raise ValueError(f"gs.bin too small ({len(gs_bytes)}) to hold 4 MB VRAM"
                         f" + {VRAM_TRAILER}-byte trailer")
    return len(gs_bytes) - GS_LOCALMEM_SIZE - VRAM_TRAILER


def read_localmem(gs_path: Path) -> tuple[int, bytes]:
    """Return (base_offset, 4 MB local-memory bytes)."""
    d = gs_path.read_bytes()
    base = localmem_base(d)
    return base, d[base:base + GS_LOCALMEM_SIZE]


def read_clut_at(localmem: bytes, cbp: int) -> bytes:
    """Read the 1024 raw bytes of a CLUT at block address ``cbp`` from VRAM."""
    off = cbp * 256
    return localmem[off:off + CLUT_BYTES]


# ---------------------------------------------------------------------------
# CSM1 CLUT un-swizzle (VRAM PSMCT32-page layout -> linear 256-entry palette)
# ---------------------------------------------------------------------------
# A 256-entry PSMCT32 CLUT occupies a 16x16 texel region of one GS page. In
# CSM1 mode the GS reads it through the PSMCT32 swizzle, so a CLUT *stored* in
# VRAM is in that swizzled order. The PS2 convention for a 16x16 PSMCT32 CLUT
# is the same 8-entry block interleave that clut.py::psmct8_csm1_swizzle models
# (within each 32-entry group, entries 8..15 <-> 16..23). That permutation is
# its own inverse, so the same helper converts both directions.
def csm1_unswizzle_clut(clut1024: bytes) -> bytes:
    """Convert a VRAM CSM1-stored CLUT to the linear [R G B A]*256 order."""
    return psmct8_csm1_swizzle(clut1024)


# ---------------------------------------------------------------------------
# Palette validation / characterisation
# ---------------------------------------------------------------------------
@dataclass
class PaletteInfo:
    cbp: int
    a80: int            # entries with alpha exactly 0x80 (fully opaque)
    distinct_rgb: int   # distinct (R,G,B) triples
    chroma: float       # mean per-entry (max-min RGB); ~0 gray, large = vivid
    valid: bool         # alpha all in [0, 0x80]


def characterise(blob: bytes, cbp: int = -1) -> PaletteInfo:
    """Validate a 1024-byte region as a PSMCT32 CLUT and characterise it."""
    if len(blob) != CLUT_BYTES:
        return PaletteInfo(cbp, 0, 0, 0.0, False)
    a80 = 0
    valid = True
    for i in range(3, CLUT_BYTES, 4):
        a = blob[i]
        if a > 0x80:
            valid = False
        if a == 0x80:
            a80 += 1
    rgb = set()
    chroma_tot = 0
    for i in range(0, CLUT_BYTES, 4):
        r, g, b = blob[i], blob[i + 1], blob[i + 2]
        rgb.add((r, g, b))
        chroma_tot += max(r, g, b) - min(r, g, b)
    return PaletteInfo(cbp, a80, len(rgb), round(chroma_tot / 256, 1), valid)


def scan_vram_cluts(localmem: bytes, min_a80: int = 100,
                    min_distinct: int = 32) -> list[PaletteInfo]:
    """Scan all of VRAM at 256-byte block granularity for resident CLUTs."""
    out: list[PaletteInfo] = []
    n_blocks = len(localmem) // 256
    for cbp in range(n_blocks):
        off = cbp * 256
        if off + CLUT_BYTES > len(localmem):
            break
        info = characterise(localmem[off:off + CLUT_BYTES], cbp)
        if info.valid and info.a80 >= min_a80 and info.distinct_rgb >= min_distinct:
            out.append(info)
    return out


# ---------------------------------------------------------------------------
# Cross-reference resident CLUTs against the on-disc 1024-byte blob pool.
# ---------------------------------------------------------------------------
@dataclass
class DiscMatch:
    cbp: int
    swizzled: bool          # did the VRAM blob need un-swizzle to match disc?
    disc_sha1: str
    locations: list[tuple[str, int]]
    exact: bool             # exact byte match (vs. nearest)
    max_abs_diff: int       # 0 if exact; else worst per-byte diff to nearest


def crossref(localmem: bytes, cbps: list[int], pool) -> list[DiscMatch]:
    """For each CBP, test both the raw and un-swizzled VRAM blob against every
    disc-pool blob; report exact matches (or the nearest blob if none exact).

    ``pool`` is a list of clut_bruteforce.ClutCand (sha1, blob, locations).
    """
    by_sha = {c.sha1: c for c in pool}
    out: list[DiscMatch] = []
    for cbp in cbps:
        vram = read_clut_at(localmem, cbp)
        best: DiscMatch | None = None
        for swz in (False, True):
            cand_blob = csm1_unswizzle_clut(vram) if swz else vram
            h = hashlib.sha1(cand_blob).hexdigest()
            if h in by_sha:
                c = by_sha[h]
                out.append(DiscMatch(cbp, swz, h, c.locations, True, 0))
                best = None
                break
            # nearest by max-abs-diff (cheap; only used when no exact match)
            for c in pool:
                md = max(abs(a - b) for a, b in zip(cand_blob, c.blob))
                if best is None or md < best.max_abs_diff:
                    best = DiscMatch(cbp, swz, c.sha1, c.locations, False, md)
        else:
            if best is not None:
                out.append(best)
    return out


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------
def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Resident-CLUT recovery from a "
                                            "captured GS local-memory image")
    p.add_argument("--gs", type=Path, default=Path("/tmp/cap2/gs.bin"),
                   help="captured GS freeze blob (default: /tmp/cap2/gs.bin)")
    p.add_argument("--scan", action="store_true",
                   help="scan all VRAM for resident palette-shaped CLUTs")
    p.add_argument("--crossref", action="store_true",
                   help="cross-reference resident CLUTs against the disc pool")
    p.add_argument("--in", dest="root", default="extract",
                   help="extraction root for the disc CLUT pool (default: extract)")
    p.add_argument("--dump", type=lambda s: int(s, 0), action="append",
                   metavar="CBP", help="dump the un-swizzled CLUT at this CBP "
                                       "as a 16x16 swatch PNG (repeatable)")
    p.add_argument("--out", default="scratch/color",
                   help="output dir for swatch PNGs (default: scratch/color)")
    args = p.parse_args(argv)

    base, lm = read_localmem(args.gs)
    print(f"GS local-memory base in {args.gs}: {base} ({base:#x}); "
          f"VRAM size {len(lm)} ({len(lm):#x})")

    hits: list[PaletteInfo] = []
    if args.scan or args.crossref:
        hits = scan_vram_cluts(lm)
        print(f"resident palette-shaped CLUTs in VRAM: {len(hits)}")
        for h in hits:
            tag = " VIVID" if h.chroma > 20 else ""
            print(f"  CBP={h.cbp:6d} off={h.cbp*256:#08x} a80={h.a80:3d} "
                  f"distinctRGB={h.distinct_rgb:3d} chroma={h.chroma}{tag}")

    if args.crossref:
        from clut_bruteforce import collect_clut_pool  # noqa: E402
        root = Path(args.root)
        print(f"\ncollecting disc CLUT pool from {root} ...", flush=True)
        pool = collect_clut_pool(root)
        print(f"  {len(pool)} unique disc blobs")
        matches = crossref(lm, [h.cbp for h in hits], pool)
        print("\ncross-reference (VRAM CLUT -> on-disc blob):")
        for m in matches:
            kind = "EXACT" if m.exact else f"nearest(maxdiff={m.max_abs_diff})"
            sw = " (un-swizzled)" if m.swizzled else ""
            print(f"  CBP={m.cbp:6d}{sw}: {kind} {m.disc_sha1[:12]} "
                  f"@ {m.locations[0][0]}:{m.locations[0][1]:#x}"
                  f"{f' (+{len(m.locations)-1} more)' if len(m.locations) > 1 else ''}")

    if args.dump:
        out_dir = Path(args.out)
        out_dir.mkdir(parents=True, exist_ok=True)
        for cbp in args.dump:
            clut = csm1_unswizzle_clut(read_clut_at(lm, cbp))
            # 16x16 swatch (one pixel per palette entry, RGBA).
            write_png_rgba(out_dir / f"vram_clut_cbp{cbp}.png", 16, 16, clut)
            print(f"  dumped CBP {cbp} -> {out_dir}/vram_clut_cbp{cbp}.png")

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
