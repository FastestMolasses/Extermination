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

EMDL v3 layout (little-endian; v2 = the same file without the clip
table, magic "EMD2" and no clip_count word — the port still loads it):

  char  magic[4]      "EMD3"
  u32   bone_count    palette slots (nodes + 1 trailing identity slot)
  u32   vert_count    total vertices
  u32   index_count   total triangle indices (u32, global vertex ids)
  u32   frame_count   baked pose frames across ALL clips (>=1)
  f32   fps           default playback rate for the baked frames
  u32   tex_count     textures in the embedded table (0 = untextured)
  u32   flags
  u32   clip_count    named clips sharing the palette blob (>=1)
  i32   parents[bone_count]                  (-1 = root; informational)
  tex   { u32 width, height, byte_offset, reserved } x tex_count
        byte_offset into the RGBA8 texel blob at the end of the file
  clip  { u32 id, first_frame, frame_count; f32 fps } x clip_count
        id = source container index in the id 0x74 animation library;
        first_frame/frame_count = range into the shared palette blob
  vert  { f32 px,py,pz; f32 nx,ny,nz; f32 u,v; u32 bone; u32 tex }
        x vert_count   (tex = index into the table, 0xFFFFFFFF = none;
        UVs are normalized texture coords, REPEAT addressing — values
        outside [0,1] are intentional tiling)
  u32   indices[index_count]
  f32   palette[frame_count][bone_count][16] column-major world matrices
  u8    texels[]      RGBA8, rows top-down, per-texture at byte_offset

Vertices are BONE-LOCAL (exactly as stored on disc); vertex_world =
palette[frame][bone] * pos — the same contract as the PS2 kernel.

MULTI-CLIP (2026-06-10): --clips 346,2,3 bakes several library
containers into one shared palette blob with a clip table. The FIRST
clip's frame-0 root translation is the shared origin (recentre
semantics unchanged for it). Locomotion clips (net root XZ travel
over the clip > 3 units) are converted to IN-PLACE: the root's XZ
translation is subtracted from every node per frame (vertical bob
kept) and the measured natural ground speed is printed — the port
re-applies travel itself and scales playback rate to its own speed.
Player walk/run identified 2026-06-10 by stride scan (feet = nodes
17/18, anti-phase fore-aft swing + foot lift + root travel, all
locomotion clips head exactly +Z): clip 2 = WALK (45 frames,
24.07 u/s), clip 3 = RUN (40 frames, 47.57 u/s); the (12,13)/(22,23)
pairs are other-stance variants (upper body further from the idle/
rifle pose), 346 stays the idle/look-around used since EMDL v2.

TEXTURE/COLOR (decoded 2026-06-09 s5): vertex record qword 0 ("marker")
IS the draw's TEX0 register value — TBP0/TBW/PSM/TW/TH/CBP baked into
the disc file (validated: all 51 (TBP0,CBP) pairs of f00_id3b appear
verbatim in a live GS dump's per-draw TEX0 stream; the two variants per
pair differ only in CLD). Qword 1 holds the vertex's normalized ST.
Color requires a one-frame PCSX2 GS dump (--gsdump) whose VRAM snapshot
supplies the PSMT4 texels + 16-entry CLUTs (palettes are runtime-built;
see docs/FINDINGS.md "Texture COLOR recovered").

