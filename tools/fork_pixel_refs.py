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
         GSCAP_ROOT/<set>/<batch>/snap/gs.bin): gscap, gscap3..gscap8, gscap9
         (the half-pixel scissor edge: gs_conformance_probe9.py) and
         gscap_repeat, captured by `gscap` below.
  messages  message frames (the port's test_message_capture): the
         canonical beat replayed to the v2.6.3 reference's message state,
         displayed field and drawing buffer at the next loop top.
  route  the main route's end frames (the canonical chain
         build/fork_refs/s87/route/<beat>): the displayed and drawing buffers
         decoded from each snapshot's GS freeze (`route` below).  The v2.6.3 route
         snapshots were saved with the Metal renderer, so they hold no GS
         pixels; their original.png is the 640x480 host presentation.

Subcommands (decomp .venv python, repo root; macOS arm64 host, the fork runs
x86_64 under Rosetta; emulator runs take the shared run lock and leave
nothing running):
  gscap [SET ...]           emulator: capture + decode the b16 sets
  chain [BEATS ...]         emulator: (re)record main-route beats into the canonical
                            chain build/fork_refs/s87/route (route_capture's
                            default; route_lanes.py does it in lanes)
  compare-chain             no emulator: every traced field and the end rand()
                            state of the canonical chain against v2.6.3
  route                     no emulator: decode the route frames
  compare [fb2|b16|route]   no emulator: compare with v2.6.3, write the manifests
  message [NAME ...]        emulator: a message frame from its canonical route beat
                            (elevator_refusal: beat 02) into
                            build/fork_refs/pixels/messages/<name>/ with a manifest

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
FORK_ROUTE = ROOT / "build/fork_refs/s87/route"   # the canonical chain (slot start corrected, see below)
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
    # the half-pixel scissor edge (PCSX2_FORK_GS_DIFF.md 3.3; new on 2026-10-10, no v2.6.3 set before it)
    "gscap9": ("gs_conformance_probe9.py", ["p9_edges", "p9_game"]),
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
                "(c7cap_partb.py fb2 --fb2-out build/fork_refs/pixels/fb2, points from the canonical chain "
                "build/fork_refs/s87/route, first_control from route_capture.slot_start, lead from the v2.6.3 "
                "point minus the finishing iteration on a user slot); "
                "same per-point layout as build/s87/c7cap/fb2 (meta.json, capture.json, displayed.bin, "
                "draw.bin, z.bin, s0..s2 stage folders)",
        "port_switch": {
            "FB2": "build/fork_refs/pixels/fb2 (was build/s87/c7cap/fb2)",
            "ROUTE": "build/fork_refs/s87/route (was build/s87/route): the canonical chain, fork counters",
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
            if not (L / "snap/gs.bin").exists():      # a fork-only set: no v2.6.3 capture to compare
                fd = json.loads((F / "decode.json").read_text()) if (F / "decode.json").exists() else {}
                res[b] = {"legacy": None, "fence_ok": fd.get("fence_ok"),
                          "tests_fork_only": len(json.loads((F / "batch.json").read_text())["tests"])}
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
           "notes": {"gscap9": GSCAP9_NOTE},
           "sets": sets}
    write_json(PIX / "b16/manifest.json", doc)


GSCAP9_NOTE = (
    "the half-pixel scissor edge (tools/gs_conformance_probe9.py, PCSX2_FORK_GS_DIFF.md 3.3), the one place "
    "where the fork and v2.6.3 differ for the same GS input; the user's decision (2026-10-09): follow the fork. "
    "Its v2.6.3 capture (build/b16/gscap9, 2026-10-10) is the evidence, not the reference: every test that "
    "differs differs only where the fork drew a line or point whose first vertex lies 1/16..7/16 px past the "
    "inner scissor (8/16 past the top and left edges), and v2.6.3 left the clear colour; equal at 0, 9/16, "
    "12/16 and 1 px, for the triangle/sprite controls, and for the game's strips with the vertex at 223.5 and "
    "223.5625. Per-test lit pixels and the v2.6.3 differences: gscap9/report.json")


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
        "what": "the main route's end frames (build/fork_refs/s87/route/<beat>/state.p2s, the canonical chain) "
                "decoded from each snapshot's GS freeze: displayed (DISPFB2) and drawing (FRAME) buffers",
        "fork": fork_build(), "recorded": time.strftime("%Y-%m-%d %H:%M:%S"),
        "note": "the v2.6.3 route snapshots were saved with the Metal renderer: their GS buffers hold the "
                "uniform 0x80000000 fill and their original.png is the 640x480 host presentation, so no "
                "bit-exact v2.6.3 pixel reference exists for these frames (CAPTURES_C7.md 5)",
        "beats": beats})
    return beats


