#!/usr/bin/env python3
"""export_level.py — export a LEVEL RENDER MESH as a static EMDL v2 (.emdl)
asset for the native port (extermination-port).

THE LEVEL RENDER MESH (decoded 2026-06-09 s6; record phase + region map
CORRECTED 2026-06-09 s8 against the live resident stream). Each level chunk
carries a VU1-ready render-mesh file of 64-byte records framed in VIF blocks
(`STCYCL(4,4)` + `UNPACK V4-32 0x80qw`, 32 records per block, padded by
duplicating the last record). The record's four 16-byte rows are in the
CHARACTER/OBJECT order — the file just begins mid-record (a stray pos row at
offset 0), which previously made the walker pair every position with the
NEXT record's TEX0/UV/color (off-by-one-row bug — wrong textures, broken
strips):

    +0x00  the draw's full TEX0 register value (8 bytes, CBP included,
           verbatim) + 8 zero bytes
    +0x10  vec4 (u, v, 1.0, 0.0) — normalized per-texture ST, REPEAT
    +0x20  vec4 baked vertex color (r, g, b, 1.0) OR unit normal
           (|xyz|~=1, w~=0)
    +0x30  vec4 position (x, y, z, w); |w| ~= 1.0.  W as an int: bits
           0..9 = VU1 dmem matrix-slot address (slot = bits>>3), bit 15 =
           the GS PACKED XYZ2 ADC bit verbatim (the W word lands in output
           word 3, so bit 15 == ADC bit 111: SET = prime/no kick, CLEAR =
           the vertex kicks a triangle), bit 14 (mirrored in the float
           sign) = strip parity (winding), bit 13 = per-vertex flag of
           unconfirmed meaning (not topology).

Triangle assembly is plain GS tristrip semantics per VIF block: every
record enters the running strip; a CLEAR bit-15 vertex with two
predecessors emits a triangle; nothing else breaks the strip (TEX0 may
change MID-STRIP — the triangle belongs to the TEX0 of its kicking
vertex).

REGION MAP (chunk06.n1/f03_id43.bin, live-verified frames, office scene +
west rooms — sessions 8 and 9). The file is not all world-space:

  * 0x000000..0x0820C8  static world (slot 0, world space) — drawn through
                        the level kernel (CALL 0x237180) with camera K_L.
  * 0x088840..0x099C80  a 13-slot SUB-OBJECT assembly (the corridor door at
                        (75, 0, -188)) — object-space records whose slot
                        bits select one of 13 live matrices uploaded to
                        VU1 dmem (W = K_L^-1 * M per set, validated
                        orthonormal).
  * 0x09A140..0x09A960  3-slot door CONTROL PANEL, all slots at the
                        corridor door (75, 0, -188.2) (s9; drawn through a
                        late-pass call packet alongside glow quads).
  * 0x09AAC0..0x0A0420  3-slot wall-mounted STATION (ammo/refill unit) at
                        (57.5..60.8, ~15, -290..-296) (s9).
  * standalone object blobs, drawn through the OBJECT kernel (CALL
                        0x23C750) with per-unit placements:
                        0xA05C0 double door — TWO doorway instances:
                        office (109, 0, -252.2)/(101.3, 9, -252.5) and
                        west (57, 0, -220.5)/(57.25, 9, -228.2) (s9);
                        0xA2740 supply crate, drawn twice (s9);
                        0xA3040/0xA3940 pickup items, each drawn twice
                        (these match type-0xB entries in the live
                        placement table at 0x828170 — item pickups, baked
                        statically here);
                        0xA4240 table-top device ("battery bank") at
                        (80.1, 8.2, -244) (s9);
                        0xA8440 lockers at (116.2, 8, -184).
  * everything else     non-record data / tiny tail [0xAC540,end) —
                        skipped.

The exporter bakes the live placements statically. Glow/billboard quads
(library models 20/21/110-118, non-rigid or 2-tri overlays) are NOT
exported. Other level files fall back to "whole file static" (their
movables need their own live capture).

Output: one EMDL v2, bone_count 1 (identity palette), baked vertex color
in the EMD2 normal slot (header flags bit 0). Textures resolve to RGBA8
through a one-frame PCSX2 GS dump's VRAM snapshot (PSMT4+CLUT16 / PSMT8 /
PSMCT32).

Disc-derived output: write only into git-ignored locations
(extermination-port/assets/ is ignored there).

Usage (macOS arm64, decomp repo root):
  .venv/bin/python tools/export_level.py \
      --level extract/chunk06.n1/f03_id43.bin \
      --gsdump extract/gsdump/frame1.gs \
      --out ../extermination-port/assets/scene/00_level.emdl
"""
from __future__ import annotations

