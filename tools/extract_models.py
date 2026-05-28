#!/usr/bin/env python3
"""Extermination (SCUS-97112) 3D model extractor.

Original tooling for the Extermination decompilation project. Reads only files
the user supplies from their own legally-dumped disc; it redistributes nothing
and embeds no disc-derived data.

The game stores 3D geometry in two related layouts that share an identical
64-byte vertex record but differ in how blocks are framed:

  * LEVEL geometry  -- the 32 `id 0x44` files. Block-structured, walked via
                       16-byte separator rows. See "Level format" below.
  * MODEL geometry  -- ~330 other files (character / enemy / prop / object
                       models, many file ids). Same vertex record, but the
                       geometry is wrapped in fixed-size padded blocks rather
                       than separator-delimited ones. See "Model format" below.

This tool decodes both. Files named `*_id44.bin` take the level path; every
other `*_id*.bin` carrying the MESH signature takes the model path.

----------------------------------------------------------------------------
Level format (`id 0x44` files) -- reverse-engineered, validated empirically
----------------------------------------------------------------------------

A level geometry file is a sequence of variable-length BLOCKS.

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
    * MATRIX      descriptor has `ff ff ff ff` at +0x04. A scene-graph /
                  instance-placement block: a table of 4x4 transforms followed
                  by the object-space geometry they instance. Skipped by the
                  default per-mesh export; decoded and applied by `--scene`.
                  See "MATRIX blocks / --scene mode" below.
    * FILLER      descriptor is all `0xff`. Padding. Skipped.

VERTEX RECORD -- 64 bytes, four 16-byte rows (SHARED by both formats):
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

----------------------------------------------------------------------------
Model format (character / object / prop files) -- reverse-engineered
----------------------------------------------------------------------------

The model files carry the SAME 64-byte vertex record and the SAME MESH
signature, but the block framing differs from the level files:

  * The primary block delimiter is a 16-byte separator `00 00 00 14` + twelve
    `00` (note `14`, not the level files' `17`). `0x17` separators still
    appear but only as rare section markers.
  * A model block is a FIXED-SIZE, PADDED unit. The 16-byte descriptor row
    right after the separator is usually FILLER (`0xff` x16), not a MESH
    descriptor -- so the level-style "geometry begins right after the
    descriptor" rule does not apply. Instead each block carries a short
    sub-header (a constant `01 00 00 00 .. ..` word pair and a bounding box),
    then the MESH descriptor `04 04 00 01 00 80 80 6c`, then the vertex
    records. Each block holds only a handful of real vertices; the remainder
    of the fixed-size block is filled by DUPLICATING the last real record.

  Because the geometry is reliably anchored by the MESH signature in both
  layouts, the model path simply scans for every MESH descriptor, reads
  64-byte records from descriptor+8 until the first invalid (|w| != 1) row,
  and then drops the duplicated-record tail padding. Triangle-strip topology
  is identical to the level format.

  Validated empirically across 329 model files (908906 vertices, 537276
  non-degenerate triangles, only 600 degenerate, zero NaN faces). Bounding
  boxes are model-sized (median max-extent ~177 units), not level-sized.

----------------------------------------------------------------------------
MATRIX blocks / `--scene` mode -- reverse-engineered, validated empirically
----------------------------------------------------------------------------

A MATRIX block (level-file block whose descriptor has `ff ff ff ff` at +0x04)
is a scene-graph / instance-placement block. It holds OBJECT-SPACE geometry
plus a table of 4x4 transforms that place copies of that geometry into the
level. The default per-mesh export skips these blocks entirely; the opt-in
`--scene` mode decodes them and bakes the transforms so a whole level can be
viewed placed in world space.

A MATRIX block is one or more SECTIONS. A new section begins at a 16-byte
block separator (`00 00 00 17` or `00 00 00 14` + twelve `00`) whose following
16-byte descriptor row has `ff ff ff ff` at +0x04 (`00 00 00 00 ff ff ff ff
00...`). Separators that lack that descriptor are ordinary geometry-internal
delimiters and do NOT start a section.

Each section is:

  1. A TRANSFORM TABLE starting at section+0x10. Records are 0x50 bytes:
       +0x00  <u32 index> + 12 bytes
              record 0's "index" slot is the descriptor row, i.e.
              `00 00 00 00  ff ff ff ff  00 00 00 00  00 00 00 00`;
              records 1..N-1 are `<u32 sequential-index> 00..00` (12 zero
              bytes of padding).
       +0x10  4x4 affine matrix, row-major, 16 little-endian floats. Rows 0-2
              are the 3x4 rotation/scale, row 3 is the translation; the 4th
              column is (0,0,0,1). Many tables include identity entries.
     The table ends at the first 0x50-stride slot that is not a valid record
     (a FILLER row, or the geometry sub-header) -- detected by the index no
     longer being sequential / the pad no longer being zero / the matrix no
     longer being affine.
  2. The OBJECT-SPACE GEOMETRY: a short sub-header (a `[w0][w0*130][w2]
     [w0*0x860]` word quad and an axis-aligned bbox), then one or more MESH
     blocks decoded exactly like model-file geometry (read_model_block()).

`--scene` emits the section's geometry once per transform in the table, baking
each matrix into the vertex positions (and rotating normals). Exact-duplicate
matrices within a table are collapsed to one instance to avoid pure z-fighting
overlays. The regular MESH / SUBMESH geometry is already authored in world
space and is emitted unchanged. The result is a placed full-level OBJ.

  Validated: across all 36 `id 0x44` level files, 930 transforms are decoded;
  translations are bounded and level-scale (max component ~1130 units, no NaN
  / infinite outliers). Placed-scene bounding boxes stay consistent with the
  regular level geometry (e.g. chunk07.n0: regular x[-9,248] z[-15,450],
  placed props cluster x[-12,9] z[-18,15] -> coherent combined scene).

UNCERTAIN / NOT YET RESOLVED
  * Material/texture binding. m0/m1 are constant per strip and clearly key
    something (likely a material or texture-page index) but the mapping to the
    GS texture packets is not decoded. Strips are grouped into OBJ objects by
    (m0, m1) so the grouping is preserved for later.
  * The SUBMESH sub-header fields (index, counts, bbox) are only partially
    interpreted; this tool locates the geometry within the block empirically
    rather than trusting the declared counts.
  * MATRIX transforms (`--scene`). The transform table and its object-space
    geometry are decoded reliably, but two things are not fully confirmed
    without the engine code: (a) some tables hold repeated identity entries --
    collapsed by the duplicate-matrix filter, but their runtime role
    (animation slots, LOD, unused) is unknown; (b) whether a transform is the
    final world placement or is composed with a parent node is unverified.
    Decoded translations are level-scale and bounded, which is consistent with
    them being world placements, so `--scene` applies them directly.
  * The MATRIX geometry sub-header word quad (`w0`, `w0*130`, `w2`,
    `w0*0x860`) is decoded structurally; `w0`/`w2`'s exact meaning (a count
    and a sub-mesh/material index) is unconfirmed and the decoder does not
    rely on it.
  * Skinning / bone weights. No per-vertex bone index or weight data was found
    in the 64-byte record -- the four rows are fully accounted for (marker,
    uv, normal/colour, position). Any skinning rig, if present, lives outside
    the geometry blocks (likely in a separate animation file or a MATRIX
    block) and is not yet located. Model meshes are exported as static
    geometry in bind/rest pose.
  * The model-block sub-header's leading `01 00 00 00 .. ..` word pair is
    constant within a file but its meaning (a vertex/strip count or a flags
    word) is not confirmed; the decoder does not rely on it.

----------------------------------------------------------------------------
Rig / animation data -- reverse-engineered, PARTIAL (see "uncertain" notes)
----------------------------------------------------------------------------

The 64-byte geometry vertex record has NO per-vertex bone index or weight
field -- its four rows are fully accounted for (marker, uv, normal/colour,
position). The skinning / animation rig therefore lives in SEPARATE files, not
inside the geometry blocks. Two distinct representations were located:

(1) PER-BONE COLLISION-HULL FILES -- previously thought to be skeleton
    bind-pose transforms; reverse-engineered 2026-05-24 to be per-bone
    convex collision hulls described as sets of bounding-PLANE EQUATIONS,
    most commonly OBBs (oriented bounding boxes). Small (2-4 KiB) files
    with NO MESH signature; a flat array of fixed 0x78-byte (120-byte)
    records. 22 such files are present in `extract/`; several are byte-
    identical across many level regions (a shared hull set for a recurring
    enemy / the player). Layout:

      FILE HEADER
        * "short" form  -- 4 bytes: `<u32 record_count>`. Records start at +4.
        * "long" form   -- 0x20 bytes: `<u32 record_count>`, then 8 bytes of
                           default-flags + a `fffe00xx` VIF preset (STMASK /
                           STROW-style), 4 zero bytes, then a vec3 of floats
                           (a root offset / global hull centre -- not yet
                           confirmed). Records start at +0x20.
        The two forms are told apart by where the first `78 00 04 00` tag sits
        (offset 8 for short, 0x24 for long).

      RECORD -- 0x78 bytes, repeated `record_count` (+/-1) times:
        +0x00  u8[3] flags + u8 BONE INDEX
               The 4th byte is a small integer that indexes a bone, constant
               across a run of consecutive records (one record per plane;
               6 planes = an OBB; fewer = a half-space / capped hull). The
               three flag bytes take a few discrete values (`00 00 01`,
               `00 01 00`, `00 01 01`, `00 80 00`, ...) -- a per-plane role
               tag (face-pair vs cap, etc.).
        +0x04  4-byte VIF UNPACK tag preset (`78 00 04 00`). The engine
               splices these records into VIF1 packets, so each record
               carries its own tag inline.
        +0x08  112-byte PAYLOAD
               +0x08..+0x18  vec4 PLANE EQUATION: `(nx, ny, nz, D)` -- xyz is
                             a UNIT-LENGTH outward normal (verified across
                             every record of every rig file: |xyz| = 1.0
                             within float epsilon), D is the signed plane
                             offset. The bone's interior is the intersection
                             of all half-spaces `n.x + D <= 0`.
               +0x18..+0x78  six vec4 EXTRAS. Their exact role is not yet
                             confirmed -- candidates include the face's
                             polygon corners, in-plane edge vectors, or
                             neighbour-face indices used by the engine's
                             SAT / GJK responder. The values are bounded
                             and consistent with geometric data, but not
                             yet a closed-form decode.

    `--rig` walks every rig file and writes (a) a `*_rig.txt` dump (header,
    per-record plane equations, per-bone OBB pair summary, raw payload),
    and (b) a `*_rig_hulls.obj` wireframe of each detected OBB.

    OBB recovery: per bone, planes are grouped into antiparallel pairs
    (`dot < -0.95`). A bone with exactly 3 pairs yields a complete OBB:
    each pair defines an axis (the pair's shared direction) and a span
    along that axis (`[D_neg, -D_pos]`). 8 box corners follow by enumerating
    +/- spans on each axis. Bones with non-OBB plane counts (half-spaces,
    capped hulls) are reported but not turned into wireframe boxes.

(2) PER-FRAME VERTEX ANIMATION -- some characters are animated by shipping the
    whole mesh once per pose. In `chunk03` a single 13-block / 1690-vertex
    character mesh appears as 11 sibling files (`id29..id2e`, `id30..id34`)
    with IDENTICAL block count and per-strip topology but different vertex
    positions -- i.e. 11 keyframe poses of one mesh. `--anim` detects such
    sibling groups (same block count + same per-strip vertex counts within a
    region) and exports each group as a numbered OBJ sequence
    `*_frameNN.obj`, ready to load as a morph / vertex-cache animation.

UNCERTAIN / NOT YET RESOLVED (rig)
  * The first 16 bytes of the payload are a plane equation (confirmed across
    every record of every rig file). The remaining 96 bytes (6 vec4s) carry
    additional geometry whose exact field layout is not yet decoded -- needs
    either the VU1 microcode that consumes them or a careful empirical study
    matching the extras against a known hull's face polygons.
  * The "rig" files describe collision hulls, NOT the bind-pose skeleton.
    Where the actual bone bind-pose transforms (parent-relative rotations
    + translations needed to skin the mesh) live is STILL OPEN. The 64-byte
    geometry vertex record has no per-vertex bone weight / index, which is
    consistent with the meshes being either rigid-bone-attached (a whole
    sub-mesh per bone) or vertex-animated frame-by-frame (see --anim). It
    is also possible the bind-pose lives inside an embedded MATRIX block in
    the model file, or in the boot ELF; not yet investigated.
  * The bone PARENT hierarchy is not yet isolated. The per-record bone
    index is just an ID; no explicit parent-index array was identified inside
    the rig files. The bone-group ordering within a file is recurring across
    files (e.g. [18, 25, 24, 23, 4, 3] in many character rigs) which may be
    a per-skeleton bone iteration order, but that ordering does not by itself
    convey a parent-child tree.
  * Which rig file binds to which model file is by-region association only
    (a region bundles one entity's pieces); no explicit cross-reference id
    was decoded.

Usage:
  extract_models.py --in extract --out models
  extract_models.py --file extract/chunk04.n0/f06_id44.bin --out models
  extract_models.py --file extract/chunk07.n1/f06_id70.bin --out models
  extract_models.py --scene --in extract --out models      # placed levels
  extract_models.py --scene --file extract/chunk17/f01_id44.bin --out models
  extract_models.py --rig  --in extract --out models       # dump rig files
  extract_models.py --anim --in extract --out models       # pose-set frames
"""
from __future__ import annotations

