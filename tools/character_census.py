#!/usr/bin/env python3
"""character_census.py - every character mesh on the disc, and the one each scripted actor binds.

Static census over the user's own files. Runs natively on macOS arm64
(python3 + numpy; no emulator, no container):

  * the boot ELF (config/SCUS_971.12, pinned by SHA-256 through
    cut_content_census.Elf);
  * the extracted DATA.DAT files (extract/, rebuilt into one contiguous image
    from extract/manifest.txt) and extract/OVERLAY/AREAxx.BIN;
  * the disc image (--iso, the user's own dump or the byte-identical rebuild)
    for INDEX.IDX (load sets) and the level-load GS uploads (textures);
  * optionally the captured opening GS state (--gs) for the player's textures.

Everything it writes is disc-derived and goes to ignored build/ only
(default build/cut_content/characters/); never commit it.

What it does (addresses are the original's; docs/FINDINGS.md "Cut-content
census" sections 7-9):

  meshes     every raw mesh blob on the DATA.DAT image (header {blocks, qwc,
             nodes, size, bounds}, first VIF UNPACK at +0x48): node slots
             from the vertex W bits, TEX0 keys, and the 176-byte-vertex face
             morph meshes (export_opening_faces.records).
  rigs       every keyed-animation container (the rig_probe rule without the
             +4 sentinel; +8 = 0x20 + 4n), frame-0 pose from the first key of
             each channel (extract_models' bit readers, export_native's
             matrix convention: local = R(conj q) + t, world = parent * local).
  load sets  INDEX.IDX descriptors: D_0028A490[slot] for (area, sub) = the
             global sectors, then the area's top block (sector area + 4),
             then its nested block `sub` (001FFCD0 states 4/7/8/11). A
             resident entry {u24 offset, u8 slot} lands at descriptor base
             + resident start (+0x14) + offset (docs/REPACK.md).
  actors     op14 (001BAC00) entry lists, walked from each op14 record's +0x14
             pointer to the -1 sentinel; per entry 001BAD40's command picks
             the bind (jump table 0x26E170): 0/4/6 D_0028A490[model], 1/8 the
             area model table (slot 0x43) entry `model`, 2 the global model
             table (slot 0x37) entry `model` (both through 001C6120), 3 a
             camera track, 5 an attachment (001C5C90 -> 001B1020 -> global
             table); the clip bank is D_0028A490[bank]. Command 0 also runs
             001BA8E0 (face slot, head node, shadow kind) and 001BB0E0's
             001BA580 maps the body to the speaker slot whose message mailbox
             D_008106D4 drives that face (FACE_ATTACH / TALK_SLOT below).
             A list's sub-area is the one whose load set holds every slot it
             names. Placements (D_0024D7C0[area][sub], 0x28-byte records,
             param low byte -> actor +0xD) whose behaviour calls 001B10B0
             bind D_0028A490[param] (or the call's constant) with the call's
             constant clip bank; placements whose behaviour reaches a
             model-table binder (001B0EA0 / 001B0FD0 / 001C22A0 area table,
             001C2360 global table) draw that table's entry at their param
             byte (placed_models.tsv).
  inventory  every 5+-node blob plus every blob an actor binds: decode
             (export_native.load_mesh_sections), geometry hash, rig pairing by
             cross-node edge coherence, frame-0 pose, heights; face fits and
             shared body parts.
  statics    every 1- to 4-node blob of 48+ records and 4+ units: the
             character-body texture pages it repeats exactly (texels decoded
             from the load set's disc GS-upload replay,
             export_level.background_disc_localmem + clut_pair), the
             placements that draw it, and (--render) textured contact sheets
             of all of them, for finding static or posed bodies no rig drives.
  render     flat front/side previews, textured ones where the replay covers
             every texture, per-area actor sheets and the statics contact
             sheets (--render).

Usage (macOS arm64, decomp repo root or a worktree):
  python3 tools/character_census.py --iso Extermination-rebuilt.iso
  python3 tools/character_census.py --iso Extermination-rebuilt.iso --render \
      --gs build/startup-reference/opening_gs.bin

Outputs (OUT = --out): actors.tsv (every op14 entry and 001B10B0
placement with its load set, body, face, clip bank and speaker slot),
placed_models.tsv (model-table placements), actors.json (all of them),
inventory.json (unique geometries, rigs, face fits, shared parts),
statics.json, and with --render renders/, textured/, sheets/ (index.tsv
lists each area sheet's bodies; statics_NN.png cell order is statics.json
order, 40 per sheet, 8 columns); cache/ holds the scans.

Limits: a static census. A script started from object code, a bind through a
function pointer, or a slot overwritten at run time by another loader is not
followed; overlay modules and the ELF are not scanned for meshes.
"""
from __future__ import annotations

import argparse
import bisect
import csv
import hashlib
import json
import pickle
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

import numpy as np

TOOLS = Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))
import clut_pair as cp                      # noqa: E402
import cut_content_census as ccs            # noqa: E402
import export_level as el                   # noqa: E402
import export_native as native              # noqa: E402
import export_opening_faces as opfaces      # noqa: E402
import extract_models as em                 # noqa: E402
import extract_subtextures as est           # noqa: E402
import placements as plc                    # noqa: E402
from clut import apply_clut                 # noqa: E402
from repack import png                      # noqa: E402
from repack.iso import inventory as iso_inventory  # noqa: E402

MAIN = ccs.MAIN
REPO = TOOLS.parent
OVERLAY_BASE = ccs.OVERLAY_BASE
SIG = 0x01000404                            # STCYCL 4,4 before each UNPACK V4-32
GLOBAL_SECTORS = (1, 2, 3, 27, 28)          # INDEX sectors resident in every load set
AREA_MODEL_TABLE, GLOBAL_MODEL_TABLE = 0x43, 0x37   # D_0028A59C, D_0028A56C
PLACEMENT_DESC = 0x24D7C0                   # D_0024D7C0[area] -> per-sub table pointers
BIND_FN, FACE_FN, TALK_FN = 0x1B10B0, 0x1BA8E0, 0x1BA580
# Binders that take a model-table entry at the actor's +0xD byte (the
# placement param): 001B0EA0 (and its wrapper 001B0FD0, the generic placed
# prop 001C4820's init) and 001C22A0 read the area table D_0028A59C, 001C2360
# the global table D_0028A56C (all byte-matched or NEARMISS C).
TABLE_BINDERS = {0x1B0EA0: AREA_MODEL_TABLE, 0x1B0FD0: AREA_MODEL_TABLE,
                 0x1C22A0: AREA_MODEL_TABLE, 0x1C2360: GLOBAL_MODEL_TABLE}

# 001BA8E0 (body type -> face slot D_0028A490[i], head node actor +0x94,
# shadow kind actor +0x96), read off the original compare chain: the face
# index sits in each branch's delay slot (2026-10-16; src/func_001BA8E0.c
# and the port's em_roger_actor_original.c agree). 0x6A / 0x6C only set the
# kind.
FACE_ATTACH = {
    0x3B: (0x18, 7, None), 0x3E: (0x19, 7, None), 0x3F: (0x1A, 7, None), 0x40: (0x1B, 7, None),
    0x47: (0x88, 7, 0x29), 0x48: (0x88, 7, 0x29), 0x49: (0x88, 7, 0x29),
    0x4F: (0x8E, 7, 0x2A), 0x50: (0x93, 7, 0x2A), 0x51: (0x95, 7, 0x2A),
    0x54: (0x89, 7, 0x2B), 0x55: (0x8F, 7, 0x2B), 0x59: (0x90, 7, 0x2C), 0x5A: (0x94, 7, 0x2C),
    0x5D: (0x8A, 7, 0x2D), 0x5E: (0x91, 7, 0x2D), 0x61: (0x8B, 6, 0x2E), 0x64: (0x8C, 6, 0x2F),
    0x66: (0x92, 6, 0x30), 0x68: (0x8D, 6, 0x31), 0x6A: (None, None, 0x33), 0x6C: (None, None, 0x34),
}
# 001BA580 (body type -> speaker slot: the D_008106D4 mailbox 001FD950 sets
# for a message record's +4 slot starts / stops this body's face talk),
# read off the original compare chain the same way (port activity_index).
TALK_SLOT = {0x3B: 0, 0x3E: 0, 0x3F: 0, 0x40: 0, 0x47: 1, 0x48: 1, 0x49: 1,
             0x4F: 2, 0x50: 2, 0x51: 2, 0x54: 3, 0x55: 3, 0x59: 4, 0x5A: 4,
             0x5D: 5, 0x5E: 5, 0x61: 6, 0x64: 7, 0x66: 8, 0x68: 9}
