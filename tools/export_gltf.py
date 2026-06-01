#!/usr/bin/env python3
"""
export_gltf.py -- Export an Extermination (SCUS-97112) character as a glTF 2.0
binary (.glb) bundle containing:

  * the skeleton hierarchy (parent table from id 0x71 entry 0),
  * one rigid mesh per bone (the bone's Q4.12 object-space vertex packet),
  * one animation per id 0x71 entry (rotation + translation tracks per bone).

The per-bone mesh is parented directly to its bone node ("node-attached" rigid
skinning) -- no glTF `skin` object is needed because every vertex belongs
entirely to its parent bone with weight 1.0. The bone node's TRS is what
animates; the mesh moves rigidly with the node.

USAGE
    python3 tools/export_gltf.py \
        --mesh extract/chunk21/f17_id8f.bin \
        --skel extract/chunk05/f04_id71.bin \
        --out  models/Extermination_Player.glb

If --skel is omitted, the script auto-pairs the first id 0x71 file under
extract/ whose bone count >= the mesh's per-bone section count.

The output .glb opens in Blender (File > Import > glTF 2.0), Maya 2022+, and
any compliant viewer (gltf-viewer, three.js, Babylon Sandbox, etc.). All
57 clips show up under the file's Animations panel.

LEVEL MODE
    python3 tools/export_gltf.py level \
        --level extract/chunk04.n0/f06_id44.bin \
        --out   models/chunk04.n0_f06_scene.glb

Exports an id 0x44 level file as a single placed, textured scene .glb:
world-space MESH/SUBMESH geometry plus MATRIX-instanced props (transforms
baked in), grouped into one mesh per texture sheet, textures resolved across
the extract tree and embedded as RGBA PNGs. `--all-levels` batch-exports every
id 0x44 file; `--no-textures` emits geometry only. See build_level_glb().
"""

from __future__ import annotations

import argparse
import importlib.util
import json
import struct
import sys
from pathlib import Path
from typing import List, Tuple


# ---------------------------------------------------------------------------
# Load sibling modules without importing extract_models.py wholesale (it has
# its own argparse-heavy entry point).

_HERE = Path(__file__).resolve().parent

# Ensure sibling modules that use bare imports (e.g. extract_subtextures.py's
# `from clut import ...`) resolve regardless of how this script is launched.
if str(_HERE) not in sys.path:
    sys.path.insert(0, str(_HERE))


def _load(name: str, fname: str):
    if name in sys.modules:
        return sys.modules[name]
    spec = importlib.util.spec_from_file_location(name, _HERE / fname)
    mod = importlib.util.module_from_spec(spec)
    sys.modules[name] = mod
    spec.loader.exec_module(mod)
    return mod


ad = _load("_anim_decoder", "anim_decoder.py")
em = _load("_extract_models", "extract_models.py")
est = _load("_extract_subtextures", "extract_subtextures.py")
vr = _load("_vram_residency", "vram_residency.py")


# ---------------------------------------------------------------------------
# Global GS VRAM residency map
#
# Textures are very commonly uploaded by a DIFFERENT file than the geometry
# that samples them ("cross-file texture residency"). Rather than scan each
# mesh's chunk dir then the whole tree ad-hoc per export, we build ONE
# residency map for the whole disc once (cached under scratch/) and resolve any
# DBP against it. The map snaps near-miss DBPs to the same physical sheet (the
# affine sheet_field->DBP relation is only exact on the universal slot trio).
# See tools/vram_residency.py.
# ---------------------------------------------------------------------------
_RESIDENCY: "vr.ResidencyMap | None" = None


def _residency(search_root: Path | None) -> "vr.ResidencyMap | None":
    """Lazily build (and cache) the global residency map for `search_root`."""
    global _RESIDENCY
    if search_root is None or not search_root.is_dir():
        return None
    if _RESIDENCY is None or _RESIDENCY.root != search_root:
        try:
            _RESIDENCY = vr.ResidencyMap.load_or_scan(search_root)
        except Exception:  # noqa: BLE001 -- never let texture lookup break export
            return None
    return _RESIDENCY


# ---------------------------------------------------------------------------
# Texture sheet resolution and PNG encoding
#
# Each character mesh's per-strip marker `m0` carries a `sheet_field`
# (`(m0 >> 15) & 0x3FFF`) that names a GS VRAM base, which maps to a
# texture-upload BITBLTBUF DBP via the affine
#     sheet_field = DBP * (2048/1920) - 584.8
# (see docs/FINDINGS.md "Material -> texture binding"). The player rig
# references three universal-slot DBPs {10752, 12672, 14592}. We resolve
# each DBP by scanning the mesh's sibling files (same chunk dir) plus an
# optional fall-back search root for the first GS texture upload matching
# that DBP, then decode the PSMT8 sheet via extract_subtextures and write
# it as an RGBA PNG using the identity-grayscale CLUT (full-color binding
# is still unresolved -- see docs/FINDINGS.md "Color source").
# ---------------------------------------------------------------------------


def _sheet_field_to_dbp(sf: int) -> int:
    return round((sf + 584.8) / (2048.0 / 1920.0))


def _find_transfer_for_dbp(mesh_path: Path, dbp: int,
                           search_root: Path | None) -> tuple[Path, "est.Transfer"] | None:
    """Locate the best GS texture upload for `dbp`, disc-wide.

    Consults the global VRAM residency map first (resolves cross-file uploads
    and snaps near-miss DBPs to the same physical sheet), preferring the mesh's
    own chunk dir. Falls back to the legacy same-dir-then-tree scan only if the
    residency map is unavailable or finds nothing.
    """
    rm = _residency(search_root)
    if rm is not None:
        up = rm.resolve_dbp(dbp, prefer_dir=mesh_path.parent)
        if up is not None:
            src = rm.root / up.src_rel
            try:
                d = src.read_bytes()
            except OSError:
                d = None
            if d is not None:
                t = est.Transfer(src.name, up.dbp, up.tw, up.th, up.trxreg_off,
                                 src_path=src)
                if est.decode_transfer(d, t):
                    return (src, t)

    # Legacy fallback: same-chunk preference, then the whole tree.
    candidates: list[Path] = []
    candidates.extend(sorted(mesh_path.parent.glob("*.bin")))
    seen = set(candidates)
    if search_root and search_root.is_dir():
        for p in sorted(search_root.rglob("*.bin")):
            if p not in seen:
                candidates.append(p)
    for f in candidates:
        try:
            d = f.read_bytes()
        except Exception:
            continue
        xfers = est.scan_transfers(d, f.name)
        for t in xfers:
            if t.dbp == dbp:
                if est.decode_transfer(d, t):
                    return (f, t)
    return None


def _png_rgba_bytes(width: int, height: int, indexed: bytes) -> bytes:
    """8-bit indexed pixels -> RGBA PNG bytes (identity grayscale CLUT)."""
    import zlib as _zlib

    def chunk(typ: bytes, data: bytes) -> bytes:
        return (struct.pack(">I", len(data)) + typ + data +
                struct.pack(">I", _zlib.crc32(typ + data) & 0xFFFFFFFF))

    ihdr = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)  # 6 = RGBA
    raw = bytearray()
    for y in range(height):
        raw.append(0)  # filter byte
        row = indexed[y * width:(y + 1) * width]
        for px in row:
            raw.append(px); raw.append(px); raw.append(px); raw.append(0xFF)
    return (b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", ihdr) +
            chunk(b"IDAT", _zlib.compress(bytes(raw), 6)) +
            chunk(b"IEND", b""))


# ---------------------------------------------------------------------------
# id 0x71 entry parsing


def _entry_slices(id71_bytes: bytes) -> List[Tuple[int, int]]:
    n = struct.unpack_from("<I", id71_bytes, 0)[0]
    offs = list(struct.unpack_from(f"<{n + 1}I", id71_bytes, 4))
    out = []
    for i in range(n):
        s = offs[i]
        e = offs[i + 1]
        if e == 0xFFFFFFFF or e > len(id71_bytes):
            e = len(id71_bytes)
        out.append((s, e))
    return out


def _decode_entry_streams(entry: bytes):
    """Return (bc, parents_raw, rot_streams, trn_streams)."""
    bc = entry[0]
    s1 = struct.unpack_from("<I", entry, 0x08)[0]
    s2 = struct.unpack_from("<I", entry, 0x0c)[0]
    s3 = struct.unpack_from("<I", entry, 0x10)[0]
    parents = list(struct.unpack_from(f"<{bc}I", entry, 0x28))
    sec1 = entry[s1:s2]
    sec2 = entry[s2:s3]
    rot = ad.parse_rotation_section(sec1, bc)
    trn = ad.parse_translation_section(sec2, bc)
    return bc, parents, rot, trn


def _signed_parents(raw: List[int]) -> List[int]:
    n = len(raw)
    return [-1 if (p == i or p >= n) else p for i, p in enumerate(raw)]


# ---------------------------------------------------------------------------
# Mesh + bone section extraction (re-uses extract_models helpers).


