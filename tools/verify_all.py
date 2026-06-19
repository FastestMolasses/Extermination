#!/usr/bin/env python3
"""verify_all.py — one-command regression check for the Extermination decomp.

Runs the full verification suite and prints a per-stage PASS/FAIL table with a
single overall exit code (0 = all passed, 1 = any failure). This is the
"did I break anything?" guard — run it after any change to the build pipeline,
the symbol map, or the asset tools.

Stages (each can be skipped with a flag):

  boot-elf   [container]  fill_unmatched + link.py, assert the rebuilt boot ELF
                          is byte-identical to the original (0x175b00 loadable
                          bytes).
  overlays   [container]  build all 19 overlays, assert 19/19 byte-identical.
  match      [host]       objdiff report; assert matched_code_percent has not
                          regressed below a floor (default 95.0).
  gltf       [host]       run the character + level glTF exporters on a sample,
                          assert pygltflib round-trips the output (skipped if
                          pygltflib or the sample assets are absent).
  selftest   [host]       run the pure-Python decoder self-tests
                          (anim_decoder).
  gs-offset  [host]       cross-validate the GS freeze-blob VRAM base between
                          gs_vram.py (formula: len - 4MB - 84) and
                          clut_pair.py (palette-scored detection) on a real
                          PCSX2 save state (skipped if none present).

The container stages use Apple's `container` CLI. They are the slow part; pass
--no-container to skip them and run only the host-side checks (fast).

Usage:
    .venv/bin/python tools/verify_all.py
    .venv/bin/python tools/verify_all.py --no-container      # host checks only
    .venv/bin/python tools/verify_all.py --only match,selftest
    .venv/bin/python tools/verify_all.py --match-floor 96.0

This script reads only project files plus the user-supplied, git-ignored boot
ELF / extracted assets. It writes nothing to the repo (build artifacts only).
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]  # tools/verify_all.py -> repo root
IMAGE = "exterm-permuter"  # superset of exterm-toolchain (+ i386 libs for ee-gcc, permuter deps)
OBJDIFF_CLI = ROOT / "tools" / "bin" / "objdiff-cli"
VENV_PY = Path(sys.executable)

# ANSI (only if stdout is a tty).
_TTY = sys.stdout.isatty()
def _c(code: str, s: str) -> str:
    return f"\033[{code}m{s}\033[0m" if _TTY else s
GREEN = lambda s: _c("32", s)
RED   = lambda s: _c("31", s)
DIM   = lambda s: _c("2", s)


class Stage:
    def __init__(self, name: str, container: bool):
        self.name = name
        self.container = container
        self.ok: bool | None = None
        self.detail = ""
        self.secs = 0.0


def container_sh(script: str, timeout: int = 900) -> subprocess.CompletedProcess:
    """Run a shell script inside the toolchain container, repo bind-mounted."""
    return subprocess.run(
        ["container", "run", "--rm", "-v", f"{ROOT}:/work", "-w", "/work",
         IMAGE, "sh", "-c", script],
        capture_output=True, text=True, timeout=timeout,
    )


# ---------------------------------------------------------------- boot-elf

def run_boot_elf(st: Stage) -> None:
    """fill + link inside the container; assert byte-identical loadable region.

    Uses --jobs 1 + --no-fill split because the bind mount intermittently hits
    EDEADLK under the default parallel copy (documented in PROGRESS.md).
    """
    # Retry the fill once on the known intermittent EDEADLK.
    for attempt in range(2):
        r = container_sh(
            "python3 tools/decomp/fill_unmatched.py --clean --jobs 1 2>&1 | tail -2")
        if "Resource deadlock" not in r.stdout:
            break
        time.sleep(1)
    r = container_sh(
        "python3 tools/decomp/link.py --no-fill 2>&1 | grep -E '(PASS|FAIL|verify)'")
    out = r.stdout
    if "PASS — 0x175b00 loadable bytes are identical" in out:
        st.ok = True
        st.detail = "boot ELF byte-identical (0x175b00 loadable bytes)"
    else:
        st.ok = False
        st.detail = (out.strip().splitlines() or ["link produced no verify line"])[-1]


# ---------------------------------------------------------------- overlays

def run_overlays(st: Stage) -> None:
    r = container_sh(
        "python3 tools/overlay/build.py --all --no-extract --no-yaml --no-splat "
        "2>&1 | tail -25")
    out = r.stdout
    # Expect a final "N/19 overlays passed" line.
    passed = None
    for line in out.splitlines():
        line = line.strip()
        if line.endswith("overlays passed"):
            passed = line
    if passed and passed.startswith("19/19"):
        st.ok = True
        st.detail = passed
    else:
        st.ok = False
        st.detail = passed or "no overlay summary line found"


# ---------------------------------------------------------------- match

def run_match(st: Stage, floor: float) -> None:
    """objdiff report; assert matched_code_percent >= floor."""
    if not OBJDIFF_CLI.exists():
        st.ok = None
        st.detail = "skipped (objdiff-cli not in tools/bin/)"
        return
    expected = ROOT / "build" / "expected"
    obj = ROOT / "build" / "obj"
    if not (expected.is_dir() and any(expected.glob("*.o"))
            and obj.is_dir() and any(obj.glob("*.o"))):
        st.ok = None
        st.detail = "skipped (no build/expected+obj — run build.py build first)"
        return
    out_json = ROOT / "scratch" / "verify_report.json"
    out_json.parent.mkdir(exist_ok=True)
    r = subprocess.run(
        [str(OBJDIFF_CLI), "report", "generate", "-p", str(ROOT),
         "-o", str(out_json)],
        capture_output=True, text=True,
    )
    if r.returncode != 0:
        msg = (r.stderr or r.stdout)
        # A missing .o means build/expected or build/obj is stale vs the
        # current objdiff.json (e.g. after a rename + splat re-segment).
        # That's a "rebuild first" condition, not a match regression.
        if "No such file" in msg or "os error 2" in msg:
            st.ok = None
            st.detail = ("skipped (build/expected stale vs objdiff.json — "
                         "run build.py build)")
        else:
            st.ok = False
            st.detail = msg.strip().splitlines()[-1][:120]
        return
    m = json.loads(out_json.read_text())["measures"]
    pct = m.get("matched_code_percent", 0.0)
    fn = m.get("matched_functions", 0)
    tot = m.get("total_functions", 0)
    st.ok = pct >= floor
    st.detail = (f"matched_code {pct:.2f}% (floor {floor:.1f}), "
                 f"functions {fn}/{tot}")


# ---------------------------------------------------------------- gltf

def _validate_glb_structural(path: Path) -> tuple[bool, str]:
    """Built-in GLB structural validator (no pygltflib needed).

    Checks the 12-byte header (magic 'glTF', version 2, total length ==
    file size) and that the chunk table (JSON + BIN) sums to the declared
    length. Also parses the JSON chunk and reports mesh/animation/image
    counts. Returns (ok, detail).
    """
    data = path.read_bytes()
    if len(data) < 12 or data[:4] != b"glTF":
        return False, "bad GLB magic"
    import struct as _s
    version, total = _s.unpack_from("<II", data, 4)
    if version != 2:
        return False, f"GLB version {version} != 2"
    if total != len(data):
        return False, f"declared length {total} != file size {len(data)}"
    # Walk chunks.
    off = 12
    json_obj = None
    while off + 8 <= len(data):
        clen, ctype = _s.unpack_from("<II", data, off)
        off += 8
        if off + clen > len(data):
            return False, "chunk overruns file"
        if ctype == 0x4E4F534A:  # 'JSON'
            json_obj = json.loads(data[off:off + clen].decode("utf-8"))
        off += clen
    if off != len(data):
        return False, "chunk table does not sum to file length"
    if json_obj is None:
        return False, "no JSON chunk"
    return True, (f"{len(json_obj.get('meshes', []))} meshes, "
                  f"{len(json_obj.get('animations', []))} animations, "
                  f"{len(json_obj.get('images', []))} images")


def run_gltf(st: Stage) -> None:
    mesh = ROOT / "extract" / "chunk21" / "f17_id8f.bin"
    skel = ROOT / "extract" / "chunk05" / "f04_id71.bin"
    if not (mesh.exists() and skel.exists()):
        st.ok = None
        st.detail = "skipped (sample assets not extracted)"
        return
    out_glb = ROOT / "scratch" / "verify_player.glb"
    out_glb.parent.mkdir(exist_ok=True)
    r = subprocess.run(
        [str(VENV_PY), str(ROOT / "tools" / "export_gltf.py"),
         "--mesh", str(mesh), "--skel", str(skel), "--out", str(out_glb)],
        capture_output=True, text=True,
    )
    if r.returncode != 0 or not out_glb.exists():
        st.ok = False
        msg = (r.stderr or r.stdout).strip().splitlines()
        st.detail = (msg[-1][:120] if msg else "export failed")
        return
    # Prefer pygltflib (strict) if available, else built-in structural check.
    try:
        import pygltflib
        g = pygltflib.GLTF2().load_binary(str(out_glb))
        st.ok = True
        st.detail = (f"player.glb (pygltflib): {len(g.meshes)} meshes, "
                     f"{len(g.animations)} animations, {len(g.images or [])} images")
    except ImportError:
        ok, detail = _validate_glb_structural(out_glb)
        st.ok = ok
        st.detail = f"player.glb (structural): {detail}"
    except Exception as e:  # noqa: BLE001
        st.ok = False
        st.detail = f"pygltflib rejected output: {e}"[:120]


# ---------------------------------------------------------------- selftest

def run_selftest(st: Stage) -> None:
    """Run pure-Python decoder self-tests."""
    dec = ROOT / "tools" / "anim_decoder.py"
    if not dec.exists():
        st.ok = None
        st.detail = "skipped (anim_decoder.py absent)"
        return
    # anim_decoder runs its _selftest when invoked directly.
    r = subprocess.run([str(VENV_PY), str(dec), "--selftest"],
                       capture_output=True, text=True)
    st.ok = r.returncode == 0
    st.detail = ("anim_decoder self-test passed" if st.ok
                 else (r.stderr or r.stdout).strip().splitlines()[-1][:120] if (r.stderr or r.stdout) else "self-test failed")


# ---------------------------------------------------------------- gs-offset

def run_gs_offset(st: Stage) -> None:
    """Cross-validate the GS freeze-blob VRAM base on a real save state.

    Guards the 2026-06-09 correction (base = len - 0x400000 - 84, not
    len - 0x400000): gs_vram.localmem_base's fixed formula must agree with
    clut_pair.detect_vram_base's independent palette-scored detection, and
    the corrected base must strictly out-score the legacy one on
    TEX0-referenced CLUTs. Reads only the user's own save state; writes
    nothing.
    """
    sdir = Path.home() / "Library/Application Support/PCSX2/sstates"
    states = sorted(sdir.glob("*.p2s")) if sdir.is_dir() else []
    if not states:
        st.ok = None
        st.detail = "skipped (no .p2s save states in PCSX2 sstates dir)"
        return
    sys.path.insert(0, str(ROOT / "tools"))
    try:
        from parse_pcsx2_state import extract_all
        from gs_vram import (GS_LOCALMEM_SIZE, VRAM_TRAILER, characterise,
                             csm1_unswizzle_clut, localmem_base)
        from clut_pair import detect_vram_base, harvest_tex0
    except ImportError as e:
        st.ok = None
        st.detail = f"skipped (import: {e})"[:120]
        return
    import tempfile
    p2s = states[0]
    with tempfile.TemporaryDirectory(prefix="gs_offset_") as td:
        out = Path(td)
        extract_all(p2s, out)
        gs = (out / "gs.bin").read_bytes()
        ee = (out / "ee.bin").read_bytes()
        vu1 = out / "vu1_dmem.bin"
        blobs = [ee] + ([vu1.read_bytes()] if vu1.is_file() else [])
    cbps = sorted({k[4] for k in harvest_tex0(blobs)})
    if not cbps:
        st.ok = False
        st.detail = f"no TEX0 CBPs harvested from {p2s.name} — cannot validate"
        return

    def score(base: int) -> int:
        lm = gs[base:base + GS_LOCALMEM_SIZE]
        n = 0
        for c in cbps:
            i = characterise(csm1_unswizzle_clut(lm[c*256:c*256 + 1024]), c)
            if i.valid and i.a80 >= 100 and i.distinct_rgb >= 32:
                n += 1
        return n

    formula = localmem_base(gs)
    detected = detect_vram_base(gs, cbps)
    expect = len(gs) - GS_LOCALMEM_SIZE - VRAM_TRAILER
    s_new, s_old = score(expect), score(len(gs) - GS_LOCALMEM_SIZE)
    if formula == detected == expect and s_new > s_old:
        st.ok = True
        st.detail = (f"{p2s.name}: base {formula} (formula==detected), "
                     f"palette score {s_new} > legacy {s_old}")
    else:
        st.ok = False
        st.detail = (f"{p2s.name}: formula={formula} detected={detected} "
                     f"expect={expect}, score new={s_new} legacy={s_old}")


STAGE_FNS = {
    "boot-elf": (True, run_boot_elf),
    "overlays": (True, run_overlays),
    "match":    (False, None),    # needs floor arg, dispatched specially
    "gltf":     (False, run_gltf),
    "selftest": (False, run_selftest),
    "gs-offset": (False, run_gs_offset),
}


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--no-container", action="store_true",
                   help="skip the container stages (boot-elf, overlays)")
    p.add_argument("--only", help="comma-separated stage list to run")
    p.add_argument("--match-floor", type=float, default=95.0,
                   help="fail if matched_code_percent drops below this (default 95.0)")
    args = p.parse_args(argv)

    order = ["boot-elf", "overlays", "match", "gltf", "selftest", "gs-offset"]
    if args.only:
        want = {s.strip() for s in args.only.split(",")}
        order = [s for s in order if s in want]

    stages = []
    for name in order:
        is_container, _fn = STAGE_FNS[name]
        if args.no_container and is_container:
            continue
        stages.append(Stage(name, is_container))

    for st in stages:
        t0 = time.time()
        print(DIM(f"[run] {st.name} ..."), flush=True)
        try:
            if st.name == "match":
                run_match(st, args.match_floor)
            else:
                STAGE_FNS[st.name][1](st)
        except Exception as e:  # noqa: BLE001
            st.ok = False
            st.detail = f"exception: {e}"[:160]
        st.secs = time.time() - t0

    print()
    print("=" * 72)
    print("  verify_all summary")
    print("=" * 72)
    any_fail = False
    for st in stages:
        if st.ok is True:
            tag = GREEN("PASS")
        elif st.ok is None:
            tag = DIM("SKIP")
        else:
            tag = RED("FAIL")
            any_fail = True
        print(f"  {st.name:<10} {tag}  {st.detail}  {DIM(f'({st.secs:.1f}s)')}")
    print("=" * 72)
    if any_fail:
        print(RED("  RESULT: FAIL — see failing stage(s) above"))
        return 1
    print(GREEN("  RESULT: all checks passed"))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