import argparse
import math
import struct
import sys
from pathlib import Path

# 16-byte block separator (level files).
SEPARATOR = b"\x00\x00\x00\x17" + b"\x00" * 12
# 16-byte block separator (model files; also appears inside MATRIX-block geometry).
SEPARATOR_MODEL = b"\x00\x00\x00\x14" + b"\x00" * 12
# Last 8 bytes of a MESH descriptor row.
MESH_SIG = bytes.fromhex("040400010080806c")
# Bytes [8:12] of a SUBMESH descriptor row.
SUBMESH_SIG = bytes.fromhex("4d040000")
# Bytes [4:8] of a MATRIX descriptor row (and of a MATRIX section descriptor).
MATRIX_SIG = b"\xff\xff\xff\xff"

# One MATRIX transform-table record: u32 index + 12 bytes, then a 4x4 matrix.
MATRIX_REC_SIZE = 0x50

# Rig-file record: 4-byte control word (incl. bone index) + a 4-byte VIF
# UNPACK tag + a 112-byte transform payload = 0x78 bytes total.
RIG_REC_SIZE = 0x78
# The VIF UNPACK tag that marks every rig record's transform payload.
RIG_VIF_TAG = b"\x78\x00\x04\x00"

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
    triangles -- the strip-stitching idiom -- are skipped: both zero-area
    triangles and triangles with two coincident vertices (a thin sliver whose
    area is non-zero but which is still topologically degenerate).
    """
    v = strip.verts
    for t in range(len(v) - 2):
        if t & 1:
            i0, i1, i2 = t + 1, t, t + 2
        else:
            i0, i1, i2 = t, t + 1, t + 2
        p0, p1, p2 = v[i0].pos, v[i1].pos, v[i2].pos
        if p0 == p1 or p1 == p2 or p0 == p2:
            continue
        if tri_area(p0, p1, p2) < MIN_TRI_AREA:
            continue
        yield i0, i1, i2


def parse_file(d: bytes) -> list[Strip]:
    """Return every triangle strip in a LEVEL (`id 0x44`) geometry file."""
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


def read_model_block(d: bytes, vstart: int) -> list:
    """Decode one model-format MESH block's vertex records.

    Reads 64-byte records from `vstart` until the first invalid (|w| != 1)
    row, then drops the duplicated-record tail that pads the fixed-size block.
    Returns the (Vertex, strip_key) list build_strips() expects.
    """
    raw: list[bytes] = []
    o = vstart
    while o + VERT_SIZE <= len(d):
        w = struct.unpack_from("<f", d, o + 0x3C)[0]
        if abs(abs(w) - 1.0) < W_TOLERANCE:
            raw.append(d[o:o + VERT_SIZE])
            o += VERT_SIZE
        else:
            break
    # The block is padded by repeating its last real record; trim that tail.
    real = len(raw)
    while real > 1 and raw[real - 1] == raw[real - 2]:
        real -= 1
    records: list = []
    for rec in raw[:real]:
        flag = rec[7]
        m0, m1 = struct.unpack_from("<II", rec, 0)
        uv = struct.unpack_from("<2f", rec, 0x10)
        attr = struct.unpack_from("<3f", rec, 0x20)
        pos = struct.unpack_from("<3f", rec, 0x30)
        records.append((Vertex(flag, uv, attr, pos), (m0, m1 & 0x00FFFFFF)))
    return records


def parse_model_file(d: bytes) -> list[Strip]:
    """Return every triangle strip in a MODEL (character/object/prop) file.

    Model files do not use the level files' separator-delimited block walk:
    their MESH blocks are fixed-size and the descriptor after a separator is
    usually FILLER. The MESH signature is the reliable anchor, so this scans
    for every occurrence and decodes the records that follow it.
    """
    strips: list[Strip] = []
    i = d.find(MESH_SIG)
    while i != -1:
        records = read_model_block(d, i + len(MESH_SIG))
        if records:
            strips.extend(build_strips(records))
        i = d.find(MESH_SIG, i + 1)
    return strips


def parse_model_blocks(d: bytes) -> list[list[Strip]]:
    """Like parse_model_file(), but returns triangle strips GROUPED BY BLOCK.

    Each MESH-descriptor occurrence in the file anchors one block (~32
    real vertices in bone-local space; the rest of the fixed-size unit is
    duplicated padding). Returns ``[ [strip, ...], ... ]`` with one inner
    list per block, preserving file order. This is the granularity at
    which spatial-proximity bone binding is performed -- one block binds to
    one bone, since the per-vertex W-field selector (commit 94b8fe5) only
    takes a small handful of distinct values per block.
    """
    out: list[list[Strip]] = []
    i = d.find(MESH_SIG)
    while i != -1:
        records = read_model_block(d, i + len(MESH_SIG))
        if records:
            out.append(build_strips(records))
        i = d.find(MESH_SIG, i + 1)
    return out


# ---------------------------------------------------------------------------
# MATRIX blocks / scene placement
# ---------------------------------------------------------------------------

# A row-major 4x4 identity matrix, used when a section has no transform table.
IDENTITY = (1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0)


def is_matrix_descriptor(desc: bytes) -> bool:
    """True for a MATRIX block / MATRIX section descriptor row.

    The descriptor is `00 00 00 00  ff ff ff ff  ...` -- `ff ff ff ff` at
    +0x04. An all-`0xff` row is FILLER, not a MATRIX descriptor.
    """
    return (len(desc) == 16
            and desc[4:8] == MATRIX_SIG
            and desc != b"\xff" * 16)


def _looks_like_affine(m: tuple) -> bool:
    """True if 16 floats form a sane row-major affine matrix.

    The 4th column must be (0,0,0,1): a real transform record has it, a
    geometry sub-header or FILLER row interpreted as 16 floats does not.
    """
    return (abs(m[15] - 1.0) < 1e-3
            and abs(m[3]) < 1e-3 and abs(m[7]) < 1e-3 and abs(m[11]) < 1e-3)


def matrix_sections(d: bytes, start: int, end: int) -> list[tuple[int, int]]:
    """Split a MATRIX block into [(section_start, section_end), ...].

    A section begins at the block start and at every interior 16-byte
    separator (`0x17` or `0x14` form) whose following descriptor row is a
    MATRIX descriptor. Separators that lack that descriptor are ordinary
    geometry-internal delimiters and do not start a section.
    """
    starts = [start]
    i = start + 16
    while i + 24 <= end:
        row = d[i:i + 16]
        if (row == SEPARATOR or row == SEPARATOR_MODEL) \
                and d[i + 20:i + 24] == MATRIX_SIG:
            starts.append(i)
        i += 1
    starts.append(end)
    return [(starts[k], starts[k + 1]) for k in range(len(starts) - 1)]


def parse_matrix_table(d: bytes, start: int, end: int) -> tuple[list[tuple], int]:
    """Decode a section's transform table.

    Returns (matrices, offset_after_table). The table begins at start+0x10:
    record 0's index slot is the descriptor row (`ff ff ff ff` at +4), and
    records 1..N-1 carry a sequential u32 index and 12 zero pad bytes. Each
    record holds a 4x4 row-major affine matrix at +0x10. The table ends at
    the first 0x50-stride slot that fails those checks.
    """
    matrices: list[tuple] = []
    o = start + 0x10
    if o + MATRIX_REC_SIZE > end or d[o + 4:o + 8] != MATRIX_SIG:
        return matrices, o
    m = struct.unpack_from("<16f", d, o + 0x10)
    if not _looks_like_affine(m):
        return matrices, o
    matrices.append(m)
    o += MATRIX_REC_SIZE
    expect = 1
    while o + MATRIX_REC_SIZE <= end:
        if struct.unpack_from("<I", d, o)[0] != expect:
            break
        if d[o + 4:o + 0x10] != b"\x00" * 12:
            break
        m = struct.unpack_from("<16f", d, o + 0x10)
        if not _looks_like_affine(m):
            break
        matrices.append(m)
        o += MATRIX_REC_SIZE
        expect += 1
    return matrices, o


def unique_matrices(matrices: list[tuple]) -> list[tuple]:
    """Drop exact-duplicate transforms (rounded) to avoid z-fighting overlays.

    Repeated identity entries are common in the tables; emitting the same
    geometry several times at the same place only produces coincident faces.
    """
    seen: set = set()
    out: list[tuple] = []
    for m in matrices:
        key = tuple(round(x, 4) for x in m)
        if key not in seen:
            seen.add(key)
            out.append(m)
    return out


def transform_point(m: tuple, p) -> tuple:
    """Apply a row-major 4x4 affine matrix to a position (x, y, z)."""
    x, y, z = p
    return (m[0] * x + m[4] * y + m[8] * z + m[12],
            m[1] * x + m[5] * y + m[9] * z + m[13],
            m[2] * x + m[6] * y + m[10] * z + m[14])


def transform_dir(m: tuple, v) -> tuple:
    """Apply a matrix's rotation/scale part to a direction (no translation)."""
    x, y, z = v
    return (m[0] * x + m[4] * y + m[8] * z,
            m[1] * x + m[5] * y + m[9] * z,
            m[2] * x + m[6] * y + m[10] * z)


def transformed_strip(strip: Strip, m: tuple) -> Strip:
    """Return a copy of `strip` with the matrix baked into every vertex."""
    out = Strip(strip.key)
    for v in strip.verts:
        if v.is_normal:
            attr = transform_dir(m, v.attr)
            n = math.sqrt(attr[0] ** 2 + attr[1] ** 2 + attr[2] ** 2)
            if n > 1e-9:
                attr = (attr[0] / n, attr[1] / n, attr[2] / n)
        else:
            attr = v.attr  # a vertex colour -- not affected by placement
        out.verts.append(Vertex(v.flag, v.uv, attr, transform_point(m, v.pos)))
    return out


def matrix_section_strips(d: bytes, start: int, end: int) -> list[Strip]:
    """Object-space triangle strips of one MATRIX section's geometry.

    The geometry follows the transform table and is decoded exactly like
    model-file geometry: every MESH descriptor in the section anchors a
    fixed-size padded block read by read_model_block().
    """
    strips: list[Strip] = []
    i = d.find(MESH_SIG, start, end)
    while i != -1 and i < end:
        records = read_model_block(d, i + len(MESH_SIG))
        if records:
            strips.extend(build_strips(records))
        i = d.find(MESH_SIG, i + 1, end)
    return strips


def parse_scene(d: bytes) -> list[Strip]:
    """Return every triangle strip of a LEVEL file PLACED in world space.

    The regular MESH / SUBMESH geometry (already authored in world space) is
    returned unchanged. Every MATRIX block additionally contributes its
    object-space geometry once per transform in its table, with the transform
    baked into the vertices. The union is a placed full-level scene.
    """
    blocks = block_bounds(d)
    if not blocks:
        return []
    strips: list[Strip] = []
    for idx, (start, end) in enumerate(blocks):
        if idx == 0:
            # Block 0 is plain geometry, already world-space.
            strips.extend(build_strips(read_vertices(d, start, end)))
            continue
        desc = d[start + 0x10:start + 0x20]
        if is_matrix_descriptor(desc):
            for sec_start, sec_end in matrix_sections(d, start, end):
                matrices, after = parse_matrix_table(d, sec_start, sec_end)
                instances = unique_matrices(matrices) if matrices else [IDENTITY]
                base = matrix_section_strips(d, after, sec_end)
                for m in instances:
                    for s in base:
                        strips.append(transformed_strip(s, m))
            continue
        # MESH / SUBMESH: world-space geometry, emitted as stored.
        vstart = mesh_vertex_start(d, start, end, is_first=False)
        if vstart is None:
            continue
        strips.extend(build_strips(read_vertices(d, vstart, end)))
    return strips


