#!/usr/bin/env python3
"""fork_pixel_refs.py - the v2.6.3 pixel references re-recorded on the PCSX2 fork.

The user's decision of 2026-10-09: re-record the pixel references in the
agent-debug fork (docs/PCSX2_FORK.md, "Retiring the 2.6.3 app" item 4), with
the deterministic field phase of the phase-locked chain.  Every set goes to
build/fork_refs/pixels/<set>/ (ignored) in the layout the port's tests read
today, so the port's switch is a path change, plus a manifest.json that
records per frame the tick, the field phase (D_00810E88 / CSR FIELD / OFY),
the drawing buffer (D_00810E80 / FRAME) and the fork build, and the
comparison with the v2.6.3 frame.

Sets:
  fb2    the 19 framebuffer points of docs/CAPTURES_C7.md 5b (the port's
         tools/test_fb2_pixels.py).  Captured by `c7cap_partb.py fb2
         --fb2-out build/fork_refs/pixels/fb2` (fork default): each point
         loads the fork chain's state at the same game point and first
         replays the v2.6.3 session's free frames after its load (`--lead
         legacy`), so s0, s1 and s2 are the v2.6.3 ticks.
  b16    the GS conformance batches of docs/GS_CONFORMANCE.md (the port's
         test_gs_raster_reference.py / test_gs_fog_conformance.py, which read
         GSCAP_ROOT/<set>/<batch>/snap/gs.bin): gscap, gscap3..gscap8 and
         gscap_repeat, captured by `gscap` below.
  route  the main route's end frames (build/fork_refs/s87/route/<beat>,
         already re-recorded): the displayed and drawing buffers decoded from
         each snapshot's GS freeze (`route` below).  The v2.6.3 route
         snapshots were saved with the Metal renderer, so they hold no GS
         pixels; their original.png is the 640x480 host presentation.

Subcommands (decomp .venv python, repo root; macOS arm64 host, the fork runs
x86_64 under Rosetta; emulator runs take the shared run lock and leave
nothing running):
  gscap [SET ...]           emulator: capture + decode the b16 sets
  route                     no emulator: decode the route frames
  compare [fb2|b16|route]   no emulator: compare with v2.6.3, write the manifests

Nothing here embeds original code or data; it names addresses only.  All
outputs are derived from the user's own disc and stay in ignored build/.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import struct
import subprocess
import sys
import time
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

PIX = ROOT / "build/fork_refs/pixels"
LEGACY_FB2 = ROOT / "build/s87/c7cap/fb2"
LEGACY_B16 = ROOT / "build/b16"
LEGACY_ROUTE = ROOT / "build/s87/route"
FORK_ROUTE = ROOT / "build/fork_refs/pixels/chain/s87/route"   # the corrected chain (see "The corrected chain")
FORK_REPO = ROOT.parent / "pcsx2-fork"
PY = ROOT / ".venv/bin/python"
W, H = 512, 224
RAND_STATE = 0x2426C8          # the game's random-number state word (PCSX2_FORK_GS_DIFF.md 5.4)
FRAME_INDEX, FIELD, VSYNC = 0x810E80, 0x810E88, 0x810E90
B16_SETS = {                    # set -> (tool, batches)
    "gscap": ("gs_conformance.py", ["layout", "raster", "shade", "texture", "pixel", "frame", "probe2"]),
    "gscap3": ("gs_conformance_probe3.py", ["p3_start", "p3_z", "p3_stq"]),
    "gscap4": ("gs_conformance_probe4.py", ["p4_rcp", "p4_z", "p4_misc"]),
    "gscap5": ("gs_conformance_probe5.py", ["p5_s", "p5_q"]),
    "gscap6": ("gs_conformance_probe6.py", ["p6_cov", "p6_tfx", "p6_z"]),
    "gscap7": ("gs_conformance_probe7.py", ["p7_lvl", "p7_wrap", "p7_z", "p7_zc", "p7_scope", "p7_flush"]),
    "gscap8": ("gs_conformance_probe8.py", ["p8_span", "p8_class", "p8_misc", "p8_gif", "p8_more", "p8_gif2"]),
    "gscap_repeat": ("gs_conformance.py", ["layout", "raster", "shade", "texture", "pixel", "frame", "probe2"]),
}
# The fields compared between a fork row and the v2.6.3 row of the same tick
# (route_capture's sampler; counters, vsync and the phase are compared apart).
ROW_SKIP = {"counter", "f", "vs", "fi", "fld", "k", "vsync", "hw", "inside_step", "snapshot"}


def fork_build() -> dict:
    head = subprocess.run(["git", "-C", str(FORK_REPO), "rev-parse", "HEAD"], capture_output=True, text=True)
    return {"repo": str(FORK_REPO), "repo_head": head.stdout.strip() or None}


def write_json(path: Path, doc) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix(".tmp")
    tmp.write_text(json.dumps(doc, indent=1) + "\n")
    tmp.replace(path)


def ee_of_state(state: Path) -> bytes:
    from parse_pcsx2_state import extract_zstd_entry
    return extract_zstd_entry(state, "eeMemory.bin")


def u32(b: bytes, a: int) -> int:
    return struct.unpack_from("<I", b, a)[0]


def rgb_compare(a: np.ndarray, b: np.ndarray) -> dict:
    """Two (224, 512) PSMCT32 word arrays: RGB and alpha differences."""
    A = a.view(np.uint8).reshape(H, W, 4).astype(np.int16)
    B = b.view(np.uint8).reshape(H, W, 4).astype(np.int16)
    d = np.abs(A - B)
    m = d[..., :3].max(-1)
    px = m > 0
    out = {"rgb_pixels_differ": int(px.sum()), "alpha_pixels_differ": int((d[..., 3] > 0).sum()),
           "words_differ": int((a != b).sum())}
    if px.any():
        ys, xs = np.nonzero(px)
        out.update({"bbox_xyxy": [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
                    "rows_with_difference": int(len(np.unique(ys))),
                    "max_channel_error": int(m.max()), "mean_channel_error_over_differing": round(float(m[px].mean()), 2),
                    "only_row_223": bool((ys == 223).all())})
    return out


def blobs(mask: np.ndarray) -> dict:
    """Connected regions (4-neighbour) of a difference mask: their count and
    the largest one's size and box.  Many small blobs = scattered sprites."""
    seen = np.zeros_like(mask, bool)
    sizes, boxes = [], []
    ys, xs = np.nonzero(mask)
    for y0, x0 in zip(ys, xs):
        if seen[y0, x0]:
            continue
        stack, n, bx = [(y0, x0)], 0, [x0, y0, x0, y0]
        seen[y0, x0] = True
        while stack:
            y, x = stack.pop()
            n += 1
            bx = [min(bx[0], x), min(bx[1], y), max(bx[2], x), max(bx[3], y)]
            for yy, xx in ((y + 1, x), (y - 1, x), (y, x + 1), (y, x - 1)):
                if 0 <= yy < H and 0 <= xx < W and mask[yy, xx] and not seen[yy, xx]:
                    seen[yy, xx] = True
                    stack.append((yy, xx))
        sizes.append(n)
        boxes.append(bx)
    if not sizes:
        return {"count": 0}
    i = int(np.argmax(sizes))
    return {"count": len(sizes), "largest": sizes[i], "largest_box_xyxy": [int(v) for v in boxes[i]],
            "median_size": float(np.median(sizes))}


