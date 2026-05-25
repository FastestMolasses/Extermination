#!/usr/bin/env python3
"""parse_pcsx2_state.py - Parse a PCSX2 v2 (.p2s) save state and dump the
component memory blobs (EE RAM, VU0/VU1 instr+data, IOP RAM, scratchpad,
hardware-register dumps).

A .p2s file is a regular ZIP archive in which each entry is compressed with
zstd (zipfile compression method 93). Python's standard `zipfile` does not
support method 93 (yet), so we parse the local file headers manually and feed
the compressed payload through the `zstandard` package.

Usage:
    python3 tools/parse_pcsx2_state.py <savestate.p2s> [--out DIR]

Outputs (in DIR, default = /tmp):
    ee.bin            32 MiB EE main RAM
    iop.bin            2 MiB IOP RAM
    vu0_dmem.bin       4 KiB VU0 data memory
    vu0_imem.bin       4 KiB VU0 micro memory
    vu1_dmem.bin      16 KiB VU1 data memory
    vu1_imem.bin      16 KiB VU1 micro memory
    scratchpad.bin    16 KiB EE scratchpad
    gs.bin            ~4 MiB GS local memory + state
    ... (everything else in the archive)

Also prints (when run as a script) the EE address of the first 64-byte
column-major affine matrix run that looks like a posed character skeleton
(see docs/FINDINGS.md "Live bone matrices in EE RAM").

Disc-data safety: this script ingests a user-owned save state from PCSX2 and
emits scratch files into the user's chosen output directory; it does not
write to the repo and is safe to invoke on any user machine.
"""
from __future__ import annotations

import argparse
import os
import struct
import sys
import zipfile
from pathlib import Path

try:
    import zstandard  # type: ignore
except ImportError:
    sys.exit("error: this tool needs the `zstandard` package "
             "(pip install zstandard inside .venv).")


# Canonical short-name mapping for the entries we care about. Anything not in
# this table is dumped with its raw filename (minus .bin/.dat).
SHORT_NAMES = {
    "eeMemory.bin":     "ee.bin",
    "iopMemory.bin":    "iop.bin",
    "vu0Memory.bin":    "vu0_dmem.bin",
    "vu0MicroMem.bin":  "vu0_imem.bin",
    "vu1Memory.bin":    "vu1_dmem.bin",
    "vu1MicroMem.bin":  "vu1_imem.bin",
    "Scratchpad.bin":   "scratchpad.bin",
    "GS.bin":           "gs.bin",
    "SPU2.bin":         "spu2.bin",
    "eeHwRegs.bin":     "ee_hwregs.bin",
    "iopHwRegs.bin":    "iop_hwregs.bin",
}


def extract_zstd_entry(p2s_path: Path, name: str) -> bytes:
    """Read one zstd-compressed entry out of a PCSX2 save-state ZIP."""
    with zipfile.ZipFile(p2s_path) as z:
        info = z.getinfo(name)
    with open(p2s_path, "rb") as f:
        f.seek(info.header_offset)
        lfh = f.read(30)
        sig = struct.unpack("<I", lfh[:4])[0]
        if sig != 0x04034b50:
            raise ValueError(f"bad local file header signature {sig:#x} at "
                             f"{info.header_offset:#x}")
        nlen, elen = struct.unpack("<HH", lfh[26:30])
        f.seek(info.header_offset + 30 + nlen + elen)
        comp = f.read(info.compress_size)
    dctx = zstandard.ZstdDecompressor()
    try:
        return dctx.decompress(comp, max_output_size=info.file_size)
    except zstandard.ZstdError:
        # Fall back to streaming for frames without a recorded content size.
        return dctx.stream_reader(comp).read()


def extract_all(p2s_path: Path, out_dir: Path) -> dict[str, Path]:
    """Extract every entry in the save state into `out_dir`. Returns a mapping
    of canonical short name -> output path."""
    out_dir.mkdir(parents=True, exist_ok=True)
    written: dict[str, Path] = {}
    with zipfile.ZipFile(p2s_path) as z:
        infos = list(z.infolist())
    for info in infos:
        name = info.filename
        short = SHORT_NAMES.get(name, name.replace(" ", "_"))
        out_path = out_dir / short
        if info.compress_type == 93:
            data = extract_zstd_entry(p2s_path, name)
        else:
            # Stored entries (e.g. Screenshot.png) — use the stdlib path.
            with zipfile.ZipFile(p2s_path) as z:
                data = z.read(name)
        out_path.write_bytes(data)
        written[short] = out_path
    return written


