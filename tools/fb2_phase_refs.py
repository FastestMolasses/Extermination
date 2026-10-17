#!/usr/bin/env python3
"""fb2_phase_refs.py - fb2 points on the PCSX2 fork at the other field phase.

Why.  The fb2 frame points (build/fork_refs/pixels/fb2, docs/CAPTURES_C7.md
5b) are drawn by the original with the frame index D_00810E80 (the drawing
buffer) and the field D_00810E88 (the half-line offset, OFY 1936.0 / 1936.5)
that its run had at that tick.  Neither is game state: the buffer follows the
main-loop iteration count, the half line the vsync count, and at loads both
follow disc timing (docs/PCSX2_FORK_GS_DIFF.md section 5).  The port runs the
disc at host speed, so at some points it draws the same tick on the other
phase (the port's GS_EXACT.md 10.3: first_control, 10, 11, 12 both other;
15_level_exit the half line only).  This tool records, on the fork, the same
game state with the port's phase, as a sibling set of the fb2 points.

How (no memory is written; the phase comes from the original's own timing):
  scan     cold-boot variants from fork-states slot01_title (fork_states.py's
           opening pass: title-driver delay d, New Game movie skip vsync k),
           each scored at first control against v2.6.3 slot 04 with every
           compared span (rand() state and player clock included); prints and
           stores the frame index and field each gives.
  origin   save one variant's first-control loop top (score 0) as a chain
           origin: _origin/<variant>/slot04_first_control/.
  chain    replay the canonical chain build/fork_refs/s87/route beat by beat
           from that origin, open loop: each beat in its own session from the
           previous beat's end state (as the canonical chain was made), the
           same lead-in, the same recorded inputs on the same frames (with the
           same v2.6.3 pad corrections), the same tail; every row is compared
           with the canonical row (all fields but the time keys).
  capture  the fb2 points from the variant chain with c7cap_partb's own fb2
           capture and decode (same lead, same two steps, same layout), into
           build/fork_refs/pixels/fb2_phase/<point>/.
  manifest compare each point with the primary fb2 point: game rows, rand()
           state, phase, pixels; write fb2_phase/manifest.json.

Outputs are git-ignored and disc-derived (build/fork_refs/pixels/fb2_phase/).
Runs are hidden, through ForkSession (the run lock, owned sessions), and leave
no emulator running.  No original code or data is embedded; addresses only.

    .venv/bin/python tools/fb2_phase_refs.py scan --variants d4,d5,k60
    .venv/bin/python tools/fb2_phase_refs.py origin --variant d5
    .venv/bin/python tools/fb2_phase_refs.py chain --variant d5 --beats 01,...,12
    .venv/bin/python tools/fb2_phase_refs.py capture --variant d5 --points first_control,10_cage_roof_roger
    .venv/bin/python tools/fb2_phase_refs.py manifest
    .venv/bin/python tools/fb2_phase_refs.py capture --variant d4 --points 08_truck_crossing,13_east_tower,14_roger_encounter --out build/fork_refs/pixels/fb2_phase_d4b_1016
    .venv/bin/python tools/fb2_phase_refs.py manifest --root build/fork_refs/pixels/fb2_phase_d4b_1016 --port-phase press15
"""
from __future__ import annotations

import argparse
import json
import shutil
import struct
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import pcsx2_session  # noqa: E402
import route_capture as rc  # noqa: E402
import fork_states as fs  # noqa: E402

OUTROOT = ROOT / "build/fork_refs/pixels/fb2_phase"
ORIGIN = OUTROOT / "_origin"
CHAIN = OUTROOT / "_chain"
LOGS = OUTROOT / "logs"
CANON = ROOT / "build/fork_refs/s87/route"          # the canonical chain
PRIMARY = ROOT / "build/fork_refs/pixels/fb2"        # the primary fb2 points
RAND = 0x2426C8


