#!/usr/bin/env python3
"""clut_pair.py - Colored texture extraction from a PCSX2 .p2s save state.

Original tooling for the Extermination decompilation project. Ingests only the
user's own PCSX2 save state (and, optionally, the user's locally extracted
disc data for verification); redistributes nothing. All output lands under a
git-ignored directory (default: ``extract/textures_colored/``).

WHAT THIS TOOL DOES
-------------------
Extermination's PSMT8 textures sample 256-entry CLUTs that are synthesised at
runtime per-entry into GS VRAM (no disc blob matches any resident palette --
see docs/FINDINGS.md). Color is therefore only recoverable from a live frame.
This tool takes one .p2s save state and emits every recoverable
(texture, CLUT) pairing of that frame as colored RGBA PNGs plus a JSON
manifest, by combining:

  1. the 4 MB GS local memory inside the save state's GS freeze blob
     (texture indices AND palettes both live there), and
  2. the engine-built ``TEX0`` register qwords still present in EE RAM
     (and VU1 data memory), which carry the authoritative pairing.

THE PAIRING RULE (what a future exporter must reproduce)
--------------------------------------------------------
There is NO static texture->palette table on disc. The engine builds a
``TEX0`` qword per draw call; that qword is the ONLY place the association
exists:

    TEX0 (PSMT8 draws):
      TBP0 [0:13]   texture base pointer, 256-byte GS blocks
      TBW  [14:19]  buffer width /64 texels (always 8 here -> 512-wide)
      PSM  [20:25]  0x13 = PSMT8
      TW/TH[26:33]  log2 width/height
      CBP  [37:50]  CLUT base pointer, 256-byte GS blocks
      CPSM [51:54]  0 = PSMCT32 palette,  CSM [55] = 0 = CSM1,
      CSA  [56:60]  0 for PSMT8,          CLD [61:63] 1 = (re)load CLUT

Harvesting: the engine stages its GIF/VIF command buffers in EE RAM ring
buffers that survive into the save state. Scanning every 8-byte-aligned
qword for PSM==0x13 with a strict plausibility filter (TW/TH in 4..10,
TBW in {2,4,8,16}, CPSM=CSM=CSA=0, CLD<=5, 0<CBP<0x4000) recovers the
frame's (TBP0 -> CBP) pairings; the same value's repeat count approximates
its draw count. Pairings whose CBP currently holds a valid resident palette
(alpha all <= 0x80, mostly opaque, many distinct colors) are CURRENT;
the rest are stale slots whose palette has since been recycled.

Observed allocator structure (gameplay capture, SCUS-97112 snow level):
  * big 512-wide sheets are treated as a sequence of 256x128-texel strips
    (64 blocks each); strip k at TBP0 = sheet_base + 64*k pairs with
    CBP = clut_base + 4*k (one 1024-byte CLUT per strip, allocated
    contiguously: observed 7424+64k <-> 8368+4k).
  * small cache entries pack at 16-block (64x64) / 4-block (32x32)
    granularity; a 64x64 entry occupies exactly 16 contiguous blocks,
    a 32x32 entry 4 blocks (GS block layout is hierarchical/Z-order).
  * CLUTs are stored in CSM1 order: recover the linear palette with the
    entries 8..15 <-> 16..23 swap per 32-entry group
    (clut.psmct8_csm1_swizzle). Alpha is 0..0x80 (scale x2 for PNG).

GS FREEZE LAYOUT -- VRAM BASE IS 425, NOT 509 (correction, 2026-06-09)
----------------------------------------------------------------------
In the .p2s ``GS.bin`` freeze blob (v9), GS local memory starts at byte
offset ``len - 0x400000 - 84`` (= 425 for the 4194813-byte blob), i.e.
there are 84 bytes of trailing freeze state AFTER the 4 MB. The earlier
``len - 0x400000`` (= 509) reading (fixed in gs_vram.py on 2026-06-09)
was off by 84 bytes, which scrambles every 256-byte-block-addressed
read at sub-block level.
Proof: simulating a known disc texture upload through the documented
PSMCT32 page/block/column swizzle matches the captured VRAM byte-exactly
(full 8 KB pages) only with base 425 -- every match lands 256-byte
block-aligned there. ``--vram-base`` overrides; ``detect`` (default)
scores both candidates by how many TEX0-referenced CBPs validate as
resident palettes.

LIMITATIONS (honest)
--------------------
* Level geometry and skinned characters are drawn through the VU1 path:
  their TEX0s are built inside VU1/VIF buffers and do NOT persist in EE
  RAM; VU1 dmem only retains the final batch of the frame (here: the
  snowflake particles). Their sub-texture palettes are therefore NOT all
  recoverable from a save state alone. A PCSX2 **GS dump** (.gs), which
  records every register write of a frame, contains every draw's TEX0 --
  that is the right capture format to wire color into the glTF/EMDL
  exporters (see tools/parse_gsdump.py).
* Colors are frame-bound: palettes are modulated by level lighting at
  build time, so a night level yields night-tinted palettes.

USAGE (macOS arm64, repo .venv has the required ``zstandard``)
    .venv/bin/python tools/clut_pair.py \
        "$HOME/Library/Application Support/PCSX2/sstates/SCUS-97112 (4CDC5F74).01.p2s" \
        --out extract/textures_colored/01

Outputs: ``tbp{TBP0}_{W}x{H}_cbp{CBP}.png`` (current pairings),
``stale/...`` (recycled CLUT slot -- palette no longer resident),
``cluts/cbp{CBP}.png`` 16x16 swatches, and ``manifest.json``.
"""
from __future__ import annotations