def load_per_bone_meshes(mesh_path: Path):
    """Return list of per-bone vertex lists: [[(x,y,z), ...], ...].
    Index in the list = bone index (matches the mesh's section table)."""
    d = mesh_path.read_bytes()
    table = em._find_bone_section_table(d)
    if table is None:
        raise RuntimeError(f"no per-bone VIF section table in {mesh_path}")
    table_off, entries = table
    out: List[List[Tuple[float, float, float]]] = []
    for i, off in enumerate(entries):
        sect_start = table_off + off
        sect_end = (table_off + entries[i + 1]
                    if i + 1 < len(entries) else len(d))
        verts = em.decode_objspace_bone_vertices(d, sect_start, sect_end)
        out.append([(x, y, z) for (x, y, z, _vid) in verts])
    return out


def load_per_bone_meshes_with_vids(mesh_path: Path):
    """Return list of [(x, y, z, vid), ...] per bone."""
    d = mesh_path.read_bytes()
    table = em._find_bone_section_table(d)
    if table is None:
        raise RuntimeError(f"no per-bone VIF section table in {mesh_path}")
    table_off, entries = table
    out = []
    for i, off in enumerate(entries):
        sect_start = table_off + off
        sect_end = (table_off + entries[i + 1]
                    if i + 1 < len(entries) else len(d))
        verts = em.decode_objspace_bone_vertices(d, sect_start, sect_end)
        out.append(list(verts))
    return out


# ---------------------------------------------------------------------------
# Triangle topology + smooth normals
#
# The per-bone VIF stream is a generalized triangle strip. Each record carries
# a monotonic `vid` (vertex id) that steps by +2 between adjacent strip verts
# (the engine reserves the odd parity for an internal flag; in practice every
# stored vid is even). A delta other than +2 (+1, +4, +5, +7, +9, ..., +64,
# +90, etc.) signals a STRIP RESTART -- the next vertex begins a new strip.
#
# Within a strip of N verts we emit N-2 triangles with the standard
# alternating winding. Degenerate triangles (coincident vertex positions) are
# dropped -- the PS2 idiom for stitching strips into one draw.
#
# Per-vertex normals are computed by averaging the face normals of the
# triangles each vertex participates in. The packed 4-byte normal/lighting
# field at +0x06 in the VIF record was inspected (signed-byte / 127, IEEE
# float, unsigned bytes / 255) -- none gives consistently unit-length
# vectors, so the exact quantisation needs the VU1 microcode decode. Smooth
# face-averaged normals look fine for preview shading; we can revisit later.


def _build_strips(vids: list) -> list:
    """Split a vid sequence into [[local_idx, ...], ...] strips on any
    non-+2 delta. Strips shorter than 3 are dropped."""
    if not vids:
        return []
    strips = []
    cur = [0]
    for i in range(1, len(vids)):
        if vids[i] - vids[i - 1] == 2:
            cur.append(i)
        else:
            if len(cur) >= 3:
                strips.append(cur)
            cur = [i]
    if len(cur) >= 3:
        strips.append(cur)
    return strips


def _strip_triangles(strip: list, positions: list) -> list:
    """Emit (i0, i1, i2) tuples for one strip, with PS2 alternating winding,
    skipping degenerate (coincident-position) triangles."""
    tris = []
    n = len(strip)
    for t in range(n - 2):
        if t & 1:
            a, b, c = strip[t + 1], strip[t], strip[t + 2]
        else:
            a, b, c = strip[t], strip[t + 1], strip[t + 2]
        pa, pb, pc = positions[a], positions[b], positions[c]
        if pa == pb or pb == pc or pa == pc:
            continue
        tris.append((a, b, c))
    return tris


def _face_averaged_normals(positions: list, tris: list) -> list:
    """Per-vertex normals by face-area-weighted averaging."""
    import math as _m
    n = len(positions)
    nx = [0.0] * n
    ny = [0.0] * n
    nz = [0.0] * n
    for a, b, c in tris:
        ax, ay, az = positions[a]
        bx, by, bz = positions[b]
        cx, cy, cz = positions[c]
        ux, uy, uz = bx - ax, by - ay, bz - az
        vx, vy, vz = cx - ax, cy - ay, cz - az
        fx = uy * vz - uz * vy
        fy = uz * vx - ux * vz
        fz = ux * vy - uy * vx
        # cross-product magnitude == 2 * triangle area -> natural weighting
        for idx in (a, b, c):
            nx[idx] += fx
            ny[idx] += fy
            nz[idx] += fz
    out = []
    for i in range(n):
        m = _m.sqrt(nx[i] * nx[i] + ny[i] * ny[i] + nz[i] * nz[i])
        if m > 1e-9:
            out.append((nx[i] / m, ny[i] / m, nz[i] / m))
        else:
            out.append((0.0, 1.0, 0.0))
    return out


def triangulate_bone(verts_with_vid: list):
    """Given a bone's decoded [(x,y,z,vid), ...] records, return
    (positions, normals, indices_flat). All three are aligned so that
    `positions[i] / normals[i]` is the i-th vertex and `indices_flat`
    is a flat list of triangle vertex indices."""
    positions = [(x, y, z) for (x, y, z, _v) in verts_with_vid]
    vids = [v for (_x, _y, _z, v) in verts_with_vid]
    strips = _build_strips(vids)
    tris = []
    for s in strips:
        tris.extend(_strip_triangles(s, positions))
    normals = _face_averaged_normals(positions, tris)
    indices = [i for t in tris for i in t]
    return positions, normals, indices


# ---------------------------------------------------------------------------
# Quaternion helpers


def _quat_from_decoded(qx, qy, qz, qw):
    """The decoded sample is already a unit-ish quaternion (NLERP source).
    Normalize defensively before writing to glTF."""
    n = (qx * qx + qy * qy + qz * qz + qw * qw) ** 0.5
    if n == 0.0:
        return (0.0, 0.0, 0.0, 1.0)
    return (qx / n, qy / n, qz / n, qw / n)


# ---------------------------------------------------------------------------
# Animation track building
#
# For each bone we want continuous keyframes with TIME values in seconds and
# matching rotation/translation VALUES. The raw stream stores discrete records
# whose +0x0A field is t_next = the frame at which THIS record's sample is
# reached. We treat each record as a STEP keyframe at its t_next, with an
# implicit "previous-value-at-t_prev" sample so the in-between interpolation
# matches the engine's NLERP behaviour.
#
# glTF interpolation "LINEAR" gives us NLERP on quats (glTF runtimes normalise
# the lerped quat each frame). That matches anim_decoder.sample_bone(normalize=True).


FPS = 30.0


def _bone_rot_track(frames):
    """Return (times_sec, quats) for a bone's rotation stream.
    Empty -> ([], [])."""
    if not frames:
        return [], []
    times = []
    vals = []
    last_t = None
    for kf in frames:
        t_sec = kf.t_next / FPS
        if last_t is not None and t_sec <= last_t:
            # ensure strictly increasing times (glTF requires this)
            t_sec = last_t + 1.0 / FPS
        q = _quat_from_decoded(*kf.values)
        times.append(t_sec)
        vals.append(q)
        last_t = t_sec
    # Prepend a frame-0 sample equal to the first record (so the clip starts
    # cleanly at t=0 with the held value).
    if times[0] > 0.0:
        times.insert(0, 0.0)
        vals.insert(0, vals[0])
    return times, vals


def _bone_trn_track(frames):
    if not frames:
        return [], []
    times = []
    vals = []
    last_t = None
    for kf in frames:
        t_sec = kf.t_next / FPS
        if last_t is not None and t_sec <= last_t:
            t_sec = last_t + 1.0 / FPS
        times.append(t_sec)
        vals.append(kf.values)  # (tx, ty, tz)
        last_t = t_sec
    if times[0] > 0.0:
        times.insert(0, 0.0)
        vals.insert(0, vals[0])
    return times, vals


# ---------------------------------------------------------------------------
# glTF buffer builder