# ---------------------------------------------------------------------------
# Live bone-matrix scanner: walks EE RAM for runs of 64-byte column-major
# affine matrices (the format the engine stages in .bss before DMA'ing it to
# VU1 dmem for the per-bone skinning kernel).
# ---------------------------------------------------------------------------

def _is_colmaj_affine(buf: bytes, off: int) -> bool:
    """Column-major affine test: storage is 4 contiguous columns, each a
    qword (x, y, z, w); the 4th column is the translation, w=1; the first
    three columns are rotation/scale axes with reasonable magnitude."""
    if off + 64 > len(buf):
        return False
    m = struct.unpack_from("<16f", buf, off)
    # In byte order: m[3]=col0.w, m[7]=col1.w, m[11]=col2.w, m[15]=col3.w
    if not (abs(m[3]) < 0.01 and abs(m[7]) < 0.01 and abs(m[11]) < 0.01
            and abs(m[15] - 1.0) < 0.01):
        return False
    for c in (0, 4, 8):
        mag = (m[c] * m[c] + m[c + 1] * m[c + 1] + m[c + 2] * m[c + 2]) ** 0.5
        if not (0.01 < mag < 100.0):
            return False
    return True


def _is_colmaj_identity(buf: bytes, off: int) -> bool:
    m = struct.unpack_from("<16f", buf, off)
    for i, v in enumerate(m):
        expected = 1.0 if i in (0, 5, 10, 15) else 0.0
        if abs(v - expected) > 1e-5:
            return False
    return True


def find_bone_matrix_runs(ee_path: Path, min_len: int = 4) -> list[tuple[int, int, bool]]:
    """Return a list of (ee_address, run_length, has_non_identity) tuples for
    every run of >= `min_len` consecutive column-major affine matrices, tight
    64-byte stride, in EE RAM."""
    buf = ee_path.read_bytes()
    runs: list[tuple[int, int, bool]] = []
    i = 0
    n = len(buf)
    while i < n - 64:
        if _is_colmaj_affine(buf, i):
            run_len = 0
            any_nonident = False
            while _is_colmaj_affine(buf, i + run_len * 64):
                if not _is_colmaj_identity(buf, i + run_len * 64):
                    any_nonident = True
                run_len += 1
                if run_len > 256:
                    break
            if run_len >= min_len:
                runs.append((i, run_len, any_nonident))
            i += max(run_len, 1) * 64
        else:
            i += 16
    return runs


def dump_matrix_run(ee_path: Path, ee_addr: int, count: int, out_path: Path) -> None:
    """Dump `count` 64-byte column-major matrices starting at EE address
    `ee_addr` as a small JSON file with translation + 3x3 rotation per bone.
    The matrices are kept in storage order (column-major: each row of the
    output `cols` list is one column of the math matrix)."""
    import json
    with open(ee_path, "rb") as f:
        f.seek(ee_addr)
        raw = f.read(count * 64)
    bones = []
    for i in range(count):
        m = struct.unpack_from("<16f", raw, i * 64)
        bones.append({
            "index": i,
            "col0": list(m[0:4]),
            "col1": list(m[4:8]),
            "col2": list(m[8:12]),
            "col3": list(m[12:16]),  # translation in col3.xyz
        })
    out_path.write_text(json.dumps({
        "ee_address": f"0x{ee_addr:08x}",
        "count": count,
        "stride_bytes": 64,
        "storage": "column-major (each col = one qword)",
        "bones": bones,
    }, indent=2))


