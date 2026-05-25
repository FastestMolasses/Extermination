# Extermination — Reverse-Engineering Findings

Technical reference for formats and facts established so far. The authoritative,
exhaustive format details live in the docstrings of the `tools/` scripts; this
file summarises them and records findings that have no other home.

_Last updated: 2026-05-25 — id 0x71 entry "sections" structurally decoded; the three per-bone payload sections are NOT bind-pose matrices (section 1 is per-bone object-space vertex data in the standard 12-byte Q4.12+normal+vid record; sections 2 and 3 are mostly-empty VIF priming headers). Bind-pose matrices live elsewhere. Earlier: per-bone object-space Q4.12 vertex decode confirmed on the 28-bone player rig (`chunk21/f17_id8f.bin`); `extract_models.py --object-space` ships per-bone bind-pose point clouds._

## Target identity

Pinned in `CLAUDE.md`. In short: **SCUS-97112** (USA), boot ELF `SCUS_971.12`,
compiled with **Metrowerks CodeWarrior** (`mwccps2` 2.3.1.01), **no DWARF and
no symbol table** — blind matching, the hardest tier.

## Disc layout

| Path | What it is |
|---|---|
| `SYSTEM.CNF` | boot config, names the boot ELF |
| `SCUS_971.12` | boot executable (EE MIPS) |
| `EXTER.BIN` | exactly 1 GiB — **disc padding/dummy** (`"EXTERMINATION"` + `0xAA` filler). Ignore. |
| `EXTER1.DAT` | an MPEG-PS video (FMV) |
| `DATA/DATA.DAT` + `DATA/INDEX.IDX` | the main asset archive (see below) |
| `OVERLAY/AREA*.BIN` | runtime code overlays, `MWo3` (Metrowerks) format, **19 files** (see below) |
| `MOVIE/*.PSS` | cutscene FMVs (Sony PSS / MPEG-PS) |
| `STREAM/MUSIC.DAT`, `STREAM/VOICE.DAT` | streamed music and dialogue |
| `IRX/*` | IOP-side modules (mostly stock Sony SDK) |

## `OVERLAY/AREA*.BIN` — MWo3 runtime code overlays

**19 overlay files**, IDs 1–19 (AREA05, AREA09, AREA10, AREA12 absent — cut content).
All load to the arena base at **vram 0x00823500**. Full format documentation and
the architectural plan for matching them: see `docs/OVERLAYS.md`.

**MWo3 header** (64 bytes = 0x40):

| Offset | Field | Notes |
|---|---|---|
| 0x00 | magic `MWo3` | 4 bytes ASCII |
| 0x04 | overlay_id (u32 LE) | 1-based; maps to ELF PT_LOAD slot |
| 0x08 | load_address (u32 LE) | always 0x00823500 |
| 0x0C | text_size (u32 LE) | bytes of code in file |
| 0x10 | data_size (u32 LE) | bytes of initialized data in file |
| 0x14 | bss_size (u32 LE) | bytes of zero-init BSS (not in file) |
| 0x18 | static_init_address (u32 LE) | vram of C++ static ctor list start |
| 0x1C | static_init_end_address (u32 LE) | vram of C++ static ctor list end |
| 0x20 | overlay_name[32] | NUL-terminated ASCII, e.g. `Area07.bin` |

**File layout**: `[0x40-byte header][text_size bytes .text][data_size bytes .data]`
Invariant: `filesize = 0x40 + text_size + data_size` — verified exact for all 19 files.

**Text layout**: first 0x40 bytes of .text are always zeros (nop sled / alignment
gap). First function of every overlay is at **vram 0x00823540** (+0x40 from arena base).

**Boot ELF dispatch**: `func_001E7780` (0x4D4 bytes) reads a two-byte area/state ID
from BSS at `D_00810700` and dispatches to one of 17 fixed overlay vram addresses
(hardcoded `jal func_8XXXXXX` instructions in the boot ELF).

**Inspector tool**: `tools/overlay/inspect_mwo3.py` (original code).

**Embedded-asset scan (2026-05-24).** `tools/scan_overlay_assets.py` scans
both .text and .data of every overlay for known asset signatures: `VAGp`,
`SShd`, nested `MWo3`, GS texture DMA packets (`07 XX 00 60` +
BITBLTBUF/TRXREG/IMAGE-GIF), 1024-byte CLUT-shaped blobs, and ASCII runs.
**Result across all 19 overlays: 0 textures, 0 CLUTs, 0 VAGs, 0 sound
banks, 0 nested overlays, 0 GS DMA packets.** The data sections contain
only per-area constants/tables for the area's code. The only non-binary
content is **135 short ASCII strings** (lengths 6–15 bytes; ~75% mixed-
case debug/labels, ~20% ALL_CAPS identifier-like, ~4% source-path-like,
~1% printf format specifiers); AREA04 / AREA13 / AREA19 hold most of
them, AREA00 / AREA01 / AREA02 / AREA06 / AREA18 / AREA20 / AREA22 hold
≤3 each. All real game assets live in `DATA.DAT` / `STREAM/`, not in the
overlays. Strings may still be useful as overlay-function naming hints
(parallel to `tools/decomp/name_functions.py`).

## `DATA.DAT` / `INDEX.IDX` archive

Two-level container, fully validated by exact tiling. Tool: `tools/extract_data.py`.

- `INDEX.IDX` = N sectors of `0x800`; sector *i* is the descriptor for chunk *i*
  (`{id, off, size, flags, ...}`), plus nested sub-chunk blocks of `0x70` bytes
  at `+0x100`, plus a file-entry table (`[u24 offset, u8 file_id]`).
- Top-level chunks + nested sub-chunks form **81 leaf regions** that tile
  `DATA.DAT` exactly; **603 files** tile every region exactly.
- Full layout: see the `extract_data.py` module docstring.

### Asset taxonomy

`file_id` is an **asset-role tag**, not a format tag — content varies by region.
Observed content types: SShd sound banks, float-array geometry/model data,
`07 XX 00 60` GS texture packets, ASCII text (`id 0x80`), tables/padding. A
region bundles one logical entity's pieces (geometry + textures + audio).

## Textures

Textures are PS2 **GS texture-upload packets** in `DATA.DAT` — 28 of them, each
a file beginning `07 XX 00 60`. Tool: `tools/extract_textures.py`.

- The packet is GIF/DMA data: GS register writes (`BITBLTBUF` 0x50, `TRXPOS`
  0x51, `TRXREG` 0x52, `TRXDIR` 0x53) then an IMAGE-mode GIF tag with the payload.
- `TRXREG` gives the transfer region `w x h` in PSMCT32 (32-bit). The real
  texture is **8-bit indexed (PSMT8)**, dimensions `(w*2) x (h*2)` — the
  PSMCT32-page / PSMT8-page footprint ratio is 2x per axis.
- The index data is in PS2 **swizzled** VRAM order. The tool recovers it with
  the documented GS pipeline — write the PSMCT32 transfer into simulated VRAM,
  read it back as PSMT8, using the hardware page/block/column swizzle tables.
  This was verified **byte-identical** to the standard combined `unswizzle8`,
  so the swizzle is provably correct. Results are 512-wide sheets.
- The extractor also finds packets **embedded** in larger `id 0x44` level
  files (57 packets total, vs 28 standalone). A few embedded results look
  noisy — likely false-positive signature matches.
