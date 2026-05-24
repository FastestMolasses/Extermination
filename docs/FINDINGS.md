# Extermination — Reverse-Engineering Findings

Technical reference for formats and facts established so far. The authoritative,
exhaustive format details live in the docstrings of the `tools/` scripts; this
file summarises them and records findings that have no other home.

_Last updated: 2026-05-24_

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
    work — likely the engine's per-asset palette table in the boot ELF, or
    a stride/index encoded in the geometry `m0`/`m1` marker bits that we
    haven't decoded yet (`m1>>10 & 0x3FFF` has 21 distinct values per
    level, similar to the per-sheet CLUT count).
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

### Rig / animation

Skinning/animation data is **not** in the geometry record; it lives in
separate files, in two representations.

**Rig / skeleton-transform files.** 23 small (2-4 KiB) files with no MESH
signature — instead a flat array of fixed **0x78-byte (120-byte) records**.
Several are byte-identical across many level regions (a shared rig, presumably
the player). Each record: `[u8x3 flags][u8 bone/joint index]`, a `78 00 04 00`
VIF UNPACK tag, then a 112-byte **VIF-packed transform payload**. The bone
index decodes reliably; the payload decodes to bind-pose matrices in the
cleanest files but a faithful general decode needs the VU1 microcode.
`extract_models.py --rig` dumps each rig file structurally.

**Per-frame vertex animation.** Some characters ship as sets of sibling pose
files — identical topology, differing vertex positions (keyframes). A
`chunk03` character is 11 poses across file ids `0x29`-`0x34`.
`extract_models.py --anim` detects pose sets and exports `*_frameNN.obj`.

Open: the exact VIF-payload transform layout (needs VU1 microcode) and the
bone parent hierarchy.

## `MUSIC.DAT` track listing

`MUSIC.DAT` decodes to 55 tracks. Per the user (cross-referenced with an online
soundtrack listing): **25 are the official soundtrack; the remaining 30 are
cutscene audio.** Not yet separated or labelled — a roadmap item.
