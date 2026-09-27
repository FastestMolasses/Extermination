#!/usr/bin/env python3
"""c7cap_partb.py - C7 original captures, part B: framebuffer feasibility and the module-0x21 load.

Drives the ORIGINAL game in the hidden, MCP-enabled PCSX2 through
tools/pcsx2_session.py / tools/route_capture.py (exact one-frame steps at the
main-loop top 0x1AAF28) and records (docs/CAPTURES_C7.md sections 5 and 6):

  fb   Framebuffer capture feasibility for the port's Original-profile pixel
       harness.  At three points (user slot 04 = first control, the end
       snapshots of route beats 03 and 07) it reads the GS privileged
       registers (PMODE, SMODE1/2, SYNCH/V, DISPFB1/2, DISPLAY1/2, BGCOLOR,
       CSR) through Pine and through the DebugServer at every main-loop top
       for a few frames, takes one save-state snapshot (to a free slot >= 16
       that is moved into the output folder at once) and analyses its GS
       freeze: the drawing contexts' FRAME/ZBUF/SCISSOR/XYOFFSET and the
       contents of GS local memory at the displayed and drawn buffers.  It
       changes no emulator setting and no file of the emulator.
  h7   The panel prompt's module-0x21 load (route beat 03, the BATTERY page
       of the status screen): the beat is re-driven by route_capture's own
       closed-loop beat function from its recorded source snapshot; per
       frame it records the loader slot 2 record 0x28A790, D_00282157,
       D_00275BD8, the status UI bytes 0x810130 and route_capture's row; over
       the load window it adds per-field samples (vsync-ISR entry 0x1AB140) of
       the same fields and of the IOP CDVD registers (0x1F402004..0F), IOP
       DMA channel 3 (0x1F8010B0..BB) and the IOP PC.  A second pass
       ("writer") arms an EE write memcheck on D_00275BD8 over the frames
       before the load request only, to name the store that raises it.

  fb2  Framebuffers with PCSX2's SOFTWARE renderer (section 5b).  The
       renderer switch is a manual, documented step (user decision
       2026-09-26: software renderer only during capture jobs): fb2 refuses to
       start unless build/startup-reference/inis/PCSX2.ini says
       [EmuCore/GS] Renderer = 13 and always ends by printing how to restore
       Renderer = 17.  Per point (the 16 route snapshots + the three fb
       points) it loads the state, snapshots the aligned loop top (s0), steps
       two frames with a neutral pad and synchronisation snapshots at the
       vsync-wait start and the vsync ISR inside each frame, and decodes the
       final loop top (s2): displayed.bin / draw.bin (512x224 PSMCT32,
       de-swizzled), z.bin (PSMZ24), displayed.png, draw.png, a 448-line
       line-doubled PREVIEW, registers.json and meta.json (frame alignment,
       field parity, per-stage buffer writes, decode proof).
       fb2-decode repeats the decode without the emulator.

Everything written here is derived from the user's own disc and stays in the
ignored build/s87/c7cap/<item>/ tree.  This file embeds no original code or
data; it names addresses only.  Save states are only read; the user's slots
01..15 are never written; the emulator runs hidden and is closed at the end
of every run.

Usage (decomp .venv python, repo root):
    .venv/bin/python tools/c7cap_partb.py fb          # live points + snapshot, then the survey
    .venv/bin/python tools/c7cap_partb.py fb-survey   # no emulator: the fb points' freeze analysis
                                                      # again, then the survey of every route snapshot
    .venv/bin/python tools/c7cap_partb.py h7 [--mode fields|writer|both]
    .venv/bin/python tools/c7cap_partb.py fb2 [--points a,b] [--steps 2]   # needs Renderer = 13 (manual)
    .venv/bin/python tools/c7cap_partb.py fb2-decode [--points a,b]        # no emulator
"""
from __future__ import annotations

import argparse
import json
import shutil
import struct
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import route_capture as rc  # noqa: E402
from pcsx2_session import FRAME_COUNTER, LOOP_TOP, VSYNC_COUNTER  # noqa: E402

OUT = ROOT / "build/s87/c7cap"
GS_PRIV = 0x12000000
GS_REGS = [("PMODE", 0x00), ("SMODE1", 0x10), ("SMODE2", 0x20), ("SRFSH", 0x30),
           ("SYNCH1", 0x40), ("SYNCH2", 0x50), ("SYNCV", 0x60), ("DISPFB1", 0x70),
           ("DISPLAY1", 0x80), ("DISPFB2", 0x90), ("DISPLAY2", 0xA0), ("EXTBUF", 0xB0),
           ("EXTDATA", 0xC0), ("EXTWRITE", 0xD0), ("BGCOLOR", 0xE0), ("CSR", 0x1000),
           ("IMR", 0x1010), ("BUSDIR", 0x1040), ("SIGLBLID", 0x1080)]
FREEZE_VRAM = 425                   # GS freeze v9: header bytes before local memory (tools/gs_vram.py)
LOCALMEM = 0x400000
CTX_REGS = ["XYOFFSET", "TEX0", "TEX1", "CLAMP", "MIPTBP1", "MIPTBP2", "SCISSOR", "ALPHA",
            "TEST", "FBA", "FRAME", "ZBUF"]


def no_emulator_left() -> bool:
    return subprocess.run(["pgrep", "-f", "PCSX2.app/Contents/MacOS/PCSX2"],
                          capture_output=True).returncode != 0


# ---------------------------------------------------------------------------
# fb

def gs_priv_pine(s) -> dict[str, int]:
    out = {}
    for name, off in GS_REGS:
        lo, hi = struct.unpack("<II", s.read(GS_PRIV + off, 8))
        out[name] = lo | (hi << 32)
    return out


def gs_priv_debug(s) -> dict[str, int]:
    out = {}
    for name, off in GS_REGS:
        r = s.debug.call({"cmd": "read_memory", "cpu": "ee", "address": GS_PRIV + off, "length": 8})
        out[name] = int.from_bytes(bytes.fromhex(r["hex"]), "little")
    return out


def dec_dispfb(v: int) -> dict:
    return {"FBP": v & 0x1FF, "byte_address": hex((v & 0x1FF) * 2048 * 4), "FBW_px": ((v >> 9) & 0x3F) * 64,
            "PSM": hex((v >> 15) & 0x1F), "DBX": (v >> 32) & 0x7FF, "DBY": (v >> 43) & 0x7FF}


def dec_display(v: int) -> dict:
    return {"DX": v & 0xFFF, "DY": (v >> 12) & 0x7FF, "MAGH": (v >> 23) & 0xF, "MAGV": (v >> 27) & 0x3,
            "DW": (v >> 32) & 0xFFF, "DH": (v >> 44) & 0x7FF}


def dec_frame(v: int) -> dict:
    return {"FBP": v & 0x1FF, "byte_address": hex((v & 0x1FF) * 2048 * 4), "FBW_px": ((v >> 16) & 0x3F) * 64,
            "PSM": hex((v >> 24) & 0x3F), "FBMSK": hex(v >> 32)}