- **Color source — partially resolved (2026-05-24).** PSMT8 always samples
  through a CLUT on GS hardware. Investigation findings:
  - **Standalone texture packets (ids 0x06..0x0c, 0x35, 0x38) carry no CLUT.**
    Every standalone texture file has exactly one BITBLTBUF/TRXREG/IMAGE-GIF
    upload; the tail after the image payload is just sector-alignment zero
    padding. No paired 16x16-PSMCT32 CLUT transfer anywhere in DATA.DAT.
  - **The boot ELF's GS register table contains 9 unique TEX0 templates** —
    8 with `PSM=0 PSMCT32` (rendering target setups) and 1 with `PSM=0x14
    PSMT4 CLD=1` (one static UI / 4-bit palette). No PSMT8 templates appear
    statically; their TEX0 (with CBP/CLD fields) is built at runtime by
    engine code, presumably from per-asset metadata.
  - **Raw 1024-byte CLUT-shaped blobs ARE present in some asset files** —
    not framed as GIF packets, just plain `[R G B A]*256` arrays sitting in
    model / character / level data. Identified in: id 0x6e/0x70/0x72
    character/prop files (1–11 candidates per file, often in pairs spaced
    exactly 0x400 apart), id 0x46 some files, and most id 0x44 level files
    (typically 4–11 candidates near the texture-sheet uploads). The blobs
    look textbook (alpha bytes in [0..0x80], 100+ entries at 0x80 = fully
    opaque, hundreds of distinct RGB triples).
  - **Binding from each blob to its texture is NOT yet known.** Heuristic
    detection (`tools/clut.py::find_clut_candidates`) finds them but
    multiple candidates per file means a per-material mapping needs more
    work — likely the engine's per-asset palette table in the boot ELF.
    **Tested 2026-05-24** the prior hypothesis that `m1>>10 & 0x3FFF`
    indexes CLUTs: **disproved.** That field is a small per-material
    running counter (e.g. {418..423}), often partially shared across
    sheets — not a palette selector. `m1>>15 & 0x3FFF` is constant per
    sheet (a redundant sheet-group tag). **Structurally promising but
    unverified:** `(m0 >> 30) & 0x3` — m0's top two bits take 1-4
    distinct values per sheet, and the count of distinct (sheet, idx)
    pairs across the 32 level files is within ±2-4 of the per-level CLUT
    candidate count for most levels (e.g. chunk04.n2 6↔6,
    chunk04.n0 11↔10, chunk15 8↔8, chunk20.n1 6↔5). Empirical render
    test (chunk04.n2, 6 CLUTs ↔ 6 pairs) was inconclusive: the level's
    in-game palette happens to be near-monochrome (industrial wall
    textures), so most CLUT-cycling outputs look similarly gray; one
    CLUT clearly produces the cleanest coherent atlas. The (sheet, idx)
    → CLUT-offset ordering remains unknown. Not wired into the
    extractors — the structural signal is real but unverified. Next
    step: decompile the boot ELF's PSMT8 TEX0 setup (TEX0 writes with
    PSM=0x13, trace CBP source, find the per-asset palette LUT).
  - **PSMT8 indices are luminance-ordered** (adjacent-index delta ~7-22 vs
    ~85 for a random palette), so the identity grayscale CLUT
    `i -> (i,i,i,255)` is a faithful luminance preview even without the
    correct color palette.
  - **Both extractors now write RGBA PNGs by default** using the identity
    grayscale CLUT (`--clut gray`, the default). `--clut auto` scans the
    texture's own file (and for the per-material extractor, the level
    directory) for a candidate CLUT and applies the first match — useful
    where the binding is unambiguous (one CLUT per file). `--no-clut`
    restores the original 8-bit grayscale PNG output for compatibility.
    `--csm1-swizzle` applies the PSMT8 CSM1 32-entry-block swap before
    indexing (the documented PS2 swizzle for CLUTs uploaded in CSM1 mode).
    See `tools/clut.py` for the recognition heuristic and PNG writers.
- Each decoded sheet is a texture **atlas** — many individual textures packed
  into one sheet, some stored flipped/rotated to pack tighter, plus
  non-texture padding. The swizzle is correct (proven above); the sheet just
  *is* a packed atlas. Cutting it into clean, correctly-oriented individual
  textures needs each texture's UV rectangle from the geometry/draw data.

### Material → texture binding, per-texture extraction

The per-strip geometry marker `m0` carries the texture binding. Its middle 14
bits — `sheet_field = (m0 >> 15) & 0x3FFF` — are a GS VRAM base address. A GS
texture upload is a `BITBLTBUF` (dest pointer `DBP`) + `TRXREG` (`w×h`) pair;
`DBP` and `sheet_field` encode the same address, related by the affine map
`sheet_field = DBP·(2048/1920) − 584.8` (exact on the universal slot trio
`DBP {10752,12672,14592} ↔ sheet_field {10884,12932,14980}`). A material binds
to the transfer whose predicted `sheet_field` matches. `m0` low 15 bits and
`m1` are per-material running indices, not sub-rectangles.

`tools/extract_subtextures.py` uses this: it scans all GS transfers (including
the `07../10` DMA-tag packets `extract_textures.py` misses), binds each
material, and crops its UV bounding box from the bound sheet → 2274 per-texture
PNGs. Limit: ~631 materials reference a texture uploaded by another file
(common/UI packets) — resolving them needs the engine's cross-file VRAM map.

Note: `extract_textures.py`'s signature only matches `07 XX 00 60` packets and
so misses the `07../10` level sheets — broaden it (or consolidate with
`extract_subtextures.py`).

## Audio — VAG ADPCM

All game audio is **PS2 VAG ADPCM** (16-byte frames). Decoder: `tools/decode_sound.py`.

- **SFX**: `SShd` sound-bank containers inside `DATA.DAT` files. A bank holds
  many sounds; each ends on a VAG flag with bit 0 set. `decode_sound.py batch`
  deduplicates across banks → 241 unique sounds.
- **`VOICE.DAT`**: one raw **mono** VAG stream, no container, no end-flags.
  Split into clips on silence gaps. → 116 clips.
- **`MUSIC.DAT`**: one raw **interleaved-stereo** VAG stream — alternating
  **64-frame (1024-byte) L/R blocks**. Deinterleave then decode.
  `decode_sound.py stream --interleave 64`. → 55 stereo tracks.

### Sample rate

Streamed audio plays at **48000 Hz** (PS2 SPU2 native rate). Evidence:
`music/clip_0000` (End Credits) matches an official-soundtrack rip — the rip is
encoded at 48000 Hz, and our clip's frame count / the rip's duration ≈ 48090 Hz.
The rate is **not stored in the audio data** (engine/SPU-pitch parameter).
SFX-bank rate is still unconfirmed (provisionally 22050). Both should be
re-confirmed from the decompiled audio engine.

## Geometry / models

**Level geometry AND character/object/prop models decoded.** Exporter:
`tools/extract_models.py` (geometry file → Wavefront OBJ). Full format details
are in that script's module docstring; summary below.

The game stores 3D geometry in **two related layouts** that share an identical
64-byte vertex record but frame their blocks differently:

- **Level geometry** — the 32 `id 0x44` files. Separator-delimited blocks.
- **Model geometry** — ~330 other files (character / enemy / prop / object
  models, many file ids). Same vertex record; fixed-size padded blocks.

`extract_models.py` now decodes **both**: `*_id44.bin` → level path,
every other `*_id*.bin` carrying the MESH signature → model path. A full
`extract/` run exports **32 level + 328 model** OBJ files (≈1.18 M vertices,
≈684 K triangles total; 243 non-geometry files correctly skipped).

### Level format (`id 0x44`)

3D geometry in the `id 0x44` level files is a sequence of **variable-length
blocks**, not a flat vertex array (the earlier "uniform 64-byte record run"
reading was a coincidence of the first block).

**Block structure.** Blocks are delimited by a 16-byte separator row
`00 00 00 17` + twelve `00`. Each separator is followed by a 16-byte
*descriptor* row naming the block kind (block 0 has no leading separator):