# Speaker slots as named by the lines they carry (FINDINGS "Cut-content
# census" 1); a project annotation, not disc data.
SPEAKERS = {0: "Dennis", 1: "Roger", 2: "Cindy", 3: "Madigan", 4: "Gary", 5: "Filel",
            6: "Travis Miller", 7: "Sonia", 8: "Morris", 9: "Falken"}
COMMANDS = {0: "body", 1: "body (area model table)", 2: "prop (global model table)",
            3: "camera track", 4: "body (posed)", 5: "attachment", 6: "body (sound)",
            7: "no bind", 8: "body (area model table)"}


def u32(buf, off):
    return struct.unpack_from("<I", buf, off)[0]


class _Blob:
    """export_native.load_mesh_sections reads a path-like; this hands it bytes."""

    def __init__(self, data: bytes):
        self.data = data

    def read_bytes(self) -> bytes:
        return self.data

    def __str__(self) -> str:
        return "<mesh blob>"


# ---------------------------------------------------------------------------
# inputs

class Disc:
    def __init__(self, args):
        self.elf = ccs.Elf(args.elf)
        self.boot = el.BootElf(args.elf)
        self.image, self.files = ccs.data_image(args.extract)
        self.starts = [off for _, off, _ in self.files]
        self.overlays = {p.stem: p.read_bytes()
                         for p in sorted((args.extract / "OVERLAY").glob("AREA*.BIN"))}
        self.iso = args.iso
        info = {f["path"]: f for f in iso_inventory(args.iso)["files"]}["DATA/INDEX.IDX"]
        with open(args.iso, "rb") as fh:
            fh.seek(info["offset"])
            self.index = fh.read(info["size"])
        self._bg = None

    def where(self, addr: int) -> str:
        i = bisect.bisect_right(self.starts, addr) - 1
        name, off, size = self.files[i]
        return f"{name}+{addr - off:#x}"

    def background(self):
        if self._bg is None:
            self._bg = el.BackgroundDisc(iso=str(self.iso))
        return self._bg


# ---------------------------------------------------------------------------
# meshes: raw blobs and face morph meshes