def diff_png(path: Path, a: np.ndarray, b: np.ndarray) -> None:
    from clut import write_png_rgba
    path.parent.mkdir(parents=True, exist_ok=True)
    A = a.view(np.uint8).reshape(H, W, 4)[..., :3]
    B = b.view(np.uint8).reshape(H, W, 4)[..., :3]
    d = np.abs(A.astype(np.int16) - B.astype(np.int16)).max(-1)
    D = np.stack([np.minimum(d * 8, 255)] * 3, -1).astype(np.uint8)
    img = np.concatenate([A, B, D], 0)
    rgba = np.concatenate([img, np.full(img.shape[:2] + (1,), 255, np.uint8)], -1)
    write_png_rgba(path, W, img.shape[0], rgba.tobytes())


# ---------------------------------------------------------------------------
# fb2

def fb2_tops(meta: dict) -> dict:
    st = {s["at"]: s for s in meta["snapshots"]}
    out = {}
    for k in ("s0", "s1", "s2"):
        s = st[k]
        hw = s.get("hw") or {}
        out[k] = {"counter": s["counter"], "vsync": s["vsync"], "csr_field": s["csr_field"],
                  "dispfb2_fbp": s["dispfb2_fbp"], "frame_fbp": s["frame_fbp"][0], "ofy": s["ofy"][0],
                  "frame_index_810E80": int(hw["frame_idx_810E80"], 16) if "frame_idx_810E80" in hw else None,
                  "field_810E88": int(hw["field_810E88"], 16) if "field_810E88" in hw else None}
    return out