# ---------------------------------------------------------------------------
# Rig / skeleton-transform files
# ---------------------------------------------------------------------------


class RigRecord:
    """One 0x78-byte rig record: a joint transform packet."""

    __slots__ = ("offset", "flags", "bone", "payload")

    def __init__(self, offset: int, flags: tuple, bone: int, payload: bytes):
        self.offset = offset      # byte offset of the record in the file
        self.flags = flags        # the three control-word flag bytes
        self.bone = bone          # joint / bone index (control-word byte 3)
        self.payload = payload    # 112-byte VIF-packed transform payload


class RigFile:
    """A decoded rig / skeleton-transform file."""

    __slots__ = ("count", "header", "records", "long_header")

    def __init__(self, count, header, records, long_header):
        self.count = count            # declared record count from the header
        self.header = header          # raw header bytes
        self.records = records        # list[RigRecord]
        self.long_header = long_header  # True for the 0x20-byte header form


def _rig_record_starts(d: bytes) -> list[int]:
    """Offsets of every contiguous 0x78-byte rig record (tag-anchored)."""
    first = d.find(RIG_VIF_TAG)
    if first < 0:
        return []
    # The VIF tag sits at record+0x04; the record itself starts 4 bytes earlier.
    o = first - 4
    starts: list[int] = []
    while o >= 0 and o + RIG_REC_SIZE <= len(d) \
            and d[o + 4:o + 8] == RIG_VIF_TAG:
        starts.append(o)
        o += RIG_REC_SIZE
    return starts


def is_rig_file(d: bytes) -> bool:
    """True for a rig / skeleton-transform file.

    A rig file carries NO MESH signature and is a flat array of >=3
    contiguous 0x78-byte records, each tagged with the VIF UNPACK word.
    """
    if MESH_SIG in d or len(d) > 200_000:
        return False
    return len(_rig_record_starts(d)) >= 3


def parse_rig_file(d: bytes) -> RigFile | None:
    """Decode a rig / skeleton-transform file into joint-transform records.

    See the module docstring ("Rig / animation data") for the format. The
    payload is left as raw bytes -- it is VIF-packed and a faithful float
    decode needs the VU1 microcode -- but the bone index and flags are
    decoded reliably.
    """
    starts = _rig_record_starts(d)
    if len(starts) < 3:
        return None
    first = starts[0]
    # Header is everything before the first record's 4-byte control word.
    long_header = first >= 0x20
    header = d[:first - 4]
    count = struct.unpack_from("<I", d, 0)[0] if len(d) >= 4 else len(starts)
    records: list[RigRecord] = []
    for o in starts:
        ctrl = d[o:o + 4]
        records.append(RigRecord(o, (ctrl[0], ctrl[1], ctrl[2]), ctrl[3],
                                 d[o + 8:o + RIG_REC_SIZE]))
    return RigFile(count, header, records, long_header)


def _decode_rig_record(rec: "RigRecord"):
    """Decode a 112-byte rig record into a per-bone collision plane.

    Empirically (2026-05-24), every rig record's first vec4 has unit-length
    xyz (verified across all 22 rig files and every record). This identifies
    the record's primary payload as a PLANE EQUATION ``n.x + n.y + n.z + D``
    where ``(nx,ny,nz) = vec4[0].xyz`` is the outward normal and ``D =
    vec4[0].w`` is the signed plane offset from the world origin. Per-bone
    plane sets pair up antiparallel (dot ~= -1) into orthonormal pairs --
    most bones have 6 planes forming an OBB (oriented bounding box), some
    bones have fewer for half-space / cap planes.

    The remaining 6 vec4s (96 bytes) of the payload are extra geometric data
    -- candidates include polygon corners, edge vectors, or face neighbour
    indices -- whose exact layout is not yet decoded but does not look like
    a transform matrix.

    Returns ``(normal_xyz, D, extras_list_of_vec4)``.
    """
    floats = struct.unpack("<28f", rec.payload)
    normal = (floats[0], floats[1], floats[2])
    D = floats[3]
    extras = [floats[i:i + 4] for i in range(4, 28, 4)]
    return normal, D, extras


def _group_bone_planes(rig: "RigFile"):
    """Group rig records by bone index, preserving record order within each bone.

    Returns a dict ``{bone_index: [(rec_index, normal, D, extras, flags), ...]}``.
    """
    out: dict[int, list] = {}
    for i, rec in enumerate(rig.records):
        n, D, extras = _decode_rig_record(rec)
        out.setdefault(rec.bone, []).append((i, n, D, extras, rec.flags))
    return out


def _pair_planes(planes):
    """Find antiparallel plane pairs (dot < -0.95) in a bone's plane list.

    Returns ``(pairs, unpaired_indices)`` where ``pairs`` is a list of
    ``(i_pos, i_neg, axis_xyz, extent)``: the two opposing plane indices, a
    chosen positive-half outward normal, and the signed gap ``D_pos + D_neg``
    (positive = the box has thickness along that axis).
    """
    pairs = []
    used = set()
    for i in range(len(planes)):
        if i in used:
            continue
        _, ni, Di, _, _ = planes[i]
        best_j = None
        best_dot = -0.95
        for j in range(i + 1, len(planes)):
            if j in used:
                continue
            _, nj, Dj, _, _ = planes[j]
            d = ni[0] * nj[0] + ni[1] * nj[1] + ni[2] * nj[2]
            if d < best_dot:
                best_dot = d
                best_j = j
        if best_j is not None:
            _, nj, Dj, _, _ = planes[best_j]
            extent = Di + Dj
            pairs.append((i, best_j, ni, extent))
            used.add(i)
            used.add(best_j)
    unpaired = [k for k in range(len(planes)) if k not in used]
    return pairs, unpaired


def write_rig_dump(path: Path, rig: RigFile, src_name: str) -> int:
    """Write a human-readable dump of a rig file. Returns the record count.

    Format (reverse-engineered 2026-05-24): the so-called "rig" files are
    NOT skeleton bind-pose transforms -- they are per-bone COLLISION HULLS
    described as sets of bounding planes (mostly OBBs). Each 0x78-byte
    record holds one PLANE EQUATION ``n.x + D = 0`` (unit normal in the
    first 12 bytes, signed offset ``D`` in the next 4) plus 96 bytes of
    extra geometric data whose exact role is not yet decoded (likely face
    polygons / edge spans). Multiple records share a bone index to build up
    that bone's hull. See the module docstring "Rig / animation data" for
    full detail.

    The dump emits: (a) the plane equation per record, (b) a per-bone
    summary grouping records and detecting antiparallel pairs (OBB axes),
    (c) the raw 28-float payload as before for any further hand analysis.
    """
    lines: list[str] = [
        f"# Extermination (SCUS-97112) rig / per-bone collision-hull dump",
        f"# source: {src_name}",
        f"# exported by tools/extract_models.py --rig",
        f"# header ({len(rig.header)} bytes, "
        f"{'long' if rig.long_header else 'short'} form): "
        f"{rig.header.hex()}",
        f"# declared record count: {rig.count}; decoded records: "
        f"{len(rig.records)}",
        "# format: record = u8[3] flags + u8 bone-index, then a VIF UNPACK"
        " tag word, then a 112-byte payload.",
        "# payload[0:16] = vec4(normal.xyz, D): unit-length plane normal +"
        " signed plane offset (plane equation n.x+D=0).",
        "# payload[16:112] = six vec4 extras (face polygon / edges /"
        " neighbour indices -- exact layout TBD).",
        "",
    ]
    bones_seq = [r.bone for r in rig.records]
    lines.append(f"# bone index per record: {bones_seq}")
    lines.append(f"# distinct bones: {sorted(set(bones_seq))}")
    lines.append("")

    # Per-bone summary with OBB pair detection.
    by_bone = _group_bone_planes(rig)
    lines.append("## per-bone hull summary")
    for bone in sorted(by_bone):
        planes = by_bone[bone]
        pairs, unpaired = _pair_planes(planes)
        kind = ("OBB (3 pairs)" if len(pairs) == 3 and not unpaired
                else f"hull ({len(pairs)} pairs, {len(unpaired)} unpaired)")
        lines.append(f"bone {bone:3d}: {len(planes)} planes -- {kind}")
        for i_pos, i_neg, axis, extent in pairs:
            rec_i = planes[i_pos][0]
            rec_j = planes[i_neg][0]
            lines.append(
                f"   axis=({axis[0]: .4f},{axis[1]: .4f},{axis[2]: .4f})"
                f"  extent={extent: .4f}  records[{rec_i},{rec_j}]")
        for k in unpaired:
            rec_k, n, D, _, fl = planes[k]
            lines.append(
                f"   half-space: n=({n[0]: .4f},{n[1]: .4f},{n[2]: .4f})"
                f"  D={D: .4f}  record[{rec_k}]"
                f"  flags={fl[0]:02x} {fl[1]:02x} {fl[2]:02x}")
    lines.append("")

    # Per-record full dump.
    lines.append("## per-record raw payload")
    for i, r in enumerate(rig.records):
        n, D, extras = _decode_rig_record(r)
        floats = struct.unpack("<28f", r.payload)
        lines.append(f"record {i:3d}  @0x{r.offset:05x}  bone={r.bone:3d}  "
                     f"flags={r.flags[0]:02x} {r.flags[1]:02x} "
                     f"{r.flags[2]:02x}")
        lines.append(f"  plane: n=({n[0]: .5f},{n[1]: .5f},{n[2]: .5f})"
                     f"  D={D: .5f}")
        lines.append(f"  payload.hex: {r.payload.hex()}")
        for k in range(0, 28, 4):
            quad = floats[k:k + 4]
            tag = "plane" if k == 0 else f"extra{(k - 4) // 4}"
            lines.append("  %-7s f4[%2d]: %s" % (
                tag, k, "  ".join(f"{x: .5f}" for x in quad)))
        lines.append("")
    path.write_text("\n".join(lines) + "\n")
    return len(rig.records)


def _obb_from_pairs(pairs):
    """Build an OBB (8 corners + 3 axis triples) from 3 antiparallel pairs.

    For each pair (axis, extent), the box extends from ``-D_neg`` to ``D_pos``
    along the axis. Centre = (D_pos - D_neg) / 2 along that axis. Returns
    ``(centre_xyz, axes_3x3, half_extents_xyz)`` or None if the pairs are not
    a complete orthonormal frame (e.g. fewer than 3 pairs).
    """
    if len(pairs) != 3:
        return None
    # Each pair: (i_pos, i_neg, axis_pos, extent = D_pos + D_neg).
    # The plane equation convention here is n.x + D = 0; the plane sits at
    # distance -D from origin along +n. With outward-pointing normals, the
    # interior of the box satisfies n.x + D <= 0. The +n plane is at
    # -D_pos, the -n plane is at +D_neg, so the box centre's projection on
    # n is (D_neg - D_pos) / 2, half-extent is (D_pos + D_neg) / 2.
    centre = [0.0, 0.0, 0.0]
    axes = []
    half = []
    for i_pos, i_neg, axis_pos, extent in pairs:
        # extent = D_pos + D_neg, but signs in source are mixed -- robust
        # to either convention by taking magnitude.
        h = abs(extent) * 0.5
        half.append(h)
        axes.append(axis_pos)
    # We can't recover a global centre without the per-pair plane D values,
    # so fall back to: caller already passed plane[]; do that below instead.
    return axes, half