class GLBBuilder:
    # glTF component types
    COMP_FLOAT = 5126
    COMP_UNSIGNED_INT = 5125
    COMP_UNSIGNED_SHORT = 5123

    def __init__(self):
        self.bin_blob = bytearray()
        self.bufferViews: list = []
        self.accessors: list = []

    def _pad4(self):
        while len(self.bin_blob) % 4 != 0:
            self.bin_blob.append(0)

    def add_accessor(self, data: bytes, comp_type: int, count: int,
                     type_str: str, mins=None, maxs=None,
                     target: int | None = None) -> int:
        self._pad4()
        byte_offset = len(self.bin_blob)
        self.bin_blob.extend(data)
        bv = {
            "buffer": 0,
            "byteOffset": byte_offset,
            "byteLength": len(data),
        }
        if target is not None:
            bv["target"] = target
        self.bufferViews.append(bv)
        acc = {
            "bufferView": len(self.bufferViews) - 1,
            "componentType": comp_type,
            "count": count,
            "type": type_str,
        }
        if mins is not None:
            acc["min"] = list(mins)
        if maxs is not None:
            acc["max"] = list(maxs)
        self.accessors.append(acc)
        return len(self.accessors) - 1

    def add_vec3_float_normal(self, normals: List[Tuple[float, float, float]]) -> int:
        """A vec3 attribute accessor for normals (no min/max required, no
        component bounds tracking)."""
        if not normals:
            raise ValueError("empty normal accessor")
        data = struct.pack(f"<{len(normals) * 3}f",
                           *[c for v in normals for c in v])
        return self.add_accessor(
            data, self.COMP_FLOAT, len(normals), "VEC3",
            target=34962,
        )

    def add_indices_auto(self, idx: List[int], max_index: int) -> int:
        """u16 if max_index fits, else u32."""
        if max_index < 65536:
            data = struct.pack(f"<{len(idx)}H", *idx)
            return self.add_accessor(
                data, self.COMP_UNSIGNED_SHORT, len(idx), "SCALAR",
                target=34963,
            )
        data = struct.pack(f"<{len(idx)}I", *idx)
        return self.add_accessor(
            data, self.COMP_UNSIGNED_INT, len(idx), "SCALAR",
            target=34963,
        )

    def add_vec3_float(self, verts: List[Tuple[float, float, float]]) -> int:
        if not verts:
            # glTF disallows empty accessors; caller should skip.
            raise ValueError("empty vec3 accessor")
        data = struct.pack(f"<{len(verts) * 3}f",
                           *[c for v in verts for c in v])
        xs = [v[0] for v in verts]
        ys = [v[1] for v in verts]
        zs = [v[2] for v in verts]
        return self.add_accessor(
            data, self.COMP_FLOAT, len(verts), "VEC3",
            mins=(min(xs), min(ys), min(zs)),
            maxs=(max(xs), max(ys), max(zs)),
            target=34962,  # ARRAY_BUFFER
        )

    def add_vec3_float_anim(self, vecs: List[Tuple[float, float, float]]) -> int:
        data = struct.pack(f"<{len(vecs) * 3}f",
                           *[c for v in vecs for c in v])
        return self.add_accessor(
            data, self.COMP_FLOAT, len(vecs), "VEC3",
        )

    def add_vec4_float_anim(self, vecs: List[Tuple[float, float, float, float]]) -> int:
        data = struct.pack(f"<{len(vecs) * 4}f",
                           *[c for v in vecs for c in v])
        return self.add_accessor(
            data, self.COMP_FLOAT, len(vecs), "VEC4",
        )

    def add_scalar_float(self, vals: List[float]) -> int:
        data = struct.pack(f"<{len(vals)}f", *vals)
        return self.add_accessor(
            data, self.COMP_FLOAT, len(vals), "SCALAR",
            mins=(min(vals),), maxs=(max(vals),),
        )

    def add_indices_u32(self, idx: List[int]) -> int:
        data = struct.pack(f"<{len(idx)}I", *idx)
        return self.add_accessor(
            data, self.COMP_UNSIGNED_INT, len(idx), "SCALAR",
            target=34963,  # ELEMENT_ARRAY_BUFFER
        )

    def add_vec2_float(self, uvs: List[Tuple[float, float]]) -> int:
        if not uvs:
            raise ValueError("empty vec2 accessor")
        data = struct.pack(f"<{len(uvs) * 2}f",
                           *[c for v in uvs for c in v])
        return self.add_accessor(
            data, self.COMP_FLOAT, len(uvs), "VEC2",
            target=34962,
        )

    def add_raw_blob(self, blob: bytes) -> int:
        """Append a raw byte blob and return its bufferView index (no accessor).
        Used for embedded image data."""
        self._pad4()
        byte_offset = len(self.bin_blob)
        self.bin_blob.extend(blob)
        self.bufferViews.append({
            "buffer": 0,
            "byteOffset": byte_offset,
            "byteLength": len(blob),
        })
        return len(self.bufferViews) - 1


# ---------------------------------------------------------------------------
# Main build (unified single-pass; see build_glb_unified below).


def _unused_staged_build(mesh_path: Path, skel_path: Path, out_path: Path,
                         fps: float = 30.0) -> dict:
    global FPS
    FPS = fps

    skel_bytes = skel_path.read_bytes()
    entries = _entry_slices(skel_bytes)
    if not entries:
        raise RuntimeError(f"{skel_path}: no entries")

    # Entry 0 owns the canonical parent table + bind-pose-ish frame 0.
    e0_start, e0_end = entries[0]
    e0 = skel_bytes[e0_start:e0_end]
    bc0, raw_parents0, rot0, trn0 = _decode_entry_streams(e0)
    parents_signed = _signed_parents(raw_parents0)
    n_bones = bc0  # use the raw count so bone indices match the mesh sections.

    # Per-bone meshes from the character file.
    per_bone_verts = load_per_bone_meshes(mesh_path)
    n_mesh_sections = len(per_bone_verts)

    # ----- glTF scaffolding ------------------------------------------------
    gb = GLBBuilder()

    # Build meshes: one primitive per bone with vertices. Skip empty bones.
    mesh_for_bone: dict[int, int] = {}
    for i in range(min(n_bones, n_mesh_sections)):
        verts = per_bone_verts[i]
        if not verts:
            continue
        # Triangulate? The per-bone packets are a *point cloud* of
        # pre-strip Q4.12 vertices (no faces survived the VIF decode).
        # Emit them as POINTS primitive so they at least show up in viewers
        # that support points; for triangle topology we lack the index
        # buffer. (glTF mode 0 = POINTS.)
        pos_acc = gb.add_vec3_float(verts)
        prim = {
            "attributes": {"POSITION": pos_acc},
            "mode": 0,  # POINTS
        }
        mesh = {"primitives": [prim], "name": f"bone_{i:02d}_mesh"}
        mesh_for_bone[i] = -1  # placeholder; fill once meshes list is built
        # we'll re-emit below in a deterministic order

    # Re-emit meshes deterministically.
    meshes = []
    mesh_idx_for_bone: dict[int, int] = {}
    # we need to rebuild because we lost the position accessors above; do it cleanly:
    gb = GLBBuilder()
    for i in range(min(n_bones, n_mesh_sections)):
        verts = per_bone_verts[i]
        if not verts:
            continue
        pos_acc = gb.add_vec3_float(verts)
        prim = {"attributes": {"POSITION": pos_acc}, "mode": 0}
        meshes.append({"primitives": [prim], "name": f"bone_{i:02d}_mesh"})
        mesh_idx_for_bone[i] = len(meshes) - 1

    # ----- Nodes: scene root + 1 node per bone ----------------------------
    # Sample frame 0 TRS from clip 0 for bind-ish default pose.
    bone_nodes: list = []
    for i in range(n_bones):
        q = ad.sample_bone(rot0[i], 0.0) if rot0[i] else (0.0, 0.0, 0.0, 1.0)
        t = ad.sample_bone(trn0[i], 0.0, normalize=False) if trn0[i] else (0.0, 0.0, 0.0)
        if q is None:
            q = (0.0, 0.0, 0.0, 1.0)
        if t is None:
            t = (0.0, 0.0, 0.0)
        node = {
            "name": f"bone_{i:02d}",
            "rotation": list(_quat_from_decoded(*q)),
            "translation": [t[0], t[1], t[2]],
        }
        if i in mesh_idx_for_bone:
            node["mesh"] = mesh_idx_for_bone[i]
        bone_nodes.append(node)

    # Children: derive from parent table. Self-parent or -1 -> root.
    children_of: dict[int, list[int]] = {}
    roots: list[int] = []
    for i, p in enumerate(parents_signed):
        if p < 0 or p == i:
            roots.append(i)
        else:
            children_of.setdefault(p, []).append(i)
    for i, node in enumerate(bone_nodes):
        if i in children_of:
            node["children"] = children_of[i]

    # Root scene node holds all bone roots as children.
    scene_root = {"name": "Skeleton", "children": list(roots)}
    nodes = bone_nodes + [scene_root]
    scene_root_idx = len(nodes) - 1

    # ----- Animations ------------------------------------------------------
    animations: list = []
    total_samples = 0
    total_tracks = 0
    for clip_idx, (s, e) in enumerate(entries):
        entry_bytes = skel_bytes[s:e]
        try:
            bc, _, rot_streams, trn_streams = _decode_entry_streams(entry_bytes)
        except Exception:
            continue
        channels = []
        samplers = []

        for bone_i in range(min(bc, n_bones)):
            # Rotation track
            r_times, r_vals = _bone_rot_track(rot_streams[bone_i])
            if r_times:
                in_acc = gb.add_scalar_float(r_times)
                out_acc = gb.add_vec4_float_anim(r_vals)
                samplers.append({
                    "input": in_acc,
                    "output": out_acc,
                    "interpolation": "LINEAR",
                })
                channels.append({
                    "sampler": len(samplers) - 1,
                    "target": {"node": bone_i, "path": "rotation"},
                })
                total_samples += len(r_times)
                total_tracks += 1

            # Translation track
            t_times, t_vals = _bone_trn_track(trn_streams[bone_i])
            if t_times:
                in_acc = gb.add_scalar_float(t_times)
                out_acc = gb.add_vec3_float_anim(t_vals)
                samplers.append({
                    "input": in_acc,
                    "output": out_acc,
                    "interpolation": "LINEAR",
                })
                channels.append({
                    "sampler": len(samplers) - 1,
                    "target": {"node": bone_i, "path": "translation"},
                })
                total_samples += len(t_times)
                total_tracks += 1

        if channels:
            animations.append({
                "name": f"clip_{clip_idx:02d}",
                "channels": channels,
                "samplers": samplers,
            })

    # We need to re-emit per-bone mesh accessors into the SAME GLBBuilder so
    # the buffer contains both meshes and animation data. We started fresh
    # above for animations -- fix by re-adding meshes now (their pos accessors
    # were lost) and patching mesh.primitives to point to the new accessor IDs.
    # Easier path: rebuild from scratch with one pass. Refactor below.
    raise RuntimeError("internal: pipeline should not reach here -- "
                       "use the unified single-pass build path")