def fb2_rows(cap: dict) -> list[dict]:
    return [{k: v for k, v in r.items() if k not in ROW_SKIP} for r in cap["rows"]]


def compare_fb2() -> dict:
    import c7cap_partb as cp
    out_dir = PIX / "fb2"
    legacy_refs = json.loads((ROOT / "build/fork_refs/legacy_refs.json").read_text())
    points = {}
    for d in sorted(p for p in out_dir.iterdir() if (p / "meta.json").exists()):
        label = d.name
        L, F = LEGACY_FB2 / label, d
        lm, fm = json.loads((L / "meta.json").read_text()), json.loads((F / "meta.json").read_text())
        lc, fc = json.loads((L / "capture.json").read_text()), json.loads((F / "capture.json").read_text())
        src = fc["source"]
        lt, ft = fb2_tops(lm), fb2_tops(fm)
        lrec = (lm.get("recorded_snapshot") or {}).get("counter")
        if lrec is None and len(src) == 2:
            lrec = legacy_refs["slots"][src]["counter"]
        frec = fc.get("fork", {}).get("loaded_counter")
        ent = {"source": src, "fork_state": fc.get("source_state"),
               "fork_build": {k: (fc.get("fork", {}).get("hello") or {}).get(k) for k in ("rev", "hash", "server_version")},
               "lead_frames": fc.get("fork", {}).get("lead_frames"),
               "tick": {"fork_recorded_counter": frec, "legacy_recorded_counter": lrec,
                        "fork_s1": ft["s1"]["counter"], "legacy_s1": lt["s1"]["counter"],
                        "s1_after_recorded_equal": (ft["s1"]["counter"] - frec) == (lt["s1"]["counter"] - lrec)
                        if frec is not None and lrec is not None else None},
               "fork_tops": ft, "legacy_tops": lt}
        # phase of the displayed field: drawn in s1 -> s2 into s1's FRAME with s1's OFY
        ph = {"buffer_equal": ft["s1"]["frame_fbp"] == lt["s1"]["frame_fbp"],
              "ofy_equal": ft["s1"]["ofy"] == lt["s1"]["ofy"],
              "fork_s1": [ft["s1"]["frame_fbp"], ft["s1"]["ofy"]], "legacy_s1": [lt["s1"]["frame_fbp"], lt["s1"]["ofy"]],
              "legacy_extra_vsyncs_to_s1": (lt["s1"]["vsync"] - lt["s0"]["vsync"] - 1)
              + ((lt["s0"]["vsync"] - lm["recorded_snapshot"]["vsync"] - (lt["s0"]["counter"] - lrec))
                 if (lm.get("recorded_snapshot") or {}).get("vsync") is not None else 0),
              "fork_extra_vsyncs_to_s1": (ft["s1"]["vsync"] - ft["s0"]["vsync"] - 1)}
        ent["phase"] = ph
        # game state: the sampled rows and the random-number state at s2
        lr, fr = fb2_rows(lc), fb2_rows(fc)
        ent["rows_differ"] = [sorted(k for k in set(a) | set(b) if a.get(k) != b.get(k)) for a, b in zip(lr, fr)]
        le, fe = ee_of_state(L / "s2/state.p2s"), ee_of_state(F / "s2/state.p2s")
        ent["rand_state_s2"] = {"legacy": hex(u32(le, RAND_STATE)), "fork": hex(u32(fe, RAND_STATE)),
                                "equal": u32(le, RAND_STATE) == u32(fe, RAND_STATE)}
        # pixels
        px = {}
        for name in ("displayed.bin", "draw.bin", "z.bin"):
            a = np.fromfile(L / name, "<u4").reshape(H, W)
            b = np.fromfile(F / name, "<u4").reshape(H, W)
            c = {"bit_exact": bool((a == b).all())}
            if name == "z.bin":
                c["words_differ"] = int((a != b).sum())
            else:
                c.update(rgb_compare(a, b))
                if not c["bit_exact"]:
                    A = a.view(np.uint8).reshape(H, W, 4)[..., :3].astype(np.int16)
                    B = b.view(np.uint8).reshape(H, W, 4)[..., :3].astype(np.int16)
                    c["blobs"] = blobs(np.abs(A - B).max(-1) > 0)
                    if name == "displayed.bin":
                        diff_png(PIX / "_compare/fb2" / f"{label}.png", a, b)
            px[name.split(".")[0]] = c
        ent["pixels"] = px
        ent["cause"] = fb2_cause(ent)
        points[label] = ent
        print(label, ent["cause"]["summary"], px["displayed"].get("rgb_pixels_differ"), flush=True)
    return points