import argparse
import json
import sys
import tempfile
from collections import Counter
from pathlib import Path

_HERE = Path(__file__).resolve().parent
if str(_HERE) not in sys.path:
    sys.path.insert(0, str(_HERE))

from clut import apply_clut, write_png_rgba                    # noqa: E402
from extract_textures import psmt8_byte                       # noqa: E402
from gs_vram import (GS_LOCALMEM_SIZE, VRAM_TRAILER,          # noqa: E402
                     characterise, csm1_unswizzle_clut)


# ---------------------------------------------------------------------------
# TEX0 harvesting from EE RAM / VU1 dmem
# ---------------------------------------------------------------------------
def tex0_fields(q: int) -> dict:
    return {
        "tbp0": q & 0x3FFF, "tbw": (q >> 14) & 0x3F, "psm": (q >> 20) & 0x3F,
        "tw": (q >> 26) & 0xF, "th": (q >> 30) & 0xF,
        "cbp": (q >> 37) & 0x3FFF, "cpsm": (q >> 51) & 0xF,
        "csm": (q >> 55) & 1, "csa": (q >> 56) & 0x1F, "cld": (q >> 61) & 7,
    }


def plausible_psmt8_tex0(f: dict) -> bool:
    """Strict filter: engine-built PSMT8 TEX0s observed in this game always
    have TBW=8 (512-wide buffer), CPSM=PSMCT32, CSM1, CSA=0; allow a little
    slack on TBW for safety. Sizes 16x16 .. 1024x1024."""
    return (f["psm"] == 0x13
            and 4 <= f["tw"] <= 10 and 4 <= f["th"] <= 10
            and f["tbw"] in (2, 4, 8, 16)
            and f["cpsm"] == 0 and f["csm"] == 0 and f["csa"] == 0
            and f["cld"] <= 5
            and 0 < f["cbp"] < 0x4000)


def harvest_tex0(blobs: list[bytes]) -> Counter:
    """Scan memory images for plausible PSMT8 TEX0 qwords (8-byte aligned).
    Returns Counter[(tbp0, tbw, w, h, cbp)] -> occurrence count."""
    pairs: Counter = Counter()
    for d in blobs:
        for off in range(0, len(d) - 8, 8):
            q = int.from_bytes(d[off:off + 8], "little")
            if (q >> 20) & 0x3F != 0x13:        # cheap pre-test
                continue
            f = tex0_fields(q)
            if plausible_psmt8_tex0(f):
                pairs[(f["tbp0"], f["tbw"], 1 << f["tw"], 1 << f["th"],
                       f["cbp"])] += 1
    return pairs


# ---------------------------------------------------------------------------
# GS local memory access
# ---------------------------------------------------------------------------
def vram_candidates(gs_bytes: bytes) -> list[int]:
    """Candidate byte offsets of VRAM word 0 inside the GS freeze blob."""
    end = len(gs_bytes)
    return [end - GS_LOCALMEM_SIZE - VRAM_TRAILER,   # correct for .p2s v9
            end - GS_LOCALMEM_SIZE]                  # legacy reading