# ---------------------------------------------------------------------------
# Unified single-pass build (replaces the staged version above; the staged
# code was kept readable but loses accessor indices on the meshes-then-anims
# split. This pass builds everything against one shared GLBBuilder.)


def _bind_blocks_to_bones(blocks: list, bone_world_mats: list) -> list[int]:
    """For each MESH-descriptor block, pick the bone whose joint position is
    spatially closest to the block's vertex centroid after that bone's world
    matrix is applied (block verts are in some bone-local frame; the correct
    bone is the one whose local->world maps the centroid near its OWN joint).

    Algorithm per block:
      1. Compute centroid C_local of all real vertices across the block's
         strips (positions are in bone-local frame).
      2. For each candidate bone B:
         - Apply B's world matrix to C_local -> C_world.
         - Compute distance from C_world to B's joint translation
           (= B.world[3][:3]).
         The candidate bone is the one minimising that distance, i.e. the
         bone whose local frame "owns" this block: applying its transform
         produces a point near its own joint.

    Returns ``bone_index_for_block[block_i]``. Bones with empty mats (None)
    are skipped. Returns -1 for a block with no real vertices.
    """
    # Cache bone joint positions (column 3 of column-major 4x4).
    bone_joints: list[tuple[float, float, float] | None] = []
    for m in bone_world_mats:
        if m is None:
            bone_joints.append(None)
        else:
            bone_joints.append((m[3][0], m[3][1], m[3][2]))

    out: list[int] = []
    for block in blocks:
        # All real vertex positions in the block (bone-local frame).
        pts = []
        for s in block:
            for v in s.verts:
                pts.append(v.pos)
        if not pts:
            out.append(-1)
            continue
        cx = sum(p[0] for p in pts) / len(pts)
        cy = sum(p[1] for p in pts) / len(pts)
        cz = sum(p[2] for p in pts) / len(pts)

        best_bone = -1
        best_d2 = float("inf")
        for b, m in enumerate(bone_world_mats):
            if m is None or bone_joints[b] is None:
                continue
            # Apply column-major 4x4 to (cx, cy, cz, 1):
            # world = col0*cx + col1*cy + col2*cz + col3
            wx = m[0][0] * cx + m[1][0] * cy + m[2][0] * cz + m[3][0]
            wy = m[0][1] * cx + m[1][1] * cy + m[2][1] * cz + m[3][1]
            wz = m[0][2] * cx + m[1][2] * cy + m[2][2] * cz + m[3][2]
            jx, jy, jz = bone_joints[b]
            dx, dy, dz = wx - jx, wy - jy, wz - jz
            d2 = dx * dx + dy * dy + dz * dz
            if d2 < best_d2:
                best_d2 = d2
                best_bone = b
        out.append(best_bone)
    return out


def build_static_textured_mesh(mesh_path: Path, gb: "GLBBuilder",
                               search_root: Path | None) -> tuple[list, list, list, dict]:
    """Build the textured reference mesh from the mesh file's MESH-descriptor
    section (UV + position + normal). Returns (primitives, images, textures,
    info_dict). The primitives are split by texture sheet (one primitive per
    DBP), each referencing its own material.

    The positions in this stream are bone-local (pre-skinning), so the mesh
    appears as a jumbled blob at the scene root when bone-binding metadata
    is not yet decoded -- but the UVs and textures bind correctly per strip.

    Returns ([], [], [], {}) if no MESH descriptors found.
    """
    info: dict = {"sheets": [], "primitives": 0, "triangles": 0, "vertices": 0}
    d = mesh_path.read_bytes()
    strips = em.parse_model_file(d)
    if not strips:
        return [], [], [], info

    # Group strips by sheet_field -> DBP.
    by_dbp: dict[int, list] = {}
    for s in strips:
        m0, _m1 = s.key
        sheet_field = (m0 >> 15) & 0x3FFF
        dbp = _sheet_field_to_dbp(sheet_field)
        by_dbp.setdefault(dbp, []).append(s)

    # Resolve each DBP -> texture sheet (PSMT8 -> RGBA grayscale PNG).
    materials_for_dbp: dict[int, int] = {}
    textures_for_dbp: dict[int, int] = {}
    images_for_dbp: dict[int, int] = {}
    image_objs: list = []
    texture_objs: list = []
    material_objs: list = []
    for dbp in sorted(by_dbp.keys()):
        found = _find_transfer_for_dbp(mesh_path, dbp, search_root)
        sheet_info = {"dbp": dbp, "n_strips": len(by_dbp[dbp])}
        if found is None:
            # Synthetic placeholder texture -- still emit a material so the
            # primitive renders solid gray rather than failing strict parsers.
            sheet_info["source"] = None
            material_objs.append({
                "name": f"player_dbp{dbp}_missing",
                "pbrMetallicRoughness": {
                    "baseColorFactor": [0.5, 0.5, 0.5, 1.0],
                    "metallicFactor": 0.0, "roughnessFactor": 1.0,
                },
            })
            materials_for_dbp[dbp] = len(material_objs) - 1
            continue
        src_path, xfer = found
        png_bytes = _png_rgba_bytes(xfer.width, xfer.height, xfer.pixels)
        bv_idx = gb.add_raw_blob(png_bytes)
        image_objs.append({
            "name": f"sheet_DBP{dbp}",
            "mimeType": "image/png",
            "bufferView": bv_idx,
        })
        img_i = len(image_objs) - 1
        texture_objs.append({"source": img_i})
        tex_i = len(texture_objs) - 1
        material_objs.append({
            "name": f"player_dbp{dbp}",
            "pbrMetallicRoughness": {
                "baseColorTexture": {"index": tex_i, "texCoord": 0},
                "metallicFactor": 0.0, "roughnessFactor": 1.0,
            },
        })
        materials_for_dbp[dbp] = len(material_objs) - 1
        images_for_dbp[dbp] = img_i
        textures_for_dbp[dbp] = tex_i
        sheet_info["source"] = str(src_path.relative_to(src_path.parents[1]))
        sheet_info["size"] = [xfer.width, xfer.height]
        info["sheets"].append(sheet_info)

    # Build a primitive per DBP with merged strip triangles.
    primitives: list = []
    total_tris = 0
    total_verts = 0
    for dbp, dbp_strips in by_dbp.items():
        positions: list = []
        normals: list = []
        uvs: list = []
        indices: list = []
        for s in dbp_strips:
            base = len(positions)
            for v in s.verts:
                positions.append(v.pos)
                # attr is (nx,ny,nz) for dynamic meshes; we trust it as a
                # normal here (character meshes are dynamic-lit).
                normals.append(v.attr)
                uvs.append((v.uv[0], v.uv[1]))
            for i0, i1, i2 in em.strip_triangles(s):
                indices.append(base + i0)
                indices.append(base + i1)
                indices.append(base + i2)
        if not indices:
            continue
        pos_acc = gb.add_vec3_float(positions)
        nrm_acc = gb.add_vec3_float_normal(normals)
        uv_acc = gb.add_vec2_float(uvs)
        idx_acc = gb.add_indices_auto(indices, len(positions))
        prim = {
            "attributes": {
                "POSITION": pos_acc,
                "NORMAL": nrm_acc,
                "TEXCOORD_0": uv_acc,
            },
            "indices": idx_acc,
            "mode": 4,  # TRIANGLES
            "material": materials_for_dbp[dbp],
        }
        primitives.append(prim)
        total_tris += len(indices) // 3
        total_verts += len(positions)

    info["primitives"] = len(primitives)
    info["triangles"] = total_tris
    info["vertices"] = total_verts
    return primitives, image_objs, texture_objs, {
        **info,
        "_materials": material_objs,
    }