def parse_variant(v: str) -> tuple[int, int | None]:
    """'d5' = title delay 5, movie played; 'k60' = delay 0, movie skipped at
    vsync 60 after its driver starts; 'd3k60' both."""
    d, k = 0, None
    s = v
    if s.startswith("d"):
        s = s[1:]
        num = ""
        while s and s[0].isdigit():
            num, s = num + s[0], s[1:]
        d = int(num)
    if s.startswith("k"):
        k = int(s[1:])
    elif s:
        raise ValueError(v)
    return d, k


# ---------------------------------------------------------------------------
# scan / origin

def cmd_scan(a) -> None:
    out = ORIGIN / "scan.json"
    doc = json.loads(out.read_text()) if out.exists() else {"variants": {}}
    for v in a.variants.split(","):
        d, k = parse_variant(v)
        t0 = time.monotonic()
        r = fs.opening_pass(LOGS / "scan" / v, None, press_delay=d, skip_at=k)
        e = {"press_delay": d, "skip_at": k, "seconds": round(time.monotonic() - t0, 1),
             "marks": r["marks"],
             "points": {key: {"tick": r["best"][key][1], "score": r["best"][key][0],
                              "spans": r["best"][key][2], "phase": r["phase"].get(key)}
                        for key, _ in fs.BOOT_TARGETS}}
        doc["variants"][v] = e
        p4 = e["points"]["slot04_first_control"]
        print(v, "04:", p4["score"], p4["phase"], "commit", r["marks"].get("commit"), e["seconds"], "s", flush=True)
        ORIGIN.mkdir(parents=True, exist_ok=True)
        out.write_text(json.dumps(doc, indent=1) + "\n")


def cmd_origin(a) -> None:
    scan = json.loads((ORIGIN / "scan.json").read_text())["variants"][a.variant]
    p4 = scan["points"]["slot04_first_control"]
    if p4["score"] != 0:
        raise SystemExit(f"{a.variant}: first control scores {p4['score']} ({p4['spans']}), not 0")
    d, k = scan["press_delay"], scan["skip_at"]
    dest = ORIGIN / a.variant
    old = fs.FORK_STATES
    fs.FORK_STATES = dest                   # save_point writes FORK_STATES/<key>: never the fork-states tree
    try:
        fs.opening_pass(LOGS / "origin" / a.variant, {"slot04_first_control": p4["tick"]},
                        press_delay=d, prefix="", skip_at=k)
    finally:
        fs.FORK_STATES = old
    st = dest / "slot04_first_control"
    meta = json.loads((st / "fork_state.json").read_text())
    ph = fs.legacy_phase(st / "state.p2s")
    meta["variant"] = a.variant
    meta["phase_read"] = ph
    (st / "fork_state.json").write_text(json.dumps(meta, indent=1) + "\n")
    print("origin", a.variant, "score", meta.get("score"), "phase", ph, flush=True)


# ---------------------------------------------------------------------------
# chain: open-loop replay of the canonical beats from a variant origin

# Row keys that measure time, not game state (route_lanes.py compare's set).
TIME_KEYS = {"counter", "f", "fi", "fld", "vs", "vsync"}


def canon_trace(beat: str) -> dict:
    return json.loads((CANON / beat / "trace.json").read_text())


def chain_dir(variant: str) -> Path:
    return CHAIN / variant / "s87/route"


def variant_source(variant: str, source: str) -> Path:
    if source == "04":
        return ORIGIN / variant / "slot04_first_control/state.p2s"
    return chain_dir(variant) / source / "state.p2s"


