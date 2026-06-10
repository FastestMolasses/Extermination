#!/usr/bin/env python3
"""camera_probe.py - Dump and sanity-check the camera system state from a
PCSX2 save-state EE-RAM/scratchpad dump (produced by tools/parse_pcsx2_state.py).

Prints the camera struct at 0x008101E0, the global eye/target/up/forward
vectors and view matrices at 0x008105D0..0x00810650, the per-frame render
context's projection (ctx+0x2340), view copy (ctx+0x2380) and composed
camera matrix K = P*V (ctx+0x23C0), plus the fog row and zoom — and
verifies K == P*V numerically.

Usage:
    .venv/bin/python tools/parse_pcsx2_state.py <state.p2s> --out /tmp/cam
    .venv/bin/python tools/camera_probe.py /tmp/cam

See docs/FINDINGS.md "CAMERA SYSTEM" (2026-06-10) for the field map.

Disc-data safety: reads user-local scratch dumps only; writes nothing to
the repository.
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

CAM = 0x8101E0          # camera struct (0xD0 bytes); 0x8101D0 = 0x10-byte once-flag header
PLAYER = 0x8102B0
EYE_G, TGT_G, UP_G, FWD_G = 0x8105D0, 0x8105E0, 0x8105F0, 0x810600
VIEW_M, VIEW_T = 0x810610, 0x810650
SCALARS = 0x810690      # horizdist, |dy|, eye_height, horizdist_global, angle
AREA_ID = 0x810700
CTX_PTR = 0x275670      # -> per-frame render context


def main() -> int:
    if len(sys.argv) != 2:
        print(__doc__)
        return 1
    d = Path(sys.argv[1])
    ee = (d / "ee.bin").read_bytes()
    sp_path = d / "scratchpad.bin"
    sp = sp_path.read_bytes() if sp_path.exists() else b""

    def f(a): return struct.unpack("<f", ee[a:a + 4])[0]
    def w(a): return struct.unpack("<I", ee[a:a + 4])[0]
    def vec(a, n=4): return [round(f(a + 4 * i), 4) for i in range(n)]
    def mat(a): return [[f(a + 16 * r + 4 * c) for c in range(4)] for r in range(4)]
    def pmat(name, a):
        print(f" {name} @{a:#x}:")
        for r in mat(a):
            print("   ", [round(v, 4) for v in r])

    print("player pos +A0:", vec(PLAYER + 0xA0), " +B0:", vec(PLAYER + 0xB0))
    print(f"camera struct @{CAM:#x}:")
    b = ee[CAM:CAM + 0x10]
    print(f"  state={b[0]} sub={b[1]} mode4={b[4]} table={b[5]} MODE={b[6]} hit={b[7]}")
    print("  +0x10 desired eye :", vec(CAM + 0x10))
    print("  +0x20 desired tgt :", vec(CAM + 0x20))
    print("  +0x30 offset      :", vec(CAM + 0x30))
    print("  +0x44 yaw         :", round(f(CAM + 0x44), 4), "  +0x9C yaw:", round(f(CAM + 0x9C), 4))
    print("  +0x50/54/5C/60/64 :", [round(f(CAM + o), 3) for o in (0x50, 0x54, 0x5C, 0x60, 0x64)])
    print("  +0x8C tgt y-offset:", round(f(CAM + 0x8C), 3))
    print("  +0xB0 forward     :", vec(CAM + 0xB0))
    print("globals:")
    print("  eye   :", vec(EYE_G), "\n  target:", vec(TGT_G))
    print("  up    :", vec(UP_G), "\n  fwd   :", vec(FWD_G))
    pmat("view (lookat)", VIEW_M)
    pmat("view^T", VIEW_T)
    print("  scalars 690..6A0:", [round(f(SCALARS + 4 * i), 4) for i in range(5)])
    print("  area id:", ee[AREA_ID], ee[AREA_ID + 1], ee[AREA_ID + 2])

    ctx = w(CTX_PTR)
    print(f"render ctx @{ctx:#x}:")
    print("  fog near/far (+B8/BC):", round(f(ctx + 0xB8), 2), round(f(ctx + 0xBC), 2),
          " fog rgb (+B0):", ee[ctx + 0xB0], ee[ctx + 0xB1], ee[ctx + 0xB2])
    print("  zoom (+0x2468):", round(f(ctx + 0x2468), 3))
    pmat("P", ctx + 0x2340)
    pmat("V copy", ctx + 0x2380)
    pmat("K = P*V", ctx + 0x23C0)

    # verify K == P*V (column-vector convention: K[r] = sum_k V[r][k]*P[k])
    P, V, K = mat(ctx + 0x2340), mat(ctx + 0x2380), mat(ctx + 0x23C0)
    err = 0.0
    for r in range(4):
        for c in range(4):
            kv = sum(V[r][k] * P[k][c] for k in range(4))
            err = max(err, abs(kv - K[r][c]) / max(1.0, abs(K[r][c])))
    print(f"  K == P*V check: max rel err {err:.2e} ->", "OK" if err < 1e-3 else "MISMATCH (camera moved mid-frame?)")

    if sp:
        def fs(a): return struct.unpack("<f", sp[a:a + 4])[0]
        print("scratchpad: fwd 38A0:", [round(fs(0x38A0 + 4 * i), 3) for i in range(3)],
              " eye+4*fwd 38C0:", [round(fs(0x38C0 + 4 * i), 3) for i in range(3)],
              " zoom 3B60:", round(fs(0x3B60), 3))
    return 0


if __name__ == "__main__":
    sys.exit(main())
