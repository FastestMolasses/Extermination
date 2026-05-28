# Extermination — Reverse-Engineering Findings

Technical reference for formats and facts established so far. The authoritative,
exhaustive format details live in the docstrings of the `tools/` scripts; this
file summarises them and records findings that have no other home.

## glTF (.glb) export of the player rig (2026-05-27)

`tools/export_gltf.py` bundles a character mesh + id 0x71 skeleton/animation
file into a single glTF 2.0 binary. Output: one scene; one node per bone
plus a scene-root parent holding the parent-table roots as children; one
**TRIANGLES-mode** mesh per non-empty bone with **POSITION + NORMAL**
attributes and an index buffer (see "Per-bone triangle topology" below);
one glTF animation per id 0x71 clip entry with paired rotation (VEC4
quat) and translation (VEC3) samplers per bone.

### Per-bone triangle topology (decoded 2026-05-27)

The per-bone VIF vertex stream (`extract_models.decode_objspace_bone_vertices`)
is a **generalized triangle strip** keyed by the per-record `vid` field at
+0x0A. Within one strip every adjacent vertex pair has `delta_vid == +2`
(the engine reserves the odd-vid parity for an internal flag; every stored
vid in our corpus is even). **Any non-+2 delta signals a strip restart** --
observed restart deltas include +1, +4, +5, +7, +9, +17, +19, +43, +64, +90.
The +64 delta is the most common -- likely the engine's "advance to next
sub-mesh strip base" tag -- but every irregular delta is treated as a
restart by the decoder.

`export_gltf.triangulate_bone()` splits each bone's vertex list into
strips on irregular deltas, drops strips with fewer than 3 verts, and
emits PS2-standard alternating-winding triangles per strip (`(i, i+1,
i+2)` swapping to `(i+1, i, i+2)` on odd index). Coincident-position
triangles -- the strip-stitching degenerates -- are skipped.

Per-vertex normals are produced by **face-area-weighted averaging** of
the surrounding triangles (cross-product weighting in
`_face_averaged_normals`). The packed 4-byte field at vertex-record
+0x06 was inspected empirically (signed-byte / 127, IEEE float, unsigned
bytes / 255) -- none gives consistently unit-length vectors, so the
exact quantisation needs the VU1 microcode decode. The 4th byte
clusters tightly around 63 in some bones and around 188 in others,
suggesting it is a category/intensity tag rather than a coordinate.
Smooth face-averaged normals are good enough for preview shading; a
faithful packed-normal decode can replace them later.

Rigid attachment is achieved by parenting the per-bone mesh directly to its
bone node — no glTF `skin` object is required, because each vertex belongs
entirely to one bone with implicit weight 1.0. The bone node's TRS animates
under the runtime, dragging the rigid mesh with it.

Time conversion: the keyframe `t_next` field is treated as a frame index at
30 fps. Quat sampler interpolation is `LINEAR`; compliant runtimes
re-normalise after lerping, which matches `anim_decoder.sample_bone`'s
NLERP behaviour. Frame-0 default TRS on each bone is sampled from clip 0
(the canonical bind-ish pose), so the file looks correct even before any
animation is selected.

Validation on `extract/chunk21/f17_id8f.bin` + `extract/chunk05/f04_id71.bin`:
~1.98 MB .glb, 30 bones, 22 non-empty meshes (19 TRIANGLES + 3 POINTS
fallback for 3-vert bones with irregular vid deltas), **1739 triangles**
across 2196 object-space vertices, 57 animations, 3 420 sampler tracks,
33 562 keyframe samples. The structure round-trips through `pygltflib`
(strict parser); every bufferView lies inside the buffer, every accessor
count × stride fits its bufferView, every sampler input is strictly
time-monotonic, every quaternion output sample is unit-norm, every
NORMAL sample is unit-norm within float epsilon.

Known limitations (do not block animation preview): no UVs / no texture
binding (the per-bone VIF records carry no UV field -- texturing data
lives in a separate stream not yet located); the packed 4-byte
normal/lighting field at vertex-record +0x06 is not decoded so normals
are derived from face geometry; 3 trivial bones (3 verts each) fall back
to POINTS mode because their vid deltas form no valid strip.

### UV + texture binding for the character (2026-05-27)

Character mesh files (e.g. `chunk21/f17_id8f.bin`) carry the UV data not
in the per-bone VIF stream but in a **second**, model-format
MESH-descriptor section starting partway through the file (offset 0xe848
in the player mesh). This section is decoded by the EXISTING
`extract_models.parse_model_file()` -- 317 fixed-size 0x820-byte blocks,
each with the standard 64-byte vertex record carrying:

    +0x00  u32 m0, u32 m1                  marker / per-strip key
    +0x10  vec4 (u, v, 1.0, 0.0)           UV coordinates (floats)
    +0x20  vec4 normal_or_color            unit normal for dynamic meshes
    +0x30  vec4 (x, y, z, w=+/-1)          position (bone-local, small)

Across the player mesh: 105 distinct (m0,m1) keys, ~10000 vertices,
~6900 triangles. The per-strip `sheet_field = (m0 >> 15) & 0x3FFF`
already documented for level materials applies identically here: the
three values {10884, 12932, 14980} resolve to GS VRAM bases
`DBP = {10752, 12672, 14592}` -- the **universal character sheet trio**.

The first two sheets (DBP 10752, 12672) are uploaded by sibling files
in the same chunk dir as the mesh (`chunk21/f00_id43.bin`,
`chunk21/f01_id44.bin`); the third (DBP 14592) is uploaded by per-level
`id 0x44` files (the closest match in the dataset is
`chunk04.n2/f00_id44.bin`). `tools/export_gltf.py` now embeds all three
sheets as RGBA PNGs (identity grayscale CLUT) referenced by glTF
Materials with `baseColorTexture`.

**Unresolved.** (1) Per-bone↔MESH-block binding: the block headers
carry a bbox but no explicit bone index, and the MESH-block positions
do not match the per-bone VIF positions (different coordinate frames).
Without the bone binding the static-textured mesh appears collapsed
near the origin. Candidate: the "small u32 x 28" table at the end of
the file's prefix region (immediately before the per-bone section
table) -- size matches the bone count, but its semantics are not yet
decoded. (2) Whether the per-bone VIF stream has parallel UVs shipped
via another VIF UNPACK (currently the per-bone records are 12 bytes
each and fully accounted for as POSITION+packed_normal+vid).

## Python bind-pose evaluator (2026-05-27)

`tools/extract_models.py` exposes `bind_pose_at_t(id71_path, entry_idx, t)`
which parses an id 0x71 entry, samples the per-bone rotation and translation
streams at `t` (frames) via `anim_decoder.sample_bone`, builds local `T*R`
column-major affines, and composes world matrices through the entry's
+0x28 parent table (cycle-safe). The `--rigged` mode now defaults to
evaluating the auto-paired skeleton at entry 0 / t=0 with no PCSX2 save
state required; legacy `--bones JSON` and the new `--from-id71
PATH:ENTRY:TIME` flags coexist.

Validation: `chunk21/f17_id8f.bin` poses to 2196 world-space verts (matches
the `--object-space` per-bone sum) at humanoid scale (~24x21x23 units),
recognisable silhouette with mirrored L/R leaf bones and the high-X finger
chain on bones 21..23. The pose is animation-clip-evaluated, not a true
rest pose, so limb overlap is expected; the runtime per-bone scale
(`+0x7C`/`+0x88`) is not stored on disc and defaults to identity here.

## Anim decoder VALIDATED (2026-05-27)

`tools/anim_decoder.py` was empirically validated against the player rig
`extract/chunk05/f04_id71.bin` entry 0:

- All decoded quaternion magnitudes ≈ 1.0 (within float-truncation precision)
  across every keyframe of every bone with rotation data — confirms the
  20-bit-of-float packing format and the per-bone directory layout.
- Bones 0 and 1 are identity quats `(0, 0, 0, 1)` held for the full clip —
  consistent with skeleton-root bones that don't animate.
- Bones 2-5 show smooth quaternion interpolation with small frame-to-frame
  deltas (`|Δq|` < 0.05 between adjacent keyframes), monotonically increasing
  `t_next` values.
- The last keyframe of each stream has `t_next = 0xFFFF` — confirms the
  end-of-stream sentinel hypothesis.
- Entry 0 of f04_id71 has 30 bone-stream entries; bones 0/1 hold rest pose,
  bones 2-29 are the live animation channels. (28 of these align with the
  real player-bone count; 2 are the documented sentinel overshoots.)

The complete bind-pose data flow from disc to VU1 is now CHARACTERIZED and
PYTHON-DECODABLE end to end:

```
disc id 0x71 entry (per-bone directory + 12-byte-stride keyframe streams)
    -> anim_decoder.parse_rotation_section / parse_translation_section
    -> sample_bone(frames, t)  -- NLERP between adjacent keyframes
    -> per-bone struct +0x30/+0x40/+0x50 (quat A, quat B, blend t)
    -> func_001C6DA0 (NLERP + TRS + parent concat) -> bone+0x90
    -> func_00179BC0 publisher -> BSS 0x002863XX..0x002893XX
    -> DMA via func_00101FE0 -> VIF1 -> VU1 dmem -> vf01..vf04
    -> per-vertex transform -> XGKICK -> GS
```


_Last updated: 2026-05-25 — live bone matrices located in EE .bss via PCSX2 save state (`tools/parse_pcsx2_state.py`): the engine stages column-major 4x4 affine matrices in `0x002863XX`..`0x002893XX` as two pairs of {world,local} 21-matrix runs (double-buffered character rigs). The matrices are not stored verbatim on disc → bind pose is built at runtime. See "Live bone matrices in EE RAM" below. Earlier: id 0x71 entry "sections" structurally decoded; the three per-bone payload sections are NOT bind-pose matrices (section 1 is per-bone object-space vertex data in the standard 12-byte Q4.12+normal+vid record; sections 2 and 3 are mostly-empty VIF priming headers). Earlier: per-bone object-space Q4.12 vertex decode confirmed on the 28-bone player rig (`chunk21/f17_id8f.bin`); `extract_models.py --object-space` ships per-bone bind-pose point clouds._

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
  **Section 3 also carries the per-clip EVENT TABLE** appended after
  the priming block — see "Section 3 / event table" below.