def decode_priv(r: dict[str, int]) -> dict:
    pm, sm2, csr = r["PMODE"], r["SMODE2"], r["CSR"]
    return {"PMODE": {"EN1": pm & 1, "EN2": (pm >> 1) & 1, "MMOD": (pm >> 5) & 1, "AMOD": (pm >> 6) & 1,
                      "SLBG": (pm >> 7) & 1, "ALP": (pm >> 8) & 0xFF},
            "SMODE2": {"INT": sm2 & 1, "FFMD": (sm2 >> 1) & 1, "DPMS": (sm2 >> 2) & 3},
            "DISPFB1": dec_dispfb(r["DISPFB1"]), "DISPLAY1": dec_display(r["DISPLAY1"]),
            "DISPFB2": dec_dispfb(r["DISPFB2"]), "DISPLAY2": dec_display(r["DISPLAY2"]),
            "BGCOLOR": hex(r["BGCOLOR"] & 0xFFFFFF), "CSR_FIELD": (csr >> 13) & 1}


def freeze_analysis(gs: bytes, fbps: list[int]) -> dict:
    """Drawing-context registers of the freeze and a content summary of each
    512-wide PSMCT32 buffer page range named by `fbps` (2048-word units)."""
    assert len(gs) - LOCALMEM - FREEZE_VRAM == 84, "unexpected GS freeze layout"
    version = struct.unpack_from("<I", gs, 0)[0]
    ctx = []
    for c in (0, 1):
        base = 4 + 15 * 8 + c * 96
        regs = {n: struct.unpack_from("<Q", gs, base + i * 8)[0] for i, n in enumerate(CTX_REGS)}
        ctx.append({"FRAME": dec_frame(regs["FRAME"]), "ZBUF_raw": hex(regs["ZBUF"]),
                    "ZBP_byte_address": hex((regs["ZBUF"] & 0x1FF) * 2048 * 4),
                    "ZPSM": hex((regs["ZBUF"] >> 24) & 0xF), "SCISSOR_raw": hex(regs["SCISSOR"]),
                    "SCISSOR": {"x0": regs["SCISSOR"] & 0x7FF, "x1": (regs["SCISSOR"] >> 16) & 0x7FF,
                                "y0": (regs["SCISSOR"] >> 32) & 0x7FF, "y1": (regs["SCISSOR"] >> 48) & 0x7FF},
                    "XYOFFSET_raw": hex(regs["XYOFFSET"])})
    vram = gs[FREEZE_VRAM:FREEZE_VRAM + LOCALMEM]
    regions = {}
    for fbp in sorted(set(fbps) | {c["FRAME"]["FBP"] for c in ctx}):
        lo, n = fbp * 8192, 512 * 224 * 4
        seg = vram[lo:lo + n]
        words = {seg[i:i + 4] for i in range(0, len(seg), 4)}
        regions[hex(fbp)] = {"byte_range": [hex(lo), hex(lo + n)], "distinct_words": len(words),
                             "single_value": (next(iter(words)).hex() if len(words) == 1 else None)}
    return {"freeze_version": version, "contexts": ctx, "buffers_512x224": regions}


FB_POINTS = [
    ("first_control", "04"),
    ("route03_end", "03_panel_power"),
    ("route07_end", "07_truck_preview"),
]


def fb_run(frames: int = 8) -> dict:
    out_root = OUT / "fb"
    out_root.mkdir(parents=True, exist_ok=True)
    summary = {"item": "fb", "points": {}}
    for label, source in FB_POINTS:
        out = out_root / label
        out.mkdir(parents=True, exist_ok=True)
        src = rc.slot_path(source) if len(source) == 2 else rc.resumable(ROUTE / source / "state.p2s")
        rows = []
        try:
            with rc.open_session(src, log_dir=out / "logs") as s:
                for i in range(frames + 1):
                    if i:
                        s.step(1)
                    p = gs_priv_pine(s)
                    d = gs_priv_debug(s)
                    rows.append({"i": i, "counter": s.u32(FRAME_COUNTER), "vsync": s.u32(VSYNC_COUNTER),
                                 "pine": {k: hex(v) for k, v in p.items()},
                                 "debug_equal": p == d, "decoded": decode_priv(p)})
                snap = s.snapshot(out / "snapshot")
        finally:
            shutil.rmtree(rc.OUT / "_resume", ignore_errors=True)
        doc = {"label": label, "source": source, "rows": rows, "snapshot": snap}
        summary["points"][label] = {
            "counters": [rows[0]["counter"], rows[-1]["counter"]],
            "pine_equals_debugserver": all(r["debug_equal"] for r in rows),
            "dispfb1_fbp_seq": [hex(r["decoded"]["DISPFB1"]["FBP"]) for r in rows],
            "dispfb2_fbp_seq": [hex(r["decoded"]["DISPFB2"]["FBP"]) for r in rows],
            "pmode": rows[-1]["decoded"]["PMODE"], "smode2": rows[-1]["decoded"]["SMODE2"],
            "display1": rows[-1]["decoded"]["DISPLAY1"], "display2": rows[-1]["decoded"]["DISPLAY2"],
        }
        fb_point_freeze(out, doc, summary["points"][label])
        print(label, json.dumps(summary["points"][label])[:900], flush=True)
    (out_root / "meta.json").write_text(json.dumps(summary, indent=1) + "\n")
    return summary


LIVE_NOTE = ("rows[*].decoded and the live pmode/smode2/display*/dispfb*_fbp_seq fields decode EE reads "
             "of 0x12000000.., which return CSR for every register but SIGLBLID; they are not register "
             "values. The frozen_* fields and the freeze analysis use the save state's frozen register page.")


def fb_point_freeze(out: Path, doc: dict, point: dict) -> None:
    """Freeze analysis of one fb point's snapshot.  The displayed buffer is
    DISPFB2 from the FROZEN register page (the live EE reads mirror CSR, so a
    DISPFB decoded from them is garbage); the drawn buffers come from the
    freeze's FRAME registers.  Rewrites <point>/fb.json and fills `point`."""
    from parse_pcsx2_state import extract_zstd_entry
    state = out / "snapshot" / "state.p2s"
    regs = priv_from_state(state)
    dp = decode_priv(regs)
    gs = (out / "snapshot" / "gs.bin").read_bytes()
    if gs != extract_zstd_entry(state, "GS.bin"):
        raise RuntimeError(f"{out}/snapshot/gs.bin differs from the state's GS.bin")
    fa = freeze_analysis(gs, [dp["DISPFB2"]["FBP"]])
    fa["frozen_registers"] = {"raw": {k: hex(regs[k]) for k in
                                      ("PMODE", "SMODE2", "DISPFB2", "DISPLAY2", "CSR")}, "decoded": dp}
    doc["freeze"] = fa
    doc["note"] = LIVE_NOTE
    (out / "fb.json").write_text(json.dumps(doc, indent=1) + "\n")
    point.update({
        "note": LIVE_NOTE,
        "frozen_csr_field": dp["CSR_FIELD"],
        "frozen_dispfb2_fbp": hex(dp["DISPFB2"]["FBP"]),
        "freeze_frame": [c["FRAME"] for c in fa["contexts"]],
        "freeze_scissor": [c["SCISSOR"] for c in fa["contexts"]],
        "freeze_buffers": fa["buffers_512x224"],
    })


