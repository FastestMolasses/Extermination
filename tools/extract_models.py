#!/usr/bin/env python3
"""Extermination (SCUS-97112) 3D model extractor.

Original tooling for the Extermination decompilation project. Reads only files
the user supplies from their own legally-dumped disc; it redistributes nothing
and embeds no disc-derived data.

----------------------------------------------------------------------------
Geometry format (reverse-engineered, validated empirically)
----------------------------------------------------------------------------

3D geometry lives in the large `id 0x44` level files (`*_id44.bin` under the
`extract/` tree). A geometry file is a sequence of variable-length BLOCKS.

BLOCK STRUCTURE
  Blocks are delimited by a 16-byte separator row:

      00 00 00 17  00 00 00 00  00 00 00 00  00 00 00 00

  Immediately after every separator is a 16-byte DESCRIPTOR row that names the
  block kind. The first block in a file has no leading separator. The observed
  descriptor kinds are:

    * MESH        descriptor ends with `04 04 00 01 00 80 80 6c`.
                  Geometry begins at separator+0x20 (i.e. right after the
                  descriptor) and runs to the next separator.
    * SUBMESH     descriptor starts `0X 00 00 00 .. .. .. .. 4d 04 00 00 ..`.
                  A mesh with an extra ~0x40-byte sub-header (an index, a
                  vertex count and a bounding box). The mesh descriptor
                  `04 04 00 01 00 80 80 6c` appears again partway into the
                  block; geometry begins right after it.
    * MATRIX      descriptor has `ff ff ff ff` at +0x04. A scene-graph node /
                  instance-transform table (4x4 matrices), NOT raw geometry.
                  Skipped by this tool.
    * FILLER      descriptor is all `0xff`. Padding. Skipped.

VERTEX RECORD -- 64 bytes, four 16-byte rows:
    +0x00  marker row : `<u32 m0> <u32 m1> 00 00 00 00 00 00 00 00`
                        m0/m1 are constant within one triangle strip (a
                        per-strip id / material-ish key). Byte 7 of the record
                        (the high byte of m1) is the STRIP FLAG: 0x00 for the
                        two priming vertices of a strip, 0x20 for every
                        continuation vertex.
    +0x10  vec4       : (u, v, 1.0, 0.0)  -- texture coordinates
    +0x20  vec4       : either a unit NORMAL (nx,ny,nz,0)  -- dynamic/skinned
                        meshes -- or a vertex COLOR (r,g,b,1) with components
                        in 0..1 -- static/world meshes with baked lighting.
                        Distinguished per-vertex: |xyz|~=1 => normal.
    +0x30  vec4       : world-space POSITION (x, y, z, w) with w ~= +/-1.0.

  A real vertex always has |w| ~= 1.0. Header/padding rows that share the
  64-byte grid do not, which is the reliable validity test.

TRIANGLE TOPOLOGY -- triangle strips.
  Within a block, consecutive valid vertices form strips. A new strip starts
  at a vertex whose strip flag is 0x00 when the previous vertex's flag was not
  0x00 (the `00 00 20 20 ...` pattern). Each strip of N vertices yields N-2
  triangles with the standard alternating winding. Degenerate (zero-area)
  triangles -- the PS2 idiom for stitching strips into one draw call -- are
  dropped on export. Invalid (|w| != 1) rows also break the current strip.

  Validated on `chunk04.n0/f06_id44.bin`: 19271 non-degenerate triangles,
  zero spanning the level, coherent bounding box, unit normals.

UNCERTAIN / NOT YET RESOLVED
  * Material/texture binding. m0/m1 are constant per strip and clearly key
    something (likely a material or texture-page index) but the mapping to the
    GS texture packets is not decoded. Strips are grouped into OBJ objects by
    (m0, m1) so the grouping is preserved for later.
  * The SUBMESH sub-header fields (index, counts, bbox) are only partially
    interpreted; this tool locates the geometry within the block empirically
    rather than trusting the declared counts.
  * The MATRIX blocks hold instance transforms; this tool exports geometry in
    its stored object-space coordinates and does not apply them.

Usage:
  extract_models.py --in extract --out models
  extract_models.py --file extract/chunk04.n0/f06_id44.bin --out models
"""
from __future__ import annotations

import argparse
import math
import struct
import sys
from pathlib import Path

# 16-byte block separator.
SEPARATOR = b"\x00\x00\x00\x17" + b"\x00" * 12
# Last 8 bytes of a MESH descriptor row.
MESH_SIG = bytes.fromhex("040400010080806c")
# Bytes [8:12] of a SUBMESH descriptor row.
SUBMESH_SIG = bytes.fromhex("4d040000")

VERT_SIZE = 0x40
# A real vertex's position w is ~= +/-1.0; padding/header rows are not.
W_TOLERANCE = 0.05
# A normal has |xyz| ~= 1.0; a vertex colour does not.
NORMAL_TOLERANCE = 0.15
# Triangles with area below this are the strip-stitching degenerates -> drop.
MIN_TRI_AREA = 1e-6