def build_proxy_bound_blocks(mesh_path: Path, gb: "GLBBuilder",
                             bone_world_mats: list,
                             search_root: Path | None
                             ) -> tuple[list, list, list, list, dict]:
    """Build TEXTURED per-block meshes BOUND by spatial proximity to bones.

    Replaces the single un-bound static mesh: each MESH-descriptor block
    becomes its own mesh whose primitives are split by texture sheet (DBP),
    and the mesh is attached to a glTF node parented to the nearest-fit bone
    node. The block's vertex positions stay in bone-local frame; the bone
    node's world transform (from id 0x71 frame 0) places it in world space.

    Returns (per_block_meshes, per_block_bone_index, image_objs, texture_objs,
    info_dict). `per_block_meshes[i]` is a glTF mesh dict for block i (or
    None for empty blocks), and `per_block_bone_index[i]` is the bone the
    block is parented to (or -1 if no candidate fit / empty block).
    """
    info: dict = {"blocks": 0, "bound": 0, "primitives": 0,
                  "triangles": 0, "vertices": 0, "sheets": []}
    d = mesh_path.read_bytes()
    blocks = em.parse_model_blocks(d)
    if not blocks:
        return [], [], [], [], info

    # Resolve textures up front (same as build_static_textured_mesh): scan
    # every block's strips to enumerate sheet DBPs.
    by_dbp_global: dict[int, int] = {}  # dbp -> presence
    for block in blocks:
        for s in block:
            m0, _m1 = s.key
            sheet_field = (m0 >> 15) & 0x3FFF
            dbp = _sheet_field_to_dbp(sheet_field)
            by_dbp_global[dbp] = by_dbp_global.get(dbp, 0) + 1

    materials_for_dbp: dict[int, int] = {}
    image_objs: list = []
    texture_objs: list = []
    material_objs: list = []
    for dbp in sorted(by_dbp_global.keys()):
        found = _find_transfer_for_dbp(mesh_path, dbp, search_root)
        sheet_info = {"dbp": dbp, "n_strips_total": by_dbp_global[dbp]}
        if found is None:
            sheet_info["source"] = None
            material_objs.append({
                "name": f"player_dbp{dbp}_missing",
                "pbrMetallicRoughness": {
                    "baseColorFactor": [0.5, 0.5, 0.5, 1.0],
                    "metallicFactor": 0.0, "roughnessFactor": 1.0,
                },
            })
            materials_for_dbp[dbp] = len(material_objs) - 1
            info["sheets"].append(sheet_info)
            continue
        src_path, xfer = found
        png_bytes = _png_rgba_bytes(xfer.width, xfer.height, xfer.pixels)
        bv_idx = gb.add_raw_blob(png_bytes)
        image_objs.append({
            "name": f"sheet_DBP{dbp}",
            "mimeType": "image/png",
            "bufferView": bv_idx,
        })
        img_i = len(image_objs) - 1
        texture_objs.append({"source": img_i})
        tex_i = len(texture_objs) - 1
        material_objs.append({
            "name": f"player_dbp{dbp}",
            "pbrMetallicRoughness": {
                "baseColorTexture": {"index": tex_i, "texCoord": 0},
                "metallicFactor": 0.0, "roughnessFactor": 1.0,
            },
        })
        materials_for_dbp[dbp] = len(material_objs) - 1
        sheet_info["source"] = str(src_path.relative_to(src_path.parents[1]))
        sheet_info["size"] = [xfer.width, xfer.height]
        info["sheets"].append(sheet_info)

    # Spatial-proximity binding: block -> bone.
    bone_for_block = _bind_blocks_to_bones(blocks, bone_world_mats)

    per_block_meshes: list = []
    total_tris = 0
    total_verts = 0
    total_prims = 0
    bound_count = 0
    for bi, block in enumerate(blocks):
        if not block:
            per_block_meshes.append(None)
            continue
        # Group strips of this block by DBP.
        by_dbp: dict[int, list] = {}
        for s in block:
            m0, _m1 = s.key
            sheet_field = (m0 >> 15) & 0x3FFF
            dbp = _sheet_field_to_dbp(sheet_field)
            by_dbp.setdefault(dbp, []).append(s)
        primitives: list = []
        for dbp, dbp_strips in by_dbp.items():
            positions: list = []
            normals: list = []
            uvs: list = []
            indices: list = []
            for s in dbp_strips:
                base = len(positions)
                for v in s.verts:
                    positions.append(v.pos)
                    normals.append(v.attr)
                    uvs.append((v.uv[0], v.uv[1]))
                for i0, i1, i2 in em.strip_triangles(s):
                    indices.append(base + i0)
                    indices.append(base + i1)
                    indices.append(base + i2)
            if not indices:
                continue
            pos_acc = gb.add_vec3_float(positions)
            nrm_acc = gb.add_vec3_float_normal(normals)
            uv_acc = gb.add_vec2_float(uvs)
            idx_acc = gb.add_indices_auto(indices, len(positions))
            prim = {
                "attributes": {
                    "POSITION": pos_acc,
                    "NORMAL": nrm_acc,
                    "TEXCOORD_0": uv_acc,
                },
                "indices": idx_acc,
                "mode": 4,
                "material": materials_for_dbp[dbp],
            }
            primitives.append(prim)
            total_tris += len(indices) // 3
            total_verts += len(positions)
            total_prims += 1
        if not primitives:
            per_block_meshes.append(None)
            continue
        per_block_meshes.append({
            "primitives": primitives,
            "name": f"block_{bi:02d}_bound_bone_{bone_for_block[bi]:02d}",
        })
        if bone_for_block[bi] >= 0:
            bound_count += 1

    info["blocks"] = len(blocks)
    info["bound"] = bound_count
    info["primitives"] = total_prims
    info["triangles"] = total_tris
    info["vertices"] = total_verts
    info["_materials"] = material_objs
    return per_block_meshes, bone_for_block, image_objs, texture_objs, info