- **MESH** — descriptor ends `04 04 00 01 00 80 80 6c`. Geometry starts right
  after the descriptor.
- **SUBMESH** — descriptor `0X 00 00 00 .. .. .. .. 4d 04 00 00 ..`: a mesh
  with an extra ~0x40-byte sub-header (an index, a vertex count, a bounding
  box). The MESH descriptor reappears inside the block; geometry follows it.
- **MATRIX** — `ff ff ff ff` at descriptor+0x04: a scene-graph /
  instance-placement block — a table of 4x4 transforms followed by the
  object-space geometry they instance. Skipped by the default per-mesh
  export; decoded and applied by `extract_models.py --scene` (see "MATRIX
  blocks and `--scene`" below).
- **FILLER** — all-`0xff` descriptor: padding. Skipped.

**Vertex record — 64 bytes, four 16-byte rows:**

| Offset | Contents |
|---|---|
| `+0x00` | marker: `<u32 m0> <u32 m1>` + 8 zero bytes. m0/m1 are constant within one strip (a per-strip key). **Byte 7 is the strip flag**: `0x00` for a strip's 2 priming vertices, `0x20` for continuation vertices. |
| `+0x10` | vec4 `(u, v, 1.0, 0.0)` — texture coordinates |
| `+0x20` | vec4 — either a **unit normal** `(nx,ny,nz,0)` (dynamic meshes) or a **vertex color** `(r,g,b,1)` with components 0..1 (static/world meshes with baked lighting). Distinguished per-vertex by `|xyz|≈1`. |
| `+0x30` | vec4 world-space **position** `(x, y, z, w)`, `w ≈ ±1.0` |

A real vertex always has `|w| ≈ 1.0`; header/padding rows that share the
64-byte grid do not — this is the reliable validity test.

**Topology — triangle strips.** Consecutive valid vertices form strips; a new
strip begins at a flag-`0x00` vertex following a non-`0x00` one (the
`00 00 20 20 …` priming pattern). N strip vertices → N−2 triangles with
alternating winding. **Degenerate (zero-area) triangles** are the PS2 idiom
for stitching strips into one draw call and are dropped on export.

**Validation.** On `chunk04.n0/f06_id44.bin`: 19271 non-degenerate triangles,
zero degenerate or level-spanning faces, coherent bounding box
(≈ 455 × 162 × 1813 units), unit normals. All 32 `id 0x44` files export to
valid OBJ.

### Model format (character / object / prop files)

The model files carry the **same 64-byte vertex record** and the **same MESH
descriptor** `04 04 00 01 00 80 80 6c`, but the block framing differs:

- The primary block delimiter is a 16-byte separator `00 00 00 14` + twelve
  `00` — note `14`, not the level files' `17`. (`0x17` separators still occur
  in model files, but only as rare section markers.)
- A model block is a **fixed-size, padded** unit (`0x880` bytes in the files
  examined). The 16-byte descriptor row right after the separator is usually
  **FILLER** (`0xff`×16), *not* a MESH descriptor — so the level rule
  "geometry begins right after the descriptor" does not apply. Each block
  carries a short sub-header (a constant `01 00 00 00 .. ..` word pair and a
  bounding box), then the MESH descriptor, then the vertex records.
- Each block holds only a handful of real vertices; the rest of the
  fixed-size block is filled by **duplicating the last real record**.

Because the MESH descriptor reliably anchors the geometry in *both* layouts,
the model path scans for every MESH descriptor, reads 64-byte records from
descriptor+8 until the first invalid (`|w| ≠ 1`) row, then **drops the
duplicated-record tail padding**. Triangle-strip topology is identical to the
level format.

**Validation.** Empirically across the 328 exported model files: ≈905 K
vertices, ≈538 K non-degenerate triangles, ~600 strip-stitch degenerates
(dropped), zero NaN and zero out-of-range faces. Bounding boxes are
model-sized — median max-extent ≈177 units — not level-sized (e.g.
`chunk07.n1/f06_id70.bin` → 90 × 60 × 275; `chunk04.n0/f05_id71.bin` →
50 × 43 × 140). One outlier (`chunk30/f00_id38.bin`) is a flat 2D element
(z-extent 0, x/y ≈ 2000) — likely a billboard/skybox/map plane.

### MATRIX blocks and `--scene` (placed full-level export)

A **MATRIX** block (`ff ff ff ff` at descriptor+0x04) is a scene-graph /
instance-placement block: it holds **object-space geometry plus a table of
4x4 transforms** that place copies of that geometry into the level. The
default per-mesh export skips MATRIX blocks; `extract_models.py --scene`
decodes them and bakes the transforms, writing placed `*_scene.obj` files.

A MATRIX block is one or more **sections**. A section begins at the block
start and at every interior 16-byte separator (`00 00 00 17` *or* `00 00 00
14` form) whose following descriptor row is a **MATRIX descriptor**
`00 00 00 00 ff ff ff ff 00…`. Separators lacking that descriptor are
ordinary geometry-internal delimiters and do not start a section.

Each section is:

1. A **transform table** at section+0x10. Records are **0x50 bytes**:
   `<u32 index>` + 12 bytes, then a **4x4 row-major affine matrix** (16 LE
   floats; rows 0-2 are the 3x4 rotation/scale, row 3 the translation, 4th
   column `(0,0,0,1)`). Record 0's index slot *is* the descriptor row
   (`ff ff ff ff` at +4); records 1..N-1 carry a sequential u32 index and 12
   zero pad bytes. The table ends at the first 0x50-stride slot failing those
   checks (a FILLER row or the geometry sub-header). Tables often include
   repeated identity entries.
2. The **object-space geometry**: a short sub-header — a word quad
   `[w0][w0*130][w2][w0*0x860]` then an axis-aligned bbox — followed by one
   or more MESH blocks decoded exactly like model-file geometry.

`--scene` emits each section's geometry once per transform in its table, with
the matrix baked into vertex positions (and normals rotated). Exact-duplicate
matrices in a table are collapsed to one instance to avoid pure z-fighting
overlays. The regular MESH/SUBMESH geometry — already authored in world space
— is emitted unchanged. The union is a placed full-level OBJ.

**Validation.** Across all 36 `id 0x44` files, **930 transforms** decode;
translations are bounded and level-scale (max component ≈ 1130 units). A full
`--scene` run produces **32 placed level scenes** (≈331 K verts, ≈168 K
triangles) with **zero NaN / infinite / outlier vertices**; the largest
single-scene extent is ≈2739 units — consistent with the level geometry. The
default per-mesh export is byte-identical with and without the new code
(verified by diffing all 360 OBJ outputs); `--scene` is purely opt-in.

**Uncertain.** Two things are unverified without the engine code: (a) the
runtime role of the repeated identity entries (animation slots, LOD, or
unused) — they are collapsed by the duplicate filter; (b) whether a transform
is the final world placement or is composed with a parent node. Decoded
translations are level-scale and bounded, consistent with world placements,
so `--scene` applies them directly. The sub-header word quad is decoded
structurally; `w0`/`w2`'s exact meaning (a count and a sub-mesh/material
index) is unconfirmed and the decoder does not rely on it. The MATRIX
geometry observed so far is self-contained — no MATRIX block was seen
referencing geometry in another file.

**Still uncertain (other geometry).** Material/texture binding: m0/m1 are
constant per strip and clearly key a material or texture page, but the
mapping to the GS texture packets is not decoded — the exporter groups strips
into OBJ objects by (m0, m1) to preserve the grouping. The SUBMESH sub-header
fields are not fully interpreted (geometry is located empirically).
**Skinning / bone data**: no per-vertex bone index or weight was found — the
four record rows are fully accounted for (marker, uv, normal/colour,
position). The animation rig lives in separate files — now located and
partially reversed (see "Rig / animation" below). The model-block sub-header's
leading `01 00 00 00 .. ..` word pair is constant within a file but its
meaning is unconfirmed; the decoder does not rely on it.

