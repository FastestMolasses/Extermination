#!/usr/bin/env python3
"""export_native.py — export the player character as a single binary asset
(.emdl) consumed by the native port (extermination-port), which loads it with
plain fread (zero third-party deps on the port side).

Reuses the proven decoders from export_gltf.py / extract_models.py /
anim_decoder.py: per-bone Q4.12 tristrip mesh, id 0x71 skeleton parent table,
and keyframe rot/trn streams (NLERP-sampled, composed through the parent
table into per-frame WORLD matrix palettes).

The output is disc-derived: write it only into git-ignored locations
(extermination-port/assets/ is ignored there).

EMDL v1 layout (little-endian):

  char  magic[4]      "EMD1"
  u32   bone_count    bones exported (mesh sections kept)
  u32   vert_count    total vertices
  u32   index_count   total triangle indices (u32, global vertex ids)
  u32   frame_count   baked pose frames (>=1)
  f32   fps           playback rate for the baked frames
  u32   reserved[2]
  i32   parents[bone_count]                  (-1 = root; informational)
  vert  { f32 px,py,pz; f32 nx,ny,nz; u32 bone } x vert_count
  u32   indices[index_count]
  f32   palette[frame_count][bone_count][16] column-major world matrices

Vertices are in BONE-LOCAL object space (exactly as stored on disc); the
palette matrices are the composed world transforms — vertex_world =
palette[frame][bone] * pos. That mirrors the PS2 pipeline (per-bone packets
+ a matrix palette in VU1 dmem).

Usage (macOS arm64, repo root):
  .venv/bin/python tools/export_native.py \
      --mesh extract/chunk21/f17_id8f.bin \
      --skel extract/chunk05/f04_id71.bin \
      --clip 0 \
      --out ../extermination-port/assets/player.emdl
  # or a single static frame from a live PCSX2 capture:
  .venv/bin/python tools/export_native.py --live extract/live/player_bones_live.json \
      --out ../extermination-port/assets/player_live.emdl
"""
from __future__ import annotations

import argparse
import importlib.util
import json
import struct
import sys
from pathlib import Path

TOOLS = Path(__file__).resolve().parent


def _load(name: str, fname: str):
    spec = importlib.util.spec_from_file_location(name, TOOLS / fname)
    mod = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules[name] = mod
    spec.loader.exec_module(mod)
    return mod


eg = _load("_export_gltf", "export_gltf.py")
ad = _load("_anim_decoder", "anim_decoder.py")

FPS = 30.0


# ---------------------------------------------------------------------------
# Small column-major mat4 helpers (matrices are tuples of 4 column 4-tuples,
# matching the engine's storage convention).

def mat_identity():
    return ((1.0, 0.0, 0.0, 0.0), (0.0, 1.0, 0.0, 0.0),
            (0.0, 0.0, 1.0, 0.0), (0.0, 0.0, 0.0, 1.0))


def mat_from_quat_trn(q, t):
    """Column-major affine matrix from unit quat (x,y,z,w) + translation."""
    x, y, z, w = q
    xx, yy, zz = x * x, y * y, z * z
    xy, xz, yz = x * y, x * z, y * z
    wx, wy, wz = w * x, w * y, w * z
    return (
        (1.0 - 2.0 * (yy + zz), 2.0 * (xy + wz), 2.0 * (xz - wy), 0.0),
        (2.0 * (xy - wz), 1.0 - 2.0 * (xx + zz), 2.0 * (yz + wx), 0.0),
        (2.0 * (xz + wy), 2.0 * (yz - wx), 1.0 - 2.0 * (xx + yy), 0.0),
        (t[0], t[1], t[2], 1.0),
    )


def mat_mul(a, b):
    """a * b, column-major (b applied first)."""
    out = []
    for c in range(4):
        col = []
        for r in range(4):
            col.append(sum(a[k][r] * b[c][k] for k in range(4)))
        out.append(tuple(col))
    return tuple(out)


# ---------------------------------------------------------------------------

def load_mesh_sections(mesh_path: Path):
    """Returns (sections, section_bone): per-section (positions, normals,
    indices) plus the section -> GLOBAL bone index mapping from the mesh's
    prefix directory (decode_bone_section_table — multiple sections can rig
    one bone; -1 = no bone / static). Mirrors build_glb_unified."""
    per_bone = eg.load_per_bone_meshes_with_vids(mesh_path)
    sections = [eg.triangulate_bone(v) for v in per_bone]
    sec_table = eg.decode_bone_section_table(mesh_path.read_bytes())
    if sec_table is not None:
        _bp, section_bone, _offs = sec_table
    else:
        section_bone = list(range(len(sections)))
    return sections, section_bone