def build_glb_unified(mesh_path: Path, skel_path: Path, out_path: Path,
                      fps: float = 30.0,
                      search_root: Path | None = None) -> dict:
    global FPS
    FPS = fps

    skel_bytes = skel_path.read_bytes()
    entries = _entry_slices(skel_bytes)
    if not entries:
        raise RuntimeError(f"{skel_path}: no entries")

    e0_start, e0_end = entries[0]
    e0 = skel_bytes[e0_start:e0_end]
    bc0, raw_parents0, rot0, trn0 = _decode_entry_streams(e0)
    parents_signed = _signed_parents(raw_parents0)
    n_bones = bc0

    per_bone_verts_vid = load_per_bone_meshes_with_vids(mesh_path)
    n_mesh_sections = len(per_bone_verts_vid)

    gb = GLBBuilder()

    # 1. Meshes -- one TRIANGLES primitive per non-empty bone with POSITION,
    # NORMAL and an index buffer. Triangle topology comes from generalized
    # tristrip decoding of the per-bone VIF vid stream (see triangulate_bone).
    # Normals are face-area-weighted vertex averages -- the packed 4-byte
    # field in the VIF record is not yet decoded.
    meshes: list = []
    mesh_idx_for_bone: dict[int, int] = {}
    total_tris = 0
    bones_as_points = 0
    for i in range(min(n_bones, n_mesh_sections)):
        vwv = per_bone_verts_vid[i]
        if not vwv:
            continue
        positions, normals, indices = triangulate_bone(vwv)
        if indices:
            pos_acc = gb.add_vec3_float(positions)
            nrm_acc = gb.add_vec3_float_normal(normals)
            idx_acc = gb.add_indices_auto(indices, len(positions))
            prim = {
                "attributes": {"POSITION": pos_acc, "NORMAL": nrm_acc},
                "indices": idx_acc,
                "mode": 4,  # TRIANGLES
            }
            total_tris += len(indices) // 3
        else:
            # Bone with <3 verts or no decodable strip -- fall back to POINTS.
            pos_acc = gb.add_vec3_float(positions)
            prim = {"attributes": {"POSITION": pos_acc}, "mode": 0}
            bones_as_points += 1
        meshes.append({
            "primitives": [prim],
            "name": f"bone_{i:02d}_mesh",
        })
        mesh_idx_for_bone[i] = len(meshes) - 1

    # 2. Bone nodes with frame-0 default TRS.
    bone_nodes: list = []
    for i in range(n_bones):
        q = ad.sample_bone(rot0[i], 0.0) if rot0[i] else (0.0, 0.0, 0.0, 1.0)
        t = ad.sample_bone(trn0[i], 0.0, normalize=False) if trn0[i] else (0.0, 0.0, 0.0)
        if q is None:
            q = (0.0, 0.0, 0.0, 1.0)
        if t is None:
            t = (0.0, 0.0, 0.0)
        nq = _quat_from_decoded(*q)
        node = {
            "name": f"bone_{i:02d}",
            "rotation": [nq[0], nq[1], nq[2], nq[3]],
            "translation": [t[0], t[1], t[2]],
        }
        if i in mesh_idx_for_bone:
            node["mesh"] = mesh_idx_for_bone[i]
        bone_nodes.append(node)

    children_of: dict[int, list[int]] = {}
    roots: list[int] = []
    for i, p in enumerate(parents_signed):
        if p < 0 or p == i:
            roots.append(i)
        else:
            children_of.setdefault(p, []).append(i)
    for i, node in enumerate(bone_nodes):
        if i in children_of:
            node["children"] = children_of[i]

    scene_root = {"name": "Skeleton", "children": list(roots)}
    nodes = bone_nodes + [scene_root]
    scene_root_idx = len(nodes) - 1

    # 3. Animations
    animations: list = []
    total_samples = 0
    total_tracks = 0
    track_per_clip: list[int] = []
    for clip_idx, (s, e) in enumerate(entries):
        entry_bytes = skel_bytes[s:e]
        try:
            bc, _, rot_streams, trn_streams = _decode_entry_streams(entry_bytes)
        except Exception:
            track_per_clip.append(0)
            continue
        channels = []
        samplers = []
        for bone_i in range(min(bc, n_bones)):
            r_times, r_vals = _bone_rot_track(rot_streams[bone_i])
            if r_times:
                in_acc = gb.add_scalar_float(r_times)
                out_acc = gb.add_vec4_float_anim(r_vals)
                samplers.append({
                    "input": in_acc, "output": out_acc,
                    "interpolation": "LINEAR",
                })
                channels.append({
                    "sampler": len(samplers) - 1,
                    "target": {"node": bone_i, "path": "rotation"},
                })
                total_samples += len(r_times)
                total_tracks += 1
            t_times, t_vals = _bone_trn_track(trn_streams[bone_i])
            if t_times:
                in_acc = gb.add_scalar_float(t_times)
                out_acc = gb.add_vec3_float_anim(t_vals)
                samplers.append({
                    "input": in_acc, "output": out_acc,
                    "interpolation": "LINEAR",
                })
                channels.append({
                    "sampler": len(samplers) - 1,
                    "target": {"node": bone_i, "path": "translation"},
                })
                total_samples += len(t_times)
                total_tracks += 1
        if channels:
            animations.append({
                "name": f"clip_{clip_idx:02d}",
                "channels": channels,
                "samplers": samplers,
            })
            track_per_clip.append(len(channels))
        else:
            track_per_clip.append(0)

    # 3b. Textured MESH-descriptor blocks with SPATIAL-PROXIMITY bone binding.
    # Each block (~32 verts in bone-local frame) is attached to the bone node
    # whose world transform best places the block's centroid near that bone's
    # joint. This is an approximation -- the true per-block bone-INDEX TABLE
    # is not yet located on disc -- but it produces a recognisable textured
    # humanoid placed in world space instead of a blob collapsed at origin.
    # See docs/FINDINGS.md "Per-block bone binding (proxy)".
    bone_world_mats: list = []
    try:
        world_full, _parents_unused = em.bind_pose_at_t(skel_path, 0, 0.0)
        # Pad / truncate to n_bones length so block-binder indices align with
        # bone node indices.
        for i in range(n_bones):
            if i < len(world_full):
                bone_world_mats.append(world_full[i])
            else:
                bone_world_mats.append(None)
    except Exception:
        bone_world_mats = [None] * n_bones

    block_meshes, bone_for_block, image_objs, texture_objs, static_info = \
        build_proxy_bound_blocks(mesh_path, gb, bone_world_mats, search_root)
    material_objs = static_info.pop("_materials", []) if static_info else []

    # Each non-empty block becomes a mesh + a node parented under its bound
    # bone, so the bone's animated TRS carries the block with it.
    block_node_indices: list[int] = []
    for bi, m in enumerate(block_meshes):
        if m is None:
            continue
        meshes.append(m)
        mesh_idx = len(meshes) - 1
        bone_i = bone_for_block[bi]
        node = {
            "name": f"block_{bi:02d}_node",
            "mesh": mesh_idx,
        }
        nodes.append(node)
        node_idx = len(nodes) - 1
        block_node_indices.append(node_idx)
        if 0 <= bone_i < n_bones:
            bn = bone_nodes[bone_i]
            kids = bn.setdefault("children", [])
            kids.append(node_idx)
        else:
            # Fallback: attach to scene root if no bone fit was found.
            scene_root.setdefault("children", []).append(node_idx)
    static_node_idx = None  # legacy compat; static mesh no longer at scene root

    # 4. Pad buffer to 4
    while len(gb.bin_blob) % 4 != 0:
        gb.bin_blob.append(0)

    scene_nodes = [scene_root_idx]
    if static_node_idx is not None:
        scene_nodes.append(static_node_idx)

    gltf = {
        "asset": {
            "version": "2.0",
            "generator": "Extermination decomp export_gltf.py",
        },
        "scene": 0,
        "scenes": [{"name": "Scene", "nodes": scene_nodes}],
        "nodes": nodes,
        "meshes": meshes,
        "accessors": gb.accessors,
        "bufferViews": gb.bufferViews,
        "buffers": [{"byteLength": len(gb.bin_blob)}],
    }
    if image_objs:
        gltf["images"] = image_objs
    if texture_objs:
        gltf["textures"] = texture_objs
    if material_objs:
        gltf["materials"] = material_objs
    if animations:
        gltf["animations"] = animations

    # 5. Serialize as .glb
    json_bytes = json.dumps(gltf, separators=(",", ":")).encode("utf-8")
    # pad JSON to 4-byte multiple with spaces.
    while len(json_bytes) % 4 != 0:
        json_bytes += b" "
    bin_bytes = bytes(gb.bin_blob)
    # binary chunk also padded to 4 (already is by gb).
    total_len = 12 + 8 + len(json_bytes) + 8 + len(bin_bytes)
    header = struct.pack("<III", 0x46546C67, 2, total_len)  # 'glTF', ver, len
    json_chunk = struct.pack("<II", len(json_bytes), 0x4E4F534A) + json_bytes  # 'JSON'
    bin_chunk = struct.pack("<II", len(bin_bytes), 0x004E4942) + bin_bytes  # 'BIN\0'

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_bytes(header + json_chunk + bin_chunk)

    return {
        "out_path": str(out_path),
        "size_bytes": len(header) + len(json_chunk) + len(bin_chunk),
        "n_bones": n_bones,
        "n_meshes": len(meshes),
        "n_triangles": total_tris,
        "bones_as_points": bones_as_points,
        "n_animations": len(animations),
        "total_animation_tracks": total_tracks,
        "total_animation_samples": total_samples,
        "tracks_per_clip_first_5": track_per_clip[:5],
        "buffer_bytes": len(bin_bytes),
        "json_bytes": len(json_bytes),
        "parents": parents_signed,
        "static_textured": static_info,
        "n_images": len(image_objs),
        "n_textures": len(texture_objs),
        "n_materials": len(material_objs),
    }


# ---------------------------------------------------------------------------
# LEVEL scene export (id 0x44 files)
#
# A level file holds world-space MESH / SUBMESH geometry plus MATRIX
# instance-placement blocks. `extract_models.parse_scene()` already returns
# every triangle strip PLACED in world space: the regular MESH/SUBMESH strips
# unchanged, and every MATRIX section's geometry emitted once per (deduped)
# transform with the matrix baked into the vertices.
#
# MATRIX transforms are ABSOLUTE world placements, not parent-composed
# (resolved empirically 2026-06-01): on chunk04.n0/f06_id44.bin the MATRIX
# sections' object-space geometry already spans the full level Z extent
# (Z[-1515,6]) at their identity (record-0) transform, and the per-instance
# translations are small additive offsets (a few units) that scatter copies of
# a base prop WITHIN the level footprint -- never an outer frame that would
# collapse everything to the origin. Applying each matrix directly keeps the
# instanced geometry inside the static world bbox and overlapping it, which is
# exactly what an absolute placement does. So we bake the transforms in (same
# as the OBJ `--scene` path) and emit the union as one placed scene.
#
# Each strip's marker `m0` carries the texture sheet:
#   sheet_field = (m0 >> 15) & 0x3FFF  ->  GS DBP (see _sheet_field_to_dbp).
# We group strips by DBP, resolve each DBP to a texture sheet (preferring the
# level's own chunk dir, then the whole extract tree -- ~all level textures are
# cross-file, uploaded by sibling id 0x43/0x44/0x06 files), and embed the
# PSMT8 sheet as an RGBA grayscale PNG. Strips whose DBP cannot be resolved get
# a solid-gray placeholder material so the geometry still renders.
#
# The `+0x20` attribute is a unit normal on dynamic meshes and a baked vertex
# colour on static/world meshes (distinguished per-vertex by `Vertex.is_normal`
# -- see extract_models). For a clean preview we emit a geometric (face-area-
# weighted) NORMAL per strip-group and pass the colour-or-normal attr through
# as COLOR_0 only when it is clearly a colour (so lit viewers shade by geometry
# and unlit viewers can still show baked vertex colour).
# ---------------------------------------------------------------------------


def _strip_dbp(strip) -> int:
    m0, _m1 = strip.key
    sheet_field = (m0 >> 15) & 0x3FFF
    return _sheet_field_to_dbp(sheet_field)


