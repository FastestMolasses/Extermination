#!/usr/bin/env python3
"""gs_conformance.py - GS conformance harness: designed primitives drawn by
PCSX2's SOFTWARE renderer and recorded pixel for pixel (docs/GS_CONFORMANCE.md).

Clean room: everything here rests on public GS documentation (register and
field meanings, the GIF tag and DMA tag formats, pixel formats) and on
measurements made with this tool.  No emulator source was read.  PCSX2's
software renderer is the reference we have, not real hardware.

Mechanism (only the DebugServer / Pine tools; no emulator change):
  1. Load user slot 04 (first control) hidden through route_capture's
     retrying session (-statefile: the slot file is never written), run to
     the main loop's vsync-wait start 0x1AAFF0 and then to the vsync ISR
     entry 0x1AB140.  By the ISR every GS write of the game's frame has
     landed and nothing reaches the GS until the next loop top
     (docs/CAPTURES_C7.md 5b), so the GS is ours.
  2. Write the batch's DMA tag list and GIF packets into free EE RAM
     (0x01C00000.., zero in every EE capture).  The DebugServer's
     write_memory does NOT reach the DMAC registers (probe, measured), so a
     16-instruction EE routine at 0x01A00000 does it: D2_QWC = 0,
     D2_TADR = tag list, D2_CHCR = 0x105 (DIR from memory, source chain,
     STR), then polls D2_CHCR.STR until 0 and stops on a breakpoint.  It is
     entered with set_pc; the PC and the GPRs it uses are restored exactly.
  3. Save-state snapshot into a free slot >= 40, moved out at once; the GS
     freeze holds the 4 MB local memory (tools/gs_vram.py layout).
  4. Decode every test's buffers with the swizzle maps MEASURED by the
     'layout' batch (known uploads), cross-checked against the documented
     PSMCT32 table (c7cap_partb.gs_word_map).

Every test writes its complete GS state and clears its own buffers first
(Z is pre-filled through a CT32 view so even the byte Z24 never writes is
known), so nothing the game left in the GS reaches a result.

Emulator: the agent-debug fork by default (2026-10-09; it always runs the
software renderer, from fork slot 04 of fork-states/manifest.json, outputs in
build/fork_refs/b16/gscap/ unless GSCAP_OUT is set); `--emulator legacy` =
the v2.6.3 app (Renderer must then be switched to 13 by hand) until it is
retired.  Options go before the subcommand.

Subcommands:
  capture [BATCH ...]    emulator: build, kick, snapshot (Renderer must be 13)
  decode  [BATCH ...]    no emulator: decode snapshots into per-test buffers
  list                   the batches and tests

Outputs (ignored): build/b16/gscap/<batch>/batch.json (every test's inputs:
register writes, uploads, structured primitives), packet.bin, snap/gs.bin,
kick.json, <test>.npz (decoded colour and Z), <test>.png, decode.json.
Analysis: tools/gs_conformance_analyse.py.
"""
from __future__ import annotations

import argparse
import hashlib
import os
import json
import shutil
import struct
import subprocess
import sys
import time
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

OUT = Path(os.environ.get("GSCAP_OUT", ROOT / "build/b16/gscap"))
LIVE_INI = ROOT / "build/startup-reference/inis/PCSX2.ini"
SSTATES = ROOT / "build/startup-reference/portable-data/sstates"
SERIAL = "SCUS-97112 (0AE679AF)"
SOURCE_SLOT = "04"                  # first control (user slot, read only)
FRAME_COUNTER = 0x70003B64
VSYNC_WAIT = 0x1AAFF0
VSYNC_ISR = 0x1AB140
FREE_SLOT_MIN = 40
PACKET_BASE = 0x01C00000            # zero in every EE capture (0x19F0000..0x1FF0000)
TAG_BASE = 0x01BF0000
CODE_BASE = 0x01A00000
CODE_DATA = CODE_BASE + 0x100
FREEZE_VRAM = 425                   # GS freeze: header bytes before local memory (tools/gs_vram.py)
LOCALMEM = 0x400000
PAGE_BYTES = 8192

# DMAC channel 2 (GIF) and control registers (EE hardware register map)
D2_CHCR, D2_MADR, D2_QWC, D2_TADR = 0x1000A000, 0x1000A010, 0x1000A020, 0x1000A030
D_CTRL, D_STAT, D_PCR, D_ENABLER, GIF_STAT = 0x1000E000, 0x1000E010, 0x1000E020, 0x1000F520, 0x10003020