### Section 3 / event table (DECODED 2026-05-27)

Section 3's full layout is now characterized:

```
+0x000  30 * u32 inner offset sub-table            (0x78 bytes)
+0x078  30 * 36-byte empty VIF-priming records     (0x438 bytes)
+0x4b0  EVENT TABLE:
          s16 count
          s16 reserved (0)
          count * { s16 frame, u16 flag }
          0xFFFFFFFF sentinel(s)
```

The 0x4b0 header offset is **invariant across every id 0x71 entry**
seen (30 bones * 36 bytes + 0x78 directory = 0x4b0 exactly). The
event table is per-CLIP (per-entry), not per-bone — a single linear
list of `(frame, flag)` pairs that fire when the clip time crosses
each frame index.

Consumer: `func_001C64F0` (per-frame time advancer). The function
calls `func_001C8480` (the clip resolver, which caches
`D_00275BF8` = entry header pointer, `D_00275BEC` = section3 base),
then reads the entry header's u32 at offset +0x14. When non-zero
this word is a fast-path pointer to the event table head (= `s3_off
+ 0x4b0`); when zero, the entry has no events and the loop is
skipped.

Per-frame walk (annotated, in C):

```c
clip_state* hdr = D_00275BF8;
int evt_off = hdr->u32[+0x14];        // 0 => no events
if (evt_off && !(bone->flags & 0x8000)) {
    s16* tbl   = (s16*)((u8*)hdr + evt_off);
    s16  count = tbl[0];                       // s16 count
    u8*  recs  = (u8*)tbl + 4;                 // skip count+pad
    int  ft    = (int)bone->time_frames_f32;   // bone+0x3C
    for (int i = 0; i < count; i++) {
        s16 frame =  *(s16*)(recs + 4*i + 0);
        u16 flag  = *(u16*)(recs + 4*i + 2);
        if (frame == (s16)ft) {
            status |= flag;            // OR into low 16 bits of return
            break;                     // FIRST-hit only, one event/frame
        }
    }
}
```

The OR-accumulator that holds the flag is also where the clip-state
machine writes its control bits (`0x1000`, `0x3000`, `0x4000`,
`0x8000`) on end-of-clip and looping transitions — so the function's
return word is a 16-bit packed `{ high 4 bits: state, low 12 bits:
event-flag OR-merge }` carried back to the bone driver.

**Cross-clip stats.** 400 id 0x71 entries scanned across all rigs;
35 entries carry a non-empty event table (77 events total). **Every
flag value observed is `0x0009`**, suggesting a single dominant
event type (likely a footstep / sound trigger) on the player's
locomotion clips. The 5 entries on the player rig with events are
7, 8, 9, 10, 32, with 2/1/2/4/2 events at frames {22,57}, {7},
{6,29}, {18,27,41,60}, {12,32} respectively — all sparse, monotonic
within a clip, and clip-relative. Other rigs/clips may exercise
other flag bits not present in this dataset.

**Python decoder.** `tools/anim_decoder.py::parse_event_section`
takes the section3 byte slice and returns a list of `AnimEvent`
records. Round-tripped by the module self-test.

**Doc errata.** The entry-header layout above lists `+0x14..+0x1f
zero padding`; the +0x14 u32 is in fact the optional event-table
pointer (zero when absent), and the +0x14 word IS used by the
runtime. The remaining +0x18..+0x1f bytes were zero in every entry
scanned and remain unverified-but-empty.

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

### Live bone matrices in EE RAM (2026-05-25, PCSX2 save state)

Confirmed by parsing a PCSX2 v2 save state captured with a character on
screen (tool: `tools/parse_pcsx2_state.py`):

- The engine stages **column-major 4x4 affine matrices** in a fixed .bss
  region of the boot ELF (.bss spans vram `0x00275B00`..`0x00823500`).
  Storage convention: each consecutive qword (16 bytes) is one column of
  the math matrix; the 4th column carries `(tx, ty, tz, 1)` translation,
  the first three columns are the rotation/scale axes. Stride 64 bytes
  (NO 32-byte VU1 padding — that padding is added at upload time).
- In this save state, four populated runs were found, organised as two
  pairs of {world-space, local/relative}, each pair occupying a
  contiguous 0xE00-byte slot:
  - `0x00286340` (world, 21 matrices, translation ~ (+85,+10,-293))
  - `0x00287140` (world, 21 matrices, identical translations — second
    instance / double buffer)
  - `0x00287F40` (local, 21 matrices, all translations within a few
    units of origin — limb-length offsets)
  - `0x00288D40` (local, 21 matrices, identical to the previous local
    set — second instance)
- Slot size is 0xE00 (3584) bytes, of which the first 0x800 hold up to
  32 matrices and the remaining 0x600 is zero padding. Two slots
  pack into 0x1C00 (the "world / local pair"); the two pairs at
  `0x00286340` and `0x00287F40` are 0x1C00 apart, consistent with a
  small ring/double-buffer of per-character rigs.
- Neither the world nor the local matrix sets occur anywhere in the
  user's extracted disc data — confirming the matrices are
  **constructed at runtime** (animation evaluation: bind-pose blended
  with a clip), not stored verbatim on disc.

VIF-DMA verification: VU1 dmem at the moment of capture held a 32-slot
**identity-matrix table** at `0x200`..`0xe00` (stride 0x60 = matrix + 32B
pad), with the EE source being four other .bss buffers at `0x004837A0`,
`0x00495850`, `0x004F37A0`, `0x00505850` (each preceded by an identical
64-byte DMA/VIF-tag header containing a REF tag, a `0x6CC00020` UNPACK
V4-32 opcode, and a self-referential pointer). These are evidently the
matrix-upload buffers for **non-skinned objects** (everything posed by
identity), while the 21-matrix runs at `0x002863XX` are the live
character rigs. The currently-loaded VU1 microprogram is at EE
`0x0023C780` (the static-mesh kernel), not the per-bone skinning kernel
at `0x00234610` — i.e. the renderer was between skinned-character
draws when the state was captured.