# ---------------------------------------------------------------------------
# The chain (2026-10-09 finding, applied in route_capture since 2026-10-10)
#
# The v2.6.3 user slots were saved by hand at the first instruction of the
# vsync wait (EE PC 0x1AAFF0): the iteration's game logic had already run, its
# frame-index toggle and the vsync ISR had not.  A v2.6.3 session that loads
# such a slot finishes that iteration without new logic, so its next loop top
# (counter + 1) is the first full update.  route_capture.slot_start therefore
# starts a slot beat from the fork state of that loop top with the v2.6.3
# lead-in minus one; the canonical chain build/fork_refs/s87/route was
# recorded that way (until 2026-10-10 this file patched route_capture at run
# time and wrote a separate copy, build/fork_refs/pixels/chain, now deleted).


def chain(beats: list[str]) -> None:
    import route_capture as rc
    import pcsx2_session as ps
    rc.use_fork("phase", True)
    ps.FORK_MTVU = False
    for name, source, fn in rc.BEATS:
        if name[:2] in beats or name in beats:
            rc.run_beat(name, source, fn)


def compare_chain() -> dict:
    """Every traced field of the canonical chain against the v2.6.3 trace, row by row."""
    out = {}
    base = FORK_ROUTE
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
    write_json(FORK_ROUTE / "compare_v263.json", out)
    return out


# ---------------------------------------------------------------------------
# message frames
#
# The elevator refusal (route beat 02, "Switch / No power..."): the v2.6.3
# reference was build/startup-reference/elevator/refusal (the disposable
# elevator_refusal_probe.py: a hand-played slot with poked power, saved at the
# vsync wait while the refusal line 0x8000001A was presented for the fifth
# time, message block +0x68 = 5; its original.png is the 640x480 host
# presentation).  On the fork the canonical beat 02 holds the same refusal
# without pokes; the capture replays it exactly and stops at the vsync wait
# whose message block equals the v2.6.3 one.

MSG_BLOCK = 0x2821B0            # message block: mode, phase, line id; +0x68 present ticks
MESSAGES = {
    "elevator_refusal": {"beat": "02_elevator_refusal", "line": 0x8000001A, "present": 5,
                         "legacy": ROOT / "build/startup-reference/elevator/refusal"},
}
TEXT_BAND_640 = (200, 400, 440, 476)   # the text band used on the v2.6.3 640x480 screenshot