def fb2_cause(e: dict) -> dict:
    disp = e["pixels"]["displayed"]
    if disp["bit_exact"]:
        return {"summary": "bit-exact", "causes": []}
    causes = []
    if not e["tick"]["s1_after_recorded_equal"]:
        causes.append("tick: s1 is not the same number of frames after the recorded state")
    if not (e["phase"]["ofy_equal"] and e["phase"]["buffer_equal"]):
        x = e["phase"]["legacy_extra_vsyncs_to_s1"]
        causes.append("field phase: the displayed field was drawn with another OFY/buffer"
                      + (f" (the v2.6.3 session's post-load hitch: {x} extra vsyncs before s1, none in the fork)"
                         if x else ""))
    if any(e["rows_differ"]):
        causes.append("game state: the sampled rows differ (" + ",".join(sorted(set(sum(e["rows_differ"], [])))) + ")")
    if not e["rand_state_s2"]["equal"]:
        causes.append("random-number state differs (rand-driven sprites: snow, flames, lights)")
    if disp.get("only_row_223"):
        causes.append("row 223 only: the upstream scissor culling change (4fa2b8e45)")
    if not causes:
        causes.append("new: tick, phase, rows and random state equal")
    return {"summary": "; ".join(c.split(":")[0].split(" (")[0] for c in causes), "causes": causes}


def manifest_fb2(points: dict) -> None:
    doc = {
        "what": "the 19 fb2 framebuffer points (docs/CAPTURES_C7.md 5b) re-recorded on the PCSX2 fork "
                "(c7cap_partb.py fb2 --corrected-chain --fb2-out build/fork_refs/pixels/fb2, lead from the v2.6.3 point); "
                "same per-point layout as build/s87/c7cap/fb2 (meta.json, capture.json, displayed.bin, "
                "draw.bin, z.bin, s0..s2 stage folders)",
        "port_switch": {
            "FB2": "build/fork_refs/pixels/fb2 (was build/s87/c7cap/fb2)",
            "ROUTE": "build/fork_refs/pixels/chain/s87/route (was build/s87/route): the corrected chain, fork counters",
            "FIRST_CONTROL_COUNTER": fork_first_control_counter(),
        },
        "fork": fork_build(), "recorded": time.strftime("%Y-%m-%d %H:%M:%S"),
        "renderer": 13, "mtvu": False,
        "pairing_rule": "compare by tick (frames after the recorded state), field phase (s1 OFY) and each "
                        "run's own drawing buffer (PCSX2_FORK_GS_DIFF.md 8)",
        "summary": {"points": len(points),
                    "displayed_bit_exact": sum(p["pixels"]["displayed"]["bit_exact"] for p in points.values()),
                    "draw_bit_exact": sum(p["pixels"]["draw"]["bit_exact"] for p in points.values()),
                    "z_bit_exact": sum(p["pixels"]["z"]["bit_exact"] for p in points.values())},
        "points": points}
    write_json(PIX / "fb2/manifest.json", doc)


def fork_first_control_counter() -> int | None:
    t = FORK_ROUTE / "01_battery/trace.json"
    return json.loads(t.read_text())["first_counter"] if t.exists() else None