Usage (macOS arm64, repo root):
  # THE player export (multi-clip + held weapons; --attach reuses
  # export_props' equipment merge, the same machinery as
  # `export_props.py --attach`; --no-glow skips the aura quads — the
  # s43 fidelity call, see the export_props docstring; full clip list
  # = the s36 superset, idle = DIRECTORY id 349 after the 2026-06-11
  # directory-resolution fix — the old "346" was the same content
  # under the shifted enumeration; ,70,68 = the door LOCKED-TRY
  # gestures 0x46/0x44 appended 2026-06-11 for em_door's locked
  # sequence — verified limbs-only try-and-fail clips, peak node
  # deviation 5.4/6.6 u around f64-68, exact return to rest at f199;
  # ,30,31,32,33,86,87,42,92,452,455 = the PLAYER DAMAGE/DEATH set
  # (2026-06-11 damage-pipeline decode, FINDINGS "PLAYER DAMAGE"):
  # unarmed flinches 0x1E/0x1F (35 f) + 0x20/0x21 (50 f), armed
  # flinches 0x56/0x57 (30/50 f, aim posture), normal death 0x2A
  # (130 f fall-to-ground, head y 14.6 -> 1.4), armed death 0x5C
  # (130 f), infected death 0x1C4 (300 f succumb), infected flinch
  # 0x1C7 (90 f) — all motion-audited fall/stagger shapes.
  # ,54,94,115,375 = the four "non-sentinel" clips the old baker
  # skipped (s76 baker fix; FINDINGS "NON-SENTINEL ANIM CONTAINER" +
  # "WOODEN CRATE … unused player clips"): id 54 (60 f, arm-raised
  # look-up/reach), 94 (20 f, crouch/duck), 115 (20 f, reach-to-back
  # equip), 375 (80 f, two-handed weapon inspect). REAL animations (a
  # research agent wrongly called them vestigial — confirmed real via
  # EM_CLIP_DEMO). Baked-but-UNWIRED: the port plays none of them yet
  # (their trigger states are undecoded — a follow-up); they ride the
  # asset byte-safely (the player render resolves clips by id, so the
  # default capture stays byte-identical):
  .venv/bin/python tools/export_native.py --attach --no-glow \
      --mesh extract/chunk28/f00_id3b.bin \
      --anim extract/chunk28/f01_id3c.bin \
      --clips 349,2,3,69,67,75,272,273,283,51,274,275,276,277,278,279,280,281,282,1,267,268,269,270,271,0,450,10,70,68,30,31,32,33,86,87,42,92,452,455,54,94,115,375 \
      --gsdump extract/gsdump/frame1.gs \
      --out ../extermination-port/assets/player.emdl
  # ANIMATED export: bake clip N from an id 0x74 animation library file
  # (channel encodings decoded 2026-06-09 s4 — see FINDINGS.md):
  python3 tools/export_native.py --mesh extract/chunk28/f00_id3b.bin \
      --anim extract/chunk28/f01_id3c.bin --clip 346 \
      --out ../extermination-port/assets/player.emdl
  # ENEMY/CREATURE from an ENCOUNTER PACKAGE (multi-segment file with an
  # IN-FILE animation bank — blob ids 0x70/0xd0/... that the default 0x74
  # enumeration skips; see FINDINGS.md "encounter package"): pick the mesh
  # segment, then select clips on the matching in-file parent table with
  # --rig-nodes (= segment max_slot + 1) or pin one with --anim-hdr:
  python3 tools/export_native.py --mesh extract/chunk21/f17_id8f.bin \
      --segment 1 --anim extract/chunk21/f17_id8f.bin --rig-nodes 44 \
      --clip 4 --out ../extermination-port/assets/enemy_test.emdl
  # TEXTURED NPC with no GS dump of its level: resolve texels/CLUTs from a
  # PCSX2 save state's VRAM instead (--p2s; the mesh TEX0 keys must be
  # resident in that state — chunk15/f18_id94 seg 1 is 68/68 in state 01).
  # The rig can live in a SIBLING file (chunk15: f12_id44.bin), and one
  # file can hold several same-node-count rigs — pick the parent-table
  # family whose posed cross-bone edges are shortest (see FINDINGS s14):
  .venv/bin/python tools/export_native.py --mesh extract/chunk15/f18_id94.bin \
      --segment 1 --anim extract/chunk15/f12_id44.bin --rig-nodes 21 \
      --clip 7 --p2s "$HOME/Library/Application Support/PCSX2/sstates/SCUS-97112 (4CDC5F74).01.p2s" \
      --out ../extermination-port/assets/npc_test.emdl
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
import math
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
    blob_nodes = None
    if struct.unpack_from("<I", d, 0)[0] < 0x1000 and \
            d[0x48:0x50] == bytes.fromhex("040400010080806c"):
        payloads, blob_nodes = _walk_blob_blocks(d)
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
            if slot < 2 and blob_nodes != 1:
                # packed character meshes never address dmem qw < 16
                # (slots 0/1 are kernel scratch) -- records there are
                # junk. EXCEPTION: single-node raw blobs (the per-area
                # MODEL-TABLE entries bound via *(D_0028A59C), e.g. the
                # placed-crawler CRATE husk, AREA11 id 0x0D): the prop
                # kernel keeps its one matrix set at dmem qw 0, W is
                # +/-1.0 with flag bits 14/15 only, so every vertex is
                # slot 0. (FINDINGS "CRAWLER RESOLVED", 2026-06-10.)
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


