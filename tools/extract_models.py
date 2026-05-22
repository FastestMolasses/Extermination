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

(1) RIG / SKELETON-TRANSFORM FILES -- a family of small (2-4 KiB) files that
    carry NO MESH signature and instead hold a flat array of fixed 0x78-byte
    (120-byte) records. 23 such files are present in `extract/`; several are
    byte-identical across many level regions (a shared rig, presumably the
    player). Layout:

      FILE HEADER
        * "short" form  -- 4 bytes: `<u32 record_count>`. Records start at +4.
        * "long" form   -- 0x20 bytes: `<u32 record_count>` then a `fffe00xx`
                           sentinel and a float vec3 (a root offset / bbox
                           centre). Records start at +0x20.
        The two forms are told apart by where the first `78 00 04 00` tag sits
        (offset 8 for short, 0x24 for long).

      RECORD -- 0x78 bytes, repeated `record_count` (+/-1) times:
        +0x00  u8[3] flags + u8 BONE / JOINT INDEX
               The 4th byte is a small integer that indexes a bone/joint; it
               is constant across a run of consecutive records (several
               records per joint -- keyframes or sub-transforms). The three
               flag bytes take a few discrete values (`00 01`, `01 00`,
               `00 80`, ...) -- a per-record type/interpolation tag.
        +0x04  `78 00 04 00` -- a PS2 VIF UNPACK tag (the engine DMAs these
               records straight to VU1; `0x78` is the UNPACK opcode).
        +0x08  112-byte transform PAYLOAD -- the VIF-unpacked joint transform
               (a matrix and/or quaternion+translation). In the cleanest
               files the payload decodes to exact +/-1.0 / 0.0 entries
               (axis-aligned bind-pose matrices); in general it is VIF-packed
               and a faithful float decode needs the VU1 program, so this
               tool dumps the payload as raw bytes + a best-effort float view
               rather than asserting a matrix layout.

    `--rig` walks every rig file and writes a `*_rig.txt` dump: the header,
    and per record the bone index, flags, and payload (hex + float view).

(2) PER-FRAME VERTEX ANIMATION -- some characters are animated by shipping the
    whole mesh once per pose. In `chunk03` a single 13-block / 1690-vertex
    character mesh appears as 11 sibling files (`id29..id2e`, `id30..id34`)
    with IDENTICAL block count and per-strip topology but different vertex
    positions -- i.e. 11 keyframe poses of one mesh. `--anim` detects such
    sibling groups (same block count + same per-strip vertex counts within a
    region) and exports each group as a numbered OBJ sequence
    `*_frameNN.obj`, ready to load as a morph / vertex-cache animation.

UNCERTAIN / NOT YET RESOLVED (rig)
  * The 112-byte rig-record payload is VIF-packed; without the VU1 microcode
    the exact field layout (matrix vs quaternion+translation, fixed-point
    scale) is not confirmed. The dump is structural and honest about this.
  * The bone PARENT hierarchy is not yet isolated -- the per-record index is
    the joint id, but no explicit parent-index array was identified inside the
    rig files (it may be implied by record order, or live in the model-file
    sub-header). Reported as open.
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


def write_rig_dump(path: Path, rig: RigFile, src_name: str) -> int:
    """Write a human-readable dump of a rig file. Returns the record count.

    Each record is shown with its joint index, flag bytes, and the 112-byte
    transform payload as both hex and a best-effort 28-float view. The float
    view is informational only -- the payload is VIF-packed (see docstring).
    """
    lines: list[str] = [
        f"# Extermination (SCUS-97112) rig / skeleton-transform dump",
        f"# source: {src_name}",
        f"# exported by tools/extract_models.py --rig",
        f"# header ({len(rig.header)} bytes, "
        f"{'long' if rig.long_header else 'short'} form): "
        f"{rig.header.hex()}",
        f"# declared record count: {rig.count}; decoded records: "
        f"{len(rig.records)}",
        "# record = u8[3] flags + u8 bone-index, then a 78 00 04 00 VIF tag,"
        " then a 112-byte VIF-packed transform payload.",
        "",
    ]
    # Joint indices in order -- a quick view of the skeleton layout.
    bones = [r.bone for r in rig.records]
    lines.append(f"# joint index per record: {bones}")
    lines.append(f"# distinct joints: {sorted(set(bones))}")
    lines.append("")
    for i, r in enumerate(rig.records):
        floats = struct.unpack("<28f", r.payload)
        lines.append(f"record {i:3d}  @0x{r.offset:05x}  bone={r.bone:3d}  "
                     f"flags={r.flags[0]:02x} {r.flags[1]:02x} "
                     f"{r.flags[2]:02x}")
        lines.append(f"  payload.hex: {r.payload.hex()}")
        for k in range(0, 28, 4):
            quad = floats[k:k + 4]
            lines.append("  payload.f4[%2d]: %s" % (
                k, "  ".join(f"{x: .5f}" for x in quad)))
        lines.append("")
    path.write_text("\n".join(lines) + "\n")
    return len(rig.records)


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
        name = f"{path.parent.name}_{path.stem}_rig.txt"
        n = write_rig_dump(out_dir / name, rig, f"{path.parent.name}/{path.name}")
        bones = sorted(set(r.bone for r in rig.records))
        print(f"{path.parent.name}/{path.name}: {n} joint records, "
              f"{len(bones)} distinct joint indices -> {name}")
        found += 1
    print(f"\n{found} rig file(s) dumped to {out_dir}/")
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