def replay_policy(t: dict):
    """A Route policy that sets the canonical beat's recorded inputs and
    teleports on their frames, for the canonical frame count (tail included)."""
    by_f: dict[int, list[dict]] = {}
    for x in t["inputs"]:
        by_f.setdefault(x["f"], []).append(x)
    tps: dict[int, list[dict]] = {}
    for x in t.get("teleports", []):
        tps.setdefault(x["f"], []).append(x)
    frames = t["frames"]

    def fn(r: rc.Route) -> dict:
        for f in range(frames):
            for x in by_f.get(f, []):
                r.set_pad(x["buttons"], x["lx"], x["ly"])
            for x in tps.get(f, []):
                r.teleport(*x["position"], x["reason"])
            r.step(1)
        return {}          # inputs at f = frames: Route.save's set_pad(0), repeated by run_replay
    return fn


def row_diff(a: dict, b: dict) -> list[str]:
    return sorted(k for k in set(a) | set(b) if k not in TIME_KEYS and a.get(k) != b.get(k))


def compare_rows(alt: list[dict], can: list[dict]) -> dict:
    keys: dict[str, int] = {}
    first = None
    for i, (x, y) in enumerate(zip(alt, can)):
        d = row_diff(x, y)
        for k in d:
            keys[k] = keys.get(k, 0) + 1
        if d and first is None:
            first = {"row": i, "keys": d}
    phase = [(x.get("fi"), x.get("fld"), y.get("fi"), y.get("fld")) for x, y in zip(alt, can)]
    return {"rows": [len(alt), len(can)], "equal": not keys and len(alt) == len(can),
            "keys": keys, "first_difference": first,
            "phase_alt_vs_canonical": sorted({f"fi {p[0]}/{p[2]} fld {p[1]}/{p[3]}" for p in phase})}


def run_replay(variant: str, beat: str) -> dict:
    t = canon_trace(beat)
    src = variant_source(variant, t["source"])
    if not src.exists():
        raise FileNotFoundError(f"{beat}: source {src} (run its source beat first)")
    out = chain_dir(variant) / beat
    if out.exists():
        shutil.rmtree(out)
    out.mkdir(parents=True)
    lead = t["lead_in_frames"]
    pad_fix = [{k: v for k, v in c.items() if k != "applied"} for c in (t.get("v263_pad_corrections") or [])] or None
    with rc.open_session(src, log_dir=OUTROOT / "logs/chain" / variant / beat) as s:
        if lead:
            s.step(lead)                     # the pad as the session left it, as run_beat does
        r = rc.Route(s)
        r.pad_fix = pad_fix
        r.begin()
        if beat == "15_level_exit":              # beat 15's own sampler and FMV-frame timeout
            rc.use_exit_sampler(r)
            s.boundary_timeout = 900.0
        replay_policy(t)(r)
        r.set_pad(0)
        info = s.snapshot(out)
        doc = {"beat": beat, "variant": variant, "source": t["source"], "source_state": str(src),
               "lead_in_frames": lead, "tail_idle_frames": t.get("tail_idle_frames"),
               "canonical_trace": str((CANON / beat / "trace.json").relative_to(ROOT)),
               "v263_pad_corrections": r.pad_fix, "frames": r.frame_index,
               "first_counter": r.rows[0]["counter"], "last_counter": r.rows[-1]["counter"],
               "inputs": r.inputs, "teleports": r.teleports, "snapshot": info, "rows": r.rows}
    cmp = compare_rows(r.rows, t["rows"])
    cmp["inputs_equal"] = r.inputs == t["inputs"]
    ee = (out / "eeMemory.bin").read_bytes()
    cmp["rand_end"] = {"alt": hex(struct.unpack_from("<I", ee, RAND)[0]),
                       "canonical": hex(struct.unpack_from("<I", (CANON / beat / "eeMemory.bin").read_bytes(), RAND)[0])}
    cmp["rand_end"]["equal"] = cmp["rand_end"]["alt"] == cmp["rand_end"]["canonical"]
    doc["compare_canonical"] = cmp
    (out / "trace.json").write_text(json.dumps(doc, separators=(",", ":")) + "\n")
    ok = rc.resumes(out / "state.p2s", OUTROOT / "logs/chain" / variant / (beat + "_check"))
    cmp["resumes"] = bool(ok)
    (out / "compare.json").write_text(json.dumps(cmp, indent=1) + "\n")
    print(beat, variant, "rows", cmp["rows"], "equal", cmp["equal"], "inputs", cmp["inputs_equal"],
          "rand", cmp["rand_end"]["equal"], "phase", cmp["phase_alt_vs_canonical"], "first", cmp["first_difference"],
          "resumes", ok, flush=True)
    return cmp