# ---------------------------------------------------------------------------
# GS general-purpose register addresses (GS User's Manual register map)
REG = {
    "PRIM": 0x00, "RGBAQ": 0x01, "ST": 0x02, "UV": 0x03, "XYZF2": 0x04, "XYZ2": 0x05,
    "TEX0_1": 0x06, "TEX0_2": 0x07, "CLAMP_1": 0x08, "CLAMP_2": 0x09, "FOG": 0x0A,
    "XYZF3": 0x0C, "XYZ3": 0x0D, "TEX1_1": 0x14, "TEX1_2": 0x15, "TEX2_1": 0x16,
    "TEX2_2": 0x17, "XYOFFSET_1": 0x18, "XYOFFSET_2": 0x19, "PRMODECONT": 0x1A,
    "PRMODE": 0x1B, "TEXCLUT": 0x1C, "SCANMSK": 0x22, "TEXA": 0x3B, "FOGCOL": 0x3D,
    "TEXFLUSH": 0x3F, "SCISSOR_1": 0x40, "SCISSOR_2": 0x41, "ALPHA_1": 0x42,
    "ALPHA_2": 0x43, "DIMX": 0x44, "DTHE": 0x45, "COLCLAMP": 0x46, "TEST_1": 0x47,
    "TEST_2": 0x48, "PABE": 0x49, "FBA_1": 0x4A, "FBA_2": 0x4B, "FRAME_1": 0x4C,
    "FRAME_2": 0x4D, "ZBUF_1": 0x4E, "ZBUF_2": 0x4F, "BITBLTBUF": 0x50, "TRXPOS": 0x51,
    "TRXREG": 0x52, "TRXDIR": 0x53,
}
REG_NAME = {v: k for k, v in REG.items()}
PSM = {"CT32": 0x00, "CT24": 0x01, "CT16": 0x02, "CT16S": 0x0A, "T8": 0x13, "T4": 0x14,
       "Z32": 0x30, "Z24": 0x31, "Z16": 0x32, "Z16S": 0x3A}
PRIM_TYPE = {"point": 0, "line": 1, "linestrip": 2, "tri": 3, "tristrip": 4, "trifan": 5, "sprite": 6}


def f32bits(v: float) -> int:
    return struct.unpack("<I", struct.pack("<f", v))[0]


def fx4(v: float) -> int:
    """12.4 fixed point (unsigned 16-bit), as XYZ/XYOFFSET/UV use."""
    r = round(v * 16)
    assert 0 <= r < 0x10000 and abs(r - v * 16) < 1e-6, v
    return r