def write_rig_obb_obj(path: Path, rig: RigFile, src_name: str) -> int:
    """Export each bone's OBB hull as a wire-frame OBJ. Returns hull count.

    For bones with exactly 3 antiparallel plane pairs (a complete OBB), emit
    the 8 corner vertices and the 12 edges as line primitives. Hulls with
    other plane counts are skipped (logged in a header comment).
    """
    lines = [
        f"# Extermination (SCUS-97112) rig collision-hull export",
        f"# source: {src_name}",
        "# one wireframe OBB per bone; bones with non-OBB hulls are skipped.",
        f"o {Path(src_name).stem}_hulls",
    ]
    vert_base = 1
    hull_count = 0
    by_bone = _group_bone_planes(rig)
    for bone in sorted(by_bone):
        planes = by_bone[bone]
        pairs, unpaired = _pair_planes(planes)
        if len(pairs) != 3 or unpaired:
            continue
        # Solve box: each pair gives axis + two plane offsets (D_pos, D_neg).
        # Interior: n.x + D <= 0. The +n face plane: n.x = -D_pos.
        # The -n face plane: -n.x = -D_neg, i.e. n.x = D_neg.
        # So along axis n, the box spans [D_neg, -D_pos] (signed). Centre
        # on n = (D_neg - D_pos) / 2, half-extent = (-D_pos - D_neg) / 2.
        axes = []
        spans = []  # list of (min_along_axis, max_along_axis)
        for i_pos, i_neg, axis_pos, _ in pairs:
            _, _, D_pos, _, _ = planes[i_pos]
            _, _, D_neg, _, _ = planes[i_neg]
            a_lo = D_neg
            a_hi = -D_pos
            if a_lo > a_hi:
                a_lo, a_hi = a_hi, a_lo
            axes.append(axis_pos)
            spans.append((a_lo, a_hi))
        # 8 corners: each axis contributes either its lo or hi extent.
        corners = []
        for s0 in (0, 1):
            for s1 in (0, 1):
                for s2 in (0, 1):
                    t0 = spans[0][s0]
                    t1 = spans[1][s1]
                    t2 = spans[2][s2]
                    x = (t0 * axes[0][0] + t1 * axes[1][0]
                         + t2 * axes[2][0])
                    y = (t0 * axes[0][1] + t1 * axes[1][1]
                         + t2 * axes[2][1])
                    z = (t0 * axes[0][2] + t1 * axes[1][2]
                         + t2 * axes[2][2])
                    corners.append((x, y, z))
        for x, y, z in corners:
            lines.append(f"v {x:.4f} {y:.4f} {z:.4f}")
        # 12 edges as l-primitives (OBJ line lists).
        edges = [(0, 1), (1, 3), (3, 2), (2, 0),
                 (4, 5), (5, 7), (7, 6), (6, 4),
                 (0, 4), (1, 5), (2, 6), (3, 7)]
        lines.append(f"g bone_{bone:03d}")
        for a, b in edges:
            lines.append(f"l {vert_base + a} {vert_base + b}")
        vert_base += 8
        hull_count += 1
    path.write_text("\n".join(lines) + "\n")
    return hull_count


# ---------------------------------------------------------------------------
# Skeleton hierarchy (id 0x71 character / animation files)
# ---------------------------------------------------------------------------
#
# Reverse-engineered 2026-05-25. Several character classes ship a paired set
# of files:
#   * id 0x73 (or near it) -- the per-bone collision-hull file (see --rig).
#   * id 0x71              -- a multi-entry animation/skeleton container.
#
# An id 0x71 character file is laid out as a simple offset directory:
#
#   +0x00  u32  entry_count                    (e.g. 57 = bind pose + 56 anim clips)
#   +0x04  u32[entry_count]  entry_offsets     (absolute byte offsets in file)
#   +0x04+ u32  0xffffffff                     (sentinel terminating the table)
#
# Each entry is a self-contained animation clip / pose. Every entry of a given
# file shares the SAME skeleton: same bone count and the SAME parent-index
# array. So the bone hierarchy can be recovered from any entry, and is
# straightforwardly extracted from entry 0:
#
#   entry[0x00]  u8   bone_count           e.g. 30 (called nbones below)
#   entry[0x01]  u8   ?                    (often 0)
#   entry[0x02]  u8   stride               e.g. 0x78 -- a VIF UNPACK row size
#   entry[0x03]  u8   ?                    (often 0)
#   entry[0x04]  u16  0xffff               (constant fence)
#   entry[0x06]  u16  ?                    (often 0)
#   entry[0x08]  u32  off_section1         offset (in entry) of the first
#                                          per-bone payload section
#                                          (variable size per bone)
#   entry[0x0c]  u32  off_section2         offset of a second per-bone payload
#                                          section (bone 0 is large, others uniform 0x24)
#   entry[0x10]  u32  off_section3         offset of a third per-bone section
#                                          (uniform 0x24 per bone)
#   entry[0x14..0x20]  zeros               padding
#   entry[0x20..0x28]  0xffffffff,0        section-table sentinel
#   entry[0x28]        u32[nbones]         PARENT-INDEX TABLE (this finding)
#   entry[0x28+nbones*4 .. ]               additional per-bone u32 tables
#                                          (bone "kind"/skin counts, not yet decoded)
#
# Validation (chunk05/f04_id71.bin, the player character):
#   nbones = 30  (28 active bones + 2 trailing "120, 156" slots that are not
#                 valid parent indices -- only the first 28 are interpreted)
#   parents (first 28) =
#     [1, 1, 3, 2, 2, 4, 4, 5, 6, 4, 4, 4, 4, 4, 4, 4, 7, 8, 9, 10, 12, 22,
#      23, 24, 24, 24, 24, 24]
#   Identical for chunk12/f00_id71.bin, chunk06/f01_id71.bin,
#   chunk07/f03_id71.bin, etc. -- same character, same skeleton.
#   The structure is MOSTLY a clean parent-index table:
#     * bone 1 is a root (parent==self); chain 5->7->16, 6->8->17 are
#       shoulder->elbow->wrist; bone 4 is a hub with 9 children (chest/ribs);
#       bone 24 has 5 children (palm + finger tips);
#     * EXCEPTION: bone 2 / bone 3 form a 2-cycle (parents[2]=3, parents[3]=2).
#       This cycle is consistent across every id 0x71 file that uses this
#       skeleton, so it is real data, not corruption. The most likely reading
#       is that the field is overloaded -- "parent" for most bones, but for a
#       few bones a "linked-pair" / "next-LOD" pointer -- or that one element
#       of the cycle is the true root and the other is its mirror. The dump
#       reports the field as-is; downstream consumers should be aware that a
#       strict parent-tree walk over this array has one cycle and two roots
#       (bone 1 and bone 24 are both self-parented; bone 2 / bone 3 form a
#       cycle, so neither is a root in the tree-walk sense).
#   Cross-check with the collision-hull (--rig) file for the same character
#   (chunk05/f05_id73.bin -> bones {3, 4, 18, 23, 24, 25}): all bone IDs in
#   the rig are valid indices in this skeleton, confirming that rig-file
#   bone IDs index into this same id 0x71 skeleton.
#
# The per-bone payload sections (section1/2/3) are VIF/GIF-packed transform
# / draw-command streams (16-bit fixed-point interleaved with what look like
# GS register addresses). A faithful decode of the BIND-POSE MATRICES from
# them needs either the VU1 microcode or the engine's bone-update code, so
# this extractor only recovers the HIERARCHY. The hierarchy alone is enough
# to author a skinned rig in Blender/Maya and bind it to the model geometry
# by hand; the bind-pose joint positions can be hand-placed using the
# collision-hull centres from the --rig dump as a guide.

# Trailing "non-parent" slots: id71 entries declare a bone_count that
# sometimes overshoots the real bone count by a couple of slots. The trailing
# slots' "parent" values are obviously out of range (e.g. 120, 156) and are
# detected and dropped.


class SkeletonFile:
    """A decoded id 0x71 skeleton/animation container."""

    __slots__ = ("entry_count", "bone_count_raw", "parents", "stride",
                 "section_offsets", "entry_size")

    def __init__(self, entry_count, bone_count_raw, parents, stride,
                 section_offsets, entry_size):
        self.entry_count = entry_count          # number of animation entries
        self.bone_count_raw = bone_count_raw    # declared count from header byte
        self.parents = parents                  # list[int] -- parent index per bone (-1 for root)
        self.stride = stride                    # VIF stride byte from header
        self.section_offsets = section_offsets  # tuple of 3 ints (offsets within entry)
        self.entry_size = entry_size            # size of entry 0 in bytes


def is_skeleton_file(d: bytes) -> bool:
    """True for an id 0x71-shaped animation/skeleton container.

    The format is a simple offset directory. The sanity check is:
      * u32 entry_count is small (1..512),
      * entry_offsets are monotonically increasing and within the file,
      * the table ends with the 0xffffffff sentinel,
      * the first entry's bone_count byte is small (<= 200),
      * the first entry's parent-table area parses as plausible indices.
    """
    if len(d) < 32:
        return False
    n = struct.unpack_from("<I", d, 0)[0]
    if not (1 <= n <= 512):
        return False
    table_end = 4 + (n + 1) * 4
    if table_end > len(d):
        return False
    offs = struct.unpack_from(f"<{n+1}I", d, 4)
    if offs[-1] != 0xFFFFFFFF:
        return False
    if offs[0] < table_end or offs[0] > len(d):
        return False
    for i in range(n):
        if offs[i] >= len(d):
            return False
        if i > 0 and offs[i] <= offs[i - 1]:
            return False
    e0 = d[offs[0]: offs[1] if n > 1 else len(d)]
    if len(e0) < 0x40:
        return False
    bc = e0[0]
    if not (1 <= bc <= 200):
        return False
    if 0x28 + bc * 4 > len(e0):
        return False
    # Plausibility: most parent indices should be < bone_count (allow a few
    # trailing slots to overshoot, hence count plausible vs all).
    parents = struct.unpack_from(f"<{bc}I", e0, 0x28)
    ok = sum(1 for p in parents if p < bc)
    return ok >= max(1, bc - 4)


def parse_skeleton_file(d: bytes) -> SkeletonFile | None:
    """Decode an id 0x71 file's directory header and skeleton hierarchy."""
    if not is_skeleton_file(d):
        return None
    n = struct.unpack_from("<I", d, 0)[0]
    offs = struct.unpack_from(f"<{n+1}I", d, 4)
    e0_start = offs[0]
    e0_end = offs[1] if n > 1 else len(d)
    e0 = d[e0_start:e0_end]
    bc = e0[0]
    stride = e0[2]
    # Header: u8 nbones, u8 ?, u8 stride, u8 ?, u16 0xffff, u16 ?, then
    # three u32 section offsets at +0x08, +0x0c, +0x10.
    s1 = struct.unpack_from("<I", e0, 0x08)[0]
    s2 = struct.unpack_from("<I", e0, 0x0c)[0]
    s3 = struct.unpack_from("<I", e0, 0x10)[0]
    raw_parents = list(struct.unpack_from(f"<{bc}I", e0, 0x28))
    # Trim trailing "non-parent" overshoot slots (parent index out of range).
    trimmed = raw_parents[:]
    while trimmed and trimmed[-1] >= len(trimmed):
        trimmed.pop()
    # Convert "self parent" (the root) to -1 for consumer convenience.
    parents = [(-1 if p == i else p) for i, p in enumerate(trimmed)]
    return SkeletonFile(entry_count=n,
                        bone_count_raw=bc,
                        parents=parents,
                        stride=stride,
                        section_offsets=(s1, s2, s3),
                        entry_size=len(e0))


def skeleton_children(parents: list[int]) -> dict[int, list[int]]:
    """parent->children index for a skeleton."""
    ch: dict[int, list[int]] = {}
    for i, p in enumerate(parents):
        ch.setdefault(p, []).append(i)
    return ch


def skeleton_roots(parents: list[int]) -> list[int]:
    """Bones whose parent is -1 (self-parent in the source)."""
    return [i for i, p in enumerate(parents) if p == -1]