def cmd_chain(a) -> None:
    pcsx2_session.FORK_MTVU = False          # as route_capture's default (--fork-mtvu off)
    rc.use_fork("phase", True)
    for beat in a.beats.split(","):
        run_replay(a.variant, beat)


# ---------------------------------------------------------------------------
# capture: c7cap_partb's fb2 capture and decode from the variant chain

# The port's field at s1 of each point (the port's docs/GS_EXACT.md 10.3, the
# harness's field_phase): (drawing FRAME FBP, OFY).
PORT_PHASE = {
    "first_control": ("0x38", 1936.5),
    "10_cage_roof_roger": ("0x38", 1936.5),
    "11_crevice_prompt": ("0x0", 1936.0),
    "12_crevice_jump": ("0x38", 1936.5),
    "15_level_exit": ("0x0", 1936.0),
}
# Since the port's fixtures press New Game at title menu tick 15 (port
# STARTUP.md; 2026-10-16), the New Game movie enters on D_00810E80 = 1 as in
# the canonical run and the port's field at every point is the other one:
# the d4 chain's.  The second set (manifest --root <folder> --port-phase
# press15) records 08, 13 and 14 there.
PORT_PHASE_PRESS15 = {
    "first_control": ("0x0", 1936.0),
    "08_truck_crossing": ("0x38", 1936.5),
    "10_cage_roof_roger": ("0x0", 1936.0),
    "11_crevice_prompt": ("0x38", 1936.5),
    "12_crevice_jump": ("0x0", 1936.0),
    "13_east_tower": ("0x38", 1936.5),
    "14_roger_encounter": ("0x0", 1936.0),
    "15_level_exit": ("0x38", 1936.5),
}
PORT_PHASES = {"press14": PORT_PHASE, "press15": PORT_PHASE_PRESS15}
POINT_SOURCE = {"first_control": "04"}


def cmd_capture(a) -> None:
    import c7cap_partb as cp
    pcsx2_session.FORK_MTVU = False          # the primary fb2 set's setting (VU1 on the EE thread)
    rc.use_fork("phase", True)
    origin = ORIGIN / a.variant / "slot04_first_control/state.p2s"
    real_slot_start = rc.slot_start

    def slot_start(slot: str):
        if slot != "04":
            raise KeyError(slot)
        return origin, real_slot_start(slot)[1]      # the variant origin, the same lead correction
    rc.slot_start = slot_start
    cp.ROUTE = chain_dir(a.variant)
    out_root = Path(a.out).resolve() if a.out else OUTROOT
    cp.FB2 = out_root
    cp.FB2_LEAD = "legacy"
    labels = a.points.split(",")
    cp.FB2_POINTS = [(lb, POINT_SOURCE.get(lb, lb)) for lb in labels]
    try:
        cp.fb2_capture(labels, a.steps)
        for lb in labels:
            cp.fb2_decode(lb)
            c = json.loads((out_root / lb / "capture.json").read_text())
            c["variant"] = a.variant
            c["variant_origin"] = str(origin.relative_to(ROOT))
            c["variant_route"] = str(chain_dir(a.variant).relative_to(ROOT))
            (out_root / lb / "capture.json").write_text(json.dumps(c, indent=1) + "\n")
    finally:
        rc.slot_start = real_slot_start
        print("no emulator process left:", cp.no_emulator_left(), flush=True)


# ---------------------------------------------------------------------------
# manifest: each point against the primary fb2 point