import argparse
import importlib.util
import json
import math
import struct
import sys
from pathlib import Path

TOOLS = Path(__file__).resolve().parent


def _load(name: str, fname: str):
    spec = importlib.util.spec_from_file_location(name, TOOLS / fname)
    mod = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules[name] = mod
    spec.loader.exec_module(mod)
    return mod


en = _load("_export_native", "export_native.py")   # tex0_fields, write_emdl

REC = 64
W_TOL = 0.25

IDENT34 = [[1.0, 0.0, 0.0, 0.0], [0.0, 1.0, 0.0, 0.0], [0.0, 0.0, 1.0, 0.0]]

# ---------------------------------------------------------------------------
# Live placements for chunk06.n1/f03_id43.bin (office scene), recovered
# 2026-06-09 s8 from the running game's per-frame DMA chain: per draw unit,
# W = K_L^-1 * M (K_L = the static level draw's camera upload; M = the
# unit's dmem matrix set). Rows are the first three rows of the affine
# world matrix [R | t].

SUBOBJ_SLOTS = [   # 13-set palette, REF 4420qw @ file 0x88840 (corridor door)
    [[0.0, -0.0, 1.0, 74.999991], [0.0, 1.0, 0.0, 0.000004], [-1.0, 0.0, 0.0, -188.199997]],
    [[0.0, -0.0, 1.0, 74.999991], [0.0, 1.0, 0.0, 0.000004], [-1.0, 0.0, 0.0, -188.199997]],
    [[0.0, -0.0, 1.0, 72.299978], [0.0, 1.0, 0.0, 5.010016], [-1.0, 0.0, 0.0, -184.699261]],
    [[-0.0, -0.0, -1.0, 72.299981], [-0.0, 1.0, -0.0, 5.010055], [1.0, 0.0, -0.0, -191.700766]],
    [[0.0, -0.0, 1.0, 79.191377], [0.0, 1.0, 0.0, 5.01002], [-1.0, 0.0, 0.0, -185.899879]],
    [[0.0, -0.0, 1.0, 74.999991], [0.0, 1.0, 0.0, 0.000004], [-1.0, 0.0, 0.0, -188.199997]],
    [[-0.0, -0.0, -1.0, 67.849986], [-0.0, 1.0, -0.0, 5.010003], [1.0, 0.0, -0.0, -189.299983]],
    [[0.0, -0.0, 1.0, 67.849986], [0.0, 1.0, 0.0, 5.010006], [-1.0, 0.0, 0.0, -187.099964]],
    [[0.0, -0.0, 1.0, 74.999997], [0.0, 1.0, 0.0, 0.000066], [-1.0, 0.0, 0.0, -189.550035]],
    [[-0.0, -0.0, -1.0, 79.19138], [-0.0, 1.0, -0.0, 5.010064], [1.0, 0.0, -0.0, -190.500127]],
    [[0.0, -0.0, 1.0, 72.313878], [0.0, 1.0, 0.0, 4.609347], [-1.0, 0.0, 0.0, -186.549028]],
    [[-0.0, -0.0, -1.0, 72.313875], [-0.0, 1.0, -0.0, 4.609303], [1.0, 0.0, -0.0, -189.851001]],
    [[0.0, -0.0, 1.0, 74.999991], [0.0, 1.0, 0.0, 0.000004], [-1.0, 0.0, 0.0, -188.199997]],
]