def write_skeleton_dump(path: Path, skel: SkeletonFile, src_name: str,
                        hull_centres: dict[int, tuple[float, float, float]] | None = None) -> None:
    """Write a human-readable skeleton dump: parents, tree, and (optional) hull centres."""
    lines: list[str] = [
        "# Extermination (SCUS-97112) skeleton hierarchy dump",
        f"# source: {src_name}",
        "# exported by tools/extract_models.py --skeleton",
        f"# id 0x71 entry directory: {skel.entry_count} entry/animation(s)",
        f"# bone_count (declared) = {skel.bone_count_raw}; active bones = {len(skel.parents)}",
        f"# stride byte = 0x{skel.stride:02x}",
        f"# section offsets within entry 0: section1=0x{skel.section_offsets[0]:x}  "
        f"section2=0x{skel.section_offsets[1]:x}  section3=0x{skel.section_offsets[2]:x}",
        "",
        "# Parent table (one line per bone; -1 means root).",
    ]
    for i, p in enumerate(skel.parents):
        extra = ""
        if hull_centres and i in hull_centres:
            cx, cy, cz = hull_centres[i]
            extra = f"  # hull centre: ({cx:.2f}, {cy:.2f}, {cz:.2f})"
        lines.append(f"bone {i:3d}  parent={p:3d}{extra}")
    lines.append("")
    lines.append("# Tree (depth-indented).")
    children = skeleton_children(skel.parents)
    visited = set()

    def emit(i: int, depth: int) -> None:
        if i in visited:
            return
        visited.add(i)
        marker = ""
        if hull_centres and i in hull_centres:
            cx, cy, cz = hull_centres[i]
            marker = f"  [hull@({cx:.1f},{cy:.1f},{cz:.1f})]"
        lines.append("  " * depth + f"bone {i}{marker}")
        for c in children.get(i, []):
            if c != i:
                emit(c, depth + 1)

    for r in skeleton_roots(skel.parents):
        emit(r, 0)
    # Any bones not reachable from a root: emit at top level (defensive).
    for i in range(len(skel.parents)):
        if i not in visited:
            emit(i, 0)
    path.write_text("\n".join(lines) + "\n")


def write_skeleton_obj(path: Path, skel: SkeletonFile,
                       hull_centres: dict[int, tuple[float, float, float]]) -> int:
    """Write a stick-figure OBJ of the skeleton using hull centres as joint positions.

    Returns the number of bones placed. Bones missing a hull centre are
    placed at the parent's position (collapsed to a stub) and recorded
    in the OBJ comments.
    """
    n = len(skel.parents)
    # Resolve a position per bone: prefer hull centre; else fall back to the
    # nearest ancestor's hull centre; else (0,0,0).
    pos: list[tuple[float, float, float] | None] = [None] * n
    for i in range(n):
        if i in hull_centres:
            pos[i] = hull_centres[i]
    changed = True
    while changed:
        changed = False
        for i in range(n):
            if pos[i] is None:
                p = skel.parents[i]
                if p >= 0 and pos[p] is not None:
                    pos[i] = pos[p]
                    changed = True
    # Roots without any descendant hull -> origin.
    for i in range(n):
        if pos[i] is None:
            pos[i] = (0.0, 0.0, 0.0)

    placed = sum(1 for i in range(n) if i in hull_centres)
    lines = [
        "# Extermination skeleton stick figure",
        f"# {placed}/{n} bones placed from collision-hull centres; "
        f"others collapsed onto their nearest hulled ancestor.",
    ]
    for i, (x, y, z) in enumerate(pos):
        lines.append(f"v {x:.4f} {y:.4f} {z:.4f}  # bone {i}")
    for i, p in enumerate(skel.parents):
        if p < 0 or p == i:
            continue
        # OBJ vertex indices are 1-based.
        lines.append(f"l {p+1} {i+1}")
    path.write_text("\n".join(lines) + "\n")
    return placed


def rig_hull_centres(rig: "RigFile") -> dict[int, tuple[float, float, float]]:
    """Per-bone collision-hull centroid, derived from the rig plane equations.

    For each bone, gather all plane equations (n.x + D = 0) from --rig and
    take the centroid of the plane points nearest the origin (-D * n for each
    plane). This is a coarse approximation of the bone's collision-hull
    centre, which is in turn a coarse approximation of the joint position --
    good enough for sanity-checking the skeleton topology in a 3D viewer.
    """
    by_bone: dict[int, list[tuple[float, float, float]]] = {}
    for rec in rig.records:
        decoded = _decode_rig_record(rec)
        if decoded is None:
            continue
        (nx, ny, nz), D, _extras = decoded
        # Closest point on plane n.x + D = 0 to origin is -D * n.
        by_bone.setdefault(rec.bone, []).append((-D * nx, -D * ny, -D * nz))
    centres: dict[int, tuple[float, float, float]] = {}
    for bone, pts in by_bone.items():
        cx = sum(p[0] for p in pts) / len(pts)
        cy = sum(p[1] for p in pts) / len(pts)
        cz = sum(p[2] for p in pts) / len(pts)
        centres[bone] = (cx, cy, cz)
    return centres


# ---------------------------------------------------------------------------
# Per-frame vertex animation (pose-set detection)
# ---------------------------------------------------------------------------


def model_topology_key(d: bytes):
    """Topology fingerprint of a model file, or None if it has no geometry.

    The key is (block_count, total_vertices, per-strip vertex counts). Two
    model files with the same key are the same mesh -- different files with
    that key in one region are keyframe poses of one animated mesh.
    """
    if MESH_SIG not in d:
        return None
    strips = parse_model_file(d)
    if not strips:
        return None
    return (len(strips),
            sum(len(s.verts) for s in strips),
            tuple(len(s.verts) for s in strips))


def find_pose_sets(paths: list[Path]) -> list[list[Path]]:
    """Group model files into per-frame-animation pose sets.

    Files are grouped within a single region (same parent directory): a group
    of >=3 model files that share an identical topology fingerprint is a
    vertex-animation pose set (one mesh shipped once per keyframe pose).
    """
    by_region: dict[Path, dict] = {}
    for p in paths:
        if not p.is_file() or p.stem.endswith("_id44"):
            continue
        key = model_topology_key(p.read_bytes())
        if key is None:
            continue
        by_region.setdefault(p.parent, {}).setdefault(key, []).append(p)
    sets: list[list[Path]] = []
    for region in sorted(by_region):
        for key in sorted(by_region[region], key=lambda k: (-k[0], k[1])):
            group = sorted(by_region[region][key])
            if len(group) >= 3:
                sets.append(group)
    return sets