CAP_SKIP = TIME_KEYS | {"hw", "k", "inside_step", "snapshot"}


def tops(meta: dict) -> dict:
    out = {}
    for x in meta["snapshots"]:
        if x["at"] in ("s0", "s1", "s2"):
            out[x["at"]] = {"counter": x["counter"], "vsync": x["vsync"], "csr_field": x["csr_field"],
                            "dispfb2_fbp": x["dispfb2_fbp"], "frame_fbp": x["frame_fbp"][0], "ofy": x["ofy"][0],
                            "frame_index_810E80": int(x["hw"]["frame_idx_810E80"], 16),
                            "field_810E88": int(x["hw"]["field_810E88"], 16)}
    return out


def rand_at(folder: Path) -> str:
    from parse_pcsx2_state import extract_zstd_entry
    ee = extract_zstd_entry(folder / "state.p2s", "eeMemory.bin")
    return hex(struct.unpack_from("<I", ee, RAND)[0])


def pixel_diff(a: Path, b: Path, z: bool = False) -> dict:
    import numpy as np
    x = np.frombuffer(a.read_bytes(), "<u4")
    y = np.frombuffer(b.read_bytes(), "<u4")
    if z:
        return {"bit_exact": bool((x == y).all()), "words_differ": int((x != y).sum())}
    rgb = (x & 0xFFFFFF) != (y & 0xFFFFFF)
    return {"bit_exact": bool((x == y).all()), "rgb_pixels_differ": int(rgb.sum()),
            "alpha_pixels_differ": int(((x >> 24) != (y >> 24)).sum())}


def point_entry(label: str, root: Path = OUTROOT, port_phase: dict = PORT_PHASE) -> dict:
    d = root / label
    cap = json.loads((d / "capture.json").read_text())
    meta = json.loads((d / "meta.json").read_text())
    pcap = json.loads((PRIMARY / label / "capture.json").read_text())
    pmeta = json.loads((PRIMARY / label / "meta.json").read_text())
    variant = cap["variant"]
    rows = []
    for x, y in zip(cap["rows"], pcap["rows"]):
        diff = sorted(k for k in set(x) | set(y) if k not in CAP_SKIP and x.get(k) != y.get(k))
        rows.append({"k": x["k"], "counter": [x["counter"], y["counter"]], "vsync": [x["vsync"], y["vsync"]],
                     "fields_equal": not diff, "differing": diff})
    t_alt, t_pri = tops(meta), tops(pmeta)
    want = port_phase.get(label)
    s1 = t_alt["s1"]
    phase = {
        "port_s1": {"frame_fbp": want[0], "ofy": want[1]} if want else None,
        "phase_s1": {"frame_fbp": s1["frame_fbp"], "ofy": s1["ofy"]},
        "primary_s1": {"frame_fbp": t_pri["s1"]["frame_fbp"], "ofy": t_pri["s1"]["ofy"]},
        "equals_port": bool(want) and (s1["frame_fbp"], s1["ofy"]) == want,
        "half_line_equals_port": bool(want) and s1["ofy"] == want[1],
        "buffer_equals_port": bool(want) and s1["frame_fbp"] == want[0],
        "frame_index_differs_from_primary": [t_alt[k]["frame_index_810E80"] != t_pri[k]["frame_index_810E80"] for k in ("s0", "s1", "s2")],
        "field_differs_from_primary": [t_alt[k]["field_810E88"] != t_pri[k]["field_810E88"] for k in ("s0", "s1", "s2")],
    }
    chain = {}
    for c in sorted(chain_dir(variant).glob("*/compare.json")):
        cc = json.loads(c.read_text())
        chain[c.parent.name] = {"rows": cc["rows"], "equal": cc["equal"], "inputs_equal": cc["inputs_equal"],
                                "rand_end_equal": cc["rand_end"]["equal"], "resumes": cc.get("resumes"),
                                "phase_alt_vs_canonical": cc["phase_alt_vs_canonical"],
                                "first_difference": cc["first_difference"]}
    scan = json.loads((ORIGIN / "scan.json").read_text())["variants"][variant]
    origin_meta = json.loads((ORIGIN / variant / "slot04_first_control/fork_state.json").read_text())
    rand = {"phase": rand_at(d / "s2"), "primary": rand_at(PRIMARY / label / "s2")}
    rand["equal"] = rand["phase"] == rand["primary"]
    return {
        "variant": variant,
        "origin": {"state": cap["variant_origin"], "press_delay": scan["press_delay"], "skip_at": scan["skip_at"],
                   "score_vs_v263_slot04": origin_meta.get("score"), "score_spans": origin_meta.get("score_spans"),
                   "phase": origin_meta.get("phase_read"), "how": origin_meta.get("how")},
        "source": cap["source"], "source_state": cap["source_state"],
        "lead_frames": cap.get("fork", {}).get("lead_frames"),
        "fork_build": {k: cap.get("fork", {}).get("hello", {}).get(k) for k in ("rev", "hash")},
        "chain_vs_canonical": chain,
        "rows_vs_primary": rows,
        "rows_equal": all(r["fields_equal"] for r in rows) and len(rows) == 3,
        "rand_state_s2": rand,
        "tops": t_alt, "primary_tops": t_pri,
        "phase": phase,
        "pixels_vs_primary": {"displayed": pixel_diff(d / "displayed.bin", PRIMARY / label / "displayed.bin"),
                              "draw": pixel_diff(d / "draw.bin", PRIMARY / label / "draw.bin"),
                              "z": pixel_diff(d / "z.bin", PRIMARY / label / "z.bin", z=True)},
    }