### Rig / per-bone collision hulls

Skinning/animation data is **not** in the geometry record; it lives in
separate files. Two representations:

**Per-bone collision-hull files** (formerly mislabeled "rig / skeleton-
transform" files; reclassified 2026-05-24). 22 small (2-4 KiB) files
with no MESH signature — a flat array of fixed **0x78-byte (120-byte)
records**. Several are byte-identical across many level regions (the same
recurring enemy / player). Each record: `[u8x3 flags][u8 bone-index]`, a
`78 00 04 00` VIF UNPACK-tag preset, then a 112-byte payload:

- **Payload bytes 0..16 = `vec4(nx, ny, nz, D)` — a plane equation.**
  `(nx,ny,nz)` is a unit-length outward normal (verified across every
  record of every rig file, |xyz|=1.0 within float epsilon); `D` is the
  signed plane offset. The bone's interior is the intersection of all its
  half-spaces `n·x + D ≤ 0`.
- **Payload bytes 16..112 = six vec4 extras.** Empirically structured
  (face-corner pairs appear with sign-flipped values, consistent with
  edge endpoints of the face polygon), but the exact field layout is not
  yet decoded — needs either the VU1 microcode that consumes these or
  a careful per-OBB empirical match against the recovered face polygons.

Plane sets group by bone index. Most bones have **6 records = 3
antiparallel pairs = an OBB (oriented bounding box)** — verified on
several files including `chunk03/f10_id12.bin` where the three pairs
have dot(n,n') ≈ -1.0 and the recovered extents are bone-scale (~2-12
units). Some bones carry fewer planes (half-spaces / capped hulls).

**Bone parent hierarchy — found in id 0x71 files (2026-05-25).** The
hierarchy lives in a separate file class from the collision hulls.
Several character classes ship a paired set of files:

- id 0x71 (and a small number of related ids: 0x78, 0x84, 0x96, 0x52,
  0x5b, 0x65, 0x7c, 0x11, 0x13, 0x21, 0x27) — a multi-entry animation
  container whose entry header carries the bone PARENT-INDEX TABLE;
- id 0x73 / 0x6f / 0x6e / 0x53 / 0x5c / 0x96 / 0x3a — the per-bone
  collision-hull rig file described above.

An id 0x71 file is a simple offset directory:

| Offset | Field | Notes |
|---|---|---|
| 0x00 | u32 entry_count | usually 1..57; entries are animation clips |
| 0x04 | u32[entry_count] entry_offs | absolute byte offsets in the file |
| ...  | u32 0xffffffff | sentinel terminating the table |

Each entry is self-contained and shares the SAME skeleton (same bone
count and same parent table) as every other entry of the file, so the
hierarchy can be read from entry 0:

| Offset (in entry) | Field |
|---|---|
| 0x00 | u8 bone_count |
| 0x02 | u8 stride (a VIF UNPACK row size, e.g. 0x78) |
| 0x04, 0x08, 0x0c | u32 offsets of three per-bone payload sections within the entry |
| 0x20 | `ffffffff 00000000` (section-table sentinel) |
| 0x28 | u32[bone_count] PARENT-INDEX TABLE |
| ...  | further per-bone u32 tables (bone "kind" / skin counts, not decoded) |

**Validation.** For the player character (chunk05/f04_id71.bin):
- bone_count = 30 (28 valid bones + 2 trailing "non-parent" overshoot
  slots `120, 156` which are dropped);
- the same 28-entry parent array appears in chunk06/f01_id71,
  chunk07/f03_id71, chunk08/f05_id71, chunk11/f00_id71,
  chunk12/f00_id71, chunk20/f01_id71 — same character, same skeleton;
- shape is anatomically coherent: shoulder→elbow→wrist chains
  (5→7→16, 6→8→17), a chest/spine hub (bone 4) with 9 children, a hand
  hub (bone 24) with 5 children = palm + finger tips;
- the collision-hull file for the same character
  (chunk05/f05_id73.bin) has bones `{3, 4, 18, 23, 24, 25}` — every
  bone ID is a valid index in this 28-bone skeleton.

**Caveat.** The table contains one consistent 2-cycle
(`parents[2]=3`, `parents[3]=2`) and two self-parented roots (bones 1
and 24). The cycle is repeatable across every id 0x71 file using this
skeleton, so it is real data — likely the field is overloaded for a
couple of bones (a "linked-pair" / mirror pointer rather than parent),
or one of {2, 3} is the true root and the other is a mirror. The
exporter writes the field as-is; downstream consumers should treat a
strict tree walk as having one cycle.

**Bind-pose matrices — STILL NOT EXTRACTED, but the three id 0x71 entry
"sections" turn out NOT to be bind-pose matrices** (investigated
2026-05-25). Detailed structural decode of the player rig
(`chunk05/f04_id71.bin`, entry 0):

Corrected entry header layout (28-byte header):

| Offset (in entry) | Field |
|---|---|
| +0x00 | u16 bone_count_raw (0x1e = 30; high 2 are overshoot sentinels, real bones = 28) |
| +0x02 | u16 stride (0x78 — a VIF UNPACK row size) |
| +0x04 | u16 `0xffff` then 2-byte 0 |
| +0x08 | u32 section1_off (relative to entry start) |
| +0x0c | u32 section2_off |
| +0x10 | u32 section3_off |
| +0x14..+0x1f | zero padding |
| +0x20 | `ffffffff 00000000` (section-table sentinel) |
| +0x28..+0x98 | u32[28] parent-index table (28 entries, NOT 30 — the
              raw bone_count includes 2 overshoot slots, so the actual
              parent table is 28*4 = 0x70 bytes, ending exactly at the
              first section's offset 0x98) |

Each section starts with a 30-entry inner u32 offset sub-table (120 bytes),
followed by per-bone variable-length payload at those offsets:

- **Section 1** (entry 0: rel 0x98, total 0x1680 bytes = 5760): variable
  per-bone payload (36 bytes for bones 0,1; 36..348 bytes for bones
  2..27; 36 bytes for the overshoot sentinels). Each payload is a stream
  of **12-byte records** with the exact same layout as the per-bone
  object-space vertex format (int16 x/y/z Q4.12 + 4-byte packed normal
  + uint16 vid stepping by 4, with the standard `0000/7700/ffff`
  priming-then-terminator pattern). **This is NOT a matrix table — it
  is per-bone object-space vertex data**, the same content as the
  separate id 0x74/0x8b/0x8f character-mesh files.
- **Section 2** (entry 0: rel 0x1718, total 0x60c bytes = 1548): a
  36-byte stride per bone, EXCEPT bone 0 is 384 bytes. The 36-byte
  records each consist of a single 12-byte payload repeated 3 times
  with the trailing 2 bytes following the same `0000/7700/ffff` priming
  pattern — i.e. these are **empty VIF priming headers** (2 priming
  records + 0xffff terminator, no body). The 384-byte bone-0 record
  expands the same priming header to 32 chunks: 3 priming + 29 data
  chunks whose middle int16s form a smooth animated curve, suggesting
  this is a **per-frame keyframe stream** for a single "root" channel
  (likely root-translation/orientation across the clip's frames).
- **Section 3** (entry 0: rel 0x1d24, total 0x4bc bytes = 1212):
  uniform 36-byte stride per bone (no special bone-0 case). All
  records observed are the same 3x-repeated empty priming header.

Section 2's bone-0 special record, plus the variable-size Section 1
payloads, vary entry-to-entry — the 57 entries in
`chunk05/f04_id71.bin` are **animation clips**, not poses. The
header at +0x04 reads `0xffff` and the parent table is invariant
across entries (verified — same skeleton across all 7 paired player
id 0x71 files), but the per-entry payload differs, consistent with
keyframed clip data, not bind-pose matrices.

**Implication.** The bind-pose / inverse-bind-pose joint transforms are
NOT stored in the id 0x71 file at all. They must live elsewhere — most
likely (a) inferred at runtime from the rest-pose mesh + collision-hull
centres, (b) stored in a still-unidentified header in the mesh files
(ids 0x74/0x8b/0x8f), or (c) baked directly into the VIF microcode
preamble that the engine ships to VU1 before each per-bone vertex
upload. The 4-row matrix loaded into vf01..vf04 in the per-bone
rigid-skinning kernel (#5/#6, #7/#8, #9/#10 family) comes from a
**dmem address**; tracing that back to its EE-side source needs the
DMA/VIF1 dispatcher in the decompiled engine.

Until the engine code reveals the matrix source, the hierarchy +
collision-hull centres remain the practical guide for hand-rigging in
Blender/Maya.

`extract_models.py --skeleton` walks every id 0x71-shaped file and
writes (a) a `*_skeleton.txt` dump (parents, tree, hull centres where
known), and (b) when a matching rig file exists in the same region, a
`*_skeleton.obj` stick figure with joints placed at the collision-hull
centres. 26 skeletons across the game are recognised; the dumps for
the player character mark 6 hulled joints (bones 3, 4, 18, 23, 24, 25),
all other joints are collapsed onto their nearest hulled ancestor.

The geometry vertex record has no per-vertex bone weight/index field
(every byte of the 64-byte record is accounted for: marker / strip
flag, UV, normal-or-colour, position — see "Vertex record" above).
Skinning is therefore **per-bone rigid attachment**: each vertex
belongs to exactly one bone, no per-vertex weights.

**Per-bone rigid attachment — confirmed (2026-05-25).** Character
mesh files (ids 0x70, 0x72, 0x74, 0x88, 0x89, 0x8b, 0x8f, 0xa0, 0x96
and others) store a **two-stage representation**:

1. **VIF prefix** — a stream of small fixed-width quantised vertex
   packets organised **per bone**. Each section is one bone's
   bind-pose vertices, set up to be uploaded to VU1 with that bone's
   joint matrix preloaded; VU1 transforms them and emits world-space
   strips. The per-bone section boundaries are recorded in a **u32
   offset table** sitting in the prefix region, terminated by a zero
   u32. The table's length is the file's bone-section count and
   matches the paired skeleton's bone count for true characters
   (e.g. `chunk17/f14_id8b.bin` and `chunk21/f17_id8f.bin` both have
   28-entry tables matching the 28-bone player skeleton at
   `chunk05/f04_id71.bin`). The packet payload appears to be
   6-byte quantised xyz + u16 vertex-id records with a strictly
   monotonically-increasing vertex id; the engine resets the counter
   between bones. **Fully decoding the packets needs the VU1
   microcode** — they carry no full floats, only Q12-style
   fixed-point or s16-normalised values that VU1 multiplies by the
   bone matrix.

2. **MESH-descriptor section** — the regular post-skinning,
   already-world-space strips decoded by the existing `parse_model_file`
   path. These are what the renderer would draw at bind pose. The
   existing default exporter (and `--scene`, `--anim`) write these
   as-is.

Across the 603 files in `DATA.DAT`, **17 files carry a detectable
per-bone section table** (the prefix u32 table at length 10..40 with
first entry < 0x200, terminated by a zero u32 and ending before the
first MESH descriptor). The remaining model files are static
geometry / single-bone props that the VU1 microcode draws without
a per-bone section table.

`extract_models.py --skinned` walks every model file, detects the
per-bone table where present, and writes (a) a `*_skinned.obj` with
the world-space strips grouped by (m0,m1) sub-mesh key, and (b) a
`*_skinned.txt` recording the table offset, all section offsets,
and (when present) the paired id 0x71 skeleton with a
bone-count-match check. The OBJ is the same world-space geometry
as the default exporter — what's new is the bone-section metadata,
which a downstream tool (Blender) can use as a guide for binding
the mesh to a hand-built rig. Per-vertex weights are not needed:
each VIF section is one bone's exclusive geometry, so re-binding
in DCC is a per-sub-mesh assignment.

**Still unknown.** (a) The VIF packet quantisation format — without
the VU1 microcode the per-bone bind-pose vertex positions can't be
reconstructed in object space. (b) The mapping from section index
in the table to bone index in the id 0x71 skeleton — empirically
the first few small offsets in the table (e.g. 0x5, 0xa, 0xb, 0xc,
0xd, 0xf, 0x10) look like indices into a sub-table rather than
prefix byte offsets, suggesting the format is `[7-byte sub-index
header][bone offsets...]`; needs the VU1 / engine bone-update code
to confirm. (c) Bind-pose joint transforms (still blocked on VU1
microcode, as previously noted for id 0x71 entries).

**Prefix-region structure (investigated 2026-05-25).** The mesh
file's PREFIX REGION — the bytes from file offset 0 up to the
per-bone section table — was investigated as a candidate location
for bind-pose joint transforms. Findings:

- The region's size varies wildly between files of the same
  skeleton: `chunk21/f17_id8f.bin` has an 0x22c8-byte prefix while
  `chunk17/f14_id8b.bin` has a 0x173c8-byte prefix (~94 KiB) — too
  much variation to be a per-bone bind-pose array.
- The region is structured as **32-byte records of two vec4 floats
  each** (verified: byte positions [3] and [7] are the constant
  sign+exponent bytes of floats; bytes [8..11] are always zero —
  the third float in vec4#1 is identically 0.0). The first vec4 is
  per-record-varying; the second is mostly constant across records
  with one slowly-varying float at `+0x18`.
- The values are world-scale (hundreds, e.g. 237.78, 1188.04,
  662.30) and bounded, consistent with positions or AABB extents.
  The `+0x18` field walks monotonically with a step of ~0.15 units
  per record across long runs, with occasional discontinuities.
- The **last 0x40 bytes of the prefix** are not a record — they hold
  a MATRIX-style descriptor (`00 00 00 00 ff ff ff ff ...` at
  prefix_end−0x40), then a 28-element u32 array of small ints, then
  the existing 28-element u32 "section table". The two arrays read
  back-to-back as the existing detector's "section table" hit
  (which is why the section table looked like it began with the
  small ints `5,10,11,12,13,15,16,...`): the real layout is
  `[small u32 × 28][large u32 × 28]`, two parallel per-bone tables
  (probably `[vid_terminator_count][byte_offset]` pairs).

This region is **NOT bind-pose matrices**: there are zero 4×4
matrix-shaped runs anywhere in the prefix of either character file
(scanned by `last_row≈(0,0,0,1) + |row0|≈1`). The most plausible
reading of the float records is **per-bone or per-frame transform
control data driven by VU1**, not a recoverable affine bind-pose
table. Bind-pose joint transforms remain located only inside the
id 0x71 entry sections (themselves VIF/GIF-packed and blocked on
VU1 microcode), or possibly composed at runtime in code.

A future investigation should trace the EE-side function that DMAs
bone matrices to VU1 — VU1 microcode kernel pair #5/#6 (vram
`0x002346b0` / `0x002346f0` helpers, mains at `0x00234610` /
`0x002346f4`) loads a 4-row matrix into vf01..vf04 from a fixed
dmem address — back to func_0011AA50 and its callers.

**Header forms:**
- Short (4 bytes): `<u32 record_count>`. Records start at +4.
- Long (0x20 bytes): `<u32 record_count>`, 8 bytes of default flag /
  `fffe00xx` VIF preset (STMASK/STROW-style), 4 zero bytes, then a vec3
  of floats (an unverified root offset / global hull centre). Records
  start at +0x20.

`extract_models.py --rig` walks every rig file and writes (a) a
`*_rig.txt` dump (plane equations per record, per-bone OBB-pair summary,
raw payload), and (b) a `*_rig_hulls.obj` wireframe of every recovered
OBB (one wire-box per bone with exactly 3 antiparallel plane pairs).

**Per-frame vertex animation.** Some characters ship as sets of sibling
pose files — identical topology, differing vertex positions (keyframes).
A `chunk03` character is 11 poses across file ids `0x29`-`0x34`.
`extract_models.py --anim` detects pose sets and exports `*_frameNN.obj`.

## VU1 microcode (boot ELF static, 2026-05-25)

Located 48 VU1 microcode programs statically embedded in the boot ELF, all
between **vram 0x00230824..0x00240F88** (the data half of the single PROGBITS
section). Tool: `tools/disasm_vu.py` (`catalog` lists them; `disasm <vram>
<size>` runs a partial disassembly; `scan` is a fallback heuristic).

**Packet wrapper.** Each microcode block is wrapped as a complete VIF1 DMA
packet ready to ship straight to VIF1:

```
+0x00:  0x00000000     VIF NOP
+0x04:  0x05000000     STMOD  (mode=0)
+0x08:  0x03000000     FLUSHE
+0x0c:  0x02000000     OFFSET
+0x10:  0x4A NN AAAA   MPG (NN = num qwords, 0=>256; AAAA = imem qword addr)
+0x14:  microcode body, NN*8 bytes (VU1 64-bit instructions, LE)
```

The 4-word setup template is followed by the MPG tag and the microcode body
contiguously, then either another MPG tag (multi-segment program for programs
larger than 256 qwords) or a different VIF1 cmd. Some packets carry leading
STCYCL/STMASK variants instead of the all-zero template — the MPG cmd is
still the reliable signature.

**MPG-tag scan finds 48 programs.** `tools/disasm_vu.py catalog` lists them
all with their packet vram, body size, and imem destination. Highlights:

- **5-segment programs** at imem 0x000/0x100/0x200/0x300/0x400 — these are
  the same logical program (each MPG can carry at most 256 qwords, so a
  large kernel is split). Three such 5-segment sets exist (total per-kernel
  size ~10 KB): packets starting at 0x00237750, 0x00239CC0, 0x0023E8D0.
  These are the most likely candidates for the main character/geometry
  rendering kernel.
- **Single-program uploads** (one MPG, no continuation) target imem 0x0000
  and are 60-256 qwords each — 22 such standalone programs. Likely
  per-effect kernels (particles, decals, projector, environment-mapping,
  etc.).
- **Small 15-qw subroutines** uploaded to imem 0x0800: 6 distinct stubs,
  each 0x78 bytes. These are `bal`-jump targets called from the larger
  kernels (no `vcallms` is actually used anywhere in the boot ELF
  microcode — all subroutine invocation is plain `bal`+`jr`).

**Disassembler v2 (2026-05-25).** `tools/disasm_vu.py` now decodes:

- LOWER primary ops (LQ/SQ/ILW/ISW/IADDIU/branches/jr/jalr/fcand/fcor/fseq/...).
- LOWER special / LOWER1 group: **XGKICK**, **LQI/SQI/LQD/SQD** (auto-inc/dec
  vector load/store), DIV / SQRT / RSQRT / RINIT / RGET / RNEXT / RXOR,
  MTIR/MFIR/MFP, XTOP/XITOP, ILWR/ISWR, WAITP/WAITQ, MOVE.
- LOWER2/3/4 EFU ops (ESADD / ERSADD / ELENG / ERLENG / EATAN* / ESIN / ESQRT / ...).
- UPPER bc-form: ADDbc / SUBbc / MULbc / MADDbc / MSUBbc / MAXbc / MINIbc.
- UPPER primary: MUL / MADD / MSUB / ADD / SUB / OPMSUB / MAX / MINI plus the
  `q` and `i` register variants.
- UPPER special (op 0x3C/0x3D/0x3E/0x3F): MULAbc / MADDAbc / MSUBAbc /
  ADDAbc / SUBAbc / OPMULA / MULAi / MULAq / ADDAi / ITOF{0,4,12,15} /
  FTOI{0,4,12,15} / ABS / CLIP / NOP.
- The I-bit imm32 follower (`<imm32 I = ...>`).
- Dest mask printing (`.xyzw`, `.xy`, etc.) and flag column (`I/E/M/D/T`).

Still placeholder (rare or non-canonical sub-encodings): a few `lspec_xxx`
and `upp_xx` opcodes that show up sparsely (<5% of body for any kernel) —
none affect classification. The byte-order convention is now empirically
nailed down: **bytes +0..3 = LOWER pipe word, bytes +4..7 = UPPER pipe word.**

**Per-program op profile (`disasm_vu.py profile`).** The 48 packets group
into **25 logical kernels** (some kernels are split across 2-5 MPG segments
because each MPG carries at most 256 qw). Every kernel has at least 1
XGKICK — confirming the decoder is correct. Highlights:

- **3 big 5-segment kernels** (#14 0x00237750, #15 0x00239CBC, #24 0x0023E8CC),
  ~1183-1235 qw, 3 XGKICKs each, 140 LQ / 74 SQ, very high int-alu (166-168) +
  branch (89-92) + `bal` use. These are the **world/level-geometry
  renderers** with three sub-paths (likely opaque / alpha-test / additive).
- **4 medium 2-segment kernels** (#0/1/2/4, ~322-382 qw, all imem 0x0000)
  sharing a near-identical op signature: 26-41 mul*, 4 madd*, 6-8 msub*,
  13-19 add*, 10-16 sub*, **17-20 ftoi/itof**, 1-2 XGKICK, **53-66 LQ /
  20-29 SQ**, 36-38 branches. Highest ftoi/itof+LQ density of any group —
  the dequantize-and-transform signature of a **skinned-mesh renderer**.
  Variants 0/1/2/4 are likely opaque vs alpha vs additive vs env-mapped.
- **3 paired main+helper kernels** (#5+#6, #7+#8, #9+#10): each is a
  ~145-168 qw main at imem 0x0000 plus a 15-qw helper at imem 0x0800.
  The mains have 1 XGKICK, 4 ftoi/itof, 25 LQ / 4 SQ, 9 branches.
  These look like **per-bone rigid-skinning kernels** (lean: transform
  one batch of bone-anchored vertices, no animation interpolation), with
  the imem-0x800 helper functioning as a per-vertex inner transform.
- **Standalone setup/effect kernels** (#3, #12, #13, #16-22): 62-138 qw
  single-segment programs, low mul/madd, often 1 XGKICK or none. Likely
  particle / decal / env-map / sprite / projector kernels.

**Most likely skinning kernel: the #5+#6 / #7+#8 / #9+#10 family.**

Pseudocode reconstruction of the main-routine pattern (~150 qw):

```
init:
    xtop  vi01           ; read VIF UNPACK top-of-buffer marker
    iaddiu vi02, vi00, 1 ; vertex count guard
    ; load 4-row bone matrix from a fixed dmem offset into vf01..vf04
    lq    vf01, BONE+0(vi00)
    lq    vf02, BONE+1(vi00)
    lq    vf03, BONE+2(vi00)
    lq    vf04, BONE+3(vi00)

per_vertex_loop:
    lqi   vfV, (vi01++)        ; load 1 quantized vertex qword from VIF stream
    bal   imem_0x800           ; -> 15-qw helper: dequantize + transform
    sqi   vfV, (vi02++)        ; write transformed vertex to output buffer
    iaddiu vi03, vi03, -1      ; decrement count
    ibne  vi03, vi00, per_vertex_loop

emit:
    ; write GIF tag preamble
    sq   gif_tag, OUTBUF+0
    xgkick  vi02               ; kick the assembled packet
end:
    nop ; [E]
    nop
```

The 15-qw helper at imem 0x0800 (5 mul*, 2 ftoi/itof, 8 LQ) is the
dequantize+transform inner kernel:

```
xform_one_vertex:
    ; vfIN already has the raw quantized qword loaded by caller
    itof12  vfTMP, vfIN         ; treat as Q4.12 fixed-point -> float
    mulAi   ACC,   vf01, vfTMP.x ; bone row 0 * x
    maddAi  ACC,   vf02, vfTMP.y ; + bone row 1 * y
    maddAi  ACC,   vf03, vfTMP.z ; + bone row 2 * z
    madd    vfOUT, vf04, vfTMP.w ; + translation row (w as 1.0 sentinel)
    ftoi0   vfINT, vfOUT        ; convert to GS integer coord
    jr      ra
```

**This pseudocode is INFERRED from the op-frequency profile, not a
byte-level decode.** The encoding edge-cases around `mulAi` / `maddAi`
modifier (i-reg loaded via the preceding I-flag) make a fully literal
trace of the 15-qw helper require nailing down the I-bit sequence — left
for follow-up.

**Per-bone object-space vertex format -- CONFIRMED (2026-05-25).**
Each per-bone VIF section consists of a 28-byte priming header (two
12-byte priming records + a 4-byte 0xffff terminator) followed by a
stream of uniform **12-byte vertex records** terminated by another vid
sentinel `0xffff`. The body layout is:

    +0x00  int16  x       Q4.12 fixed-point (dequant: raw / 4096.0)
    +0x02  int16  y       Q4.12
    +0x04  int16  z       Q4.12
    +0x06  4 bytes        packed normal / lighting / VIF padding (not yet decoded)
    +0x0a  uint16 vid     monotonically-increasing vertex id (steps of 2)

The Q4.12 dequant comes straight from the **ITOF12** instruction in the
15-qw helper at imem 0x0800 invoked by the per-bone rigid-skinning
kernel pair (#5/#6, #7/#8, #9/#10) at vram 0x00234610 / 0x002346F4.

**Validation (player rig `chunk21/f17_id8f.bin`, 28-bone skeleton).**
- 28 bone sections in the prefix table (matches the 28-bone player
  skeleton at `chunk05/f04_id71.bin`);
- bones 0..5 are tiny index-only sections (1..5 bytes, no records);
  bones 6..27 carry between 3 and 131 records each;
- TOTAL 2196 unique object-space vertices vs. 4049 unique world-space
  positions in the existing `--skinned` OBJ -- the world-space count is
  larger because the post-stripification mesh duplicates shared verts
  across strip-restart pairs and across (m0,m1) sub-meshes; same order
  of magnitude as expected;
- per-bone bboxes are bone-scale (0.5..16 units across, never exceeding
  the Q4.12 saturation range of +/-8) and anatomically plausible:
  bone 6 (3v at +1.7,0,+2.0) reads as a small attachment; bones 22/23
  (centred at +4.25 x) and bones 14/24..27 (centred at -2.6..-3.7 x)
  form left/right mirror-pairs consistent with a humanoid skeleton;
  bones 12/13/17/18 carry large y/z extents matching the spine/torso.

The 4-byte field at +0x06 is signed-byte-like (alternating positive /
negative components, components in [-127, 127], magnitude often <1) and
correlates smoothly across consecutive vertices -- almost certainly a
packed normal or vertex-colour, but the exact format is not yet
decoded. The decoder writes only positions.

Object-space export wired into `extract_models.py --object-space`:
writes a `*_objspace.obj` (one `o bone_NN` group per bone, points only,
no faces -- the stream is pre-stripification quantised vertices) plus a
`*_objspace.txt` summary with per-bone vertex counts and bboxes.

**What this unblocks (status of the three blockers).**

- **id 0x74 object-space character vertices**: RESOLVED (2026-05-25).
  Q4.12 dequant confirmed empirically on the 28-bone player rig
  (`chunk21/f17_id8f.bin`): 12-byte records of `int16 x/y/z (Q4.12) +
  4-byte packed normal + uint16 vid`. Per-bone vertex counts and bboxes
  are anatomically plausible. Wired into `extract_models.py
  --object-space`. See "Per-bone object-space vertex format" section
  above.
- **id 0x71 bind-pose joint transforms**: probably handled by one of the
  small standalone kernels (#3 or #19) given their setup-shaped profile
  (high int-alu, low XGKICK, no ftoi/itof). Still needs a targeted walk.
- **PSMT8 TEX0 CLUT binding**: confirmed unaffected by VU1 microcode —
  needs EE-side draw-setup decompilation as previously noted.

### Bind-pose matrix EE-side trace (2026-05-25, partial)

Followed up the FINDINGS hypothesis that the EE-side bone-matrix DMA
would lead back from kernel-pair #5/#6 via `func_0011AA50` →
`func_0011AB60` (ctc2+vcallmsr dispatcher). Two concrete results:

**1. VU1 dmem source of the bone matrix — RESOLVED.** The skinning
kernel main at vram `0x00234610` loads its 4-row matrix into
**vf28..vf31** from **VU1 dmem qwords `0x000..0x003`** (bytes
`0x0000..0x003F`), via four absolute-address LQ instructions
`lq vfNN, K(vi0)` with K in {0,1,2,3} (LQ immediate is 11-bit
**unsigned**, not sign-extended — corrected from initial reading).
Additionally vf16..vf21 are loaded from the **top of dmem**
(qw 0x3F8, 0x3FA, 0x3FB, 0x3FD, 0x3FE, 0x3FF) — six quadwords of
shared constants/state (clip planes, near/far, light dirs, etc.).
The VIF1 wrapper just before kernel #5 (vram `0x002345f8..0x00234610`)
sets `BASE=0x0020` and `OFFSET=0x0190`, confirming VU1 dmem layout:

- **qw `0x000..0x01F`** — fixed shared region (matrix + state).
  UNPACKed with absolute-address mode (FLG=1).
- **qw `0x020..0x01AF` / `0x01B0..0x033F`** — double-buffered per-batch
  vertex stream (BASE+OFFSET).
- **qw `0x3F8..0x3FF`** — top-of-dmem shared constants.

**2. EE-side caller chain — NEGATIVE RESULT.** The hypothesised
`func_0011AA50` / `func_0011AB60` cluster (and its siblings
`0011A9F0`, `0011AB20`, `0011AB70`) **has no callers anywhere**:

- No `jal` to any of those addresses anywhere in the boot ELF
  (brute-force u32 scan of `0x300..end`).
- No `lui`+`addiu`/`ori` pair computing any of those addresses
  (function-pointer load) anywhere in the boot ELF.
- Same for all 19 area overlays — zero references.
- Same for the entire VIF1 helper library at vram
  `0x0011BA00..0x0011BCF8` (`func_0011BA00` VIF status,
  `func_0011BA10` VIF op status, `func_0011BA60` VIF1 register
  snapshot to 0x40-byte struct, `func_0011BC38` VIF1-FIFO drain
  loop targeting `0x10005000`, `func_0011BC98` VIF1-FIFO read).
  Every one of these functions is statically unreferenced.
- The `ctc2 vi27` + `vcallmsr` instructions appear exactly **once
  each in the whole boot ELF** (inside `func_0011AB60`); since
  AB60 is unreferenced, **`vcallmsr` is effectively unused** — VU1
  programs are kicked by VIF1 `MSCAL`/`MSCNT` tags in DMA chains,
  not by EE-side `vcallmsr`. This matches the earlier finding that
  no microcode uses `vcallms` either.

**Interpretation.** The AA50/AB60/AB70/AB20/BA00..BCF8 cluster is
**dead-stripped library code** — Sony-SDK / handwritten VPU/VIF
helpers that the linker pulled in but no compiled code calls. The
real engine inlines equivalent logic (or builds DMA tags directly).

**3. The actual EE-side VU1/VIF1/GIF DMA pipeline.** Surveyed every
`lui 0x1000`+`addiu/ori` pair in the boot ELF that resolves to an
EE MMIO address. All hardware accesses concentrate in **boot/init
code**, not overlays:

- **Zero overlay code touches VIF1/GIF DMA or VIF1 FIFO** — verified
  for all 19 area overlays across the relevant ranges (`0x10005000`,
  `0x10009000-0x100090FF`, `0x1000A000-0x1000A0FF`, `0x10003C00`).
- **All VIF1 DMA submissions live in `func_00100A60`** (vram
  `0x00100A60`, size 0x314) and **`func_00100EB8`** (vram
  `0x00100EB8`, size 0x68C — 419 instructions). They write
  MADR/QWC/CHCR of channel 1 (`0x10009010/20/00`) and channel 2
  (GIF, `0x1000A000`); the giant `func_00100EB8` carries 5 separate
  VIF1 DMA submissions plus 5 raw FIFO writes to `0x10005000`,
  interleaved with VIF1_STAT polls — this is the **VU1
  microcode-upload / setup driver** invoked at game-init time
  (probably builds the per-program DMA chains that ship the 48
  microcode packets cataloged earlier).
- **VIF1 STAT poll** (`0x10003C00`) is the universal sync primitive
  (16 sites in the boot ELF).

**Implication for the bind-pose hunt.** The per-frame bone-matrix
construction + DMA dispatcher is **NOT in any function the static
disassembly can reach from a named entry point**. The most plausible
remaining path is:

1. Per-frame engine code (likely in `func_001E7780` area-state
   dispatcher's call tree, or one of the per-frame draw functions)
   builds a VIF1 DMA chain in EE scratch RAM containing
   `UNPACK V4-32 num=4 dest=0 (absolute)` for the matrix +
   `UNPACK ... dest=BASE+OFFSET` for the per-bone vertex stream +
   `MSCAL imem=0x0000` to kick the kernel.
2. The chain head is written to VIF1_TADR (or MADR) and CHCR is
   started — but this happens via a **runtime-set function pointer**
   in BSS, populated during init; static cross-reference finds
   nothing.
3. The matrix data itself is constructed per-frame from the bone
   hierarchy + animation clip data (from id 0x71) by code that
   composes parent-relative transforms.

**Next step (NOT done this session).** Reverse-engineer
`func_00100EB8` first to confirm it is the static VU1 init / packet
shipper, then walk its callees and the area-state dispatcher
(`func_001E7780`) to find the per-frame draw entry point. The bone
matrix builder will be reachable through that chain. Practical
shortcuts: (a) instrument PCSX2 to log writes to VIF1 FIFO
`0x10005000` and VIF1_TADR with a per-frame breakpoint, capturing
the DMA chain bytes the engine actually ships — the matrix UNPACK
to dest=0 will be in there; (b) decompile `func_00100EB8`
(419 instructions, all `lui`+`ori` MMIO addressing — large but
mechanical).

**Verification status.**
- VU1 dmem matrix source qw 0..3 — confirmed by direct disassembly
  of kernel #5.
- EE-side caller chain back to a named function — **not found**;
  cluster AA50/AB60/etc. is dead code; live VU1 dispatch goes
  through `func_00100EB8` (unanalysed in detail).
- Source data struct for bone matrices — still unknown. Best guess
  remains runtime construction from id 0x71 animation clip data
  (section 1 quantised per-bone vertices + section 2 root-channel
  keyframes) composed with the parent table.

**No tool deliverable this session** — the trace stops at an
unanalysed 419-instruction VU1 driver function. `extract_models.py
--rigged` cannot be wired up until the matrix source is identified.

### Bind-pose hunt via PCSX2 GS dump — format dead-end (2026-05-25)

Tried to recover bone matrices from a PCSX2 GS dump of one live
gameplay frame (`Extermination_SCUS-97112_20260525055057.gs.zst`).
Decompressed (zstd) and parsed with new `tools/parse_gsdump.py`.
Successfully decoded the PCSX2 "new" dump container (magic
`0xFFFFFFFF`, `GSDumpHeader` then state-blob then 8192-byte regs
then a packet stream with id ∈ {Transfer, VSync, ReadFIFO2,
Registers}).

The capture contains 4 vsyncs, each with **706 Transfer packets,
all on `PATH1new`** (=3, GIF FIFO from VU1 via XGKICK). Each frame
has 522 PRIM=TRISTRIP and 25 PRIM=SPRITE GIFtags, all FLG=PACKED.
Zero PATH2 (VIF1→GIF passthrough) and zero PATH3 (EE GIF DMA)
transfers. Total packet stream ~4.3 MB; state blob 4.00 MB
(≈4 MB GS local memory + GS regs).

**Conclusion: a PCSX2 GS dump is not the right capture format for
this question.** It contains only:

- The GS local memory snapshot (4 MB framebuffer/texture memory).
- The GS register set.
- The post-VU1 GIF stream (PATH1 from XGKICK / PATH3 from EE).

It does **not** contain:

- EE main memory (32 MB).
- VU0/VU1 micro/data memory.
- The raw VIF1 packet stream — by the time GIF data is captured,
  VU1 has already consumed the VIF1 UNPACKs and applied skinning,
  so the bone matrices have been folded into world-space vertex
  positions and are unrecoverable from PATH1.

The bone-matrix UNPACK described in the task (V4-32, num=4, dest=0)
is a VIF1 packet, not a GIF packet, and it leaves no trace in the
GS dump. Even the post-skin vertex stream on PATH1 doesn't let us
recover matrices: each strip's vertices are already in clip space,
mixing the per-bone matrix with the per-bone bind-pose vertex
data, and inverting that without knowing the bind pose is the same
chicken-and-egg problem we started with.

**What would actually work (recorded for next session):**

1. **PCSX2 save state.** A full save state freezes EE RAM + VU0/VU1
   memory + VIF state + IOP. Loading it in a custom build of
   PCSX2 with a one-shot logger on `dmacWrite(VIF1_CHCR ...)` or
   on FIFO writes to `0x10005000` would capture the next frame's
   VIF1 DMA chain bytes (matrix UNPACK + vertex UNPACK + MSCAL).
   The EE source address is the chain MADR at the moment of kick.
2. **PCSX2 patch (best path).** Add ~30 lines to PCSX2's VIF1
   handler logging every UNPACK with cmd, dest, num, format, and
   the source EE address (MADR/TADR), dumped to a text file. Run
   one frame; grep for `dest=0 num=4 fmt=V4-32`; cross-reference
   against EE RAM dumped via PCSX2's memory tools.
3. **Static decomp of `func_00100EB8`.** 419 instructions, all
   `lui`+`ori` MMIO addressing — large but mechanical. Will
   reveal the VU1/VIF1 driver and unlock the call graph.

`tools/parse_gsdump.py` is left in the tree as a usable PCSX2
GS-dump reader (header decode, packet stream walker, GIFtag
decoder for PACKED/REGLIST/IMAGE) for any future GS-side
investigation (texture/PSM analysis, draw-call counting, etc.) —
it just isn't the right tool for the bind-pose question.

## `MUSIC.DAT` track listing

`MUSIC.DAT` decodes to 55 tracks. Per the user (cross-referenced with an online
soundtrack listing): **25 are the official soundtrack; the remaining 30 are
cutscene audio.** Not yet separated or labelled — a roadmap item.