def fb_refresh_points() -> dict:
    """No emulator: recompute the freeze analysis of the recorded fb points
    (fb.json `freeze`, meta.json `freeze_*`) from their snapshots.  The live
    rows are kept as recorded."""
    meta_path = OUT / "fb" / "meta.json"
    if not meta_path.exists():
        return {}
    summary = json.loads(meta_path.read_text())
    for label, _ in FB_POINTS:
        out = OUT / "fb" / label
        if not (out / "fb.json").exists() or label not in summary["points"]:
            continue
        doc = json.loads((out / "fb.json").read_text())
        point = summary["points"][label]
        for k in ("freeze_frame", "freeze_scissor", "freeze_buffers"):
            point.pop(k, None)
        fb_point_freeze(out, doc, point)
        print(label, "field", point["frozen_csr_field"], "shown", point["frozen_dispfb2_fbp"],
              "buffers", sorted(point["freeze_buffers"]), flush=True)
    meta_path.write_text(json.dumps(summary, indent=1) + "\n")
    return summary


ROUTE = ROOT / "build/s87/route"
INTERNALS = "PCSX2 Internal Structures.dat"
GS_PRIV_AFTER_TAG = 1246    # PS2MEM_GS copy: bytes after the "EE-Subsystems" freeze tag (rcnt + mem freeze)


def priv_from_state(state: Path) -> dict[str, int]:
    """The GS privileged registers as the save state froze them (PCSX2's
    gsFreeze copies the 0x2000-byte register page into the internal
    structures right after the EE counters and memory freeze).  Live EE
    reads of 0x12000000.. return CSR for every register but SIGLBLID
    (PCSX2 gsRead8/16/32/64 mirror CSR, as the hardware does)."""
    from parse_pcsx2_state import extract_zstd_entry
    d = extract_zstd_entry(state, INTERNALS)
    base = d.index(b"EE-Subsystems") + GS_PRIV_AFTER_TAG
    regs = {n: struct.unpack_from("<Q", d, base + off)[0] for n, off in GS_REGS}
    if regs["CSR"] >> 16 & 0xFFFF != 0x551B:
        raise RuntimeError(f"GS register page not found in {state}")
    return regs


def fb_survey() -> dict:
    """Privileged registers and drawing contexts of every AREA11 route
    snapshot and of the fb points (no emulator)."""
    from parse_pcsx2_state import extract_zstd_entry
    states = sorted(ROUTE.glob("*/state.p2s")) + [OUT / "fb" / lab / "snapshot/state.p2s" for lab, _ in FB_POINTS]
    rows = []
    for st in states:
        if not st.exists():
            continue
        regs = priv_from_state(st)
        gs = extract_zstd_entry(st, "GS.bin")
        dp = decode_priv(regs)
        fa = freeze_analysis(gs, [dp["DISPFB2"]["FBP"]])
        c0 = int(fa["contexts"][0]["XYOFFSET_raw"], 16)
        c1 = int(fa["contexts"][1]["XYOFFSET_raw"], 16)
        rows.append({"state": str(st.relative_to(ROOT)), "raw": {k: hex(regs[k]) for k in
                     ("PMODE", "SMODE1", "SMODE2", "DISPFB1", "DISPLAY1", "DISPFB2", "DISPLAY2", "BGCOLOR", "CSR")},
                     "decoded": dp, "frame": [c["FRAME"] for c in fa["contexts"]],
                     "zbuf": [c["ZBP_byte_address"] + "/" + c["ZPSM"] for c in fa["contexts"]],
                     "scissor": [c["SCISSOR"] for c in fa["contexts"]],
                     "ofx_ofy": [[(c0 & 0xFFFF) / 16, (c0 >> 32 & 0xFFFF) / 16],
                                 [(c1 & 0xFFFF) / 16, (c1 >> 32 & 0xFFFF) / 16]],
                     "buffers": fa["buffers_512x224"]})
        r = rows[-1]
        print(st.parent.name if st.parent.name != "snapshot" else st.parent.parent.name,
              "field", dp["CSR_FIELD"], "shown", hex(dp["DISPFB2"]["FBP"]), "drawn",
              [hex(f["FBP"]) for f in r["frame"]], "ofy", [o[1] for o in r["ofx_ofy"]],
              "buffers", {k: v["single_value"] or v["distinct_words"] for k, v in r["buffers"].items()}, flush=True)
    (OUT / "fb" / "survey.json").write_text(json.dumps(rows, indent=1) + "\n")
    return {"states": len(rows)}

# ---------------------------------------------------------------------------
# h7

H7_SPANS = [
    ("counter", FRAME_COUNTER, 4), ("vsync", VSYNC_COUNTER, 4),
    ("slot2", 0x28A790, 0x20),          # loader slot 2 record (+9 step, +0xB sub-step, +0xE module, +0x16 chunk)
    ("gate", 0x282154, 8),              # D_00282154..5B; [3] = D_00282157, the loader's read gate
    ("bd8", 0x275BD8, 4),               # D_00275BD8, the "module load pending" byte the ITEM root waits on
    ("ui", 0x810130, 0x10),             # status UI object
    ("req", 0x8106B0, 0x10),            # D_008106B0.. (B0/B1 page request)
    ("spad", 0x70003B8C, 8),
]
H7_BEAT = "03_panel_power"
H7_FIELD_WINDOW = (386, 418)            # recorded f: the load request is f391, BD8 clears in f414
H7_WRITER_WINDOW = (385, 392)           # BD8 rises in f390 (STATUS_LOAD_WAIT_PROBE.md)


class H7Sampler:
    def __init__(self, s, c7):
        self.s, self.c7 = s, c7
        self.body = b"".join(struct.pack("<BI", 2, a + i) for _n, a, n in H7_SPANS for i in range(0, n, 4))
        self.stream = c7.StreamSampler(s)

    def record(self, phase: str, f: int, iop: bool) -> dict:
        data = self.s.pine.request(self.body)
        r, off = {}, 0
        for name, _a, n in H7_SPANS:
            r[name] = data[off:off + n]
            off += n
        sl = r["slot2"]
        rec = {"f": f, "phase": phase, "counter": struct.unpack_from("<I", r["counter"])[0],
               "vsync": struct.unpack_from("<I", r["vsync"])[0],
               "slot2": sl.hex(), "s_state": sl[0], "s8": sl[8], "s9_step": sl[9], "sA": sl[0xA],
               "sB_sub": sl[0xB], "sE_module": struct.unpack_from("<H", sl, 0xE)[0],
               "s14_count": struct.unpack_from("<H", sl, 0x14)[0], "s16_chunk": struct.unpack_from("<H", sl, 0x16)[0],
               "gate_282157": r["gate"][3], "gate": r["gate"].hex(), "bd8": r["bd8"][0],
               "ui": r["ui"].hex(), "req": r["req"][:10].hex(), "spad": r["spad"].hex()}
        if iop:
            rec["iop"] = self.stream.iop()
        return rec