def bake_clip_palettes(skel_path: Path, clip: int):
    """Sample clip keyframes at 1-frame steps and compose world matrices.
    Returns (parents, frames) where frames is [ [mat4 x bone_count] ... ]."""
    data = skel_path.read_bytes()
    entries = eg._entry_slices(data)
    if not (0 <= clip < len(entries)):
        raise SystemExit(f"clip {clip} out of range (0..{len(entries) - 1})")
    s, e = entries[clip]
    bc, parents_raw, rot, trn = eg._decode_entry_streams(data[s:e])
    parents = eg._signed_parents(parents_raw)

    # The final record's t_next can be a 0xFFFF "hold forever" sentinel —
    # exclude sentinels when measuring the clip length.
    clip_len = 0
    for streams in (rot, trn):
        for fr in streams:
            for kf in fr:
                t = int(kf.t_next)
                if t < 0x8000:
                    clip_len = max(clip_len, t)
    clip_len = max(clip_len, 1)

    frames = []
    for f in range(clip_len + 1):
        locals_ = []
        for b in range(bc):
            q = ad.sample_bone(rot[b], float(f)) if rot[b] else (0, 0, 0, 1)
            t = ad.sample_bone(trn[b], float(f), normalize=False) \
                if trn[b] else (0.0, 0.0, 0.0)
            locals_.append(mat_from_quat_trn(q, t))
        # Cycle-safe composition, mirroring extract_models.py
        # _compose_world_from_local: the player parent table contains a
        # 2<->3 cycle; a bone whose chain revisits itself acts as a root.
        world = [None] * bc

        def compose(b, seen):
            if world[b] is None:
                p = parents[b]
                if p < 0 or b in seen:
                    world[b] = locals_[b]
                else:
                    world[b] = mat_mul(compose(p, seen | {b}), locals_[b])
            return world[b]

        for b in range(bc):
            compose(b, set())
        frames.append(world)
    return parents, frames


def load_live_palette(live_json: Path):
    """One frame of world matrices from a live PCSX2 node capture."""
    data = json.loads(live_json.read_text())
    mats = []
    for m in data["buffers"][0]["matrices"]:
        mats.append(tuple(tuple(m[f"col{c}"]) for c in range(4)))
    return [mats]


def recentre(frames):
    """Subtract frame-0 root translation so the character sits at the
    origin (the port supplies its own camera/world placement)."""
    ox, oy, oz = frames[0][0][3][:3]
    out = []
    for fr in frames:
        nf = []
        for m in fr:
            c3 = (m[3][0] - ox, m[3][1] - oy, m[3][2] - oz, m[3][3])
            nf.append((m[0], m[1], m[2], c3))
        out.append(nf)
    return out


def write_emdl(out_path: Path, sections, section_bone, parents, frames,
               fps: float):
    """Palette slots = the skeleton's GLOBAL bones plus one trailing identity
    slot for sections with no bone (section_bone -1). Each vertex carries the
    palette slot of its section — several sections can share a bone, exactly
    like the glTF section->bone parenting."""
    n_bones = len(frames[0])
    id_slot = n_bones        # identity matrix slot for unmapped sections

    verts = bytearray()
    indices = []
    vbase = 0
    used = set()
    for i, (pos, nrm, idx) in enumerate(sections):
        b = section_bone[i] if i < len(section_bone) else -1
        slot = b if 0 <= b < n_bones else id_slot
        used.add(slot)
        for p, n in zip(pos, nrm):
            verts += struct.pack("<6fI", p[0], p[1], p[2],
                                 n[0], n[1], n[2], slot)
        indices.extend(vbase + k for k in idx)
        vbase += len(pos)

    ident = mat_identity()
    pal = bytearray()
    for fr in frames:
        for b in range(n_bones):
            for col in fr[b]:
                pal += struct.pack("<4f", *col)
        for col in ident:
            pal += struct.pack("<4f", *col)

    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("wb") as f:
        f.write(b"EMD1")
        f.write(struct.pack("<4If2I", n_bones + 1, vbase, len(indices),
                            len(frames), fps, 0, 0))
        for b in range(n_bones):
            p = parents[b] if b < len(parents) else -1
            f.write(struct.pack("<i", p if 0 <= p < n_bones else -1))
        f.write(struct.pack("<i", -1))   # identity slot
        f.write(verts)
        f.write(struct.pack(f"<{len(indices)}I", *indices))
        f.write(pal)

    print(f"wrote {out_path}")
    print(f"  palette: {n_bones}+1 slots ({len(used)} referenced)")
    print(f"  verts  : {vbase}  tris: {len(indices) // 3}")
    print(f"  frames : {len(frames)} @ {fps} fps")


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--mesh", default="extract/chunk21/f17_id8f.bin")
    ap.add_argument("--skel", default="extract/chunk05/f04_id71.bin")
    ap.add_argument("--clip", type=int, default=0)
    ap.add_argument("--live", help="player_bones_live.json: export that "
                    "single captured frame instead of baking a clip")
    ap.add_argument("--out", required=True)
    args = ap.parse_args(argv)

    sections, section_bone = load_mesh_sections(Path(args.mesh))
    print(f"mesh sections: {len(sections)} "
          f"({sum(len(s[0]) for s in sections)} verts)")
    print(f"section -> bone: {section_bone}")

    if args.live:
        frames = recentre(load_live_palette(Path(args.live)))
        # parent table only informational here; derive length from capture
        parents = [-1] * len(frames[0])
    else:
        parents, frames = bake_clip_palettes(Path(args.skel), args.clip)
        print(f"clip {args.clip}: {len(frames)} frames, "
              f"{len(frames[0])} bones, parents {parents}")
        frames = recentre(frames)

    write_emdl(Path(args.out), sections, section_bone, parents, frames, FPS)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