BLOB_A8440 = [   # 1040qw @ 0xA8440, one instance
    [[-0.0, -0.0, -1.0, 116.199986], [-0.0, 1.0, -0.0, 8.000108], [1.0, 0.0, 0.0, -184.000062]],
]
BLOB_A05C0 = [   # 520qw @ 0xA05C0, ONE unit with 2 slots (records pick 0/1)
    [[1.0, -0.0, -0.0, 108.999973], [0.0, 1.0, -0.0, 0.000004], [0.0, 0.0, 1.0, -252.200051]],
    [[1.0, -0.0, 0.0, 101.310513], [0.0, 1.0, -0.0, 8.999992], [0.0, 0.0, 1.0, -252.451882]],
]
BLOB_A05C0_W = [   # the SAME 2-slot double door at the WEST doorway (s9 live)
    [[0.0, 0.0, -1.0, 56.999988], [-0.0, 1.0, 0.0, 0.000006], [1.0, -0.0, 0.0, -220.499989]],
    [[0.0, -0.0, -1.0, 57.251972], [-0.0, 1.0, 0.0, 9.000002], [1.0, -0.0, 0.0, -228.189473]],
]
SUBOBJ_9A100 = [   # 130qw 3-slot door control panel; all slots at the corridor door (s9)
    [[-0.0, 0.0, 1.0, 74.999983], [0.0, 1.0, -0.0, -0.000008], [-1.0, -0.0, 0.0, -188.199956]],
] * 3
SUBOBJ_9AA80 = [   # 1430qw 3-slot wall station ("ammo refill"), SW room (s9)
    [[-0.0, 0.0, 1.0, 57.500001], [0.0, 1.0, -0.0, 14.999998], [-1.0, -0.0, 0.0, -292.600003]],
    [[-0.0, 0.0, 1.0, 59.999936], [0.0, 1.0, -0.0, 14.900021], [-1.0, -0.0, 0.0, -295.999924]],
    [[-0.0, 0.0, 1.0, 60.799965], [-0.0, 1.0, -0.0, 15.350014], [-1.0, -0.0, -0.0, -289.999945]],
]
BLOB_A2700 = [   # 130qw supply crate, TWO instances, south room floor (s9)
    [[0.962218, 0.0, 0.27228, 75.699979], [0.0, 1.0, 0.0, -0.000012], [-0.27228, -0.0, 0.962218, -301.999949]],
    [[0.999194, 0.0, -0.040133, 82.500003], [0.0, 1.0, -0.0, -0.000025], [0.040133, -0.0, 0.999194, -302.399962]],
]
BLOB_A4240 = [   # 1040qw table-top device ("battery bank"), one instance (s9)
    [[1.0, 0.0, 0.0, 80.099981], [-0.0, 1.0, -0.0, 8.199997], [0.0, -0.0, 1.0, -243.999971]],
]
BLOB_A3940 = [   # 0x82qw @ 0xA3940, TWO instances
    [[0.99956, -0.0, 0.029672, 115.599991], [-0.0, 1.0, -0.0, 9.399987], [-0.029672, 0.0, 0.99956, -280.09995]],
    [[0.999945, -0.0, -0.010472, 116.299984], [0.0, 1.0, -0.0, 1.499905], [0.010472, 0.0, 0.999945, -289.899902]],
]
BLOB_A3040 = [   # 0x82qw @ 0xA3040, TWO instances
    [[0.978147, -0.0, 0.207912, 116.300018], [-0.0, 1.0, -0.0, 9.400155], [-0.207912, 0.0, 0.978148, -266.600032]],
    [[0.999194, -0.0, 0.040133, 116.599989], [0.0, 1.0, -0.0, 1.500027], [-0.040133, 0.0, 0.999194, -264.199968]],
]

# region = (lo, hi, mode, mats); mode "world" ignores mats, "slots" indexes
# mats by the record slot bits, "instances" replays the region once per mat.
CHUNK06N1_REGIONS = [
    (0x000000, 0x0820C8, "world", None),
    (0x088840, 0x099C80, "slots", SUBOBJ_SLOTS),
    (0x09A140, 0x09A960, "slots", SUBOBJ_9A100),
    (0x09AAC0, 0x0A0420, "slots", SUBOBJ_9AA80),
    (0x0A05C0, 0x0A2640, "slots", BLOB_A05C0),
    (0x0A05C0, 0x0A2640, "slots", BLOB_A05C0_W),   # second doorway instance
    (0x0A2740, 0x0A2F60, "instances", BLOB_A2700),
    (0x0A3040, 0x0A3860, "instances", BLOB_A3040),
    (0x0A3940, 0x0A41C0, "instances", BLOB_A3940),
    (0x0A4240, 0x0A8340, "instances", BLOB_A4240),
    (0x0A8440, 0x0AC540, "instances", BLOB_A8440),
]
CHUNK06N1_SHA1 = "unpinned"   # match by name+size instead (disc data stays local)
CHUNK06N1_SIZE = 0xAC800