# ---------------------------------------------------------------------------
# b16

def gscap(sets: list[str]) -> None:
    for name in sets:
        tool, _ = B16_SETS[name]
        env = dict(os.environ, GSCAP_OUT=str(PIX / "b16" / name))
        for cmd in ("capture", "decode"):
            r = subprocess.run([str(PY), str(ROOT / "tools" / tool), "--emulator", "fork", cmd],
                               cwd=ROOT, env=env)
            if r.returncode:
                raise SystemExit(f"{tool} {cmd} for {name}: exit {r.returncode}")


def compare_b16() -> dict:
    import gs_conformance as G
    G.OUT = PIX / "b16" / "gscap"          # the fork run's own layout maps
    sets = {}
    for name, (_tool, batches) in B16_SETS.items():
        F0, L0 = PIX / "b16" / name, LEGACY_B16 / name
        if not F0.exists():
            continue
        res = {}
        for b in batches:
            F, L = F0 / b, L0 / b
            if not (F / "snap/gs.bin").exists():
                res[b] = {"captured": False}
                continue
            e = {"packet_equal": (F / "packet.bin").read_bytes() == (L / "packet.bin").read_bytes()}
            fk, lk = json.loads((F / "kick.json").read_text()), json.loads((L / "kick.json").read_text())
            e["fork_at"], e["legacy_at"] = fk["at"], lk["at"]
            e["dma_done"] = [fk["dma_done"], lk["dma_done"]]
            fv, lv = G.vram_of((F / "snap/gs.bin").read_bytes()), G.vram_of((L / "snap/gs.bin").read_bytes())
            a, c = np.frombuffer(fv, np.uint8), np.frombuffer(lv, np.uint8)
            e["vram_bytes_differ"] = int((a != c).sum())
            doc = json.loads((F / "batch.json").read_text())
            tests, bad = 0, {}
            for t in doc["tests"]:
                fz, lz = F / f"{t['name']}.npz", L / f"{t['name']}.npz"
                if not (fz.exists() and lz.exists()):
                    bad[t["name"]] = "not decoded"
                    continue
                fa, la = dict(np.load(fz)), dict(np.load(lz))
                tests += 1
                diff = {k: int((fa[k] != la[k]).sum()) for k in fa if k in la and fa[k].shape == la[k].shape}
                if any(diff.values()) or set(fa) != set(la):
                    bad[t["name"]] = diff
            e.update(b16_regions(G, fv, lv, doc))
            fd = json.loads((F / "decode.json").read_text()) if (F / "decode.json").exists() else {}
            e.update({"tests": tests, "tests_bit_exact": tests - len(bad), "tests_differ": bad,
                      "fence_ok": fd.get("fence_ok")})
            res[b] = e
        sets[name] = res
        n = sum(v.get("tests", 0) for v in res.values())
        x = sum(v.get("tests_bit_exact", 0) for v in res.values())
        print(name, f"{x}/{n} tests bit-exact", flush=True)
    return sets


def _images(o, out: list) -> list:
    if isinstance(o, dict):
        if "image" in o:
            out.append(o["image"])
        for v in o.values():
            _images(v, out)
    elif isinstance(o, list):
        for v in o:
            _images(v, out)
    return out