class Vertex:
    """One 64-byte geometry record."""

    __slots__ = ("flag", "uv", "attr", "pos", "is_normal")

    def __init__(self, flag: int, uv, attr, pos):
        self.flag = flag
        self.uv = uv
        self.attr = attr  # (x,y,z) -- a normal or an RGB colour
        self.pos = pos    # (x,y,z)
        n = math.sqrt(attr[0] ** 2 + attr[1] ** 2 + attr[2] ** 2)
        self.is_normal = abs(n - 1.0) < NORMAL_TOLERANCE


class Strip:
    """A run of vertices forming one triangle strip, keyed by its marker."""

    __slots__ = ("key", "verts")

    def __init__(self, key: tuple[int, int]):
        self.key = key
        self.verts: list[Vertex] = []


def find_separators(d: bytes) -> list[int]:
    """Offsets of every 16-byte block separator row."""
    out = []
    i = d.find(SEPARATOR)
    while i != -1:
        out.append(i)
        i = d.find(SEPARATOR, i + 1)
    return out


def block_bounds(d: bytes) -> list[tuple[int, int]]:
    """[(start, end), ...] for every block, including the leading block 0."""
    seps = find_separators(d)
    if not seps:
        return []
    edges = [0] + seps + [len(d)]
    return [(edges[i], edges[i + 1]) for i in range(len(edges) - 1)]


def mesh_vertex_start(d: bytes, start: int, end: int, is_first: bool) -> int | None:
    """Return the offset where a block's vertex records begin, or None.

    `is_first` marks block 0, which has no leading separator/descriptor.
    """
    if is_first:
        # Block 0 begins straight into vertex data.
        return start
    desc = d[start + 0x10:start + 0x20]
    if len(desc) < 16:
        return None
    if desc[8:16] == MESH_SIG:
        return start + 0x20
    if desc[8:12] == SUBMESH_SIG:
        # A submesh: the real mesh descriptor appears again inside the block.
        p = d.find(MESH_SIG, start, end)
        if p < 0:
            return None
        return p + 8  # right after the 16-byte descriptor row
    # MATRIX / FILLER / unknown -- not raw geometry.
    return None


def read_vertices(d: bytes, vstart: int, end: int) -> list[tuple | None]:
    """Decode 64-byte records into (Vertex, strip_key) pairs.

    A None entry marks an invalid row -- a header/padding record whose
    position w is not ~= +/-1.0 -- which breaks the current triangle strip.
    The strip key is (m0, m1 & 0xFFFFFF): the marker fields with the per-vertex
    0x20 strip flag masked out, so it is constant across a whole strip.
    """
    out: list[tuple | None] = []
    o = vstart
    while o + VERT_SIZE <= end:
        w = struct.unpack_from("<f", d, o + 0x3C)[0]
        if abs(abs(w) - 1.0) < W_TOLERANCE:
            flag = d[o + 7]
            m0, m1 = struct.unpack_from("<II", d, o)
            uv = struct.unpack_from("<2f", d, o + 0x10)
            attr = struct.unpack_from("<3f", d, o + 0x20)
            pos = struct.unpack_from("<3f", d, o + 0x30)
            out.append((Vertex(flag, uv, attr, pos), (m0, m1 & 0x00FFFFFF)))
        else:
            out.append(None)
        o += VERT_SIZE
    return out


def build_strips(records: list) -> list[Strip]:
    """Split a block's vertex list into triangle strips.

    A new strip begins at a flag-0x00 vertex following a non-0x00 vertex (the
    `00 00 20 20 ...` priming pattern); an invalid (None) row also breaks it.
    """
    strips: list[Strip] = []
    cur: Strip | None = None
    prev_flag: int | None = None
    for rec in records:
        if rec is None:
            cur = None
            prev_flag = None
            continue
        v, key = rec
        new_strip = (
            cur is None
            or (v.flag == 0 and prev_flag is not None and prev_flag != 0)
            or key != cur.key
        )
        if new_strip:
            cur = Strip(key)
            strips.append(cur)
        cur.verts.append(v)
        prev_flag = v.flag
    return [s for s in strips if len(s.verts) >= 3]


def tri_area(a, b, c) -> float:
    ux, uy, uz = b[0] - a[0], b[1] - a[1], b[2] - a[2]
    vx, vy, vz = c[0] - a[0], c[1] - a[1], c[2] - a[2]
    cx = uy * vz - uz * vy
    cy = uz * vx - ux * vz
    cz = ux * vy - uy * vx
    return 0.5 * math.sqrt(cx * cx + cy * cy + cz * cz)