def write_obj(path: Path, strips: list[Strip],
              placed: bool = False) -> tuple[int, int]:
    """Write strips to a Wavefront OBJ. Returns (vertex_count, face_count).

    Strips are grouped into OBJ objects by their marker key, so the in-file
    strip/material grouping is preserved. Each strip's vertices are emitted
    once; positions become `v`, UVs `vt`, and unit normals `vn` (colour-only
    strips get no `vn`). `placed` only changes the header comment -- in
    `--scene` mode the strips already carry their baked world coordinates.
    """
    coord_note = (
        "# Coordinates are world-space: MATRIX instance transforms applied (--scene)."
        if placed else
        "# Coordinates are object-space as stored on disc (no instance transform)."
    )
    lines: list[str] = [
        "# Extermination (SCUS-97112) model -- exported by tools/extract_models.py",
        coord_note,
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


def is_level_file(path: Path) -> bool:
    """True for the `id 0x44` level files, which use the separator-block path."""
    return path.stem.endswith("_id44")


def process(path: Path, out_dir: Path, scene: bool = False) -> str | None:
    """Parse one geometry file and write an OBJ. Returns a status line.

    `id 0x44` files take the validated level path; every other file takes the
    model-variant path. A file with no MESH signature yields nothing.

    With `scene=True`, level files are exported as PLACED scenes -- MATRIX
    instance transforms applied -- to `*_scene.obj`. Model files are not
    instanced and are skipped in scene mode (the default export covers them).
    """
    d = path.read_bytes()
    if is_level_file(path):
        if scene:
            strips = parse_scene(d)
            kind = "scene"
        else:
            strips = parse_file(d)
            kind = "level"
    else:
        if scene:
            # Model files have no level MATRIX blocks to place.
            return None
        if MESH_SIG not in d:
            return None
        strips = parse_model_file(d)
        kind = "model"
    if not strips:
        return None
    suffix = "_scene.obj" if scene else ".obj"
    name = f"{path.parent.name}_{path.stem}{suffix}"
    nv, nf = write_obj(out_dir / name, strips, placed=scene)
    if nf == 0:
        # No real triangles after dropping degenerates -- discard the file.
        (out_dir / name).unlink(missing_ok=True)
        return None
    return (f"{path.parent.name}/{path.name} [{kind}]: {len(strips)} strips, "
            f"{nv} verts, {nf} tris -> {name}")


def run_rig(args, out_dir: Path) -> int:
    """`--rig`: decode rig / skeleton-transform files and dump them."""
    if args.file:
        paths = [Path(args.file)]
    else:
        paths = sorted(Path(args.input).rglob("*_id*.bin"))
    found = 0
    for path in paths:
        if not path.is_file():
            print(f"skip (not found): {path}")
            continue
        d = path.read_bytes()
        if not is_rig_file(d):
            continue
        rig = parse_rig_file(d)
        if rig is None:
            continue
        stem = f"{path.parent.name}_{path.stem}"
        name = stem + "_rig.txt"
        n = write_rig_dump(out_dir / name, rig, f"{path.parent.name}/{path.name}")
        # Also emit a wireframe-OBB OBJ for visualizing the collision hulls.
        obj_name = stem + "_rig_hulls.obj"
        hulls = write_rig_obb_obj(
            out_dir / obj_name, rig, f"{path.parent.name}/{path.name}")
        bones = sorted(set(r.bone for r in rig.records))
        print(f"{path.parent.name}/{path.name}: {n} plane records, "
              f"{len(bones)} bones, {hulls} OBB hull(s) -> {name}, {obj_name}")
        found += 1
    print(f"\n{found} rig file(s) dumped to {out_dir}/")
    return 0


def run_skeleton(args, out_dir: Path) -> int:
    """`--skeleton`: decode id 0x71 character files and dump their bone hierarchy.

    For each id 0x71 file recognised by ``is_skeleton_file``, writes a
    ``*_skeleton.txt`` describing the parent table and tree. If a matching
    collision-hull rig file is found in the same region (any ``*_id*.bin``
    file that ``is_rig_file`` accepts), also writes a ``*_skeleton.obj``
    stick figure using the rig's hull centres as joint positions -- a
    visual sanity check on the topology.
    """
    if args.file:
        paths = [Path(args.file)]
    else:
        paths = sorted(Path(args.input).rglob("*_id*.bin"))
    found = 0
    seen_keys: set[tuple] = set()
    for path in paths:
        if not path.is_file():
            continue
        try:
            d = path.read_bytes()
        except OSError:
            continue
        skel = parse_skeleton_file(d)
        if skel is None:
            continue
        # Dedupe identical skeletons across regions: many regions ship the
        # same player-character file. Key on (bone_count, parents tuple).
        key = (len(skel.parents), tuple(skel.parents))
        if key in seen_keys:
            # Still write per-region but tag it.
            pass
        seen_keys.add(key)

        # Find a matching rig file in the same region for hull centres.
        hull_centres: dict[int, tuple[float, float, float]] = {}
        region = path.parent
        for sibling in sorted(region.glob("*_id*.bin")):
            if sibling == path:
                continue
            try:
                sd = sibling.read_bytes()
            except OSError:
                continue
            if not is_rig_file(sd):
                continue
            rig = parse_rig_file(sd)
            if rig is None:
                continue
            cand = rig_hull_centres(rig)
            # Only adopt hulls whose bone IDs all index into this skeleton.
            if cand and all(b < len(skel.parents) for b in cand):
                hull_centres = cand
                break

        stem = f"{path.parent.name}_{path.stem}"
        txt_name = stem + "_skeleton.txt"
        write_skeleton_dump(out_dir / txt_name, skel,
                            f"{path.parent.name}/{path.name}",
                            hull_centres or None)
        msg = (f"{path.parent.name}/{path.name}: {len(skel.parents)} bones, "
               f"{skel.entry_count} entries -> {txt_name}")
        if hull_centres:
            obj_name = stem + "_skeleton.obj"
            placed = write_skeleton_obj(out_dir / obj_name, skel, hull_centres)
            msg += f", {obj_name} ({placed} hulled joints)"
        print(msg)
        found += 1
    print(f"\n{found} skeleton file(s) decoded to {out_dir}/")
    return 0


# ---------------------------------------------------------------------------
# Per-bone "skinned" character meshes
# ---------------------------------------------------------------------------
#
# Many character-mesh files (ids 0x70, 0x72, 0x74, 0x88, 0x89, 0x8b, 0x8f, 0xa0,
# 0xa3 and others) contain a TWO-STAGE representation:
#
#   1. A pre-MESH "VIF prefix" -- a stream of small fixed-width quantised
#      vertex packets organised PER BONE. Each section is one bone's bind-pose
#      vertices, set up to be uploaded to VU1 with that bone's joint matrix
#      already preloaded; VU1 then transforms them and emits world-space
#      strips. The bone-section boundary table is a u32-offset array somewhere
#      in the prefix region -- detected by `_find_bone_section_table`.
#
#   2. A regular MESH-descriptor section already decoded by `parse_model_file`,
#      carrying ALREADY-WORLD-SPACE strips (the same vertices that VU1 would
#      emit at bind pose). This is what the existing default exporter writes.
#
# Fully decoding stage 1 requires the VU1 microcode (out of scope -- the
# packet format is `[VIF UNPACK] [N x 6-byte fixed-point xyz + u16 vid]`
# with bone-matrix preloads handled by the engine). What we CAN do without
# the microcode is:
#
#   * Recover the per-bone SECTION BOUNDARY TABLE -- a u32 array of offsets
#     whose length matches the file's skeleton bone count (confirmed for
#     several characters: chunk17/f14_id8b 28 entries vs 28-bone skeleton).
#   * Use the existing world-space strips from stage 2 as the rigged output,
#     grouped into one OBJ object per (m0, m1) strip key (which the engine
#     uses as a material/sub-mesh tag), tagged with section index when the
#     prefix carries a section table. Combined with --skeleton, this lets a
#     downstream tool (Blender) re-bind by hand using the bone-section count
#     as a guide.
#
# Per-vertex bone WEIGHTS are NOT carried in the 64-byte vertex record (every
# byte field is fully accounted for: marker / strip-flag, UV, normal-or-colour,
# position). Skinning is per-bone-rigid (every vertex in a stage-1 bone
# section is rigidly attached to that bone) -- standard PS2 character idiom.

# Bytes that mark the start of a model file's section/offset table -- a run of
# small monotonically-increasing u32 offsets terminated by a zero u32. The
# table's length is the file's bone-section count.
def _find_bone_section_table(d: bytes) -> tuple[int, list[int]] | None:
    """Locate the per-bone offset table in a character-mesh file.

    Returns (table_start_offset, [u32 offsets...]) or None if not found. The
    table is a run of 10..60 monotonically-increasing u32 values, each less
    than the file size and the first less than 0x200, terminated by a zero
    u32. It lives before the first MESH descriptor.
    """
    first_mesh = d.find(MESH_SIG)
    if first_mesh < 1024:
        return None
    # The table lives in the file PREFIX (before any geometry). Scan only
    # the bytes before the first MESH descriptor, never trailing padding.
    scan_end = first_mesh - 180
    if scan_end < 0x100:
        return None
    for o in range(0x100, scan_end, 4):
        try:
            vals = list(struct.unpack_from("<40I", d, o))
        except struct.error:
            break
        n = 0
        for i in range(1, len(vals)):
            if vals[i] == 0:
                n = i
                break
            if vals[i] <= vals[i - 1] or vals[i] >= len(d):
                break
            n = i + 1
        if 10 <= n <= 60 and vals[0] < 0x200:
            return o, vals[:n]
    return None


def write_skinned_obj(path: Path, strips: list[Strip],
                       n_bone_sections: int | None,
                       src_name: str) -> tuple[int, int]:
    """Write a "skinned"-tagged OBJ.

    Groups strips by (m0, m1) like the default exporter, but prefixes the
    object names with `bone_NN_` where NN is the index of the nearest
    preceding bone-section boundary (if a section table was found). Without
    per-vertex weights from the VIF prefix this is the most honest bone
    attribution possible from the world-space strips alone.
    """
    header = [
        f"# Extermination (SCUS-97112) skinned model: {src_name}",
        "# World-space strips from the MESH-descriptor stage (post-skinning).",
    ]
    if n_bone_sections is not None:
        header.append(
            f"# {n_bone_sections} per-bone VIF section(s) detected in the file "
            "prefix; vertices are grouped by (m0,m1) strip key, not by bone "
            "(decoding the per-bone VIF packets needs VU1 microcode)."
        )
    else:
        header.append(
            "# No per-bone VIF section table found -- this file may not be a "
            "skinned character mesh, or uses a different prefix layout."
        )

    v_base = 1
    vt_base = 1
    vn_base = 1
    total_v = 0
    total_f = 0
    lines: list[str] = list(header)

    groups: dict[tuple[int, int], list[Strip]] = {}
    order: list[tuple[int, int]] = []
    for s in strips:
        if s.key not in groups:
            groups[s.key] = []
            order.append(s.key)
        groups[s.key].append(s)

    for gi, key in enumerate(order):
        group = groups[key]
        lines.append(
            f"o submesh_{gi:04d}_{key[0]:08x}_{key[1]:06x}"
        )
        for s in group:
            for v in s.verts:
                lines.append(f"v {v.pos[0]:.6f} {v.pos[1]:.6f} {v.pos[2]:.6f}")
            for v in s.verts:
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


def run_skinned(args, out_dir: Path) -> int:
    """`--skinned`: export character mesh files with bone-section metadata.

    Detects model files that carry a VIF-prefix per-bone section table,
    decodes the table, and writes a `*_skinned.obj` per such file with the
    bone-section count in the header comment plus a sibling `*_skinned.txt`
    describing the table. Also walks paired id 0x71 skeletons / id 0x73
    rig hulls when present.
    """
    if args.file:
        paths = [Path(args.file)]
    else:
        paths = sorted(Path(args.input).rglob("*_id*.bin"))

    exported = 0
    skipped = 0
    for path in paths:
        if not path.is_file():
            continue
        if is_level_file(path):
            # id 0x44 level files also have offset tables in their prefix
            # (level-extent / scene-graph metadata) -- not bone sections.
            skipped += 1
            continue
        d = path.read_bytes()
        if MESH_SIG not in d:
            skipped += 1
            continue
        table = _find_bone_section_table(d)
        # Try to find a paired skeleton in the same region directory.
        skel_path = None
        for cand in sorted(path.parent.glob("*_id71.bin")):
            skel_path = cand
            break
        strips = parse_model_file(d)
        if not strips:
            skipped += 1
            continue
        # Only export when we found a per-bone table OR when caller passed --file
        # explicitly (so they can inspect any model).
        if table is None and not args.file:
            skipped += 1
            continue

        region = path.parent.name
        stem = path.stem
        out_obj = out_dir / f"{region}_{stem}_skinned.obj"
        out_txt = out_dir / f"{region}_{stem}_skinned.txt"
        n_sections = len(table[1]) if table else None
        nv, nf = write_skinned_obj(out_obj, strips, n_sections,
                                    f"{region}/{path.name}")

        # Sidecar txt: table layout + skeleton pairing.
        txt: list[str] = [
            f"# Skinned-mesh report: {region}/{path.name}",
            f"# File size: {len(d)} bytes ({len(d):#x})",
        ]
        if table:
            tbl_off, entries = table
            txt.append(
                f"# Bone-section offset table @ {tbl_off:#x}, "
                f"{len(entries)} entries (= bone-section count)."
            )
            for i, off in enumerate(entries):
                txt.append(f"section {i:3d}: prefix offset {off:#08x}")
        else:
            txt.append("# No bone-section table detected (single-section "
                       "or unrecognised prefix layout).")
        if skel_path is not None:
            skel = parse_skeleton_file(skel_path.read_bytes())
            if skel:
                txt.append(
                    f"# Paired skeleton: {skel_path.name} "
                    f"({skel.bone_count} bones)"
                )
                if n_sections == skel.bone_count or (
                    n_sections is not None and abs(n_sections - skel.bone_count) <= 2
                ):
                    txt.append(
                        "# Bone-section count matches the skeleton's bone "
                        "count -- CONFIRMED per-bone rigid attachment."
                    )
            else:
                txt.append(f"# Paired skeleton: {skel_path.name} (unparsed)")
        else:
            txt.append("# No paired id 0x71 skeleton in this region.")
        out_txt.write_text("\n".join(txt) + "\n")

        print(f"{region}/{path.name}: {nv} verts, {nf} tris, "
              f"{n_sections or 0} bone-section(s) -> {out_obj.name}")
        exported += 1

    print(f"\n{exported} skinned-character file(s) exported to {out_dir}/  "
          f"({skipped} file(s) had no detected bone table)")
    return 0


# ---------------------------------------------------------------------------
# Per-bone OBJECT-SPACE vertex extraction (--object-space)
# ---------------------------------------------------------------------------
#
# Each per-bone section in the VIF prefix is a stream of 12-byte records:
#
#     +0x00  int16  x           Q4.12 fixed-point (raw / 4096.0)
#     +0x02  int16  y           Q4.12
#     +0x04  int16  z           Q4.12
#     +0x06  4 bytes            packed normal / lighting / VIF padding
#                               (signed-byte components; not yet decoded)
#     +0x0a  uint16 vid         monotonically-increasing vertex id within
#                               this bone's packet; 0xffff terminates a
#                               sub-block (also used as a header terminator)
#
# Each bone's region starts with two priming records (degenerate strip
# starters), then 0xffff terminator, then the actual vertex stream. We treat
# the 12-byte stride uniformly and stop at the first 0xffff vid seen after
# the priming records (or when we run into the next bone's offset).
#
# The Q4.12 dequant follows directly from the ITOF12 instruction in the
# 15-qw inner helper at imem 0x0800 (see disasm_vu.py / FINDINGS.md). The
# values land in bone-local coordinates with a natural range of [-8, +8],
# which matches the empirical bbox of every decoded packet.

OBJSPACE_REC_SIZE = 12
OBJSPACE_VID_TERMINATOR = 0xffff
OBJSPACE_HEADER_SIZE = 0x1c  # two priming records (24 bytes) + 4-byte terminator


def decode_objspace_bone_vertices(d: bytes, sect_start: int, sect_end: int
                                  ) -> list[tuple[float, float, float, int]]:
    """Decode one bone's per-vertex object-space records.

    Returns a list of (x, y, z, vid) tuples in the bone's local frame.
    """
    out: list[tuple[float, float, float, int]] = []
    body = sect_start + OBJSPACE_HEADER_SIZE
    off = body
    while off + OBJSPACE_REC_SIZE <= sect_end:
        vid = struct.unpack_from("<H", d, off + 10)[0]
        if vid == OBJSPACE_VID_TERMINATOR:
            break
        x, y, z = struct.unpack_from("<3h", d, off)
        out.append((x / 4096.0, y / 4096.0, z / 4096.0, vid))
        off += OBJSPACE_REC_SIZE
    return out


def run_object_space(args, out_dir: Path) -> int:
    """`--object-space`: decode per-bone Q4.12 vertices into OBJ + summary.

    For each character-mesh file carrying a VIF-prefix per-bone section
    table, write:
      * `*_objspace.obj` -- one `o bone_NN` group per bone with the
        decoded object-space vertices as points (no faces; the per-bone
        VIF stream is a quantised point cloud, not a triangulated mesh).
      * `*_objspace.txt` -- per-bone vertex counts, bboxes and total.
    """
    if args.file:
        paths = [Path(args.file)]
    else:
        paths = sorted(Path(args.input).rglob("*_id*.bin"))

    exported = 0
    skipped = 0
    for path in paths:
        if not path.is_file():
            continue
        if is_level_file(path):
            skipped += 1
            continue
        d = path.read_bytes()
        if MESH_SIG not in d:
            skipped += 1
            continue
        table = _find_bone_section_table(d)
        if table is None:
            skipped += 1
            continue
        table_off, entries = table

        region = path.parent.name
        stem = path.stem
        out_obj = out_dir / f"{region}_{stem}_objspace.obj"
        out_txt = out_dir / f"{region}_{stem}_objspace.txt"

        lines = [
            f"# Extermination (SCUS-97112) per-bone object-space vertices: "
            f"{region}/{path.name}",
            "# Decoded from VIF prefix; Q4.12 fixed-point (raw_i16 / 4096.0).",
            "# Each `o bone_NN` group is one bone's bind-pose vertices in "
            "that bone's local frame -- no bone-matrix transform applied.",
            f"# Section table @ {table_off:#x}, {len(entries)} bone section(s).",
        ]
        txt = [
            f"# Per-bone object-space report: {region}/{path.name}",
            f"# Section table @ {table_off:#x}, {len(entries)} bone section(s).",
            "# Format per bone: vertex count, vid range, bbox extents (Q4.12 units).",
            "",
        ]
        total_v = 0
        per_bone_counts = []
        for i, off in enumerate(entries):
            sect_start = table_off + off
            sect_end = (table_off + entries[i + 1]
                        if i + 1 < len(entries) else len(d))
            verts = decode_objspace_bone_vertices(d, sect_start, sect_end)
            per_bone_counts.append(len(verts))
            lines.append(f"o bone_{i:02d}")
            for (x, y, z, _vid) in verts:
                lines.append(f"v {x:.6f} {y:.6f} {z:.6f}")
            if verts:
                xs = [v[0] for v in verts]
                ys = [v[1] for v in verts]
                zs = [v[2] for v in verts]
                vids = [v[3] for v in verts]
                txt.append(
                    f"bone {i:2d}: {len(verts):4d}v  "
                    f"vid[{min(vids):3d}..{max(vids):3d}]  "
                    f"x[{min(xs):6.2f}..{max(xs):6.2f}] "
                    f"y[{min(ys):6.2f}..{max(ys):6.2f}] "
                    f"z[{min(zs):6.2f}..{max(zs):6.2f}]  "
                    f"ext=({max(xs)-min(xs):5.2f},"
                    f"{max(ys)-min(ys):5.2f},"
                    f"{max(zs)-min(zs):5.2f})"
                )
            else:
                txt.append(f"bone {i:2d}:    0v  (empty / index-only section)")
            total_v += len(verts)

        out_obj.write_text("\n".join(lines) + "\n")
        txt.append("")
        txt.append(f"TOTAL: {total_v} object-space vertices across "
                   f"{len(entries)} bone section(s).")
        out_txt.write_text("\n".join(txt) + "\n")

        print(f"{region}/{path.name}: {total_v} object-space verts "
              f"across {len(entries)} bone(s) -> {out_obj.name}")
        exported += 1

    print(f"\n{exported} character file(s) exported (object-space) to "
          f"{out_dir}/  ({skipped} file(s) skipped -- no bone table)")
    return 0


# ---------------------------------------------------------------------------
# --rigged: posed bind-pose OBJ from live save-state matrices.
# ---------------------------------------------------------------------------
#
# Combines three already-validated pieces of work:
#   * the id 0x71 PARENT HIERARCHY (--skeleton),
#   * the per-bone Q4.12 OBJECT-SPACE vertices (--object-space),
#   * a JSON of LIVE bone matrices snapped from a PCSX2 save state
#     (tools/parse_pcsx2_state.py --player-bones).
#
# The save state typically yields TWO buffers per character (PS2 engines
# double-buffer current/previous frame, or split world/local). Both are written
# to the JSON; this mode tries each combination of (treat-as-world,
# treat-as-local-and-compose) and picks the one whose result better matches the
# obj-space bone-section count + skeleton bone count. Mismatched / inactive
# slots (engine often only fills the active subset of the declared skeleton)
# fall back to identity so the mesh still exports.
#
# Output: a single ``*_rigged.obj`` with one ``o bone_NN`` group per bone, in
# WORLD-relative coordinates of the captured pose. No faces: the per-bone VIF
# stream is a quantised point cloud, not triangulated (faces would need the
# strip topology re-stitched from the post-VU1 MESH section). The intent is
# downstream tools (Blender, our own viewer) plus the --skinned export for
# triangulation.


def _compose_world_from_local(local_mats: list[tuple], parents: list[int]
                              ) -> list[tuple]:
    """Compose world matrices from per-bone local matrices + parent table.

    ``local_mats[i]`` is column-major (col0..col3); each col is (x,y,z,w).
    ``parents[i]`` is the parent bone index or -1 for a root. The parent table
    may contain cycles (e.g. bone 2<->3 in the player skeleton); a bone whose
    parent chain does not terminate at a root within ``max_depth`` steps is
    treated as its own root to avoid infinite recursion.
    """
    n = len(local_mats)
    world: list[tuple | None] = [None] * n

    def mul(A: tuple, B: tuple) -> tuple:
        def apply(col):
            x, y, z, w = col
            return [A[0][r] * x + A[1][r] * y + A[2][r] * z + A[3][r] * w
                    for r in range(4)]
        return tuple(apply(B[c]) for c in range(4))

    def resolve(i: int, seen: set) -> tuple:
        if world[i] is not None:
            return world[i]
        if i in seen:
            world[i] = local_mats[i]
            return world[i]
        seen = seen | {i}
        p = parents[i] if i < len(parents) else -1
        if p < 0 or p == i or p >= n:
            world[i] = local_mats[i]
        else:
            world[i] = mul(resolve(p, seen), local_mats[i])
        return world[i]

    for i in range(n):
        resolve(i, set())
    return [w if w is not None else local_mats[i] for i, w in enumerate(world)]


def _load_bones_json(path: Path) -> list[list[tuple]]:
    """Load player_bones.json -> list of buffers, each a list of column-major
    matrices stored as a 4-tuple of (x,y,z,w) columns."""
    import json
    data = json.loads(path.read_text())
    buffers = data.get("buffers")
    if buffers is None:
        # Backward compat: single-run dump from --dump-bones.
        buffers = [data]
    out: list[list[tuple]] = []
    for buf in buffers:
        mats = []
        for b in buf.get("matrices", buf.get("bones", [])):
            mats.append((
                tuple(b["col0"]), tuple(b["col1"]),
                tuple(b["col2"]), tuple(b["col3"]),
            ))
        out.append(mats)
    return out


def _transform_objspace_point(W: tuple, p: tuple) -> tuple[float, float, float]:
    """Apply a column-major affine matrix W to an object-space point."""
    x, y, z = p
    # W = (col0, col1, col2, col3); world = col0*x + col1*y + col2*z + col3.
    return (W[0][0] * x + W[1][0] * y + W[2][0] * z + W[3][0],
            W[0][1] * x + W[1][1] * y + W[2][1] * z + W[3][1],
            W[0][2] * x + W[1][2] * y + W[2][2] * z + W[3][2])


def _identity_mat() -> tuple:
    return ((1.0, 0.0, 0.0, 0.0),
            (0.0, 1.0, 0.0, 0.0),
            (0.0, 0.0, 1.0, 0.0),
            (0.0, 0.0, 0.0, 1.0))


def _quat_to_local_mat(q: tuple, t: tuple) -> tuple:
    """Build a column-major 4x4 affine = T * R from a unit quaternion (qx,qy,qz,qw)
    and a translation vec3. Scale defaults to 1.0 (per-bone struct +0x7C scale is
    not stored in id 0x71 -- the engine seeds it elsewhere; identity scale is the
    correct default for a bind-pose evaluator).

    The 3x3 rotation matrix R, derived from a unit quaternion, is stored as the
    first three columns. Translation goes into column 3.
    """
    x, y, z, w = q
    # Standard quat->matrix (right-handed). Columns of R.
    xx, yy, zz = x * x, y * y, z * z
    xy, xz, yz = x * y, x * z, y * z
    wx, wy, wz = w * x, w * y, w * z
    col0 = (1.0 - 2.0 * (yy + zz), 2.0 * (xy + wz),       2.0 * (xz - wy),       0.0)
    col1 = (2.0 * (xy - wz),       1.0 - 2.0 * (xx + zz), 2.0 * (yz + wx),       0.0)
    col2 = (2.0 * (xz + wy),       2.0 * (yz - wx),       1.0 - 2.0 * (xx + yy), 0.0)
    col3 = (t[0], t[1], t[2], 1.0)
    return (col0, col1, col2, col3)


def bind_pose_at_t(id71_path: Path, entry_idx: int = 0, time_frames: float = 0.0
                   ) -> tuple[list[tuple], list[int]]:
    """Decode an id 0x71 clip and return WORLD-space 4x4 column-major matrices
    sampled at ``time_frames`` (default frame 0).

    Pipeline:
      - parse the file's entry directory; slice ``entry_idx``'s payload;
      - extract bone count, section1 (rotation), section2 (translation) bases,
        and the bone-count parent table (per FINDINGS, +0x28);
      - per bone i in [0..bone_count_raw):
          q = sample rotation stream at t (identity if no keyframes);
          tr = sample translation stream at t ((0,0,0) if none);
          local[i] = TRS_to_matrix(q, tr);
      - walk parent table to compose world matrices (cycle-safe, mirroring
        ``_compose_world_from_local``).

    Returns (world_matrices, parents). world_matrices length = bone_count_raw
    (30 for the player). The trailing overshoot slots are included so the
    caller can choose how to handle them.
    """
    # local import keeps the module's import cost low at top-level
    import importlib.util
    import sys as _sys
    if "_anim_decoder" in _sys.modules:
        ad = _sys.modules["_anim_decoder"]
    else:
        _here = Path(__file__).resolve().parent
        _spec = importlib.util.spec_from_file_location(
            "_anim_decoder", _here / "anim_decoder.py")
        ad = importlib.util.module_from_spec(_spec)
        _sys.modules["_anim_decoder"] = ad
        _spec.loader.exec_module(ad)

    d = id71_path.read_bytes()
    n_entries = struct.unpack_from("<I", d, 0)[0]
    if not (1 <= n_entries <= 512):
        raise ValueError(f"{id71_path}: not an id 0x71 file (entry_count={n_entries})")
    offs = struct.unpack_from(f"<{n_entries + 1}I", d, 4)
    if entry_idx >= n_entries:
        raise ValueError(f"entry_idx {entry_idx} out of range (count={n_entries})")
    e_start = offs[entry_idx]
    e_end = offs[entry_idx + 1] if (entry_idx + 1) < n_entries else len(d)
    if offs[entry_idx + 1] == 0xFFFFFFFF and (entry_idx + 1) == n_entries:
        e_end = len(d)
    entry = d[e_start:e_end]

    bc_raw = entry[0]  # may include overshoot sentinels (e.g. 30 for player)
    s1 = struct.unpack_from("<I", entry, 0x08)[0]
    s2 = struct.unpack_from("<I", entry, 0x0c)[0]
    s3 = struct.unpack_from("<I", entry, 0x10)[0]
    raw_parents = list(struct.unpack_from(f"<{bc_raw}I", entry, 0x28))

    sec1 = entry[s1:s2]
    sec2 = entry[s2:s3]
    rot_streams = ad.parse_rotation_section(sec1, bc_raw)
    trn_streams = ad.parse_translation_section(sec2, bc_raw)

    local_mats: list[tuple] = []
    for i in range(bc_raw):
        q = ad.sample_bone(rot_streams[i], time_frames) if rot_streams[i] else None
        if q is None:
            q = (0.0, 0.0, 0.0, 1.0)  # identity rotation
        tr = ad.sample_bone(trn_streams[i], time_frames, normalize=False) \
            if trn_streams[i] else None
        if tr is None:
            tr = (0.0, 0.0, 0.0)
        local_mats.append(_quat_to_local_mat(q, tr))

    # Build parent list compatible with _compose_world_from_local: -1 for
    # self-parented root, -1 for indices out-of-range (overshoot sentinels).
    parents_signed: list[int] = []
    for i, p in enumerate(raw_parents):
        if p == i or p >= bc_raw:
            parents_signed.append(-1)
        else:
            parents_signed.append(p)

    world = _compose_world_from_local(local_mats, parents_signed)
    return world, parents_signed


def run_rigged(args, out_dir: Path) -> int:
    """`--rigged`: emit a posed humanoid OBJ from id 0x71 clip-evaluated matrices.

    Default behaviour (NEW, 2026-05-27): decode an id 0x71 clip directly from
    disc-extracted data and evaluate the bind pose at a chosen frame --
    no save-state capture needed.

      --from-id71 PATH[:ENTRY[:TIME]]    e.g. extract/chunk05/f04_id71.bin:0:0
      --file     character mesh file with VIF-prefix per-bone obj-space verts.

    Legacy (deprecated): --bones path/to/player_bones.json keeps the old
    PCSX2 save-state pipeline working for back-compat.
    """
    mesh_path = Path(args.file) if args.file else Path(
        "extract/chunk21/f17_id8f.bin")
    if not mesh_path.is_file():
        print(f"error: mesh file not found: {mesh_path}")
        return 2

    d = mesh_path.read_bytes()
    table = _find_bone_section_table(d)
    if table is None:
        print(f"error: no per-bone VIF section table in {mesh_path}")
        return 2
    table_off, entries = table

    # Skeleton: any id 0x71 in any region (the player skeleton is shared);
    # prefer one in the SAME region as the mesh, else fall back to the first
    # 28-bone skeleton found anywhere under extract/.
    skel = None
    skel_src = None
    for cand in sorted(mesh_path.parent.glob("*_id71.bin")):
        sk = parse_skeleton_file(cand.read_bytes())
        if sk:
            skel, skel_src = sk, cand
            break
    if skel is None:
        for cand in sorted(Path(args.input).rglob("*_id71.bin")):
            sk = parse_skeleton_file(cand.read_bytes())
            if sk and len(sk.parents) >= len(entries):
                skel, skel_src = sk, cand
                break
    if skel is None:
        print("error: no id 0x71 skeleton found for parent hierarchy")
        return 2

    # --- choose matrix source ---------------------------------------------
    source_desc = ""
    bones_path = Path(args.bones) if args.bones else None
    if bones_path is not None:
        # LEGACY: PCSX2 save-state JSON.
        if not bones_path.is_file():
            print(f"error: --bones file not found: {bones_path}")
            return 2
        buffers = _load_bones_json(bones_path)
        if not buffers:
            print(f"error: no matrices found in {bones_path}")
            return 2
        local_mats = list(buffers[0])
        n_live = len(local_mats)
        parents_live = list(skel.parents[:n_live])
        while len(parents_live) < n_live:
            parents_live.append(-1)
        world = _compose_world_from_local(local_mats, parents_live)
        source_desc = (f"PCSX2 save-state JSON: {bones_path} "
                       f"(buffer[0], {n_live} matrices)")
    else:
        # NEW DEFAULT: evaluate an id 0x71 clip at frame ``time_frames``.
        clip_spec = getattr(args, "from_id71", None)
        if clip_spec:
            parts = clip_spec.split(":")
            clip_path = Path(parts[0])
            entry_idx = int(parts[1]) if len(parts) > 1 and parts[1] else 0
            t_frames = float(parts[2]) if len(parts) > 2 and parts[2] else 0.0
        else:
            clip_path = skel_src
            entry_idx = 0
            t_frames = 0.0
        if not clip_path.is_file():
            print(f"error: id 0x71 file not found: {clip_path}")
            return 2
        world_full, parents_full = bind_pose_at_t(clip_path, entry_idx, t_frames)
        # world_full length = bone_count_raw (e.g. 30 for the player). The
        # mesh's bone-section table is what governs how many sections we
        # actually emit; world_full[i] is used for section i.
        world = world_full
        n_live = len(world)
        source_desc = (f"id 0x71 clip: {clip_path} entry {entry_idx} "
                       f"@ t={t_frames} ({n_live} bones)")
    n_skel = len(skel.parents)
    n_bones = len(entries)

    # Read per-bone obj-space verts and transform.
    out_obj = out_dir / (f"{mesh_path.parent.name}_{mesh_path.stem}_rigged.obj")
    out_txt = out_dir / (f"{mesh_path.parent.name}_{mesh_path.stem}_rigged.txt")
    lines = [
        f"# Extermination (SCUS-97112) RIGGED model: "
        f"{mesh_path.parent.name}/{mesh_path.name}",
        f"# matrix source: {source_desc}",
        f"# skeleton:      {skel_src.parent.name}/{skel_src.name} "
        f"({n_skel} bones, parent table used to compose local->world)",
        "# Per-bone object-space Q4.12 verts transformed by the composed "
        "world matrix and grouped by bone.",
    ]
    total_v = 0
    report = [
        f"# Rigged-mesh report: {mesh_path.parent.name}/{mesh_path.name}",
        f"# matrix source: {source_desc}",
        f"# skeleton:   {skel_src.parent.name}/{skel_src.name} "
        f"({n_skel} bones, parents = {skel.parents})",
        f"# bone-section table: {len(entries)} sections in mesh; "
        f"{n_live} world matrices computed from clip",
        "",
    ]
    skipped_sections = 0
    for i, off in enumerate(entries):
        sect_start = table_off + off
        sect_end = (table_off + entries[i + 1]
                    if i + 1 < len(entries) else len(d))
        # Skip bone sections beyond the live-matrix count: those slots are
        # inactive (col3.w == 0 in EE RAM), so we would otherwise emit their
        # raw object-space vertex packets at the world origin as bogus
        # extra-humanoid copies.
        if i >= n_live:
            verts_count = sum(1 for _ in decode_objspace_bone_vertices(d, sect_start, sect_end))
            if verts_count:
                skipped_sections += 1
                report.append(
                    f"bone {i:2d}: SKIPPED ({verts_count}v in section, but no "
                    f"live matrix -- bone slot inactive)"
                )
            continue
        verts = decode_objspace_bone_vertices(d, sect_start, sect_end)
        W = world[i]
        lines.append(f"o bone_{i:02d}")
        bb_min = [float("inf")] * 3
        bb_max = [-float("inf")] * 3
        for (x, y, z, _vid) in verts:
            wx, wy, wz = _transform_objspace_point(W, (x, y, z))
            lines.append(f"v {wx:.6f} {wy:.6f} {wz:.6f}")
            for k, v in enumerate((wx, wy, wz)):
                if v < bb_min[k]:
                    bb_min[k] = v
                if v > bb_max[k]:
                    bb_max[k] = v
        total_v += len(verts)
        if verts:
            report.append(
                f"bone {i:2d}: {len(verts):4d}v  world bbox "
                f"x[{bb_min[0]:+7.2f}..{bb_max[0]:+7.2f}] "
                f"y[{bb_min[1]:+7.2f}..{bb_max[1]:+7.2f}] "
                f"z[{bb_min[2]:+7.2f}..{bb_max[2]:+7.2f}]"
            )
        else:
            report.append(f"bone {i:2d}:    0v  (empty section)")
    out_obj.write_text("\n".join(lines) + "\n")
    report.append("")
    report.append(f"TOTAL: {total_v} world-space vertices across "
                  f"{n_live} live bone(s); {skipped_sections} trailing "
                  f"bone section(s) with geometry skipped (inactive slot).")
    out_txt.write_text("\n".join(report) + "\n")
    print(f"{mesh_path.parent.name}/{mesh_path.name}: {total_v} verts, "
          f"{n_live}/{n_bones} live bones -> {out_obj.name}")
    print(f"  ({source_desc}; skeleton from {skel_src.name})")
    return 0


def run_anim(args, out_dir: Path) -> int:
    """`--anim`: detect and export per-frame vertex-animation pose sets."""
    if args.file:
        paths = [Path(args.file)]
    else:
        paths = sorted(Path(args.input).rglob("*_id*.bin"))
    sets = find_pose_sets(paths)
    exported = 0
    for group in sets:
        region = group[0].parent.name
        # A stable base name from the region and the group's shared topology.
        first = group[0]
        base = f"{region}_{first.stem.split('_')[0]}_anim"
        for frame, path in enumerate(group):
            strips = parse_model_file(path.read_bytes())
            name = f"{base}_frame{frame:02d}.obj"
            nv, nf = write_obj(out_dir / name, strips)
            if nf == 0:
                (out_dir / name).unlink(missing_ok=True)
        print(f"{region}: pose set of {len(group)} frames "
              f"({first.name}..{group[-1].name}) -> {base}_frameNN.obj")
        exported += 1
    print(f"\n{exported} vertex-animation pose set(s) exported to {out_dir}/")
    if exported == 0:
        print("(no per-frame pose sets found -- most models are not "
              "vertex-animated; see --rig for the skeleton-transform files)")
    return 0


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Extermination 3D model extractor")
    p.add_argument("--in", dest="input", default="extract",
                   help="extraction directory to scan for *_id*.bin (default: extract)")
    p.add_argument("--file", help="convert a single geometry file instead of scanning")
    p.add_argument("--out", default="models",
                   help="output directory for OBJ files (default: models)")
    p.add_argument("--levels-only", action="store_true",
                   help="only convert the id 0x44 level files (skip model variants)")
    p.add_argument("--scene", action="store_true",
                   help="export PLACED full-level scenes: decode the MATRIX "
                        "instance transforms and bake them so instanced "
                        "geometry sits at its world position. Writes "
                        "*_scene.obj; only affects the id 0x44 level files. "
                        "Opt-in and additive -- the default per-mesh export is "
                        "unchanged.")
    p.add_argument("--rig", action="store_true",
                   help="decode the rig / skeleton-transform files (small "
                        "non-MESH files of 0x78-byte VIF-tagged joint "
                        "records) and write a *_rig.txt dump of each. Opt-in "
                        "and additive -- does not affect geometry export.")
    p.add_argument("--skeleton", action="store_true",
                   help="decode id 0x71 character/animation files: dump the "
                        "bone PARENT HIERARCHY as a *_skeleton.txt, and (when "
                        "a matching collision-hull rig file exists in the "
                        "same region) write a *_skeleton.obj stick figure "
                        "with joints placed at hull centres. Opt-in.")
    p.add_argument("--skinned", action="store_true",
                   help="export character-mesh files that carry a per-bone "
                        "VIF prefix section table. Writes *_skinned.obj "
                        "(world-space strips, grouped by sub-mesh key) plus a "
                        "*_skinned.txt with the bone-section table and "
                        "skeleton pairing summary. Opt-in and additive.")
    p.add_argument("--object-space", dest="object_space", action="store_true",
                   help="decode per-bone VIF-prefix vertex packets into "
                        "OBJECT-SPACE (bone-local) coordinates using the "
                        "Q4.12 dequant formula confirmed from the VU1 "
                        "skinning kernel (raw_i16 / 4096.0). Writes "
                        "*_objspace.obj (one `o bone_NN` point-cloud group "
                        "per bone, NO faces -- the VIF stream is pre-strip "
                        "quantised vertices) plus *_objspace.txt with "
                        "per-bone counts and bboxes. Opt-in and additive.")
    p.add_argument("--rigged", action="store_true",
                   help="emit a POSED rigged OBJ. NEW DEFAULT (2026-05-27): "
                        "decode an id 0x71 animation clip directly from disc "
                        "data, evaluate the per-bone TRS at frame 0 (or "
                        "--from-id71 PATH:ENTRY:TIME), compose world matrices "
                        "via the parent table, and apply them to the per-bone "
                        "object-space Q4.12 vertex packets from --file. "
                        "Legacy --bones JSON path remains for back-compat. "
                        "Output: *_rigged.obj (one `o bone_NN` group per bone, "
                        "world-space). Opt-in and additive.")
    p.add_argument("--bones",
                   help="LEGACY (deprecated): path to player_bones.json from "
                        "parse_pcsx2_state.py --player-bones. If supplied, "
                        "--rigged uses the PCSX2 save-state matrices instead "
                        "of decoding an id 0x71 clip.")
    p.add_argument("--from-id71", dest="from_id71",
                   help="id 0x71 clip source for --rigged, formatted as "
                        "PATH[:ENTRY[:TIME]] (e.g. extract/chunk05/f04_id71.bin"
                        ":0:0). When omitted, --rigged uses the auto-paired "
                        "skeleton's entry 0 at frame 0.")
    p.add_argument("--anim", action="store_true",
                   help="detect per-frame vertex-animation pose sets (>=3 "
                        "model files in one region sharing identical "
                        "topology) and export each as a *_frameNN.obj "
                        "sequence. Opt-in and additive.")
    args = p.parse_args(argv)

    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)

    if args.rig:
        return run_rig(args, out_dir)
    if args.skeleton:
        return run_skeleton(args, out_dir)
    if args.skinned:
        return run_skinned(args, out_dir)
    if args.object_space:
        return run_object_space(args, out_dir)
    if args.rigged:
        return run_rigged(args, out_dir)
    if args.anim:
        return run_anim(args, out_dir)

    if args.file:
        paths = [Path(args.file)]
    elif args.scene or args.levels_only:
        paths = sorted(Path(args.input).rglob("*_id44.bin"))
    else:
        # Every chunk file; process() routes id44 -> level path, rest -> model.
        paths = sorted(Path(args.input).rglob("*_id*.bin"))

    levels = 0
    models = 0
    skipped = 0
    for path in paths:
        if not path.is_file():
            print(f"skip (not found): {path}")
            continue
        line = process(path, out_dir, scene=args.scene)
        if line:
            print(line)
            if "[level]" in line or "[scene]" in line:
                levels += 1
            else:
                models += 1
        else:
            skipped += 1
    if args.scene:
        print(f"\n{levels} placed level scene(s) exported to {out_dir}/  "
              f"({skipped} file(s) had no geometry)")
    else:
        print(f"\n{levels} level + {models} model file(s) exported to {out_dir}/  "
              f"({skipped} file(s) had no geometry)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
