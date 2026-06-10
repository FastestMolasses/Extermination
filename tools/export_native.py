#!/usr/bin/env python3
"""export_native.py — export a skinned character as a single binary asset
(.emdl) consumed by the native port (extermination-port), which loads it with
plain fread (zero third-party deps on the port side).

Pipeline (decoded 2026-06-09, see docs/FINDINGS.md "Skinned-character
pipeline FULLY DECODED"): the render mesh is a stream of VIF-ready blocks
(STCYCL(4,4) + UNPACK V4-32, 32 records of [tex][ST][normal][pos+w] per
block) consumed by the 62-qw VU1 kernel at vram 0x0023C780. Each vertex's
bone is encoded in its position-W float read as an integer: bits 0..9 are
the VU1 dmem qword address of the node's 7-qw matrix set at qw 8*node,
bit 15 is the strip-restart flag. This exporter bakes that per-vertex node
index into the EMDL verts and emits a world-matrix palette per frame.

The output is disc-derived: write it only into git-ignored locations
(extermination-port/assets/ is ignored there).

EMDL v2 layout (little-endian):

  char  magic[4]      "EMD2"
  u32   bone_count    palette slots (nodes + 1 trailing identity slot)
  u32   vert_count    total vertices
  u32   index_count   total triangle indices (u32, global vertex ids)
  u32   frame_count   baked pose frames (>=1)
  f32   fps           playback rate for the baked frames
  u32   tex_count     textures in the embedded table (0 = untextured)
  u32   reserved
  i32   parents[bone_count]                  (-1 = root; informational)
  tex   { u32 width, height, byte_offset, reserved } x tex_count
        byte_offset into the RGBA8 texel blob at the end of the file
  vert  { f32 px,py,pz; f32 nx,ny,nz; f32 u,v; u32 bone; u32 tex }
        x vert_count   (tex = index into the table, 0xFFFFFFFF = none;
        UVs are normalized texture coords, REPEAT addressing — values
        outside [0,1] are intentional tiling)
  u32   indices[index_count]
  f32   palette[frame_count][bone_count][16] column-major world matrices
  u8    texels[]      RGBA8, rows top-down, per-texture at byte_offset

Vertices are BONE-LOCAL (exactly as stored on disc); vertex_world =
palette[frame][bone] * pos — the same contract as the PS2 kernel.

TEXTURE/COLOR (decoded 2026-06-09 s5): vertex record qword 0 ("marker")
IS the draw's TEX0 register value — TBP0/TBW/PSM/TW/TH/CBP baked into
the disc file (validated: all 51 (TBP0,CBP) pairs of f00_id3b appear
verbatim in a live GS dump's per-draw TEX0 stream; the two variants per
pair differ only in CLD). Qword 1 holds the vertex's normalized ST.
Color requires a one-frame PCSX2 GS dump (--gsdump) whose VRAM snapshot
supplies the PSMT4 texels + 16-entry CLUTs (palettes are runtime-built;
see docs/FINDINGS.md "Texture COLOR recovered").

Usage (macOS arm64, repo root):
  # ANIMATED export: bake clip N from an id 0x74 animation library file
  # (channel encodings decoded 2026-06-09 s4 — see FINDINGS.md):
  python3 tools/export_native.py --mesh extract/chunk28/f00_id3b.bin \
      --anim extract/chunk28/f01_id3c.bin --clip 346 \
      --out ../extermination-port/assets/player.emdl
  # pose a mesh with a live PCSX2 node capture (world matrices, node order;
  # capture with the emulator PAUSED so all nodes are from one frame):
  python3 tools/export_native.py --mesh extract/chunk28/f00_id3b.bin \
      --live extract/live/npc_nodes_live.json \
      --out ../extermination-port/assets/player.emdl
  # without --live/--anim: identity palette (bone-local parts overlap).
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

# ---------------------------------------------------------------------------
# Skinned-mesh loader (DECODED 2026-06-09 session 2)
#
# The render mesh is a stream of VIF-ready blocks: each block is
# `STCYCL(4,4)` + `UNPACK V4-32 128qw -> TOPS+0` followed by 32 records
# of 4 qwords [tex/marker, ST, normal, pos+w]. The engine DMA-REFs the
# whole stream and runs the 62-qw skinning kernel at vram 0x0023C780
# once per block (MSCAL/MSCNT). That kernel reads the position W float
# AS AN INTEGER (`ilw .w`):
#
#     bits 0..9   absolute VU1 dmem qword address of this vertex's
#                 matrix set (7 qw: 4 transform rows + 3 normal-matrix
#                 rows), laid out at qw 8 * node_index (nodes 0..1 carry
#                 no skin; lowest observed address is qw 16 = node 2)
#     bit 15      strip-restart flag (vertex emits no triangle)
#     sign/k bits make the word a valid +-1.0-ish float (winding parity)
#
# So per-vertex node = (w_bits & 0x3FF) >> 3, and positions/normals
# are BONE-LOCAL. Posed world = node_world_matrix * pos, exactly what the
# port's skinning shader does with the EMDL palette.

VERT_QW = 4 * 16     # 4 qwords per record
BLOCK_VERTS = 32


def _walk_blob_blocks(d: bytes):
    """Block payloads of a raw mesh blob (header: n_blocks, total_qwc,
    n_nodes, size; first UNPACK at +0x48). Returns (payloads, n_nodes)."""
    n_blocks, _qwc, n_nodes, size = struct.unpack_from("<4I", d, 0)
    payloads = []
    o = 0x48
    while o + 8 <= size and len(payloads) < n_blocks:
        w0, w1 = struct.unpack_from("<2I", d, o)
        if w0 == 0x01000404 and (w1 >> 24) & 0xff == 0x6c:
            num = (w1 >> 16) & 0xff
            qw = 256 if num == 0 else num
            payloads.append(d[o + 8:o + 8 + qw * 16])
            o += 8 + qw * 16
        else:
            o += 4   # skip MSCAL/MSCNT words
    return payloads, n_nodes


def _walk_meshsig_blocks(d: bytes, segment: int):
    """Block payloads of a packed model file (player f17_id8f style):
    MESH_SIG-tagged 0x820 blocks, with MATRIX-descriptor separator blocks
    splitting the file into segments (LOD/variant sets, each with its own
    node-slot space). Returns payloads of `segment`."""
    em = _load("_extract_models", "extract_models.py")
    bounds = em.block_bounds(d)
    seg = 0
    payloads = []
    for i, (s, e) in enumerate(bounds):
        if not i:
            continue
        desc = d[s + 0x10:s + 0x20]
        if len(desc) < 16:
            continue
        if em.is_matrix_descriptor(desc):
            seg += 1
            continue
        if desc[8:16] == em.MESH_SIG and seg == segment:
            payloads.append(d[s + 0x20:s + 0x20 + 0x800])
    return payloads


def tex0_fields(q: int) -> dict:
    return {
        "tbp0": q & 0x3FFF, "tbw": (q >> 14) & 0x3F, "psm": (q >> 20) & 0x3F,
        "tw": (q >> 26) & 0xF, "th": (q >> 30) & 0xF,
        "cbp": (q >> 37) & 0x3FFF,
    }


# CLD (bits 61-63) is per-draw CLUT-cache control, not texture identity:
# the same texture appears with CLD=1 (first use) and CLD=0 (reuse).
TEX0_KEY_MASK = ~(7 << 61) & (2 ** 64 - 1)


def load_mesh_sections(mesh_path: Path, segment: int = 0):
    """Decode the skinned mesh into one section with PER-VERTEX bones,
    UVs and texture ids.

    Returns (sections, max_slot, tex_table) with sections = [(pos, nrm,
    tris, bones, uvs, texids)]; positions/normals bone-local, bone =
    dmem-slot index, texid = index into tex_table (list of TEX0 field
    dicts in first-use order; 0xFFFFFFFF = no/implausible texture).
    """
    d = mesh_path.read_bytes()
    if struct.unpack_from("<I", d, 0)[0] < 0x1000 and \
            d[0x48:0x50] == bytes.fromhex("040400010080806c"):
        payloads, _n = _walk_blob_blocks(d)
    else:
        payloads = _walk_meshsig_blocks(d, segment)
    if not payloads:
        raise SystemExit(f"no skinned-mesh blocks found in {mesh_path}")

    raw_pos, raw_nrm, raw_bone, raw_uv, raw_tex = [], [], [], [], []
    tris = []
    weld = {}
    max_slot = 0
    tex_table: list[dict] = []
    tex_index: dict[int, int] = {}     # TEX0 (CLD-masked) -> table index
    NO_TEX = 0xFFFFFFFF

    def tex_of(q0: int) -> int:
        key = q0 & TEX0_KEY_MASK
        ti = tex_index.get(key)
        if ti is None:
            f = tex0_fields(key)
            # plausibility: indexed PSM, sane log2 dims (engine uses
            # PSMT4 for characters/level, PSMT8 for UI sheets)
            if f["psm"] not in (0x13, 0x14) or not (4 <= f["tw"] <= 10) \
                    or not (4 <= f["th"] <= 10):
                ti = NO_TEX
            else:
                ti = len(tex_table)
                f["key"] = key
                tex_table.append(f)
            tex_index[key] = ti
        return ti

    def vid_of(p, n, b, uv, t):
        key = (round(p[0], 4), round(p[1], 4), round(p[2], 4),
               round(n[0], 3), round(n[1], 3), round(n[2], 3), b,
               round(uv[0], 5), round(uv[1], 5), t)
        i = weld.get(key)
        if i is None:
            i = len(raw_pos)
            weld[key] = i
            raw_pos.append(p)
            raw_nrm.append(n)
            raw_bone.append(b)
            raw_uv.append(uv)
            raw_tex.append(t)
        return i

    for payload in payloads:
        run = []   # welded ids of the running strip, parallel restart flags
        for r in range(0, len(payload) - VERT_QW + 1, VERT_QW):
            w = struct.unpack_from("<f", payload, r + 0x3c)[0]
            if abs(abs(w) - 1.0) > 0.25:
                break
            wbits = struct.unpack_from("<I", payload, r + 0x3c)[0]
            # node index = dmem qword address / 8 (matrix sets at qw 8*n;
            # validated on the live NPC pose: joint-edge coherence has a
            # sharp minimum at this mapping)
            slot = (wbits & 0x3FF) >> 3
            if slot < 2:
                continue
            max_slot = max(max_slot, slot)
            pos = struct.unpack_from("<3f", payload, r + 0x30)
            nrm = struct.unpack_from("<3f", payload, r + 0x20)
            uv = struct.unpack_from("<2f", payload, r + 0x10)
            tex = tex_of(int.from_bytes(payload[r:r + 8], "little"))
            vi = vid_of(pos, nrm, slot, uv, tex)
            restart = bool(wbits & 0x8000)
            run.append((vi, restart))
            k = len(run)
            if k >= 3 and not restart:
                a, b, c = run[k - 3][0], run[k - 2][0], run[k - 1][0]
                if a != b and b != c and a != c:
                    # alternate winding by strip parity
                    if (k & 1) == 0:
                        tris.extend((a, b, c))
                    else:
                        tris.extend((b, a, c))

    sections = [(raw_pos, raw_nrm, tris, raw_bone, raw_uv, raw_tex)]
    return sections, max_slot, tex_table


def bake_id74_palettes(anim_path: Path, clip: int):
    """Bake one id 0x74 clip (mesh-companion animation library) into
    per-frame world-matrix palettes.

    Channel encodings (decoded + live-verified 2026-06-09 s4): rotation =
    4x20-bit truncated-float local quat (x,y,z,w), translation/scale =
    3x26-bit vec3, sparse keys with lerp semantics. The engine composes
    world = parent_world * local with the local rotation built from the
    CONJUGATE of the stored quat (pinned against live world matrices:
    conj gives 0.008 max element error vs 15.2 for the direct quat).

    Returns (parents, frames, fps): frames = [[mat4 x n] x clip_len].
    Clips advance one frame per 60 Hz tick on the PS2 (live node cursor
    rate = 1/(clip_len-1) per tick), so fps = 60.
    """
    em = _load("_extract_models", "extract_models.py")
    d = anim_path.read_bytes()
    hdrs = em.find_id74_headers(d)
    if not hdrs:
        raise SystemExit(f"no id 0x74 animation containers in {anim_path}")
    if not (0 <= clip < len(hdrs)):
        raise SystemExit(f"clip {clip} out of range (0..{len(hdrs) - 1} "
                         f"in {anim_path.name})")
    pre = em.parse_id74_prefix(d, hdrs[clip])
    n, parents = pre["n"], pre["parents"]

    def keyframes(chan):
        return [[ad.Keyframe(t_next=f, values=v) for f, v in node_keys]
                for node_keys in pre[chan]]

    rot, trn, scl = keyframes("rot"), keyframes("trn"), keyframes("scl")

    frames = []
    for f in range(pre["clip_len"]):
        world = []
        for b in range(n):
            q = ad.sample_bone(rot[b], float(f)) if rot[b] else (0, 0, 0, 1)
            t = ad.sample_bone(trn[b], float(f), normalize=False) \
                if trn[b] else (0.0, 0.0, 0.0)
            s = ad.sample_bone(scl[b], float(f), normalize=False) \
                if scl[b] else (1.0, 1.0, 1.0)
            # engine convention: local rotation = R(conj(q))
            local = mat_from_quat_trn((-q[0], -q[1], -q[2], q[3]), t)
            if any(abs(c - 1.0) > 1e-4 for c in s):
                local = tuple(
                    tuple(v * (s[c] if c < 3 else 1.0) for v in col)
                    if c < 3 else col
                    for c, col in enumerate(local))
            p = parents[b]
            world.append(local if p < 0 else mat_mul(world[p], local))
        frames.append(world)
    return parents, frames, 60.0


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


def build_texture_blob(gsdump: Path | None, tex_table: list[dict]):
    """Resolve each mesh TEX0 to RGBA8 texels via a GS dump's VRAM
    snapshot (PSMT4/PSMT8 indices + runtime-built CLUTs both live there).
    Without a dump every texture is a 1x1 mid-grey placeholder.

    Returns (entries, blob): entries = [{w, h, off}] parallel to
    tex_table."""
    entries, blob = [], bytearray()
    lm = None
    if gsdump is not None:
        pg = _load("_parse_gsdump", "parse_gsdump.py")
        state_data, _regs, _pkts, _serial, _crc = pg.parse(gsdump, quiet=True)
        lm = pg.dump_vram(state_data)
        cp = _load("_clut_pair", "clut_pair.py")
        from clut import apply_clut
    for f in tex_table:
        w, h = 1 << f["tw"], 1 << f["th"]
        if lm is None:
            entries.append({"w": 1, "h": 1, "off": len(blob)})
            blob += b"\x80\x80\x80\xff"
            continue
        if f["psm"] == 0x14:
            idx = cp.read_psmt4(lm, f["tbp0"], f["tbw"], w, h)
            pal = cp.read_clut16_rgba(lm, f["cbp"]) + bytes(1024 - 64)
        else:
            idx = cp.read_psmt8(lm, f["tbp0"], f["tbw"], w, h)
            pal = cp.read_clut_rgba(lm, f["cbp"])
        entries.append({"w": w, "h": h, "off": len(blob)})
        blob += apply_clut(idx, pal)
    return entries, bytes(blob)


def write_emdl(out_path: Path, sections, section_bone, parents, frames,
               fps: float, tex_entries=None, tex_blob=b""):
    """Vertices carry their own palette slot (per-vertex bone, decoded from
    the position-W dmem address — see load_mesh_sections). A trailing
    identity slot soaks up any vertex whose slot exceeds the palette."""
    n_bones = len(frames[0])
    id_slot = n_bones        # identity matrix slot for unmapped vertices
    tex_entries = tex_entries or []
    NO_TEX = 0xFFFFFFFF

    verts = bytearray()
    indices = []
    vbase = 0
    used = set()
    for i, sec in enumerate(sections):
        pos, nrm, idx = sec[0], sec[1], sec[2]
        bones = sec[3] if len(sec) > 3 else None
        uvs = sec[4] if len(sec) > 4 else None
        texs = sec[5] if len(sec) > 5 else None
        for vi, (p, n) in enumerate(zip(pos, nrm)):
            b = bones[vi] if bones is not None else (
                section_bone[i] if i < len(section_bone) else -1)
            slot = b if 0 <= b < n_bones else id_slot
            used.add(slot)
            uv = uvs[vi] if uvs is not None else (0.0, 0.0)
            t = texs[vi] if texs is not None else NO_TEX
            if not (0 <= t < len(tex_entries)):
                t = NO_TEX
            verts += struct.pack("<8f2I", p[0], p[1], p[2],
                                 n[0], n[1], n[2], uv[0], uv[1], slot, t)
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
        f.write(b"EMD2")
        f.write(struct.pack("<4If2I", n_bones + 1, vbase, len(indices),
                            len(frames), fps, len(tex_entries), 0))
        for b in range(n_bones):
            p = parents[b] if b < len(parents) else -1
            f.write(struct.pack("<i", p if 0 <= p < n_bones else -1))
        f.write(struct.pack("<i", -1))   # identity slot
        for e in tex_entries:
            f.write(struct.pack("<4I", e["w"], e["h"], e["off"], 0))
        f.write(verts)
        f.write(struct.pack(f"<{len(indices)}I", *indices))
        f.write(pal)
        f.write(tex_blob)

    print(f"wrote {out_path}")
    print(f"  palette : {n_bones}+1 slots ({len(used)} referenced)")
    print(f"  verts   : {vbase}  tris: {len(indices) // 3}")
    print(f"  frames  : {len(frames)} @ {fps} fps")
    print(f"  textures: {len(tex_entries)} ({len(tex_blob)} texel bytes)")


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--mesh", default="extract/chunk21/f17_id8f.bin")
    ap.add_argument("--skel", default="extract/chunk05/f04_id71.bin")
    ap.add_argument("--anim", help="id 0x74 animation-library file (e.g. "
                    "extract/chunk28/f01_id3c.bin): bake clip --clip from "
                    "it into a multi-frame EMDL")
    ap.add_argument("--clip", type=int, default=0,
                    help="container index inside --anim (default 0)")
    ap.add_argument("--live", help="live node-matrix JSON (world matrices, "
                    "node order): export that single captured pose")
    ap.add_argument("--segment", type=int, default=0,
                    help="model segment for MESH_SIG-style files (X-separator"
                    " groups; default 0)")
    ap.add_argument("--gsdump", help="PCSX2 1-frame GS dump (.gs) of a scene "
                    "with this model on screen: source of colored texels "
                    "(VRAM snapshot resolves each marker TEX0's PSMT4 "
                    "indices + CLUT). Without it textures are grey 1x1.")
    ap.add_argument("--out", required=True)
    args = ap.parse_args(argv)

    sections, max_slot, tex_table = load_mesh_sections(Path(args.mesh),
                                                       args.segment)
    nverts = sum(len(s[0]) for s in sections)
    ntris = sum(len(s[2]) for s in sections) // 3
    print(f"mesh: {nverts} verts, {ntris} tris, max node slot {max_slot}, "
          f"{len(tex_table)} textures")
    tex_entries, tex_blob = build_texture_blob(
        Path(args.gsdump) if args.gsdump else None, tex_table)

    fps = FPS
    if args.live:
        frames = recentre(load_live_palette(Path(args.live)))
        parents = [-1] * len(frames[0])
        print(f"palette: 1 live frame, {len(frames[0])} node matrices")
    elif args.anim:
        parents, frames, fps = bake_id74_palettes(Path(args.anim), args.clip)
        frames = recentre(frames)
        print(f"palette: clip {args.clip} of {args.anim} -> "
              f"{len(frames)} frames, {len(frames[0])} nodes @ {fps} fps")
    else:
        # No pose source: identity palette (bone-local parts shown
        # overlapping at the origin).
        n = max_slot + 1
        frames = [[mat_identity()] * n]
        parents = [-1] * n
        print("note: no --live/--anim; exporting identity palette "
              "(bone-local parts will overlap)")

    write_emdl(Path(args.out), sections, [], parents, frames, fps,
               tex_entries, tex_blob)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