def strip_triangles(strip: Strip):
    """Yield (i0, i1, i2) local vertex indices for non-degenerate triangles.

    Standard triangle-strip winding with the alternating flip; degenerate
    (zero-area) triangles -- the strip-stitching idiom -- are skipped.
    """
    v = strip.verts
    for t in range(len(v) - 2):
        if t & 1:
            i0, i1, i2 = t + 1, t, t + 2
        else:
            i0, i1, i2 = t, t + 1, t + 2
        if tri_area(v[i0].pos, v[i1].pos, v[i2].pos) < MIN_TRI_AREA:
            continue
        yield i0, i1, i2


def parse_file(d: bytes) -> list[Strip]:
    """Return every triangle strip in a geometry file."""
    blocks = block_bounds(d)
    if not blocks:
        return []
    strips: list[Strip] = []
    for idx, (start, end) in enumerate(blocks):
        vstart = mesh_vertex_start(d, start, end, is_first=(idx == 0))
        if vstart is None:
            continue
        records = read_vertices(d, vstart, end)
        strips.extend(build_strips(records))
    return strips


def write_obj(path: Path, strips: list[Strip]) -> tuple[int, int]:
    """Write strips to a Wavefront OBJ. Returns (vertex_count, face_count).

    Strips are grouped into OBJ objects by their marker key, so the in-file
    strip/material grouping is preserved. Each strip's vertices are emitted
    once; positions become `v`, UVs `vt`, and unit normals `vn` (colour-only
    strips get no `vn`).
    """
    lines: list[str] = [
        "# Extermination (SCUS-97112) model -- exported by tools/extract_models.py",
        "# Coordinates are object-space as stored on disc (no instance transform).",
    ]
    v_base = 1   # OBJ indices are 1-based
    vt_base = 1
    vn_base = 1
    total_v = 0
    total_f = 0

    # Group strips by marker key for stable, meaningful OBJ objects.
    groups: dict[tuple[int, int], list[Strip]] = {}
    order: list[tuple[int, int]] = []
    for s in strips:
        if s.key not in groups:
            groups[s.key] = []
            order.append(s.key)
        groups[s.key].append(s)

    for gi, key in enumerate(order):
        group = groups[key]
        lines.append(f"o mesh_{gi:04d}_{key[0]:08x}_{key[1]:06x}")
        for s in group:
            # Emit this strip's vertex attributes.
            for v in s.verts:
                lines.append(f"v {v.pos[0]:.6f} {v.pos[1]:.6f} {v.pos[2]:.6f}")
            for v in s.verts:
                # OBJ texture V axis is bottom-up; PS2 stores top-down.
                lines.append(f"vt {v.uv[0]:.6f} {1.0 - v.uv[1]:.6f}")
            has_normals = all(v.is_normal for v in s.verts)
            if has_normals:
                for v in s.verts:
                    lines.append(
                        f"vn {v.attr[0]:.6f} {v.attr[1]:.6f} {v.attr[2]:.6f}"
                    )
            nverts = len(s.verts)
            for i0, i1, i2 in strip_triangles(s):
                def ref(i: int) -> str:
                    vi = v_base + i
                    ti = vt_base + i
                    if has_normals:
                        ni = vn_base + i
                        return f"{vi}/{ti}/{ni}"
                    return f"{vi}/{ti}"
                lines.append(f"f {ref(i0)} {ref(i1)} {ref(i2)}")
                total_f += 1
            v_base += nverts
            vt_base += nverts
            if has_normals:
                vn_base += nverts
            total_v += nverts

    path.write_text("\n".join(lines) + "\n")
    return total_v, total_f


def process(path: Path, out_dir: Path) -> str | None:
    """Parse one geometry file and write an OBJ. Returns a status line."""
    d = path.read_bytes()
    strips = parse_file(d)
    if not strips:
        return None
    name = f"{path.parent.name}_{path.stem}.obj"
    nv, nf = write_obj(out_dir / name, strips)
    if nf == 0:
        # No real triangles after dropping degenerates -- discard the file.
        (out_dir / name).unlink(missing_ok=True)
        return None
    return f"{path.parent.name}/{path.name}: {len(strips)} strips, {nv} verts, {nf} tris -> {name}"


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Extermination 3D model extractor")
    p.add_argument("--in", dest="input", default="extract",
                   help="extraction directory to scan for *_id44.bin (default: extract)")
    p.add_argument("--file", help="convert a single geometry file instead of scanning")
    p.add_argument("--out", default="models",
                   help="output directory for OBJ files (default: models)")
    args = p.parse_args(argv)

    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)

    if args.file:
        paths = [Path(args.file)]
    else:
        paths = sorted(Path(args.input).rglob("*_id44.bin"))

    done = 0
    skipped = 0
    for path in paths:
        if not path.is_file():
            print(f"skip (not found): {path}")
            continue
        line = process(path, out_dir)
        if line:
            print(line)
            done += 1
        else:
            skipped += 1
    print(f"\n{done} model file(s) exported to {out_dir}/  "
          f"({skipped} file(s) had no geometry)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