def scan_anim_containers(d: bytes, rig_nodes: int | None = None) -> list[int]:
    """Header offsets of EVERY keyed-animation container in `d`, any blob
    id (rig_probe's id-agnostic anchor scan). extract_models'
    find_id74_headers only matches blob ids 0x74/0x2c and therefore misses
    in-file enemy/creature banks (e.g. chunk21/f17_id8f's 11 id-0x70 +
    30 id-0xd0 clips at 0x53030..0xc4e30). `rig_nodes` filters to
    containers whose parent table has exactly that many nodes — pair a
    mesh segment with the in-file table where rig_nodes == max_slot + 1
    (FINDINGS.md "encounter package")."""
    rp = _load("_rig_probe", "rig_probe.py")
    return [h["hdr"] for h in rp.scan_anim_headers(d)
            if rig_nodes is None or h["n"] == rig_nodes]


def anim_directory(d: bytes) -> list[int] | None:
    """The engine's container DIRECTORY of a clip-library file, when the
    file has one (anim_clip_resolve / func_001C6120, FINDINGS "ANIM ID
    MAPPING" + 2026-06-11 correction): leading u32 COUNT, then COUNT u32
    entries; `entry & ~3` = the byte offset of container id i (low 2 bits
    are flags). **Anim ids are DIRECTORY indices** — NOT enumeration-scan
    indices. The historic find_id74_headers scan order silently skips
    containers whose +0x4 halfword is not the 0xFFFE/0xFFFF sentinel
    (chunk28/f01_id3c has 4 such: directory ids 54/94/115/375), shifting
    every scan index >= 54 off the engine's id by up to +3 — the door
    "open" clips 69/67 actually baked the LOCKED tries 70/68 until this
    resolver landed. Returns the cleaned offset list, or None when the
    file does not start with a plausible directory (in-file banks)."""
    if len(d) < 8:
        return None
    cnt = struct.unpack_from("<I", d, 0)[0]
    if not (1 <= cnt <= 0x1000) or 4 + 4 * cnt > len(d):
        return None
    offs = [o & ~3 for o in struct.unpack_from(f"<{cnt}I", d, 4)]
    prev = 4 + 4 * cnt - 4          # first entry may be pad-aligned up
    for o in offs:
        if o < prev or o + 0x28 > len(d):
            return None
        prev = o
    return offs


def _is_anim_container(d: bytes, hdr: int) -> bool:
    """Structural check that `hdr` points at a keyed-animation container,
    INDEPENDENT of the +0x04 0xFFFE/0xFFFF metadata sentinel that
    rig_probe.scan_anim_headers keys on. A directory (anim_directory) is
    the engine's own authoritative container list, so a directory-resolved
    offset is baked through this looser gate — it admits the s45
    "non-sentinel header variant" (+0x04 = a clip-chain link, +0x06 = a
    category; populated instead of the null 0xFFFF/0x0000). The pose data
    — bone_count, the per-bone section directories, the keyframe streams —
    is structurally identical, so these bake the same (verified s76: bug
    clip bank ids 14/16/20/22/23/27/33/34 all decode to full 15-bone,
    unit-quat poses). Header: u16 bone_count(+0), u16 clip_len(+2), then
    u32 section offsets at +8/+C/+10 (rotation/translation/event)."""
    if hdr < 0 or hdr + 0x18 > len(d):
        return False
    bc, clen = struct.unpack_from("<HH", d, hdr)
    s1, s2, s3 = struct.unpack_from("<III", d, hdr + 8)
    if not (1 <= bc <= 0x80 and 1 <= clen <= 0x400):
        return False
    if not (0x14 <= s1 < s2 < s3 and hdr + s3 <= len(d)):
        return False
    if hdr + s1 + 4 * bc > len(d):
        return False
    seclen = s2 - s1                 # every section-1 dir entry inside sec 1
    return all(struct.unpack_from("<I", d, hdr + s1 + 4 * i)[0] < seclen
               for i in range(bc))


