#!/usr/bin/env python3
"""PCSX2 GS dump (.gs) parser: per-draw TEX0 pairing table + colored textures.

Original tooling for the Extermination decompilation project. Ingests only a
GS dump the user recorded locally from their own copy of the game; nothing
disc-derived is redistributed. All output lands in git-ignored directories.

WHY THIS EXISTS
---------------
Characters and level geometry draw through VU1: their TEX0 qwords are built
in VIF/VU1 buffers and do NOT survive in a save-state freeze (see
docs/FINDINGS.md "Texture COLOR recovered"). A PCSX2 GS dump records every
GIF primitive of a frame, so it contains every draw's TEX0 — the complete,
authoritative texture<->CLUT pairing table — plus a snapshot of GS local
memory (same 425-byte-header freeze blob as the .p2s: VRAM at
``len - 0x400000 - 84``), so the dump alone suffices to emit colored PNGs.

CONTAINER LAYOUT (new format, magic 0xFFFFFFFF)
-----------------------------------------------
  u32 magic = 0xFFFFFFFF
  u32 header_size
  GSDumpHeader { u32 state_version, state_size, serial_offset, serial_size,
                 crc, ssw, ssh, ss_offset, ss_size }   (rest is screenshot)
  u8[state_size] state_data   (GS freeze blob, incl. 4 MB local memory)
  u8[8192]      regs_data     (GSPrivRegSet)
  packets:
    u8 id
      id==0 Transfer: u8 path, u32 length, u8[length] GIF data
      id==1 VSync:    u8 field
      id==2 ReadFIFO2: u32 size
      id==3 Registers: u8[8192]

USAGE (macOS arm64)
    python3 tools/parse_gsdump.py extract/gsdump/frame1.gs                 # summary
    python3 tools/parse_gsdump.py frame1.gs --json extract/gsdump/runs.json
    python3 tools/parse_gsdump.py frame1.gs --textures extract/textures_colored/gsdump
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from collections import Counter
from pathlib import Path

_HERE = Path(__file__).resolve().parent
if str(_HERE) not in sys.path:
    sys.path.insert(0, str(_HERE))

PATH_NAMES = {0: "PATH1old", 1: "PATH2", 2: "PATH3", 3: "PATH1new", 4: "Dummy"}

GIF_PACKED, GIF_REGLIST, GIF_IMAGE, GIF_IMAGE2 = 0, 1, 2, 3

# GS register addresses (A+D / REGLIST descriptors)
GS_PRIM, GS_TEX0_1, GS_TEX0_2 = 0x00, 0x06, 0x07
GS_TEX2_1, GS_TEX2_2 = 0x16, 0x17
GS_XYZF2, GS_XYZ2, GS_XYZF3, GS_XYZ3 = 0x04, 0x05, 0x0C, 0x0D
GS_ST, GS_UV = 0x02, 0x03


# ---------------------------------------------------------------------------
# Container
# ---------------------------------------------------------------------------
def parse(path, quiet=False):
    with open(path, "rb") as f:
        buf = f.read()
    o = 0
    magic, = struct.unpack_from("<I", buf, o); o += 4
    assert magic == 0xFFFFFFFF, f"unexpected magic {magic:#x}"
    header_size, = struct.unpack_from("<I", buf, o); o += 4
    hdr = buf[o:o+header_size]
    state_version, state_size, serial_off, serial_sz, crc, ssw, ssh, ss_off, ss_sz = \
        struct.unpack_from("<9I", hdr, 0)
    serial = hdr[serial_off:serial_off+serial_sz].decode("ascii", "replace")
    o += header_size
    state_data = buf[o:o+state_size]; o += state_size
    regs = buf[o:o+8192]; o += 8192

    transfer_counts = Counter()
    transfer_bytes = Counter()
    vsyncs = 0
    pkts = []
    while o < len(buf):
        pid = buf[o]; o += 1
        if pid == 0:                       # Transfer
            ppath = buf[o]; o += 1
            plen, = struct.unpack_from("<I", buf, o); o += 4
            data = buf[o:o+plen]; o += plen
            transfer_counts[ppath] += 1
            transfer_bytes[ppath] += plen
            pkts.append(("T", ppath, plen, data))
        elif pid == 1:                     # VSync
            field = buf[o]; o += 1
            vsyncs += 1
            pkts.append(("V", field))
        elif pid == 2:                     # ReadFIFO2
            sz, = struct.unpack_from("<I", buf, o); o += 4
            pkts.append(("R", sz))
        elif pid == 3:                     # Registers
            o += 8192
            pkts.append(("G",))
        else:
            print(f"unknown packet id {pid} at {o-1}; stopping")
            break

    if not quiet:
        print(f"magic OK; serial={serial!r} crc={crc:08x}")
        print(f"state_size={state_size} ({state_size:#x})  "
              f"screenshot={ssw}x{ssh} ss_size={ss_sz}")
        print(f"vsyncs in capture: {vsyncs}")
        for k, n in sorted(transfer_counts.items()):
            print(f"  path {PATH_NAMES[k]} ({k}): {n} packets, "
                  f"{transfer_bytes[k]} bytes")
    return state_data, regs, pkts, serial, crc


# ---------------------------------------------------------------------------
# TEX0 / PRIM field decoding
# ---------------------------------------------------------------------------
def tex0_fields(q: int) -> dict:
    return {
        "tbp0": q & 0x3FFF, "tbw": (q >> 14) & 0x3F, "psm": (q >> 20) & 0x3F,
        "tw": (q >> 26) & 0xF, "th": (q >> 30) & 0xF,
        "tcc": (q >> 34) & 1, "tfx": (q >> 35) & 3,
        "cbp": (q >> 37) & 0x3FFF, "cpsm": (q >> 51) & 0xF,
        "csm": (q >> 55) & 1, "csa": (q >> 56) & 0x1F, "cld": (q >> 61) & 7,
    }


def prim_fields(p: int) -> dict:
    return {
        "prim": p & 7, "iip": (p >> 3) & 1, "tme": (p >> 4) & 1,
        "fge": (p >> 5) & 1, "abe": (p >> 6) & 1, "aa1": (p >> 7) & 1,
        "fst": (p >> 8) & 1, "ctxt": (p >> 9) & 1, "fix": (p >> 10) & 1,
    }


def _f32(b: bytes, off: int) -> float:
    return struct.unpack_from("<f", b, off)[0]


# ---------------------------------------------------------------------------
# GIF stream walker with draw-run accumulation
# ---------------------------------------------------------------------------
class DrawTable:
    """Consumes GIF primitives across packets; groups vertex kicks into runs.

    A run = a maximal sequence of vertex kicks sharing (PRIM, active-context
    TEX0). Each run records vertex count and the ST/UV bounding box, which
    is what identifies the texture subrect a character piece samples."""

    def __init__(self):
        self.tex0 = [None, None]          # raw qwords, context 1/2
        self.prim = 0
        self.runs = []
        self._cur = None
        self.tex0_writes = 0
        self.image_qwords = 0             # IMAGE-mode data seen (uploads)

    # -- run bookkeeping ----------------------------------------------------
    def _ctx(self):
        return (self.prim >> 9) & 1

    def _begin_or_continue(self):
        key = (self.prim, self.tex0[self._ctx()])
        if self._cur is None or self._cur["key"] != key:
            self._flush()
            self._cur = {"key": key, "verts": 0, "kicks": 0,
                         "st": [None] * 4, "uv": [None] * 4,
                         "pending_st": None, "pending_uv": None}
        return self._cur

    def _flush(self):
        if self._cur and self._cur["kicks"]:
            prim_raw, tex0_raw = self._cur["key"]
            self.runs.append({
                "prim": prim_fields(prim_raw),
                "tex0_raw": tex0_raw,
                "tex0": tex0_fields(tex0_raw) if tex0_raw is not None else None,
                "verts": self._cur["verts"], "kicks": self._cur["kicks"],
                "st_bbox": self._cur["st"] if self._cur["st"][0] is not None else None,
                "uv_bbox": self._cur["uv"] if self._cur["uv"][0] is not None else None,
            })
        self._cur = None

    def _vertex(self, kick: bool):
        cur = self._begin_or_continue()
        cur["verts"] += 1
        cur["kicks"] += kick
        for which in ("st", "uv"):
            pend = cur["pending_" + which]
            if pend is not None:
                box = cur[which]
                if box[0] is None:
                    box[:] = [pend[0], pend[1], pend[0], pend[1]]
                else:
                    box[0] = min(box[0], pend[0]); box[1] = min(box[1], pend[1])
                    box[2] = max(box[2], pend[0]); box[3] = max(box[3], pend[1])

    # -- register writes ----------------------------------------------------
    def _write_reg(self, addr: int, data: int):
        if addr == GS_PRIM:
            self._flush()
            self.prim = data
        elif addr in (GS_TEX0_1, GS_TEX0_2):
            unit = addr - GS_TEX0_1
            if self.tex0[unit] != data:
                self._flush()
            self.tex0[unit] = data
            self.tex0_writes += 1
        elif addr in (GS_TEX2_1, GS_TEX2_2):
            # TEX2 updates the CLUT-side fields of TEX0 (PSM, CBP, CPSM,
            # CSM, CSA, CLD); texture-side fields persist.
            unit = addr - GS_TEX2_1
            keep = self.tex0[unit] or 0
            mask = ((0x3F << 20) | (0x3FFF << 37) | (0xF << 51) |
                    (1 << 55) | (0x1F << 56) | (7 << 61))
            merged = (keep & ~mask) | (data & mask)
            if self.tex0[unit] != merged:
                self._flush()
            self.tex0[unit] = merged
            self.tex0_writes += 1
        elif addr in (GS_XYZ2, GS_XYZF2):
            self._vertex(kick=True)
        elif addr in (GS_XYZ3, GS_XYZF3):
            self._vertex(kick=False)
        elif addr == GS_ST and self._cur is not None:
            pass                                   # raw ST via A+D: rare; skip
        # other registers don't affect the pairing table

    # -- packed descriptors ---------------------------------------------------
    def _packed_reg(self, desc: int, q: bytes):
        if desc == 0x0E:                           # A+D
            self._write_reg(q[8] & 0x7F, int.from_bytes(q[:8], "little"))
        elif desc in (GS_TEX0_1, GS_TEX0_2):
            self._write_reg(desc, int.from_bytes(q[:8], "little"))
        elif desc == GS_ST:
            cur = self._begin_or_continue()
            cur["pending_st"] = (_f32(q, 0), _f32(q, 4))
        elif desc == GS_UV:
            cur = self._begin_or_continue()
            u = (int.from_bytes(q[0:2], "little") & 0x3FFF) / 16.0
            v = (int.from_bytes(q[4:6], "little") & 0x3FFF) / 16.0
            cur["pending_uv"] = (u, v)
        elif desc in (GS_XYZ2, GS_XYZF2):
            adc = (q[15] >> 7) & 1                 # bit 111
            self._vertex(kick=not adc)
        elif desc == GS_PRIM:
            self._write_reg(GS_PRIM, int.from_bytes(q[:8], "little") & 0x7FF)
        # RGBAQ/CLAMP/FOG/NOP: no pairing impact

    # -- stream walk ----------------------------------------------------------
    def feed(self, data: bytes):
        """Walk one transfer packet's GIF data (PATH1 packets are complete
        XGKICK streams; this game uses no cross-packet continuation)."""
        o, n = 0, len(data)
        while o + 16 <= n:
            lo = int.from_bytes(data[o:o+8], "little")
            regs = int.from_bytes(data[o+8:o+16], "little")
            o += 16
            nloop = lo & 0x7FFF
            flg = (lo >> 58) & 3
            nreg = (lo >> 60) & 0xF or 16
            if (lo >> 46) & 1:                     # PRE: GIFtag sets PRIM
                self._write_reg(GS_PRIM, (lo >> 47) & 0x7FF)
            if nloop == 0:
                continue
            if flg == GIF_PACKED:
                descs = [(regs >> (4 * i)) & 0xF for i in range(nreg)]
                for _ in range(nloop):
                    for d in descs:
                        if o + 16 > n:
                            return
                        self._packed_reg(d, data[o:o+16])
                        o += 16
            elif flg == GIF_REGLIST:
                descs = [(regs >> (4 * i)) & 0xF for i in range(nreg)]
                total = nloop * nreg
                for i in range(total):
                    if o + 8 > n:
                        return
                    d = descs[i % nreg]
                    if d != 0x0E:                  # A+D is a NOP in REGLIST
                        self._write_reg(d, int.from_bytes(data[o:o+8], "little"))
                    o += 8
                if total & 1:
                    o += 8                         # pad to qword
            else:                                  # IMAGE / IMAGE2
                self.image_qwords += nloop
                o += nloop * 16

    def finish(self):
        self._flush()
        return self.runs


def build_table(pkts) -> DrawTable:
    dt = DrawTable()
    for p in pkts:
        if p[0] == "T":
            dt.feed(p[3])
    dt.finish()
    return dt


# ---------------------------------------------------------------------------
# Aggregation + texture emission
# ---------------------------------------------------------------------------
def aggregate(runs) -> list[dict]:
    """Unique textured pairings with draw counts and merged ST extents."""
    agg: dict = {}
    for r in runs:
        t = r["tex0"]
        if t is None or not r["prim"]["tme"]:
            continue
        key = (t["tbp0"], t["tbw"], t["psm"], t["tw"], t["th"], t["cbp"],
               t["csa"])
        e = agg.setdefault(key, {
            "tbp0": t["tbp0"], "tbw": t["tbw"], "psm": t["psm"],
            "width": 1 << t["tw"], "height": 1 << t["th"], "cbp": t["cbp"],
            "csa": t["csa"], "runs": 0, "verts": 0,
            "st_bbox": None, "uv_bbox": None})
        e["runs"] += 1
        e["verts"] += r["verts"]
        for which in ("st_bbox", "uv_bbox"):
            b = r[which]
            if b is None:
                continue
            if e[which] is None:
                e[which] = list(b)
            else:
                e[which] = [min(e[which][0], b[0]), min(e[which][1], b[1]),
                            max(e[which][2], b[2]), max(e[which][3], b[3])]
    return sorted(agg.values(), key=lambda e: (e["tbp0"], e["cbp"]))


def dump_vram(state_data: bytes) -> bytes:
    """GS local memory inside the dump's freeze blob — same layout as the
    .p2s v9 GS.bin: VRAM at len - 0x400000 - 84 (= 425)."""
    from gs_vram import GS_LOCALMEM_SIZE, VRAM_TRAILER
    base = len(state_data) - GS_LOCALMEM_SIZE - VRAM_TRAILER
    assert base >= 0, "state blob smaller than 4 MB VRAM + trailer"
    return state_data[base:base + GS_LOCALMEM_SIZE]


def emit_textures(state_data: bytes, pairs: list[dict], out_dir: Path,
                  source: str) -> dict:
    from clut import apply_clut, write_png_rgba
    from clut_pair import (read_clut16_rgba, read_clut_rgba, read_psmt4,
                           read_psmt8)

    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "cluts").mkdir(exist_ok=True)
    lm = dump_vram(state_data)

    manifest = {"source_dump": source,
                "pairing_rule": "per-draw TEX0 from the GS dump GIF stream "
                                "(authoritative; includes all VU1 draws)",
                "pairs": []}
    seen_cluts = set()
    for e in pairs:
        if e["psm"] not in (0x13, 0x14):           # PSMT8 / PSMT4
            continue
        w, h, cbp = e["width"], e["height"], e["cbp"]
        if e["psm"] == 0x13:
            idx = read_psmt8(lm, e["tbp0"], e["tbw"], w, h)
            pal = read_clut_rgba(lm, cbp)
            n_entries = 256
        else:
            idx = read_psmt4(lm, e["tbp0"], e["tbw"], w, h)
            pal16 = read_clut16_rgba(lm, cbp)
            pal = pal16 + bytes(1024 - 64)         # apply_clut wants 256 ents
            n_entries = 16
        opaque = sum(1 for i in range(3, n_entries * 4, 4) if pal[i] >= 0x80)
        distinct = len({pal[i:i + 3] for i in range(0, n_entries * 4, 4)})
        resident = opaque >= n_entries // 2 and distinct >= n_entries // 4
        name = f"tbp{e['tbp0']}_{w}x{h}_cbp{cbp}.png"
        write_png_rgba(out_dir / name, w, h, apply_clut(idx, pal))
        if cbp not in seen_cluts:
            seen_cluts.add(cbp)
            side = 16 if n_entries == 256 else 4
            write_png_rgba(out_dir / "cluts" / f"cbp{cbp}.png", side, side,
                           pal[:n_entries * 4])
        rec = dict(e)
        rec["png"] = name
        rec["clut_resident"] = resident
        rec["clut"] = {"entries": n_entries, "opaque_entries": opaque,
                       "distinct_rgb": distinct}
        manifest["pairs"].append(rec)
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2))
    return manifest


# ---------------------------------------------------------------------------
def main(argv):
    ap = argparse.ArgumentParser(description="GS dump TEX0/draw analysis")
    ap.add_argument("dump", type=Path)
    ap.add_argument("--json", type=Path, help="write per-run draw table")
    ap.add_argument("--textures", type=Path,
                    help="emit colored PNGs from dump VRAM (git-ignored dir)")
    args = ap.parse_args(argv)

    state_data, regs, pkts, serial, crc = parse(args.dump)
    dt = build_table(pkts)
    runs = dt.runs
    pairs = aggregate(runs)

    textured = [r for r in runs if r["prim"]["tme"] and r["tex0"]]
    print(f"\nGIF walk: {dt.tex0_writes} TEX0 writes, {len(runs)} draw runs "
          f"({len(textured)} textured), image-mode qwords: {dt.image_qwords}")
    print(f"unique textured pairings: {len(pairs)}")
    for e in pairs:
        st = e["st_bbox"] or e["uv_bbox"]
        sts = (f"  st=({st[0]:.3f},{st[1]:.3f})..({st[2]:.3f},{st[3]:.3f})"
               if st else "")
        print(f"  TBP0={e['tbp0']:5d} {e['width']:4d}x{e['height']:<4d} "
              f"psm={e['psm']:#04x} CBP={e['cbp']:5d} csa={e['csa']:2d} "
              f"runs={e['runs']:3d} verts={e['verts']:5d}{sts}")

    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(
            {"serial": serial, "crc": f"{crc:08x}",
             "runs": runs, "unique_pairs": pairs}, indent=1))
        print(f"wrote {args.json}")
    if args.textures:
        m = emit_textures(state_data, pairs, args.textures, str(args.dump))
        print(f"emitted {len(m['pairs'])} colored textures -> {args.textures}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