def scan_blobs(img: bytes) -> dict:
    w = np.frombuffer(img, dtype="<u4", count=len(img) // 4)
    cand = np.nonzero(w[:-1] == SIG)[0]
    cand = cand[((w[cand + 1] >> 24) & 0x7F) == 0x6C]
    blobs, tiles, end = [], [], 0
    for ci in cand:
        h = int(ci) * 4 - 0x48
        if h < end or h < 0:
            continue
        nb, qwc, nn, size = struct.unpack_from("<4I", img, h)
        if not (1 <= nb <= 4096 and 1 <= nn <= 256 and 0x50 <= size <= 0x400000
                and h + size <= len(img)):
            continue
        if nn > 64:                       # terrain tile sets (word 3 = 211 / 219), not node counts
            tiles.append(h)
            end = h + size
            continue
        raw = img[h:h + size]
        face = None
        if nn == 1 and u32(raw, 0x34) == 7:
            try:
                face = opfaces.records(raw)
            except ValueError:
                face = None
        if face is None:
            payloads, _ = native._walk_blob_blocks(raw)
            if len(payloads) != nb:
                continue
            recs = np.concatenate([np.frombuffer(p, np.uint8)[:len(p) // 64 * 64].reshape(-1, 64)
                                   for p in payloads])
            wf = recs[:, 60:64].copy().view("<f4")[:, 0]
            recs = recs[np.abs(np.abs(wf) - 1.0) <= 0.25]
            if not len(recs):
                continue
            wb = recs[:, 60:64].copy().view("<u4")[:, 0]
            slots = Counter(((wb & 0x3FF) >> 3).tolist())
            tex = recs[:, 0:8].copy().view("<u8")[:, 0] & np.uint64(native.TEX0_KEY_MASK)
            blobs.append({"off": h, "blocks": nb, "nodes": nn, "size": size, "face": False,
                          "records": int(len(recs)), "slots": dict(slots),
                          "tex0": sorted({int(t) for t in np.unique(tex)}),
                          "bounds": [round(x, 3) for x in struct.unpack_from("<8f", raw, 0x10)]})
        else:
            blobs.append({"off": h, "blocks": nb, "nodes": 1, "size": size, "face": True,
                          "records": 32 * len(face), "slots": {0: 32 * len(face)}, "tex0": [],
                          "bounds": [round(x, 3) for x in struct.unpack_from("<8f", raw, 0x10)]})
        end = h + size
    return {"blobs": blobs, "tiles": len(tiles)}


# ---------------------------------------------------------------------------
# rigs: keyed-animation containers, frame 0

def _first_key(img, tbl: int, rel: int, widths):
    off = tbl + rel
    if off + em.ANIMKEY_REC_SIZE > len(img):
        return None
    if struct.unpack_from("<H", img, off + 10)[0] == em.ANIMKEY_TERMINATOR:
        return None
    vals, bit, buf = [], 0, img[off:off + 10]
    for wd in widths:
        vals.append(em._id74_truncfloat(em._id74_bits(buf, bit, wd), wd))
        bit += wd
    return tuple(vals)


def local_matrix(q, t, s) -> np.ndarray:
    """export_native.bake_id74_palettes' local matrix (R(conj q) + t, scale on
    the basis columns), as a row-major numpy 4x4."""
    m = native.mat_from_quat_trn((-q[0], -q[1], -q[2], q[3]), t)
    if s is not None and any(abs(c - 1.0) > 1e-4 for c in s):
        m = tuple(tuple(v * s[c] for v in col) if c < 3 else col for c, col in enumerate(m))
    return np.array(m, dtype=np.float64).T


def scan_rigs(img: bytes) -> list[dict]:
    w = np.frombuffer(img, dtype="<u4", count=len(img) // 4)
    idx = np.nonzero(w[8:] == 0xFFFFFFFF)[0]         # parents[0] == -1 at +0x20
    lo, hi = w[idx] & 0xFFFF, w[idx] >> 16
    keep = (lo >= 1) & (lo <= 64) & (hi > 0) & (w[idx + 2] == 0x20 + 4 * lo)
    rigs = []
    for i in idx[keep]:
        h = int(i) * 4
        n, clip = struct.unpack_from("<2H", img, h)
        parents = struct.unpack_from(f"<{n}i", img, h + 0x20)
        if not all(-1 <= p < k for k, p in enumerate(parents)):
            continue
        trn, scl = struct.unpack_from("<2I", img, h + 0x0C)
        try:
            rtab = h + 0x20 + 4 * n
            rot = [_first_key(img, rtab, r, em.ID74_ROT_WIDTHS)
                   for r in struct.unpack_from(f"<{n}I", img, rtab)]
            tr = [_first_key(img, h + trn, r, em.ID74_VEC_WIDTHS)
                  for r in struct.unpack_from(f"<{n}I", img, h + trn)]
            sc = [_first_key(img, h + scl, r, em.ID74_VEC_WIDTHS)
                  for r in struct.unpack_from(f"<{n}I", img, h + scl)]
        except struct.error:
            continue
        world = np.zeros((n, 4, 4))
        for b in range(n):
            loc = local_matrix(rot[b] or (0.0, 0.0, 0.0, 1.0), tr[b] or (0.0, 0.0, 0.0), sc[b])
            world[b] = loc if parents[b] < 0 else world[parents[b]] @ loc
        rigs.append({"off": h, "n": n, "clip": clip, "parents": list(parents), "W0": world})
    return rigs


# ---------------------------------------------------------------------------
# load sets: INDEX.IDX descriptors -> D_0028A490 slot maps

def _block_entries(block: bytes) -> dict[int, int]:
    groups = struct.unpack_from("<H", block, 0xC)[0] + struct.unpack_from("<H", block, 0xE)[0] \
        + u32(block, 0x10)
    table, base, resident = 0x20 + 8 * groups, u32(block, 4), u32(block, 0x14)
    out = {}
    for i in range(u32(block, 0x1C)):
        word = u32(block, table + 4 * i)
        out[word >> 24] = base + resident + (word & 0xFFFFFF)
    return out


def load_blocks(index: bytes) -> dict:
    """{(sector, nested or None): {"base", "size", "slots": {slot: addr}}}."""
    blocks = {}
    for sector in range(len(index) // 0x800):
        desc = index[sector * 0x800:(sector + 1) * 0x800]
        if u32(desc, 0) != sector:
            continue
        parts = [(None, desc)] + [(n, desc[0x100 + 0x70 * n:0x170 + 0x70 * n])
                                  for n in range(min(u32(desc, 0x18), 8))]
        for n, block in parts:
            if u32(block, 4):
                blocks[(sector, n)] = {"base": u32(block, 4), "size": u32(block, 8),
                                       "slots": _block_entries(block)}
    return blocks


class LoadSets:
    def __init__(self, blocks: dict):
        self.blocks = blocks
        self.globals = {}
        for sector in GLOBAL_SECTORS:
            for slot, addr in blocks.get((sector, None), {"slots": {}})["slots"].items():
                self.globals[slot] = (addr, f"chunk{sector:02d}")

    def subs(self, area: int) -> list:
        n = sorted(k[1] for k in self.blocks if k[0] == area + 4 and k[1] is not None)
        return n or [None]

    def resolve(self, area: int, sub, slot: int):
        """(DATA.DAT address, providing block) of D_0028A490[slot] or None."""
        sector = area + 4
        if sub is not None and slot in self.blocks.get((sector, sub), {"slots": {}})["slots"]:
            return self.blocks[(sector, sub)]["slots"][slot], f"chunk{sector:02d}.n{sub}"
        if slot in self.blocks.get((sector, None), {"slots": {}})["slots"]:
            return self.blocks[(sector, None)]["slots"][slot], f"chunk{sector:02d}"
        return self.globals.get(slot)

    def block_of(self, addr: int):
        """The descriptor block whose region holds `addr` (nested before top)."""
        best = None
        for key, b in self.blocks.items():
            if b["base"] <= addr < b["base"] + b["size"]:
                if best is None or key[1] is not None:
                    best = key
        return best


def model_table_entry(img: bytes, table: int, index: int) -> int:
    """001C6120: table + (word[index + 1] >> 2 << 2), index & 0x7FFF."""
    return table + (u32(img, table + 4 * ((index & 0x7FFF) + 1)) >> 2 << 2)


# ---------------------------------------------------------------------------
# scripts: op14 entry lists, scene speakers

def op14_lists(blob: bytes) -> list[dict]:
    """Every op14 record and its 0x2C-byte entry list, plus the line starts
    of the script it belongs to (its run and the runs its JUMPs reach)."""
    runs = ccs.script_runs(blob, OVERLAY_BASE)
    run_at = {run[0]: run for run in runs}
    owner = {off: run[0] for run in runs for off in run}
    out = []
    for run in runs:
        for off in run:
            if u32(blob, off) & 0xFFF != 0x14:
                continue
            lst = u32(blob, off + 0x14) - OVERLAY_BASE
            if not 0 <= lst < len(blob):
                continue
            entries, e = [], lst
            while struct.unpack_from("<h", blob, e)[0] != -1 and len(entries) < 64:
                cls, _, mode, _, model, bank, clip, cmd = struct.unpack_from("<BBBBhhhH", blob, e)
                entries.append({"addr": OVERLAY_BASE + e, "class": cls, "mode": mode,
                                "model": model & 0xFFFF, "bank": bank & 0xFFFF, "clip": clip,
                                "command": cmd, "step": struct.unpack_from("<f", blob, e + 0xC)[0],
                                "callback": u32(blob, e + 0x28)})
                e += 0x2C
            # the script: this run, then every run a JUMP (bit 30) reaches
            lines, todo, seen = [], [owner[off]], set()
            while todo:
                start = todo.pop()
                if start in seen or start not in run_at:
                    continue
                seen.add(start)
                for r in run_at[start]:
                    word, sub = u32(blob, r), u32(blob, r + 8)
                    op = word & 0xFFF
                    if op == 0x0C and sub in (0, 1):
                        lines.append(u32(blob, r + 0x14))
                    elif op == 0x15:
                        lines.append(sub)
                    elif (op == 0x07 and sub in (9, 10, 11, 12)) or op == 0x1A:
                        lines.append(u32(blob, r + 0x18))
                    if word & 0x40000000:
                        todo.append(owner.get(u32(blob, r + 4) - OVERLAY_BASE, -1))
            out.append({"record": OVERLAY_BASE + off, "list": OVERLAY_BASE + lst,
                        "entries": entries, "lines": lines})
    return out


def scene_speakers(records, lines) -> list[int]:
    slots = set()
    for start in lines:
        if start & 0x80000000 or start >= len(records):
            continue
        for i in ccs.run_chain(records, start):
            if records[i][2] != 0xFF:
                slots.add(records[i][2])
    return sorted(slots)


# ---------------------------------------------------------------------------
# placements and the binders their behaviours reach

def function_starts(overlay: str | None) -> list[int]:
    """Function start addresses: docs/FUNCTIONS.csv for the ELF; for an
    overlay the src/overlays/AREAxx/ names + 0x40 (splat names overlay code
    from the end of the 0x40-byte MWo3 header, which also loads at
    0x823500, so runtime = name + 0x40)."""
    if overlay is None:
        rows = (REPO / "docs" / "FUNCTIONS.csv").read_text().splitlines()[1:]
        return sorted(int(r.split(",")[0], 16) for r in rows)
    out = set()
    for p in (REPO / "src" / "overlays" / overlay).glob("*.c"):
        try:
            out.add(int(p.stem.rsplit("_", 1)[-1], 16) + 0x40)
        except ValueError:
            pass
    return sorted(out)


def _reg_source(code: bytes, base: int, at: int, reg: int, lo: int, depth: int = 0,
                delay: bool = True):
    """What sets `reg` for the instruction at `at` (a jal: its delay slot
    first): ("const", v), ("param",) for a byte load of actor +0xD, or
    ("?",). Register moves (andi 0xFF, paddub/addu/or with $zero) are
    followed."""
    for addr in ([at + 4] if delay else []) + list(range(at - 4, max(lo, at - 0x60) - 4, -4)):
        word = u32(code, addr - base)
        op, rs, rt, imm = word >> 26, (word >> 21) & 31, (word >> 16) & 31, word & 0xFFFF
        rd = (word >> 11) & 31
        move = None
        if op == 0x0C and rt == reg and imm == 0xFF:
            move = rs
        elif op == 0 and word & 0x3F in (0x21, 0x25, 0x2D) and rd == reg and rt == 0:
            move = rs
        elif op == 0x1C and word & 0x7FF == 0x628 and rd == reg and rt == 0:
            move = rs                                     # paddub rd, rs, $zero
        if move is not None:
            if depth >= 3:
                return ("?",)
            return _reg_source(code, base, addr, move, lo, depth + 1, delay=False)
        if rt != reg or op not in (0x08, 0x09, 0x0D, 0x0F, 0x20, 0x21, 0x23, 0x24, 0x25):
            if op == 0 and rd == reg and word & 0x3F in (0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
                                                         0x26, 0x27, 0x2A, 0x2B):
                return ("?",)                             # other ALU writes
            continue
        if op == 0x09 and rs == 0:
            return ("const", imm - 0x10000 if imm & 0x8000 else imm)
        if op == 0x0D and rs == 0:
            return ("const", imm)
        if op == 0x24 and imm == 0xD:
            return ("param",)
        return ("?",)
    return ("?",)


def _guard(code: bytes, base: int, site: int, lo: int):
    """The case value of a compare chain guarding `site`: the nearest branch
    before it is a bne/bnel falling through to it, comparing a register just
    loaded with an immediate (None when there is no such guard)."""
    for addr in range(site - 4, max(lo, site - 0x30) - 4, -4):
        word = u32(code, addr - base)
        op = word >> 26
        if op in (0x04, 0x14, 0x06, 0x07, 0x16, 0x17, 0x01, 0x02, 0x03) or \
                (op == 0 and word & 0x3F in (0x08, 0x09)):
            return None
        if op in (0x05, 0x15):
            for reg in ((word >> 21) & 31, (word >> 16) & 31):
                src = _reg_source(code, base, addr, reg, max(lo, addr - 0x10), delay=False)
                if src[0] == "const":
                    return src[1]
            return None
    return None


def bind_sites(code: bytes, base: int, starts: list[int]):
    """({function start: [(site, model source, bank source, case guard)]} for
    every jal 001B10B0, {function start: {other jal targets}})."""
    sites, calls = defaultdict(list), defaultdict(set)
    for off in range(0, len(code) - 3, 4):
        word = u32(code, off)
        if word >> 26 != 3:
            continue
        addr, target = base + off, (word & 0x3FFFFFF) << 2
        i = bisect.bisect_right(starts, addr) - 1
        if i < 0:
            continue
        fn = starts[i]
        if target == BIND_FN:
            sites[fn].append((addr, _reg_source(code, base, addr, 5, fn),
                              _reg_source(code, base, addr, 6, fn), _guard(code, base, addr, fn)))
        else:
            calls[fn].add(target)
    return sites, calls


def placement_tables(disc: Disc, area: int, subs: list) -> dict:
    """{sub: [Placement]}: 001B6990's table D_0024D7C0[area][sub] (the
    pointer array and the tables live in ELF .data or in the overlay)."""
    ov = disc.overlays.get(f"AREA{area:02d}")
    if ov is None:
        return {}

    def read(vaddr, n):
        if vaddr >= OVERLAY_BASE:
            o = vaddr - OVERLAY_BASE
            if o + n > len(ov):
                raise ValueError(f"{vaddr:#x} beyond the overlay")
            return ov[o:o + n]
        return disc.boot.read(vaddr, n)

    desc = u32(disc.boot.read(PLACEMENT_DESC + 4 * area, 4), 0)
    out = {}
    for sub in subs:
        if not desc:
            break
        table = u32(read(desc + 4 * (sub or 0), 4), 0)
        if not table:
            continue
        recs = []
        for i in range(512):
            raw = read(table + plc.REC_SIZE * i, plc.REC_SIZE)
            if struct.unpack_from("<H", raw)[0] == plc.SENTINEL:
                break
            p = plc.parse_record(raw, 0, i)
            p.vaddr = table + plc.REC_SIZE * i
            recs.append(p)
        out[sub] = recs
    return out


# ---------------------------------------------------------------------------
# actors: op14 entries and binding placements -> load-set slots -> blobs

def _entry_needs(e: dict) -> list:
    """(role, slot) pairs an op14 entry reads from D_0028A490."""
    model, cmd = e["model"], e["command"]
    if model in (0x270C, 0x270D):
        return []
    if model == 0x270E:
        return [("bank", e["bank"])]
    needs = []
    if cmd in (0, 4, 6):
        needs.append(("body", model))
        if cmd == 0 and FACE_ATTACH.get(model, (None,))[0] is not None:
            needs.append(("face", FACE_ATTACH[model][0]))
    elif cmd in (1, 8):
        needs.append(("table", AREA_MODEL_TABLE))
    elif cmd in (2, 5):
        needs.append(("table", GLOBAL_MODEL_TABLE))
    if cmd != 5:
        needs.append(("bank", e["bank"]))
    return needs


def resolve_entry(disc: Disc, sets: LoadSets, area: int, sub, e: dict) -> dict:
    model, cmd = e["model"], e["command"]
    row = {"command": cmd, "role": COMMANDS.get(cmd, "?"), "model": model, "bank": e["bank"],
           "clip": e["clip"], "class": e["class"], "mode": e["mode"]}
    if model in (0x270C, 0x270D):
        row["role"] = f"message {model:#x}"
        return row
    if model == 0x270E:
        row["role"] = "track-driven spawn (001C8140)"
    elif cmd == 3:
        row["role"] = "camera track"
    if model != 0x270E and cmd in (0, 4, 6):
        got = sets.resolve(area, sub, model)
        if got:
            row["body"] = {"addr": got[0], "via": f"{got[1]} slot {model:#x}"}
        if cmd == 0 and model in FACE_ATTACH:
            face, node, kind = FACE_ATTACH[model]
            row["shadow_kind"] = kind
            if face is not None:
                fgot = sets.resolve(area, sub, face)
                row["face"] = {"slot": face, "node": node,
                               "addr": fgot[0] if fgot else None,
                               "via": f"{fgot[1]} slot {face:#x}" if fgot else "not loaded"}
        if cmd == 0 and model in TALK_SLOT:
            row["speaker"] = TALK_SLOT[model]
    elif model != 0x270E and cmd in (1, 2, 5, 8):
        table = AREA_MODEL_TABLE if cmd in (1, 8) else GLOBAL_MODEL_TABLE
        got = sets.resolve(area, sub, table)
        if got:
            index = model & 0xFF if cmd == 5 else model
            row["body"] = {"addr": model_table_entry(disc.image, got[0], index),
                           "via": f"{got[1]} slot {table:#x} table entry {index:#x}"}
    if cmd != 5 and model not in (0x270C, 0x270D):
        got = sets.resolve(area, sub, e["bank"])
        row["bank_at"] = disc.where(got[0]) if got else None
    return row


def resolve_op14(disc: Disc, sets: LoadSets, tables) -> list[dict]:
    rows = []
    for name, blob in disc.overlays.items():
        area = int(name[4:])
        records = tables.get(f"area{area:02d}", [])
        for lst in op14_lists(blob):
            needs = [n for e in lst["entries"] for n in _entry_needs(e)]
            scored = []
            for sub in sets.subs(area):
                ok = sum(1 for _, slot in needs if sets.resolve(area, sub, slot))
                scored.append((ok, sub))
            best = max(s for s, _ in scored) if scored else 0
            cands = [sub for s, sub in scored if s == best]
            resolved = {sub: [resolve_entry(disc, sets, area, sub, e) for e in lst["entries"]]
                        for sub in cands}
            same = len({json.dumps(v, sort_keys=True) for v in resolved.values()}) == 1
            speakers = scene_speakers(records, lst["lines"])
            for sub in (cands[:1] if same else cands):
                for e, row in zip(lst["entries"], resolved[sub]):
                    row.update({"area": area, "source": "op14", "script": lst["record"],
                                "list": lst["list"], "entry": e["addr"],
                                "sub": sub if (len(cands) == 1 or not same) else None,
                                "sub_rule": ("single load set" if cands == [None] else
                                             "only load set holding every slot" if len(cands) == 1 else
                                             "same in every candidate sub" if same else "ambiguous"),
                                "slots_resolved": f"{best}/{len(needs)}",
                                "scene_speakers": speakers})
                    if "speaker" in row:
                        row["speaks_in_scene"] = row["speaker"] in speakers
                    rows.append(row)
    return rows


def resolve_placements(disc: Disc, sets: LoadSets) -> list[dict]:
    """Placements whose behaviour (or a function it calls) reaches 001B10B0
    or a model-table binder (TABLE_BINDERS)."""
    elf_starts = function_starts(None)
    elf_sites, elf_calls = bind_sites(disc.elf.text(), 0x100000, elf_starts)
    rows = []
    for name, blob in disc.overlays.items():
        area = int(name[4:])
        starts = function_starts(name)
        ov_sites, ov_calls = bind_sites(blob, OVERLAY_BASE, starts)
        sites, calls = {**elf_sites, **ov_sites}, {**elf_calls, **ov_calls}
        for sub, recs in placement_tables(disc, area, sets.subs(area)).items():
            groups = {}
            for p in recs:
                st = starts if p.behavior >= OVERLAY_BASE else elf_starts
                i = bisect.bisect_right(st, p.behavior) - 1
                if i < 0 or not 0x100000 <= p.behavior < OVERLAY_BASE + len(blob):
                    continue
                fn = st[i]
                fns = [fn] + sorted(calls.get(fn, ()))
                reach = [s for f in fns for s in sites.get(f, [])]
                param = p.param & 0xFF
                tables = sorted({TABLE_BINDERS[f] for f in fns if f in TABLE_BINDERS})
                if not reach and tables:
                    got = sets.resolve(area, sub, tables[0])
                    g = groups.setdefault((p.behavior, ("table", tables[0], param)), {
                        "fn": fn, "placements": [], "banks": set(), "sites": set(), "p": p,
                        "faces": False, "talks": False, "table": tables[0],
                        "addr": model_table_entry(disc.image, got[0], param) if got else None,
                        "via": f"{got[1]} slot {tables[0]:#x} table entry {param:#x}" if got else None})
                    g["placements"].append(p.index)
                    g.setdefault("positions", []).append([round(x, 1) for x in p.pos])
                    continue
                if not reach:
                    continue
                # a switch on the param binds only in its own case
                chosen = [s for s in reach if s[3] == param] or [s for s in reach if s[3] is None]
                for site, msrc, bsrc, guard in chosen:
                    slot = param if msrc[0] == "param" else (msrc[1] if msrc[0] == "const" else None)
                    g = groups.setdefault((p.behavior, slot), {
                        "fn": fn, "placements": [], "banks": set(), "sites": set(), "p": p,
                        "faces": any(FACE_FN in calls.get(f, ()) for f in fns),
                        "talks": any(TALK_FN in calls.get(f, ()) for f in fns)})
                    g["placements"].append(p.index)
                    g["sites"].add(site)
                    if bsrc[0] == "const":
                        g["banks"].add(bsrc[1])
            for (behaviour, slot), g in groups.items():
                p = g["p"]
                if "table" in g:
                    row = {"area": area, "sub": sub, "source": "placement", "behaviour": behaviour,
                           "function": g["fn"], "role": "placed model (table)", "model": slot[2],
                           "placements": sorted(set(g["placements"])), "positions": g["positions"],
                           "class": p.spawn_class, "sub_rule": "placement table of this sub",
                           "calls_face_attach": False, "calls_talk": False}
                    if g["addr"] is not None:
                        row["body"] = {"addr": g["addr"], "via": g["via"]}
                    rows.append(row)
                    continue
                banks = sorted(g["banks"])
                row = {"area": area, "sub": sub, "source": "placement", "behaviour": behaviour,
                       "function": g["fn"], "bind_site": min(g["sites"]), "role": "body (001B10B0)",
                       "model": slot, "bank": banks[0] if len(banks) == 1 else None, "banks": banks,
                       "placements": sorted(set(g["placements"])), "class": p.spawn_class,
                       "pos": [round(x, 1) for x in p.pos], "sub_rule": "placement table of this sub",
                       "calls_face_attach": g["faces"], "calls_talk": g["talks"]}
                if slot is not None:
                    got = sets.resolve(area, sub, slot)
                    if got:
                        row["body"] = {"addr": got[0], "via": f"{got[1]} slot {slot:#x}"}
                    if g["faces"] and FACE_ATTACH.get(slot, (None,))[0] is not None:
                        face, node, kind = FACE_ATTACH[slot]
                        fgot = sets.resolve(area, sub, face)
                        row["shadow_kind"] = kind
                        row["face"] = {"slot": face, "node": node, "addr": fgot[0] if fgot else None,
                                       "via": f"{fgot[1]} slot {face:#x}" if fgot else "not loaded"}
                    if slot in TALK_SLOT:
                        row["speaker"] = TALK_SLOT[slot]
                        row["speaker_path"] = "001BA580 reached" if g["talks"] else "001BA580 not reached"
                if row["bank"] is not None:
                    got = sets.resolve(area, sub, row["bank"])
                    row["bank_at"] = disc.where(got[0]) if got else None
                rows.append(row)
    return rows


# ---------------------------------------------------------------------------
# inventory: decode, hash, rig pairing, frame-0 pose

def decode_blob(img: bytes, blob: dict) -> dict | None:
    """export_native.load_mesh_sections on the blob; low slots are kept (via a
    one-node header copy) when more than 2% of the records use them."""
    raw = bytearray(img[blob["off"]:blob["off"] + blob["size"]])
    low = sum(c for s, c in blob["slots"].items() if int(s) < 2)
    keep_low = blob["nodes"] < 5 or low > 0.02 * blob["records"]
    if keep_low:
        struct.pack_into("<I", raw, 8, 1)
    try:
        sections, max_slot, tex = native.load_mesh_sections(_Blob(bytes(raw)))
    except SystemExit:
        return None
    pos, nrm, tris, bones, uvs, texids = sections[0]
    if not tris:
        return None
    return {"pos": np.array(pos, np.float64), "nrm": np.array(nrm, np.float64),
            "tris": np.array(tris, np.int64).reshape(-1, 3), "bones": np.array(bones, np.int64),
            "uvs": np.array(uvs, np.float64), "texids": np.array(texids, np.int64),
            "max_slot": max_slot, "tex": tex, "keep_low": keep_low}


def geometry_hash(img: bytes, blob: dict) -> str:
    """SHA-1 of every valid record's ST, normal and position+W (no TEX0, which
    is a per-load-set VRAM address)."""
    payloads, _ = native._walk_blob_blocks(img[blob["off"]:blob["off"] + blob["size"]])
    g = hashlib.sha1()
    for p in payloads:
        r = np.frombuffer(p, np.uint8)[:len(p) // 64 * 64].reshape(-1, 64)
        wf = r[:, 60:64].copy().view("<f4")[:, 0]
        good = np.abs(np.abs(wf) - 1.0) <= 0.25
        n = int(np.argmin(good)) if not good.all() else len(r)
        g.update(r[:n, 16:64].tobytes())
    return g.hexdigest()[:16]


def pose(mesh: dict, W: np.ndarray):
    P = np.concatenate([mesh["pos"], np.ones((len(mesh["pos"]), 1))], axis=1)
    M = W[np.clip(mesh["bones"], 0, len(W) - 1)]
    return np.einsum("nij,nj->ni", M, P)[:, :3]


def cross_edges(mesh: dict) -> np.ndarray:
    t = mesh["tris"]
    e = np.unique(np.sort(np.concatenate([t[:, [0, 1]], t[:, [1, 2]], t[:, [2, 0]]]), axis=1), axis=0)
    b = mesh["bones"]
    return e[b[e[:, 0]] != b[e[:, 1]]]


def coherence(mesh: dict, cross: np.ndarray, W: np.ndarray) -> float:
    """Mean distance between the two ends of every triangle edge that joins two
    nodes, posed (FINDINGS s14/s15 pairing rule): ~0 for the right rig."""
    if not len(cross) or mesh["bones"].max() >= len(W):
        return float("inf")
    P = pose(mesh, W)
    return float(np.mean(np.linalg.norm(P[cross[:, 0]] - P[cross[:, 1]], axis=1)))


def pair_rig(mesh: dict, nodes: int, rigs_by_n: dict) -> dict | None:
    cross = cross_edges(mesh)
    scored = []
    for n in dict.fromkeys((nodes, mesh["max_slot"] + 1)):
        scored = sorted(((coherence(mesh, cross, r["W0"]), i, r) for i, r in enumerate(rigs_by_n.get(n, []))),
                        key=lambda x: (x[0], x[1]))
        scored = [s for s in scored if np.isfinite(s[0])]
        if scored:
            break
    if not scored:
        return None
    best, _, rig = scored[0]
    near = [r for s, _, r in scored if s <= best * 1.15 + 1e-6 and r["parents"] == rig["parents"]]
    # frame-0 pose: among the near containers, the tallest upright one
    Pm = pose(mesh, rig["W0"])
    top = int(mesh["bones"][int(np.argmax(Pm[:, 1]))])
    picks = []
    for r in near:
        P = pose(mesh, r["W0"])
        y0 = P[:, 1].min()
        head = P[mesh["bones"] == top]
        lean = np.linalg.norm(head[:, [0, 2]].mean(0) - P[P[:, 1] < y0 + 0.5][:, [0, 2]].mean(0))
        picks.append((head[:, 1].max() - y0, -lean, r))
    hmax = max(p[0] for p in picks)
    pick = max((p for p in picks if p[0] >= 0.96 * hmax), key=lambda p: p[1])[2]
    return {"rig": rig["off"], "n": rig["n"], "score": round(best, 3), "near": len(near),
            "pose": pick["off"], "pose_clip_len": pick["clip"], "head_node": top, "W": pick["W0"]}


def inventory(disc: Disc, blobs: list[dict], rigs: list[dict], wanted: set, cache: Path) -> dict:
    """{blob offset: record} for every 5+-node blob and every offset in `wanted`."""
    rigs_by_n = defaultdict(list)
    for r in rigs:
        rigs_by_n[r["n"]].append(r)
    posed = cache / "posed"
    posed.mkdir(parents=True, exist_ok=True)
    out = {}
    for b in blobs:
        if b["face"] or not (b["nodes"] >= 5 or b["off"] in wanted):
            continue
        mesh = decode_blob(disc.image, b)
        if mesh is None:
            out[b["off"]] = {"off": b["off"], "where": disc.where(b["off"]), "decoded": False}
            continue
        rec = {"off": b["off"], "where": disc.where(b["off"]), "decoded": True, "nodes": b["nodes"],
               "blocks": b["blocks"], "records": b["records"], "verts": len(mesh["pos"]),
               "tris": len(mesh["tris"]), "max_slot": mesh["max_slot"], "tex_count": len(mesh["tex"]),
               "geometry": geometry_hash(disc.image, b),
               "authored_height": round(b["bounds"][6] - b["bounds"][2], 2), "rig": None}
        W = None
        if b["nodes"] >= 5:
            rig = pair_rig(mesh, b["nodes"], rigs_by_n)
            if rig:
                W = rig.pop("W")
                rec["rig"] = {k: (f"{v:#x}" if k in ("rig", "pose") else v) for k, v in rig.items()}
                rec["rig"]["at"] = disc.where(rig["rig"])
                rec["_head"] = mesh["pos"][mesh["bones"] == rig["head_node"]]
                rec["_clouds"] = {int(n): pts for n in np.unique(mesh["bones"])
                                  if len(pts := np.unique(np.round(mesh["pos"][mesh["bones"] == n], 4), axis=0)) >= 12}
        if W is None:
            W = np.tile(np.eye(4), (max(mesh["max_slot"] + 1, 1), 1, 1))
        P = pose(mesh, W)
        shift = np.array([(P[:, 0].max() + P[:, 0].min()) / 2, P[:, 1].min(), (P[:, 2].max() + P[:, 2].min()) / 2])
        P -= shift
        W = W.copy()
        W[:, :3, 3] -= shift
        rec["posed_extent"] = [round(float(x), 2) for x in np.ptp(P, axis=0)]
        np.savez_compressed(posed / f"{b['off']:08x}.npz", P=P.astype(np.float32), T=mesh["tris"].astype(np.int32),
                            B=mesh["bones"].astype(np.int16), UV=mesh["uvs"].astype(np.float32),
                            TX=mesh["texids"].astype(np.int32), W=W.astype(np.float32))
        rec["_tex"] = mesh["tex"]
        out[b["off"]] = rec
    return out


def shared_parts(disc: Disc, inv: dict) -> list[dict]:
    """Body parts two different rigged meshes share: a node's bone-local
    vertex cloud that matches another mesh's node (symmetric mean nearest
    distance under 1% of the part's size), i.e. parts exported from one
    source model."""
    clouds, seen = [], set()
    for rec in inv.values():
        if not rec.get("_clouds") or rec["geometry"] in seen:
            continue
        seen.add(rec["geometry"])
        for node, pts in rec["_clouds"].items():
            clouds.append((rec, node, pts, np.sort(np.ptp(pts, axis=0))))
    out = []
    for i, (ra, na, pa, ea) in enumerate(clouds):
        for rb, nb, pb, eb in clouds[i + 1:]:
            if ra is rb or not 0.5 <= len(pa) / len(pb) <= 2.0 or max(ea.max(), eb.max()) < 0.8:
                continue
            if np.any(np.abs(ea - eb) > np.maximum(0.25, 0.2 * np.maximum(ea, eb))):
                continue
            d = np.sqrt(((pa[:, None, :] - pb[None, :, :]) ** 2).sum(-1))
            c = 0.5 * (d.min(1).mean() + d.min(0).mean())
            if c / max(ea.max(), eb.max()) < 0.01:
                out.append({"a": ra["where"], "a_node": na, "b": rb["where"], "b_node": nb,
                            "mean_nn": round(float(c), 4)})
    return out


def face_fits(disc: Disc, blobs: list[dict], inv: dict) -> list[dict]:
    """Each face morph mesh (base shape, bone-local) against every rigged
    body's head node: median nearest-vertex distance (0 = the body's own head)."""
    disc.face_verts = {}
    heads = [(rec["off"], rec["rig"]["head_node"], rec["_head"]) for rec in inv.values()
             if rec.get("rig") and len(rec["_head"]) >= 10]
    out = []
    for b in blobs:
        if not b["face"]:
            continue
        sections, textures, _ = opfaces.mesh(bytes(disc.image[b["off"]:b["off"] + b["size"]]))
        F = np.array(sections[0][0])
        fits = []
        for off, node, cloud in heads:
            d = np.sqrt(((F[:, None, :] - cloud[None, :, :]) ** 2).sum(-1)).min(1)
            fits.append((float(np.median(d)), off, node))
        fits.sort()
        disc.face_verts[b["off"]] = F
        out.append({"off": b["off"], "where": disc.where(b["off"]), "packets": b["blocks"],
                    "verts": len(F), "tex_count": len(textures),
                    "fits": [{"median": round(m, 3), "body": disc.where(o), "body_off": o, "head_node": n}
                             for m, o, n in fits[:3]]})
    return out


# ---------------------------------------------------------------------------
# textures: the load set's disc GS uploads, replayed (001FFCD0 states 4/7/8/11)

class Vram:
    def __init__(self, disc: Disc, gs: Path | None = None):
        self.disc, self.cache, self.texcache = disc, {}, {}
        self.base = self._replay(27, None, None)      # the global chunk27 upload
        self.opening = None
        if gs is not None:
            import gs_vram
            _, self.opening = gs_vram.read_localmem(gs)

    def _replay(self, sector: int, sub, base):
        lm = bytearray(base[0]) if base else bytearray(4 << 20)
        cov = np.frombuffer(bytes(base[1]), np.uint8).copy() if base else np.zeros((4 << 20) // 256, np.uint8)
        try:
            new, log, _ = el.background_disc_localmem(self.disc.background(), sector - 4, sub or 0)
        except SystemExit:
            return bytes(lm), cov
        fresh = np.zeros_like(cov)
        for dbp, dbw, dx, dy, w, h in log:
            for yy in range(dy, dy + h, 8):
                for xx in range(dx, dx + w, 8):
                    fresh[((dbp * 256 + est.psmct32_word(xx, yy, max(dbw, 1)) * 4) & 0x3FFFFF) >> 8] = 1
        view = np.frombuffer(bytes(lm), np.uint8).reshape(-1, 256).copy()
        m = fresh.astype(bool)
        view[m] = np.frombuffer(new, np.uint8).reshape(-1, 256)[m]
        return view.tobytes(), cov | fresh

    def at(self, key):
        """(local memory, coverage) for the load-set block `key` (sector, sub)."""
        if key not in self.cache:
            sector, sub = key
            self.cache[key] = self.base if sector in GLOBAL_SECTORS else self._replay(sector, sub, self.base)
        return self.cache[key]

    @staticmethod
    def covered(cov, f: dict) -> bool:
        w, h = 1 << f["tw"], 1 << f["th"]
        n = max(1, (w * h * (4 if f["psm"] == 0x14 else 8) // 8 + 255) // 256)
        return all(b < len(cov) and cov[b] for b in list(range(f["tbp0"], f["tbp0"] + n)) + [f["cbp"]])

    def texels(self, key, f: dict, lm=None):
        ck = (key, f["key"], lm is not None)
        if ck not in self.texcache:
            lm, cov = (lm, None) if lm is not None else self.at(key)
            if cov is not None and not self.covered(cov, f):
                self.texcache[ck] = None
            else:
                w, h = 1 << f["tw"], 1 << f["th"]
                if f["psm"] == 0x14:
                    idx = cp.read_psmt4(lm, f["tbp0"], f["tbw"], w, h)
                    pal = cp.read_clut16_rgba(lm, f["cbp"]) + bytes(1024 - 64)
                else:
                    idx = cp.read_psmt8(lm, f["tbp0"], f["tbw"], w, h)
                    pal = cp.read_clut_rgba(lm, f["cbp"])
                self.texcache[ck] = np.frombuffer(apply_clut(idx, pal), np.uint8).reshape(h, w, 4)
        return self.texcache[ck]


def tex_fields(key: int) -> dict | None:
    f = native.tex0_fields(key)
    if f["psm"] not in (0x13, 0x14) or not (4 <= f["tw"] <= 10 and 4 <= f["th"] <= 10):
        return None
    f["key"] = key
    return f


def blob_textures(vram: Vram, key, keys) -> list:
    """Decoded textures of a blob in its load set (skipping uncovered and
    near-uniform ones), as (content hash, RGBA array)."""
    out = []
    for k in keys:
        f = tex_fields(k)
        t = vram.texels(key, f) if f is not None else None
        if t is None or len(np.unique(t.reshape(-1, 4), axis=0)) < 8:
            continue
        out.append((hashlib.sha1(t.tobytes()).hexdigest()[:16], t))
    return out


def statics(disc: Disc, sets: LoadSets, vram: Vram, blobs: list[dict], bodies: set,
            placed: dict, min_records: int) -> list[dict]:
    """Every 1- to 4-node blob with min_records records or more and a largest
    extent of 4 units or more, with the character-body texture pages it
    repeats exactly and the placements that draw it. The contact sheets
    (--render) are the inspection; the page match only flags reuse."""
    pages = defaultdict(set)
    for b in blobs:
        if b["off"] not in bodies:
            continue
        key = sets.block_of(b["off"])
        for h, _ in blob_textures(vram, key, b["tex0"]) if key else ():
            pages[h].add(b["off"])
    out = []
    for b in blobs:
        bb = b["bounds"]
        extent = max(bb[5] - bb[1], bb[6] - bb[2], bb[7] - bb[3])
        if b["face"] or b["nodes"] >= 5 or b["records"] < min_records or extent < 4:
            continue
        key = sets.block_of(b["off"])
        texs = blob_textures(vram, key, b["tex0"]) if key else []
        shared = defaultdict(int)
        for h, _ in texs:
            for o in pages.get(h, ()):
                shared[o] += 1
        out.append({"off": b["off"], "where": disc.where(b["off"]), "nodes": b["nodes"],
                    "records": b["records"], "extent": round(extent, 2),
                    "block": f"chunk{key[0]:02d}" + (f".n{key[1]}" if key[1] is not None else "") if key else None,
                    "textures": len(texs),
                    "shares_texture_with": [{"body": disc.where(o), "pages": n}
                                            for o, n in sorted(shared.items(), key=lambda x: -x[1])],
                    "placed_by": placed.get(b["off"], [])})
    return sorted(out, key=lambda r: (-bool(r["placed_by"] or r["shares_texture_with"]), -r["records"]))


# ---------------------------------------------------------------------------
# previews (orthographic, z-buffered, flat-shaded; textured where covered)

def render(P, T, view: str, ppu: float, sample=None, pad: int = 4) -> np.ndarray:
    sx, sz = (P[:, 0], P[:, 2]) if view == "front" else (-P[:, 2], P[:, 0])
    light = np.array([0.35, 0.45, 0.82] if view == "front" else [0.82, 0.45, -0.35])
    light /= np.linalg.norm(light)
    w = max(8, int(np.ceil(np.ptp(sx) * ppu)) + 2 * pad)
    h = max(8, int(np.ceil(np.ptp(P[:, 1]) * ppu)) + 2 * pad)
    X, Y = pad + (sx - sx.min()) * ppu, pad + (P[:, 1].max() - P[:, 1]) * ppu
    img = np.full((h, w, 3), 236, np.uint8)
    zb = np.full((h, w), -np.inf)
    fn = np.cross(P[T[:, 1]] - P[T[:, 0]], P[T[:, 2]] - P[T[:, 0]])
    ln = np.linalg.norm(fn, axis=1)
    shade = 0.28 + 0.72 * np.abs((fn / np.maximum(ln, 1e-12)[:, None]) @ light)
    for ti in np.argsort(sz[T].mean(1)):
        if ln[ti] < 1e-12:
            continue
        i0, i1, i2 = T[ti]
        x0, x1, x2, y0, y1, y2 = X[i0], X[i1], X[i2], Y[i0], Y[i1], Y[i2]
        den = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2)
        if abs(den) < 1e-12:
            continue
        ax, bx = max(int(min(x0, x1, x2)), 0), min(int(np.ceil(max(x0, x1, x2))), w - 1)
        ay, by = max(int(min(y0, y1, y2)), 0), min(int(np.ceil(max(y0, y1, y2))), h - 1)
        if ax > bx or ay > by:
            continue
        gx, gy = np.meshgrid(np.arange(ax, bx + 1) + 0.5, np.arange(ay, by + 1) + 0.5)
        l0 = ((y1 - y2) * (gx - x2) + (x2 - x1) * (gy - y2)) / den
        l1 = ((y2 - y0) * (gx - x2) + (x0 - x2) * (gy - y2)) / den
        l2 = 1 - l0 - l1
        z = l0 * sz[i0] + l1 * sz[i1] + l2 * sz[i2]
        sub = zb[ay:by + 1, ax:bx + 1]
        m = (l0 >= -1e-6) & (l1 >= -1e-6) & (l2 >= -1e-6) & (z > sub)
        if not m.any():
            continue
        sub[m] = z[m]
        if sample is not None:
            col = np.clip(sample(ti, np.stack([l0[m], l1[m], l2[m]], 1)) * (0.45 + 0.55 * shade[ti]), 0, 255)
        else:
            col = np.clip(np.array([170.0, 190.0, 215.0]) * shade[ti], 0, 255)
        img[ay:by + 1, ax:bx + 1][m] = col
    return img


def save_png(path: Path, img: np.ndarray) -> None:
    h, w = img.shape[:2]
    png.write(path, w, h, np.concatenate([img, np.full((h, w, 1), 255, np.uint8)], 2).tobytes())


def sampler(sets: LoadSets, vram: Vram, rec: dict, z):
    """(texel sampler for render(), coverage "n/m") or (None, None)."""
    if not rec.get("_tex"):
        return None, None
    key = sets.block_of(rec["off"])
    lm = vram.opening if (vram.opening is not None and key and key[0] == 28) else None
    texs = [vram.texels(key, f, lm) if key else None for f in rec["_tex"]]
    UV, TX, T = z["UV"], z["TX"], z["T"]

    def sample(ti, bary):
        t = int(TX[T[ti, 0]])
        if t < 0 or t >= len(texs) or texs[t] is None:
            return np.full((len(bary), 3), 150.0)
        tex = texs[t]
        uv = bary @ UV[T[ti]].astype(np.float64)
        return tex[np.floor(uv[:, 1] * tex.shape[0]).astype(int) % tex.shape[0],
                   np.floor(uv[:, 0] * tex.shape[1]).astype(int) % tex.shape[1], :3].astype(np.float64)
    return sample, f"{sum(t is not None for t in texs)}/{len(texs)}"


def contact_sheets(disc: Disc, sets: LoadSets, vram: Vram, recs: list[dict], out: Path,
                   stem: str, per: int = 40, cell: int = 150) -> list[str]:
    """Textured front views, each scaled to fit a cell, `per` to a sheet."""
    names = []
    for s0 in range(0, len(recs), per):
        cells = []
        for rec in recs[s0:s0 + per]:
            z = np.load(disc.cache / "posed" / f"{rec['off']:08x}.npz")
            P = z["P"].astype(np.float64)
            sample, _ = sampler(sets, vram, rec, z)
            img = render(P, z["T"], "front", (cell - 10) / max(np.ptp(P[:, [0, 1]], axis=0).max(), 1e-3), sample)
            cells.append(img[:cell, :cell])
        cols = 8
        sheet_img = np.full((cell * ((len(cells) + cols - 1) // cols), cell * cols, 3), 250, np.uint8)
        for i, c in enumerate(cells):
            y, x = cell * (i // cols), cell * (i % cols)
            sheet_img[y:y + c.shape[0], x:x + c.shape[1]] = c
        name = f"{stem}_{s0 // per:02d}.png"
        save_png(out / name, sheet_img)
        names.append(name)
    return names


def preview(disc: Disc, sets: LoadSets, vram: Vram | None, rec: dict, out: Path, ppu=None) -> dict:
    z = np.load(disc.cache / "posed" / f"{rec['off']:08x}.npz")
    P, T = z["P"].astype(np.float64), z["T"]
    big = max(np.ptp(P, axis=0).max(), 1e-3)
    ppu = ppu or 400.0 / big
    res = {}
    sample, coverage = sampler(sets, vram, rec, z) if vram is not None else (None, None)
    a, b = render(P, T, "front", ppu, sample), render(P, T, "side", ppu, sample)
    img = np.full((max(a.shape[0], b.shape[0]), a.shape[1] + b.shape[1] + 20, 3), 236, np.uint8)
    img[:a.shape[0], :a.shape[1]] = a
    img[:b.shape[0], a.shape[1] + 20:] = b
    name = f"{rec['where'].replace('/', '_').replace('+', '_')}_n{rec['nodes']}.png"
    save_png(out / name, img)
    res["render"] = f"{out.name}/{name}"
    if coverage:
        res["texture_coverage"] = coverage
    return res


def sheet(disc: Disc, recs: list[dict], path: Path, ppu: float = 10.0) -> None:
    """Front views at one scale, feet on one baseline, in the given order."""
    cells = []
    for rec in recs:
        z = np.load(disc.cache / "posed" / f"{rec['off']:08x}.npz")
        cells.append(render(z["P"].astype(np.float64), z["T"], "front", ppu))
    if not cells:
        return
    h = max(c.shape[0] for c in cells)
    img = np.full((h, sum(c.shape[1] + 8 for c in cells), 3), 250, np.uint8)
    x = 0
    for c in cells:
        img[h - c.shape[0]:, x:x + c.shape[1]] = c
        x += c.shape[1] + 8
    save_png(path, img)


# ---------------------------------------------------------------------------

def _cached(path: Path, fresh: bool, build):
    if path.exists() and not fresh:
        return pickle.loads(path.read_bytes())
    value = build()
    path.write_bytes(pickle.dumps(value))
    return value


def _hx(v):
    return f"{v:#x}" if isinstance(v, int) else v


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--elf", type=Path, default=MAIN / "config" / "SCUS_971.12")
    ap.add_argument("--extract", type=Path, default=MAIN / "extract")
    ap.add_argument("--iso", type=Path, required=True, help="the user's disc image (INDEX.IDX, GS uploads)")
    ap.add_argument("--gs", type=Path, help="captured opening GS state (player textures)")
    ap.add_argument("--out", type=Path, default=MAIN / "build" / "cut_content" / "characters")
    ap.add_argument("--render", action="store_true", help="write previews and sheets")
    ap.add_argument("--no-statics", action="store_true", help="skip the texture-page scan of 1-4-node blobs")
    ap.add_argument("--static-min-records", type=int, default=48)
    ap.add_argument("--rescan", action="store_true", help="ignore cached blob / rig scans")
    args = ap.parse_args(argv)
    if not args.out.resolve().is_relative_to((MAIN / "build").resolve()) and \
            not args.out.resolve().is_relative_to((REPO / "build").resolve()):
        raise SystemExit("--out must be inside an ignored build/ directory")
    cache = args.out / "cache"
    cache.mkdir(parents=True, exist_ok=True)
    disc = Disc(args)
    disc.cache = cache
    scanned = _cached(cache / "blobs.pkl", args.rescan, lambda: scan_blobs(disc.image))
    blobs = scanned["blobs"]
    rigs = _cached(cache / "rigs.pkl", args.rescan, lambda: scan_rigs(disc.image))
    by_off = {b["off"]: b for b in blobs}
    sets = LoadSets(load_blocks(disc.index))
    tables = ccs.message_tables(disc.elf)
    rows = resolve_op14(disc, sets, tables) + resolve_placements(disc, sets)
    print(f"blobs {len(blobs)} (faces {sum(b['face'] for b in blobs)}), rigs {len(rigs)}, actor rows {len(rows)}")

    wanted = {r[k]["addr"] for r in rows for k in ("body", "face") if r.get(k) and r[k].get("addr") is not None}
    inv = inventory(disc, blobs, rigs, wanted, cache)
    faces = face_fits(disc, blobs, inv)
    parts = shared_parts(disc, inv)
    vram = None if (args.no_statics and not args.render) else Vram(disc, args.gs)
    bodies = {r["body"]["addr"] for r in rows
              if r.get("body") and by_off.get(r["body"]["addr"], {}).get("nodes", 0) >= 5}
    bodies |= {rec["off"] for rec in inv.values() if rec.get("rig") and 19 <= rec["rig"]["n"] <= 21}
    placed = defaultdict(list)
    for r in rows:
        if r["role"] == "placed model (table)" and r.get("body"):
            placed[r["body"]["addr"]].append({"area": r["area"], "sub": r["sub"], "placements": r["placements"],
                                              "positions": r["positions"], "via": r["body"]["via"]})
    found = [] if args.no_statics else statics(disc, sets, vram, blobs, bodies, placed, args.static_min_records)

    for r in rows:
        for k in ("body", "face"):
            if not r.get(k) or r[k].get("addr") is None:
                continue
            a = r[k]["addr"]
            r[k]["where"] = disc.where(a)
            b = by_off.get(a)
            r[k]["blob"] = bool(b)
            if b and not b["face"] and a in inv:
                rec = inv[a]
                r[k].update({"nodes": b["nodes"], "verts": rec.get("verts"), "geometry": rec.get("geometry"),
                             "authored_height": rec.get("authored_height")})
            body = r.get("body", {}).get("addr")
            if k == "face" and a in disc.face_verts and inv.get(body, {}).get("_head") is not None:
                F, head = disc.face_verts[a], inv[body]["_head"]
                d = np.sqrt(((F[:, None, :] - head[None, :, :]) ** 2).sum(-1)).min(1)
                r[k]["fit_median"] = round(float(np.median(d)), 3)
                r[k]["fits_body"] = r[k]["fit_median"] < 0.02
        if "speaker" in r:
            r["speaker_name"] = SPEAKERS.get(r["speaker"])

    by_entry = defaultdict(list)
    for r in rows:
        if r["source"] == "op14" and r["sub_rule"] == "ambiguous":
            by_entry[r["entry"]].append(r)
    for group in by_entry.values():
        if len({r.get("body", {}).get("geometry") for r in group}) == 1:
            for r in group:
                r["sub_rule"] = "ambiguous sub, same geometry in each"

    if args.render:
        for d in ("renders", "textured", "sheets"):
            (args.out / d).mkdir(exist_ok=True)
        done = {}
        for off, rec in sorted(inv.items()):
            if not rec.get("decoded") or rec["geometry"] in done:
                continue
            done[rec["geometry"]] = True
            rec.update(preview(disc, sets, None, rec, args.out / "renders"))
            if rec.get("_tex"):
                t = preview(disc, sets, vram, rec, args.out / "textured")
                rec["render_textured"], rec["texture_coverage"] = t["render"], t.get("texture_coverage")
        missing = [by_off[s["off"]] for s in found if s["off"] not in inv]
        statics_inv = inventory(disc, missing, rigs, {b["off"] for b in missing}, cache)
        names = contact_sheets(disc, sets, vram, [inv.get(s["off"]) or statics_inv[s["off"]] for s in found],
                               args.out / "sheets", "statics")
        for i, s in enumerate(found):
            s["sheet"], s["cell"] = names[i // 40], i % 40   # 8 columns, row-major
        for s in found:
            if s["shares_texture_with"] or s["placed_by"]:
                rec = inv.get(s["off"]) or statics_inv[s["off"]]
                s.update(preview(disc, sets, vram, rec, args.out / "textured"))
        per_area = defaultdict(list)
        for r in rows:
            if r["role"] == "placed model (table)":
                continue
            a = r.get("body", {}).get("addr")
            if a in inv and inv[a].get("decoded") and inv[a] not in per_area[(r["area"], r.get("sub"))]:
                per_area[(r["area"], r.get("sub"))].append(inv[a])
        with open(args.out / "sheets" / "index.tsv", "w") as fh:
            for (area, sub), recs in sorted(per_area.items(), key=lambda kv: (kv[0][0], str(kv[0][1]))):
                name = f"area{area:02d}" + (f"_sub{sub}" if sub is not None else "") + ".png"
                sheet(disc, recs, args.out / "sheets" / name)
                fh.write(name + "\t" + "\t".join(r["where"] for r in recs) + "\n")

    uniq = {}
    for rec in inv.values():
        if rec.get("decoded"):
            u = uniq.setdefault(rec["geometry"], {k: v for k, v in rec.items() if not k.startswith("_")}
                                | {"occurrences": []})
            u["occurrences"].append(rec["where"])
    for u in uniq.values():
        names = set(u["occurrences"])
        u["shared_parts"] = [{"with": q["b"] if q["a"] in names else q["a"],
                              "node_here": q["a_node"] if q["a"] in names else q["b_node"],
                              "node_there": q["b_node"] if q["a"] in names else q["a_node"],
                              "mean_nn": q["mean_nn"]} for q in parts if q["a"] in names or q["b"] in names]
    json_rows = [{k: _hx(v) if k in ("script", "list", "entry", "behaviour", "function", "bind_site",
                                     "model", "bank") else v for k, v in r.items()} for r in rows]
    (args.out / "actors.json").write_text(json.dumps(json_rows, indent=1, default=str))
    (args.out / "inventory.json").write_text(json.dumps({
        "blobs": len(blobs), "faces": sum(b["face"] for b in blobs), "terrain_tile_sets": scanned["tiles"],
        "by_nodes": {"1": sum(1 for b in blobs if not b["face"] and b["nodes"] == 1),
                     "2-4": sum(1 for b in blobs if not b["face"] and 2 <= b["nodes"] <= 4),
                     "5+": sum(1 for b in blobs if not b["face"] and b["nodes"] >= 5)},
        "rigs": len(rigs), "unique": list(uniq.values()), "face_fits": faces,
        "character_bodies": sorted(disc.where(o) for o in bodies)}, indent=1, default=str))
    (args.out / "statics.json").write_text(json.dumps(found, indent=1, default=str))
    cols = ["area", "sub", "source", "where", "role", "model", "body", "nodes", "height", "geometry",
            "face", "face_fits", "speaker", "kind", "bank", "bank_at", "sub_rule", "scene_speakers"]
    with open(args.out / "actors.tsv", "w", newline="") as fh:
        w = csv.writer(fh, delimiter="\t")
        w.writerow(cols)
        for r in rows:
            if r["role"] == "placed model (table)":
                continue
            body, face = r.get("body", {}), r.get("face", {})
            w.writerow([f"AREA{r['area']:02d}", "" if r.get("sub") is None else r["sub"], r["source"],
                        _hx(r.get("entry", r.get("behaviour"))), r["role"], _hx(r.get("model")),
                        body.get("where", ""), body.get("nodes", ""), body.get("authored_height", ""),
                        body.get("geometry", ""),
                        f"{face['slot']:#x} {face.get('where', face.get('via'))}" if face else "",
                        face.get("fits_body", ""), r.get("speaker_name", ""), _hx(r.get("shadow_kind", "")),
                        _hx(r.get("bank")), r.get("bank_at", ""), r["sub_rule"],
                        " ".join(SPEAKERS.get(s, str(s)) for s in r.get("scene_speakers", []))])
    with open(args.out / "placed_models.tsv", "w", newline="") as fh:
        w = csv.writer(fh, delimiter="\t")
        w.writerow(["area", "sub", "behaviour", "table entry", "model", "nodes", "placements", "first position"])
        for r in rows:
            if r["role"] == "placed model (table)":
                body = r.get("body", {})
                w.writerow([f"AREA{r['area']:02d}", "" if r["sub"] is None else r["sub"], _hx(r["behaviour"]),
                            body.get("via", ""), body.get("where", ""), body.get("nodes", ""),
                            " ".join(map(str, r["placements"])), r["positions"][0]])
    actors = [r for r in rows if r["role"] != "placed model (table)"]
    binding = [r for r in actors if r["role"].startswith(("body", "prop", "attach"))]
    print(f"actor rows with a resolved mesh: {sum(1 for r in binding if r.get('body'))}/{len(binding)}; "
          f"faces fitting their body: {sum(1 for r in actors if r.get('face', {}).get('fits_body'))}/"
          f"{sum(1 for r in actors if r.get('face'))}; unique geometries {len(uniq)}; low-node blobs inspected "
          f"{len(found)} (repeating a body texture {sum(1 for s in found if s['shares_texture_with'])}, "
          f"placed {sum(1 for s in found if s['placed_by'])})")
    print(f"wrote {args.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
