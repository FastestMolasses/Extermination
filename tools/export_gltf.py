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


def build_glb_unified(mesh_path: Path, skel_path: Path, out_path: Path,
                      fps: float = 30.0) -> dict:
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

    # 4. Pad buffer to 4
    while len(gb.bin_blob) % 4 != 0:
        gb.bin_blob.append(0)

    gltf = {
        "asset": {
            "version": "2.0",
            "generator": "Extermination decomp export_gltf.py",
        },
        "scene": 0,
        "scenes": [{"name": "Scene", "nodes": [scene_root_idx]}],
        "nodes": nodes,
        "meshes": meshes,
        "accessors": gb.accessors,
        "bufferViews": gb.bufferViews,
        "buffers": [{"byteLength": len(gb.bin_blob)}],
    }
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
    }


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

    info = build_glb_unified(mesh_path, skel_path, Path(args.out), fps=args.fps)
    print(f"wrote {info['out_path']} ({info['size_bytes']} bytes)")
    print(f"  bones      : {info['n_bones']}")
    print(f"  meshes     : {info['n_meshes']}  (one rigid mesh per non-empty bone)")
    print(f"  triangles  : {info['n_triangles']}  (points-only fallback bones: {info['bones_as_points']})")
    print(f"  animations : {info['n_animations']}")
    print(f"  tracks     : {info['total_animation_tracks']}")
    print(f"  samples    : {info['total_animation_samples']}")
    print(f"  buffer     : {info['buffer_bytes']} bytes  |  json: {info['json_bytes']} bytes")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