# The chunk's id 0x44 file is the collision world (FINDINGS s7) BUT its
# tail carries a SECOND STATIC RENDER-MESH SECTION (448 VIF blocks, same
# record format, slot 0, world space, X[37,120] — the western part of the
# area). The live static level draw REFs blocks from BOTH files (the
# session-6 "id44 markers are not TEX0s" verdict predates the record-phase
# fix and is wrong for this section). (lo, hi) below = the section extent.
CHUNK06N1_SIBLING = ("f02_id44.bin", 0x086B40, 0x173800)


# ---------------------------------------------------------------------------
# Record walker (corrected phase: [TEX0][ST][color/normal][pos+W])

def walk_records(d: bytes, lo: int = 0, hi: int | None = None):
    """Yield (off, pos, wbits, tex0_qword, uv, attr) for every valid record
    in [lo, hi), or None once per skipped-row run (block headers / VIF tags)
    so the caller can break the running strip; resyncs by 16 bytes."""
    o = lo
    n = len(d) if hi is None else hi
    skipping = False
    while o + REC <= n:
        is_tex = (d[o + 8:o + 16] == b"\x00" * 8 and
                  d[o:o + 8] != b"\x00" * 8)
        st_zw = struct.unpack_from("<2f", d, o + 0x18)
        w = struct.unpack_from("<f", d, o + 0x3C)[0]
        if is_tex and st_zw[0] == 1.0 and st_zw[1] == 0.0 \
                and abs(abs(w) - 1.0) < W_TOL:
            q = int.from_bytes(d[o:o + 8], "little")
            uv = struct.unpack_from("<2f", d, o + 0x10)
            attr = struct.unpack_from("<4f", d, o + 0x20)
            pos = struct.unpack_from("<3f", d, o + 0x30)
            wbits = struct.unpack_from("<I", d, o + 0x3C)[0]
            yield o, pos, wbits, q, uv, attr
            o += REC
            skipping = False
        else:
            if not skipping:
                yield None
                skipping = True
            o += 16     # header / VIF-tag row — resync
    return


def mat_apply(m, v):
    return (m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3],
            m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3],
            m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3])


def mat_rotate(m, v):
    return (m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2],
            m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2],
            m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2])


def attr_to_color(attr, m=None) -> tuple:
    """Baked color, or a stand-in shade for normal-carrying records.

    |xyz| ~= 1 with w ~= 0 marks a unit NORMAL; rotate it by the record's
    placement and light it with the port's stand-in directional light so
    the EMD2 normal slot can carry colors uniformly."""
    x, y, z, w = attr
    n = math.sqrt(x * x + y * y + z * z)
    if abs(n - 1.0) < 0.02 and abs(w) < 0.1:
        if m is not None:
            x, y, z = mat_rotate(m, (x, y, z))
        lx, ly, lz = 0.4, 0.8, 0.45     # port's stand-in light direction
        ll = math.sqrt(lx * lx + ly * ly + lz * lz)
        d = max((x * lx + y * ly + z * lz) / ll, 0.0)
        s = 0.30 + 0.70 * d
        return (s, s, s)
    return (min(max(x, 0.0), 1.0), min(max(y, 0.0), 1.0),
            min(max(z, 0.0), 1.0))


# ---------------------------------------------------------------------------
# Mesh build (GS tristrips -> welded indexed triangles, per-TRIANGLE texture)