def cmd_manifest(a) -> None:
    root = Path(a.root).resolve() if a.root else OUTROOT
    port_phase = PORT_PHASES[a.port_phase]
    path = root / "manifest.json"
    m = json.loads(path.read_text()) if path.exists() else {}
    m["what"] = ("fb2 points re-recorded on the PCSX2 fork at the port's field phase: the same game state as the "
                 "primary fb2 point (build/fork_refs/pixels/fb2), drawn with the port's drawing buffer and half "
                 "line (the port's GS_EXACT.md 10.3).  Same layout as the primary set.  tools/fb2_phase_refs.py")
    if root != OUTROOT:
        m["what"] += ("  A second set: the points whose port phase is the d4 chain's once the port's fixtures "
                      "press New Game at title menu tick 15 (port_phase " + a.port_phase + "); the cross-checks "
                      "(checks) stay in " + str(OUTROOT.relative_to(ROOT)) + "/manifest.json.")
    m["port_phase"] = a.port_phase
    m["how"] = A_HOW
    m["recorded"] = time.strftime("%Y-%m-%d %H:%M:%S")
    m["fork"] = {"repo": str(pcsx2_session.FORK_REPO), "repo_head": fs.fork_head()}
    m.setdefault("points", {})
    m["findings"] = FINDINGS
    m["checks"] = checks() if root == OUTROOT else {}
    for label in (a.points.split(",") if a.points else sorted(p.name for p in root.iterdir()
                                                                if (p / "capture.json").exists())):
        m["points"][label] = point_entry(label, root, port_phase)
        e = m["points"][label]
        print(label, "variant", e["variant"], "rows_equal", e["rows_equal"], "rand", e["rand_state_s2"]["equal"],
              "s1", e["phase"]["phase_s1"], "port", e["phase"]["port_s1"], "equals_port", e["phase"]["equals_port"],
              flush=True)
    path.write_text(json.dumps(m, indent=1) + "\n")


