#!/usr/bin/env python3
"""export_shadow_proxy.py - the player's drop-shadow proxy mesh as an EMDL.

Original evidence (port docs/SHADOW_ORIGINAL.md): 001D9EE0 draws the
silhouette of the shadow into a 128x128 target with 001D4740(D_0028A490[kind])
after 001C7420 uploaded the ACTOR's node palette. For the player (kind 0x28)
the table slot is the chunk03 file id 0x28, extract/chunk03/f32_id28.bin: a
21-node skinned raw blob (the same node layout as the player mesh), drawn
untextured with one flat RGBAQ. This tool exports that file's geometry through
export_native.py with the SAME clip list as the port's player.emdl (same
palette slots, frame count and clip table; checked), so the port can draw it
with the player's live palette, as the original does.

Output is disc-derived: write it only into git-ignored locations (the port's
assets/). Runs natively on arm64 macOS (pure Python).

Usage (repo root):
  .venv/bin/python tools/export_shadow_proxy.py \
      --player-emdl ../extermination-port/assets/player.emdl \
      --out ../extermination-port/assets/player_shadow.emdl \
      --verify-ram build/startup-reference/playable_ee.bin
"""
from __future__ import annotations

import argparse
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SHADOW_TABLE = 0x28A490          # D_0028A490, 67 record pointers (slot = file id)
PLAYER_KIND = 0x28               # player+0x96


def emdl_header(data: bytes) -> dict:
    if data[:4] != b"EMD3":
        raise SystemExit("not an EMD3 file")
    bones, verts, indices, frames, fps, textures, flags, clips = struct.unpack_from("<4If3I", data, 4)
    o = 36 + 4 * bones + 16 * textures
    table = [struct.unpack_from("<3If", data, o + 16 * i) for i in range(clips)]
    return dict(bones=bones, verts=verts, indices=indices, frames=frames, textures=textures,
                clips=table, palette_offset=36 + 4 * bones + 16 * textures + 16 * clips + 40 * verts + 4 * indices)


def palette(data: bytes) -> bytes:
    h = emdl_header(data)
    start = h["palette_offset"]
    return data[start:start + h["frames"] * h["bones"] * 64]


def verify_ram(mesh: bytes, ram_path: Path, kind: int) -> None:
    ram = ram_path.read_bytes()
    pointer = struct.unpack_from("<I", ram, SHADOW_TABLE + 4 * kind)[0]
    if ram[pointer:pointer + len(mesh)] != mesh:
        raise SystemExit(f"D_0028A490[{kind:#x}] = {pointer:#x} does not hold the mesh file")
    tag = struct.pack("<HBBI", struct.unpack_from("<I", mesh, 4)[0], 0, 0x30, pointer + 0x40)
    if ram.find(tag) < 0:
        raise SystemExit("no captured REF of the proxy mesh (qwc, +0x40) in the capture")
    print(f"verified: D_0028A490[{kind:#x}] = {pointer:#x} holds the file; the captured "
          f"silhouette pass REFs it (qwc {struct.unpack_from('<I', mesh, 4)[0]})")


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--kind", type=lambda x: int(x, 0), default=PLAYER_KIND,
                    help="actor+0x96 kind; D_0028A490 slot = chunk03 file id (0x28 player, "
                    "0x29 the kind Roger carries)")
    ap.add_argument("--mesh", help="default extract/chunk03/f{kind-8:02d}_id{kind:02x}.bin")
    ap.add_argument("--anim", default="extract/chunk28/f01_id3c.bin")
    ap.add_argument("--player-emdl", "--clips-from", dest="player_emdl", required=True,
                    help="the EMDL whose clip table/palette layout the proxy mirrors (player.emdl)")
    ap.add_argument("--out", required=True)
    ap.add_argument("--verify-ram", type=Path, help="a captured EE RAM image to content-match against")
    args = ap.parse_args(argv)

    if args.mesh is None:
        args.mesh = f"extract/chunk03/f{args.kind - 8:02d}_id{args.kind:02x}.bin"
    mesh = (ROOT / args.mesh).read_bytes() if not Path(args.mesh).is_absolute() else Path(args.mesh).read_bytes()
    if args.verify_ram:
        verify_ram(mesh, args.verify_ram if args.verify_ram.is_absolute() else ROOT / args.verify_ram, args.kind)
    player = Path(args.player_emdl).read_bytes()
    ids = [c[0] for c in emdl_header(player)["clips"]]
    if not ids:
        raise SystemExit("player.emdl has no clip table")
    subprocess.run([sys.executable, str(ROOT / "tools/export_native.py"), "--mesh", args.mesh,
                    "--anim", args.anim, "--clips", ",".join(map(str, ids)), "--out", args.out],
                   cwd=ROOT, check=True)
    out = Path(args.out) if Path(args.out).is_absolute() else ROOT / args.out
    shadow = out.read_bytes()
    hs, hp = emdl_header(shadow), emdl_header(player)
    if hs["textures"] != 0:
        raise SystemExit("the proxy mesh is drawn untextured; got textures")
    if (hs["bones"], hs["frames"], [c[:3] for c in hs["clips"]]) != (hp["bones"], hp["frames"], [c[:3] for c in hp["clips"]]):
        raise SystemExit("clip/palette layout differs from player.emdl")
    # The original skins the proxy with the actor's LIVE node palette
    # (001C7420 uploads actor+0x110 nodes), so the port must pass the
    # player's current palette; the baked frames only mirror the layout.
    # Report how much of today's player.emdl bake this exporter reproduces.
    size = hs["bones"] * 64
    ps, pp = palette(shadow), palette(player)
    same = sum(ps[i:i + size] == pp[i:i + size] for i in range(0, len(ps), size))
    print(f"layout matches {args.player_emdl}: {hs['bones']} palette slots, {hs['frames']} frames, "
          f"{len(hs['clips'])} clips; baked frames byte-identical to it: {same}/{hs['frames']}")
    print(f"proxy mesh: {hs['verts']} verts, {hs['indices'] // 3} tris, untextured")
    return 0


if __name__ == "__main__":
    sys.exit(main())