def bake_id74_palettes(anim_path: Path, clip: int,
                       rig_nodes: int | None = None,
                       anim_hdr: int | None = None):
    """Bake one keyed-animation container (mesh-companion library OR
    in-file bank) into per-frame world-matrix palettes.

    Channel encodings (decoded + live-verified 2026-06-09 s4): rotation =
    4x20-bit truncated-float local quat (x,y,z,w), translation/scale =
    3x26-bit vec3, sparse keys with lerp semantics. The engine composes
    world = parent_world * local with the local rotation built from the
    CONJUGATE of the stored quat (pinned against live world matrices:
    conj gives 0.008 max element error vs 15.2 for the direct quat).

    Container selection: `anim_hdr` picks the container at that file
    offset directly. Otherwise, when the file has a leading container
    DIRECTORY (anim_directory — clip libraries like chunk28/f01_id3c or
    the slot-0x39 bank), `clip` is the DIRECTORY index = the engine's
    anim id (2026-06-11 fix: the old enumeration order skipped 4
    non-sentinel containers and shifted every player-library index >= 54
    by up to +3 — door open ids 0x45/0x43 actually baked the locked
    tries). Only files with NO directory (in-file enemy banks) fall back
    to the historic find_id74_headers enumeration; `rig_nodes` switches
    to the id-agnostic whole-file scan filtered to that node count.

    Returns (parents, frames, fps): frames = [[mat4 x n] x clip_len].
    Clips advance one frame per 60 Hz tick on the PS2 (live node cursor
    rate = 1/(clip_len-1) per tick), so fps = 60.
    """
    em = _load("_extract_models", "extract_models.py")
    d = anim_path.read_bytes()
    if anim_hdr is not None:
        hdr = anim_hdr
        if not any(h == hdr for h in scan_anim_containers(d)):
            raise SystemExit(f"--anim-hdr {hdr:#x}: no animation container "
                             f"header at that offset in {anim_path.name}")
    elif rig_nodes is None and (dirs := anim_directory(d)) is not None:
        # Clip-library file with a leading directory: resolve the way the
        # engine does — `clip` IS the anim id / directory index (see
        # anim_directory; the historic scan order is shifted >= id 54).
        if not (0 <= clip < len(dirs)):
            raise SystemExit(f"clip {clip} out of range (directory has "
                             f"{len(dirs)} containers in {anim_path.name})")
        hdr = dirs[clip]
        # The directory is authoritative — bake at its offset directly,
        # through the structural gate (NOT the +0x04 sentinel signature
        # that scan_anim_containers requires). This admits the s45/s76
        # "non-sentinel header variant" (the bug bank's ids 14/16/20/22/
        # 23/27/33/34 and the player library's 54/94/115/375): the metadata
        # at +0x04/+0x06 is populated, but the pose streams are identical
        # and bake fine. (Was a wrongful bail — the "undecoded format" wall.)
        if not _is_anim_container(d, hdr):
            raise SystemExit(
                f"clip {clip}: directory offset {hdr:#x} is not a parseable "
                f"animation container in {anim_path.name}")
    else:
        if rig_nodes is None:
            hdrs = em.find_id74_headers(d)
            what = "id 0x74"
        else:
            hdrs = scan_anim_containers(d, rig_nodes)
            what = f"{rig_nodes}-node"
        if not hdrs:
            raise SystemExit(f"no {what} animation containers in {anim_path}")
        if not (0 <= clip < len(hdrs)):
            raise SystemExit(f"clip {clip} out of range (0..{len(hdrs) - 1} "
                             f"{what} containers in {anim_path.name})")
        hdr = hdrs[clip]
    pre = em.parse_id74_prefix(d, hdr)
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