def checks() -> dict:
    """Cross-checks kept under _check/: (1) the d7 chain's point 15 has the
    primary's phase and must equal the primary bit for bit; (2) the d7 and d4
    chains reach points 10 and 12 on the same half line in the other buffer."""
    out = {}
    c15 = OUTROOT / "_check/d7/15_level_exit"
    if (c15 / "meta.json").exists():
        out["15_level_exit_d7_vs_primary"] = {
            "what": "d7 chain (another boot, field-only flip at first control) at point 15, where its phase "
                    "equals the primary's: same game state + same phase must give the same pixels",
            "s1": tops(json.loads((c15 / "meta.json").read_text()))["s1"],
            "primary_s1": tops(json.loads((PRIMARY / "15_level_exit/meta.json").read_text()))["s1"],
            "rand_s2": [rand_at(c15 / "s2"), rand_at(PRIMARY / "15_level_exit/s2")],
            **{n.split(".")[0]: pixel_diff(c15 / n, PRIMARY / "15_level_exit" / n, n == "z.bin")
               for n in ("displayed.bin", "draw.bin", "z.bin")}}
    for lb in ("10_cage_roof_roger", "12_crevice_jump"):
        a, b = OUTROOT / "_check/d7" / lb, OUTROOT / lb
        if (a / "meta.json").exists() and (b / "meta.json").exists():
            out[f"{lb}_d7_vs_d4"] = {
                "what": "the same tick on the same half line (OFY 1936.5) drawn into the other buffer (d7: 0x0, "
                        "d4: 0x38): does the buffer address change a pixel?",
                "d7_s1": tops(json.loads((a / "meta.json").read_text()))["s1"],
                "d4_s1": tops(json.loads((b / "meta.json").read_text()))["s1"],
                "rand_s2": [rand_at(a / "s2"), rand_at(b / "s2")],
                **{n.split(".")[0]: pixel_diff(a / n, b / n, n == "z.bin") for n in ("displayed.bin", "draw.bin", "z.bin")}}
    fmv = {}
    for v in ["canonical"] + sorted(p.name for p in CHAIN.iterdir() if p.is_dir()):
        t = (CANON if v == "canonical" else chain_dir(v)) / "15_level_exit/trace.json"
        if not t.exists():
            continue
        rows = json.loads(t.read_text())["rows"]
        i = next(i for i in range(1, len(rows)) if rows[i]["vs"] - rows[i - 1]["vs"] > 1)
        fmv[v] = {"row": i, "before": {"fi": rows[i - 1]["fi"], "fld": rows[i - 1]["fld"], "vs": rows[i - 1]["vs"]},
                  "after": {"fi": rows[i]["fi"], "fld": rows[i]["fld"], "vs": rows[i]["vs"]},
                  "vsyncs": rows[i]["vs"] - rows[i - 1]["vs"],
                  "end": {"fi": rows[-1]["fi"], "fld": rows[-1]["fld"]}}
    out["beat15_departure_fmv_frame"] = fmv
    return out


