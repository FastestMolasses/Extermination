#!/usr/bin/env python3
"""
export_gltf.py -- Export an Extermination (SCUS-97112) character as a glTF 2.0
binary (.glb) bundle containing:

  * the skinned render mesh (stage-2 VIF strip blocks, textured per sheet),
  * a glTF skin: node hierarchy from the keyed-animation container's parent
    table (rig = the table with n == max_slot + 1; ids 0x70/0x74/0xd0/...),
    per-vertex JOINTS_0/WEIGHTS_0 (single influence, weight 1.0),
  * one animation per matching container (rotation/translation/scale tracks).

Pipeline (decoded 2026-06-09, FINDINGS.md "Skinned-character pipeline FULLY
DECODED" / "id 0x74 channel encodings FULLY DECODED"): geometry records are
[tex/marker][ST][normal][pos+w] qwords; each vertex's node is encoded in its
position-W float read as an integer (bits 0..9 = VU1 dmem qword address of
the node's matrix set at qw 8*node, so node = (W_bits & 0x3FF) >> 3; bit 15 =
strip restart). Positions/normals are BONE-LOCAL, so the skin's inverse bind
matrices are identity and vertex_world = joint_world * pos -- the same
contract as the PS2 kernel and the EMDL exporter (export_native.py).
Animation channels are sparse keyframes: rot = 4x20-bit truncated-float quat
(the engine composes with the CONJUGATE of the stored quat -- conjugated here
too), trn/scl = 3x26-bit vec3, played at 60 fps.

USAGE
    python3 tools/export_gltf.py \
        --mesh extract/chunk28/f00_id3b.bin \
        --anim extract/chunk28/f01_id3c.bin \
        --out  models/chunk28_character.glb

If --anim is omitted, containers embedded in the mesh file are used, else
the same-chunk sibling file with the most matching-node-count containers.
Without any animation source the mesh exports rigid (bone-local parts
overlap at the origin -- same as export_native.py's identity palette).
`--clips 0,346` / `--clips 0-9` selects containers (default: all).
(--skel is accepted for backward compatibility and ignored: the id 0x71
clips target a different rig pairing than the id 0x74 node slots.)

The output .glb opens in Blender (File > Import > glTF 2.0), Maya 2022+, and
any compliant viewer (gltf-viewer, three.js, Babylon Sandbox, etc.). Every
exported clip shows up under the file's Animations panel.

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
# Skinned-character mesh loader (corrected pipeline, 2026-06-09)
#
# The old per-bone object-space decoders (load_per_bone_meshes*) are GONE:
# the id 0x74 prefix records they read are animation keyframes, not vertices
# (FINDINGS.md "id 0x74 prefix is ANIMATION, not geometry"). Real geometry is
# the stage-2 VIF strip-block stream consumed by the 62-qw VU1 skinning
# kernel at vram 0x0023C780. Block discovery is shared with
# export_native.load_mesh_sections (raw-blob and MESH_SIG-style files); this
# loader additionally keeps each record's ST (UV) and marker m0 (texture
# sheet field) so the glTF output can be textured per sheet.

VERT_REC = 0x40   # 4 qwords per record: [tex/marker][ST][normal][pos+w]


def load_skinned_sections(mesh_path: Path, segment: int = 0,
                          fallback_dbp: int = -1):
    """Decode the skinned render mesh, grouped by texture-sheet DBP.

    Returns (groups, max_slot) with groups = {dbp: [pos, nrm, uv, slot,
    tris]}: bone-local positions/normals, per-vertex node slot decoded from
    the position-W bits ((W_bits & 0x3FF) >> 3 -- validated live, see
    FINDINGS.md), and flat triangle index lists per group. Strip walking
    mirrors export_native.load_mesh_sections exactly (restart bit 15,
    alternating winding by strip parity), with one addition: a marker
    (sheet) change also breaks the running strip so no triangle straddles
    two texture sheets.
    """
    en = _load("_export_native", "export_native.py")
    d = mesh_path.read_bytes()
    if struct.unpack_from("<I", d, 0)[0] < 0x1000 and \
            d[0x48:0x50] == bytes.fromhex("040400010080806c"):
        payloads, _n = en._walk_blob_blocks(d)
    else:
        payloads = en._walk_meshsig_blocks(d, segment)
    if not payloads:
        raise RuntimeError(f"no skinned-mesh blocks found in {mesh_path}")

    groups: dict[int, list] = {}   # dbp -> [pos, nrm, uv, slot, tris]
    welds: dict[int, dict] = {}    # dbp -> weld map
    max_slot = 0

    def vid_of(dbp, p, n, t, b):
        g = groups.setdefault(dbp, [[], [], [], [], []])
        weld = welds.setdefault(dbp, {})
        key = (round(p[0], 4), round(p[1], 4), round(p[2], 4),
               round(n[0], 3), round(n[1], 3), round(n[2], 3),
               round(t[0], 5), round(t[1], 5), b)
        i = weld.get(key)
        if i is None:
            i = len(g[0])
            weld[key] = i
            g[0].append(p)
            g[1].append(n)
            g[2].append(t)
            g[3].append(b)
        return i

    for payload in payloads:
        run = []        # (welded id, restart flag) of the running strip
        run_dbp = None
        for r in range(0, len(payload) - VERT_REC + 1, VERT_REC):
            w = struct.unpack_from("<f", payload, r + 0x3c)[0]
            if abs(abs(w) - 1.0) > 0.25:
                break
            wbits = struct.unpack_from("<I", payload, r + 0x3c)[0]
            slot = (wbits & 0x3FF) >> 3
            if slot < 2:        # nodes 0..1 carry no skin
                continue
            max_slot = max(max_slot, slot)
            m0 = struct.unpack_from("<I", payload, r)[0]
            dbp = _sheet_field_to_dbp((m0 >> 15) & 0x3FFF) \
                if m0 else fallback_dbp
            pos = struct.unpack_from("<3f", payload, r + 0x30)
            nrm = struct.unpack_from("<3f", payload, r + 0x20)
            uv = struct.unpack_from("<2f", payload, r + 0x10)
            restart = bool(wbits & 0x8000)
            if dbp != run_dbp:
                run = []
                run_dbp = dbp
            vi = vid_of(dbp, pos, nrm, uv, slot)
            run.append((vi, restart))
            k = len(run)
            if k >= 3 and not restart:
                a, b, c = run[k - 3][0], run[k - 2][0], run[k - 1][0]
                if a != b and b != c and a != c:
                    # alternate winding by strip parity
                    if (k & 1) == 0:
                        groups[dbp][4].extend((a, b, c))
                    else:
                        groups[dbp][4].extend((b, a, c))

    groups = {dbp: g for dbp, g in groups.items() if g[4]}
    if not groups:
        raise RuntimeError(f"no triangles decoded from {mesh_path}")
    return groups, max_slot


# ---------------------------------------------------------------------------
# id 0x74 animation containers -> skeleton + glTF tracks
#
# Channel encodings (FINDINGS.md "id 0x74 channel encodings FULLY DECODED",
# live-verified): rot = 4x20-bit truncated-float local quat with the engine
# composing R(CONJUGATE(q)), trn/scl = 3x26-bit vec3, sparse keys with LERP
# semantics, clips advance at 60 fps. extract_models.parse_id74_prefix
# decodes a container to per-node (frame, values) key lists.


def select_anim_containers(d: bytes, max_slot: int):
    """Containers in `d` usable as this mesh segment's rig + clips.

    Pairing rule (PROGRESS session 4b, rig_probe survey of 126 prefixed
    files): a segment's rig is the in-file parent table with
    n == max_slot + 1 (slots 0..1 exist but carry no skin). Only when no
    such table exists do we fall back to the dominant node count
    > max_slot (cross-file libraries like chunk28's f01_id3c). Containers
    are enumerated with rig_probe.scan_anim_headers, which finds EVERY
    blob id (0x70/0x74/0xd0/...; extract_models.find_id74_headers only
    knows 0x74/0x2c) -- e.g. chunk21/f17_id8f pairs segment 0 with its 11
    id-0x70 20-node clips and segment 1 with its 30 id-0xd0 44-node
    creature clips, NOT with the 21-node id-0x74 cutscene track.

    Returns (n_nodes, [header_offset, ...]), or (0, []) if none qualify.
    """
    rp = _load("_rig_probe", "rig_probe.py")
    counts: dict[int, list[int]] = {}
    for h in rp.scan_anim_headers(d):
        if h["n"] > max_slot:
            counts.setdefault(h["n"], []).append(h["hdr"])
    if not counts:
        return 0, []
    n_nodes = max_slot + 1 if max_slot + 1 in counts else \
        max(counts, key=lambda n: len(counts[n]))
    return n_nodes, counts[n_nodes]


def _rot_track(keys: list, fps: float):
    """(times_sec, quats) for one node's rotation keys, conjugated to the
    engine's composition convention and normalised (the stored quats keep
    only 11 mantissa bits, |q| ~ 0.9997)."""
    times, vals = [], []
    last = None
    for frame, q in keys:
        t = frame / fps
        if last is not None and t <= last:
            t = last + 1e-4      # glTF needs strictly increasing input
        times.append(t)
        vals.append(_quat_from_decoded(-q[0], -q[1], -q[2], q[3]))
        last = t
    return times, vals


def _vec_track(keys: list, fps: float):
    times, vals = [], []
    last = None
    for frame, v in keys:
        t = frame / fps
        if last is not None and t <= last:
            t = last + 1e-4
        times.append(t)
        vals.append(v)
        last = t
    return times, vals


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

    def add_joints_u16(self, slots: List[int]) -> int:
        """JOINTS_0: one u16 vec4 per vertex, single influence in lane 0."""
        data = struct.pack(f"<{len(slots) * 4}H",
                           *[c for s in slots for c in (s, 0, 0, 0)])
        return self.add_accessor(
            data, self.COMP_UNSIGNED_SHORT, len(slots), "VEC4",
            target=34962,
        )

    def add_weights_one(self, count: int) -> int:
        """WEIGHTS_0: (1, 0, 0, 0) per vertex (rigid single-bone skinning)."""
        data = struct.pack("<4f", 1.0, 0.0, 0.0, 0.0) * count
        return self.add_accessor(
            data, self.COMP_FLOAT, count, "VEC4",
            target=34962,
        )

    def add_mat4_float(self, mats: List[tuple]) -> int:
        """MAT4 accessor (column-major column tuples, e.g. inverse binds)."""
        data = struct.pack(f"<{len(mats) * 16}f",
                           *[v for m in mats for col in m for v in col])
        return self.add_accessor(data, self.COMP_FLOAT, len(mats), "MAT4")

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
# Character build: skinned + textured + animated (corrected pipeline).
#
# Replaces the 2026-05 build_glb_unified path (void per-bone object-space
# decoders + spatial-proximity block binding + id 0x71 clips on the wrong
# rig pairing). The skin's inverse bind matrices are identity because the
# disc vertices are already bone-local: vertex_world = joint_world * pos,
# the exact PS2-kernel / EMDL contract.


_IDENT_COLS = ((1.0, 0.0, 0.0, 0.0), (0.0, 1.0, 0.0, 0.0),
               (0.0, 0.0, 1.0, 0.0), (0.0, 0.0, 0.0, 1.0))


def _resolve_anim_source(mesh_path: Path, mesh_bytes: bytes, max_slot: int,
                         anim_path: Path | None):
    """Pick the id 0x74 animation library for this mesh.

    Returns (anim_bytes, src_path, n_nodes, headers); (None, None, 0, [])
    when there is no usable source. Order: explicit --anim file; containers
    embedded in the mesh file itself (e.g. chunk21/f17_id8f); the same-chunk
    sibling with the most qualifying containers (e.g. chunk28/f00_id3b ->
    f01_id3c, the 455-clip library).
    """
    if anim_path is not None:
        d = anim_path.read_bytes()
        n, hdrs = select_anim_containers(d, max_slot)
        if not hdrs:
            raise RuntimeError(f"{anim_path}: no id 0x74 containers with "
                               f"more than {max_slot} nodes")
        return d, anim_path, n, hdrs
    n, hdrs = select_anim_containers(mesh_bytes, max_slot)
    if hdrs:
        return mesh_bytes, mesh_path, n, hdrs
    best = None
    for sib in sorted(mesh_path.parent.glob("*_id*.bin")):
        if sib.resolve() == mesh_path.resolve():
            continue
        try:
            d = sib.read_bytes()
        except OSError:
            continue
        n, hdrs = select_anim_containers(d, max_slot)
        if hdrs and (best is None or len(hdrs) > len(best[3])):
            best = (d, sib, n, hdrs)
    return best if best else (None, None, 0, [])


def _parse_clip_selection(spec: str, n_clips: int) -> list[int]:
    """'all' | '0,346' | '0-9,400-410' -> sorted container index list."""
    if not spec or spec == "all":
        return list(range(n_clips))
    out: set[int] = set()
    for part in spec.split(","):
        part = part.strip()
        if "-" in part:
            a, b = part.split("-", 1)
            out.update(range(int(a), int(b) + 1))
        elif part:
            out.add(int(part))
    bad = sorted(i for i in out if not (0 <= i < n_clips))
    if bad:
        raise RuntimeError(f"clip(s) {bad} out of range (0..{n_clips - 1})")
    return sorted(out)


def build_character_glb(mesh_path: Path, out_path: Path,
                        anim_path: Path | None = None, segment: int = 0,
                        fps: float = 60.0, search_root: Path | None = None,
                        no_textures: bool = False,
                        clips: str = "all") -> dict:
    mesh_bytes = mesh_path.read_bytes()
    groups, max_slot = load_skinned_sections(mesh_path, segment)

    anim_bytes, anim_src, n_nodes, headers = _resolve_anim_source(
        mesh_path, mesh_bytes, max_slot, anim_path)
    clip_sel = _parse_clip_selection(clips, len(headers)) if headers else []

    # Skeleton: parent table + rest pose (frame-0 keys of container 0; all
    # containers in a library share one parent table). Slots past the rig's
    # node count -- or every slot when no animation source exists -- become
    # root-parented identity joints, mirroring EMDL's identity slot.
    n_joints = max(n_nodes, max_slot + 1)
    parents = [-1] * n_joints
    rest = [((0.0, 0.0, 0.0, 1.0), (0.0, 0.0, 0.0), (1.0, 1.0, 1.0))
            for _ in range(n_joints)]
    if headers:
        pre0 = em.parse_id74_prefix(anim_bytes, headers[0])
        for i, p in enumerate(pre0["parents"]):
            parents[i] = p if -1 <= p < n_joints else -1
        for i in range(n_nodes):
            q = pre0["rot"][i][0][1] if pre0["rot"][i] else (0.0, 0.0, 0.0, 1.0)
            t = pre0["trn"][i][0][1] if pre0["trn"][i] else (0.0, 0.0, 0.0)
            s = pre0["scl"][i][0][1] if pre0["scl"][i] else (1.0, 1.0, 1.0)
            rest[i] = (q, t, s)

    gb = GLBBuilder()

    # Materials: one per texture-sheet DBP (identity-grayscale PNG -- the
    # colour-CLUT binding is engine-synthesised, see FINDINGS "Color source").
    image_objs: list = []
    texture_objs: list = []
    material_objs: list = []
    material_for_dbp: dict[int, int] = {}
    sheet_infos: list = []
    for dbp in sorted(groups):
        n_tris = len(groups[dbp][4]) // 3
        found = None if (no_textures or dbp < 0) else \
            _find_transfer_for_dbp(mesh_path, dbp, search_root)
        if found is not None:
            src_path, xfer = found
            bv = gb.add_raw_blob(
                _png_rgba_bytes(xfer.width, xfer.height, xfer.pixels))
            image_objs.append({"name": f"sheet_DBP{dbp}",
                               "mimeType": "image/png", "bufferView": bv})
            texture_objs.append({"source": len(image_objs) - 1})
            material_objs.append({
                "name": f"mat_dbp{dbp}",
                "pbrMetallicRoughness": {
                    "baseColorTexture": {"index": len(texture_objs) - 1,
                                         "texCoord": 0},
                    "metallicFactor": 0.0, "roughnessFactor": 1.0,
                },
                "doubleSided": True,
            })
            try:
                src_rel = str(src_path.relative_to(search_root)) \
                    if search_root else src_path.name
            except ValueError:
                src_rel = src_path.name
            sheet_infos.append({"dbp": dbp, "n_tris": n_tris,
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
            sheet_infos.append({"dbp": dbp, "n_tris": n_tris, "source": None})
        material_for_dbp[dbp] = len(material_objs) - 1

    # One skinned mesh; one primitive per texture sheet. JOINTS_0 carries the
    # per-vertex node slot directly (skin.joints is ordered so that joint
    # index == node slot).
    primitives: list = []
    total_tris = 0
    total_verts = 0
    for dbp in sorted(groups):
        pos, nrm, uv, slot, tris = groups[dbp]
        primitives.append({
            "attributes": {
                "POSITION": gb.add_vec3_float(pos),
                "NORMAL": gb.add_vec3_float_normal(nrm),
                "TEXCOORD_0": gb.add_vec2_float(uv),
                "JOINTS_0": gb.add_joints_u16(slot),
                "WEIGHTS_0": gb.add_weights_one(len(slot)),
            },
            "indices": gb.add_indices_auto(tris, len(pos)),
            "mode": 4,  # TRIANGLES
            "material": material_for_dbp[dbp],
        })
        total_tris += len(tris) // 3
        total_verts += len(pos)

    # Nodes: joints 0..n_joints-1 first (node index == slot), then the
    # skinned-mesh node, then the scene root.
    joint_nodes: list = []
    for i in range(n_joints):
        q, t, s = rest[i]
        node = {
            "name": f"node_{i:02d}",
            "rotation": list(_quat_from_decoded(-q[0], -q[1], -q[2], q[3])),
            "translation": [t[0], t[1], t[2]],
        }
        if any(abs(c - 1.0) > 1e-4 for c in s):
            node["scale"] = [s[0], s[1], s[2]]
        joint_nodes.append(node)
    children_of: dict[int, list[int]] = {}
    roots: list[int] = []
    for i, p in enumerate(parents):
        if 0 <= p < n_joints and p != i:
            children_of.setdefault(p, []).append(i)
        else:
            roots.append(i)
    for i, kids in children_of.items():
        joint_nodes[i]["children"] = kids

    nodes = list(joint_nodes)
    mesh_node_idx = len(nodes)
    nodes.append({"name": f"{mesh_path.stem}_mesh", "mesh": 0, "skin": 0})
    scene_root_idx = len(nodes)
    nodes.append({"name": mesh_path.stem,
                  "children": roots + [mesh_node_idx]})

    skin = {
        "name": f"{mesh_path.stem}_skin",
        "joints": list(range(n_joints)),
        # Identity inverse binds: disc vertices are bone-local already.
        "inverseBindMatrices": gb.add_mat4_float([_IDENT_COLS] * n_joints),
    }

    # Animations: one glTF animation per selected container. Keys are the
    # sparse disc keyframes verbatim (times = frame/fps); LINEAR sampling
    # matches the engine's lerp-between-keys (NLERP vs engine SLERP is
    # within ~0.6 deg on 40-deg key gaps, see FINDINGS).
    animations: list = []
    total_channels = 0
    total_keys = 0
    for ci in clip_sel:
        pre = em.parse_id74_prefix(anim_bytes, headers[ci])
        channels: list = []
        samplers: list = []

        def add_channel(path, times, vals, node_i, is_quat):
            nonlocal total_keys
            out_acc = gb.add_vec4_float_anim(vals) if is_quat \
                else gb.add_vec3_float_anim(vals)
            samplers.append({"input": gb.add_scalar_float(times),
                             "output": out_acc, "interpolation": "LINEAR"})
            channels.append({"sampler": len(samplers) - 1,
                             "target": {"node": node_i, "path": path}})
            total_keys += len(times)

        for i in range(min(pre["n"], n_joints)):
            if pre["rot"][i]:
                times, vals = _rot_track(pre["rot"][i], fps)
                add_channel("rotation", times, vals, i, True)
            if pre["trn"][i]:
                times, vals = _vec_track(pre["trn"][i], fps)
                add_channel("translation", times, vals, i, False)
            scl = pre["scl"][i]
            # Scale has been constant (1,1,1) in everything sampled so far;
            # emit the track only when a key actually deviates.
            if scl and any(abs(c - 1.0) > 1e-4 for _f, v in scl for c in v):
                times, vals = _vec_track(scl, fps)
                add_channel("scale", times, vals, i, False)
        if channels:
            animations.append({"name": f"clip_{ci:03d}",
                               "channels": channels, "samplers": samplers})
            total_channels += len(channels)

    while len(gb.bin_blob) % 4 != 0:
        gb.bin_blob.append(0)

    gltf = {
        "asset": {"version": "2.0",
                  "generator": "Extermination decomp export_gltf.py"},
        "scene": 0,
        "scenes": [{"name": "Scene", "nodes": [scene_root_idx]}],
        "nodes": nodes,
        "meshes": [{"name": mesh_path.stem, "primitives": primitives}],
        "skins": [skin],
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

    return {
        "out_path": str(out_path),
        "size_bytes": len(header) + len(json_chunk) + len(bin_chunk),
        "anim_source": str(anim_src) if anim_src is not None else None,
        "n_containers": len(headers),
        "n_clips": len(animations),
        "n_nodes": n_nodes,
        "n_joints": n_joints,
        "max_slot": max_slot,
        "n_vertices": total_verts,
        "n_triangles": total_tris,
        "n_primitives": len(primitives),
        "total_channels": total_channels,
        "total_keys": total_keys,
        "sheets": sheet_infos,
        "sheets_resolved": sum(1 for s in sheet_infos if s.get("source")),
        "n_materials": len(material_objs),
        "buffer_bytes": len(bin_bytes),
        "json_bytes": len(json_bytes),
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


def main(argv: list[str]) -> int:
    # Subcommand dispatch: `export_gltf.py level ...` exports an id 0x44 level
    # scene; the default (no/other first token) exports a skinned character.
    if argv and argv[0] == "level":
        return _level_main(argv[1:])

    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--mesh", required=True,
                   help="skinned character-mesh file (raw VIF blob like "
                        "extract/chunk28/f00_id3b.bin, or MESH_SIG-style like "
                        "extract/chunk21/f17_id8f.bin)")
    p.add_argument("--anim",
                   help="id 0x74 animation-library file (e.g. "
                        "extract/chunk28/f01_id3c.bin). If omitted, use "
                        "containers embedded in the mesh file, else the "
                        "same-chunk sibling with the most containers.")
    p.add_argument("--skel",
                   help="DEPRECATED, ignored (was the id 0x71 clip source; "
                        "those clips target a different rig pairing than the "
                        "id 0x74 node slots -- see docs/FINDINGS.md)")
    p.add_argument("--segment", type=int, default=0,
                   help="model segment for MESH_SIG-style files (MATRIX-"
                        "separator groups; default 0)")
    p.add_argument("--clips", default="all",
                   help="containers to export as animations: 'all' (default) "
                        "or comma/range list like '0,346' / '0-9'")
    p.add_argument("--fps", type=float, default=60.0,
                   help="keyframe playback rate (default: 60 -- clips "
                        "advance one frame per 60 Hz tick on the PS2)")
    p.add_argument("--no-textures", action="store_true",
                   help="skip texture resolution -- untextured primitives")
    p.add_argument("--search-root", default="extract",
                   help="root for cross-file texture resolution "
                        "(default: extract)")
    p.add_argument("--out", default="models/Extermination_Character.glb",
                   help="output .glb path")
    args = p.parse_args(argv)

    mesh_path = Path(args.mesh)
    if not mesh_path.is_file():
        print(f"error: mesh not found: {mesh_path}", file=sys.stderr)
        return 2
    if args.skel:
        print("# note: --skel is deprecated and ignored (animations now come "
              "from id 0x74 containers; see --anim)")
    anim_path = Path(args.anim) if args.anim else None
    if anim_path is not None and not anim_path.is_file():
        print(f"error: anim library not found: {anim_path}", file=sys.stderr)
        return 2

    info = build_character_glb(mesh_path, Path(args.out),
                               anim_path=anim_path, segment=args.segment,
                               fps=args.fps,
                               search_root=Path(args.search_root),
                               no_textures=args.no_textures,
                               clips=args.clips)
    print(f"wrote {info['out_path']} ({info['size_bytes']} bytes)")
    print(f"  joints     : {info['n_joints']}  (rig nodes: {info['n_nodes']},"
          f" max vertex slot: {info['max_slot']})")
    print(f"  vertices   : {info['n_vertices']}   triangles: "
          f"{info['n_triangles']}   primitives: {info['n_primitives']}")
    if info["anim_source"]:
        print(f"  animations : {info['n_clips']}/{info['n_containers']} "
              f"containers from {info['anim_source']}")
        print(f"  channels   : {info['total_channels']}   keys: "
              f"{info['total_keys']}")
    else:
        print("  animations : none (no id 0x74 source found -- rigid export, "
              "bone-local parts overlap)")
    print(f"  textures   : {info['sheets_resolved']}/{len(info['sheets'])} "
          f"sheets resolved, {info['n_materials']} materials")
    for sh in info["sheets"]:
        src = sh.get("source") or "MISSING (gray placeholder)"
        sz = sh.get("size")
        szs = f"{sz[0]}x{sz[1]}" if sz else "-"
        print(f"    DBP={sh['dbp']:6d} ({sh['n_tris']:5d} tris)  {szs:>9}  "
              f"source={src}")
    print(f"  buffer     : {info['buffer_bytes']} bytes  |  json: "
          f"{info['json_bytes']} bytes")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