def recentre(frames, origin=None):
    """Subtract a shared origin (default: frame-0 root translation) so the
    character sits at the origin (the port supplies its own camera/world
    placement)."""
    ox, oy, oz = origin if origin is not None else frames[0][0][3][:3]
    out = []
    for fr in frames:
        nf = []
        for m in fr:
            c3 = (m[3][0] - ox, m[3][1] - oy, m[3][2] - oz, m[3][3])
            nf.append((m[0], m[1], m[2], c3))
        out.append(nf)
    return out


# Net root XZ travel above this (units) marks a clip as locomotion: its
# baked root motion is stripped so it plays in place (the port drives the
# world translation itself). The player is ~15 units tall; locomotion
# clips travel 17..31 units, while idle/look-around roots drift < 1.
LOCO_TRAVEL_MIN = 3.0


def strip_root_motion(frames):
    """Make a locomotion clip play IN PLACE: subtract the root's XZ
    translation from every node, per frame (the vertical bob is kept).
    Returns (frames, speed) with speed = the clip's natural ground speed
    in units/sec at the baked 60 fps (the port scales playback by
    move_speed / speed so the feet track the ground)."""
    out = []
    for fr in frames:
        rx, rz = fr[0][3][0], fr[0][3][2]
        nf = []
        for m in fr:
            c3 = (m[3][0] - rx, m[3][1], m[3][2] - rz, m[3][3])
            nf.append((m[0], m[1], m[2], c3))
        out.append(nf)
    dx = frames[-1][0][3][0] - frames[0][0][3][0]
    dz = frames[-1][0][3][2] - frames[0][0][3][2]
    speed = math.hypot(dx, dz) / max(len(frames) - 1, 1) * 60.0
    return out, speed


def root_travel(frames):
    dx = frames[-1][0][3][0] - frames[0][0][3][0]
    dz = frames[-1][0][3][2] - frames[0][0][3][2]
    return math.hypot(dx, dz)


def translate_frames(frames, off):
    """Bake a constant world translation into every palette matrix (used
    to place a scene-file export off to the side — the port poses scene
    EMDLs verbatim at frame 0, no per-item placement)."""
    ox, oy, oz = off
    out = []
    for fr in frames:
        out.append([(m[0], m[1], m[2],
                     (m[3][0] + ox, m[3][1] + oy, m[3][2] + oz, m[3][3]))
                    for m in fr])
    return out


def bake_id74_clips(anim_path: Path, clip_ids: list[int],
                    rig_nodes: int | None = None):
    """Bake several animation containers into one shared palette blob.

    The FIRST clip's frame-0 root translation is the shared origin (so a
    single-clip 346 export reproduces the old recentre output exactly);
    locomotion clips are converted to in-place (see strip_root_motion).
    `rig_nodes` switches the enumeration the indices refer to (see
    bake_id74_palettes).

    Returns (parents, frames, clip_table, fps) with clip_table =
    [{id, first, count, fps}] ranges into the concatenated frames."""
    parents = None
    frames: list = []
    clip_table = []
    origin = None
    fps = 60.0
    for ci in clip_ids:
        p, fr, fps = bake_id74_palettes(anim_path, ci, rig_nodes)
        if parents is None:
            parents = p
        elif p != parents:
            raise SystemExit(f"clip {ci}: parent table differs from clip "
                             f"{clip_ids[0]} — not the same rig")
        if origin is None:
            origin = fr[0][0][3][:3]
        fr = recentre(fr, origin)
        note = ""
        travel = root_travel(fr)
        if travel > LOCO_TRAVEL_MIN:
            fr, speed = strip_root_motion(fr)
            note = (f" — locomotion (root travel {travel:.1f}u) -> "
                    f"in-place, natural speed {speed:.2f} u/s")
        clip_table.append({"id": ci, "first": len(frames),
                           "count": len(fr), "fps": fps})
        frames.extend(fr)
        print(f"  clip {ci}: {len(fr)} frames @ {fps:.0f} fps{note}")
    return parents, frames, clip_table, fps