def detect_vram_base(gs_bytes: bytes, cbps: list[int]) -> int:
    """Pick the base at which the most TEX0-referenced CBPs hold valid
    resident palettes. Falls back to the .p2s-v9 base when nothing scores."""
    best, best_n = None, -1
    for base in vram_candidates(gs_bytes):
        if base < 0:
            continue
        lm = gs_bytes[base:base + GS_LOCALMEM_SIZE]
        n = 0
        for cbp in cbps:
            info = characterise(csm1_unswizzle_clut(
                lm[cbp * 256:cbp * 256 + 1024]), cbp)
            if info.valid and info.a80 >= 100 and info.distinct_rgb >= 32:
                n += 1
        if n > best_n:
            best, best_n = base, n
    return best if best is not None else vram_candidates(gs_bytes)[0]


def read_psmt8(lm: bytes, tbp0: int, tbw: int, w: int, h: int) -> bytes:
    """Read a PSMT8 texture out of GS local memory through the hardware
    page/block/column swizzle. TBP0 in 256-byte blocks; TBW in 64-texel
    units (pages-per-row = TBW*64/128)."""
    ppr = max(1, (tbw * 64) // 128)
    base = tbp0 * 256
    out = bytearray(w * h)
    for y in range(h):
        row = y * w
        for x in range(w):
            out[row + x] = lm[(base + psmt8_byte(x, y, ppr)) & 0x3FFFFF]
    return bytes(out)


def read_clut_rgba(lm: bytes, cbp: int) -> bytes:
    """Resident CLUT at ``cbp`` -> linear RGBA palette, PNG-ready alpha."""
    lin = bytearray(csm1_unswizzle_clut(lm[cbp * 256:cbp * 256 + 1024]))
    for i in range(3, 1024, 4):                  # GS alpha 0..0x80 -> 0..255
        lin[i] = min(255, lin[i] * 2)
    return bytes(lin)


def read_psmt4(lm: bytes, tbp0: int, tbw: int, w: int, h: int) -> bytes:
    """Read a PSMT4 texture out of GS local memory (one byte per texel,
    values 0..15). TBP0 in 256-byte blocks; TBW in 64-texel units
    (pages-per-row = TBW*64/128)."""
    from extract_textures import psmt4_nibble
    ppr = max(1, (tbw * 64) // 128)
    base = tbp0 * 512                            # nibble address
    out = bytearray(w * h)
    for y in range(h):
        row = y * w
        for x in range(w):
            a = base + psmt4_nibble(x, y, ppr)
            b = lm[(a >> 1) & 0x3FFFFF]
            out[row + x] = (b >> 4) & 0xF if a & 1 else b & 0xF
    return bytes(out)


# CSM1 16-entry CLUT (PSMT4, CPSM=PSMCT32): the entries occupy the first 64
# bytes at CBP as an 8x2 PSMCT32 region; entry i sits at word CLUT16_WORD[i].
CLUT16_WORD = [0, 1, 4, 5, 8, 9, 12, 13, 2, 3, 6, 7, 10, 11, 14, 15]


def read_clut16_rgba(lm: bytes, cbp: int) -> bytes:
    """Resident 16-entry PSMT4 CLUT at ``cbp`` -> 64-byte linear RGBA
    palette, PNG-ready alpha."""
    blk = lm[cbp * 256:cbp * 256 + 64]
    lin = bytearray(64)
    for i, wi in enumerate(CLUT16_WORD):
        lin[i * 4:i * 4 + 4] = blk[wi * 4:wi * 4 + 4]
    for i in range(3, 64, 4):                    # GS alpha 0..0x80 -> 0..255
        lin[i] = min(255, lin[i] * 2)
    return bytes(lin)


# ---------------------------------------------------------------------------
# Main extraction
# ---------------------------------------------------------------------------
def extract_state(p2s: Path, out_dir: Path, vram_base: int | None = None,
                  min_draws: int = 1) -> dict:
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "stale").mkdir(exist_ok=True)
    (out_dir / "cluts").mkdir(exist_ok=True)

    # --- obtain the memory images -----------------------------------------
    if p2s.is_dir():                       # pre-extracted state directory
        state_dir = p2s
    else:
        from parse_pcsx2_state import extract_all
        state_dir = Path(tempfile.mkdtemp(prefix="clut_pair_"))
        extract_all(p2s, state_dir)
    gs = (state_dir / "gs.bin").read_bytes()
    ee = (state_dir / "ee.bin").read_bytes()
    vu1 = (state_dir / "vu1_dmem.bin").read_bytes() \
        if (state_dir / "vu1_dmem.bin").is_file() else b""
    shot = state_dir / "Screenshot.png"
    if shot.is_file():
        (out_dir / "screenshot_ref.png").write_bytes(shot.read_bytes())

    # --- harvest pairings ---------------------------------------------------
    pairs = harvest_tex0([ee, vu1])
    cbps = sorted({k[4] for k in pairs})
    base = vram_base if vram_base is not None else detect_vram_base(gs, cbps)
    lm = gs[base:base + GS_LOCALMEM_SIZE]

    # --- render -------------------------------------------------------------
    manifest = {
        "source_state": str(p2s),
        "vram_base": base,
        "vram_base_note": ("GS local memory starts at len(gs.bin)-0x400000-84 "
                           "in the .p2s v9 freeze (84 trailing state bytes)"),
        "pairing_rule": ("texture<->CLUT binding is the runtime-built TEX0 "
                         "qword (TBP0 -> CBP); harvested from EE RAM/VU1 dmem "
                         "command buffers. No disc-level binding exists; "
                         "palettes are engine-synthesised into GS VRAM."),
        "pairs": [],
        "resident_cluts": [],
    }

    seen_cluts: set[int] = set()
    for (tbp0, tbw, w, h, cbp), n in sorted(pairs.items()):
        if n < min_draws:
            continue
        info = characterise(csm1_unswizzle_clut(
            lm[cbp * 256:cbp * 256 + 1024]), cbp)
        resident = info.valid and info.a80 >= 100 and info.distinct_rgb >= 32
        confidence = ("high" if resident and n >= 8 else
                      "medium" if resident else "low")
        name = f"tbp{tbp0}_{w}x{h}_cbp{cbp}.png"
        dest = out_dir / name if resident else out_dir / "stale" / name
        rgba = apply_clut(read_psmt8(lm, tbp0, tbw, w, h),
                          read_clut_rgba(lm, cbp))
        write_png_rgba(dest, w, h, rgba)
        if cbp not in seen_cluts:
            seen_cluts.add(cbp)
            write_png_rgba(out_dir / "cluts" / f"cbp{cbp}.png", 16, 16,
                           read_clut_rgba(lm, cbp))
        manifest["pairs"].append({
            "tbp0": tbp0, "tbw": tbw, "width": w, "height": h, "cbp": cbp,
            "draws_in_ee_ram": n,
            "clut": {"valid_alpha": info.valid, "opaque_entries": info.a80,
                     "distinct_rgb": info.distinct_rgb, "chroma": info.chroma},
            "clut_resident": resident, "confidence": confidence,
            "png": str(dest.relative_to(out_dir)),
        })

    from gs_vram import scan_vram_cluts
    manifest["resident_cluts"] = [
        {"cbp": hh.cbp, "chroma": hh.chroma, "opaque_entries": hh.a80,
         "distinct_rgb": hh.distinct_rgb}
        for hh in scan_vram_cluts(lm)]

    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2))
    return manifest


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description="Colored PSMT8 texture extraction "
                                             "from a PCSX2 .p2s save state")
    ap.add_argument("p2s", type=Path,
                    help=".p2s save state (or a pre-extracted state dir)")
    ap.add_argument("--out", type=Path,
                    default=Path("extract/textures_colored/out"),
                    help="output directory (must be git-ignored)")
    ap.add_argument("--vram-base", type=int, default=None,
                    help="override GS local-memory byte offset in gs.bin "
                         "(default: auto-detect; 425 for the v9 .p2s freeze)")
    ap.add_argument("--min-draws", type=int, default=1,
                    help="only emit pairings seen at least N times (default 1)")
    args = ap.parse_args(argv)

    m = extract_state(args.p2s, args.out, args.vram_base, args.min_draws)
    cur = [p for p in m["pairs"] if p["clut_resident"]]
    print(f"VRAM base: {m['vram_base']}")
    print(f"pairings: {len(m['pairs'])} total, {len(cur)} with resident CLUT")
    for p in cur:
        print(f"  TBP0={p['tbp0']:5d} {p['width']:4d}x{p['height']:<4d} "
              f"CBP={p['cbp']:5d} draws={p['draws_in_ee_ram']:4d} "
              f"chroma={p['clut']['chroma']:5.1f} conf={p['confidence']} "
              f"-> {p['png']}")
    print(f"resident palette-shaped CLUTs in VRAM: "
          f"{len(m['resident_cluts'])}")
    print(f"output: {args.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