def _resolve_level_sheets(level_path: Path, dbps: list[int],
                          search_root: Path | None) -> dict[int, tuple[Path, "est.Transfer"]]:
    """Resolve each wanted DBP to (source_file, decoded transfer).

    Consults the global VRAM residency map first: every wanted DBP is resolved
    disc-wide (cross-file uploads + near-miss snapping to the same physical
    sheet), preferring the level's own chunk dir. Only DBPs the map can't
    resolve fall through to the legacy same-dir-then-tree scan. Missing DBPs are
    simply absent from the returned dict (the caller emits a placeholder).
    """
    want = set(dbps)
    found: dict[int, tuple[Path, "est.Transfer"]] = {}

    rm = _residency(search_root)
    if rm is not None:
        for dbp in want:
            up = rm.resolve_dbp(dbp, prefer_dir=level_path.parent)
            if up is None:
                continue
            src = rm.root / up.src_rel
            try:
                dd = src.read_bytes()
            except OSError:
                continue
            t = est.Transfer(src.name, up.dbp, up.tw, up.th, up.trxreg_off,
                             src_path=src)
            if est.decode_transfer(dd, t):
                found[dbp] = (src, t)

    if want <= set(found):
        return found

    # Legacy fallback for any DBP the residency map didn't resolve.
    def scan_dir(files: list[Path]):
        for p in files:
            if want <= set(found):
                return
            try:
                dd = p.read_bytes()
            except Exception:
                continue
            for t in est.scan_transfers(dd, p.name):
                if t.dbp in want and t.dbp not in found:
                    t.src_path = p
                    if est.decode_transfer(dd, t):
                        found[t.dbp] = (p, t)

    scan_dir(sorted(level_path.parent.glob("*.bin")))
    if not (want <= set(found)) and search_root and search_root.is_dir():
        seen = set(level_path.parent.glob("*.bin"))
        rest = [p for p in sorted(search_root.rglob("*.bin")) if p not in seen]
        scan_dir(rest)
    return found


def build_level_glb(level_path: Path, out_path: Path,
                    search_root: Path | None = None,
                    no_textures: bool = False) -> dict:
    """Export an id 0x44 level file as a single placed, textured .glb.

    Geometry: extract_models.parse_scene() (world-space MESH/SUBMESH + baked
    MATRIX instances). Texturing: per-DBP sheet resolution + embedded PNG.
    One glTF mesh per DBP group (one TRIANGLES primitive), all under a single
    scene-root node. Returns a stats dict.
    """
    d = level_path.read_bytes()
    strips = em.parse_scene(d)
    if not strips:
        raise RuntimeError(f"{level_path}: no geometry strips")

    # Group strips by texture sheet DBP.
    by_dbp: dict[int, list] = {}
    for s in strips:
        by_dbp.setdefault(_strip_dbp(s), []).append(s)

    gb = GLBBuilder()

    # Resolve + embed textures (unless suppressed).
    image_objs: list = []
    texture_objs: list = []
    material_objs: list = []
    material_for_dbp: dict[int, int] = {}
    sheet_infos: list = []
    resolved: dict[int, tuple[Path, "est.Transfer"]] = {}
    if not no_textures:
        resolved = _resolve_level_sheets(level_path, sorted(by_dbp.keys()),
                                         search_root)
    for dbp in sorted(by_dbp.keys()):
        n_strips = len(by_dbp[dbp])
        if dbp in resolved:
            src_path, xfer = resolved[dbp]
            png = _png_rgba_bytes(xfer.width, xfer.height, xfer.pixels)
            bv = gb.add_raw_blob(png)
            image_objs.append({
                "name": f"sheet_DBP{dbp}",
                "mimeType": "image/png",
                "bufferView": bv,
            })
            img_i = len(image_objs) - 1
            texture_objs.append({"source": img_i})
            tex_i = len(texture_objs) - 1
            material_objs.append({
                "name": f"mat_dbp{dbp}",
                "pbrMetallicRoughness": {
                    "baseColorTexture": {"index": tex_i, "texCoord": 0},
                    "metallicFactor": 0.0, "roughnessFactor": 1.0,
                },
                "doubleSided": True,
            })
            try:
                src_rel = str(src_path.relative_to(search_root)) \
                    if search_root else src_path.name
            except ValueError:
                src_rel = src_path.name
            sheet_infos.append({"dbp": dbp, "n_strips": n_strips,
                                "source": src_rel,
                                "size": [xfer.width, xfer.height]})
        else:
            material_objs.append({
                "name": f"mat_dbp{dbp}_missing",
                "pbrMetallicRoughness": {
                    "baseColorFactor": [0.6, 0.6, 0.6, 1.0],
                    "metallicFactor": 0.0, "roughnessFactor": 1.0,
                },
                "doubleSided": True,
            })
            sheet_infos.append({"dbp": dbp, "n_strips": n_strips,
                                "source": None})
        material_for_dbp[dbp] = len(material_objs) - 1

    # One mesh per DBP group; merge that group's strip triangles into a single
    # TRIANGLES primitive (POSITION + NORMAL + TEXCOORD_0).
    meshes: list = []
    mesh_node_indices: list = []
    total_tris = 0
    total_verts = 0
    xs_all: list[float] = []
    ys_all: list[float] = []
    zs_all: list[float] = []
    for dbp in sorted(by_dbp.keys()):
        positions: list = []
        normals: list = []
        uvs: list = []
        indices: list = []
        for s in by_dbp[dbp]:
            base = len(positions)
            local_pos = [v.pos for v in s.verts]
            for v in s.verts:
                positions.append(v.pos)
                uvs.append((v.uv[0], v.uv[1]))
            tris = list(em.strip_triangles(s))
            # Per-strip geometric normals (handles colour-attr static meshes
            # uniformly; dynamic-mesh stored normals are close to these too).
            snorm = _strip_geom_normals(local_pos, tris)
            normals.extend(snorm)
            for i0, i1, i2 in tris:
                indices.append(base + i0)
                indices.append(base + i1)
                indices.append(base + i2)
        if not indices:
            continue
        pos_acc = gb.add_vec3_float(positions)
        nrm_acc = gb.add_vec3_float_normal(normals)
        uv_acc = gb.add_vec2_float(uvs)
        idx_acc = gb.add_indices_auto(indices, len(positions))
        prim = {
            "attributes": {
                "POSITION": pos_acc,
                "NORMAL": nrm_acc,
                "TEXCOORD_0": uv_acc,
            },
            "indices": idx_acc,
            "mode": 4,  # TRIANGLES
        }
        if material_for_dbp:
            prim["material"] = material_for_dbp[dbp]
        meshes.append({"primitives": [prim], "name": f"sheet_dbp{dbp}"})
        total_tris += len(indices) // 3
        total_verts += len(positions)
        xs_all.extend(p[0] for p in positions)
        ys_all.extend(p[1] for p in positions)
        zs_all.extend(p[2] for p in positions)

    if not meshes:
        raise RuntimeError(f"{level_path}: no renderable triangles")

    # Nodes: one node per mesh, all under a single scene-root. Geometry is
    # already world-placed (MATRIX transforms baked), so node transforms are
    # identity -- the node layer simply names each sheet group for the DCC tool.
    nodes: list = []
    for mi, m in enumerate(meshes):
        nodes.append({"name": m["name"], "mesh": mi})
        mesh_node_indices.append(len(nodes) - 1)
    scene_root = {"name": level_path.stem, "children": list(mesh_node_indices)}
    nodes.append(scene_root)
    scene_root_idx = len(nodes) - 1

    while len(gb.bin_blob) % 4 != 0:
        gb.bin_blob.append(0)

    gltf = {
        "asset": {"version": "2.0",
                  "generator": "Extermination decomp export_gltf.py (level)"},
        "scene": 0,
        "scenes": [{"name": "Scene", "nodes": [scene_root_idx]}],
        "nodes": nodes,
        "meshes": meshes,
        "accessors": gb.accessors,
        "bufferViews": gb.bufferViews,
        "buffers": [{"byteLength": len(gb.bin_blob)}],
    }
    if image_objs:
        gltf["images"] = image_objs
    if texture_objs:
        gltf["textures"] = texture_objs
    if material_objs:
        gltf["materials"] = material_objs

    json_bytes = json.dumps(gltf, separators=(",", ":")).encode("utf-8")
    while len(json_bytes) % 4 != 0:
        json_bytes += b" "
    bin_bytes = bytes(gb.bin_blob)
    total_len = 12 + 8 + len(json_bytes) + 8 + len(bin_bytes)
    header = struct.pack("<III", 0x46546C67, 2, total_len)
    json_chunk = struct.pack("<II", len(json_bytes), 0x4E4F534A) + json_bytes
    bin_chunk = struct.pack("<II", len(bin_bytes), 0x004E4942) + bin_bytes
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_bytes(header + json_chunk + bin_chunk)

    bbox = None
    if xs_all:
        bbox = (round(min(xs_all), 1), round(max(xs_all), 1),
                round(min(ys_all), 1), round(max(ys_all), 1),
                round(min(zs_all), 1), round(max(zs_all), 1))
    n_resolved = sum(1 for s in sheet_infos if s.get("source"))
    return {
        "out_path": str(out_path),
        "size_bytes": len(header) + len(json_chunk) + len(bin_chunk),
        "n_strips": len(strips),
        "n_meshes": len(meshes),
        "n_triangles": total_tris,
        "n_vertices": total_verts,
        "n_dbp_groups": len(by_dbp),
        "n_textures": len(texture_objs),
        "n_materials": len(material_objs),
        "sheets_resolved": n_resolved,
        "sheets_total": len(by_dbp),
        "bbox": bbox,
        "sheets": sheet_infos,
    }