def b16_regions(G, fv: bytes, lv: bytes, doc: dict) -> dict:
    """Where the two local memories differ: the 8 KiB pages, how many of them
    lie in a test's colour / Z buffer or the fence page (the batch's own
    output), and the CT32 uploads compared rectangle by rectangle through the
    measured maps (T8 / T4 and unaligned uploads are counted, not compared)."""
    import math
    a, c = np.frombuffer(fv, np.uint8), np.frombuffer(lv, np.uint8)
    pages = set(np.nonzero((a != c).reshape(-1, 8192).any(1))[0].tolist())
    used = {doc["fence"]["page"]}
    for t in doc["tests"]:
        if t["name"] == "upload":
            continue
        for page, psm in ((t["fbp"], t["psm"]), (t.get("zbp"), t.get("zpsm"))):
            if psm:
                ph = 64 if psm in ("CT16", "CT16S", "Z16", "Z16S") else 32
                used |= set(range(page, page + math.ceil(t["w"] / 64) * math.ceil(t["h"] / ph)))
    maps = G.load_maps()
    checked = equal = skipped = 0
    for i in _images(doc["tests"], []):
        if i["psm"] != "CT32" or i["dbp"] % 32 or i["dbw"] % 64:
            skipped += 1
            continue
        h = ((i["dy"] + i["h"] + 31) // 32) * 32
        sl = (slice(i["dy"], i["dy"] + i["h"]), slice(i["dx"], i["dx"] + i["w"]))
        x = G.decode_buffer(fv, i["dbp"] // 32, i["dbw"], h, "CT32", maps)[sl]
        y = G.decode_buffer(lv, i["dbp"] // 32, i["dbw"], h, "CT32", maps)[sl]
        checked += 1
        equal += bool((x == y).all())
    return {"pages_differ": len(pages), "pages_differ_in_test_buffers": len(pages & used),
            "ct32_uploads_checked": checked, "ct32_uploads_equal": equal, "uploads_not_compared": skipped}


def manifest_b16(sets: dict) -> None:
    hello = None
    for name in sets:
        for b in sets[name]:
            snap = PIX / "b16" / name / b / "snap/snapshot.json"
            if snap.exists():
                s = json.loads(snap.read_text())
                hello = {"rev": s.get("fork_rev"), "hash": s.get("fork_hash")}
                break
        if hello:
            break
    doc = {"what": "the GS conformance batches (docs/GS_CONFORMANCE.md) re-captured on the PCSX2 fork "
                   "from fork state phase/04; same layout as build/b16/<set>/<batch>/ "
                   "(the rest of local memory holds the game's own frame and textures at the kick, a "
                   "different game moment in each run, so whole-memory bytes are not comparable) "
                   "(batch.json, packet.bin, kick.json, snap/gs.bin, <test>.npz, decode.json)",
           "port_switch": {"GSCAP_ROOT": "build/fork_refs/pixels/b16 (was build/b16)"},
           "fork": {**fork_build(), "build": hello}, "renderer": 13, "mtvu": False,
           "recorded": time.strftime("%Y-%m-%d %H:%M:%S"),
           "summary": {name: {"batches": len(v), "tests": sum(x.get("tests", 0) for x in v.values()),
                              "tests_bit_exact": sum(x.get("tests_bit_exact", 0) for x in v.values()),
                              "batches_vram_identical": sum(x.get("vram_bytes_differ") == 0 for x in v.values()),
                              "pages_differ_in_test_buffers": sum(x.get("pages_differ_in_test_buffers", 0)
                                                                  for x in v.values()),
                              "ct32_uploads_equal": [sum(x.get("ct32_uploads_equal", 0) for x in v.values()),
                                                     sum(x.get("ct32_uploads_checked", 0) for x in v.values())]}
                       for name, v in sets.items()},
           "sets": sets}
    write_json(PIX / "b16/manifest.json", doc)


# ---------------------------------------------------------------------------
# route frames

def route() -> dict:
    import c7cap_partb as cp
    from clut import write_png_rgba
    beats = {}
    for d in sorted(p for p in FORK_ROUTE.iterdir() if (p / "state.p2s").exists()):
        out = PIX / "route" / d.name
        out.mkdir(parents=True, exist_ok=True)
        regs = cp.priv_from_state(d / "state.p2s")
        dp = cp.decode_priv(regs)
        gs = (d / "gs.bin").read_bytes()
        fa = cp.freeze_analysis(gs, [0x0, 0x38])
        vram = gs[cp.FREEZE_VRAM:cp.FREEZE_VRAM + cp.LOCALMEM]
        shown, drawn = dp["DISPFB2"]["FBP"], fa["contexts"][0]["FRAME"]["FBP"]
        disp, draw = cp.gs_buffer(vram, shown), cp.gs_buffer(vram, drawn)
        (out / "displayed.bin").write_bytes(disp.astype("<u4").tobytes())
        (out / "draw.bin").write_bytes(draw.astype("<u4").tobytes())
        write_png_rgba(out / "displayed.png", W, H, cp.rgba_bytes(disp))
        write_png_rgba(out / "draw.png", W, H, cp.rgba_bytes(draw))
        snap = json.loads((d / "snapshot.json").read_text())
        ee = (d / "eeMemory.bin").read_bytes()
        shot = cp.read_png_rgba(d / "original.png")
        meta = {"beat": d.name, "snapshot": str((d / "state.p2s").relative_to(ROOT)),
                "counter": snap["main_loop_counter"], "vsync": snap["vsync_counter"],
                "frame_index_810E80": ee[FRAME_INDEX], "field_810E88": ee[FIELD],
                "csr_field": dp["CSR_FIELD"], "dispfb2_fbp": hex(shown), "frame_fbp": hex(drawn),
                "ofy": [(int(c["XYOFFSET_raw"], 16) >> 32 & 0xFFFF) / 16 for c in fa["contexts"]],
                "fork_build": {"rev": snap.get("fork_rev"), "hash": snap.get("fork_hash")},
                "gs_field_equals_displayed": bool((shot == disp.view(np.uint8).reshape(H, W, 4)[..., :3]).all())
                if shot.shape == (H, W, 3) else None,
                "files": {"displayed.bin": "DISPFB2 buffer, 512x224 PSMCT32 words, raster order (R,G,B,A bytes)",
                          "draw.bin": "the FRAME (drawing) buffer", "*.png": "the same, alpha opaque"}}
        # the v2.6.3 snapshot of the same beat
        lg = LEGACY_ROUTE / d.name
        if (lg / "gs.bin").exists():
            lv = (lg / "gs.bin").read_bytes()[cp.FREEZE_VRAM:cp.FREEZE_VRAM + cp.LOCALMEM]
            lb = {k: cp.gs_buffer(lv, p) for k, p in (("0x0", 0), ("0x38", 0x38))}
            uniform = {k: bool((v == cp.FILL).all()) for k, v in lb.items()}
            lsnap = json.loads((lg / "snapshot.json").read_text())
            lp = cp.read_png_rgba(lg / "original.png") if (lg / "original.png").exists() else None
            meta["legacy"] = {"counter": lsnap.get("main_loop_counter"), "buffers_uniform_0x80000000": uniform,
                              "pixel_reference": not all(uniform.values()),
                              "original_png": list(lp.shape) if lp is not None else None,
                              "coarse_luma_corr_with_fork_displayed": cp.corr(
                                  cp.coarse_gray(disp.view(np.uint8).reshape(H, W, 4)[..., :3]), cp.coarse_gray(lp))
                              if lp is not None else None}
        write_json(out / "meta.json", meta)
        beats[d.name] = meta
        print(d.name, meta["counter"], meta["frame_index_810E80"], meta["field_810E88"], meta["dispfb2_fbp"],
              meta.get("legacy", {}).get("coarse_luma_corr_with_fork_displayed"), flush=True)
    write_json(PIX / "route/manifest.json", {
        "what": "the main route's end frames (build/fork_refs/pixels/chain/s87/route/<beat>/state.p2s, the corrected chain) "
                "decoded from each snapshot's GS freeze: displayed (DISPFB2) and drawing (FRAME) buffers",
        "fork": fork_build(), "recorded": time.strftime("%Y-%m-%d %H:%M:%S"),
        "note": "the v2.6.3 route snapshots were saved with the Metal renderer: their GS buffers hold the "
                "uniform 0x80000000 fill and their original.png is the 640x480 host presentation, so no "
                "bit-exact v2.6.3 pixel reference exists for these frames (CAPTURES_C7.md 5)",
        "beats": beats})
    return beats


# ---------------------------------------------------------------------------
# The corrected chain (2026-10-09 finding)
#
# The v2.6.3 user slots 01..04 and 15 were saved by hand at the first
# instruction of the vsync wait (EE PC 0x1AAFF0): the iteration's game logic
# had already run, its frame-index toggle and the vsync ISR had not.  A v2.6.3
# session that loads such a slot finishes that iteration without new logic,
# so its next loop top (counter + 1) is the first full update.  The fork's
# states are saved at the loop top.  phase/slot04 (counter 3299) was matched to
# slot 04's stored frame index and field (1, 0), which are the mid-iteration
# values; the v2.6.3 loop top after the load has (0, 1).  So the phase chain
# runs one game update ahead of the v2.6.3 chain in everything that moves on
# its own (rand(), the player's clock, the r9 attachment, the snow), while
# input-driven rows still agree.  base/slot04 (counter 3300, (0, 1)) is the
# v2.6.3 loop top 4084: started there with the v2.6.3 lead-in minus the one
# finishing iteration, a beat lands on the v2.6.3 tick, phase AND update count.

MID_ITERATION_SLOTS = {"01", "02", "03", "04", "15"}       # saved at PC 0x1AAFF0 (checked 2026-10-09)
CHAIN = PIX / "chain"


def use_corrected_chain() -> None:
    import route_capture as rc
    import pcsx2_session as ps
    rc.use_fork("phase", True)
    ps.FORK_MTVU = False
    rc.FORK_REFS = CHAIN                                  # beat folders: pixels/chain/s87/route/...
    rc.FORK_PHASE_BEATS = CHAIN / "_state_links"          # never into fork-states/phase/beats
    orig_ref, orig_source = rc.legacy_reference, rc.beat_source

    def legacy_reference(name: str, source: str):
        ref = orig_ref(name, source)
        if ref is not None and source in MID_ITERATION_SLOTS:
            ref = dict(ref, lead_in=ref["lead_in"] - 1, lead_in_note="minus the v2.6.3 finishing iteration")
        return ref

    def beat_source(source: str) -> Path:
        if source in MID_ITERATION_SLOTS:
            return ps.fork_state(source, "base")
        return orig_source(source)

    rc.legacy_reference, rc.beat_source = legacy_reference, beat_source


def chain(beats: list[str]) -> None:
    import route_capture as rc
    use_corrected_chain()
    for name, source, fn in rc.BEATS:
        if name[:2] in beats or name in beats:
            rc.run_beat(name, source, fn)


def compare_chain() -> dict:
    """Every traced field of the corrected chain against the v2.6.3 trace, row by row."""
    out = {}
    base = CHAIN / "s87/route"
    for d in sorted(p for p in base.iterdir() if (p / "trace.json").exists()):
        L = json.loads((LEGACY_ROUTE / d.name / "trace.json").read_text())["rows"]
        F = json.loads((d / "trace.json").read_text())
        rows = F["rows"]
        diff: dict[str, list[int]] = {}
        for a, b in zip(L, rows):
            for k in a:
                if k not in ("counter", "f") and a.get(k) != b.get(k):
                    diff.setdefault(k, []).append(a["f"])
        le, fe = (LEGACY_ROUTE / d.name / "eeMemory.bin").read_bytes(), (d / "eeMemory.bin").read_bytes()
        out[d.name] = {"rows": [len(L), len(rows)], "lead_in": F.get("lead_in_frames"),
                       "phase": (F.get("phase") or {}).get("legacy"),
                       "fields_differ": {k: {"rows": len(v), "first": v[0]} for k, v in diff.items()},
                       "rand_end_equal": le[RAND_STATE:RAND_STATE + 4] == fe[RAND_STATE:RAND_STATE + 4]}
        print(d.name, out[d.name]["rows"], sorted(out[d.name]["fields_differ"]), "rand equal", out[d.name]["rand_end_equal"],
              flush=True)
    write_json(CHAIN / "s87/route/compare_v263.json", out)
    return out


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd", required=True)
    g = sub.add_parser("gscap")
    g.add_argument("sets", nargs="*")
    sub.add_parser("route")
    ch = sub.add_parser("chain")
    ch.add_argument("beats", nargs="*")
    sub.add_parser("compare-chain")
    c = sub.add_parser("compare")
    c.add_argument("sets", nargs="*")
    a = ap.parse_args()
    if a.cmd == "gscap":
        gscap(a.sets or list(B16_SETS))
    elif a.cmd == "route":
        route()
    elif a.cmd == "chain":
        chain(a.beats or [f"{i:02d}" for i in range(15)])
    elif a.cmd == "compare-chain":
        compare_chain()
    else:
        for s in a.sets or ["fb2", "b16"]:
            if s == "fb2":
                manifest_fb2(compare_fb2())
            elif s == "b16":
                manifest_b16(compare_b16())
            elif s == "route":
                route()


if __name__ == "__main__":
    main()