Caveat: 21 active matrices does NOT match any of the dumped 28-bone or
20-bone rigs. Possibilities (TBD): (a) a not-yet-dumped 21-bone rig
(some character/object that isn't in `models/*_skeleton.txt`); (b) the
posed character is a 28-bone rig but only 21 slots are populated this
frame (e.g. an `m_used_count` field controlling the upload count).
The full rig identification is the next step — it requires either (i)
finding the per-character "model instance" struct that owns the matrix
pointer (no direct pointers to `0x00286340` are present in EE memory,
suggesting it is computed as a base + offset by the engine), or (ii)
decompiling the function that writes the matrix buffer.

JSON exports of all four runs are available via
`tools/parse_pcsx2_state.py --scan-bones --dump-bones`.

**Bind-pose extraction — RESOLVED (2026-05-25).** A second PCSX2 save
state captured with the player alone in a small room (no NPCs loaded)
confirms the per-character layout: only the 0x1C00 pair starting at
`0x00287F40` is populated (the other pair at `0x00286340` is zeroed
when no other character is on screen), and that pair holds two copies
of a 21-active-matrix buffer (the player's current and previous-frame
local transforms — both buffers are local-frame matrices, not
world+local, used by the engine for inter-frame interpolation).

The player's id 0x71 declares 28 bones, but only 21 are live at
runtime: the trailing 7 slots (col0w = 0, no translation) are
inactive / reserved. The captured 21 active matrices are bone-local
(parent-relative), with shoulder-elbow translations of ~4 units along
local X and clean L/R symmetry around Z — the limb-length scale of
the bind-pose skeleton, modulated by the captured walk animation.

`tools/parse_pcsx2_state.py --player-bones` emits a structured
`player_bones.json` that pairs the two captured buffers and tags them
with the canonical skeleton (`chunk05/f04_id71.bin`) and mesh
(`chunk21/f17_id8f.bin`) sources. `extract_models.py --rigged --bones
player_bones.json --file chunk21/f17_id8f.bin` composes world
matrices using the id 0x71 parent table, applies them to the per-bone
Q4.12 object-space vertex packets, and writes a posed
`*_rigged.obj` (one `o bone_NN` group per bone, world-space). For
inactive bone slots (the trailing 7 of 28) the composer **skips the
section entirely** rather than padding with identity: an identity
fallback was found to plant the trailing sections' raw Q4.12
object-space verts (range ~+/-8 per axis) at the world origin as
stacked phantom copies of the body. Open in Blender, the corrected
output is a single posed figure of 1366 verts across 21 live bones;
the 7 skipped sections (~830 verts) belong to inactive bone slots
whose semantics aren't yet pinned down -- candidates include
per-material vertex re-emissions, LOD packets, or accessory-bone
geometry that this captured frame happens not to draw. This closes
the "bind-pose joint transforms" question: the transforms are
constructed at runtime from an animation system, snapshot-able from a
save state, and combine with the per-bone vertex packets to yield a
posed model.

Confirmation that both buffers are LOCAL (not one world + one local):
composing buffer[0] as local through the parent table does NOT
reproduce buffer[1]; the per-bone translations differ only on bones 1
and 4 by tenths of a unit (consistent with frame-to-frame
interpolation), and mirror-pair symmetry on bones 5/6, 8/9, 10/11,
12/13, 17/18, 19/20 is identical between buffers -- both are
parent-relative limb-length offsets.

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

## func_00100EB8 — GS-VRAM-to-EE readback driver (2026-05-27)

**Major correction to the 2026-05-25 "VU1 microcode-upload / setup driver"
guess in "Bind-pose matrix EE-side trace" above.** Reading `func_00100EB8`
end-to-end together with its packet-builder `func_00100D78` and its only
caller `func_001D7410` shows it is NOT the VU1 microcode uploader. It is
the engine's **synchronous GS-local-memory READBACK driver** — it pushes
a 7-qword BITBLTBUF/TRXPOS/TRXREG/TRXDIR=1 packet to GS, waits for the
transfer setup to complete, then DMAs pixel data **back** from GS VRAM
into an EE buffer via VIF1 in reverse-FIFO mode, draining tail bytes
from the VIF1 FIFO. The packet-builder, the readback, and the format
jump-table all line up exactly. The "5 DMA submissions + 5 raw FIFO
writes" framing in the prior session's PROGRESS note was a miscount —
there is **1 outbound DMA**, **1 inbound DMA**, and the "FIFO writes"
are actually `lq` *reads* from `0x10005000` (VIF1 FIFO in reverse-FIFO
mode), not writes.

### Function signature and call site

`func_00100EB8($a0 = packet_struct, $a1 = dst_buffer)` — 0x68C / 419
instructions, vram `0x00100EB8`. Only static caller is
`func_001D7410` (vram `0x001D7410`), which loops `D_00275C08` items
and per item does:

```
func_00100D78(sp+0x50, w, h, dst_x, dst_y, src_x, src_y, opcode, ...)
DisableDmacHandler(0)
func_00100EB8(sp+0x50, item->offset+8)   /* read pixels back to RAM */
sub_D2_TADR_08x(0, 0)                    /* idle-wait VIF1/GIF/VU1 */
```

`D_00275C08` is a per-frame counter (zeroed at the end of
`func_001D7410` and other places — it's a small queue of pending
GS-readback requests). `D_008172C0` indexes into a 0x4C0-stride array
of GS-readback descriptors; the 0x48-byte sub-record at +0x9C\*0x4C0
holds the request that gets shipped this frame.

### func_00100D78 — packet builder (proves the contents)

`func_00100D78($a0 = pkt, a1=w, a2=DBP, a3=DBW, t0=h, t1=DPSM, t2=DSAY,
t3=DSAX, ...)` — 0x140 bytes. Stores a GIFtag at +0x4..+0x18 then 8 GS
register fields at +0x10..+0x68, returns **7** (the packet's qword
count, exactly the value `func_00100EB8` writes to D1_QWC). Decoded
field layout in the 0x70-byte struct:

| Offset | Stored value | GS field |
|---|---|---|
| +0x00 | 0 | reserved / DMA tag slot |
| +0x04 | `0x06008000` | GIFtag word 0: NLOOP=0, EOP=1, PRE=0, NREG=6, FLG=0 PACKED |
| +0x08 | `0x13000000` | GIFtag word 1 (top half): REGS field |
| +0x0C | `0x50000006` | GIFtag word 2: NREG-tail / continuation flags |
| +0x10 | `(t4 with PSM bits)` | low qw of BITBLTBUF (DBP/DBW/DPSM packed in) |
| +0x18 | `(t5)` | high qw of BITBLTBUF (SBP/SBW/SPSM) |
| +0x20 | `(a1<<32 \| a2<<16 \| a3)` | TRXPOS or TRXREG payload |
| +0x28 | `0x50` | **BITBLTBUF register address (GS 0x50)** |
| +0x30 | `(t1\|t0)` | TRXREG payload (W \| H) |
| +0x38 | `0x51` | **TRXPOS register address (GS 0x51)** |
| +0x40 | `(t2\|t3)` | TRXREG (used as TADR slot later by `func_00100EB8`) |
| +0x48 | `0x52` | **TRXREG register address (GS 0x52)** |
| +0x50 | 0 | |
| +0x58 | `0x61` | **NOP / FINISH register (GS 0x61)** |
| +0x60 | `0x1` | TRXDIR payload — **direction=1 ⇒ GS-VRAM → EE** |
| +0x68 | `0x53` | **TRXDIR register address (GS 0x53)** |

Returns 7. So the struct is a **GIF PATH2 packet via VIF1**: 1 qword
GIFtag header (+0x00) + 6 qwords of A+D-format register writes
(addr/data pairs at +0x10/+0x28, +0x18/+0x38, +0x20/+0x48,
+0x30/+0x58, +0x40/+0x68 — and +0x50/+0x60 unused/tail).

The constant `0x1` at TRXDIR = **VRAM→EE**, which is what proves the
function is a READBACK, not an upload.

### func_00100EB8 — what each section does

The function walks the packet struct, decodes the BITBLTBUF / TRXREG
fields it just received (re-extracting DPSM, w, h from the qwords
the builder packed), runs the PSM→stride jump table to compute exact
byte sizes for the readback, then issues:

1. **Format jump table** (`jtbl_0026B130`, opcodes 0..0x3A): a
   per-PSM expansion-factor calculator. Cases observed:
   - PSMCT32 (`PSM=0x00`): bytes-per-pixel = 4, divide W\*H by 4
     for qword count.
   - PSMCT24 (`PSM=0x01`): bpp=3, ÷5.333.
   - PSMCT16 / PSMCT16S (`PSM=0x02 / 0x0A`): bpp=2, ÷8.
   - PSMT8 / PSMT8H (`PSM=0x13 / 0x1B`): bpp=1, ÷16. **PSMT8 is
     case 0x13** — directly relevant for the engine's PSMT8 textures.
   - PSMT4 / PSMT4HL / PSMT4HH (`PSM=0x14 / 0x24 / 0x2C`): bpp=0.5, ÷32.
   It computes (a) `$s2` = whole-qword count, (b) `$s3` = leftover
   byte-tail count, (c) `$s1` = sub-qword pad count, (d) `$s6` =
   extra trailing qwords (the per-PSM "block-tail" rounding).

2. **Patch DMA tag** at `pkt+0x40` (`.L00101094`): OR with
   `0x20000000` (uncached segment) then write the 8-byte chain-end
   tag containing `qwc | dst_addr`. This builds the DMA chain header
   in-place.

3. **Wait D1_CHCR.STR clear** (`.L001010BC`) — VIF1 channel idle.

4. **Watchdog**: `SetCPUTimerHandler(0)`, `SetCPUTimer(prev | 0x200)`
   — installs a 1-shot timeout while we wait on GS.

5. **GS_CSR = 2** at `0x12001000` — clears GS FINISH flag.

6. **First DMA submission** (outbound, `.L001011BC` block):
   - `D1_QWC = 7`  (the packet size from `func_00100D78`)
   - `D1_MADR = pkt | 0x80000000` if `pkt & 0x70000000 == 0x70000000`
     (scratchpad-RAM source bit), else `pkt & 0x0FFFFFFF`
   - `D1_CHCR = 0x101` — STR|DIR=1 (MEM→FIFO), normal mode
   - Wait STR clear, wait `GS_CSR.FINISH` (bit 1) set — the GS has
     processed the BITBLTBUF/TRXPOS/TRXREG/TRXDIR setup.

7. **Switch to reverse FIFO** (`.L00101208`):
   - `VIF1_STAT = 0x800000` — sets **VFS** (VIF1 forced reverse-FIFO mode)
   - `GS_IMR = 1` — mask GS interrupts during readback

8. **Second DMA submission** (inbound, `.L00101228..L00101314`),
   gated on `$s2 != 0`:
   - `D1_QWC = $s2`  (whole-qword count from step 1)
   - `D1_MADR = dst_buffer` (with SPR-bit handling on `$a1`)
   - `D1_CHCR = 0x100` — STR only, **DIR=0 = FIFO→MEM (reverse direction)**
   - Wait STR clear

9. **Three FIFO drain loops** for the trailing partial qwords that
   the qword-aligned DMA can't move:
   - `.L00101388` loop (gated on `$s1`): for each of `$s1` qwords —
     poll `VIF1_STAT & 0x1F000000` (FQC field, FIFO qword count) until
     non-zero, then `lq` from `0x10005000` and `sq` to
     `dst + (s2*16) + i*16`.
   - `.L00101440` loop (gated on `$s3`): copy `$s3` **bytes** from a
     1-qword temp buffer at `$sp+0`  to dst — the sub-qword tail.
   - `.L00101464..L00101480` loop (gated on `$s6 > 0`): same FIFO-
     drain pattern, `$s6` extra qwords to scratch on the stack
     (`sq $v0, 0($sp)`) — leftover that doesn't fit in dst.

10. **Restore state** (`.L001014D0`):
    - `VIF1_STAT = 0` (exit reverse-FIFO mode)
    - `GS_IMR = 0` (re-enable GS interrupts)
    - `SetCPUTimer(prev)` — restore watchdog
    - `GS_CSR = 2` again (re-clear FINISH so the next submission can
      wait on it cleanly)
    - **Final 1-qword FIFO write**: `lq` from `D_00241040`, `sq` to
      `0x10005000` — writes a fixed terminating GIFtag (the "stop"
      tag) to the VIF1 FIFO so the next forward-FIFO transfer starts
      from a known state. This is the **only direction-write to
      `0x10005000`** in the whole function.

### Error / timeout path

Several inline waits (`$s0` is a saturating per-call timeout counter)
fall through to one of three error handlers via
`func_00122B58(error_string)`:
- `D_0026B050` (`func_001009C8` chain): "DMAC STR timeout" before
  setup-write
- `D_0026B088`: "GS FINISH timeout" before readback
- `D_0026B0B8` / `D_0026B0F8`: "VIF1 FIFO drain timeout"

Each error path also restores `GS_CSR = 0x100` and `GS_IMR = 0` then
returns `-1`. Success returns `0`.

### What was uploaded where, item by item

- **Per-DMA-submission summary (2 of them, not 5):**
  1. `QWC=7`, `MADR=pkt_struct`, `CHCR=0x101` (MEM→FIFO, normal) —
     ships the BITBLTBUF/TRXPOS/TRXREG/TRXDIR=1 setup packet to GIF
     PATH2 via VIF1 DIRECT. Source: caller's stack-built struct.
  2. `QWC=s2`, `MADR=dst_buffer`, `CHCR=0x100` (FIFO→MEM, reverse) —
     bulk pixel readback from GS local memory to `$a1`. Source: GS
     VRAM rectangle programmed in step 1.
- **Per-FIFO-write summary (1 of them, not 5):** A single 16-byte
  `sq` to `0x10005000` of the constant qword at `D_00241040`,
  emitted at function exit to leave a known good "stop" tag in
  VIF1 FIFO after exiting forced-reverse mode. The remaining 3
  "FIFO accesses" are `lq` *reads* from VIF1 FIFO during the
  reverse-direction drain loops, not writes.
- **Microcode IDs uploaded:** **None.** `func_00100EB8` does not
  upload any VU1 microcode. It is GS-only (PATH2 + GS-VRAM
  readback). The 48 VU1 microcode kernels cataloged in `tools/
  disasm_vu.py catalog` are uploaded by a different, still-
  unidentified driver — most likely a one-shot at engine init
  that walks the 48-packet table and DMAs each through VIF1 PATH1.
- **GS / TEX0 writes found:** Zero TEX0 writes. The function
  writes BITBLTBUF, TRXPOS, TRXREG, TRXDIR — the **image transfer
  registers** — not the texture-sampler registers TEX0_1/TEX0_2.
  So this driver doesn't bind textures for drawing; it copies
  pixel data out of VRAM. No PSMT8 CLUT setup happens here.
- **Bind-pose matrix source:** Still unknown. `func_00100EB8`
  reads pixels from VRAM, not matrices to VU1, so the bind-pose
  hunt has to go elsewhere. The remaining candidate VIF1 DMA
  driver is whatever loop ships the 48 microcode packets at
  init, and whatever per-frame submitter ships geometry +
  bone-matrix UNPACK packets. Neither is reachable by static
  cross-reference from a named entry point (confirmed in the
  prior session).
- **CLUT setup:** Still unknown. Not touched by this function.

### What the caller `func_001D7410` is doing

The driving loop reads each of `D_00275C08` items from a per-frame
queue (the 0x48-byte entry inside `D_008172C0[id*0x4C0]+offset`),
calls `func_00100D78` to build a per-item readback setup packet
on the stack, then `func_00100EB8` to execute the readback into
`item->offset+8`. The first arg `func_00100D78(sp+0x50, w, h, ...)`
takes the dimensions from per-item fields. This is **per-frame
GS→EE readback for game-side use of rendered data** — most
plausibly: shadow / projection / lightmap baking captured at
small resolutions per frame, or rendered light occlusion masks for
the visibility system. The 0x48-byte stride and the per-item
metadata suggest a small fixed queue (~8-32 items per frame max).

The 0x1F000000 FIFO-mask and the watchdog timer both point at a
truly synchronous "wait for GS to finish, drain pixels, return"
operation — consistent with this being game-logic feedback from
rendered data, not a streaming render path.

### What this unblocks / doesn't

- **Bind-pose source** — NOT unblocked. Search continues in the
  per-frame draw dispatcher's call tree.
- **PSMT8 CLUT setup** — NOT unblocked. CLUT binding happens at
  draw time (TEX0 writes) elsewhere, not in this readback driver.
- **Microcode upload chain** — NOT unblocked. The 48 VU1 packets
  ship via a different driver, still unidentified.
- **GS readback feedback** — **NEWLY identified.** The engine
  does per-frame VRAM readback for some game-state purpose
  (resolution and content TBD). This is itself an unexpected
  finding worth following up — what game code consumes the
  readback data the loop in `func_001D7410` produces? That trace
  starts at the caller `func_001AAE40` (vram `0x001AAE40`, the
  only static caller of `func_001D7410`).

## VU1 microcode upload path — generic-DMA pump architecture (2026-05-27)

Resolves the open question left by the 2026-05-27 `func_00100EB8` correction.
The boot ELF does **not** ship VU1 microcode packets via hard-coded writes to
`D1_MADR/QWC/CHCR` (`0x10009010/20/00`). The only function in the entire boot
that touches those addresses with their absolute literals is `func_00100EB8`
(GS-VRAM readback) and the busy-wait sync `sub_D2_TADR_08x` — neither uploads
VU1 microcode. The engine instead uses an **indirected DMA dispatch**:

### The DMA dispatch primitives

- **`func_00101BB8(ch)`** — channel-base accessor. Returns
  `D_00241050[ch]` for `ch < 10`, where `D_00241050` is a table of all ten
  DMAC channel base addresses (`0x10008000`, `0x10009000`, `0x1000A000`,
  `0x1000B000`, `0x1000B400`, `0x1000C000`, `0x1000C400`, `0x1000C800`,
  `0x1000D000`, `0x1000D400`). Every DMA dispatch in the engine indirects
  through this. **This is why all earlier greps for `0x10009000` came up
  empty — only the readback driver inlines the literal.**

- **`func_00101FE0($a0=ch_base, $a1=madr, $a2=qwc)`** — the actual
  **DMA kick**. Polls `ch_base[0].STR` (CHCR bit 0x100) until clear, then
  writes `MADR = $a1`, `QWC = $a2`, and sets `CHCR |= 0x101` (STR + DIR).
  Generic — works for any channel — and is the only outbound-DMA kicker
  in the boot ELF.

- **`func_00102468(ch_base, mode, timeout)`** — paired DMA-wait/sync
  used after every kick (with `func_00122B58` as the yield-to-scheduler
  callback during the spin).

- **`func_00101BE0(reset_mode)`** — global DMAC reset; iterates the
  channel-mask table `D_00241090` and zeroes CHCR/MADR/TADR/QWC for each
  active channel, then touches D_CTRL (`0x1000E000`) and D_STAT
  (`0x1000E010`).

- **`func_00101BB8`** has **5 callers** in the boot ELF —
  `func_001CCB10`, `func_001CCBD0`, `func_001CCCC0` (all GS-VRAM **uploaders**
  symmetric to `func_00100EB8`'s readback), `func_001D21E0`, and
  `func_00200830`. The three `0x1CCxxx` callers all call `func_00101FE0`
  with `qwc=0x17` (23 qw — exactly the BITBLTBUF+TRXPOS+TRXREG+TRXDIR
  setup packet plus a one-page IMAGE-mode transfer header) and build the
  packet via `func_001CCE80`. These are the **CPU→GS texture-page DMA
  uploaders** (PATH3 via VIF1 channel).

### VIF1 bootstrap

`func_00100278` (0x68 bytes) is the **VIF1/VU1 cold-start primer**:
resets VIF1 FBRST (`0x10003C10 <- 1`), VIF1 ERR (`0x10003C20 <- 2`), sets
VU1 control bit `0x200` in `$vi28`, then **DMA-bypasses by streaming
two quadwords directly from `D_00241020` to the VIF1 FIFO at
`0x10005000`**. Those two quadwords decode as VIF commands
`STCYCL(WL=4 CL=4); STMASK; NOP; MSCNT; MSKPATH3; NOP; FLUSH; NOP` — a
clean idle/sync pattern that primes VU1 and parks PATH3 before normal
DMA dispatch starts. This runs once at boot from `_start`.

### The catalog-vram-referencing wrappers

Two dozen functions in the `0x001D3xxx-0x001D5xxx` and
`0x001E7xxx-0x001E9xxx` clusters each reference exactly one catalog
packet vram via a `D_xxxxxxxx` label. Pattern (representative,
`func_001D4960` referencing offset `+0x524` inside packet `0x0023976c`):

1. Call `func_001D4750($a0=ch)` — builds a giant pre-built UNPACK
   header in BSS scratch at `D_00817240..D_008172BC` (containing four
   4x4 matrices and a `0x6C0403F5` and `0x6C080000` VIF UNPACK V4-32
   command — the **per-bone matrix UNPACK to VU dest=0x03F5 NUM=4**),
   then memcpys it into a chain-buffer cursor `0x10($t0)` where `$t0 =
   D_00275670[ch]` (the per-channel DMA chain-buffer base, BSS).
2. Call `func_001D2090($a0=ch, $a1=&packet_offset)` — appends a 2-tag
   pair: (tag id=0x30 = REF, qwc=1, addr=$a1) followed by (tag id=0x50 =
   CNT continuation). This **chains the microcode packet into the
   per-channel DMA chain buffer without copying it**.
3. The chain buffer is later kicked by code we have not yet pinpointed
   (likely a per-frame flush in the `func_001D1C50` / `func_001D7C30` /
   `func_001D30A0` call tree) that walks `D_00275670[1]`, calls
   `func_00101BB8(1)`, and hands the chain head to `func_00101FE0`
   with chain mode.

`D_00275670` and `D_00275674` are the engine's per-channel
DMA-chain-buffer base + write-cursor pair (`gp_rel` accessed from ~100
functions). Every microcode packet, matrix UNPACK, vertex UNPACK, and
MSCAL tag is appended here per-frame; one bulk kick per channel per
frame ships them all.

### Identified microcode-shipping wrappers (sample)

Each function builds a single REF tag pointing at a catalog packet
(meaning that packet is uploaded via DMA-REF tag, not memcpyed):

| Function | Catalog packet | Microcode role |
|---|---|---|
| `func_001E7D20` | `0x0023460c` | **Skinning kernel #1 main** (vram->IMEM 0x0000, 153 qw) — the bone-matrix consumer at IMEM dest=0, qw 0..3 |
| `func_001E9280` | `0x00234c10` | bone-helper #1 (15 qw, IMEM 0x0800) |
| `func_001E9E60` | `0x0023402c` | preceding kernel pair (81 qw, IMEM 0x0100) |
| `func_001F0720` | `0x00232d6c` | early kernel data half (66 qw, IMEM 0x0100) |
| `func_001D3AD0..001D5170` | various | static-geometry kernels |

### Bone-matrix UNPACK source — REVISED 2026-05-27

**Previous hypothesis (that some other function per-frame-fills
`D_00817240..D_008172BC` with bone matrices) is REFUTED.** A full
search of `build/asm/matchings/main/code/*.s` (3014 functions, 100%
coverage) and `src/` finds exactly **one writer** to any address in
`D_00817240..D_008172BC`: `func_001D4750` itself. No other function
contains a LUI/ADDIU pair, gp-rel reference, or symbol reference that
resolves anywhere inside this range.

What `func_001D4750` actually does (lines 4-77 of the splat asm):
the first ~120 instructions are 28 `sw` stores of literal immediates
(`0x3F800000` = 1.0, `0xBF800000` = -1.0, `0x42000000` = 32.0, and
`$zero`) into `D_00817240..D_008172AF`, followed by 4 stores of
`0x4B000040` / `0x4B000080` into `D_008172B0..D_008172BC`. The decoded
content is:

```
D_00817240 (qw 0..3 / 64 bytes): a fixed 4x4 matrix
                  ( 1  0  0  0 )
                  ( 0 -1  0  0 )    <- Y-flip / handedness
                  ( 0  0  1  0 )
                  ( 0  0  0  1 )
D_00817280 (qw 4..6 / 48 bytes): three quadwords of (32.0, 32.0, 32.0, 0)
                  — almost certainly the GS PRIM-context fixed-point
                  bias / sub-pixel offset constants
D_008172B0 (4 words / 16 bytes): packed VIF tag immediates
                  0x4B000040, 0x4B000040, 0x4B000040, 0x4B000080
                  (UNPACK V4-16 / V4-32 NUMs to a fixed dest)
```

The BSS scratch is purely a **once-built constant UNPACK header**. On
every call (per-channel, per-frame), `func_001D4750` rewrites the same
constants and memcpys the block into the chain buffer via
`func_00102958`. So the staging area is **idempotent boilerplate, not
a per-frame bone-matrix sink**. The "four 4x4 matrices" reading was
wrong — there is one fixed transform + a constants triplet + VIF tags.

**Implication: the per-bone matrix data does NOT pass through
`D_00817240`.** The actual per-bone matrices ship via a different
mechanism — most likely the second UNPACK that `func_001D4750` appends
(the `0x6C0403F5` immediate, V4-32 NUM=4 dest=0x03F5), whose source
pointer is passed *as a chain-buffer field* and built by the CALLER
of `func_001D4750`. The two `jal func_00102958` calls memcpy from
`D_00817240` (the constants) — but the chain ALSO contains a REF tag
appended by `func_001D2090` in each caller (see
`func_001D4960` line 14: `D_00816440` as the catalog packet base
plus `0x9C(t0) << 7` per-channel offset). That `D_00816440`-relative
buffer is the next thing to investigate — it is indexed per channel
by `0x9C(t0)` (a counter that increments per draw) and is the most
likely live per-bone matrix arena.

**Next hunt target:** `D_00816440` and `0x9C` offset in the per-
channel struct rooted at `D_00275670`. Search writers to that range,
not `D_00817240`.

### D_00816440 arena — writer identified (2026-05-27)

**Writer:** `func_001D2E20` (vram 0x001D2E20). One-shot initializer.
Structure: a 2-iteration outer loop (`slti $v1, 0x2`), each iteration
calls `func_00121870(dst, src, 0x80)` (a generic block-copy) 14 times
to fill 14 destination arenas from 14 contiguous source blobs, then
advances all dst/src pointers by 0x80 and repeats.

| dst arena (per channel) | source blob (main image .data) |
|------------------------|--------------------------------|
| `D_00816440..D_00817140` (14 arenas, 0x100 apart) | `D_002514D0..D_00251B50` (14 src, 0x80 apart) |

Result after the call: each of the 14 0x100-byte per-channel arenas
holds **two 0x80-byte VIF-UNPACK-prefixed matrix blobs** copied from
the static source table. The 0x100 stride for arena labels reconciles
with the runtime stride seen at REF-tag build time (`<<7` = 0x80) —
each arena is 2 slots × 0x80, and `0x9C(t0)` ping-pongs between the
two halves.

**Source data nature.** The source blocks at `D_002514D0` start with
the header word `0x01000404` followed by `0x6C0703F9` — that is a
VIF UNPACK-V4-32 tag (`6C` opcode + NUM/dest) plus its `0x80` payload.
So the "matrices" being moved are **static, pre-formatted VIF UNPACK
packets baked into the boot ELF** — not anim-evaluator output. The
14 destinations correspond 1:1 to 14 PS2 character/skeleton-channel
slots used by the engine's draw catalog.

**Runtime consumers (REF-tag builders) of `D_00816440 + (ch<<7)`:**
`func_001D30A0`, `func_001D38A0`, `func_001D3E40`, `func_001D4960`,
`func_001D4DA0` — all consume the arena address as a DMA REF source.

**Caller chain (writer side):**
```
func_001ACA20 / func_001AE040  (engine state setup)
  -> func_001D19E0
       -> func_001D2E20      <-- the arena initializer (this finding)
       -> func_001D9720      (sibling — likely VIF-state companion)
```

**Connection to the 4× per-character 21-matrix EE-RAM buffers at
`0x002863xx..0x002893xx`:** *None observed in static disassembly of
this path.* `func_001D2E20` does not touch the `0x00286xxx..0x00289xxx`
range. The `D_00816440` arena and the four 21-matrix BSS buffers are
**two distinct skinning pipelines**:

- `D_00816440` arena: 14 hardcoded per-channel VIF-UNPACK packets,
  initialized once from static `.data`, then chained by REF tag
  per draw (`func_001D4960`/etc.). This is a **fixed/identity-pose
  pipeline** — likely menu, cutscene, or bind-pose-only actors.
- `0x002863xx..0x002893xx`: 4× 21-bone matrix buffers — the live
  per-frame anim-evaluator output for in-game character skinning,
  identified separately via PCSX2 save-state. Their writer was NOT
  located by this search and is the *next* hunt target.

**Still unknown.**
1. ~~Writers to the 4 per-character 21-matrix buffers at `0x002863xx`.~~
   **RESOLVED 2026-05-27 — see "Live bone matrix writers" below.**
2. Whether `func_001D9720` reuploads or patches `D_00816440` per
   frame (sibling call right after the one-shot init) — quick read
   suggests it's VIF1 cold-start related, not a per-frame matrix
   writer, but unverified.
3. Whether the static blobs at `D_002514D0..D_00251B50` are pure
   identity matrices or carry per-channel scale/orientation biases
   (header word is uniform `0x01000404 / 0x6C0703F9`; payload not
   inspected).

### Live bone matrix writers — RESOLVED (2026-05-27)

Static disassembly of `build/asm/matchings/main/code/` finds exactly
**three functions** that load the addresses of any of
`D_00286340 / D_00287140 / D_00287F40 / D_00288D40` (the 4 live bone
buffers identified via PCSX2 save state) with `lui+addiu`:

- `func_0017A130` (0x6C8) — **per-frame dispatcher**. Reads a float
  selector at `+0x278` of its sole arg (a per-actor struct also passed
  to `func_00179BC0`/`func_0017A0B0`), picks one of 5 routing branches,
  and for each branch makes a sequence of `func_0017A0B0(actor, mode)`
  → `func_00179BC0(actor, slot_index<<16>>16, buffer_base)` calls.
  The buffer base passed in `$a2` is one of the four fixed BSS
  addresses, hardcoded per branch. The branch selector picks between
  {world,local} pair A (`0x00286340 / 0x00287F40`) and pair B
  (`0x00287140 / 0x00288D40`); the second pair only ever appears in the
  late branches, consistent with the FINDINGS observation that when
  only the player is on screen only the `0x00287F40` pair is populated.
- `func_00148B40` (0x1010) — a **larger sibling** of `func_0017A130`
  with the same four buffer addresses interleaved; the same
  ($s3,$s4,$s5)=(local,local',world) register pattern, suggesting a
  multi-actor or transitional/cross-fade variant of the same routing.
  Called only from `func_00147700` (only caller of `func_00147390`).
- `func_0017B660` (0x2A8) — references only `D_00287F40 / D_00288D40`
  (the player-only pair), and is the writer-side of the
  player-character path. Called only from `func_0017C030`, in turn
  called from `func_001612D0` (writer) and `func_001837B0` (writer).

**The inner writer is `func_00179BC0`** (0xD4 bytes). Signature:
`(actor *a0, short slot_idx_a1, matrix_buffer *a2)`. Body:

1. Reads `lbu $v1, 0x1F0($a0)` — actor "kind" byte. Cases `0x31` and
   `0x34` take one normalisation path (build a `cvt.s.w` of
   `lh 0x276($a0)` and call `func_001749F0(actor, 0.0, n)`); otherwise
   call `func_001749F0(actor, 0.0, 1.0)` — these set up an
   animation-time / blend-weight before evaluation.
2. Calls `func_001C6DA0(actor)` — likely the per-actor animation
   evaluator that fills the per-bone source structs.
3. Loops `for (i = 0; i < lbu($s3 + 0xC); i++)` — **bone count** is a
   byte at offset `+0xC` of the actor struct.
4. Per iteration: `ptr = *(D_00275B40 + i*4); func_00102958(dest, ptr + 0x90); dest += 0x40;`

`func_00102958` is a pure 64-byte qword copy (`lq/sq` × 4) — confirmed
matrix-sized.

**`D_00275B40` is a runtime-resolved pointer to a flat array of
per-bone source pointers.** It is initialised by `func_001CB5B0`:
`D_00275B40 = *D_00275B48 + 0x110`. The per-bone source struct holds a
fully composed 4x4 column-major matrix at `+0x90`; that struct is
populated upstream by the animation evaluator chain rooted at
`func_001C6DA0`. **So `func_00179BC0` is the matrix *publisher***: it
gathers already-computed per-bone matrices from a heap-allocated
animation scratch (reached via the `D_00275B40` pointer-of-pointers
table) and copies them into the four fixed BSS slots that the
skinning UNPACK pipeline reads from.

This explains *why* no direct EE pointer to `0x00286340` exists in
captured memory: the destination address is a compile-time constant
baked into `func_0017A130` / `func_00148B40` / `func_0017B660`, never
stored.

**Caller chain (live anim pipeline):**
```
func_001ACA20 / func_001AE040   (engine state setup — same top as D_00816440 pipeline)
  -> func_0015BCF0
       -> func_0015BA50
            -> func_0015B130                   -> func_001612D0 -> func_0017C030 -> func_0017B660 (player-only writer)
            -> func_0015B530                   -> func_001837B0 -> func_0017C030 -> func_0017B660
            (sibling) func_00147390 -> func_00147700 -> func_00148B40           (multi-actor writer)
       -> ... per-actor: func_0017A130                                          (general dispatcher writer)
                            -> func_0017A0B0   (frame/slot index selector)
                            -> func_00179BC0   (the publisher above)
                                 -> func_001749F0   (anim-time / blend setup)
                                 -> func_001C6DA0   (per-actor anim evaluator — fills +0x90 source matrices)
                                 -> func_00102958   (64-byte matrix copy, per bone)
```

The **next target for true "where does anim data live"** is therefore
`func_001C6DA0` and the heap region returned by `*D_00275B48 + 0x110`
— this is the per-bone source-struct array, and its inputs are the
actual animation clips. From the PCSX2 capture the source matrices
match the bone-local interpolated pose, so the evaluator at
`func_001C6DA0` is doing the keyframe lookup + SLERP/LERP + matrix
compose, almost certainly off the id 0x71 entry data (skeleton +
clips) — but **this writer does not itself read id 0x71**; the
question is now pushed into `func_001C6DA0`.

**No connection to the static D_00816440 pipeline at the writer
level** — the two arenas (D_00816440 fixed-pose VIF arena and the
0x002863xx live bone slots) are independent staging areas with
independent fillers, sharing only the top-level per-frame entry
points `func_001ACA20` / `func_001AE040`.

### PSMT8 TEX0 setup — NOT YET FOUND

No EE function in the boot ELF builds a TEX0 register write with
PSM=0x13 visible in static disassembly. Per the per-frame PCSX2 GS dump
analysis (2026-05-25), all draw GIFtags are PATH1 PACKED TRISTRIP/SPRITE,
implying **TEX0 is composed inside VU1 microcode** (one of the 256-qw
kernels at IMEM=0 in the `0x002354cc / 0x002364dc / 0x00237750 /
0x00238760 / 0x00239cc0` families) and shipped via XGKICK. The per-material
CLUT pointer is therefore a VU1 dmem field that EE writes per-draw via
a small UNPACK to a fixed dest. Identifying that UNPACK dest requires
disassembling the kernel's TEX0-emit sequence — left for next session.

### Caller-chain summary

```
_start
  -> func_00100278        (VIF1/VU1 cold-start: STCYCL, MSCNT, MSKPATH3)
  -> func_00101BE0        (DMAC global reset)
  -> ...
  -> func_001D1C50        (per-frame engine init / chain head builder)
       -> func_001D2830   (per-frame state machine)
       -> func_001D7C30   (per-frame builder)
       -> func_001D30A0   (per-frame draw walker)
            -> func_001D4960 / func_001E7D20 / etc.
                 -> func_001D4750  (UNPACK matrix builder)
                 -> func_001D2090  (REF tag append for microcode packet)
                 -> ... appends to D_00275670[1] chain buffer ...
       -> (unidentified flush; calls func_00101BB8(1) + func_00101FE0)
            -> DMA kicks chain on VIF1
```

### Updates to prior FINDINGS

- The 2026-05-25 claim that `func_00100EB8` "is the static VU1 init /
  packet shipper" was corrected on 2026-05-27 to GS-VRAM readback.
- The 2026-05-25 claim that "All VIF1 DMA submissions live in
  `func_00100A60` (`sub_D2_TADR_08x`) and `func_00100EB8`" was wrong:
  `sub_D2_TADR_08x` is sync only; `func_00100EB8` is readback only.
  **All outbound VIF1 DMA goes through `func_00101FE0`** via the
  channel-base table at `D_00241050` returned by `func_00101BB8`. The
  earlier blind grep for `0x10009` could not see this because the
  channel base is loaded once per dispatch from the table, never as
  a literal `lui`/`ori` pair.

## Per-bone animation evaluator (`func_001C6DA0`, 2026-05-27)

`func_001C6DA0` is the per-actor per-frame animation/pose evaluator
called from `func_00179BC0` immediately BEFORE the publisher's per-bone
matrix copy loop. It is **the producer** of the 4x4 source matrices at
each per-bone struct's `+0x90` field (`D_00275B40[i].matrix`, consumed
by `func_00102958`).

**Size:** 0x680 bytes / 426 asm lines. **One argument:** `$a0 = actor*`.

**Inputs (per-bone struct, base = `*(actor + 0x110 + 4*bone_idx)`):**
| Offset | Type | Use |
|---|---|---|
| `+0x18..0x20` | 3x int32 quantised | per-axis scale factors (Q?, used as `vmulx.xyz` on identity-axis vec4s for the "fast" path bone 0) |
| `+0x30` | quat (vec4) | source quaternion A |
| `+0x40` | quat (vec4) | source quaternion B |
| `+0x50` | f32 | quat blend factor `t` (clamped <= 1.0) |
| `+0x60..0x6C` | f32 vec3 | local translation (TRS T) |
| `+0x64` | int16 | parent bone index, `-1` = root |
| `+0x70..0x7C` | f32 vec3 | local Euler rotation (TRS R) |
| `+0x7C..0x84` | f32 vec3 | local scale (TRS S, float) |
| `+0x88, +0x8A, +0x8C` | s16 x3 | additional per-axis fine scale, decoded by `* 2^-12` (`0x39800000` = 1/4096) and `vmulx.xyz`'d into the X/Y/Z basis vectors |
| `+0x90` | OUTPUT 4x4 | composed world-space bone matrix (the matrix this whole pipeline produces) |

**Actor struct fields read:**
| Offset | Use |
|---|---|
| `+0x0C` | u8 bone_count (loop limit) |
| `+0x60..0x6C` | actor scale vec3 (a3 to TRS builder) |
| `+0xB0..0xBC` | actor translation vec3 (a1 to TRS builder) |
| `+0xC0..0xCC` | actor Euler rotation vec3 (a2 to TRS builder) |
| `+0xD0..+0x10C` | OUTPUT actor root 4x4 matrix (built by `func_001C94B0`) |
| `+0x110[]` | array of per-bone struct pointers (D_00275B40 = this+0x110 for the current actor; `func_001CB5B0` just publishes the pointer) |

**Scratch workspace (`D_70003400..D_70003600+`):** an EE scratchpad
region used as transient matrix storage:
- `D_70003400` = composed local 4x4 (output of `func_00102C58` per bone)
- `D_70003440` = parent matrix (loaded into `vf04..vf07` and multiplied
  with local via vmulax/vmadday/vmaddaz/vmaddw)
- `D_70003450/0x60` = X/Y/Z basis-rotated identity rows used to apply
  the s16 fine scale (`+0x88..+0x8C`)
- `D_70003600` = output of `func_001CA0A0` (the quaternion blend buffer)

**Math identified:**
1. **`func_001C94B0(actor+0xD0, actor+0xB0, actor+0xC0, actor+0x60)`** —
   actor root matrix = `T(pos) * R(eulerZ) * R(eulerY) * R(eulerX) * S(scale)`.
2. For each bone `i = 0..bone_count`:
   - **Bone 0** (fast path, no quat blend): `func_001029C0` loads identity
     into D_70003400/D_70003440, `func_00102C58` applies Euler rotation
     from bone+0x70..0x78 (calls into a chain of `func_00102A60` /
     `func_00102B08` / `func_00102BB0` — the standard rotateX/Y/Z helpers
     used elsewhere in the boot ELF), then applies the s16 fine scale
     via three `vmulx.xyz` writes on the basis vectors.
   - **Bone i>=1:** `func_001CA0A0(D_70003600, bone+0x30, bone+0x40, bone+0x50)`
     blends the two quats with factor `t` (clamped to <=1; if `dot < 0`
     it negates B for shortest-path) — this is **quaternion NLERP**.
     Then `func_001CA1C0` (127 lines, contains `vopmsub` and Gram-Schmidt-
     looking ops) converts the blended quat into a 3x3 basis rotation
     and writes it into the actor's bone slot starting at bone+0x00.
     Then the same identity / Euler / s16-scale pipeline as bone 0.
3. Final per-bone concatenation:
   - `vf04..vf07 = D_70003400` (local TRS)
   - `vf08..vf0B = D_70003440` (parent matrix)
   - 4x4 multiply by `vmulax / vmadday / vmaddaz / vmaddw` → vf12..vf15
   - Store **either** into `bone+0x90` (parent index != -1, parent matrix
     selected by `*(actor + 0x110 + 4*parent_idx) + 0x90`), **or** into
     `bone+0x90` using `actor+0xD0` (root bone, parent index == -1).
   - This is the produced world-space 4x4 the publisher (`func_00179BC0`)
     then copies into the EE BSS staging buffer for VU1 upload.

**Inputs are 100% RAM-resident.** The function reads no `D_70xxxx`
constants except the scratchpad workspace it itself initialises. **No
disc data, no id 0x71 sections, no BSS animation tables** are read by
this function. The per-bone struct fields (`+0x30` quats, `+0x60`
translation, `+0x70` Euler, `+0x88` s16 scale) are populated by some
**other system** that runs before this evaluator — that upstream system
is what reads the id 0x71 entry data (the keyframe stream in section 2's
bone-0 384-byte record, plus per-bone interpolation data) and writes
the two quats + blend factor into each bone's slot. `func_001C6DA0`
itself is purely the runtime quaternion-blend + TRS-composition step.

**Refutes the "id 0x71 is the direct anim source for func_001C6DA0"
guess.** id 0x71 keyframe streams are not consumed here. They must be
unpacked by an upstream sampler that writes the `+0x30/+0x40/+0x50` quat
pair + blend factor into each per-bone struct. Finding that sampler is
the next investigation.

**Confirmed bind-pose data flow chain (end-to-end):**
```
disc id 0x71 entry sections     [keyframe quats + per-frame data]
        |
        v
[upstream anim sampler -- unidentified function]
        |
        v   writes per-bone struct fields:
        |     +0x30 quat A, +0x40 quat B, +0x50 blend t,
        |     +0x60 trans, +0x70 euler, +0x88 s16 scale
        v
func_001C6DA0 (per-actor anim evaluator)
        |
        |   for each bone:
        |     - NLERP quats A/B by t  -> 3x3 rotation
        |     - build local TRS matrix
        |     - multiply by parent (sibling bone's +0x90)
        |       or by actor root (+0xD0)
        |     - write composed 4x4 to bone +0x90
        v
func_00179BC0 (publisher)
        |   for each bone: func_00102958(scratchpad_dst, bone+0x90)
        v
EE BSS staging at 0x002863XX..0x002893XX   (the 21-matrix buffers)
        |
        v   DMA via func_00101FE0 -> VIF1
        v
VU1 dmem (4-row matrix loaded into vf01..vf04 per kernel #5/#6/...)
        |
        v   per-vertex transform: ITOF12 vert; ACC = bone * vert; FTOI0
        v
GS via XGKICK -> rendered skinned mesh
```

**Still unknown:**
- The exact **packing of the s16 values at +0x88..0x8C**. The `1/4096`
  conversion factor matches the Q4.12 vertex-packing convention used
  elsewhere in the engine, suggesting these are a per-bone fine scale
  delta applied on top of the float `+0x7C..+0x84` scale.
- Whether `func_001CA1C0`'s output is a pure-rotation 3x3 written back
  to `bone+0x00..+0x30` (which would be a per-frame derived rotation
  matrix cache), or whether it overlaps the quat fields used as input.

## Upstream keyframe sampler chain (`func_001C8480` / `func_001C67E0` / `func_001C8D50`, 2026-05-27)

The **upstream sampler** that reads id 0x71 clip data and writes each
per-bone struct's `+0x30/+0x40/+0x50` quat-blend slot has been located.
It is split between a **clip resolver**, a **clip-change initializer**,
and a **per-frame time advancer**.

### Clip resolver — `func_001C8480` (vram 0x001C8480)

`func_001C8480(clip_table_ptr, clip_id_short)` is the id 0x71 entry
header reader.

1. Calls `func_001C6120(clip_table, clip_id)` which returns the **clip
   entry base pointer** (`v0`). This is one id 0x71 entry. The pointer
   is cached in `D_00275BF8`.
2. Reads three section-table offsets off the entry header:
   * `entry+0x08` -> `section1 = entry + *(entry+0x08)`, cached at `D_00275BF4`
   * `entry+0x0C` -> `section2 = entry + *(entry+0x0C)`, cached at `D_00275BF0`
   * `entry+0x10` -> `section3 = entry + *(entry+0x10)`, cached at `D_00275BEC`

This **confirms** the id 0x71 entry header layout from FINDINGS:
section pointers at +0x08/+0x0C/+0x10 are relative to the entry base.
All three downstream sampler stages read from `D_00275BEC/F0/F4/F8`,
so id 0x71 IS the disc-side source for the sampler.

### Clip-change init — `func_001C67E0` (vram 0x001C67E0)

`func_001C67E0(actor, clip_id, prev_t, new_t)` runs when an actor
switches animation clips (callers: `func_001749F0`, which is the
clip-switch arbiter called from `func_00179BC0` and `func_0017A130`).

1. Writes `actor+0x2C |= 0x8000 | clip_id` (clip selector with "active" bit).
2. Calls `func_001C8480(actor+0x40, clip_id)` -> populates the section
   pointers in BSS.
3. `func_00128250()` returns a frame counter/index; written to
   `bone0+0x8E`.
4. **Calls `func_001C8D50(actor+0x110, num_bones, new_t, prev_t)`** —
   this is the **per-bone sampler** (see below).
5. If `prev_t == 0` (initial bind): also calls
   `func_001C64F0(actor)` — the per-frame time advancer.

### Per-bone sampler — `func_001C8D50` (vram 0x001C8D50)

`func_001C8D50(per_bone_array=actor+0x110, num_bones, f12=new_t, f13=prev_t)`.

For each bone `i` in `[0, num_bones)`, with `bone = *(actor+0x110+4*i)`:

1. **Quat A (previous keyframe) capture.** Blend the bone's current
   `+0x30/+0x40/+0x50` quats with its current blend factor into the VU0
   scratchpad `D_70003600` via `func_001CA0A0(D_70003600, bone+0x30,
   bone+0x40, t=*(bone+0x50))`. Copy the result back to `bone+0x30`
   with `sq $v0, 0x30($bone)`. This snapshots "where we were" before
   the clip switch.
2. **Quat B (next keyframe) sample.** Call
   `func_001281C0(f12=new_t)` -> scalar int (a fixed-point frame
   index from the new time). Pass that index into
   `func_001C8F10(D_008111F0, bone_idx, frame_idx)` — a per-bone clip
   indexer that uses the cached section pointers
   (`D_00275BEC/F0/F4/F8`) to write the new target quat into
   `D_00811220/30/40` (channel state at fixed BSS slots).
3. Second `func_001CA0A0(D_70003600, D_00811220, D_00811230,
   t=*D_00811240)` blends into scratchpad again.
4. Copy scratchpad to **`bone+0x40` (quat B slot)** with
   `sq $a0, 0x40($bone)`.
5. Write blend-factor state:
   * `bone+0x50 = 0.0`            (blend t reset to 0)
   * `bone+0x54 = 1.0 / new_t`    (per-frame increment for the lerp)
   * `bone+0x60 = new_t`          (clip-segment duration)
   * `bone+0x66 = D_00811256`     (clip kind flag for channel 0)
6. **Three additional channels** are sampled in the same pattern via
   `func_001C90D0`/`func_001C92C0` for the **translation** and
   **scale** TRS components. These write:
   * `bone+0x58 = new_t`, `bone+0x68 = clip kind flag`  (channel 1 — translation)
   * `bone+0x5C = new_t`, `bone+0x6A = clip kind flag`  (channel 2 — scale)
   And both call `func_001C86A0(bone+0x0C, ...)` and
   `func_001C86A0(bone+0x24, bone+0x18)` to write the per-bone
   translation and scale source vectors used by the rest of the TRS
   build in `func_001C6DA0`.

So the per-bone struct layout is now richer:

| Offset | Type    | Meaning |
|--------|---------|---------|
| `+0x00`..`+0x2F` | mixed | TRS source vectors (translation, scale, scratch) |
| `+0x0C` | vec3    | translation source A |
| `+0x18` | vec3    | translation/scale scratch |
| `+0x24` | vec3    | scale source A |
| `+0x30` | quat    | **quat A** (rotation channel, prev keyframe) |
| `+0x40` | quat    | **quat B** (rotation channel, next keyframe) |
| `+0x50` | f32     | quat blend t (advanced each frame, clamped to 1.0) |
| `+0x54` | f32     | 1/duration -- per-frame t increment for rotation channel |
| `+0x58` | f32     | translation channel duration |
| `+0x5C` | f32     | scale channel duration |
| `+0x60` | f32     | rotation channel total duration |
| `+0x66` | u16     | rotation channel kind/flag (loaded from clip header) |
| `+0x68` | u16     | translation channel kind/flag |
| `+0x6A` | u16     | scale channel kind/flag |
| `+0x90` | mat4    | composed output (written by `func_001C6DA0`) |

### Per-frame time advancer — `func_001C64F0` (vram 0x001C64F0)

`func_001C64F0(actor, f12=delta_seconds)` advances `actor+0x3C`
(clip-time) by `delta_seconds`, clamped to `1.0`. On each frame it
re-resolves the clip via `func_001C8480` (so the cached section
pointers stay valid for this actor) and walks the keyframe stream:
for each event-table entry it compares the current frame index
(produced by `func_001281C0(actor+0x3C)`) against an event s16,
loading the matching record. The first 80 lines establish this; the
remainder is event dispatch + the per-bone blend-factor bump that
writes `bone+0x50 += per-frame increment`.

### Final end-to-end chain

```
disc id 0x71 clip entry  [header @ +0x00; sections @ +0x08/+0x0C/+0x10]
         |
         v   func_001C6120(clip_table, clip_id) -> entry base
         v   func_001C8480 caches base + sections into BSS globals
         v     D_00275BF8 = entry base
         v     D_00275BF4 = entry + *(entry+0x08)  (section 1)
         v     D_00275BF0 = entry + *(entry+0x0C)  (section 2)
         v     D_00275BEC = entry + *(entry+0x10)  (section 3)
         |
         v   clip-change: func_001C67E0 -> func_001C8D50(actor+0x110, N)
         v     per bone i:
         v       quat A <- NLERP(old A, old B, old t)   (snapshot current pose)
         v       sample new keyframe via func_001C8F10
         v       quat B <- NLERP(scratch, new clip quat pair, clip-mix t)
         v       bone+0x50 = 0; bone+0x54 = 1/duration; bone+0x60 = duration
         v       sample translation channel -> bone+0x0C, channel state
         v       sample scale channel       -> bone+0x24, channel state
         |
         v   each frame: func_001C64F0 advances bone+0x50 toward 1.0
         |
         v   func_00179BC0(actor, ..., src_clip):
         v     func_001749F0 (clip switch detect; may re-invoke 001C67E0)
         v     func_001C6DA0(actor)  -- per-bone TRS+NLERP using +0x30/+0x40/+0x50
         v                              writes 4x4 to bone+0x90
         v     for each bone: func_00102958(scratchpad, bone+0x90)  (publish)
         |
         v   EE BSS staging @ 0x00286340 .. 0x00288D40   (21-matrix buffers)
         |
         v   DMA via func_00101FE0 -> VIF1
         |
         v   VU1 dmem -> vf01..vf04 per skinning kernel
         |
         v   GS via XGKICK -> rendered skinned mesh
```

### What's still unknown

- Exact format of the **event-table records** that `func_001C64F0`
  walks (it loads s16 event-id pairs at section3+4*i, with payload
  s16 at section3+4*i+2).
- The **scale-channel** decoder (`func_001C92C0`, called for `0x18(actor)`
  output) has not yet been read line-by-line; by structural symmetry with
  the rotation/translation samplers it almost certainly uses the same
  12-byte-per-keyframe record stride and a 3-channel "truncated-float"
  sample, but the bit width per channel needs confirmation.

## Keyframe stream format — decoded 2026-05-27 (`func_001C8F10`/`func_001C90D0`/`func_001C84D0`/`func_001C85D0`)

The rotation, translation, and scale samplers all share the same outer
structure: each is invoked as `f(actor, bone_idx, time_int_frames)`, indexes
a cached section base via `bone_idx`, walks a list of fixed-stride
keyframe records, picks the one straddling the integer time, decodes the
sample, and stores it into the actor's "next keyframe" slot.

### Section indexing (bone_idx -> stream start)

The id 0x71 entry-section bases cached at runtime are u32 directories of
per-bone byte offsets:

| symbol         | what it points at         | consumed by                  |
| -------------- | ------------------------- | ---------------------------- |
| `D_00275BF4`   | id 0x71 entry section 1   | `func_001C8F10`  — rotation  |
| `D_00275BF0`   | id 0x71 entry section 2   | `func_001C90D0`  — translation; also `func_001C92C0` (scale) |

For each sampler:

```
section_base = D_00275BFx                            ; u8 *
rel_off      = ((u32 *) section_base)[bone_idx]      ; u32 little-endian
stream_start = section_base + rel_off                ; first keyframe record
```

So section1/section2 begin with a `nbones * 4`-byte table of u32 relative
offsets, immediately followed by the per-bone keyframe streams. (FINDINGS
already noted section 1 was *not* per-bone bind-pose vertex data for the
animated-clip files; this is what it actually is — the per-bone rotation
keyframe directory + streams.)

### Per-keyframe record (12 bytes, both channels)

```
offset  size  field
+0x00   10 B  sample payload (packed; see decode below)
+0x0A    2 B  u16 t_next   -- the time at which THIS record's sample is reached
```

The sampler walks `record[i]` until `time_int < record[i].t_next AND
time_int >= record[i+1].t_prev`. Because successive records overlap their
time field (`+0xA` of record[i] is read as "this record's end time", and
`+0x16 == +0xA` of record[i+1] is "next record's end time"), each pair
defines a `[t_prev, t_next)` interval where:

```
t_prev = (u16) *(stream + i*12 + 0x0A)
t_next = (u16) *(stream + i*12 + 0x16)   ; == t_prev of record[i+1]
```

The blend parameter written to `D_00811240` (the value the caller later
hands to the SLERP/LERP at `f12`) is the **standard linear interpolation
coefficient**:

```
t_blend = (time_now_int - t_prev) / (t_next - t_prev)   ; in [0, 1)
```

(Implementation detail: time values are loaded with `lhu` and converted via
the MWCC unsigned-16-to-float idiom `srl/andi/or; cvt.s.w; add.s`, so they
are **u16 frame counts**, not signed. Sample-decode helpers
`func_001C84D0` and `func_001C85D0` are called with `$a0 = record + 0x00`,
so the payload starts at byte 0 of the record.)

### Sample payload: "top-N-bits-of-IEEE-float" packing

Both `func_001C84D0` (rotation, 4 channels) and `func_001C85D0`
(translation, 3 channels) use the same pattern, repeated per channel:

1. Stitch a 32-bit word from several `lhu` loads with shifts and `or`s,
   so that bit `k` of the channel maps to bit `k - N_low` of the assembled
   word (i.e. the channel's "msb-first" bits land at bit positions
   `Nlow..31` of the temporary).
2. `sw` the word to a scratchpad u32 at `0x70003600`, then `lw` and
   `sll` it left by `(32 - channel_width)` so the channel's bits land in
   bits 31 downto `(32 - W)` of the temporary.
3. Re-store, then `lwc1` the same scratchpad address into an FPU
   register. **This is a bit-cast, not a value conversion.** Because the
   channel was placed in the high bits, the resulting float has the
   stored bits as: sign (bit 31) + exponent (bits 23..30) + top mantissa
   bits, with the rest of the mantissa zero. The on-disc value is
   therefore the **top W bits of a standard IEEE 754 single-precision
   float**, with the low `32 - W` mantissa bits truncated to zero.

The shift amounts give the channel widths:

| channel    | code path                              | shift used | width W |
| ---------- | -------------------------------------- | ---------- | ------- |
| rot.x/y/z/w | `func_001C84D0`                       | `sll $r,12` | **20 bits** |
| tx/ty/tz   | `func_001C85D0`                        | `sll $r, 6` | **26 bits** |

Bit layout within the 10-byte payload:

Rotation (4 × 20 bits = 80 bits = 10 bytes):
```
bits  0..19  : qx        (top 20 bits of an IEEE float, low 12 mantissa = 0)
bits 20..39  : qy
bits 40..59  : qz
bits 60..79  : qw
```

Translation (3 × 26 bits = 78 bits, padded into 10 bytes):
```
bits  0..25  : tx        (top 26 bits of an IEEE float, low 6 mantissa = 0)
bits 26..51  : ty
bits 52..77  : tz
bits 78..79  : padding   (last 2 bits of the 10-byte cell)
```

Encoder side: pack `f = (uint32_be) bit_cast<u32>(float) >> (32 - W)` into
the channel; decoder side: `float = bit_cast<float>(packed_channel <<
(32 - W))`. So both rotation and translation are **lossy floats** (not
fixed-point, not smallest-three) — just IEEE-754 with the bottom of the
mantissa shaved off. ~9 mantissa bits left for rotation (precision
~1/512 per channel, then renormalized to a unit quat in the SLERP),
~15 mantissa bits left for translation (precision ~1 part in 32k of the
exponent's local scale — plenty for world-space positions).

The decoded sample is written to actor offsets:

| sampler          | actor write           | meaning                       |
| ---------------- | --------------------- | ----------------------------- |
| `func_001C8F10`  | `+0x40` (vec4, via `func_001C84D0(payload, actor+0x40)`) | quat B (next keyframe rotation) |
| `func_001C90D0`  | `+0x0C` (vec3, via `func_001C85D0(payload, actor+0x0C)`) | translation sample            |
| `func_001C92C0`  | (not yet read)        | scale sample                  |

And the blend `t` to `D_00811240` (= `0x50(actor)`), with the time
deltas to `0x60(actor)` (`t_next - time_now`) and `1/(t_next - t_prev)`
to `0x54(actor)`. The slerp/lerp downstream (`func_001CA0A0`) consumes
`D_00811240` as its `f12`.

### Why the 0x70003600 scratchpad?

EE address `0x70003600` is **inside the EE Scratchpad** (16 KB SPRAM at
`0x70000000`). MWCC is using it as a uncached u32<->float bit-cast
buffer (the kind of thing a C compiler with a `union { u32 i; float f; }`
would emit on a CPU with no `mtc1`/`bitcast` shortcut; here MWCC emits
plain `sw`+`lwc1` against scratchpad memory, which is single-cycle).
This is a strong fingerprint for recognising "truncated-IEEE bit-cast"
elsewhere in the binary.

## Per-bone scale fields — writers identified, NOT a disc-side channel (2026-05-27)

Following the keyframe-stream decode, the open question was: who fills
`+0x7C..0x84` (f32 vec3 scale) and `+0x88/+0x8A/+0x8C` (s16 Q4.12 fine
scale) on each bone struct, since `func_001C6DA0` consumes them but the
keyframe sampler chain (`func_001C8D50` → `func_001C90D0` /
`func_001C92C0`) only writes the +0x18..+0x20, +0x24..+0x2C, +0x30..,
+0x40.., +0x50..+0x60 region (rotation + translation samples). Search
of all splat asm for `sh ..., 0x88($x)` / `sh ..., 0x8A($x)` /
`sh ..., 0x8C($x)` returns ~9 functions; only four are clearly bone-
struct writers, and **none of them is a per-frame keyframe decoder**:

| function | what it does | when it runs |
|---|---|---|
| `func_001C6200` | bone-state **reset**: for every bone `i < actor+0xC`, writes `+0x88=+0x8A=+0x8C=0x1000` (Q4.12 → 1.0), `+0x7C=+0x80=+0x84=0`, `+0x70=+0x74=+0x78=0`, `+0x64=-1` (clear current clip), then `jal func_001029C0` (probably matrix-identity). | Skeleton init / clip-clear |
| `func_001C62C0` | same as `func_001C6200` but with bone count taken from `$a1` and base pointer from `$a2`. Companion entry point. | Skeleton init |
| `func_001C63E0` | same shape; takes pointer/count in different registers. | Skeleton init |
| `func_001C06E0` (lines 484–502 and 603–611) | mutates **bone[0] only** (`actor+0x124`) — writes the same s16 value (`actor+0x2A`) to `+0x88`, `+0x8A`, `+0x8C`. `+0x2A` is a per-actor counter that decrements by `0x40` per frame and is clamped to `>= 0x1000` (1.0). This is a **uniform-scale pulse** on the actor's root bone — clearly a gameplay/animation effect (hit-react, breathing, growth), not skeletal animation. | Gameplay state update |
| `func_001BDD70` / `func_001BDCA0` | iterates `D_00275B40[1..0x10]`, writes a decaying scalar to `+0x80` (just one axis of the float vec3) and a uniform `0x1000` to all three s16 fine-scale slots. Looks like a **global "wobble" decay loop** for up to 16 actors / particles. | Gameplay effect |

No writer matches "per-frame per-bone keyframe decoder reading from
disc data" — there is **no scale animation channel** in the id-0x71
file format.

### Implication for the model exporter

The "default scale=1" that `tools/extract_models.py --rigged` currently
applies to every bone is **the correct rest-pose value** and matches
what the engine's own skeleton-init paths write:

- `+0x88 = +0x8A = +0x8C = 0x1000` in Q4.12 → multiplier `0x1000 *
  2^-12 = 1.0`, which `func_001C6DA0` multiplies via `vmulx.xyz` into
  each axis basis (identity → unchanged).
- `+0x7C = +0x80 = +0x84 = 0.0` — these are an *additive* float vec3,
  applied via `func_001C94B0` to the actor-root path. Zero ≡ no
  contribution.

So the rigged export is already as accurate as it can be from disc data
alone — the scale fields are **runtime gameplay state**, not part of
the animation file format. They do not need to be parsed in
`anim_decoder.py`.

Confirming evidence: section 3 of id 0x71 ("uniform 36 bytes per bone"
flagged by the previous agent as a candidate) is therefore **not**
scale keyframes; an earlier characterization as "events" stays the
most plausible reading.

### What's still load-bearing

If the model viewer ever animates the actor's root bone during certain
gameplay states (hit-react, idle breathing), it would need to mirror
`func_001C06E0`'s `actor+0x2A` clock to recover that effect — but that
is a runtime-only animation that has no representation on disc.

## `MUSIC.DAT` track listing

`MUSIC.DAT` decodes to 55 tracks. Per the user (cross-referenced with an online
soundtrack listing): **25 are the official soundtrack; the remaining 30 are
cutscene audio.** Not yet separated or labelled — a roadmap item.