def h7_run(mode: str) -> dict:
    import c7cap_capture as c7
    out = OUT / "h7" / mode
    out.mkdir(parents=True, exist_ok=True)
    cl = c7.ClosedLoop(H7_BEAT, None, out / "logs")
    frames, fields, writes = [], [], []
    st: dict = {"armed": False}
    cur = {"f": 1}
    t0 = time.monotonic()
    w0, w1 = H7_FIELD_WINDOW if mode == "fields" else H7_WRITER_WINDOW

    def arm_writer(s, on: bool) -> None:
        if on:
            s.debug.call({"cmd": "set_memcheck", "address": 0x275BD8, "end": 0x275BD9, "type": "write",
                          "description": "c7cap h7 BD8"})
        else:
            s.debug.call({"cmd": "remove_memcheck", "address": 0x275BD8, "end": 0x275BD9})
        st["armed"] = on

    def on_memcheck(pc: int) -> None:
        s = cl.s
        try:
            bt = s.debug.call({"cmd": "get_backtrace", "max_frames": 8})["frames"]
            bt = [{"entry": x["entry"], "pc": x["pc"]} for x in bt]      # no disassembly kept
        except Exception as exc:
            bt = [repr(exc)]
        writes.append({"f": cur["f"], "counter": s.u32(FRAME_COUNTER), "vsync": s.u32(VSYNC_COUNTER),
                       "pause_pc": hex(pc), "ra": hex(s.reg("ra")),
                       "bd8_now": s.read(0x275BD8, 4)[0], "backtrace": bt})
        if len(writes) > 200:
            arm_writer(s, False)

    def hook(row: dict) -> None:
        s = cl.s
        if "sampler" not in st:
            st["sampler"] = H7Sampler(s, c7)
            s.on_other = on_memcheck
        sm = st["sampler"]
        f = row["f"]
        in_win = w0 <= f <= w1
        rec = sm.record("top", f, iop=in_win)
        rec["ui_row"], rec["m1F0"], rec["clip"] = row["ui"], row["m1F0"], row["clip"]
        frames.append(rec)
        cur["f"] = f + 1
        if mode == "fields":
            if f + 1 == w0:
                s.arm(c7.ISR, lambda pc: fields.append(sm.record("isr", cur["f"], iop=True)))
            if f + 1 == w1 + 1:
                s.disarm(c7.ISR)
        else:
            if f + 1 == w0 and not st["armed"]:
                arm_writer(s, True)
            if f >= w1:
                if st["armed"]:
                    arm_writer(s, False)
                raise c7.StopBeat

    try:
        cl.run(hook)
    finally:
        if getattr(cl, "s", None) is not None:
            try:
                if st.get("armed"):
                    arm_writer(cl.s, False)
            except Exception:
                pass
            cl.s.close()
    c7.write_jsonl(out / "frames.jsonl", frames)
    c7.write_jsonl(out / "fields.jsonl", fields)
    c7.write_jsonl(out / "writes.jsonl", writes)
    # the load sequence at frame tops: (f, +9, +0xB, +0x16, D_00282157, BD8) changes
    seq, prev = [], None
    for r in frames:
        k = (r["s_state"], r["s9_step"], r["sB_sub"], r["s16_chunk"], r["gate_282157"], r["bd8"], r["ui"][4:20])
        if k != prev:
            seq.append({"f": r["f"], "counter": r["counter"], "state": r["s_state"], "step": r["s9_step"],
                        "sub": r["sB_sub"], "chunk": r["s16_chunk"], "gate": r["gate_282157"], "bd8": r["bd8"],
                        "ui": r["ui"][4:20]})
            prev = k
    meta = {"item": "h7", "mode": mode, "beat": H7_BEAT, "window_f": [w0, w1], "run": cl.summary(),
            "seconds": round(time.monotonic() - t0, 1), "changes": seq, "writes": len(writes),
            "kicks": cl.s.kicks, "unexpected": cl.s.unexpected}
    (out / "meta.json").write_text(json.dumps(meta, indent=1) + "\n")
    print("h7", mode, json.dumps(meta["run"]), flush=True)
    return meta


# ---------------------------------------------------------------------------
# fb2: framebuffers with PCSX2's software renderer (docs/CAPTURES_C7.md 5b)
#
# The renderer switch is a MANUAL step (user decision 2026-09-26: the software
# renderer is used only during capture jobs).  This tool never edits the
# emulator's ini; `fb2` refuses to start unless the live ini says Renderer = 13
# and always ends by printing how to restore Renderer = 17.

REFERENCE_DIR = ROOT / "build/startup-reference"
LIVE_INI = REFERENCE_DIR / "inis/PCSX2.ini"       # the ini the -portable launch reads (EnablePINE etc.)
RENDERER_SW, RENDERER_METAL = 13, 17
FB2 = OUT / "fb2"
FREE_SLOT_MIN = 40                                  # temporary snapshot slots; 01..15 are the user's
VSYNC_WAIT = 0x1AAFF0      # first instruction of the main loop's vsync wait (FINDINGS "ENGINE FRAME ANATOMY" P)
VSYNC_ISR = 0x1AB140       # vsync-start interrupt handler entry

# The 16 route snapshots plus the three fb points of section 5.  route03_end /
# route07_end load the same states as routes 03 / 07 (a repeatability check).
FB2_POINTS = ([(p.name, p.name) for p in sorted(ROUTE.glob("[01][0-9]_*")) if (p / "state.p2s").exists()]
              + [("first_control", "04"), ("route03_end", "03_panel_power"), ("route07_end", "07_truck_preview")])

# GS local-memory layout facts (public GS documentation; PCSX2 GSTables.cpp
# blockTable32 / blockTable32Z / columnTable32 carry the same tables):
#  * page = 8 KiB = 2048 words; a PSMCT32/PSMZ32 page is 64 x 32 pixels; a
#    buffer FBW*64 pixels wide has FBW pages per page row; FBP/ZBP count pages.
#  * a page holds 32 blocks of 8 x 8 pixels (64 words), 8 across x 4 down,
#    numbered in the interleaved order PAGE32 below (PSMZ32/Z24 use the same
#    order XOR 24, i.e. the two page halves and the two block rows swapped).
#  * a block holds 4 columns of 8 x 2 pixels (16 words each); inside a column
#    row 0 holds words 0 1 4 5 8 9 12 13 and row 1 holds 2 3 6 7 10 11 14 15.
PAGE32 = [0, 1, 4, 5, 16, 17, 20, 21, 2, 3, 6, 7, 18, 19, 22, 23,
          8, 9, 12, 13, 24, 25, 28, 29, 10, 11, 14, 15, 26, 27, 30, 31]
COL32 = [0, 1, 4, 5, 8, 9, 12, 13, 2, 3, 6, 7, 10, 11, 14, 15]
FB_W, FB_H = 512, 224
FILL = 0x80000000


def ini_renderer(path: Path = LIVE_INI) -> int | None:
    section = None
    for line in path.read_text().splitlines():
        s = line.strip()
        if s.startswith("["):
            section = s
        elif section == "[EmuCore/GS]" and s.split("=")[0].strip() == "Renderer":
            return int(s.split("=", 1)[1])
    return None