def _strip_geom_normals(positions: list, tris: list) -> list:
    """Per-vertex face-area-weighted normals for one strip (local indices).

    `positions` is the strip's vertex positions in emission order; `tris` are
    (i0,i1,i2) local-index triangles from em.strip_triangles. Returns one
    normal per position. Falls back to +Y for isolated verts.
    """
    import math as _m
    n = len(positions)
    ax = [0.0] * n
    ay = [0.0] * n
    az = [0.0] * n
    for a, b, c in tris:
        pa, pb, pc = positions[a], positions[b], positions[c]
        ux, uy, uz = pb[0] - pa[0], pb[1] - pa[1], pb[2] - pa[2]
        vx, vy, vz = pc[0] - pa[0], pc[1] - pa[1], pc[2] - pa[2]
        fx = uy * vz - uz * vy
        fy = uz * vx - ux * vz
        fz = ux * vy - uy * vx
        for idx in (a, b, c):
            ax[idx] += fx
            ay[idx] += fy
            az[idx] += fz
    out = []
    for i in range(n):
        m = _m.sqrt(ax[i] * ax[i] + ay[i] * ay[i] + az[i] * az[i])
        if m > 1e-9:
            out.append((ax[i] / m, ay[i] / m, az[i] / m))
        else:
            out.append((0.0, 1.0, 0.0))
    return out


def _level_main(argv: list[str]) -> int:
    """Entry for `export_gltf.py level ...`."""
    p = argparse.ArgumentParser(
        prog="export_gltf.py level",
        description="Export an Extermination id 0x44 level file as a placed, "
                    "textured glTF 2.0 binary (.glb).")
    g = p.add_mutually_exclusive_group(required=True)
    g.add_argument("--level", help="id 0x44 level file "
                   "(e.g. extract/chunk04.n0/f06_id44.bin)")
    g.add_argument("--all-levels", action="store_true",
                   help="export every id 0x44 file under --search-root")
    p.add_argument("--out", help="output .glb path (single-level mode). "
                   "Default: models/<stem>_scene.glb")
    p.add_argument("--out-dir", default="models",
                   help="output dir for --all-levels (default: models)")
    p.add_argument("--search-root", default="extract",
                   help="root for cross-file texture resolution / level scan "
                        "(default: extract)")
    p.add_argument("--no-textures", action="store_true",
                   help="skip texture resolution -- geometry-only scene")
    args = p.parse_args(argv)

    search_root = Path(args.search_root)

    if args.all_levels:
        levels = sorted(search_root.rglob("*_id44.bin"))
        if not levels:
            print(f"error: no *_id44.bin under {search_root}", file=sys.stderr)
            return 2
        out_dir = Path(args.out_dir)
        ok = 0
        for lv in levels:
            out = out_dir / f"{lv.stem}_{lv.parent.name}_scene.glb"
            try:
                info = build_level_glb(lv, out, search_root=search_root,
                                       no_textures=args.no_textures)
            except Exception as exc:  # noqa: BLE001
                print(f"  SKIP {lv}: {exc}", file=sys.stderr)
                continue
            ok += 1
            print(f"  {lv.parent.name}/{lv.name}: {info['n_triangles']} tris, "
                  f"{info['sheets_resolved']}/{info['sheets_total']} sheets, "
                  f"{info['size_bytes']//1024} KiB -> {out}")
        print(f"wrote {ok}/{len(levels)} level scenes to {out_dir}")
        return 0

    level_path = Path(args.level)
    if not level_path.is_file():
        print(f"error: level not found: {level_path}", file=sys.stderr)
        return 2
    out = Path(args.out) if args.out else \
        Path("models") / f"{level_path.stem}_scene.glb"
    info = build_level_glb(level_path, out, search_root=search_root,
                           no_textures=args.no_textures)
    print(f"wrote {info['out_path']} ({info['size_bytes']} bytes)")
    print(f"  strips     : {info['n_strips']}")
    print(f"  meshes     : {info['n_meshes']}  (one per texture sheet group)")
    print(f"  triangles  : {info['n_triangles']}")
    print(f"  vertices   : {info['n_vertices']}")
    if info["bbox"]:
        bx = info["bbox"]
        print(f"  bbox       : X[{bx[0]},{bx[1]}] Y[{bx[2]},{bx[3]}] "
              f"Z[{bx[4]},{bx[5]}]  extent "
              f"{bx[1]-bx[0]:.0f} x {bx[3]-bx[2]:.0f} x {bx[5]-bx[4]:.0f}")
    print(f"  textures   : {info['sheets_resolved']}/{info['sheets_total']} "
          f"sheets resolved, {info['n_materials']} materials")
    for sh in info["sheets"]:
        src = sh.get("source") or "MISSING (gray placeholder)"
        sz = sh.get("size")
        szs = f"{sz[0]}x{sz[1]}" if sz else "-"
        print(f"    DBP={sh['dbp']:6d} ({sh['n_strips']:4d} strips)  "
              f"{szs:>9}  source={src}")
    return 0


# ---------------------------------------------------------------------------
# CLI


def _auto_pair_skel(mesh_path: Path, mesh_section_count: int,
                    search_root: Path) -> Path | None:
    # Prefer same chunk dir, else search all.
    for cand in sorted(mesh_path.parent.glob("*_id71.bin")):
        skel = em.parse_skeleton_file(cand.read_bytes())
        if skel and skel.bone_count_raw >= mesh_section_count:
            return cand
    for cand in sorted(search_root.rglob("*_id71.bin")):
        skel = em.parse_skeleton_file(cand.read_bytes())
        if skel and skel.bone_count_raw >= mesh_section_count:
            return cand
    return None


def main(argv: list[str]) -> int:
    # Subcommand dispatch: `export_gltf.py level ...` exports an id 0x44 level
    # scene; the default (no/other first token) exports a character rig.
    if argv and argv[0] == "level":
        return _level_main(argv[1:])

    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--mesh", required=True,
                   help="character-mesh file with per-bone VIF prefix "
                        "(e.g. extract/chunk21/f17_id8f.bin)")
    p.add_argument("--skel",
                   help="id 0x71 animation/skeleton file (e.g. "
                        "extract/chunk05/f04_id71.bin). If omitted, auto-pair "
                        "the first compatible id 0x71 under --search-root.")
    p.add_argument("--search-root", default="extract",
                   help="root for auto-pair search (default: extract)")
    p.add_argument("--out", default="models/Extermination_Player.glb",
                   help="output .glb path")
    p.add_argument("--fps", type=float, default=30.0,
                   help="frames-per-second for animation time conversion "
                        "(default: 30)")
    args = p.parse_args(argv)

    mesh_path = Path(args.mesh)
    if not mesh_path.is_file():
        print(f"error: mesh not found: {mesh_path}", file=sys.stderr)
        return 2

    if args.skel:
        skel_path = Path(args.skel)
    else:
        d = mesh_path.read_bytes()
        table = em._find_bone_section_table(d)
        if table is None:
            print(f"error: no bone section table in {mesh_path}", file=sys.stderr)
            return 2
        skel_path = _auto_pair_skel(mesh_path, len(table[1]), Path(args.search_root))
        if not skel_path:
            print("error: no compatible id 0x71 skeleton found", file=sys.stderr)
            return 2
        print(f"# auto-paired skeleton: {skel_path}")

    info = build_glb_unified(mesh_path, skel_path, Path(args.out),
                              fps=args.fps,
                              search_root=Path(args.search_root))
    print(f"wrote {info['out_path']} ({info['size_bytes']} bytes)")
    print(f"  bones      : {info['n_bones']}")
    print(f"  meshes     : {info['n_meshes']}  (one rigid mesh per non-empty bone + 1 static textured)")
    print(f"  triangles  : {info['n_triangles']}  (points-only fallback bones: {info['bones_as_points']})")
    print(f"  animations : {info['n_animations']}")
    print(f"  tracks     : {info['total_animation_tracks']}")
    print(f"  samples    : {info['total_animation_samples']}")
    st = info.get("static_textured") or {}
    if st:
        print(f"  blocks     : {st.get('blocks', 0)} (bound to bones: {st.get('bound', 0)})")
        print(f"  textured   : {st.get('vertices',0)} verts / {st.get('triangles',0)} tris / "
              f"{st.get('primitives',0)} prims")
        for sh in st.get("sheets", []):
            src = sh.get("source") or "MISSING"
            sz = sh.get("size", [0,0])
            n_strips = sh.get('n_strips', sh.get('n_strips_total', 0))
            print(f"    sheet DBP={sh['dbp']:5d} ({n_strips:4d} strips)  "
                  f"{sz[0]}x{sz[1]}  source={src}")
        print(f"  images/textures/materials: {info['n_images']}/{info['n_textures']}/{info['n_materials']}")
    print(f"  buffer     : {info['buffer_bytes']} bytes  |  json: {info['json_bytes']} bytes")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