class MeshBuilder:
    def __init__(self):
        self.pos, self.col, self.uv, self.tex = [], [], [], []
        self.bone = []
        self.tris = []
        self.weld = {}
        self.tex_table: list[dict] = []
        self.tex_index: dict[int, int] = {}
        self.NO_TEX = 0xFFFFFFFF
        self.n_strip_tris = 0

    def tex_of(self, q0: int) -> int:
        key = q0 & en.TEX0_KEY_MASK
        ti = self.tex_index.get(key)
        if ti is None:
            f = en.tex0_fields(key)
            # PSMT4 / PSMT8 indexed plus PSMCT32 (the backdrop texture)
            if f["psm"] not in (0x00, 0x13, 0x14) or not (4 <= f["tw"] <= 10) \
                    or not (4 <= f["th"] <= 10):
                ti = self.NO_TEX
            else:
                ti = len(self.tex_table)
                f["key"] = key
                self.tex_table.append(f)
            self.tex_index[key] = ti
        return ti

    def vid_of(self, p, c, uv, t):
        key = (round(p[0], 4), round(p[1], 4), round(p[2], 4),
               round(c[0], 3), round(c[1], 3), round(c[2], 3),
               round(uv[0], 5), round(uv[1], 5), t)
        i = self.weld.get(key)
        if i is None:
            i = len(self.pos)
            self.weld[key] = i
            self.pos.append(p)
            self.col.append(c)
            self.bone.append(0)
            self.uv.append(uv)
            self.tex.append(t)
        return i

    def add_stream(self, records, placement):
        """Consume an iterable of record tuples (pos, wbits, q, uv, attr)
        or None (strip break). GS tristrip semantics: EVERY record enters
        the strip queue; a record with bit 15 CLEAR and >= 2 predecessors
        kicks a triangle, textured by the KICK vertex's TEX0. `placement`
        maps a record to its 3x4 world matrix (or None = as-is)."""
        run = []        # (raw pos, color, uv) of the last <=2 entries + ids
        for rec in records:
            if rec is None:
                run = []
                continue
            pos, wbits, q, uv, attr = rec
            m = placement(wbits) if placement else None
            wp = mat_apply(m, pos) if m else tuple(pos)
            col = attr_to_color(attr, m)
            run.append((wp, col, uv, q))
            if len(run) > 3:
                run.pop(0)
            if (wbits & 0x8000) == 0 and len(run) == 3:
                t = self.tex_of(q)            # kick vertex's TEX0
                (pa, ca, ua, _qa), (pb, cb, ub, _qb), (pc, cc, uc, _qc) = run
                if pa != pb and pb != pc and pa != pc:
                    a = self.vid_of(pa, ca, ua, t)
                    b = self.vid_of(pb, cb, ub, t)
                    c = self.vid_of(pc, cc, uc, t)
                    # parity bit 14 = winding; put the kick vertex first
                    # (Metal flat-shading provoking vertex) and flip by
                    # parity so consistent-winding backends stay correct.
                    if wbits & 0x4000:
                        self.tris.extend((c, b, a))
                    else:
                        self.tris.extend((c, a, b))
                    self.n_strip_tris += 1


def load_level_mesh(level_path: Path):
    """Decode the render mesh into one EMDL section using the per-file
    region map (live placements baked in). Returns (sections, tex_table,
    n_tris)."""
    d = level_path.read_bytes()
    sibling = None
    if len(d) == CHUNK06N1_SIZE and level_path.name == "f03_id43.bin":
        regions = CHUNK06N1_REGIONS
        sib_name, sib_lo, sib_hi = CHUNK06N1_SIBLING
        sib_path = level_path.parent / sib_name
        if sib_path.exists():
            sibling = (sib_path.read_bytes(), sib_lo, sib_hi)
        else:
            print(f"warning: {sib_path} missing — the western half of the "
                  "area will be absent")
    else:
        print("note: no live region map for this file — exporting the whole "
              "file as static world (movable sub-objects may be missing or "
              "mis-placed)")
        regions = [(0, len(d), "world", None)]

    b = MeshBuilder()
    if sibling is not None:
        sd, slo, shi = sibling
        b.add_stream((r if r is None else r[1:] for r in
                      walk_records(sd, slo, shi)), None)
    for lo, hi, mode, mats in regions:
        if mode == "world":
            b.add_stream((r if r is None else r[1:] for r in
                          walk_records(d, lo, hi)), None)
        elif mode == "slots":
            def placement(wbits, mats=mats):
                s = (wbits & 0x3FF) >> 3
                return mats[s] if s < len(mats) else IDENT34
            b.add_stream((r if r is None else r[1:] for r in
                          walk_records(d, lo, hi)), placement)
        elif mode == "instances":
            for m in mats:
                b.add_stream((r if r is None else r[1:] for r in
                              walk_records(d, lo, hi)),
                             lambda wbits, m=m: m)
    sections = [(b.pos, b.col, b.tris, b.bone, b.uv, b.tex)]
    return sections, b.tex_table, b.n_strip_tris