def dump_player_bones(ee_path: Path, runs: list[tuple[int, int, bool]],
                       out_path: Path) -> dict | None:
    """Emit a structured player_bones.json combining the detected character
    matrix buffers. Each character slot in EE .bss is 0xE00 bytes and two slots
    pack into a 0x1C00 {world,local} pair, double-buffered. We pick the run with
    the most non-identity matrices as the player's primary buffer, and -- when a
    sibling buffer 0xE00 bytes later exists with the same length -- emit it as
    the paired buffer too. The two buffers are saved without trying to label
    them world vs local: at the time of writing, both appear to hold local /
    near-local per-bone matrices (one is the previous-frame copy used for
    interpolation), and consumers should pick whichever composes correctly
    against the published id71 parent hierarchy.
    """
    import json
    if not runs:
        return None
    # Pick the longest run; ties broken by lowest EE address.
    runs_sorted = sorted(runs, key=lambda r: (-r[1], r[0]))
    primary_addr, primary_len, _ = runs_sorted[0]
    addrs = [primary_addr]
    for addr, length, _ in runs_sorted[1:]:
        if length == primary_len and addr not in addrs:
            addrs.append(addr)
        if len(addrs) >= 2:
            break
    with open(ee_path, "rb") as f:
        ee = f.read()
    def read_run(addr: int, n: int):
        out = []
        for i in range(n):
            m = struct.unpack_from("<16f", ee, addr + i * 64)
            out.append({
                "index": i,
                "col0": list(m[0:4]),
                "col1": list(m[4:8]),
                "col2": list(m[8:12]),
                "col3": list(m[12:16]),
            })
        return out
    payload = {
        "source_note": ("Live per-bone matrices snapped from EE RAM via "
                         "tools/parse_pcsx2_state.py --player-bones. The "
                         "engine stores two buffers per character "
                         "(double-buffered current/previous local frames, "
                         "or world+local depending on slot); both are dumped."),
        "skeleton_source": "chunk05/f04_id71.bin",
        "vertex_source":   "chunk21/f17_id8f.bin",
        "stride_bytes": 64,
        "storage": "column-major (each col = one qword); col3.xyz is translation",
        "buffers": [
            {
                "ee_address": f"0x{addr:08x}",
                "count": primary_len,
                "matrices": read_run(addr, primary_len),
            }
            for addr in addrs
        ],
    }
    out_path.write_text(json.dumps(payload, indent=2))
    return payload


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("p2s", type=Path, help="path to .p2s save state")
    ap.add_argument("--out", type=Path, default=Path("/tmp/exterm_state"),
                    help="output directory for extracted blobs")
    ap.add_argument("--scan-bones", action="store_true",
                    help="after extracting, scan EE RAM for bone-matrix runs")
    ap.add_argument("--dump-bones", action="store_true",
                    help="with --scan-bones, also dump each run to JSON in --out")
    ap.add_argument("--player-bones", action="store_true",
                    help="emit a structured player_bones.json combining the "
                         "longest run (the player's primary buffer) and its "
                         "0xE00-paired sibling buffer if present. Implies "
                         "--scan-bones.")
    args = ap.parse_args()

    written = extract_all(args.p2s, args.out)
    for short, path in sorted(written.items()):
        print(f"  {short:<24s} {path.stat().st_size:>10d} bytes  ({path})")

    if args.player_bones:
        args.scan_bones = True
    if args.scan_bones and "ee.bin" in written:
        print("\nScanning EE RAM for column-major bone-matrix runs...")
        runs = find_bone_matrix_runs(written["ee.bin"])
        nontrivial = [r for r in runs if r[2]]
        print(f"  total affine runs (>=8): {len(runs)}; with non-identity: "
              f"{len(nontrivial)}")
        for ee_addr, length, _ in nontrivial:
            with open(written["ee.bin"], "rb") as f:
                f.seek(ee_addr)
                m0 = struct.unpack("<16f", f.read(64))
            tx, ty, tz = m0[12], m0[13], m0[14]
            print(f"  EE 0x{ee_addr:08x}  n={length:3d}  "
                  f"m0_translation=({tx:+.2f},{ty:+.2f},{tz:+.2f})")
            if args.dump_bones:
                jp = args.out / f"bones_ee_{ee_addr:08x}.json"
                dump_matrix_run(written["ee.bin"], ee_addr, length, jp)
                print(f"      -> {jp}")
        if args.player_bones:
            jp = args.out / "player_bones.json"
            payload = dump_player_bones(written["ee.bin"], nontrivial, jp)
            if payload is not None:
                addrs = [b["ee_address"] for b in payload["buffers"]]
                cnt = payload["buffers"][0]["count"]
                print(f"\n  player_bones.json: {len(addrs)} buffer(s) x {cnt} "
                      f"matrices each -> {jp}")
                for a in addrs:
                    print(f"      ee {a}")
            else:
                print("  (no bone runs found; nothing to dump)")


if __name__ == "__main__":
    main()