RESTORE_HINT = (f"RESTORE: in {LIVE_INI}, section [EmuCore/GS], set the line 'Renderer = 13' back to "
                f"'Renderer = 17' (Metal) after PCSX2 has exited, then diff the ini against "
                f"build/s87/c7cap/fb2/pre/inis_PCSX2.ini (only MainWindowGeometry-type keys may differ).")


def require_software_renderer() -> None:
    r = ini_renderer()
    if r != RENDERER_SW:
        raise SystemExit(
            f"fb2: {LIVE_INI} [EmuCore/GS] Renderer = {r}, not {RENDERER_SW} (software).  The switch is a "
            f"manual step: record sha256 + a copy of the ini, playtime.dat and the memcards into "
            f"build/s87/c7cap/fb2/pre/, change ONLY 'Renderer = 17' to 'Renderer = 13', run fb2, then "
            f"restore.  " + RESTORE_HINT)


def gs_word_map(w: int = FB_W, h: int = FB_H, fbw_px: int = FB_W, z: bool = False):
    """numpy (h, w) array: the local-memory word index (relative to the buffer
    base) of each pixel of a PSMCT32 (z=False) or PSMZ32/PSMZ24 (z=True) buffer."""
    import numpy as np
    ppr = fbw_px // 64
    y, x = np.mgrid[0:h, 0:w]
    page = (y // 32) * ppr + x // 64
    blk = np.array(PAGE32)[(y % 32 // 8) * 8 + (x % 64 // 8)]
    if z:
        blk = blk ^ 24
    px = (y % 8) * 8 + x % 8
    return page * 2048 + blk * 64 + (px // 16) * 16 + np.array(COL32)[px % 16]


def gs_buffer(vram: bytes, base_page: int, z: bool = False, linear: bool = False):
    """(224, 512) uint32 pixels of the buffer at page `base_page`.  linear=True
    reads the words in raster order (a deliberately wrong decode for the proof)."""
    import numpy as np
    words = np.frombuffer(vram, dtype="<u4")
    base = base_page * 2048
    if linear:
        return words[base:base + FB_W * FB_H].reshape(FB_H, FB_W).copy()
    return words[base + gs_word_map(z=z)]


def rgba_bytes(px, opaque: bool = True) -> bytes:
    import numpy as np
    b = px.astype("<u4").view(np.uint8).reshape(px.shape[0], px.shape[1], 4).copy()
    if opaque:
        b[..., 3] = 255
    return b.tobytes()


def read_png_rgba(path: Path):
    """Minimal PNG reader (8-bit RGB/RGBA, non-interlaced): the save state's
    host screenshot."""
    import zlib
    import numpy as np
    d = path.read_bytes()
    pos, idat, w, h, bpp = 8, b"", 0, 0, 4
    while pos < len(d):
        n, typ = struct.unpack(">I4s", d[pos:pos + 8])
        body = d[pos + 8:pos + 8 + n]
        if typ == b"IHDR":
            w, h, depth, ctype = struct.unpack(">IIBB", body[:10])
            if depth != 8 or ctype not in (2, 6):
                raise ValueError(f"unsupported PNG {depth}/{ctype}")
            bpp = 4 if ctype == 6 else 3
        elif typ == b"IDAT":
            idat += body
        pos += 12 + n
    raw = zlib.decompress(idat)
    stride = w * bpp
    out = np.zeros((h, stride), dtype=np.uint8)
    prev = np.zeros(stride, dtype=np.int32)
    for yy in range(h):
        f, line = raw[yy * (stride + 1)], np.frombuffer(raw, np.uint8, stride, yy * (stride + 1) + 1).astype(np.int32)
        if f == 0:
            cur = line
        elif f == 2:
            cur = (line + prev) & 255
        else:
            cur = np.zeros(stride, dtype=np.int32)
            for i in range(stride):
                a = cur[i - bpp] if i >= bpp else 0
                b = prev[i]
                c = prev[i - bpp] if i >= bpp else 0
                if f == 1:
                    p = a
                elif f == 3:
                    p = (a + b) >> 1
                else:
                    pa, pb, pc = abs(b - c), abs(a - c), abs(a + b - 2 * c)
                    p = a if pa <= pb and pa <= pc else (b if pb <= pc else c)
                cur[i] = (line[i] + p) & 255
        out[yy] = cur
        prev = cur
    return out.reshape(h, w, bpp)[..., :3]


def coarse_gray(rgb, gw: int = 64, gh: int = 48):
    """Box-average an (H, W, 3) image to a gh x gw luma grid."""
    import numpy as np
    img = rgb.astype(np.float64)
    luma = img[..., 0] * 0.299 + img[..., 1] * 0.587 + img[..., 2] * 0.114
    H, W = luma.shape
    ys = (np.arange(gh + 1) * H) // gh
    xs = (np.arange(gw + 1) * W) // gw
    return np.array([[luma[ys[j]:ys[j + 1], xs[i]:xs[i + 1]].mean() for i in range(gw)] for j in range(gh)])


def corr(a, b) -> float:
    import numpy as np
    a, b = a.ravel() - a.mean(), b.ravel() - b.mean()
    den = float(np.sqrt((a * a).sum() * (b * b).sum()))
    return round(float((a * b).sum()) / den, 4) if den else 0.0


def block_seam_ratio(px) -> float:
    """Mean |horizontal neighbour difference| across 8-pixel block seams divided
    by the mean inside blocks (about 1 for a correctly decoded image; a wrong
    block/column order raises it)."""
    import numpy as np
    rgb = px.view(np.uint8).reshape(px.shape[0], px.shape[1], 4)[..., :3].astype(np.int32)
    d = np.abs(np.diff(rgb, axis=1)).sum(axis=2)          # d[:, x] = |p[x+1] - p[x]|
    seam = d[:, 7::8].mean()
    inner = np.delete(d, np.s_[7::8], axis=1).mean()
    return round(float(seam / inner), 3) if inner else 0.0


def host_mae(shot, bufs: dict) -> dict:
    """Mean absolute RGB error between the host screenshot (resampled to
    512x224 by box average over its display rows) and each buffer: names the
    buffer PCSX2 presented when consecutive frames look alike."""
    import numpy as np
    H, W, _ = shot.shape
    ys = (np.arange(FB_H + 1) * H) // FB_H
    xs = (np.arange(FB_W + 1) * W) // FB_W
    img = shot.astype(np.float64)
    cs = np.add.reduceat(np.add.reduceat(img, ys[:-1], axis=0), xs[:-1], axis=1)
    cnt = np.outer(np.diff(ys), np.diff(xs))[..., None]
    small = cs / cnt
    return {b: round(float(np.abs(small - a.view(np.uint8).reshape(FB_H, FB_W, 4)[..., :3]).mean()), 3)
            for b, a in bufs.items() if b != "z"}


def flat_row_check(px, shot, name: str) -> dict:
    """Known flat-colour region: the longest run (>= 4) of buffer rows whose
    512 pixels are all one value (letterbox bars, fades), its RGB, and the
    mean RGB of the host screenshot over the same rows (row y of the 224-line
    field covers host rows y*H/224 .. (y+1)*H/224; the outer 8 host columns
    are skipped)."""
    import numpy as np
    uni = (px == px[:, :1]).all(axis=1)
    best, cur, start = (0, 0), 0, 0
    for y in range(FB_H):
        if uni[y]:
            if cur == 0:
                start = y
            cur += 1
            if cur > best[1] - best[0]:
                best = (start, start + cur)
        else:
            cur = 0
    if best[1] - best[0] < 4:
        return {"buffer": name, "run": None}
    y0, y1 = best
    val = int(px[y0, 0])
    H = shot.shape[0]
    band = shot[(y0 * H) // FB_H:(y1 * H) // FB_H, 8:-8].reshape(-1, 3).astype(np.float64)
    rgb = [val & 0xFF, val >> 8 & 0xFF, val >> 16 & 0xFF]
    return {"buffer": name, "rows": [y0, y1 - 1], "word": hex(val), "rgb": rgb,
            "host_rows": [(y0 * H) // FB_H, (y1 * H) // FB_H - 1],
            "host_mean_rgb": [round(float(v), 2) for v in band.mean(axis=0)],
            "host_max_abs_diff": round(float(np.abs(band - rgb).max()), 1)}


def free_slot() -> int:
    used = {int(p.name.split(".")[-2]) for p in rc.SSTATES.glob(f"{rc.SERIAL}.*.p2s")
            if p.name.split(".")[-2].isdigit()}
    slot = next(s for s in range(FREE_SLOT_MIN, 64) if s not in used)
    assert slot >= FREE_SLOT_MIN
    return slot


HW_SPANS = [("D1_CHCR", 0x10009000), ("D1_MADR", 0x10009010), ("D1_TADR", 0x10009030),
            ("D2_CHCR", 0x1000A000), ("D2_MADR", 0x1000A010), ("GIF_STAT", 0x10003020),
            ("frame_idx_810E80", 0x810E80)]       # halfword toggled at the loop's end (W)


def fb2_source(src: str) -> Path:
    return rc.slot_path(src) if len(src) == 2 else rc.resumable(ROUTE / src / "state.p2s")


def _pc(s) -> int:
    return int(s.debug.call({"cmd": "evaluate", "expression": "pc"})["result"]) & 0xFFFFFFFF


def sync_snapshot(s, folder: Path) -> dict:
    """Save-state snapshot used as a synchronisation point (PCSX2's freeze
    waits for the VU1 thread and the GS thread).  Keeps the GS freeze, the host
    screenshot and the frozen privileged register page; drops the rest."""
    snap = s.snapshot(folder, slot=free_slot())
    regs = priv_from_state(folder / "state.p2s")
    (folder / "priv.json").write_text(json.dumps({k: hex(v) for k, v in regs.items()}, indent=1) + "\n")
    for name in ("eeMemory.bin", "scratchpad.bin", "state.p2s"):
        (folder / name).unlink()
    return snap


def fb2_step(s, out: Path, k: int) -> list[dict]:
    """One main-loop frame (counter c -> c+1) with two synchronisation
    snapshots inside it: at the first instruction of the vsync wait
    (s<k>a_wait: game logic done, VIF1 DMA idle) and at the vsync ISR entry
    (s<k>b_isr: the field change).  Why: this PCSX2 runs VU1 on its own host
    thread (ini vuThread = true), so when the EE runs free between loop tops
    the last GS writes of a frame can land after later EE points (host timing;
    seen in the free-run test as a 'late tail' in the other buffer).  The two
    syncs make every frame's writes land before its vsync, as on hardware.
    The EE side is unaffected (the session rows still equal the recorded rows)."""
    before = s.u32(FRAME_COUNTER)
    marks = []
    armed = {VSYNC_WAIT: f"s{k}a_wait", VSYNC_ISR: f"s{k}b_isr"}
    for addr in armed:
        s.debug.call({"cmd": "set_breakpoint", "address": addr, "description": "fb2 sync"})
    try:
        while True:
            s._resume_to_boundary()
            pc = _pc(s)
            if pc == LOOP_TOP:
                break
            if pc not in armed or s.u32(FRAME_COUNTER) != before:
                raise RuntimeError(f"fb2_step: paused at {pc:#x} counter {s.u32(FRAME_COUNTER)} (from {before})")
            name = armed.pop(pc)
            s.debug.call({"cmd": "remove_breakpoint", "address": pc})
            marks.append({"at": name, "pc": hex(pc), "counter": before, "vsync": s.u32(VSYNC_COUNTER),
                          "hw": {n: hex(s.u32(a)) for n, a in HW_SPANS}, "snapshot": sync_snapshot(s, out / name)})
    finally:
        for addr in armed:
            s.debug.call({"cmd": "remove_breakpoint", "address": addr})
    after = s.u32(FRAME_COUNTER)
    if after != before + 1:
        raise RuntimeError(f"fb2_step: {before} -> {after}")
    s.frames_stepped += 1
    return marks


def fb2_capture(labels: list[str] | None, steps: int = 2) -> None:
    """Emulator part: per point, load the state (the session aligns to the
    next main-loop top), snapshot (s0), then step one frame at a time with a
    neutral pad (fb2_step) and snapshot after each (s1..s<steps>)."""
    require_software_renderer()
    FB2.mkdir(parents=True, exist_ok=True)
    points = [p for p in FB2_POINTS if not labels or p[0] in labels]
    for label, src in points:
        out = FB2 / label
        if out.exists():
            shutil.rmtree(out)
        out.mkdir(parents=True)
        t0 = time.monotonic()
        rows = []
        try:
            with rc.open_session(fb2_source(src), log_dir=out / "logs") as s:
                smp = rc.Sampler(s)
                for k in range(steps + 1):
                    marks = fb2_step(s, out, k) if k else []
                    row = rc.decode(smp.raw())
                    row["vsync"] = s.u32(VSYNC_COUNTER)
                    row["hw"] = {n: hex(s.u32(a)) for n, a in HW_SPANS}
                    row["k"] = k
                    row["inside_step"] = marks
                    if k < steps:       # earlier loop tops: GS freeze, screenshot, register page only
                        snap = sync_snapshot(s, out / f"s{k}")
                    else:               # the output point keeps its full save state (minus eeMemory.bin,
                        snap = s.snapshot(out / f"s{k}", slot=free_slot())    # which it contains)
                        (out / f"s{k}" / "eeMemory.bin").unlink()
                    row["snapshot"] = snap
                    rows.append(row)
                source_state = str(s.state)
        finally:
            shutil.rmtree(rc.OUT / "_resume", ignore_errors=True)
        (out / "capture.json").write_text(json.dumps(
            {"label": label, "source": src, "source_state": source_state, "renderer": ini_renderer(),
             "seconds": round(time.monotonic() - t0, 1), "rows": rows}, indent=1) + "\n")
        print(label, "counters", [r["counter"] for r in rows], "vsync", [r["vsync"] for r in rows],
              round(time.monotonic() - t0, 1), "s", flush=True)


def recorded_context(label: str, src: str) -> dict:
    """The recorded snapshot this point loads and the next recorded rows (the
    beats that start from it)."""
    ctx: dict = {}
    if len(src) == 2:
        ctx["recorded_snapshot"] = {"state": f"user slot {src}", "counter": None}
        name = src
    else:
        snap = json.loads((ROUTE / src / "snapshot.json").read_text())
        ctx["recorded_snapshot"] = {"state": f"build/s87/route/{src}/state.p2s",
                                    "counter": snap["main_loop_counter"], "vsync": snap["vsync_counter"]}
        name = src
    nxt = []
    for tr in sorted(ROUTE.glob("*/trace.json")):
        t = json.loads(tr.read_text())
        if t.get("source") == name:
            nxt.append({"beat": t["beat"], "first_counter": t["first_counter"], "trace": str(tr.relative_to(ROOT))})
    ctx["next_beats"] = nxt
    return ctx


ROW_KEYS = ["pos", "yaw", "spad", "m1F0", "cam_eye", "eye", "tgt", "screen", "fade", "msg", "ui", "req"]


def fb2_decode(label: str) -> dict:
    """No emulator: decode one captured point into displayed/draw/z buffers,
    registers.json and meta.json."""
    import numpy as np
    from parse_pcsx2_state import extract_zstd_entry
    sys.path.insert(0, str(ROOT / "tools"))
    from clut import write_png_rgba
    out = FB2 / label
    cap = json.loads((out / "capture.json").read_text())
    src = cap["source"]
    ctx = recorded_context(label, src)
    # the loaded source state's buffers (hardware-renderer fill)
    src_state = Path(cap["source_state"])
    if not src_state.exists():
        src_state = fb2_source(src)
    src_gs = extract_zstd_entry(src_state, "GS.bin")
    shutil.rmtree(rc.OUT / "_resume", ignore_errors=True)
    bufs = {"0x0": 0x0, "0x38": 0x38}
    ZBP = 0x70

    def vram_of(gs: bytes) -> bytes:
        assert len(gs) - LOCALMEM - FREEZE_VRAM == 84, "unexpected GS freeze layout"
        return gs[FREEZE_VRAM:FREEZE_VRAM + LOCALMEM]

    def digest(a) -> str:
        import hashlib
        return hashlib.sha256(a.tobytes()).hexdigest()[:16]

    prev_arr = {k: gs_buffer(vram_of(src_gs), p) for k, p in bufs.items()}
    prev_arr["z"] = gs_buffer(vram_of(src_gs), ZBP, z=True)
    src_fill = {k: bool((prev_arr[k] == FILL).all()) for k in bufs}
    snaps = []
    written_in = {k: None for k in list(bufs) + ["z"]}
    # stages in EE order: s0 (aligned loop top), then per step k the vsync-wait
    # start, the vsync ISR entry and the next loop top
    stages = []
    for row in cap["rows"]:
        for m in row.get("inside_step", []):
            stages.append((m["at"], row["k"], m["counter"], m["vsync"], m["hw"]))
        stages.append((f"s{row['k']}", row["k"], row["counter"], row["vsync"], row["hw"]))
    for at, k, counter, vsync, hw in stages:
        sd = out / at
        if (sd / "state.p2s").exists():
            regs = priv_from_state(sd / "state.p2s")
        else:
            regs = {n: int(v, 16) for n, v in json.loads((sd / "priv.json").read_text()).items()}
        dp = decode_priv(regs)
        gs = (sd / "gs.bin").read_bytes()
        fa = freeze_analysis(gs, [0x0, 0x38])
        vram = vram_of(gs)
        stats = {}
        cur_arr = {}
        for name, p in list(bufs.items()) + [("z", ZBP)]:
            arr = gs_buffer(vram, p, z=(name == "z"))
            cur_arr[name] = arr
            chg = prev_arr[name] != arr
            stats[name] = {"sha256_16": digest(arr), "distinct_words": int(len(np.unique(arr))),
                           "fill_0x80000000_words": int((arr == FILL).sum()),
                           "rows_changed_since_previous_stage": int(chg.any(axis=1).sum()),
                           "pixels_changed_since_previous_stage": int(chg.sum())}
            if chg.any():
                written_in[name] = at
        prev_arr = cur_arr
        ent = {"at": at, "k": k, "counter": counter, "vsync": vsync, "hw": hw,
               "csr_field": dp["CSR_FIELD"], "dispfb2_fbp": hex(dp["DISPFB2"]["FBP"]),
               "frame_fbp": [hex(c["FRAME"]["FBP"]) for c in fa["contexts"]],
               "ofy": [(int(c["XYOFFSET_raw"], 16) >> 32 & 0xFFFF) / 16 for c in fa["contexts"]],
               "buffers": stats, "last_written_at": dict(written_in)}
        if (sd / "original.png").exists():
            ent["host_screenshot_mae"] = host_mae(read_png_rgba(sd / "original.png"), cur_arr)
        snaps.append(ent)
    last = cap["rows"][-1]
    k = last["k"]
    sd = out / f"s{k}"
    regs = priv_from_state(sd / "state.p2s")
    dp = decode_priv(regs)
    gs = (sd / "gs.bin").read_bytes()
    fa = freeze_analysis(gs, [0x0, 0x38])
    vram = vram_of(gs)
    shown = dp["DISPFB2"]["FBP"]
    drawn = fa["contexts"][0]["FRAME"]["FBP"]
    assert fa["contexts"][1]["FRAME"]["FBP"] == drawn
    disp = gs_buffer(vram, shown)
    draw = gs_buffer(vram, drawn)
    zb = gs_buffer(vram, ZBP, z=True)
    (out / "displayed.bin").write_bytes(disp.astype("<u4").tobytes())
    (out / "draw.bin").write_bytes(draw.astype("<u4").tobytes())
    (out / "z.bin").write_bytes(zb.astype("<u4").tobytes())
    write_png_rgba(out / "displayed.png", FB_W, FB_H, rgba_bytes(disp))
    write_png_rgba(out / "draw.png", FB_W, FB_H, rgba_bytes(draw))
    prev448 = np.repeat(disp, 2, axis=0)
    write_png_rgba(out / "PREVIEW_ONLY_displayed_448_line_doubled.png", FB_W, 2 * FB_H, rgba_bytes(prev448))
    # proof of the decode: coarse agreement with the save state's host screenshot
    proof = {}
    shot = sd / "original.png"
    if shot.exists():
        host = coarse_gray(read_png_rgba(shot))
        rgb = lambda a: a.view(np.uint8).reshape(a.shape[0], a.shape[1], 4)[..., :3]
        proof = {
            "host_screenshot": str(shot.relative_to(ROOT)), "grid": "64x48 luma, box average",
            "corr_displayed_swizzled": corr(coarse_gray(rgb(disp)), host),
            "corr_draw_swizzled": corr(coarse_gray(rgb(draw)), host),
            "corr_displayed_linear_wrong": corr(coarse_gray(rgb(gs_buffer(vram, shown, linear=True))), host),
            "block_seam_ratio_swizzled": block_seam_ratio(disp),
            "block_seam_ratio_linear_wrong": block_seam_ratio(gs_buffer(vram, shown, linear=True)),
        }
    if shot.exists():
        proof["flat_rows"] = flat_row_check(disp, read_png_rgba(shot), "displayed")
        proof["flat_rows_draw"] = flat_row_check(draw, read_png_rgba(shot), "draw")
    alpha = disp >> 24
    fr = [c["FRAME"] for c in fa["contexts"]]
    registers = {
        "source": f"frozen GS register page and GS freeze of s{k}/state.p2s",
        "raw": {n: hex(regs[n]) for n in ("PMODE", "SMODE1", "SMODE2", "DISPFB1", "DISPLAY1", "DISPFB2",
                                          "DISPLAY2", "BGCOLOR", "CSR")},
        "PMODE": dp["PMODE"], "SMODE2": dp["SMODE2"], "DISPLAY2": dp["DISPLAY2"], "DISPFB2": dp["DISPFB2"],
        "CSR_FIELD": dp["CSR_FIELD"],
        "contexts": [{"FRAME": c["FRAME"], "ZBUF_raw": c["ZBUF_raw"], "ZBP_byte_address": c["ZBP_byte_address"],
                      "ZPSM": c["ZPSM"], "SCISSOR": c["SCISSOR"], "SCISSOR_raw": c["SCISSOR_raw"],
                      "XYOFFSET_raw": c["XYOFFSET_raw"],
                      "OFX_OFY": [(int(c["XYOFFSET_raw"], 16) & 0xFFFF) / 16,
                                  (int(c["XYOFFSET_raw"], 16) >> 32 & 0xFFFF) / 16]} for c in fa["contexts"]],
    }
    (out / "registers.json").write_text(json.dumps(registers, indent=1) + "\n")
    # frame alignment against the recorded rows
    rec_rows = {}
    for nb in ctx["next_beats"]:
        t = json.loads((ROOT / nb["trace"]).read_text())
        for r in t["rows"]:
            rec_rows.setdefault(r["counter"], (nb["beat"], r))
    align = []
    for row in cap["rows"]:
        m = rec_rows.get(row["counter"])
        ent = {"k": row["k"], "counter": row["counter"], "vsync": row["vsync"]}
        if m:
            beat, r = m
            diff = [key for key in ROW_KEYS if r.get(key) != row.get(key)]
            ent.update({"recorded_row": f"{beat} f{r['f']}", "fields_equal": not diff, "differing": diff})
        align.append(ent)
    rec_c = ctx["recorded_snapshot"].get("counter")
    meta = {
        "label": label, "source": src, "renderer_ini": cap.get("renderer"), **ctx,
        "session_rows": align,
        "iteration_after_recorded_snapshot": [(r["counter"] - rec_c) if rec_c else None for r in cap["rows"]],
        "snapshots": snaps,
        "outputs_from": f"s{k}", "outputs_counter": last["counter"], "outputs_vsync": last["vsync"],
        "field": {"CSR_FIELD": dp["CSR_FIELD"], "vsync_parity": last["vsync"] & 1,
                  "SMODE2": dp["SMODE2"], "displayed_fbp": hex(shown), "drawn_fbp": hex(drawn),
                  "drawn_ofy": [c["OFX_OFY"][1] for c in registers["contexts"]]},
        "displayed_buffer": {"fbp": hex(shown), "last_written_at": written_in[hex(shown)],
                             "uniform_fill": bool((disp == FILL).all()),
                             "distinct_words": int(len(np.unique(disp))),
                             "alpha_values": sorted(int(v) for v in np.unique(alpha))[:16]},
        "draw_buffer": {"fbp": hex(drawn), "last_written_at": written_in[hex(drawn)],
                        "uniform_fill": bool((draw == FILL).all()), "distinct_words": int(len(np.unique(draw)))},
        "z_buffer": {"zbp": hex(ZBP), "psm": fa["contexts"][0]["ZPSM"], "last_written_at": written_in["z"],
                     "distinct_words": int(len(np.unique(zb))), "top_byte_values": sorted(int(v) for v in np.unique(zb >> 24))[:8]},
        "source_state_buffers_uniform_fill": src_fill,
        "decode_proof": proof,
        "files": {"displayed.bin": "512x224 PSMCT32 words, raster order, little-endian (R,G,B,A bytes)",
                  "draw.bin": "same, the FRAME (drawing) buffer", "z.bin": "512x224 PSMZ24 words (low 24 bits = Z)",
                  "displayed.png": "the displayed field 512x224, RGB, alpha set opaque, no scaling",
                  "draw.png": "the drawing buffer 512x224",
                  "PREVIEW_ONLY_displayed_448_line_doubled.png": "PREVIEW ONLY: each line doubled to 448; not the GS output"},
    }
    (out / "meta.json").write_text(json.dumps(meta, indent=1) + "\n")
    return meta


def fb2_decode_all(labels: list[str] | None) -> None:
    path = FB2 / "summary.json"
    summary = json.loads(path.read_text()) if labels and path.exists() else {}   # --points: update in place
    for label, _src in FB2_POINTS:
        if labels and label not in labels:
            continue
        if not (FB2 / label / "capture.json").exists():
            continue
        m = fb2_decode(label)
        s = {"counters": [r["counter"] for r in m["session_rows"]], "field": m["field"],
             "displayed": m["displayed_buffer"], "draw": m["draw_buffer"],
             "z_written_at": m["z_buffer"]["last_written_at"],
             "row_match": [(r.get("recorded_row"), r.get("fields_equal")) for r in m["session_rows"]],
             "stages": [(x["at"], x["counter"], x["csr_field"], x["dispfb2_fbp"], x["frame_fbp"][0],
                         {b: x["buffers"][b]["rows_changed_since_previous_stage"] for b in x["buffers"]},
                         x.get("host_screenshot_mae")) for x in m["snapshots"]],
             "proof": m["decode_proof"]}
        summary[label] = s
        print(label, json.dumps(s)[:1200], flush=True)
    (FB2 / "summary.json").write_text(json.dumps(summary, indent=1) + "\n")



if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("item", choices=["fb", "fb-survey", "h7", "fb2", "fb2-decode"])
    ap.add_argument("--mode", default="both", choices=["fields", "writer", "both"])
    ap.add_argument("--frames", type=int, default=8)
    ap.add_argument("--points", default="", help="fb2: comma list of point labels (default: all)")
    ap.add_argument("--steps", type=int, default=2, help="fb2: frames stepped after the aligned load")
    a = ap.parse_args()
    labels = [x for x in a.points.split(",") if x] or None
    if a.item == "fb2":
        require_software_renderer()     # refuses (before any emulator start) unless the ini says 13
    try:
        if a.item == "fb2":
            fb2_capture(labels, a.steps)
            fb2_decode_all(labels)
        elif a.item == "fb2-decode":
            fb2_decode_all(labels)
        elif a.item == "fb":
            fb_run(a.frames)
            fb_survey()
        elif a.item == "fb-survey":
            fb_refresh_points()
            fb_survey()
        else:
            for m in (["fields", "writer"] if a.mode == "both" else [a.mode]):
                h7_run(m)
    finally:
        print("no emulator process left:", no_emulator_left(), flush=True)
        if a.item == "fb2":
            print(f"renderer in {LIVE_INI}: {ini_renderer()}.  " + RESTORE_HINT, flush=True)