FINDINGS = [
    "first_control, 10, 11, 12: the d4 boot (title driver held back 4 ticks, movie played) reaches first control "
    "with every compared span equal (score 0, rand() and player clock included) one iteration and one vsync off "
    "the canonical boot's phase; the replayed chain equals the canonical one on every row of beats 01..08, 10..15 "
    "(every field but counter/f/fi/fld/vs; inputs and end rand() equal) with fi and fld both flipped on every "
    "row, so each point's s0..s2 rows equal the primary's and its s1 drawing buffer and OFY are the port's.",
    "15_level_exit: no fork run reaches the port's phase there (buffer 0x0 with OFY 1936.0).  Roger's "
    "departure movie E001 plays inside one main-loop iteration (beat 15 row 442).  The four chains "
    "(canonical, d4, d7, d8; checks.beat15_departure_fmv_frame) all left it with fi == fld, but that is not a "
    "rule of the code (corrected 2026-10-16, fork probes build/fork_refs/probes_1016/movie_end/, decomp "
    "FINDINGS.md 'The field D_00810E88 across the main loop, movies and empty lists'): the code ends every "
    "movie on field 1 at host speed (the gate opens on a field-0 vblank, field-0/field-1 send pairs, two "
    "closing waits returning field 0 then 1), so fi == fld after it only for an entry on fi 0.  Canonical, d7 "
    "and d8 entered on fi 0 and left (1, 1); d4 entered on fi 1 and left (0, 0) only because the emulator's "
    "teardown SIF RPC (00203C90 -> 00113CD0) spanned a vblank and moved both closing waits one field (IOP "
    "timing, not reproduced); at host speed d4 leaves (0, 1), fi != fld.  Every later iteration takes one "
    "vsync.  v2.6.3's s1 (0x0, 1936.0) came from a 2-vsync iteration of its capture session (host timing).  "
    "The port's field after that movie is its hard-coded relation (port side: FIRST_LEVEL_AUDIT F1, the FIELD "
    "step).",
    "15_level_exit here is the d4 chain's point: the same game state on the port's half line (OFY 1936.0) in "
    "the other buffer (0x38, the port's 0x0).  At equal tick and half line the buffer does not change a pixel: "
    "checks *_d7_vs_d4 (points 10 and 12, buffers 0x0 and 0x38, OFY 1936.5) are bit-exact in displayed, draw "
    "and z.  That check is in AREA11; for AREA01 it is an inference.",
    "Validation of the method: the d7 chain reaches point 15 on the primary's phase from another boot, and its "
    "capture equals the primary bit for bit (checks.15_level_exit_d7_vs_primary).",
]


A_HOW = (
    "Phase without memory writes: a cold-boot variant of the canonical boot (fork_states.py's opening pass from "
    "fork-states slot01_title; the title driver's Cross presses held back d ticks, or the New Game movie skipped "
    "at vsync k) reaches first control with every compared span of v2.6.3 slot 04 equal (score 0: task slots, "
    "fade, letterbox, selectors, player actor with its clock, camera, UI, story, inventory, owner nodes and the "
    "rand() state) but after a different number of main-loop iterations and vsyncs before the AREA11 load ends "
    "(the load absorbs the delay; docs/PCSX2_FORK_GS_DIFF.md 5.2), so the frame index and field differ.  From "
    "that origin the canonical chain's beats are replayed open loop, each in its own session from the previous "
    "beat's end state, with the canonical lead-in, inputs on the same frames, pad corrections and tail; every "
    "beat's rows are compared with the canonical rows (all fields but counter, f, fi, fld, vs) and the end "
    "rand() state.  The fb2 point is then captured as the primary one (c7cap_partb fb2: the same lead, s0, "
    "two steps with the vsync-wait and vsync-ISR syncs, the same decode).")


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("scan")
    p.add_argument("--variants", required=True)
    p = sub.add_parser("origin")
    p.add_argument("--variant", required=True)
    p = sub.add_parser("chain")
    p.add_argument("--variant", required=True)
    p.add_argument("--beats", required=True)
    p = sub.add_parser("capture")
    p.add_argument("--variant", required=True)
    p.add_argument("--points", required=True)
    p.add_argument("--steps", type=int, default=2)
    p.add_argument("--out", default="", help="output folder (default build/fork_refs/pixels/fb2_phase)")
    p = sub.add_parser("manifest")
    p.add_argument("--points", default="")
    p.add_argument("--root", default="", help="the set's folder (default build/fork_refs/pixels/fb2_phase)")
    p.add_argument("--port-phase", default="press14", choices=sorted(PORT_PHASES),
                   help="the port's field per point the set is recorded for (press14: the first set's)")
    a = ap.parse_args()
    {"scan": cmd_scan, "origin": cmd_origin, "chain": cmd_chain, "capture": cmd_capture,
     "manifest": cmd_manifest}[a.cmd](a)


if __name__ == "__main__":
    import signal
    signal.signal(signal.SIGTERM, rc._term_to_interrupt)
    main()