# ---------------------------------------------------------------------------
# Texture resolution from the GS dump's VRAM snapshot

def read_psmct32_rgba(lm: bytes, tbp0: int, tbw: int, w: int, h: int) -> bytes:
    """Direct PSMCT32 read (the level's 32-bit backdrop texture)."""
    est = _load("_est_lvl", "extract_subtextures.py")
    bw = max(tbw, 1) * 64
    out = bytearray(w * h * 4)
    base = tbp0 * 256
    for y in range(h):
        for x in range(w):
            a = base + est.psmct32_word(x, y, bw) * 4
            o = (y * w + x) * 4
            if a + 4 <= len(lm):
                r, g, b, al = lm[a], lm[a + 1], lm[a + 2], lm[a + 3]
                out[o:o + 4] = bytes((r, g, b, min(255, al * 2)))
            else:
                out[o:o + 4] = b"\x00\x00\x00\xff"
    return bytes(out)


def build_texture_blob(gsdump: Path | None, tex_table: list[dict]):
    """Like export_native.build_texture_blob but adds PSMCT32 support."""
    entries, blob = [], bytearray()
    lm = None
    if gsdump is not None:
        pg = _load("_parse_gsdump_lvl", "parse_gsdump.py")
        state_data, _r, _p, _s, _c = pg.parse(gsdump, quiet=True)
        lm = pg.dump_vram(state_data)
        cp = _load("_clut_pair_lvl", "clut_pair.py")
        from clut import apply_clut
    for f in tex_table:
        w, h = 1 << f["tw"], 1 << f["th"]
        if lm is None:
            entries.append({"w": 1, "h": 1, "off": len(blob)})
            blob += b"\x80\x80\x80\xff"
            continue
        if f["psm"] == 0x00:
            rgba = read_psmct32_rgba(lm, f["tbp0"], f["tbw"], w, h)
        elif f["psm"] == 0x14:
            idx = cp.read_psmt4(lm, f["tbp0"], f["tbw"], w, h)
            pal = cp.read_clut16_rgba(lm, f["cbp"]) + bytes(1024 - 64)
            rgba = apply_clut(idx, pal)
        else:
            idx = cp.read_psmt8(lm, f["tbp0"], f["tbw"], w, h)
            pal = cp.read_clut_rgba(lm, f["cbp"])
            rgba = apply_clut(idx, pal)
        entries.append({"w": w, "h": h, "off": len(blob)})
        blob += rgba
    return entries, bytes(blob)


# ---------------------------------------------------------------------------

def main(argv):
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--level", default="extract/chunk06.n1/f03_id43.bin",
                    help="level RENDER-MESH file (64-byte [tex0][st][color]"
                    "[pos] records; the office scene's is "
                    "extract/chunk06.n1/f03_id43.bin)")
    ap.add_argument("--gsdump", help="PCSX2 1-frame GS dump (.gs) of a scene "
                    "inside this level: source of colored texels. Without "
                    "it textures are grey 1x1.")
    ap.add_argument("--out", required=True)
    args = ap.parse_args(argv)

    sections, tex_table, n_tris = load_level_mesh(Path(args.level))
    pos = sections[0][0]
    ntris = len(sections[0][2]) // 3
    xs = [p[0] for p in pos]
    ys = [p[1] for p in pos]
    zs = [p[2] for p in pos]
    print(f"level mesh: {len(pos)} verts, {ntris} tris "
          f"({n_tris} strip kicks), {len(tex_table)} textures")
    print(f"  bbox X[{min(xs):.1f},{max(xs):.1f}] "
          f"Y[{min(ys):.1f},{max(ys):.1f}] Z[{min(zs):.1f},{max(zs):.1f}]")

    tex_entries, tex_blob = build_texture_blob(
        Path(args.gsdump) if args.gsdump else None, tex_table)

    frames = [[en.mat_identity()]]   # 1 bone, 1 identity frame
    parents = [-1]
    en.write_emdl(Path(args.out), sections, [], parents, frames, 30.0,
                  tex_entries, tex_blob, flags=1)   # bit 0: vertex-color
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