def build_texture_blob(gsdump: Path | None, tex_table: list[dict],
                       p2s: Path | None = None):
    """Resolve each mesh TEX0 to RGBA8 texels via a VRAM snapshot
    (PSMT4/PSMT8 indices + runtime-built CLUTs both live there).
    Without a source every texture is a 1x1 mid-grey placeholder.

    Two VRAM sources (mutually exclusive; `gsdump` wins if both given):
      gsdump — PCSX2 1-frame GS dump (.gs): replayed register writes.
      p2s    — PCSX2 save state (.p2s, or a pre-extracted state dir,
               or a bare gs.bin freeze blob): the 4 MB GS local memory
               inside the GS freeze component, base = len(gs.bin) -
               0x400000 - 84 (gs_vram.read_localmem; layout proven
               2026-06-09, see clut_pair.py). Useful when no GS dump of
               a level exists but a save state does — texture residency
               is the caller's responsibility (mesh TEX0 TBP0/CBP are
               read as-is; a state of the wrong level yields garbage).

    Returns (entries, blob): entries = [{w, h, off}] parallel to
    tex_table."""
    entries, blob = [], bytearray()
    lm = None
    if gsdump is not None:
        pg = _load("_parse_gsdump", "parse_gsdump.py")
        state_data, _regs, _pkts, _serial, _crc = pg.parse(gsdump, quiet=True)
        lm = pg.dump_vram(state_data)
    elif p2s is not None:
        gv = _load("_gs_vram", "gs_vram.py")
        if p2s.is_dir():                       # pre-extracted state dir
            gs_path = p2s / "gs.bin"
        elif p2s.suffix.lower() == ".p2s":     # save state: pull gs.bin
            import tempfile
            pps = _load("_parse_pcsx2_state", "parse_pcsx2_state.py")
            tmp = Path(tempfile.mkdtemp(prefix="emdl_p2s_"))
            gs_path = pps.extract_all(p2s, tmp)["gs.bin"]
        else:                                  # bare gs.bin freeze blob
            gs_path = p2s
        _base, lm = gv.read_localmem(gs_path)
    if lm is not None:
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
               fps: float, tex_entries=None, tex_blob=b"", flags=0,
               clips=None):
    """Vertices carry their own palette slot (per-vertex bone, decoded from
    the position-W dmem address — see load_mesh_sections). A trailing
    identity slot soaks up any vertex whose slot exceeds the palette.

    `flags` lands in the header's flags word (bit 0: the normal slot
    carries a baked vertex COLOR — static level geometry; see
    export_level.py). Character exports keep the default 0.

    `clips` = [{id, first, count, fps}] ranges into `frames` (see
    bake_id74_clips); None = one clip covering everything (single-pose
    and single-animation exports).

    Per-vertex FLAGS (2026-06-10): bits 24..31 of a vertex's bone value
    pass through into the stored bone word (bits 0..23 stay the palette
    slot). Bit 31 = BILLBOARD+ADDITIVE glow part (camera-facing quad,
    additive blend, no depth write — see export_props.GLOW_ATTACHMENTS
    and the port's em_model.h). Plain slot values are unchanged, so every
    existing producer/consumer is unaffected."""
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
            vflags = (b & 0xFF000000) if b >= 0 else 0   # bits 24..31
            b = (b & 0x00FFFFFF) if b >= 0 else b
            slot = b if 0 <= b < n_bones else id_slot
            used.add(slot)
            uv = uvs[vi] if uvs is not None else (0.0, 0.0)
            t = texs[vi] if texs is not None else NO_TEX
            if not (0 <= t < len(tex_entries)):
                t = NO_TEX
            verts += struct.pack("<8f2I", p[0], p[1], p[2],
                                 n[0], n[1], n[2], uv[0], uv[1],
                                 slot | vflags, t)
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

    if clips is None:
        clips = [{"id": 0, "first": 0, "count": len(frames), "fps": fps}]
    for c in clips:
        if not (0 <= c["first"] and c["first"] + c["count"] <= len(frames)
                and c["count"] >= 1):
            raise SystemExit(f"clip table entry out of range: {c}")

    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("wb") as f:
        f.write(b"EMD3")
        f.write(struct.pack("<4If3I", n_bones + 1, vbase, len(indices),
                            len(frames), fps, len(tex_entries), flags,
                            len(clips)))
        for b in range(n_bones):
            p = parents[b] if b < len(parents) else -1
            f.write(struct.pack("<i", p if 0 <= p < n_bones else -1))
        f.write(struct.pack("<i", -1))   # identity slot
        for e in tex_entries:
            f.write(struct.pack("<4I", e["w"], e["h"], e["off"], 0))
        for c in clips:
            f.write(struct.pack("<3If", c["id"], c["first"], c["count"],
                                c["fps"]))
        f.write(verts)
        f.write(struct.pack(f"<{len(indices)}I", *indices))
        f.write(pal)
        f.write(tex_blob)

    print(f"wrote {out_path}")
    print(f"  palette : {n_bones}+1 slots ({len(used)} referenced)")
    print(f"  verts   : {vbase}  tris: {len(indices) // 3}")
    print(f"  frames  : {len(frames)} @ {fps} fps")
    print(f"  clips   : " + ", ".join(
        f"{c['id']}[{c['first']}..+{c['count']}]" for c in clips))
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
    ap.add_argument("--rig-nodes", type=int,
                    help="select animation containers by parent-table size "
                    "via the id-agnostic whole-file scan (in-file enemy "
                    "banks use blob ids 0x70/0xd0/... that the default "
                    "0x74-library enumeration skips); --clip/--clips then "
                    "index ONLY the matching containers. Pick the size as "
                    "mesh max_slot + 1 (printed; FINDINGS 'encounter "
                    "package' pairing rule)")
    ap.add_argument("--anim-hdr", type=lambda x: int(x, 0),
                    help="container header file offset inside --anim "
                    "(hex ok): bake exactly that container, bypassing "
                    "--clip/--rig-nodes indexing")
    ap.add_argument("--offset", help="x,y,z world translation baked into "
                    "every palette matrix after recentring (scene-file "
                    "placement; the port poses scene EMDLs verbatim)")
    ap.add_argument("--clips", help="comma list of container indices to "
                    "bake into ONE multi-clip EMDL (e.g. 346,2,3 = "
                    "idle,walk,run); locomotion clips are converted to "
                    "in-place (see bake_id74_clips)")
    ap.add_argument("--attach", action="store_true",
                    help="merge the held equipment onto the skeleton nodes "
                    "(reuses export_props.build_attached_player — the same "
                    "machinery as `export_props.py --attach`)")
    ap.add_argument("--no-glow", action="store_true",
                    help="(--attach) skip the export_props GLOW_ATTACHMENTS "
                    "aura quads (recommended for player.emdl — the quad "
                    "stand-in over-reads as a solid green sheet; see the "
                    "export_props docstring)")
    ap.add_argument("--library", default="extract/chunk27/f01_id37.bin",
                    help="(--attach) equipment model library")
    ap.add_argument("--live", help="live node-matrix JSON (world matrices, "
                    "node order): export that single captured pose")
    ap.add_argument("--segment", type=int, default=0,
                    help="model segment for MESH_SIG-style files (X-separator"
                    " groups; default 0)")
    ap.add_argument("--gsdump", help="PCSX2 1-frame GS dump (.gs) of a scene "
                    "with this model on screen: source of colored texels "
                    "(VRAM snapshot resolves each marker TEX0's PSMT4 "
                    "indices + CLUT). Without it textures are grey 1x1.")
    ap.add_argument("--p2s", help="PCSX2 save state (.p2s, or a "
                    "pre-extracted state dir, or a bare gs.bin freeze "
                    "blob) as the VRAM texel/CLUT source instead of "
                    "--gsdump — for levels with a save state but no GS "
                    "dump (the model's TEX0 keys must be resident in "
                    "that state's VRAM; see build_texture_blob)")
    ap.add_argument("--out", required=True)
    args = ap.parse_args(argv)

    if args.attach:
        # Reuse the export_props equipment merge verbatim (ATTACHMENTS:
        # rifle models -> node 4, knife 106 -> node 14 holster; identity
        # local offsets — see export_props.py for the live-frame proof).
        # export_props also resolves textures via export_level's blob
        # builder (PSMCT32-capable), exactly as `export_props.py --attach`.
        if args.p2s:
            raise SystemExit("--p2s is not supported with --attach (that "
                             "path uses export_level's PSMCT32-capable "
                             "blob builder); use --gsdump")
        props = _load("_export_props", "export_props.py")
        sections, max_slot, tex_table = props.build_attached_player(args)
        # finish_textures = build_texture_blob + the glow-layer tints
        # (export_props.GLOW_ATTACHMENTS adds tinted billboard quads).
        tex_entries, tex_blob = props.finish_textures(args, tex_table)
    else:
        sections, max_slot, tex_table = load_mesh_sections(Path(args.mesh),
                                                           args.segment)
        nverts = sum(len(s[0]) for s in sections)
        ntris = sum(len(s[2]) for s in sections) // 3
        print(f"mesh: {nverts} verts, {ntris} tris, max node slot {max_slot},"
              f" {len(tex_table)} textures")
        tex_entries, tex_blob = build_texture_blob(
            Path(args.gsdump) if args.gsdump else None, tex_table,
            Path(args.p2s) if args.p2s else None)

    fps = FPS
    clips = None
    if args.clips:
        ids = [int(x) for x in args.clips.split(",") if x.strip() != ""]
        if not args.anim:
            raise SystemExit("--clips needs --anim")
        print(f"baking {len(ids)} clips from {args.anim}:")
        parents, frames, clips, fps = bake_id74_clips(Path(args.anim), ids,
                                                      args.rig_nodes)
        print(f"palette: {len(frames)} total frames, {len(frames[0])} nodes "
              f"@ {fps} fps")
    elif args.live:
        frames = recentre(load_live_palette(Path(args.live)))
        parents = [-1] * len(frames[0])
        print(f"palette: 1 live frame, {len(frames[0])} node matrices")
    elif args.anim:
        parents, frames, fps = bake_id74_palettes(Path(args.anim), args.clip,
                                                  args.rig_nodes,
                                                  args.anim_hdr)
        frames = recentre(frames)
        # clip id: container index, or the header offset for --anim-hdr
        cid = args.anim_hdr if args.anim_hdr is not None else args.clip
        clips = [{"id": cid, "first": 0, "count": len(frames),
                  "fps": fps}]
        sel = (f"hdr {args.anim_hdr:#x}" if args.anim_hdr is not None
               else f"clip {args.clip}"
               + (f" ({args.rig_nodes}-node scan)" if args.rig_nodes
                  else ""))
        print(f"palette: {sel} of {args.anim} -> "
              f"{len(frames)} frames, {len(frames[0])} nodes @ {fps} fps")
    else:
        # No pose source: identity palette (bone-local parts shown
        # overlapping at the origin).
        n = max_slot + 1
        frames = [[mat_identity()] * n]
        parents = [-1] * n
        print("note: no --live/--anim; exporting identity palette "
              "(bone-local parts will overlap)")

    if args.offset:
        off = tuple(float(x) for x in args.offset.split(","))
        if len(off) != 3:
            raise SystemExit("--offset needs x,y,z")
        frames = translate_frames(frames, off)
        print(f"baked world offset {off}")

    write_emdl(Path(args.out), sections, [], parents, frames, fps,
               tex_entries, tex_blob, clips=clips)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