# -- register value encoders (field positions: GS User's Manual) --------------
def FRAME(fbp, fbw_px, psm="CT32", fbmsk=0):
    return fbp | (fbw_px // 64) << 16 | PSM[psm] << 24 | fbmsk << 32


def ZBUF(zbp, psm="Z24", zmsk=0):
    return zbp | (PSM[psm] & 0xF) << 24 | zmsk << 32


def XYOFFSET(ofx, ofy):
    return fx4(ofx) | fx4(ofy) << 32


def SCISSOR(x0, x1, y0, y1):
    return x0 | x1 << 16 | y0 << 32 | y1 << 48


ATST = {"NEVER": 0, "ALWAYS": 1, "LESS": 2, "LEQUAL": 3, "EQUAL": 4, "GEQUAL": 5, "GREATER": 6, "NOTEQUAL": 7}
AFAIL = {"KEEP": 0, "FB_ONLY": 1, "ZB_ONLY": 2, "RGB_ONLY": 3}
ZTST = {"NEVER": 0, "ALWAYS": 1, "GEQUAL": 2, "GREATER": 3}


def TEST(ate=0, atst="ALWAYS", aref=0, afail="KEEP", date=0, datm=0, zte=1, ztst="ALWAYS"):
    return (ate | ATST[atst] << 1 | aref << 4 | AFAIL[afail] << 12 | date << 14 | datm << 15
            | zte << 16 | ZTST[ztst] << 17)


def ALPHA(a, b, c, d, fix=0):
    """A/B/D: 0 Cs, 1 Cd, 2 zero; C: 0 As, 1 Ad, 2 FIX."""
    return a | b << 2 | c << 4 | d << 6 | fix << 32


def PRIM(kind, iip=0, tme=0, fge=0, abe=0, aa1=0, fst=0, ctxt=0, fix=0):
    return (PRIM_TYPE[kind] | iip << 3 | tme << 4 | fge << 5 | abe << 6 | aa1 << 7 | fst << 8
            | ctxt << 9 | fix << 10)


def RGBAQ(r, g, b, a=0x80, q=1.0):
    return r | g << 8 | b << 16 | a << 24 | f32bits(q) << 32


def ST(s, t):
    return f32bits(s) | f32bits(t) << 32


def UV(u, v):
    return fx4(u) | fx4(v) << 16


def XYZ2(x, y, z=0):
    return fx4(x) | fx4(y) << 16 | (z & 0xFFFFFFFF) << 32


def XYZF2(x, y, z=0, f=0xFF):
    return fx4(x) | fx4(y) << 16 | (z & 0xFFFFFF) << 32 | f << 56


def FOG(f):
    return f << 56


def FOGCOL(r, g, b):
    return r | g << 8 | b << 16


def TEX0(tbp0, tbw_px, psm, tw, th, tcc=1, tfx=0, cbp=0, cpsm="CT32", csm=0, csa=0, cld=0):
    return (tbp0 | max(1, tbw_px // 64) << 14 | PSM[psm] << 20 | tw << 26 | th << 30 | tcc << 34
            | tfx << 35 | cbp << 37 | PSM[cpsm] << 51 | csm << 55 | csa << 56 | cld << 61)


def TEX1(lcm=0, mxl=0, mmag=0, mmin=0, mtba=0, l=0, k=0):
    return lcm | mxl << 2 | mmag << 5 | mmin << 6 | mtba << 9 | l << 19 | (k & 0xFFF) << 32


def CLAMP(wms=0, wmt=0, minu=0, maxu=0, minv=0, maxv=0):
    return wms | wmt << 2 | minu << 4 | maxu << 14 | minv << 24 | maxv << 34


def TEXA(ta0=0, aem=0, ta1=0x80):
    return ta0 | aem << 15 | ta1 << 32


def DIMX(m):
    v = 0
    for i in range(4):
        for j in range(4):
            v |= (m[i][j] & 7) << (16 * i + 4 * j)
    return v


def BITBLTBUF(dbp, dbw_px, dpsm):
    return (dbp << 32) | max(1, dbw_px // 64) << 48 | PSM[dpsm] << 56


def TRXPOS(dx=0, dy=0):
    return dx << 32 | dy << 48


def TRXREG(w, h):
    return w | h << 32


# -- GIF tags and DMA tags -----------------------------------------------------
def giftag(nloop, eop=0, pre=0, prim=0, flg=0, nreg=1, regs=0xE):
    lo = nloop | eop << 15 | pre << 46 | prim << 47 | flg << 58 | (nreg & 0xF) << 60
    return struct.pack("<QQ", lo, regs)


def ad_packet(writes: list[tuple[str, int]], eop=0) -> bytes:
    out = b""
    for i in range(0, len(writes), 0x7FFF):
        part = writes[i:i + 0x7FFF]
        out += giftag(len(part), eop=eop if i + 0x7FFF >= len(writes) else 0)
        out += b"".join(struct.pack("<QQ", v & 0xFFFFFFFFFFFFFFFF, REG[r]) for r, v in part)
    return out


def image_packet(data: bytes) -> bytes:
    data = data + b"\0" * ((-len(data)) % 16)
    n = len(data) // 16
    out = b""
    for i in range(0, n, 0x7FFF):
        k = min(0x7FFF, n - i)
        out += giftag(k, flg=2, nreg=0, regs=0) + data[i * 16:(i + k) * 16]
    return out


def dma_chain(base: int, size: int) -> bytes:
    """REF tags over the packet in 0xFFFF-qword pieces, the last one REFE."""
    qw = size // 16
    tags, off = [], 0
    while qw:
        n = min(qw, 0xFFFF)
        qw -= n
        ident = 0 if qw == 0 else 3                      # REFE / REF
        tags.append(struct.pack("<QQ", n | ident << 28 | (base + off) << 32, 0))
        off += n * 16
    return b"".join(tags)


# ---------------------------------------------------------------------------
# Test model
class Test:
    """One conformance test: its own colour buffer (and Z buffer when it
    reads Z back), a full state reset, a clear, then its items.  `items`
    are ('ad', [(reg, value), ...]) or ('image', {...}) in packet order."""

    def __init__(self, name: str, desc: str, w: int = 64, h: int = 64, psm: str = "CT32",
                 zpsm: str | None = None, clear: int = 0, zclear: int = 0, ofs=(0.0, 0.0),
                 zfill_top: int = 0x5A, noclear: bool = False):
        assert w % 64 == 0
        self.name, self.desc, self.w, self.h, self.psm = name, desc, w, h, psm
        self.zpsm, self.clear, self.zclear, self.ofs = zpsm, clear, zclear, ofs
        self.zfill = (zfill_top << 24) | (zclear & 0xFFFFFF) if zpsm == "Z24" else zclear
        self.noclear = noclear
        self.items: list = []
        self.meta: dict = {}
        self.fbp = self.zbp = None

    # page footprint of a buffer (page = 64x32 for 32-bit, 64x64 for 16-bit)
    @staticmethod
    def pages(w, h, psm):
        ph = 64 if psm in ("CT16", "CT16S", "Z16", "Z16S") else 32
        return (w // 64) * ((h + ph - 1) // ph)

    def footprint(self) -> int:
        return self.pages(self.w, self.h, self.psm) + (self.pages(self.w, self.h, self.zpsm) if self.zpsm else 0)

    def ad(self, *writes):
        if self.items and self.items[-1][0] == "ad":
            self.items[-1][1].extend(writes)
        else:
            self.items.append(("ad", list(writes)))

    def image(self, dbp: int, dbw: int, psm: str, w: int, h: int, data: bytes, dx=0, dy=0, label=""):
        self.items.append(("image", {"dbp": dbp, "dbw": dbw, "psm": psm, "w": w, "h": h, "dx": dx,
                                     "dy": dy, "data": data, "label": label}))

    def x(self, v):
        return v + self.ofs[0]

    def y(self, v):
        return v + self.ofs[1]

    def reset_writes(self, scratch_zbp: int) -> list[tuple[str, int]]:
        """Full known state, Z pre-fill (through a CT32 view) and colour clear."""
        w, h = self.w, self.h
        ox, oy = self.ofs
        base = [("XYOFFSET_1", XYOFFSET(ox, oy)), ("SCISSOR_1", SCISSOR(0, w - 1, 0, h - 1)),
                ("PRMODECONT", 1), ("COLCLAMP", 1), ("DTHE", 0), ("DIMX", 0), ("PABE", 0),
                ("FBA_1", 0), ("SCANMSK", 0), ("TEXA", TEXA()), ("FOGCOL", 0), ("FOG", 0),
                ("ALPHA_1", ALPHA(0, 2, 2, 2, 0x80)), ("TEX1_1", 0), ("CLAMP_1", 0),
                ("TEST_1", TEST(zte=1, ztst="ALWAYS")), ("ZBUF_1", ZBUF(scratch_zbp, "Z24", zmsk=1))]
        spr = lambda word, z=0: [("PRIM", PRIM("sprite")),
                                 ("RGBAQ", RGBAQ(word & 0xFF, word >> 8 & 0xFF, word >> 16 & 0xFF, word >> 24 & 0xFF)),
                                 ("XYZ2", XYZ2(ox, oy, z)), ("XYZ2", XYZ2(ox + w, oy + h, z))]
        out = list(base)
        if self.noclear:
            return out + [("FRAME_1", FRAME(self.fbp, w, "CT32" if self.psm.startswith("Z3") or self.psm == "Z24"
                                                  else self.psm))]
        if self.zpsm:
            zp = "CT16" if self.zpsm in ("Z16", "Z16S") else "CT32"
            out += [("FRAME_1", FRAME(self.zbp, w, zp))] + spr(self.zfill)
        out += [("FRAME_1", FRAME(self.fbp, w, self.psm))] + spr(self.clear)
        if self.zpsm:
            out += [("ZBUF_1", ZBUF(self.zbp, self.zpsm, zmsk=0))]
        return out

    def packet(self, scratch_zbp: int) -> tuple[bytes, list]:
        pkt = ad_packet(self.reset_writes(scratch_zbp))
        rec = [{"ad": [[r, hex(v)] for r, v in self.reset_writes(scratch_zbp)], "role": "reset"}]
        for kind, body in self.items:
            if kind == "ad":
                pkt += ad_packet(body)
                rec.append({"ad": [[r, hex(v & 0xFFFFFFFFFFFFFFFF)] for r, v in body]})
            else:
                b = body
                writes = [("BITBLTBUF", BITBLTBUF(b["dbp"], b["dbw"], b["psm"])),
                          ("TRXPOS", TRXPOS(b["dx"], b["dy"])), ("TRXREG", TRXREG(b["w"], b["h"])),
                          ("TRXDIR", 0)]
                pkt += ad_packet(writes) + image_packet(b["data"]) + ad_packet([("TEXFLUSH", 0)])
                rec.append({"image": {k: v for k, v in b.items() if k != "data"},
                            "ad": [[r, hex(v)] for r, v in writes], "data_sha256":
                            hashlib.sha256(b["data"]).hexdigest(), "data_bytes": len(b["data"]),
                            "then": [["TEXFLUSH", "0x0"]]})
        return pkt, rec


class Batch:
    TEST_PAGES = 0x1B8          # pages 0..0x1B7 for test buffers (+ scratch Z at the end)
    TEX_PAGE0 = 0x1C0           # pages 0x1C0..0x1FF for textures / CLUTs

    def __init__(self, name: str, desc: str):
        self.name, self.desc = name, desc
        self.tests: list[Test] = []
        self.next_page = 0
        self.tex_next = self.TEX_PAGE0
        self.scratch_zbp = self.TEST_PAGES           # 8 pages: covers 512x64 Z24 with ZMSK 1

    def tex_alloc(self, pages: int) -> int:
        """Returns a BLOCK address (TBP0/CBP/DBP units: 64 words)."""
        p = self.tex_next
        self.tex_next += pages
        assert self.tex_next <= 0x200, f"{self.name}: texture area full"
        return p * 32

    def add(self, t: Test) -> Test:
        t.fbp = self.next_page
        self.next_page += Test.pages(t.w, t.h, t.psm)
        if t.zpsm:
            t.zbp = self.next_page
            self.next_page += Test.pages(t.w, t.h, t.zpsm)
        assert self.next_page <= self.TEST_PAGES, f"{self.name}: test area full at {t.name}"
        self.tests.append(t)
        return t

    def build(self) -> tuple[bytes, dict]:
        pkt = b""
        doc = {"batch": self.name, "desc": self.desc, "tests": []}
        for t in self.tests:
            p, rec = t.packet(self.scratch_zbp)
            doc["tests"].append({"name": t.name, "desc": t.desc, "w": t.w, "h": t.h, "psm": t.psm,
                                 "fbp": t.fbp, "zpsm": t.zpsm, "zbp": t.zbp, "clear": hex(t.clear),
                                 "zfill": hex(t.zfill), "ofs": list(t.ofs), "meta": t.meta,
                                 "packet_offset": len(pkt), "packet_bytes": len(p), "items": rec})
            pkt += p
        # fence: a known 64x32 pattern proves the whole packet was processed
        fence = [("FRAME_1", FRAME(0x1BF, 64, "CT32")), ("SCISSOR_1", SCISSOR(0, 63, 0, 31)),
                 ("XYOFFSET_1", 0), ("TEST_1", TEST()), ("ZBUF_1", ZBUF(self.scratch_zbp, zmsk=1)),
                 ("PRIM", PRIM("sprite")), ("RGBAQ", RGBAQ(0x5E, 0xC7, 0x0F, 0x3A)),
                 ("XYZ2", XYZ2(0, 0)), ("XYZ2", XYZ2(64, 32))]
        pkt += ad_packet(fence, eop=1)
        doc["fence"] = {"page": 0x1BF, "word": "0x3a0fc75e", "ad": [[r, hex(v)] for r, v in fence]}
        doc["packet_bytes"] = len(pkt)
        doc["packet_sha256"] = hashlib.sha256(pkt).hexdigest()
        return pkt, doc


# ---------------------------------------------------------------------------
# Ini guard (the switch itself is a manual step; docs/GS_CONFORMANCE.md)
def ini_renderer() -> int | None:
    section = None
    for line in LIVE_INI.read_text().splitlines():
        s = line.strip()
        if s.startswith("["):
            section = s
        elif section == "[EmuCore/GS]" and s.split("=")[0].strip() == "Renderer":
            return int(s.split("=", 1)[1])
    return None


RESTORE_HINT = (f"RESTORE: with PCSX2 stopped, set '{LIVE_INI.relative_to(ROOT)}' [EmuCore/GS] "
                f"'Renderer = 13' back to 'Renderer = 17' and diff against build/b16/gscap/pre/inis_PCSX2.ini.")


def require_software_renderer() -> None:
    import route_capture as rc
    if rc.FORK:
        return              # ForkSession always launches with Renderer = 13 on its own scratch ini
    if ini_renderer() != 13:
        raise SystemExit(f"Renderer is {ini_renderer()}, not 13 (software).  The switch is manual "
                         f"(docs/GS_CONFORMANCE.md).  " + RESTORE_HINT)


def no_emulator_left() -> bool:
    return subprocess.run(["pgrep", "-f", "PCSX2.app/Contents/MacOS/PCSX2"],
                          capture_output=True).returncode != 0


def free_slot() -> int:
    used = {int(p.name.split(".")[-2]) for p in SSTATES.glob(f"{SERIAL}.*.p2s")
            if p.name.split(".")[-2].isdigit()}
    slot = next(s for s in range(FREE_SLOT_MIN, 64) if s not in used)
    assert slot >= FREE_SLOT_MIN
    return slot


def fork_out(path: Path) -> Path:
    """On the fork, an output inside the v2.6.3 captures (build/b16/...) is
    mirrored under build/fork_refs/b16/...; any other path is used as given."""
    import route_capture as rc
    legacy = (ROOT / "build/b16").resolve()
    p = Path(path).resolve()
    if rc.FORK and (p == legacy or legacy in p.parents):
        return ROOT / "build/fork_refs/b16" / p.relative_to(legacy)
    return Path(path)


def vram_of(gs: bytes) -> bytes:
    assert len(gs) - LOCALMEM - FREEZE_VRAM == 84, "unexpected GS freeze layout"
    return gs[FREEZE_VRAM:FREEZE_VRAM + LOCALMEM]


# ---------------------------------------------------------------------------
# Emulator side
def _pc(s) -> int:
    return int(s.debug.call({"cmd": "evaluate", "expression": "pc"})["result"]) & 0xFFFFFFFF


def _run_to(s, addr: int, tries: int = 8) -> None:
    s.debug.call({"cmd": "set_breakpoint", "address": addr, "description": "gscap"})
    try:
        for _ in range(tries):
            s._resume_to_boundary()
            if _pc(s) == addr:
                return
        raise RuntimeError(f"did not reach {addr:#x} (pc {_pc(s):#x})")
    finally:
        s.debug.call({"cmd": "remove_breakpoint", "address": addr})


def write_ee(s, addr: int, data: bytes, chunk: int = 0x4000) -> None:
    for i in range(0, len(data), chunk):
        s.write(addr + i, data[i:i + chunk])


def hwregs(s) -> dict:
    names = {"D2_CHCR": D2_CHCR, "D2_MADR": D2_MADR, "D2_QWC": D2_QWC, "D2_TADR": D2_TADR,
             "D_CTRL": D_CTRL, "D_STAT": D_STAT, "D_PCR": D_PCR, "D_ENABLER": D_ENABLER,
             "GIF_STAT": GIF_STAT}
    return {n: hex(s.u32(a)) for n, a in names.items()}


def _i(op, rs=0, rt=0, imm=0):         # MIPS I-type encoding (MIPS ISA manual)
    return op << 26 | rs << 21 | rt << 16 | (imm & 0xFFFF)


# The DebugServer's write_memory does not reach the DMAC registers (probe:
# D2_QWC/MADR/CHCR read back unchanged and nothing was drawn), so the EE makes
# the stores.  Constant code (the tag-list address is loaded from a data word)
# so a later batch never meets a stale recompiled block.
T0, T1, T3, T4 = 8, 9, 11, 12
KICK_CODE = [
    _i(0x0F, 0, T0, 0x1000),                 # t0 = 0x10000000
    _i(0x0D, T0, T0, 0xA000),                # t0 |= 0xA000: D2 channel base
    _i(0x0F, 0, T4, CODE_DATA >> 16),        # t4 = data block
    _i(0x0D, T4, T4, CODE_DATA & 0xFFFF),
    _i(0x23, T4, T1, 0),                     # t1 = tag list address
    _i(0x2B, T0, 0, 0x20),                   # D2_QWC = 0
    _i(0x2B, T0, T1, 0x30),                  # D2_TADR = t1
    _i(0x0D, 0, T3, 0x105),                  # t3 = DIR 1 | MOD chain | STR
    _i(0x2B, T0, T3, 0x00),                  # D2_CHCR = t3
    0x0000000F,                              # sync
    _i(0x23, T0, T3, 0x00),                  # poll: t3 = D2_CHCR
    _i(0x0C, T3, T3, 0x100),                 # t3 &= STR
    _i(0x05, T3, 0, -3),                     # while STR set: back to the poll load
    0x00000000,                              # (delay slot)
    0x0000000F,                              # sync
    0x00000000,                              # stop here
]
KICK_END = CODE_BASE + 4 * (len(KICK_CODE) - 1)


def _gpr_category(s) -> int:
    data = s.debug.call({"cmd": "read_registers"})["data"]
    names = [k for k in data if k not in ("pc", "hi", "lo")]
    return next(i for i, k in enumerate(names) if k.upper() == "GPR")


def _gprs(s, cat) -> list[dict]:
    data = s.debug.call({"cmd": "read_registers", "category": cat})["data"]
    return next(v for k, v in data.items() if k not in ("pc", "hi", "lo"))["regs"]


def kick_and_wait(s, packet: bytes) -> dict:
    """Write tags + packet, run the injected kick/poll routine to its end,
    restore PC and GPRs.  Returns the register evidence."""
    assert len(packet) % 16 == 0
    tags = dma_chain(PACKET_BASE, len(packet))
    write_ee(s, PACKET_BASE, packet)
    write_ee(s, TAG_BASE, tags)
    assert hashlib.sha256(s.read(PACKET_BASE, len(packet))).digest() == hashlib.sha256(packet).digest(), \
        "packet readback differs"
    code = b"".join(struct.pack("<I", w) for w in KICK_CODE)
    if s.read(CODE_BASE, len(code)) != code:
        write_ee(s, CODE_BASE, code)
    write_ee(s, CODE_DATA, struct.pack("<I", TAG_BASE) + b"\0" * 12)
    before = hwregs(s)
    cat = _gpr_category(s)
    saved = _gprs(s, cat)
    pc0 = _pc(s)
    t0 = time.monotonic()
    s.debug.call({"cmd": "set_pc", "cpu": "ee", "value": hex(CODE_BASE)})
    _run_to(s, KICK_END, tries=2)
    after = hwregs(s)
    return {"before": before, "after": after, "pc0": pc0, "cat": cat, "saved": saved,
            "seconds": round(time.monotonic() - t0, 2), "tags": len(tags) // 16,
            "packet_qw": len(packet) // 16}


def restore_after_kick(s, ev: dict) -> None:
    s.debug.call({"cmd": "set_pc", "cpu": "ee", "value": hex(ev["pc0"])})
    for idx in (T0, T1, T3, T4):
        s.debug.call({"cmd": "write_register", "cpu": "ee", "category": ev["cat"], "index": idx,
                      "value": ev["saved"][idx]["value"]})
    restored = _gprs(s, ev["cat"])
    assert _pc(s) == ev["pc0"] and all(restored[i]["value"] == ev["saved"][i]["value"] for i in range(32)), \
        "register restore failed"


def snapshot(s, folder: Path) -> dict:
    snap = s.snapshot(folder, slot=free_slot())
    for name in ("eeMemory.bin", "scratchpad.bin", "state.p2s"):
        p = folder / name
        if p.exists():
            p.unlink()
    (folder / "snapshot.json").write_text(json.dumps(snap, indent=1) + "\n")
    return snap


def open_session(log_dir: Path):
    import route_capture as rc
    return rc.open_session(rc.slot_path(SOURCE_SLOT), log_dir=log_dir)


def save_inputs(b: Batch, out: Path, pkt: bytes, doc: dict) -> None:
    import numpy as np
    (out / "packet.bin").write_bytes(pkt)
    (out / "batch.json").write_text(json.dumps(doc, indent=1) + "\n")
    extra = {}
    if hasattr(b, "textures"):
        extra.update({f"tex_{k}": v for k, v in b.textures.arrays.items()})
    if hasattr(b, "dest"):
        extra["dest"] = b.dest
    if extra:
        np.savez_compressed(out / "inputs.npz", **extra)


def capture(names: list[str]) -> None:
    import gs_conformance_suite as suite
    require_software_renderer()
    batches = [b for b in suite.batches() if not names or b.name in names]
    OUT.mkdir(parents=True, exist_ok=True)
    try:
        with open_session(OUT / "logs") as s:
            for b in batches:
                t0 = time.monotonic()
                out = OUT / b.name
                if out.exists():
                    shutil.rmtree(out)
                out.mkdir(parents=True)
                pkt, doc = b.build()
                save_inputs(b, out, pkt, doc)
                _run_to(s, VSYNC_WAIT)
                _run_to(s, VSYNC_ISR)
                at = {"counter": s.u32(FRAME_COUNTER), "pc": hex(_pc(s)),
                      "fi": s.read(0x810E80, 4)[0], "fld": s.read(0x810E88, 4)[0]}
                ev = kick_and_wait(s, pkt)
                done = int(ev["after"]["D2_CHCR"], 16) & 0x100 == 0
                snap = snapshot(s, out / "snap")
                restore_after_kick(s, ev)
                rec = {"at": at, "dma_done": done, "before": ev["before"], "after": ev["after"],
                       "seconds_in_kick": ev["seconds"], "tags": ev["tags"], "packet_qw": ev["packet_qw"],
                       "snapshot": snap, "renderer": ini_renderer(),
                       "seconds": round(time.monotonic() - t0, 1)}
                (out / "kick.json").write_text(json.dumps(rec, indent=1) + "\n")
                print(b.name, "tests", len(b.tests), "qw", ev["packet_qw"], "dma_done", done,
                      "counter", at["counter"], rec["seconds"], "s", flush=True)
    finally:
        import route_capture as rc
        print("no emulator process left:", no_emulator_left())
        if not rc.FORK:
            print(RESTORE_HINT)


# ---------------------------------------------------------------------------
# Decode: measured swizzle maps (batch 'layout')
def load_maps():
    """The layout batch's measured maps of this run (OUT is final only after main())."""
    import numpy as np
    return dict(np.load(OUT / "layout" / "maps.npz"))


def decode_buffer(vram: bytes, page: int, w: int, h: int, psm: str, maps):
    """(h, w) array of the buffer's pixel values, through the measured
    per-page maps: 'ct32' / 'z32' (64x32 page -> word index), 'ct16' (64x64
    page -> halfword index), and the measured page order (row-major)."""
    import numpy as np
    if psm in ("CT32", "CT24", "Z32", "Z24"):
        key = "z32" if psm.startswith("Z") else "ct32"
        pw, ph, unit = 64, 32, np.dtype("<u4")
    elif psm in ("CT16", "CT16S", "Z16", "Z16S"):
        key = "z16" if psm.startswith("Z") else "ct16"
        pw, ph, unit = 64, 64, np.dtype("<u2")
    else:
        raise ValueError(psm)
    m = maps[key]
    arr = np.frombuffer(vram, dtype=unit)
    per_page = PAGE_BYTES // unit.itemsize
    ppr = w // pw
    y, x = np.mgrid[0:h, 0:w]
    idx = (page + (y // ph) * ppr + x // pw) * per_page + m[y % ph, x % pw]
    return arr[idx]


def write_png(path: Path, rgba_u32, opaque=True):
    import numpy as np
    h, w = rgba_u32.shape
    b = rgba_u32.astype("<u4").view(np.uint8).reshape(h, w, 4).copy()
    if opaque:
        b[..., 3] = 255
    raw = b"".join(b"\0" + b[y].tobytes() for y in range(h))
    def chunk(t, d):
        return struct.pack(">I", len(d)) + t + d + struct.pack(">I", zlib.crc32(t + d) & 0xFFFFFFFF)
    path.write_bytes(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 6, 0, 0, 0))
                     + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b""))


def ct16_to_rgba(v):
    import numpy as np
    v = v.astype(np.uint32)
    r, g, b, a = v & 31, v >> 5 & 31, v >> 10 & 31, v >> 15 & 1
    return (r << 3) | (g << 3) << 8 | (b << 3) << 16 | (a * 0x80) << 24


def decode(names: list[str]) -> None:
    import numpy as np
    import gs_conformance_suite as suite
    wanted = [b.name for b in suite.batches() if not names or b.name in names]
    if "layout" in wanted:
        suite.derive_maps(OUT / "layout")
    maps = load_maps()
    for name in wanted:
        out = OUT / name
        if not (out / "snap/gs.bin").exists():
            print(name, "not captured")
            continue
        doc = json.loads((out / "batch.json").read_text())
        vram = vram_of((out / "snap/gs.bin").read_bytes())
        fence = decode_buffer(vram, doc["fence"]["page"], 64, 32, "CT32", maps)
        fence_ok = bool((fence == int(doc["fence"]["word"], 16)).all())
        res = {"batch": name, "fence_ok": fence_ok, "vram_sha256": hashlib.sha256(vram).hexdigest(),
               "tests": {}}
        for t in doc["tests"]:
            col = decode_buffer(vram, t["fbp"], t["w"], t["h"], t["psm"], maps)
            arrays = {"color": col}
            if t["zpsm"]:
                arrays["z"] = decode_buffer(vram, t["zbp"], t["w"], t["h"], t["zpsm"], maps)
            np.savez_compressed(out / f"{t['name']}.npz", **arrays)
            png = col if t["psm"] in ("CT32", "CT24") else ct16_to_rgba(col)
            write_png(out / f"{t['name']}.png", png)
            res["tests"][t["name"]] = {"distinct": int(len(np.unique(col))),
                                        "sha256": hashlib.sha256(col.tobytes()).hexdigest()[:16]}
        (out / "decode.json").write_text(json.dumps(res, indent=1) + "\n")
        print(name, "fence_ok", fence_ok, "tests", len(doc["tests"]))


def list_batches() -> None:
    import gs_conformance_suite as suite
    for b in suite.batches():
        pkt, doc = b.build()
        json.dumps(doc)
        print(f"{b.name:12s} tests {len(b.tests):3d}  pages {b.next_page:3d}  tex pages "
              f"{b.tex_next - Batch.TEX_PAGE0:3d}  packet {len(pkt) // 16} qw  - {b.desc}")


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    for c in ("capture", "decode"):
        p = sub.add_parser(c)
        p.add_argument("batches", nargs="*")
    sub.add_parser("list")
    import route_capture as rc
    rc.add_emulator_args(ap)
    a = ap.parse_args()
    global OUT
    rc.apply_emulator_args(a)
    if rc.FORK and "GSCAP_OUT" not in os.environ:
        OUT = ROOT / "build/fork_refs/b16/gscap"     # never into the v2.6.3 captures
    OUT = fork_out(OUT)
    if a.cmd == "capture":
        capture(a.batches)
    elif a.cmd == "decode":
        decode(a.batches)
    else:
        list_batches()


if __name__ == "__main__":
    main()