def diff_box(a: np.ndarray, b: np.ndarray) -> dict:
    """Words, RGB pixels and the bounding box [x0, y0, x1, y1] where two buffers differ."""
    m = a != b
    ys, xs = np.nonzero(m)
    return {"words_differ": int(m.sum()), "rgb_pixels_differ": int(((a ^ b) & 0xFFFFFF != 0).sum()),
            "box": [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if m.any() else None}


def text_lines(px: np.ndarray, band: tuple[int, int, int, int]) -> list[list[int]]:
    """Bounding boxes [x0, y0, x1, y1] of bright near-grey text lines inside
    band (an RGB array); lines are split by rows with at most two such pixels."""
    x0, y0, x1, y1 = band
    sub = px[y0:y1, x0:x1, :3].astype(np.int16)
    m = (sub[..., 0] > 150) & (sub[..., 1] > 150) & (sub[..., 2] > 150) & (np.abs(sub[..., 0] - sub[..., 2]) < 40)
    lines, cur = [], None
    for i, row in enumerate(m):
        xs = np.nonzero(row)[0]
        if len(xs) > 2:
            box = [int(xs.min()) + x0, i + y0, int(xs.max()) + x0, i + y0]
            cur = box if cur is None else [min(cur[0], box[0]), cur[1], max(cur[2], box[2]), box[3]]
        elif cur is not None:
            lines.append(cur)
            cur = None
    if cur is not None:
        lines.append(cur)
    return lines


def message(name: str) -> dict:
    """Emulator: replay the message's route beat from the canonical chain and
    capture the message frame (see MESSAGES above)."""
    import c7cap_partb as cp
    import pcsx2_session as ps
    import route_capture as rc
    from clut import write_png_rgba
    spec = MESSAGES[name]
    rc.use_fork("phase", True)
    ps.FORK_MTVU = False
    beat = spec["beat"]
    trace = json.loads((FORK_ROUTE / beat / "trace.json").read_text())
    src = rc.beat_source(trace["source"])
    lead = int(trace.get("lead_in_frames") or 0)
    by_f: dict[int, list[dict]] = {}
    for i in trace["inputs"]:
        by_f.setdefault(i["f"], []).append(i)
    out = PIX / "messages" / name
    if out.exists():
        import shutil
        shutil.rmtree(out)                      # absolute path under build/fork_refs/pixels/messages
    out.mkdir(parents=True)
    keys = None
    mismatches: list[dict] = []
    found = None
    with rc.open_session(src, log_dir=out / "logs") as s:
        if lead:
            s.step(lead)                        # as run_beat: the pad left as the session cleared it
        r = rc.Route(s)
        r.pad_fix = rc.v263_pad_corrections(beat)
        r.begin()

        def check(row: dict) -> None:
            nonlocal keys
            ref = trace["rows"][row["f"]]
            keys = keys or sorted(set(ref) & set(row))
            d = [k for k in keys if ref.get(k) != row.get(k)]
            if d:
                mismatches.append({"f": row["f"], "keys": d})

        check(r.rows[0])
        while found is None:
            f = r.frame_index
            if f >= len(trace["rows"]) - 1:
                raise RuntimeError(f"{name}: message state not reached in the beat's {f} frames")
            for i in by_f.get(f, []):
                r.set_pad(i["buttons"], i["lx"], i["ly"])
            if r.pad_fix is not None:
                r._send_corrected_pad()
            mode, phase, line = struct.unpack("<3I", s.read(MSG_BLOCK, 12))
            if not (phase == 1 and line == spec["line"]):
                r.step(1)
                check(r.rows[-1])
                continue
            # the message is up: stop at this iteration's vsync wait and look at the block
            before = s.u32(ps.FRAME_COUNTER)
            s.debug.call({"cmd": "set_breakpoint", "address": cp.VSYNC_WAIT, "description": "message wait"})
            try:
                s._resume_to_boundary()
                if cp._pc(s) != cp.VSYNC_WAIT or s.u32(ps.FRAME_COUNTER) != before:
                    raise RuntimeError(f"{name}: expected the vsync wait of {before}, at {cp._pc(s):#x}")
                blk = s.read(MSG_BLOCK, 0x70)
                present = struct.unpack_from("<i", blk, 0x68)[0]
                if present == spec["present"]:
                    found = {"counter": before, "f_loop_top": f, "message_block": blk.hex(),
                             "wait": s.snapshot(out / "wait")}
                s.debug.call({"cmd": "remove_breakpoint", "address": cp.VSYNC_WAIT})
                s._resume_to_boundary()
            finally:
                try:
                    s.debug.call({"cmd": "remove_breakpoint", "address": cp.VSYNC_WAIT})
                except Exception:   # noqa: BLE001
                    pass
            if cp._pc(s) != cp.LOOP_TOP or s.u32(ps.FRAME_COUNTER) != before + 1:
                raise RuntimeError(f"{name}: step from {before} ended at {cp._pc(s):#x}")
            s.frames_stepped += 1
            r.frame_index += 1
            r.rows.append(dict(r.now(), f=r.frame_index))
            check(r.rows[-1])
            if found:
                found["top"] = s.snapshot(out / "top")
        fork_hello = s.hello

    def buffers(d: Path) -> dict:
        regs = cp.priv_from_state(d / "state.p2s")
        dp = cp.decode_priv(regs)
        gs = (d / "gs.bin").read_bytes()
        fa = cp.freeze_analysis(gs, [0x0, 0x38])
        vram = gs[cp.FREEZE_VRAM:cp.FREEZE_VRAM + cp.LOCALMEM]
        ee = (d / "eeMemory.bin").read_bytes()
        shown, drawn = dp["DISPFB2"]["FBP"], fa["contexts"][0]["FRAME"]["FBP"]
        return {"disp": cp.gs_buffer(vram, shown), "draw": cp.gs_buffer(vram, drawn),
                "facts": {"counter": u32(d.joinpath("scratchpad.bin").read_bytes(), 0x3B64)
                          if (d / "scratchpad.bin").exists() else None,
                          "frame_index_810E80": ee[FRAME_INDEX], "field_810E88": ee[FIELD],
                          "vsync_810E90": u32(ee, VSYNC), "csr_field": dp["CSR_FIELD"],
                          "dispfb2_fbp": hex(shown), "frame_fbp": hex(drawn),
                          "ofy": [(int(c["XYOFFSET_raw"], 16) >> 32 & 0xFFFF) / 16 for c in fa["contexts"]],
                          "rand_state": hex(u32(ee, RAND_STATE))}}

    rgb = lambda a: a.view(np.uint8).reshape(H, W, 4)[..., :3]      # noqa: E731
    wb, tb = buffers(out / "wait"), buffers(out / "top")
    for stage, b in (("wait", wb), ("top", tb)):
        for k in ("disp", "draw"):
            b[k].astype("<u4").tofile(out / stage / f"{'displayed' if k == 'disp' else 'draw'}.bin")
            write_png_rgba(out / stage / f"{'displayed' if k == 'disp' else 'draw'}.png", W, H, cp.rgba_bytes(b[k]))
    # the outputs, as fb2 (CAPTURES_C7.md 5b): at the loop top after the capture iteration, the
    # displayed field (DISPFB2: drawn in the capture iteration, the line's fifth presentation) and the
    # drawing buffer (FRAME: the previous field, about to be redrawn).  At the vsync wait itself the
    # drawing buffer is not complete: the frame's last GS writes (the second text line) land between
    # the wait and the vsync ISR, while the game waits (self_check)
    (out / "displayed.bin").write_bytes(tb["disp"].astype("<u4").tobytes())
    (out / "draw.bin").write_bytes(tb["draw"].astype("<u4").tobytes())
    write_png_rgba(out / "displayed.png", W, H, cp.rgba_bytes(tb["disp"]))
    write_png_rgba(out / "draw.png", W, H, cp.rgba_bytes(tb["draw"]))
    # v2.6.3 reference (host presentation) for the record
    leg = spec["legacy"]
    legacy = None
    if (leg / "eeMemory.bin").exists():
        le = (leg / "eeMemory.bin").read_bytes()
        lblk = le[MSG_BLOCK:MSG_BLOCK + 0x70]
        lp = cp.read_png_rgba(leg / "original.png")
        band224 = (TEXT_BAND_640[0] * W // 640, TEXT_BAND_640[1] * H // 480,
                   TEXT_BAND_640[2] * W // 640, TEXT_BAND_640[3] * H // 480)
        fb = bytes.fromhex(found["message_block"])
        legacy = {"path": str(leg.relative_to(ROOT)),
                  "message_block_equal": lblk == fb,
                  "message_block_bytes_differ": [hex(i) for i in range(0x70) if lblk[i] != fb[i]],
                  "frame_index_810E80": le[FRAME_INDEX], "field_810E88": le[FIELD],
                  "player_pos_810350": list(struct.unpack_from("<3f", le, 0x810350)),
                  "original_png": list(lp.shape),
                  "text_lines_640x480": text_lines(lp, TEXT_BAND_640),
                  "fork_text_lines_field_512x224": text_lines(rgb(tb["disp"]), band224),
                  "fork_text_lines_scaled_to_640x480": [
                      [x0 * 640 // W, y0 * 480 // H, (x1 + 1) * 640 // W - 1, (y1 + 1) * 480 // H - 1]
                      for x0, y0, x1, y1 in text_lines(rgb(tb["disp"]), band224)],
                  "note": "the v2.6.3 run was a different session (hand-played slot 06, power bit and "
                          "elevator owner poked): only the message state and the player position are "
                          "comparable; its original.png is the host presentation, not a GS field"}
    wee = (out / "wait" / "eeMemory.bin").read_bytes()
    doc = {
        "what": f"the {name} message frame on the PCSX2 fork (fork_pixel_refs.py message {name})",
        "fork": fork_build(), "fork_build": {k: fork_hello.get(k) for k in ("rev", "hash", "server_version")},
        "recorded": time.strftime("%Y-%m-%d %H:%M:%S"), "renderer": 13, "mtvu": False,
        "source": {"beat": beat, "trace": str((FORK_ROUTE / beat / "trace.json").relative_to(ROOT)),
                   "state": str(src), "lead_in_frames": lead},
        "alignment": {
            "how": "replays the canonical beat's recorded inputs from its source state (with its lead-in); "
                   "every replayed loop-top row is compared with the recorded row (all traced fields); at "
                   "the first loop top with the line in phase 1 a breakpoint on the vsync wait (0x1AAFF0) "
                   "is armed each iteration, and the capture is the wait whose message block +0x68 (present "
                   "ticks) equals the v2.6.3 reference's value, the moment the v2.6.3 slot was saved",
            "rows_replayed": len(r.rows), "rows_differ_from_recorded": mismatches,
            "tick": {"iteration_counter": found["counter"], "beat_frame_of_iteration_loop_top": found["f_loop_top"],
                     "beat_frame_of_next_loop_top": found["f_loop_top"] + 1},
            "message_block": {"mode_phase_line": [hex(v) for v in struct.unpack_from("<3I", wee, MSG_BLOCK)],
                              "present_ticks_0x68": struct.unpack_from("<i", wee, MSG_BLOCK + 0x68)[0]},
        },
        "phase": {"wait": wb["facts"], "top": tb["facts"]},
        "self_check": {
            "displayed_at_wait_equals_draw_at_top": bool((wb["disp"] == tb["draw"]).all()),
            "draw_at_wait_vs_displayed_at_top": diff_box(wb["draw"], tb["disp"]),
            "draw_at_wait_note": "the field being drawn in the capture iteration, as it stands at the vsync wait; "
                                 "the rest of the frame's GS writes land before the vsync ISR",
            "fork_gs_field_png_is_top_displayed": None},
        "outputs": {
            "displayed.bin / displayed.png": "the 512x224 displayed field (DISPFB2) at the loop top after the "
                                             "capture iteration: the field drawn in it, the line's fifth presentation",
            "draw.bin / draw.png": "the drawing buffer (FRAME) at that loop top, as fb2's draw.bin: the previous "
                                   "field (the fourth presentation; equal to the displayed field at the wait)",
            "wait/, top/": "the two save states (state.p2s, eeMemory.bin, scratchpad.bin, gs.bin, original.png = "
                           "the fork's gs_field at that moment, snapshot.json) and their decoded buffers",
            "format": "512x224 PSMCT32 words, raster order, little-endian (R,G,B,A bytes); png alpha opaque",
        },
        "legacy_v263": legacy,
    }
    shot = cp.read_png_rgba(out / "top" / "original.png")
    if shot.shape == (H, W, 3):
        doc["self_check"]["fork_gs_field_png_is_top_displayed"] = bool((shot == rgb(tb["disp"])).all())
    write_json(out / "manifest.json", doc)
    write_json(PIX / "messages/manifest.json", {
        "what": "message frames re-recorded on the fork (fork_pixel_refs.py message <name>)",
        "messages": {p.name: str((p / "manifest.json").relative_to(ROOT))
                     for p in sorted((PIX / "messages").iterdir()) if (p / "manifest.json").exists()}})
    print(json.dumps({k: doc[k] for k in ("alignment", "phase", "self_check")}, indent=1)[:4000])
    if legacy:
        print(json.dumps({k: legacy[k] for k in legacy if k != "note"}, indent=1))
    return doc


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
    m = sub.add_parser("message")
    m.add_argument("names", nargs="*", help="message frames (default: all): " + ", ".join(MESSAGES))
    a = ap.parse_args()
    if a.cmd == "message":
        for name in a.names or list(MESSAGES):
            if name not in MESSAGES:
                raise SystemExit(f"unknown message {name!r}; known: {', '.join(MESSAGES)}")
            message(name)
        return
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
