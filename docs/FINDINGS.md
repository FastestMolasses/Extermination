# Extermination — Reverse-Engineering Findings

Technical reference for formats and facts established so far. The authoritative,
exhaustive format details live in the docstrings of the `tools/` scripts; this
file summarises them and records findings that have no other home.

## glTF (.glb) export of the player rig (2026-05-27)

> SUPERSEDED (2026-06-09): the "per-bone object-space vertex packets" this
> exporter consumed are animation keyframes ("id 0x74 prefix is ANIMATION,
> not geometry"), so the geometry it produced was garbage. export_gltf.py
> was re-pointed at the corrected skinned pipeline in s4d (stage-2 strips,
> per-vertex node binding, glTF skin, keyed-container animations);
> `triangulate_bone`/`load_per_bone_meshes*` no longer exist. Kept for the
> historical record only.

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

**Per-vertex bone binding lives in the position w field (~~PARTIALLY
DECODED 2026-05-27~~ FULLY DECODED 2026-06-09 s3 — the reading below is
superseded: the W bit pattern is [dmem matrix address | strip flags],
NOT a k/512 palette selector; see "Skinned-character pipeline FULLY
DECODED").** The 64-byte vertex record's `+0x3C` `w` field
is NOT a pure sign flag. Empirically across all 317 MESH blocks of
the player mesh (`chunk21/f17_id8f.bin`):

  * `sign(w)` carries the documented `+/-1` strip-priming convention,
    but `w - sign(w)` is a quantised offset of the form
    `k / 512 + epsilon` with `k` in a tiny per-block set: most blocks
    expose only `k in {-3, -1, 0, 2}` (4 distinct integers, but the
    universe of `k` across the file is the same 4 plus close
    near-quantisation neighbours).
  * Per-block distinct-k counts: mode = 4, range 3..15, distribution
    `{3: 1, 4: 61, 5: 5, 6: 8, 7: 9, 8: 34, ...}` across 134 blocks
    sampled. Quantisation: `(w - sign(w)) * 512` rounds to integers
    within +/- 0.01; the residual ~0.001-0.010 above each integer
    varies per block (a per-block FLOAT base, likely the bone-table
    pointer or bone-base index).

Interpretation: each MESH block ships with an implicit small bone
TABLE (~4 bones / block) and each vertex's `k = round((w-sign(w))*
512)` is its per-vertex bone-selector index into that table. This
matches PS2 VU1 skinning idiom: the engine uploads a few joint
matrices into VU dmem ahead of each vertex batch, the microcode
indexes them with the packed selector, and emits world-space verts
via XGKICK. The per-block-bone count of ~4 is also consistent with
the four-matrix `vf01..vf04` register convention seen elsewhere in
the decomp.

**Still unresolved.** (a) Where the per-block bone-INDEX table lives
(mapping `k -> global bone-id` for each MESH block). The 0x48-byte
preamble at `+0xe800` immediately before the first MESH block is a
uniform block-table header (descriptor `+0c44 / +14 / +44100` plus a
single axis-aligned bbox plus zero padding) -- not per-block. The
`28 x u32` table at `+0x229c` is too short (28 entries for 317
blocks). The most-likely candidate is the residual float offset on
`(w-sign(w))*512` itself encoding the per-block base, but proving
the decode needs the VU1 microcode -- this is the same wall hit when
decoding the per-bone VIF stream's packed normal/lighting field.
(b) Coordinate-frame relationship: MESH-block positions span roughly
the same X/Y range as the rigged skeleton's world bbox but with
swapped/different Z -- a "swap YZ" mapping reduces the avg
nearest-bone distance from 2.67 -> 2.30 units (still 9/30 bones at
d>3), so the frame is close but not identical. The bone-local frame
of each block needs the bone-id mapping to validate. (c) Whether the
per-bone VIF stream has parallel UVs shipped via another VIF UNPACK
(currently the per-bone records are 12 bytes each and fully
accounted for as POSITION+packed_normal+vid).

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

### Global GS VRAM residency map (2026-06-01)

`tools/vram_residency.py` builds ONE residency map for the whole disc so any
DBP can be resolved regardless of which file uploads it (the cross-file
residency gap above). It walks every file under `extract/`, records each GS
upload's full descriptor — `(DBP, DBW, DPSM, TRXREG w×h, TRXPOS dest x/y,
source file, byte offset)` — and indexes them `DBP → [uploads]`. The scan is
cached to `scratch/vram_residency.json` (git-ignored); a corpus
size/mtime signature invalidates it. Reuses `extract_subtextures.decode_transfer`
for pixels, so it owns no duplicate decode logic.

**Disc-wide census (definitive).** Across all 605 extracted files there are
exactly **113 GS texture uploads** at only **5 distinct DBPs**:
`{7040, 7424, 10752, 12672, 14592}`. Every upload is **PSMCT32 (DPSM=0),
DBW=4** (256-texel-wide PSMCT32 → 512-wide PSMT8 sheet). The same DBP is
re-targeted by many files (VRAM is 4 MB, so a slot is reused across
levels/frames): DBP 10752 has 40 uploads, 12672 has 34, 7424 has 18, 14592
has 16, 7040 has 5. Each reuse may have **different dimensions** (e.g. DBP
14592 appears as 512×384, 512×256, 512×320, 512×64, 512×448, 512×640) — so
**DBP alone is ambiguous**; the consumer must disambiguate by same-chunk-dir
proximity (`prefer_dir`) or expected `(PSM, dims)`.

**No GS CLUT uploads exist.** The scanner found **zero** small/palette-shaped
GS transfers (it tags them `clut=True` if it does). This **confirms** the
earlier finding that CLUTs are NOT shipped as GS upload packets — they are the
raw 1024-byte `[R G B A]·256` blobs detected by `tools/clut.py`. The
color-CLUT binding question gains no new lead from GS packets; it remains a
boot-ELF per-asset palette LUT / VU1 TEX0-setup problem. (The CLUT-upload
hunt the task hoped might crack color came up empty — the palettes are simply
not GS-packetised in this game.)

**Resolution gap, quantified.** Level materials reference **31 distinct DBPs**
but only 5 are ever uploaded. The high-traffic sheets dominate (DBP 10752:
21465 strips, 12672: 29302, 14592: 12519) and resolve exactly. Near-miss DBPs
(material DBP **14562** ↔ upload **14592**, 30 words apart) are the same
physical sheet and resolve by snapping within a small tolerance (default 64
VRAM words). The remaining ~26 low DBPs (548…2581, 1152, 8802) have **no
upload anywhere on disc** — they are textures resident from common/UI packets
that live outside `DATA.DAT` (boot-ELF data or a shared bank not in the
extracted tree), so they honestly stay gray. The affine `sheet_field→DBP` map
is exact only on the universal trio, so these low addresses don't snap to a
real slot either.

**Wired into `export_gltf.py`.** Both the character path
(`_find_transfer_for_dbp`) and the level path (`_resolve_level_sheets`) now
consult the residency map first (cached, disc-wide, with same-dir preference
and near-miss snapping), falling back to the legacy per-dir scan only if the
map is unavailable. The default cross-dir tiebreak is the lowest-sorted source
path (deterministic; reproduces the previously-validated per-asset bindings,
e.g. the player's DBP 14592 → `chunk04.n2/f00_id44.bin` 512×384) rather than
"largest sheet" (which would grab an unrelated level's reuse of the slot).
Level aggregate: **91/143 → 93/143** sheets resolved (the +2 are 14562 snaps
in `chunk08.n0`/`chunk08.n1`). The player still resolves 3/3 with identical
sources to the validated baseline.

### Brute-force CLUT recovery by coherence scoring — NEGATIVE RESULT (2026-06-01)

`tools/clut_bruteforce.py` attempts to recover the per-sheet colour palette
**without the engine**: collect every 1024-byte CLUT-shaped blob disc-wide,
apply each to a texture sheet's PSMT8 indices, and rank by how "coherent" /
natural the resulting colour image looks. **Conclusion: coherence scoring
cannot disambiguate the colour palette. The binding genuinely needs the
engine (PCSX2 / boot-ELF trace).** Details below so this is not re-attempted
blind.

**Candidate pool.** Walking `extract/` with `clut.find_clut_candidates`
(α∈[0,0x80], ≥100 entries at 0x80, ≥32 distinct RGB) yields **424 candidate
hits across 50 files → 361 UNIQUE deduped 1024-byte blobs** (298 appear once,
63 appear at two locations). Pool chroma (mean per-entry RGB max−min, 0..255):
min 1.1, median 18.2, p90 41.9, max 68.3; **98 of 361 are genuinely colourful
(chroma > 30)** — so the pool is not all grayscale; vivid palettes exist.

**Sheets tested.** All 5 disc-wide texture DBPs (the only sheets that exist —
see "Global GS VRAM residency map"): 7040 (chunk03, 512×64), 7424 (chunk27
character, 512×960), 10752 (chunk01 — the **EXTERMINATION title screen**,
512×768), 12672 (chunk04.n0 level, 512×896), 14592 (chunk04.n2 level,
512×384). Each is a packed atlas (confirmed by the grayscale reference render).

**Metric.** Primary score = **Pearson correlation between |index Δ| and
|colour Δ| over adjacent NON-equal-index texel pairs** (subsampled to ≤400 K
pairs). This is degeneracy-robust by design: a flat/gray palette has ~zero
colour-Δ variance so it cannot fake a high correlation (the failure mode of a
plain "minimise adjacent RGB difference" smoothness score, which was tried
first and trivially rewards desaturation). Tie-breakers / diagnostics: mean
adjacent colour-Δ (smoothness), luminance-monotonicity over used entries,
per-entry chroma, distinct-colour degeneracy guard. Both the raw blob and its
PSMT8/CSM1 entry-swap form are scored.

**Why it fails — the decisive finding.** Across **all 5 sheets the SAME blob
wins** (`chunk19.n1/f01_id44.bin @ 0xb6690`), with z ≈ 2.5–3.2 above the mean
correlation each time. That blob has **chroma 4.6 (essentially grayscale)** and
is a monotone luminance ramp (Pearson r(index, entry-luminance) = −0.90 over
all 256 entries). It wins everywhere precisely *because* the game's PSMT8
indices are luminance-ordered (documented): **any** roughly-monotone gray ramp
correlates strongly with luminance-ordered indices, so the metric just
re-discovers the grayscale ordering and the correct *colour* palette gets no
distinguishing advantage. A single blob being optimal for four unrelated
sheets is the tell that the score keys on a generic property, not a per-sheet
binding.

**Forcing colour confirms it.** Restricting to vivid candidates (`--min-chroma
30`, 98 candidates) and re-ranking, the best colour palettes for the level
sheets (12672 / 14592) decode to **confetti / speckle noise** — random
red/gray scatter with no coherent textured surface — visibly *worse* than the
grayscale reference. No vivid CLUT in the pool produces a coherent colour image
for any sheet.

**Two structural reasons coherence scoring is the wrong tool here:**
1. **Luminance-ordered indices defeat the metric.** Smoothness/correlation is
   maximised by *any* monotone ramp; the documented index ordering means
   grayscale already satisfies it, so colour adds no score.
2. **A sheet is an atlas of many sub-textures that may each use a different
   CLUT.** Scoring one global palette per sheet is ill-posed even in principle
   — there is no single "correct" sheet-wide palette to find.

**Status:** brute-force coherence scoring is **insufficient**; recorded as a
valid negative result. The colour-CLUT binding remains a boot-ELF per-asset
palette-LUT / VU1 TEX0-setup problem (trace the PSMT8 `TEX0` `CBP` source in
the engine). `tools/clut_bruteforce.py` is retained as the disc-wide candidate
collector (361-blob pool) and a reusable scorer; output PNGs land in
`scratch/clut_bf/` (git-ignored). Not wired into any exporter — the default
colour path stays the identity grayscale CLUT.

### Resident CLUTs from a live PCSX2 capture (2026-06-02)

> **CORRECTION (2026-06-09): every VRAM read in this section and the menu
> section below was made with the WRONG freeze-blob base (509; correct is
> 425 — see "Texture COLOR recovered"), i.e. skewed +84 bytes.** 84 = 21
> CLUT entries × 4, so reads stayed entry-aligned — palette-shape
> validation and colour families were right, but exact byte content,
> "13 resident CLUTs", and the nearest-diff figures were measured on
> shifted windows. **Re-verified at the corrected base 425 (2026-06-09,
> fixed `gs_vram.py`, save states 01 = gameplay snow / 02 = title):
> the decisive negative HOLDS — 17 (state 01) and 41 (state 02) resident
> CLUTs vs the same 361-blob disc pool give ZERO exact matches (nearest
> max-byte-diff 127–224), in both swizzle directions.** The
> runtime-synthesised conclusion stands on clean reads; the per-entry EE-RAM
> probes below sampled genuine (entry-aligned) palette words, so their
> qualitative conclusion is unaffected. The CBP block addresses cited below
> are confirmed at block granularity (8368/8384/8402-runs re-found at base
> 425). The original `/tmp` captures are deleted; the save-state re-runs
> supersede their numbers.

`tools/gs_vram.py` reads the **resident** CLUTs straight out of a captured GS
local-memory image (a PCSX2 save-state GS *freeze* blob), to sidestep the
unknown offline binding entirely: the running engine has already uploaded the
real palettes into VRAM. Capture used: an in-game frame (player soldier in the
snowy-gate area; see `/tmp/cap2/Screenshot.png`, user-local, never committed).

**GS local-memory layout in `gs.bin`.** The capture's `gs.bin` is a GS *freeze*
blob, NOT the GS-dump packet format (no `0xFFFFFFFF` magic). ~~It is a small
fixed header followed by the 4 MB local memory, which ends exactly at EOF, so
**VRAM word 0 is at byte offset `len(gs.bin) - 0x400000` = 509 (0x1FD)** for
this v9 freeze~~ **[VOID — corrected 2026-06-09: the 4 MB is followed by 84
trailing state bytes; VRAM word 0 is at `len - 0x400000 - 84` = 425]**. A CLUT
pointer `CBP` (in 256-byte GS blocks) maps to file offset `base + CBP*256`.
The header is the GS display/PMODE/DISPFB state only; the internal drawing
`TEX0` (active CBP) is **not** in it — it lives in the emulator's
`PCSX2_Internal_Structures.dat`, not parsed here.

**Resident CLUTs found.** A full VRAM block-scan (α∈[0,0x80], ≥100 entries at
0x80, ≥32 distinct RGB) finds **13 resident palette-shaped CLUTs**, clustered in
two runs of contiguous GS blocks: **CBP 8368–8371** (vivid, chroma 50–65) and
**CBP 8384–8388 / 8402–8403 / 13559–13560** (chroma 4–28). The 8368-run swatches
ARE genuine vivid palettes (purples/blues/greens/oranges). The contiguous runs
imply **several CLUTs packed back-to-back** — consistent with one atlas's
sub-textures each using a different palette.

**CBP source (ee.bin).** Scanning EE RAM for PSMT8 `TEX0` (PSM=0x13) writes with
`CLD≥1` finds runtime bindings; the clean ones (`CPSM=0` PSMCT32, `CSM=0` CSM1)
point a character sheet `TBP0=7424` at CBP values in the 8272–8800 range, and
`TBP0=12800 → CBP=13560`. So the resident 8368-run CLUTs are bound to the
on-screen character/level textures, exactly where the residency map places those
sheets.

**THE BINDING — still not recoverable offline (decisive negative).** Cross-
referencing every resident CLUT against the 361-blob disc pool gives **zero
exact matches** (nearest blob max-per-byte-diff 128–235). Stronger tests also
fail: (a) the resident **RGB content (alpha-free, 4-byte stride, both swizzle
forms) appears nowhere on disc** — so it is not a disc blob with a different
alpha convention; (b) the best **per-channel linear tint fit** of any disc blob
to a resident CLUT has RMS ≈ 68/255 — so it is not a uniformly tinted disc blob
either. **Conclusion: the resident CLUTs are runtime-synthesised** (most likely
per-material/ambient lighting modulation baked into the palette before upload —
plausible for the blue-white snowy scene), not verbatim or simply-tinted copies
of on-disc blobs. There is therefore **no offline (sheet → disc-blob) binding
rule to recover from this capture**; the disc blobs are palette *inputs*, and the
GS-resident palette is an engine *output*. Recovering colour offline now requires
reproducing the engine's palette-build (decompile the PSMT8 `TEX0`/CLUT-DMA
setup and any per-material colour modulation), not a lookup.

**Follow-up probe (2026-06-02): the CLUT is assembled per-entry, not staged.**
Searched the capture's EE RAM (`ee.bin`) and scratchpad for the resident
1024-byte CLUT (CBP 8368) in both swizzled and un-swizzled forms — **no
contiguous match in either, anywhere**. But the palette's individual RGBA
colours **do** appear in EE RAM: 8/8 sampled distinct entries are each found as
4-byte words somewhere in `ee.bin`. So the engine never stages a complete
palette buffer that it then DMAs; it writes the CLUT **per entry directly into
GS VRAM** (or via small scattered transfers), drawing each colour from
material/lighting sources that live separately in EE RAM. This is why no disc
blob and no EE buffer matches the whole CLUT. Implication: reproducing colour
offline means reconstructing a *per-entry* synthesis (base material colour ×
lighting), and the result is inherently lighting-dependent — there is no single
"true texture colour" stored anywhere. The cheapest remaining experiment is a
**neutral-lit capture** (e.g. a menu / the title screen while displayed): if
the synthesis is base×light, a neutrally-lit frame's resident CLUT should
approach the unmodulated base palette and *then* may match a disc blob,
recovering at least the base colours.

**Pipeline proven (the milestone).** Applying a resident CLUT (un-swizzled via
`csm1_unswizzle_clut`) to a *known-coherent* sheet — the EXTERMINATION title
screen (DBP 10752, `chunk01/f00_id06.bin`, decodes cleanly in grayscale) —
produces a **structurally perfect, readable image** (the title text and X-ray
hand) with **wrong colours** (purple/blue/peach over a monochrome source). That
is the expected signature of a valid-but-mismatched palette: the apply +
un-swizzle path is provably correct, and the colours are wrong only because that
CLUT belongs to the snowy level, not the title. No exporter was wired to colour
(no binding rule emerged); the default path stays identity grayscale. Tool:
`tools/gs_vram.py` (`--scan`, `--crossref`, `--dump CBP`); colour PNGs land in
`scratch/color/` (git-ignored).

### Neutral-lit MENU capture — base palette is STILL synthesised (2026-06-02)

> **CORRECTION (2026-06-09): same +84-byte read skew as the section above
> (base 509 → 425). The headline negative was re-verified clean: at base
> 425 the title-screen state (02) yields 41 resident CLUTs, ZERO exact
> matches vs the 361-blob disc pool (nearest max-byte-diff 127–224). The
> conclusion of this section stands. Superseded on skewed reads: the
> per-CLUT inventories (37/19 CLUTs) and the maxdiff figures (154–235);
> the RGB-only / alpha-normalised / tint-fit variants and the EE-RAM
> per-entry counts were not re-run (captures deleted) but sampled
> entry-aligned, genuine palette words. The CBP runs (8368, 12158–12166,
> 12174, 12288) are confirmed resident at base 425 on state 02. The
> screenshot-exact title-screen renders in "Texture COLOR recovered"
> (clut_pair.py, base 425) supersede the colour-family renders here.**

The payoff experiment the prior section proposed: two **flat-lit menu**
captures (`/tmp/menu02` = MAIN MENU / EXTERMINATION title screen in colour;
`/tmp/menu03` = OPTIONS screen; both user-local, never committed). The
hypothesis was that with no 3D level ambient, the resident CLUT would equal
the *unmodulated base* palette and therefore match an on-disc blob, recovering
offline colour. **Hypothesis disproven — base palette is also engine-built.**

**Step 1 — title-screen colour reproduction VALIDATED (milestone hit).** The
title texture (DBP 10752, `chunk01/f00_id06.bin`, 512×768 PSMT8) is one packed
atlas holding the *entire* title screen (the title wordmark, the X-ray-hand
circle, the menu strings, the bottom menu items — all legible; the wordmark is
stored mirrored and flipped on draw). Applying the menu capture's **resident**
CLUTs (un-swizzled via `csm1_unswizzle_clut`) reproduces the **on-screen colour
families**: the atlas is **multi-palette** — different sub-regions sample
different resident CLUTs at draw time. Empirically on menu02:

  * **CBP 12174-run (purple/silver)** → the EXTERMINATION wordmark + menu text
    (matches the on-screen silver/purple title band) over a transparent ground.
  * **CBP 12158-12166-run (magenta / red / purple / blue)** → the X-ray hand
    (red-orange bones, magenta/purple glow) — matches the on-screen hand.
  * **CBP 12288-run (blue / cyan / steel-gray)** → a blue/steel variant region.
  * **CBP 8368-run (blue / black)** → persistent UI/font palette (also present,
    byte-identical offset, in the earlier *gameplay* capture — a global UI CLUT).

Each render is structurally perfect and the colours match the screenshot's
families, proving the per-frame colour path on a known target. (A 16×16 CLUT is
1024 bytes = **4 GS blocks**; `scan_vram_cluts` reports each 256-byte block as a
sliding-window "hit", so the true palettes start on 4-block-aligned CBPs and the
adjacent CBPs are shifted views of the same data.) Colour PNGs:
`scratch/color2/menu02_title_cbp{12174,12159,12288,8368}_unsw.png` (git-ignored).

**Step 2 — THE KEY TEST: menu CLUT vs on-disc blob = NO MATCH (decisive).**
Cross-referencing **every** resident menu CLUT (menu02: 37; menu03: 19) against
the 361-blob disc pool, with 4-block alignment, **both** CSM1 swizzle
directions, and three match keys — **exact 1024-byte**, **RGB-only
(alpha-stripped, 768 B)**, **alpha-normalized (all α→0x80)** — gives **ZERO
matches**. Nearest disc blob (RGB-only max-per-byte-diff) is **154-235/255** for
every vivid CLUT (a real match is 0). So the neutral-lit base palette is *not*
a disc blob under any alpha convention or swizzle.

**Per-entry synthesis re-confirmed.** For each vivid resident CLUT, **12/13
distinct chromatic entries appear as standalone 4-byte words in `ee.bin`**, but
the contiguous 1024-byte palette appears nowhere on disc and nowhere in EE RAM.
The engine assembles each CLUT **per entry** directly into GS VRAM from
material/colour sources scattered in EE RAM; it never stages or ships a whole
palette buffer. This holds in *both* menu frames and matches the gameplay-capture
finding exactly.

**Cross-check (menu03 / Options) consistent.** The same 12158-12166 hand-region
CLUT run is resident at the **identical VRAM offset** in both menu frames (a
persisted UI palette); zero disc matches; per-entry colours present in EE RAM.
The one difference — CBP 12288's colours are absent from menu03's EE RAM (0/13)
because the Options screen doesn't load that atlas region — further confirms the
palette set is assembled per-frame, not loaded verbatim.

**CONCLUSION — offline colour is not recoverable by lookup; it is engine-bound.**
Even flat menu lighting yields a synthesised palette, so there is **no
(texture → disc-blob) base-palette binding rule** to extract. The disc blobs are
palette *inputs* the engine combines per-entry (base material colour × lighting /
material params); the GS-resident palette is an engine *output*. Recovering
colour offline requires **reproducing the engine's per-entry palette build**
(decompile the PSMT8 `TEX0`/CLUT setup and per-material colour modulation), not a
table lookup. **The extractors therefore stay on the identity grayscale CLUT**
(no exporter wired to colour). The resident-CLUT path (`tools/gs_vram.py`) and a
live PCSX2 capture remain the only way to obtain a *specific frame's* colours.

## Audio — VAG ADPCM

All game audio is **PS2 SPU2 ADPCM ("VAG", 16-byte frames)**. Tools:
`tools/audio_export.py` (consolidated exporter, 2026-06-09 — streams + banks,
default output `extract/audio_decoded/`, git-ignored) and the older
`tools/decode_sound.py` (same decode core).

Full inventory (verified by a complete re-export 2026-06-09):

- **SFX**: `SShd` sound-bank containers inside `DATA.DAT` files — **39 banks,
  1146 sound references, 241 unique sounds** after content dedup
  (`audio_export.py sfx`). A bank's body is concatenated VAG blocks; a frame
  whose flag byte has bit 0 set ends a block, 1-frame blocks are terminators.
  Most banks share a common ~19-sound base set (player/UI) plus per-area sets.
- **`VOICE.DAT`** (27.8 MB): one raw **mono** VAG stream, no container, no
  end-flags. Split on silence gaps → **116 clips** (997 s @ 48 kHz).
- **`MUSIC.DAT`** (315 MB): one raw **interleaved-stereo** VAG stream —
  alternating **64-frame (1024-byte) L/R blocks**. Deinterleave then decode
  (`audio_export.py music`) → **55 stereo tracks** (5633 s @ 48 kHz).

**Interleave verified empirically** (`audio_export.py detect-interleave`):
deinterleave at a candidate block size, decode, and compare the mean decoded
sample step across block boundaries vs inside blocks. Only 64 frames gives a
boundary/in-block ratio ≈ 1 (continuous audio); every wrong candidate splices
unrelated audio at boundaries:

| candidate (frames) | 16 | 32 | **64** | 128 | 256 | 512 |
|---|---|---|---|---|---|---|
| boundary/in-block step ratio | 1.35 | 1.47 | **0.99** | 1.42 | 1.24 | 1.31 |

### Sample rate

Streamed audio plays at **48000 Hz** (PS2 SPU2 native rate). Evidence:
`music/clip_0000` (End Credits) matches an official-soundtrack rip — the rip is
encoded at 48000 Hz, and our clip's frame count / the rip's duration ≈ 48090 Hz.
The rate is **not stored in the audio data** (engine/SPU-pitch parameter).

**SFX-bank rate — confirmed NOT stored in the bank (2026-05-27).** The SShd
bank header is a flat structure of `{offset, size, count}` u32 triples followed
by the `SShd` sub-chunk magic; the sub-chunk fields after the magic are
offsets/counts (`0xFFFFFFFF, 0x80, 0x0B6E, 0x102, 0x41E, 0x72E, ...`), not a
sample rate. A scan of the first 0x200 bytes of a real bank
(`chunk04.n0`, SShd at +0x5C) for any u16/u32 equal to a standard rate
(8000/11025/22050/24000/44100/48000) finds **zero matches**. So there is no
per-bank Hz — like the streamed audio, SFX playback pitch is an SPU2 / sequencer
parameter (center-note based), set by the audio engine at trigger time, not a
property of the bank. `22050` remains a reasonable extraction default; a single
"correct" rate doesn't exist because each SFX can be repitched at playback.
Confirming the exact default pitch needs the decompiled audio engine.

### SFX pitch system — engine evidence (2026-06-09)

The boot ELF's voice-trigger path settles *how* SFX pitch works (read in the
local splat disassembly; addresses for future decomp work):

- **`func_00115E50`** (voice setup) calls **`func_00117918`** — a PS1-libsnd-
  style **note→pitch** conversion: a 2^(x/192) semitone/fine u16 ladder at
  **`D_00241D70`** indexed by `(note − center)` semitones + fine steps, with
  `sllv`/`srav` octave shifts. Its inputs are per-voice bytes (entry +0x2
  center note, +0x3 fine) and the trigger note.
- The result is immediately rescaled by **× 0xAC44 / 0xBB80 (44100/48000)**
  and written as voice param 6 (pitch), alongside param 5 = sample start
  address (`(entry_u16 + bank_base) << 3`) and param 3 = ADSR (entry +0x6/+0x8).
  I.e. the note system is **44100-referenced** (PS1 pitch convention, where
  0x1000 = 44100 Hz) and converted to the SPU2's 48000-based pitch register:
  a sound triggered at its center note plays at **44100 Hz**.
- **SFX triggers are tiny sequence macros embedded in the bank header**: after
  the `SShd` block comes a 128-byte program map, a u16 offset table, then
  per-sound event scripts — note-on events `A0 <note> <vel≈0x64> ...`
  terminated by the SMF end-of-track marker `FF 2F 00`. Observed trigger notes
  span ~0x21..0x5D, i.e. **sounds are deliberately repitched per trigger**;
  there is no single bank Hz.
- **Bandwidth corroboration** (rate-invariant 99%-energy occupancy of
  Nyquist): SFX median **0.90** (p25 0.60) — critically sampled, content
  right up to its Nyquist — vs **0.10–0.21** for the 48 kHz streams. So the
  SFX masters were downsampled to (near) their storage rate; at 44100 they
  would imply ~20 kHz of real content, which the rest of the game's audio
  chain never has. Sub-44100 storage with center notes set below the trigger
  range is the consistent reading.

**Verdict**: per-sound playback rate = `44100 × 2^(Δnote/12)`; the per-sound
Δ needs the bank's tone records (center-note bytes), which have **not** been
located in the bank header yet (a structural scan for the sample-offset table
found no flat copy — the loader at/around `func_00115E50`'s parser is the next
step). Until then `22050` stays the audition default for exports; confidence:
high that no Hz is stored and that the pitch base is 44100-referenced, medium
on any single sound's exact Hz.

### SShd bank format — SOLVED via the loader/trigger decomp (2026-06-10)

The open item above is closed: the tone records exist and were pinned by
reading the bank loader + voice-trigger path in the local splat disassembly.
The earlier structural scan missed them because (a) a "bank file" is really a
multi-bank **container**, (b) the per-bank header offsets are indirect, and
(c) the old exporter's "body offset" field is actually a body **size**.

**Engine code map (boot ELF, all in the SDK-compiled half < 0x120000):**

- `func_00119528` — bank registration: validates `"SShd"` at hdr+0xC, takes a
  free slot in **`D_0027C6C0`** (128 slots × 12 bytes: `{u32 inUse, u8 *eeHdr,
  u32 spuByteAddr>>3}`). The slot's SPU base is the upload address of the
  bank's body; `func_001195A8` frees a slot (after checking no active voice in
  the runtime voice table references the bank).
- `func_00117088` — per-event program resolver. Fills the parser state
  **`D_00281AC0`**: `+0x00` current program record, `+0x04` current tone
  record, `+0x08` region base, `+0x0C` sound/channel record, `+0x10` bank hdr,
  `+0x14` seq data base, `+0x18` velocity table, `+0x20` program-offset
  table, `+0x28` script table. Two modes (channel `+0x32`): 0 = sequenced
  music (program from the 16-byte per-channel block at seq+0x10+ch*16, byte
  +0x12), 1 = SFX (program from the note-on event's 4th byte; sound record
  from channel `+0x18` = sound id).
- `func_001152D8` — the per-tick sequencer over 48 channels (0x78-byte
  structs at `D_0027E0C0`): MIDI-style status dispatch — 0x80 note-off
  (`func_001177E8`), **0x90 note-on `func_00115E50`** (range-scan tone select
  via `func_001178C0`), **0xA0 note-on `func_00115850`** (direct-map: tone =
  note − prog.base_note; this is the form used by the banks' embedded trigger
  scripts `A0 <note> <vel> <prog>`, 4-byte events), 0xB0 controllers (jump
  table `jtbl_0026BF40`), 0xC0/0xE0 program change / pitch bend
  (`func_00118CF8` writes the bend byte into sound-record +0xA and live-
  updates pitch), 0xF0 meta (FF 2F end). Voice command queue:
  `func_001157F0` (ring at `D_0027F7C0`, 16-byte `{cmd,voice,value,x}`
  records, flushed via SIF RPC 0x64 to the IOP driver each tick).
- **Runtime voice table**: `D_0027CCC0`, 48 × 0x6A-byte records (one per
  hardware voice), filled by the note-on handlers (note +0x02, bank +0x22,
  tone fields +0x2E/+0x36/+0x38/+0x3A/+0x40, ADSR cached, etc.).
- `func_00117918` — note→pitch: `2^(x/192)` u16 ladder (16 steps/semitone);
  the canonical 4096-anchored table starts at vram **0x241CA4**; the code's
  base pointer `D_00241D70` is 102 entries in and indexes `+0xD0` at center,
  so the at-trigger anchor value is `T[310] = 12542`. Index =
  `0xD0 + 16*(note−center) + fine + ((bend−0x40)*bend_range>>2)` (composed
  with exact /12 octave decomposition + sllv/srav shifts). The result is
  rescaled `× 0xAC44/0xBB80` (44100/48000) and written as voice param 6 —
  the SPU2 pitch register (0x1000 = 48000 Hz). Sample start = param 5 =
  `(tone_u16[+4] + slotSpuBase>>3) << 3`; ADSR = param 3 (tone +0x6/+0x8).

**Container layout** (validated on all 40 containers, 115 banks):

    +0x00 u32 total_size
    +0x04 u32 first_bank_header_offset
    +0x08 u32 0
    +0x0C u32 bank_count N            (1..4 observed)
    +0x10 {u32 img_off, u32 img_size, u32 img_off, u32 0}
    +0x20 N rows {u32 body_size, u32 hdr_off, u32 type, u32 0}

The **sample image** `[img_off, img_off+img_size)` is the concatenation of
all N banks' VAG bodies in row order; bank i's SPU-relative zero = `img_off +
Σ body_size(rows<i)`. (The old exporter read hdr+0x04 as a body *offset* —
it is the body **size**; that mis-segmentation is why the previous export
found only 241 sounds.)

**Bank header** (at hdr_off; offsets are hdr-relative):

    +0x00 u32 header data size
    +0x04 u32 body size (this bank's VAG bytes in the image)
    +0x08 u32 0
    +0x0C "SShd"
    +0x10 u32 music program region offset   (0xFFFFFFFF if none)
    +0x14 u32 velocity table offset          (identity curve observed)
    +0x18 u32 ?                              (often 0xFFFFFFFF)
    +0x1C u32 trigger-script table offset
    +0x20 u32 sound-record region offset
    +0x24 u32 SFX program region offset

- **Trigger-script table** (+0x1C): u16 group count; u16 group offsets
  (0xFFFF absent); each group: u16 count + u16 script offsets (all relative
  to the table base). Scripts are tiny event streams: `A0 note vel prog`
  note-ons ended by SMF `FF 2F 00`. (`func_00119EA0` resolves
  group/index → script.)
- **Sound records** (+0x20): region header 16 bytes, then 16-byte records
  per sound id: `{u8 ?, u8 group, u8 ?, u8 vol(0x64), u8 pan(0x40), …,
  u8 bend(+0xA, stored 0), …, u8 +0xC(4), …, u8 +0xE(0x64)}`.
- **SFX program region** (+0x24): u16 max-program-index, u16 offsets[i+1]
  (region-relative; the trigger path reads them via sound-region+0x312,
  which equals this table because the sound region is fixed-size 0x310).
  Program record: 8-byte header `{u8 ntones|0x80 or 0xFF, u8 mvol, u8 mpan,
  u8, u8 bend_range, u8, u8 base_note, u8 top_note}` then the **TONE
  RECORDS**, 16 bytes each:

        +0x0 u8  note_lo      (range-scan select, 0x90 path)
        +0x1 u8  note_hi
        +0x2 u8  center note
        +0x3 s8  fine         (1/16-semitone steps, signed; −15..0 observed)
        +0x4 u16 sample start (<<3 = byte offset from the bank's SPU base)
        +0x6 u16 ADSR1
        +0x8 u16 ADSR2
        +0xA u8  vol?
        +0xB u8  pan?
        +0xD u8  bend range   (12 everywhere observed)
        +0xF u8  flags        (bit0 voice-keep, bit4 use prog bend range,
                               bits5/6/7 routing/FX-bus selects)

  For 0xFF (direct-map) programs ntones = top−base+1 and tone t is keyed
  only by note base+t — so **every tone has one fixed playback rate**.

**Per-tone exact rate.** With the effective trigger bend wheel at center
(empirically confirmed, below):

    rate = 44100 × 2^((310 − 192 + 16·(note − center) + fine) / 192)  Hz

(The −192 is the bend term: sound records store bend=0 = wheel fully down ×
bend_range 12 semitones = −192 ladder steps. `func_00115850` (0xA0) writes
0x40 into the bend byte before computing pitch, which would cancel that term
— but with the +310 anchor that overflows the 400-entry ladder for roughly
half the shipped tones (center−note spans up to ~37 semitones) and predicts
39% of trigger rates above 48 kHz; both impossible for working content.)

**Empirical confirmation (PCSX2 savestate, no emulator needed live):** the
texture-work savestates under `extract/textures_colored/_states/` carry SPU2
RAM + PCSX2 voice structs (`spu2.bin`: RAM at +0x104, V_Voice array stride
0xA0 in the tail; pitch u32 at +0x00, NextA/StartA/LoopStartA halfword
addresses at +0x04/08/0C). Observed: stream voices with pitch 0x759/0xEB3 —
exactly 22050/44100 in 48000-units, i.e. **the EE-computed param-6 value
lands unscaled in the SPU2 pitch register** (no IOP-driver shift); and
in-game SFX voices at pitch 1694–2134 (= 19.9–25.0 kHz), inside the bend-
at-center prediction band for the loaded banks' tones (the ×2 hypothesis
predicts 2360–4013+overflow — excluded). Per-voice exact reproduction is
blurred by the runtime LFO/portamento updater (`func_00116598`, same ×441/480
rescale), which keeps re-writing param 6 after key-on.

**Re-export (tools/audio_export.py `sfx`, rewritten 2026-06-10):** parses
containers/banks/programs/tone records and emits every tone at its exact
rate. Inventory: **40 containers** (39 region-spanning + the global
player/weapon/UI container `chunk00/f05_id05.bin`, which the old exporter
missed entirely), **115 banks, 5758 tone refs, 533 unique samples, 1206
unique (sample, rate) WAVs** + `manifest.txt` (region/bank/prog/tone/note/
center/fine/rate/offset per ref). All 5758 tone sample offsets land exactly
on VAG end-flag block boundaries (0 misses) — the layout is proven against
the data, not just the code. Rate distribution: p25 15.9 kHz, median
21.5 kHz, p75 27.1 kHz; a small tail of never-triggered/garbage tones
computes absurd rates (up to 245 kHz) and is exported as-is, flagged by the
manifest. Acoustic sanity at the computed rates: gunshot-like bank sounds
(2–12 ms attack, 90–310 ms, broadband), footstep thumps (140–200 ms,
low-band), tonal alarms F0 ≈ 405–430 Hz, voiced growls F0 ≈ 90–300 Hz, 7–9 s
ambient loops — all natural. Confidence: **high** on the layout and the
relative pitch math (engine-exact, data-verified); **medium-high** on
absolute per-sound Hz (savestate-corroborated to within the runtime
modulation; the bend=0-vs-0x40 ×2 question is settled by data/registers, not
yet by a single exactly-reproduced voice).

### Engine SOUND IDS — id → bank/script/tone/WAV resolution SOLVED (2026-06-10)

The gameplay sound ids (weapon 0x162–0x165/0x169, flashlight 0x15D, enemy
0x7D4/0x7D8, leech 0x430/0x431/0x434, …) now resolve statically all the way
to the exported WAVs. Tool: **`tools/audio_export.py soundmap`** →
`extract/audio_decoded/soundmap.json` (git-ignored; regenerate from your own
ELF + extracted banks). Chain, read from `func_001FB9F0` (+ the loader
`func_001FB3E0` and trigger `func_00119EA0` disassembly):

1. **`func_001FBD50(pos, id, x)`** (play_sound, 213 callers) computes 3-D
   volume/pan via `func_001FBF50` and tail-calls
   **`func_001FB9F0(id, 0x1000, vol, pan)`** — no id transformation.
2. **`func_001FB9F0`** masks `id &= 0x7FFF`, bins it, and fetches a 4-byte
   **sound record `{s8 group, s8 bankIdx, s8 scriptGroup, s8 scriptIdx}`**:
   - id 0x000–0x3E7 → record table **`D_0025ECA0`**`[id]` (1000 entries)
   - id 0x5DC–0x7CF → **`D_00261570`**`[id-0x5DC]` (500 entries)
   - id 0x3E8–0x5DB → area-paged: `remap = D_00264A70[area0][area1]
     [id-0x3E8]` (u8, 0xFF = absent), `record = D_00264B30[area0][area1]
     [remap]`, with `area0/area1` = the current-area bytes
     `D_00810700/D_00810701`; 24 area slots × up to 8 sub-slots
   - id 0x7D0–0x9C3 → same shape via **`D_00264AD0`/`D_00264B90`**
   - other ids → return −1. Unused rows are junk (0xFF runs / leftover
     bytes); the tool validates each record against the bank data and
     drops non-resolving ones.
3. **Bank slot**: `handle = D_00281D50[group*0x14 + bankIdx]` (BSS, 6
   groups × 20 u32). Filled by the bank streaming loader
   **`func_001FB3E0`**: each registered bank (`func_001194B8` →
   `D_0027C6C0` slot) is appended at `D_00281D50[D_00282190]
   [D_00281D30[group]++]`; on a group re-load the whole group is freed
   (`func_001195A8`). **The registration group = the bank row's `type`
   field in its container** (validated data-side: every plausible record's
   (group, bankIdx, scriptGroup, scriptIdx) resolves under this
   assignment):
   - group **1** = the global player/weapon/UI container
     `chunk00/f05_id05.bin`, types `[1,1,1,3]` → records (1,0) (1,1) (1,2)
   - group **3** = swappable music banks: the global container's bank 3
     and the `chunk50..53` single-bank containers (type 3)
   - group **2** = the loaded region container's type-2 bank (the per-area
     "common" set) → records (2,0)
   - group **4** = the region's type-4 banks (enemy sets, up to 3) →
     records (4,0)…(4,2)
4. **`func_00119EA0(handle, scriptGroup, scriptIdx)`** walks the bank's
   trigger-script table (hdr+0x1C) and starts the script on a free
   sequencer channel (`D_0027E0C0`). Script grammar (sequencer
   `func_001152D8`): `A0 note vel prog` direct-map note-ons (tone =
   note − prog.base_note), `80 xx` wait/release, `FF 2F 00` end — so one
   sound id can layer several samples with delays (e.g. 0x162 weapon draw
   = 2 layers, 0x7D8 death = 2 layers).
5. Tone record → sample offset + engine-exact rate as per the SShd section
   above; the tool keys (sha1(adpcm), rate) to the `sfx` export's
   `snd_NNNN.wav` names.

**Area→region mapping falls out as a by-product**: each (area0, area1)
cell's group-2/4 records must resolve in the loaded region container, so
script-coverage matching pins the scene: e.g. **(11,0) → chunk15** (matches
the savestate-pinned AREA11 = chunk15), (2,4) → chunk07.n0, (16,6) →
chunk24, (20,7) → chunk08.n1 (138 cells, 25 unique-exact, rest
best-coverage 0.86–0.98 with junk records accounting for the misses;
`area_scene_map` in soundmap.json records coverage + ties). Negative
cross-check: chunk15 (cutscene area) has **no type-4 bank**, and area11
sub0's high table correspondingly has **no 0x7D8 entry** — no enemies, no
death sound.

**Coverage: 1686 sound ids** (432 global-fixed, 377 global-table ids whose
records point at the area-loaded groups 2/4, 877 area-tabled ids with
per-region variants), 26 245 resolved note-on events, **0 dangling WAV
references**; output identical when generated from the pristine disc ELF
and from the rebuilt ELF. Documented-id spot checks (feature-based, at the
mapped WAVs): 0x164/0x165 fire → `snd_0363/0356.wav`, 7.5 ms attack,
~290 ms broadband decay (gunshot-shaped); 0x169 dry-fire → `snd_0350.wav`,
2 ms attack 149 ms click; 0x162 draw / 0x163 holster → bright 160–230 ms
mechanical foley; 0x15D flashlight → 920 ms switch sound; 0x7D8 enemy
death → per-region enemy-bank roars (721 + 967 ms low-frequency vocal
layers in chunk04.n0). Confidence: **high** (engine-code-exact dispatch +
data-validated bank assignment); the only heuristic part is the
non-exact-coverage cells of the area→region map.

### Engine 3-D VOLUME/PAN + VOICE STEALING decoded (2026-06-11)

**`func_001FBD50(obj, id, flat2d, float radius)`** (play_sound) decoded to
the instruction: it calls **`func_001FBF50(obj, &gainA, &gainB, flat2d,
f12=radius, f13=4096.0)`** and, on success, tail-submits
`func_001FB9F0(id, 0x1000, gainA, gainB)`. The gain pair is computed ONCE
per trigger (no per-frame re-pan; the only later volume writes are the
sequencer's own ramps). `func_001FBF50` in full:

1. **Distance — listener = the PLAYER** (`D_00810360`, the actor pos
   qword): `d = |src − player|` (3-D Euclidean via `func_001028D0` sub +
   `func_0011E748` sqrtf; the `flat2d` s8 arg zeroes both Y components
   first — callers pass 0). Source = `obj+0xB0`.
   - `d >= radius` → **return 0: the sound is not submitted at all**
     (play_sound returns −1).
   - `vol = 4096 · sinf((π/2)·(radius − d)/radius)` — a quarter-sine
     ease: full at the source, ~linear toward 0 at the rim.
2. **Pan — listener = the CAMERA**: eye `D_008105D0` (the global view
   position) + yaw `D_0081027C` = **cam struct 0x8101E0 + 0x9C** (eye is
   +0x10 = 0x8101F0 — the same block the live sessions steered).
   - forward = RotY(yaw)·(0,0,1) (identity `func_001029C0` + Euler
     `func_00102C58` on the vec (0, yaw, 0)); dir = normalize_xz(src −
     eye) (`func_001028D0`/`func_00102760`, Y zeroed).
   - `c = dot(forward, dir)` (`func_00102738`) ≡ cos(bearing − yaw) with
     bearing = atan2(dx, dz).
   - `k = min(d/18, 1)` — **the pan ramps in over the first 18 units of
     PLAYER distance** (player-attached sounds stay centered).
   - `t = c⁵·k ± (1−k)` (sign of c⁵k; ± pushes |t| to 1 as d→0):
     `t = +1` ahead (center), `0` at 90° (far channel silent), `−1`
     behind — **the far channel goes NEGATIVE = SPU2 phase inversion**,
     the engine's pseudo-surround rear cue.
   - side test `func_001B1380(yaw, src, eye)`: `wrapPi(atan2(dx,dz) −
     yaw) >= 0` (`func_0011E620` atan2f + `func_001B1470` wrap). TRUE →
     gainA = vol, gainB = vol·t; FALSE → mirrored.
   - **gainA is the LEFT channel**: gainA → channel +0x48 (via
     `func_0011A218`, clamp ±0x1000), and in the per-voice volume
     compose `func_001179E0` the +0x48 word multiplies the HIGH byte of
     the voice pan pair `D_00242630[pan>>2]` — the LUT runs (0x80,0x00)
     at pan 0 → (0x00,0x80) at pan 0x7F, i.e. high byte = left (MIDI
     pan convention, record default 0x40 → center pair (0x78,0x78)).
   - mono option `D_0028215B == 1` → both gains = vol (no pan);
     `D_0027F778 == 1` is a second, driver-level mono (both = max|L,R|).
3. **Radius is a per-call-site constant in f12**. Histogram across all
   `jal func_001FBD50` sites: **300.0 × 281**, 500.0 × 17, 450.0 × 12,
   1000.0 × 11, 800.0 × 6 (+ record-supplied: door script op 0x17 sub 1
   reads rec+0x20). Verified per-id: weapon draw 0x162 (func_0016F530),
   enemy death 0x7D8 (func_00153B50), leech spawn 0x430 (func_00154040),
   generator 0x42F (func_0015A2C0) — all 300.0. **The earlier "vol
   150/300" reading of this argument was wrong: it is the attenuation
   radius.** The non-positional submit form is `func_001FB9F0(id,
   0x1000, 0x1000, 0x1000)` (center/full), which is also exactly what
   the positional math produces for a player-attached source (d=0).

**Voice stealing — `func_00117428(toneB0, toneB1, bankHandle)`** (the
voice allocator over the 48 × 0x6A table `D_0027CCC0`, called by both
note-on handlers; scan starts at a rotating cursor `D_0027F740+0x30`):

1. **Retrigger-reuse** (only if toneB0 != 0): a voice with `+0x1A == 2`
   (active), `+0x20 ==` tone byte0 and `+0x22 ==` bank handle is
   returned immediately — same sound restarts in place. **Data-side:
   tone byte0 is 0 on every shipped SFX tone (556/556 in the global
   container) — this pass never fires for gameplay SFX** (it is a
   sequenced-music path; the 0x90 range-scan tones carry real note_lo).
2. **Free voice**: `+0x00 == 0` and `+0x1A != 3` (3 = locked/reserved).
3. **Steal**, in order: the first busy (`+0x08 == 1`) voice already in
   released state (`+0x1A == 1`) — immediate; else the OLDEST (minimum
   `+0x0A` = the note-on serial sampled from the global counter
   `D_0027F740+0x34`, incremented at the tail of every note-on) among:
   released keep-voices (`+0x08 != 1`, `+0x1A == 1`; no priority gate),
   then active normal voices, then active keep-voices — the latter two
   gated on `newToneByte1 >= voice+0x1E` (the victim's tone byte1 =
   PRIORITY, written at note-on). **Tone byte1 is uniformly 10 across
   shipped SFX tones** → among gameplay sounds the gate always passes
   and the policy reduces to released-first, then oldest.
4. Else **return −1 — the note-on is silently dropped** (the 0xA0
   handler skips the event).

Steals/key-offs reach the SPU through the `func_001157F0` command ring
(flushed to the IOP driver once per tick) — i.e. the engine's own steal
takes effect on the NEXT driver tick, not synchronously.

**Port adoption** (extermination-port 2026-06-11, `em_sfx.*`):
`em_sfx_play_at(id, pos, radius)` implements the decoded math verbatim
on signed float gains (rear inversion preserved); per-frame
`em_sfx_listener(player, cam_eye, cam_yaw)`; stealing = the engine's
48-voice budget with oldest-live kill over 64 physical lock-free slots
(kill honored at the next audio callback — the same one-tick deferral).
All translated positional call sites use radius 300.0 (doors, enemy
death, pad sounds 0x42D/0x42F/0x430, wall impact 0x189 at the hit
point); player-attached ids remain center/full = the exact d=0 result.
Confidence: **high** (instruction-level decode; pan LUT byte order
data-verified; the single un-validated convention is wrapped-bearing ≥
0 = screen-left, which is a one-line swap if live capture ever
contradicts it).

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

**MATRIX transforms are ABSOLUTE world placements, not parent-composed
(RESOLVED 2026-06-01).** Resolved empirically on `chunk04.n0/f06_id44.bin`
by comparing bounding boxes:

- The world-space MESH/SUBMESH geometry spans `X[-190,265] Y[-101,60]
  Z[-1775,38]` (the full level footprint).
- The MATRIX sections' **object-space geometry already spans `X[-190,84]
  Y[-60,24] Z[-1515,6]` at the identity (record-0) transform** — i.e. it is
  authored directly in level/world coordinates, *not* in a small per-prop
  local frame centred at the origin.
- The per-instance transforms are **small additive offsets** (translations of
  a few units, e.g. sec0 scatters copies at `(±6.8, 5.0, −34.0)`,
  `(±6.7, 5.0, −23.0)`, `(±6.7, 5.0, +23.0)` — a row of identical props).
  Record 0 of every table is the identity, so the "base" copy sits exactly
  where its geometry is already authored.

Applying each matrix **directly** keeps the instanced geometry inside the
static world bbox and overlapping it (applied-direct extent `Z[-1515,12]`,
fully inside the world `Z[-1775,38]`). If the transforms were parent-composed,
applying them without an outer frame would scatter the instances *outside* the
world bbox or collapse them toward the origin — neither happens. So both
`extract_models.py --scene` and the level glTF exporter **bake the transforms
directly** with no parent composition, and produce coherent non-overlapping
placed scenes.

**Uncertain.** Still unverified without the engine code: the runtime role of
the repeated identity entries (animation slots, LOD, or unused) — they are
collapsed by the duplicate filter. The sub-header word quad is decoded
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

### Level-scene glTF (.glb) export (2026-06-01)

`tools/export_gltf.py level` exports an `id 0x44` level file as a single
**placed, textured glTF 2.0 binary** that opens directly in Blender
(File > Import > glTF 2.0). It reuses the same infrastructure as the character
exporter — `GLBBuilder`, the `extract_subtextures` transfer scan/decode, and
the identity-grayscale PNG embed — applied to whole-level geometry.

```
python3 tools/export_gltf.py level \
    --level extract/chunk04.n0/f06_id44.bin \
    --out   models/chunk04.n0_f06_scene.glb
python3 tools/export_gltf.py level --all-levels --out-dir models   # batch
python3 tools/export_gltf.py level --level <f> --no-textures        # geom-only
```

Pipeline (`build_level_glb`):

1. **Geometry** comes straight from `extract_models.parse_scene()` — the same
   world-placed strips the OBJ `--scene` mode emits (world-space MESH/SUBMESH
   plus MATRIX-instanced props with their absolute transforms baked into the
   vertices; see "MATRIX transforms are ABSOLUTE world placements" above).
2. **Texture binding.** Strips are grouped by texture sheet via the documented
   `sheet_field = (m0 >> 15) & 0x3FFF → GS DBP` map. Each DBP is resolved to a
   `BITBLTBUF`/`TRXREG`/IMAGE-GIF transfer — **the level's own chunk dir first,
   then the whole `extract/` tree** (level textures are almost entirely
   cross-file: the reference level itself contains **zero** GS transfers; its 3
   sheets come from `chunk04.n0/f00_id43.bin` and `chunk04.n2/f00_id44.bin`).
   The PSMT8 sheet is decoded and embedded as an RGBA grayscale PNG (identity
   CLUT; colour CLUT binding still unresolved). An unresolved DBP gets a
   solid-gray placeholder material so the geometry still renders.
3. **glTF structure.** One mesh per DBP group, each a single double-sided
   TRIANGLES primitive with POSITION + NORMAL + TEXCOORD_0, referencing its
   sheet material; all meshes parented under one scene-root node. Geometry is
   pre-placed so node transforms are identity. NORMALs are per-strip
   face-area-weighted geometric normals (uniform handling of the static-mesh
   colour-vs-normal `+0x20` ambiguity).

**Validation.** Reference level `chunk04.n0/f06_id44.bin`:
`models/chunk04.n0_f06_scene.glb`, **2.4 MB**, 3 meshes, 7538 strips,
**20 647 triangles**, 47 271 vertices, bbox `X[-190,265] Y[-101,60]
Z[-1775,38]` (extent **455 × 162 × 1813** units — room/corridor scale, not the
~20-unit character scale), **3/3 texture sheets resolved** and embedded as
PNGs. Round-trips through `pygltflib`'s strict loader: every bufferView lies
inside the buffer, every accessor count × stride fits its bufferView, every
image is a valid embedded PNG, every material → texture → image reference is
in range.

**Batch.** `--all-levels` exports **32 of 36** `id 0x44` files (≈0.6–2.3 MB
each). The **4 skips** (`chunk07.n2`, `chunk11.n2`, `chunk12.n2`,
`chunk12.n3` `f01_id44.bin`) carry **no MESH signature at all** — they are
pure texture-sheet carrier files (1–2 GS transfers, zero geometry strips),
correctly skipped as non-renderable. Texture coverage: every level resolves
its 2–3 universal sheets (DBP {10752, 12672, 14592}); levels that reference 4+
distinct DBPs leave the extra sheets as gray placeholders (their uploads are
in common/UI files outside the per-level + extract-tree search, the same
~631-material cross-file residency gap noted under "Material → texture
binding"). Geometry-only (`--no-textures`) export is always available as a
fallback. Output `models/*.glb` is git-ignored (disc-derived, never committed).

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

## ~~VU1 SOFT-skinner kernel family #0/#1/#2/#4~~ VOID (2026-06-09 s2: decoder-bug artifact; kernel #0 is a ribbon/beam renderer — see the 2026-06-09 session-2 section) (2026-05-27)

The four medium 2-segment kernels at imem 0x0000 (vram heads `0x00230828`,
`0x00231798`, `0x00232568`, `0x00233828`) share an identical dmem framing
and are the consumers of the per-vertex W-field bone selector documented
in "Per-vertex bone binding lives in the position w field". Per
`docs/PROGRESS.md` "VU1 SOFT-skinner kernel #0 first-pass disassembly":

- **Per-block 4-bone matrix palette lives at dmem qw 111..122** (12 qw =
  4 affine matrices × 3 rows each). Populated per draw-batch by EE-side
  VIF1 UNPACK; the matrix payload originates from the live-pose BSS
  arena `0x002863XX..0x002893XX`.
- **Per-vertex selector decode** at vram `0x00230B70..0x00230B98`:
  `ftoi4.x` + `iswr` round-trips the W-field float through a VI register
  into `vi01`, which is then the base for `lq vf12, 0(vi01)` /
  `lq vf13, 16(vi01)` (palette indexing with +16qw matrix-slot stride).
- **The kernel does NOT contain the global-bone-id list.** That mapping
  is supplied by the EE side: whichever function builds the
  matrix-palette VIF1 UNPACK reads a per-MESH-block 4-bone-index table
  (location still unknown — search target now narrowed to a small
  fixed-offset field in the 0x820-byte MESH-block header, OR a
  separate per-block table that the EE walks during draw-list setup).

## VU1 microcode (boot ELF static, 2026-05-25)

> **2026-06-09 s3:** the per-kernel CLASSIFICATIONS below ("world
> renderers", "skinned-mesh signature", "per-bone rigid skinners")
> predate the disassembler fix and are unreliable; the verified facts:
> the CHARACTER skinner is the 62-qw kernel at 0x0023C780, kernel #0
> family are ribbon/beam renderers, #5/#7/#9 are 14-vertex effect
> kernels. The packet-wrapper/MPG catalog mechanics below remain valid.

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

**~~Most likely skinning kernel: the #5+#6 / #7+#8 / #9+#10 family~~
(2026-06-09 s3: WRONG — the character skinner is the 62-qw kernel at
0x0023C780; the #5 family are effect kernels. Kept for history.)**

> **2026-05-27 CORRECTION.** The "#6/#8/#10 helper packets" identified
> at vram `0x002346b0`/`0x002346f0`/etc. are **false positives** — the
> catalog scanner mistook interior `jalr vi15` instructions
> (`0x4a0f0800`) for MPG tags. The skinner mains (#5/#7/#9) are real
> kernels at vram `0x00234610`/`0x00234b30`/`0x00235010` (153/145/142
> qw). The imem-0x800 helper IS used (the mains contain `jalr vi15`
> calls) but lives in a separate VIF DMA upload that the catalog
> scanner doesn't surface. The pseudocode below was inferred from
> op-frequency profile; the **byte-decoded** version, including the
> corrected vf28..vf31 bone-matrix slot and the dual-ITOF
> (positions Q4.12 + normal Q4.4) decode in the helper, is in
> `docs/PROGRESS.md` (2026-05-27 entry "VU1 skinner kernel #5
> disassembled").

> **LOWER-pipe field-position convention.** `tools/disasm_vu.py`
> currently decodes operand fields as `FT=[15:11] IS=[20:16]` (PCSX2
> source convention). On this binary the correct mapping is
> **swapped**: `FT=[20:16] IS=[15:11]`. Under the swap, all bone /
> constant / vertex loads decode sensibly; under the original
> convention every load went to `vf00` (zero register). Pending
> one-line fix in `tools/disasm_vu.py`.

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

**Per-bone object-space vertex format -- ~~CONFIRMED (2026-05-25)~~ VOID (2026-06-09 s2: these records are animation keyframes; see "id 0x74 prefix is ANIMATION, not geometry").**
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
cutscene audio.**

**Duration analysis (2026-05-27).** All 55 clips are stereo. Decoded clip
durations are clearly **bimodal**, which is consistent with the soundtrack /
cutscene split but does not pin the exact 25/30 assignment:

- **Long-form cluster (~28 clips, ≥ 90 s):** looping background-music /
  soundtrack pieces — e.g. `clip_0008` at 460 s, `clip_0049` at 313 s,
  `clip_0009` at 263 s, and a band of 90–220 s tracks. These are the
  full-length looping BGM.
- **Short-form cluster (~27 clips, < 90 s):** cutscene / event stings and
  jingles — e.g. `clip_0010`/`clip_0011`/`clip_0017` at 7–10 s, plus a
  spread of 20–80 s pieces.
- `clip_0054` at 0.4 s is a stream-terminator artifact, not a real track.

The duration boundary (~90 s) gives roughly 28/27, not exactly 25/30, so a
handful of mid-length pieces (e.g. `clip_0031` 72 s, `clip_0040` 80 s) are
ambiguous by duration alone. A definitive labelling needs either the engine's
music-cue table (which clip index plays in which context) or an audio
fingerprint match against the official-soundtrack rip. The bimodal split is
recorded here so a future pass can start from the long/short partition rather
than from scratch.

## Per-bone section directory — RESOLVED 2026-06-02 (object-space mesh)

> SUPERSEDED (2026-06-09): the "object-space per-bone VIF stream" this
> directory indexes is the id 0x74 ANIMATION container's map-A section
> table, not geometry (see "id 0x74 prefix is ANIMATION, not geometry";
> the 0x2280 header and the "0x818C base" are that container's header and
> rotation map). `decode_bone_section_table` was removed from
> export_gltf.py in s4d. Kept for the historical record only.

The character mesh's **object-space per-bone VIF stream** (the untextured
rigged mesh, distinct from the textured MESH-descriptor blocks below) is
prefixed by a directory that maps each section to its GLOBAL bone index.
Recovered from a live PCSX2 capture taken mid-skinning-draw
(`SCUS-97112 (4CDC5F74).01.p2s`): the resident VU1 kernel and the live
4-bone palette in dmem confirmed the per-bone wiring, and the on-disc
directory was then located by structural search.

On the player mesh (`chunk21/f17_id8f.bin`) the directory is at file offset
`0x2280`:

```
+0x00  u32 base_ptr          file offset the section offsets are relative to (0x818C)
+0x04  u32 x 3               zero padding
+0x10  u32 bone_idx[0..L-1]  GLOBAL bone index per section; [0] = 0xffffffff sentinel
...    u32 offset[0..L-1]    section byte offset (rel. base_ptr), strictly increasing
...    u32 0                 terminator
```

Two parallel L-entry tables (L = 21 for the player): `section i` belongs to
global bone `bone_idx[i]` and starts at `base_ptr + offset[i]`. The recovered
mapping is `[-1, 0, 1, 1, 2, 3, 3, 2, 2, 2, 8, 9, 5, 6, 5, 10, 11, 12, 13,
15, 16]` — 21 sections → 14 distinct bones (multiple sections per bone, e.g.
bone 2 gets 4 sections). `tools/export_gltf.py::decode_bone_section_table`
implements this and the exporter now attaches each object-space section as a
child node of its real bone (was previously the wrong `section i == bone i`
assumption — the old `em._find_bone_section_table` heuristic keyed on
`vals[0] < 0x200` and started part-way down the bone-index list, dropping the
mapping). **This fixes the object-space rigged mesh only**; the textured
MESH-descriptor blocks (below) have a separate, still-unresolved binding.

## Per-block bone binding (proxy by spatial proximity, 2026-05-27)

> SUPERSEDED (2026-06-09): the binding is exact and PER-VERTEX, not
> per-block — node = (pos_W_bits & 0x3FF) >> 3 (see "Skinned-character
> pipeline FULLY DECODED"; the "W-field selector k" was the address bits).
> The proxy binder `_bind_blocks_to_bones` was removed from export_gltf.py
> in s4d. Kept for the historical record only.

The MESH-descriptor blocks in a character file (e.g.
`extract/chunk21/f17_id8f.bin`) store their ~32 vertices in some BONE-LOCAL
frame — not in world space. Each block also carries a per-vertex W-field
selector (commit `94b8fe5`: `k = round((w - sign(w)) * 512)`) that takes only
a small handful of distinct values per block, so each block belongs to ONE
bone (no per-vertex blending).

The per-block bone-INDEX TABLE (which bone owns each block) has NOT been
located on disc. As an interim measure, `tools/export_gltf.py` now ships a
**spatial-proximity proxy binder** (`_bind_blocks_to_bones`):

1. Sample the id 0x71 skeleton at frame 0 to get 30 column-major world
   matrices via `bind_pose_at_t()`. Bone joint = column 3, xyz components.
2. For each MESH block, compute the centroid of its real vertices (the
   bone-local coordinates).
3. For each candidate bone B, apply B's world matrix to the centroid and
   compute distance to B's own joint position.
4. Pick the bone minimising that distance — i.e. the bone whose local-frame
   "ownership" places the block's centroid nearest that bone's joint.

Validated on the player mesh: 317 blocks bind across 18 distinct bones
(out of 28 active). Distribution is plausible but biased toward bones near
the world origin (a block centroid near (0,0,0) in bone-local trivially
lands at the bone's joint after transform — distance 0). Concretely: bones
11/12 (torso/hub) draw 178 of 317 blocks; arms/legs (5,6,7,8,17,18,20)
collectively draw ~115, which is the expected order of magnitude. Hands and
fingertips are under-represented.

Exporter behaviour: each non-empty block becomes a glTF mesh attached to a
new node that is parented under its bound bone's node. The bone-node's
animated TRS therefore carries the block, so all 57 animation clips drive
the textured surface — not just the per-bone debug meshes. Replaces the
previous "static_textured" blob at scene root.

Sample output: `models/Extermination_Player_proxybind.glb` (3.0 MB; 339
meshes, 348 nodes, 57 animations, 3 PSMT8 sheets).

CAVEATS
  * Approximate. Expect ~30-50% of blocks to bind to the visually wrong
    bone, producing some floating/misplaced pieces; the goal is "recognisable
    posed humanoid in Blender" not perfect skinning.
  * The bias toward origin-centred bones is intrinsic to this heuristic and
    cannot be fixed without the real on-disc binding table.
  * The W-field per-vertex selector is currently ignored at export time
    (one bone per block, not per vertex). Wiring per-vertex weights in needs
    the decoded SELECTOR -> bone-index lookup that the engine builds at load.
  * When the real per-block bone-index table is located on disc, replace
    `_bind_blocks_to_bones()` with a table lookup; the rest of the
    block-as-glTF-mesh-parented-to-bone-node pipeline stays unchanged.

## Textured-block bone binding — the per-block field is NOT on disc (2026-06-02)

Full investigation of the textured MESH-descriptor blocks' bone binding using
the live PCSX2 capture (`/tmp/cap2/`, mid-skinning-draw). The headline result
is a **decisive negative with a structural reframing**: there is no explicit
on-disc per-block 4-bone-index table; the textured blocks are skinned at draw
time through a runtime-built 4-matrix palette indexed by a **fixed**, not
per-block, W-selector.

### File architecture of `chunk21/f17_id8f.bin` (mapped)

The character file stores the model **twice**:

- **Object-space per-bone VIF stream** — bound by the directory at file
  `0x2280` (21 sections → 14 distinct bones; RESOLVED, see section above) plus a
  long series of additional per-bone directories filling the **directory zone**
  `0x53040..0xc4e40` (≈40 directories of 20 and 44 sections each — LOD / extra-
  detail / per-region copies). Every directory has the identical layout:
  `prefix[count|tag, 0xfffe, off_a, off_b]` then `base_ptr (+0x00)`, an optional
  second base at `+0x04`, three zero words, `0xffffffff` sentinel at `+0x10`,
  the `u32 bone_idx[]` list, the strictly-increasing `u32 offset[]` list, and a
  `0` terminator. The 44-section directories repeat almost the same bone-index
  sequence as the 21-section one (`[-1,0,1,1,3,2,3,2,3,3,8,5,7,9,6,9,4,15,15,
  15,15,13,10,12,11,15,14,...]`) — the same skeleton at finer granularity.
- **Textured 0x820-byte blocks** — the UV+normal+position skin, in **two**
  uniform-stride runs: **region 1** = 134 blocks `0xe848..0x520e8`, **region 2**
  = 183 blocks `0xd6048..0x132708` (317 blocks total, matching the 317 MESH
  sigs). Each block carries the standard 64-byte vertex record (marker / UV /
  normal / position+W).

### Why there is no per-block bone table — what was ruled out

- **The W-selector is globally fixed, not per-block.** `k =
  round((w-sign(w))*512)` takes **exactly the same 4 values `{-3, -1, 0, +2}`
  in every block of both regions** (region 1 counts `{-3:700,-1:1260,0:1621,
  2:707}`, region 2 `{-3:903,-1:1847,0:2193,2:913}`). So `k` selects one of 4
  **palette slots** (a fixed slot role), and the palette *contents* — which 4
  global bones populate the slots — are swapped per draw batch by the EE side.
  The earlier "per-block small-set selector" reading was an artifact of looking
  at too few blocks; the universe is the same 4 everywhere.
- **The per-bone DIRECTORIES bind the object-space stream, not the textured
  blocks.** Their `base_ptr + offset[i]` section addresses do **not** land on
  the textured-block MESH sigs (1/44 coincidental hits), and the section
  offset deltas (~0x600) are smaller than the 0x820 block stride — they index
  variable-length VIF sections in the directory zone, a parallel representation.
- **The 276 fixed 0x20-byte records at file `0x0..0x2280`** are not per-block
  bone indices: count (276) ≠ block count (134/183/317), and each record is 8
  floats with a slowly-incrementing field (an animation/transform parameter
  stream), not small integers.
- **The block header `m0/m1` marker** keys only the texture sheet
  (`sheet_field = (m0>>15)&0x3FFF`) and a per-material running index — already
  documented as NOT a bone selector.
- **No length-134 / length-183 byte/short bone table exists** anywhere in the
  file (a full scan for a run of small varied integers finds only the directory
  `bone_idx` u32 lists themselves).

### The live capture confirms a runtime-built palette (not a disc lookup)

The captured VU1 `vu1_dmem.bin` does **not** hold a clean 4-bone palette at
qw 111..122 in this frame: that region is occupied by GS / projection setup
(qw 112 `(2048,2048,..)` screen-centre, qw 113 `1677721.5` the PS2 Z fixed-
point scale), and the "active draw" matrix at qw 90..93 is an **identity-
rotation viewport matrix with translation `(452.3, 278.6, 277.6)`** (a
screen-space MVP base; the same triple appears at several EE addresses as a
viewport constant). The palette slots are therefore **MVP-folded** (`palette[k]
= viewport · bone_world`), so palette rotations do not match the model-space
composed world bones directly, and only one slot (qw 114..116) held a single
non-orthonormal matrix — the save state caught VU1 between batches. The EE pose
buffers are present and clean: **two 21-matrix runs at EE `0x00287f40` and
`0x00288d40`** (0xE00 apart, the documented double-buffered local/world pair),
column-major affine, humanoid-scale — these are the per-bone LOCAL matrices the
engine composes through the id71 parent table and then folds with the viewport
into the per-block palette.

### Conclusion and path forward

The per-block 4-bone binding is **engine-built at draw time**: for each textured
block the EE gathers 4 bone world matrices, folds them with the viewport, and
VIF1-UNPACKs them into the VU1 palette; the vertex W-selector (`k∈{-3,-1,0,2}`)
picks the slot. There is **no on-disc per-block bone-index field to read**.
Recovering the exact slot→bone choice per block requires either (a) decompiling
the EE function that builds the palette UNPACK (the loop that reads the 4 bone
indices from a per-draw structure — likely keyed by the same material/section
ordering as the object-space directories), or (b) a capture taken precisely
during a block's UNPACK with a fully-populated palette, cross-referenced by the
XGKICK'd GIF output. The object-space rigged mesh (directory at `0x2280`) stays
the faithful posed export; the textured surface stays on the spatial-proximity
proxy until the EE palette-builder is decompiled. The exporter was **not**
changed (no clean on-disc binding emerged to wire); `verify_all --only gltf`
PASS preserved.

## Live bone-NODE array + vertex-record layout is WRONG — 2026-06-09 (PCSX2 MCP live debug)

> **SUPERSEDED same day (session 2):** the records are not vertices at
> all — see "id 0x74 prefix is ANIMATION, not geometry" below. Also
> note +0x64[0] is the node's PARENT index, not its global bone index.

First session using the PCSX2 MCP debugger (DebugServer connected; Pine
absent). Practical notes: EE **memory reads work everywhere** (including
the VU1 dmem window at physical `0x1100C000`, which read zero between
draws), but **EE breakpoints set via DebugServer never fire** (recompiler
does not honour them, pause/resume does not flush them in) — live work must
be done by reading memory while running/paused, not by trapping.

### The live per-bone NODE array (player rig, runtime)

`bone_matrix_publish` (`func_00179BC0`) disassembles to:

1. optional call `func_001749F0(obj, f12=0, f13=lh(obj+0x276) or 1.0f)`
   depending on `lbu(obj+0x1F0)` ∈ {0x31, 0x34};
2. `anim_eval_skeleton` (`func_001C6DA0`) on the character object (`a0`);
3. for `i < lbu(obj+0xC)` (bone count): copy via `func_00102958(dst, src)`
   from `*( *(gp-0x7830) + i*4 ) + 0x90` into `a2 + i*0x40`.

`*(gp-0x7830)` = `*(0x00275B40)`. In the live session it pointed to a node
POINTER TABLE at `0x008103C0` holding exactly **21 node pointers**
(`0x007D5840 .. 0x007D6880`, stride 0xD0) then NULLs. Node struct (0xD0):

    +0x18  f32[3]   scale (1,1,1 observed)
    +0x30  f32[4]   quaternion, current   (NLERP output)
    +0x40  f32[4]   quaternion, previous
    +0x50  f32[2]   clip phase + rate (e.g. 0.564, 0.0128)
    +0x58  f32[3]   clip frame info (34.0 = current clip length; 3rd varies)
    +0x64  i16[..]  [0] = this node's GLOBAL bone index; more tree/link shorts
    +0x88  i16[4]   Q4.12 constants (0x1000 = 1.0)
    +0x90  f32[16]  column-major 4x4 — translations are WORLD-scale
                    (clustered around the character's world position
                    (107.4, 0..15, -184)), i.e. composed, not parent-local

**Cross-confirmation:** reading `+0x64[0]` across the 21 nodes yields
`[-1,0,1,1,2,3,3,2,2,2,8,9,5,6,5,10,11,12,13,15,16]` — exactly the mesh
file's section→global-bone directory (`decode_bone_section_table`,
FINDINGS "Per-bone section directory"). So **live node order == mesh
section order**, and the directory's bone index is stored per node.

Full capture: `extract/live/player_bones_live.json` + `node_structs.json`
(git-ignored), raw bytes in `extract/live/nodes_raw.hex`.

### `chunk21/f17_id8f.bin` carries TWO per-bone vertex streams

- **Stream A (legacy)**: the 28-section table at `0x22xx`, sections
  `0x231C..0x8186`. This is what `em._find_bone_section_table` /
  `load_per_bone_meshes*` slice today.
- **Stream B (directory)**: the 21-section region `0x81E0..0xE848`
  addressed by the `0x818C`-based directory — *different record layout*
  (vid-like field at +6 stepping +1; at least one section, #19, is an
  INDEX/stitch stream of u16s with 0xFFFF restart sentinels).

The two streams are similar sizes (~2.0k vs ~2.2k verts) — LOD pair or
position/topology split; unresolved.

### The documented 12-byte vertex record layout is WRONG

The assumed layout (`pos3 i16 @+0, packed normal @+6, vid u16 @+10`,
`OBJSPACE_*` in extract_models.py) decodes Stream A into per-section
clouds whose y/z **saturate the full ±8 Q4.12 range** (uniform noise +
one thin coherent line — the classic misaligned-stride signature). Hard
evidence against it, from raw record dumps (12-byte stride confirmed by
the repeating vid):

- the true **vid is at +2** (steps +2 within strips; duplicated
  warmup/stitch records step +1; standalone 0xFFFF-vid restart records);
- byte +8 is **0x3F in every record** of every section inspected;
  bytes [+5..+8] decode as a **little-endian float in (0.5, 1.0)**
  (0.989, 0.846, 0.572 …) — almost certainly the per-vertex bone
  WEIGHT / position-W field that the VU1 kernel turns into a bone index
  (`k = round((w - sign(w)) * 512)`, see the VU1 notes);
- low nibbles of bytes +6 and +11 are a constant tag (0x3/0xB);
- i16s at +0 and +4 are smooth, plausible Q4.12 coordinates; the third
  coordinate is NOT a clean i16 at any even offset — the record packs
  pos + weight-float + normal into non-2-byte-aligned fields.

**Consequences:** every artifact built on the old decode — the
`--object-space` point clouds, the `--rigged` "posed figure", the glTF
geometry (`verify_all`'s glTF stage checks structure, not shape) — has
partially garbage positions. The 2026-05-25 "single posed figure in
Blender" claim does not reproduce; treat it as unverified.

**Resolution path (next session):** disassemble VU1 skinning kernel #0 /
the rigid skinner at `0x00234610` (`tools/disasm_vu.py`) and read the
exact dmem qword layout the kernel consumes, together with the VIF UNPACK
opcode that expands these 12-byte records into qwords. That pins every
field; then fix `decode_objspace_bone_vertices` + the exporters once.

### Native-port renderer status (extermination-port)

The port side is DONE and waiting on the decode fix: `tools/
export_native.py` (this repo) bakes mesh + section→bone mapping + clip-
sampled world-matrix palettes into a zero-dep binary (`EMDL`), and the
port renders it through a complete Metal skinned pipeline (depth buffer,
runtime-compiled skinning shader, bone palette via inline constants,
`EM_CAPTURE=<path.bmp>` headless screenshot for regression). With the
corrected vertex decode the same EMDL + renderer should display the
posed, animated character with no further port-side work.

## id 0x74 prefix is ANIMATION, not geometry — vertex-record question RESOLVED (2026-06-09, session 2)

Follow-up to "Live bone-NODE array + vertex-record layout is WRONG". The
plan was to pin the 12-byte record layout from the VU1 kernels + VIF
UNPACK tags. The actual answer: **there is no vertex-record layout to
pin — the records are keyframes.** The whole "per-bone object-space
vertex stream" interpretation (2026-05-25, "CONFIRMED") is void.

### The container format (blob ids 0x74, 0x2C — same layout)

A mesh file's prefix carries one or more keyed-animation containers:

    hdr+0x00  u16  n_sections      = node count (player: 21)
    hdr+0x02  u16  clip_len        frames (player f17_id8f: 401; the
                                   chunk28/f01_id3c NPC: 80)
    hdr+0x04  u16  0xfffe/0xffff
    hdr+0x08  u32  blob id         0x74 (also 0x2C sub-blobs, n=3)
    hdr+0x0c  u32  -> map B section table (header-relative)
    hdr+0x10  u32  -> map C section table (header-relative)
    hdr+0x20  i32  parents[n]      node TREE (strict: parent < index)
    then      u32  offsets[n]      map A sections (rel. to this table)

Player f17_id8f: hdr at 0x2270, map B table at 0x8ce0, map C at 0xa3fc.
A second id 0x2C container sits at 0xac40 (3 sections, same 401-frame
clip), more keyed blobs follow up to ~0xe7ff. The old "28-entry section
table at 0x22c8" was a misparse straddling the parents/offsets arrays;
the old "0x818C stream-B directory" was this header's map C pointer.

**The `parents` array is a parent table, not a bone-id directory.** The
live node struct's `+0x64[0]` short is the node's PARENT index (-1 =
root), matching this array element-for-element — the earlier reading
("this node's GLOBAL bone index") was wrong.

### The 12-byte record = one keyframe

> SUPERSEDED (2026-06-09 s4): the record is a 4x20-bit truncated-float
> bitstream (qx,qy,qz,qw) — see "id 0x74 channel encodings FULLY
> DECODED". The A/B/W1/W2 reading below is that bitstream misparsed at
> byte offsets; kept for the historical derivation.

    +0x00  i16  A        companion field (= low 16 bits of qx's field)
    +0x02  f20  W1       see below (= qy)
    +0x05  i16  B        companion field (= low 16 bits of qz's field)
    +0x07  f20  W2       (= qw)
    +0x0a  u16  frame    key time; 0xffff terminates a section

"f20" = an IEEE float stored as its top 20 bits only (sign, exponent,
11 mantissa bits). In the byte stream the float's 32-bit lane overlaps
the neighbouring i16: its low 12 bits hold [tag nibble][i16 high byte].
The tag nibble is 0x3 or 0xB — one flag bit (0x8) + constant 0b011.
This overlap is why no 3×i16 or 3×f32 parse ever produced clean
positions, and why byte +8 was "always 0x3F" (W ∈ (0.5,1) exponent).

Keys are sparse and ascending (the old "vid stepping +2" was key times
every 2 frames), with hold semantics, duplicated end keys, and 0xffff
terminators — the "warmup/stitch records" were dense key bursts.

### Live verification (PCSX2, NPC chunk28/f01_id3c @ 0x00d689c0)

For the on-screen character (21 live nodes at 0x007d5840, stride 0xD0):
**map A W1 = node local quaternion .y and W2 = .w — byte-exact** on
nodes with uncontaminated channels (node 14: channel (·,0.580,·,-0.043)
vs live quat (0.813, 0.580, -0.028, -0.043); node 20 exact too; root =
(0,0,0,1) vs channel (0,0,0,1)). A/B encode the remaining quat
components ~~in a form not yet decoded~~ (s4: they ARE qx/qz — 20-bit
truncated floats whose low 16 bits the A/B parse isolated). Map B is
translation (3x26-bit truncated floats; s4). Map C is scale (3x26; s4).

Node-struct corrections (live, same arena layout as the player session):
`+0x00` = local translation (pose-invariant bone offsets), `+0x30` =
local rotation quat (x,y,z,w; conjugate convention vs. composed world
matrices), `+0x90` = composed world matrix as before.

### Stage-2 MESH blocks are the ONLY geometry — and they are bone-local

f17_id8f block walk: 313 fixed 0x820-byte MESH blocks (32 × 64-byte
records each), 3 MATRIX-descriptor blocks, and 41 trailing 5680-byte
blocks whose "descriptor" is literally VIF code `STCYCL(4,4)` +
`UNPACK V4-32, 256qw, addr 0, FLG=1` followed by float vec4 rows
(ST / normal / position+w) — i.e. ready-to-kick VIF vertex payloads,
4 qwords per vertex.

Per-block position bboxes are limb-sized (≤ ~2.7 units) and cluster at
the origin: **block positions are in BONE-LOCAL frames**, not model
space. Rendering all blocks untransformed yields overlapping body parts
(a coherent solid "blob") — NOT a posed figure. The w field is
±(1 + k/512) with k ∈ {-3,-1,0,2} — the SAME set in every block, so it
is a per-block 4-slot palette selector (all four slots used per block)
or a strip-role code; either way it is NOT a per-block-varying global
bone id. The per-block palette→node table is NOT in the mesh file's
prefix (searched exhaustively; the prefix is fully accounted for as
animation blobs + a float track table at 0x0..0x2270 + the 0xe800
preamble). **Block→node binding is the one remaining unknown** for
posed character rendering.

### VU1 disassembler corrected — all prior kernel readings void

`tools/disasm_vu.py` had five systematic decode bugs, now fixed against
PCSX2's canonical tables (pcsx2/VUops.cpp `_vuTablesMess`):

1. UPPER bc-family ops only decoded for bc=x (ops 0x01..0x1b in the
   bc families were `upp_XX` placeholders).
2. UPPER special groups 0x3C..0x3F used invented tables; canonical is
   FD_xx[(code>>6)&0x1f] — e.g. what decoded as "itof12" was MULAx;
   the entire "dual ITOF12/ITOF4 Q4.12/Q4.4 dequant" finding was a
   decode artifact (there is NO ITOF in the per-vertex path).
3. LOWER special T3 tables were shifted (0x3BC = DIV, not WAITQ;
   0x33D = MR32; 0x73F = ERLENG; 0x7BF = WAITP; XGKICK = 0x6FC was the
   only correct anchor; lower NOP 0x8000033C = MOVE vf00,vf00).
4. I-bit immediates live in the SAME pair's lower word (not the next
   pair).
5. SQ/SQI/SQD encode source in FS [15:11] and base in IT [20:16] —
   asymmetric vs LQ. (The 2026-05-27 "global field swap" was a partial
   fix that happened to be right for LQ only.)

Corrected kernel readings (decoded end-to-end):

- **Kernel #0 family** (0x00230828 + seg2 0x00231030, and siblings):
  a RIBBON/BEAM renderer: input 4 qw per element (two endpoint
  positions, color, width), output 13 qw per element (GIF tag + 4
  corner vertices = two endpoints extruded perpendicular in screen
  space). The "per-vertex selector → matrix palette at qw 111..122"
  reading was a decode artifact; qw 110..123 are this kernel's
  constants (two matrices, fog, GIF tag template).
- **Kernel #5/#7/#9** (0x00234610 etc.): fixed 14-vertex-per-kick
  effect kernel. Reads ONE qword per vertex (float x,y,z + w scalar),
  transforms by a single matrix from dmem qw 0..3 (full MVP), builds
  UV procedurally (normalize(pos - C), plane-projection divide) and
  RGBA from a pseudo-normal directional term — a projected-shadow /
  env-effect kernel, NOT the per-bone rigid skinner. Its "helper at
  imem 0x800" is actually at instruction 0x31 of the SAME program
  (`jalr vi15, vi01` with vi01=0x31); the tail after the main's E-bit
  is that helper, fully decoded now.
- The 12-byte anim records can never be VIF-unpacked (their fields are
  not 32-bit aligned) — consistent with their being EE-side animation
  data; sections are back-to-back with no VIF tags between them.

### Tooling status after this session

- `tools/disasm_vu.py`: fixed as above (canonical tables, operands).
- `tools/extract_models.py`: `parse_id74_prefix()` is the canonical
  prefix parser; `decode_objspace_bone_vertices()` now raises (the
  geometry it produced was garbage); `--object-space` dumps the anim
  channels (`*_animchannels.txt`).
- `tools/export_native.py`: exports stage-2 strip geometry (welded,
  Y-up, grounded) as a STATIC one-frame EMDL; `--clip/--live` palettes
  are parked until block→node binding is decoded. Port renders a
  coherent solid model (overlapping bone-local parts), no longer soup.
- `tools/export_gltf.py`: ~~still contains the void per-bone decoders
  (`load_per_bone_meshes*`); needs the same surgery~~ — re-pointed
  2026-06-09 (s4d): void decoders/proxy binding deleted; characters now
  export as skinned glTF (stage-2 strips with exact per-vertex node
  binding, identity inverse binds, keyed-container animations paired by
  n == max_slot + 1, textured per sheet). Parity with EMDL verified.

## Skinned-character pipeline FULLY DECODED — posed character renders in the port (2026-06-09, session 3)

Continuation of session 2; the block→node binding is solved and verified
end-to-end (port capture shows a coherent posed character).

### The draw chain (live, EE RAM 0x29bda0 / double buffer 0x2fc5c0)

Per skinned character, the engine emits:

    REF  9qw  -> 0x815360   GS state (VIF NOP/FLUSH + DIRECT 8qw)
    REF  8qw  -> 0x816440   more state
    REF  1qw  -> 0x814220
    CALL      -> 0x0023C750 canned ELF packet: MPG of the 62-qw kernel
                            at vram 0x0023C780  <- THE character skinner
    REF  Nqw  -> mesh blob  the ENTIRE block stream in one transfer
    NEXT      -> ...

The mesh blob (e.g. chunk28/f00_id3b.bin, loaded verbatim at 0xd1c1c0)
is itself the VIF stream: header {u32 n_blocks; u32 total_qwc; u32
n_nodes; u32 byte_size; bbox floats} at +0, then per block:
`STCYCL(4,4)` + `UNPACK V4-32 128qw -> TOPS+0` + 32 vertex records
+ `MSCAL 0` (first block) / `MSCNT` (rest). What extract_models.py
calls MESH_SIG is exactly that 8-byte STCYCL+UNPACK pair.

### The 62-qw skinning kernel (vram 0x0023C780) — full decode

Per 32-vertex batch (XTOP double-buffered input, 4 qw per record:
[tex/marker][ST][normal][pos+w]):

    ilw   vi10, 3(vi14).w        ; read the position W FLOAT AS AN INT
    lq    vf28..vf31, 0..3(vi10) ; transform matrix  (absolute dmem!)
    lq    vf24..vf26, 4..6(vi10) ; normal matrix (3 rows)
    iand  vi02, vi12(=0x8000), vi10  ; bit15 = strip-restart flag
    ...   pos' = M*pos; clip; div Q,1,w; fog (qw1021); ftoi4
    ...   N' = NM*normal; light via matrix at qw 1013..1016; RGBA
    ...   ST*Q perspective; marker qword copied through (texture regs)
    sq    {marker,ST,RGBA,XYZ2} -> output at TOPS+0x81..0x84, +4/vert
    xgkick at TOPS+0x84

**Per-vertex binding: the low bits of the position W float are a VU1
dmem address.** W = ±(1 + small mantissa); reading its bit pattern:

    bits 0..9   dmem qword address of the vertex's matrix set
                = 8 * node_index (sets are 7 qw: 4 transform rows +
                3 normal-matrix rows, 8-qw stride). Lowest used slot
                is qw 16 = node 2 (root/hips carry no skin).
    bit 15      strip-restart (no triangle emitted; the kernel also
                ORs it with the clip flags to kill the GIF ADC bit)
    sign + bit14  winding parity / restart pairing

So vertex node = (W_bits & 0x3FF) >> 3. The old "w-field k =
round((w-sign)*512) per-block 4-bone selector" reading is dead: the
"k set {-3,-1,0,2}" was bits 14/15 + sign (strip flags), identical in
every block, and the "per-block float base epsilon" was the address.

The engine uploads each node's {world-composited transform, normal
matrix} set to dmem qw 8*n before kicking the blob (matrices are
pre-multiplied with the camera: the kernel clips/divides immediately).

### Validation (the acceptance test)

Exported chunk28/f00_id3b.bin (the character standing in the live
scene) with node = addr>>3 binding and the live node arena's 21 world
matrices (+0x90) as a one-frame palette: the port (extermination-port,
Metal, EM_CAPTURE) renders a fully coherent posed character — head,
face, torso, belt, arms with hands, knee pads, boots. Quantitative
check: mean posed length of node-crossing triangle edges has a SHARP
minimum at this mapping (0.88 units vs 3.6–8.6 for every shifted
mapping; intra-node baseline 0.41).

Corrections to older notes this implies:

- `extract/live/player_bones_live.json` (session 1) is the rig of the
  chunk28 character (same world spot 107.4/−184), NOT the chunk21
  f17_id8f model — its matrices fit f00_id3b (score 0.88) and fit
  f17_id8f at NO shift (≥4.1). chunk21/f17_id8f is a different
  costume/variant; its segment 1 addresses 42 node slots (rig pairing
  unresolved — possibly body+suit double rig or merged LODs).
- A model file can hold several block segments separated by
  MATRIX-descriptor blocks (f17: 132-block segment 0 with 17 slots,
  181-block segment 1 with 42 slots, plus 41 trailing 256-qw blocks).
- `tools/export_native.py` decodes all of this (`--segment`, `--live`)
  and bakes per-vertex node indices into EMDL; the port needed no
  changes.

### Addendum (2026-06-09 s3): hand/forearm artifacts explained; capture protocol

The small glitches around the hands/forearms in the first posed capture
were NOT an engine feature and NOT IK: they were temporal tearing in the
LIVE CAPTURE — the node arena was read in two MCP requests while the
emulator was running, so nodes 19/20 (the hands) came from a different
animation frame than nodes 0..18. **Protocol: pause the emulator before
multi-read captures of animated state.** With an atomic paused capture,
all 21 node world matrices are exactly parent x local(quat,trans)
compositions (max deviation 0.007 across the rig) — no IK layer exists
in the idle pose. The remaining "odd" right-hand shape is intentional
animation: the hand is curled in a WEAPON-GRIP pose; the gun is a
separate model attached to a hand node at draw time (not exported yet).

Still open (nice-to-have, not blocking): ~~map A's A/B field encoding
(rotation qx/qz companions) and map B/C payload packing for decoding
the mesh-embedded clips~~ (RESOLVED — next section); the f17 42-slot
segment's rig identity; UVs/textures in EMDL.

## id 0x74 channel encodings FULLY DECODED — animated character in the port (2026-06-09, session 4)

Closes the channel-encoding question from sessions 2/3. **The id 0x74
container's record format is byte-identical to the id 0x71 clip format
already decoded in `tools/anim_decoder.py`**: each 12-byte record is a
10-byte LSB-first bitstream of truncated IEEE-754 floats (top `W` bits
kept, low mantissa bits zeroed; decode = `raw << (32-W)` bit-cast to
f32) followed by `u16 frame` at +0x0a (key time, 0xffff terminator):

    map A (rotation)     4 x 20 bits  = local quat (qx, qy, qz, qw)
    map B (translation)  3 x 26 bits  = local translation (x, y, z)
    map C (scale)        3 x 26 bits  = local scale (so far always 1,1,1)

Session 2's partial reading — `{i16 A, f20 W1, i16 B, f20 W2, tag
nibble 0x3/0xB}` — was this bitstream misparsed at byte offsets:

- "A"/"B" are the LOW 16 BITS of the qx/qz 20-bit fields (bits 0..19
  and 40..59 of the sample);
- the "tag nibble" is their TOP 4 BITS: sign bit + the constant 0b011
  exponent prefix every |q|<2 float starts with (hence "0x3 or 0xB");
- "W1"/"W2" are the qy/qw fields (bits 20..39, 60..79), whose 32-bit
  lanes happen to land on the +0x02/+0x07 byte offsets.

Keys are sparse per node with LERP-between-keys semantics (value at
`frame` = that record's sample; engine SLERPs, NLERP is within ~0.6 deg
on 40-deg key gaps), duplicated end keys, hold past the last key.
**Hemisphere caveat (2026-06-09 s7c):** stored keys freely flip
quaternion sign between consecutive records (clip 346 bone 2, frames
96→100 and 160→164: dot(q_prev, q_next) = −0.9993, antipodal encodings
of near-identical rotations). Any interpolator must negate one side
when the dot is negative — `anim_decoder.sample_bone` now does; without
it the midpoint NLERP normalises to a ~180°-wrong quat (the port's
one-frame torso-flip bug at baked frames 98/162).

### Containers are a whole animation LIBRARY

`chunk28/f01_id3c.bin` holds **455 containers** back-to-back (the old
"50" was a 0x40000-byte truncated scan; `find_id74_headers()` in
extract_models.py now walks whole files). All 21-node, lengths 1..180
frames: container 0 = the 80-frame breathing idle, container 346
(@0x1c4c00) = a 180-frame look-around fidget with up to 50-deg node
rotations and animated hip translation. The live NPC cycles through
the library with cross-fade transitions — a paused capture taken
mid-transition matches NO single clip (this is a blend, not a decode
failure; three captures only matched single clips when the blend had
settled).

### Verification evidence

1. **Static, no live data**: decoding map A as 4x20 gives |q| = 1 to
   within 3.1e-4 (exactly the 11-bit-mantissa truncation noise) on
   every keyframe of every container in the file. Any wrong field
   split destroys this immediately.
2. **Live quats (PCSX2, paused captures, NPC node arena 0x007d5840
   stride 0xD0, +0x30 local quat)**: capture A matched container 0 @
   frame 34.2 with max error 0.153 deg over ALL 21 nodes; capture B
   matched container 346 @ frame 120.6 — node 10: decoded
   (+0.6107,-0.5206,+0.3090,+0.5104) = live EXACT (0.0000 deg), node
   19: (-0.1038,0,0,+0.9946) = live EXACT, node 4: 0.018 deg. The live
   +0x30 quats are the RAW TRUNCATED values (|q| ~ 0.9997 — the engine
   never renormalises), which is itself a fingerprint of this encoding.
3. **Translations**: map B values equal live node +0x00 exactly
   (hip y=10.9008, L/R mirror pairs 5/6, 8/9, 10/11, 17/18, 19/20;
   only the pelvis node 4 and — in moving clips — the hip node 1 have
   animated translation).
4. **Composition convention**: building locals as R(CONJUGATE(q)) and
   composing world = parent_world * local via the prefix parent table
   reproduces the live +0x90 world matrices (relative to root) with
   max element error 0.008; the un-conjugated quat gives 15.2. This is
   the same conjugate convention noted for +0x30 in session 2.
5. **Port acceptance test**: `export_native.py --mesh
   extract/chunk28/f00_id3b.bin --anim extract/chunk28/f01_id3c.bin
   --clip 346` bakes a 180-frame EMDL palette (60 fps — live cursor:
   node +0x50 = phase 0..1, +0x54 = 1/(clip_len-1) per 60 Hz tick);
   the port plays it unmodified. EM_CAPTURE at frame 60 (t = 1.0 s)
   shows a coherent character in a clearly different pose than frame 0
   (left arm lowered, head turned vs both hands raised).

### Node-struct cursor addendum (+0x50 block)

`+0x50` f32 clip phase in [0,1); `+0x54` f32 phase rate = 1/(clip_len-1)
per 60 Hz tick (so clip frames advance at 60 fps and current frame =
phase * (clip_len-1), confirmed against matched frames on two clips);
`+0x58/+0x5c/+0x60` are per-channel cursor floats (key-segment state;
not needed for offline decode).

### Tooling

- `extract_models.py`: `parse_id74_prefix(d, hdr=None)` now returns
  decoded channels {rot, trn, scl} as per-node (frame, values) lists;
  `find_id74_headers(d)` enumerates every container in a file. The old
  A/B/W1/W2 tuple shape is gone (`--object-space` dump updated).
- `export_native.py`: `--anim <library file> --clip N` bakes a
  multi-frame EMDL (per-frame TRS sampling, conjugate-quat locals,
  parent-table composition, recentre, fps=60). `--live` and the
  identity-palette fallback unchanged; the port needed no changes.

## chunk21/f17_id8f is an ENCOUNTER PACKAGE — 42-slot rig resolved (2026-06-09)

Resolved by disc-data-only structural analysis (`tools/rig_probe.py`:
probe / survey / compare). Full evidence in the tool's output; summary:

**File layout (358 blocks):**
- `0x0..0xf870` — prefix: id 0x74 21-node container + id 0x2c 3-node +
  3x id 0x24 1-node, ALL with the same 401-frame clip length: a
  synchronized multi-actor (cutscene) track set. The 21-node parent table
  is byte-identical to chunk28's player/NPC table.
- `0xf870..0x528f0` — segment 0: 132 MESH blocks, skinned slots 3..19.
- `0x528f0..0xd7070` — an ANIMATION BANK (previously misread as a "MATRIX
  separator" block): 11 id-0x70 clips on a 20-node parent table + 30
  id-0xd0 clips (40..586 frames) on a 44-node parent table.
- `0xd7070..0x132f10` — segment 1: 181 MESH blocks, skinned slots 2..43.
- 41 trailing 256-qw raw VIF blocks (different sig `...80006c`, not
  W-bit-skinned; unrelated).

**Segment 1 = one 44-node creature.** Parent table
`[-1,0,1,1,3,2,3,2,3,3,8,5,7,9,6,9,4,15,15,15,15,13,10,12,11,15,14,15,
15,16,15,21,25,22,23,24,26,29,30,31,32,36,39,40]` — single root, depth 8,
14 leaves; does not split into two trees and does not embed the 21-node
table at any offset. Slot histogram over 5792 vertices: contiguous 2..43,
every slot used (kills the two-stacked-rigs hypothesis). Fingerprint: the
tree's mirror-symmetric chains (5→11→24→35 vs 7→12→23→34) correspond to
exactly the three slot pairs with byte-identical bbox dimensions and
z-mirrored centroids — (11,12), (23,24), (34,35). Geometry: bone-local
bboxes 3..23 units (≈3x humanoid), node 15 is a hub with 8 children
sprouting long chains (multi-appendage body plan); slot 28 = dense
530-vert core, slots 33/38/41/43 = long thin tips.

**Segment 0 binds the in-file 20-node id-0x70 table, NOT the 0x74
prefix.** Mirrored-geometry slot pairs (8,9),(10,11),(14,15),(16,17),
(18,19) are all mirror nodes of the 20-node tree and contradict the
21-node tree's mirrors ((15,16)/(19,20)); slot 20 is unused.

**Generalization (survey of 126 prefixed files):** several other
multi-model packages pair mesh segments with in-file parent tables of
size max_slot+1 — chunk05.n0/f14_id88 (47-node), chunk17/f14_id8b
(33-node, 74 clips), chunk25/f29_id95 (33-node), chunk12.n0/f13_id8b
(the common 30-node enemy rig, bank files chunk*/f0x_id71.bin). The
44-node table also appears in chunk21/f09_id99.bin@0xcfde0 (same chunk,
same creature). Exporter rule: pair each segment with the in-file table
where n == max_slot + 1; pose with that bank's clips via the (decoded)
map-A/B/C channels. `parse_id74_prefix()` returns only the first
container — multi-bank files need whole-file enumeration
(`find_id74_headers()` / rig_probe's scan).

## Texture COLOR recovered — CLUT pairing rule + GS-VRAM offset fix (2026-06-09)

First colored textures for the project, extracted from existing .p2s save
states by `tools/clut_pair.py` (output under git-ignored
`extract/textures_colored/{01,02,03}/` with per-pair confidence manifests).
Validated screenshot-exact on save state 02 (title screen): the
EXTERMINATION wordmark in silver/white over the purple/magenta X-ray hand,
and a legible, correctly-colored copyright line. State 01 (snow level):
high-confidence snow-particle sprites + four 256x128 night-palette terrain
atlases; the CHARACTER's textures were NOT isolated (see below).

**Pairing rule (no disc binding exists):**
1. The only texture<->CLUT association is the runtime-built TEX0 qword:
   TBP0 (bits 0-13) names the texture, CBP (37-50) its CLUT. Harvest TEX0s
   by scanning EE RAM + VU1 dmem for 8-aligned qwords with PSM=0x13,
   TW/TH 4-10, TBW in {2,4,8,16}, CPSM=CSM=CSA=0 — the engine's GIF/VIF
   ring buffers persist in a save state.
2. Strip-allocator rule (validated in both captures): a 512-wide sheet is
   consumed as 256x128 strips; strip k at TBP0 = base + 64k pairs with
   CBP = clut_base + 4k.
3. **GS local memory starts at offset 425 in the .p2s v9 GS freeze blob
   (len - 0x400000 - 84; 84 trailing state bytes), NOT 509.** Proven by
   byte-exact 8KB-page anchoring of a disc upload simulated through the
   PSMCT32 swizzle. ALL prior GS-VRAM reads were skewed. **FIXED
   2026-06-09:** `gs_vram.py` now computes base 425 (`VRAM_TRAILER = 84`
   is the single source of truth; `clut_pair.py` imports it).
   `parse_pcsx2_state.py` was never affected (it extracts the blob
   verbatim, no offset math). The skewed-read 2026-06-02 sections above
   carry correction notes; their decisive "zero disc matches" negative was
   re-verified clean at base 425 (states 01 and 02: 0 exact matches vs the
   361-blob disc pool). `clut_bruteforce.collect_clut_pool` now skips
   tool-output dirs under `extract/` (`textures_colored`, `gsdump`,
   `live`, `scratch`) so capture files can't masquerade as disc blobs.
   Regression guard: `verify_all.py` stage **`gs-offset`** cross-validates
   `gs_vram.localmem_base` (formula) against `clut_pair.detect_vram_base`
   (palette-scored) on a local PCSX2 save state and asserts the corrected
   base strictly out-scores the legacy one (observed 7>4 on state 01,
   10>7 on state 02).
4. CSM1 CLUT entry swap (8-15 <-> 16-23 per 32 entries) + alpha 0..0x80
   scaled x2, as previously documented.

**Why the character resisted:** characters/level geometry draw through the
VU1 path — their TEX0s are built in VIF/VU1 buffers and do not persist in a
freeze (VU1 dmem retains only the last batch; in state 01 that was the
snowflakes). Their textures are micro-tile subrects of the trio sheets
(content-anchored byte-exactly back to disc at block granularity), but the
per-subrect CLUT slots cannot be attested from a freeze. **Cheap fix: a
PCSX2 GS dump (.gs) records every TEX0 of a frame** — one dump of a
character scene yields the complete per-subrect pairing table
(`tools/parse_gsdump.py` exists). That + clut_pair.py's machinery is the
wiring path for colored glTF/EMDL export.

## CHARACTER TEXTURES SOLVED — marker qword IS TEX0; textured character in the port (2026-06-09, session 5)

Closes the texture chain end-to-end: the native port renders the chunk28
character **colored, textured and animated** (EM_CAPTURE acceptance test,
clip 346 frame 60: face with skin tones + brown hair, urban-camo fatigues,
shoulder insignia, knee pads, buckled boots).

### The decisive finding: the vertex "marker" qword IS the TEX0 value

Each mesh record's qword 0 ([tex/marker][ST][normal][pos+w]) is the draw's
complete **TEX0 register value with TBP0, TBW, PSM, TW, TH AND CBP baked
in, stored verbatim in the disc file**. f00_id3b.bin carries 102 distinct
marker qwords = 51 unique (TBP0, CBP) pairs x 2 CLD variants (CLD=1 load
CLUT on first use / CLD=0 reuse; bits 61-63 are the only difference). So
for MESHES the texture<->CLUT binding IS disc-static after all — the
earlier "no disc binding exists" finding still holds for the UI/sprite
path (PSMT8 sheets with runtime-assembled strip CLUTs), but character/
level-geometry pairings ship in the vertex stream. The VU1 kernel copies
the marker through to the GIF packet, which is why TEX0 writes appear
per-vertex in a GS dump (49,424 writes in one frame).

Qword 1 = the vertex's **normalized ST** (true UVs, REPEAT semantics —
values run past 1, observed up to ~21.8; the kernel multiplies by Q=1/w
only for the GS's STQ perspective division, so the disc floats are the
real texture coordinates).

### Characters are PSMT4 (16-color), not PSMT8

All 51 character pairings have PSM=0x14 (PSMT4) — this is why the .p2s
EE-RAM harvest (PSM==0x13 filter) never saw them. Each is a small
standalone texture (32x16 .. 128x128, TBW=8) with its own 16-entry CLUT
at its own CBP (CSA=0, CPSM=PSMCT32, CSM1). The 16-entry CSM1 CLUT
occupies the first 64 bytes at CBP*256 as an 8x2 PSMCT32 region: linear
entry i sits at word [0,1,4,5,8,9,12,13,2,3,6,7,10,11,14,15][i]
(columnTable32 rows 0-1). PSMT4 texel swizzle (128x128 pages, 8x4 blocks
of 32x16, nibble-addressed): tools/extract_textures.py `BLK4`/`COL4`/
`psmt4_nibble` (tables validated as a 0..511 permutation);
clut_pair.py `read_psmt4` / `read_clut16_rgba`.

### GS dump (.gs) capture + tooling

A 1-frame PCSX2 GS dump (Tools > Save Single Frame GS Dump; needs "Show
Advanced Settings"; lands in PCSX2/snaps/*.gs.zst) of the office scene
with the character on screen:

- Container: magic 0xFFFFFFFF, header, **freeze blob with the SAME
  425/+4MB/84-trailer layout as the .p2s GS.bin** (state_size 0x4001fd),
  8KB GSPrivRegSet, then packets (Transfer/VSync/ReadFIFO2/Registers).
- This frame: 4 vsyncs, 3,140 transfer packets — ALL PATH1 (VU1 XGKICK;
  confirms everything draws through VU1), ZERO image-mode uploads (all
  textures/CLUTs already resident -> the dump's own VRAM snapshot is
  authoritative for the whole frame).
- `tools/parse_gsdump.py` (rewritten): full GIF-stream walker (PACKED
  incl. A+D, REGLIST, IMAGE; PRE/PRIM, TEX2 merge, per-context TEX0,
  ADC) -> per-draw runs with vert counts + ST/UV bboxes; `--json` table;
  `--textures` emits colored PNGs straight from the dump (PSMT4+PSMT8).
  Frame stats: 14,328 draw runs, 171 unique textured pairings (51 = the
  character, rest = level + UI).
- Cross-validation: **all 51 mesh-marker pairs of f00_id3b appear in the
  dump's per-draw table** (100%); extracted PNGs are recognizably the
  character's face/gear at correct colors.

### Pipeline wiring (EMDL v2 + port)

- `tools/export_native.py`: reads per-vertex UV (qword 1) + marker TEX0
  (qword 0, CLD-masked dedup) -> texture table; `--gsdump frame.gs`
  resolves each TEX0 against the dump's VRAM through the PSMT4/8 + CLUT
  machinery and embeds native-size RGBA8 textures.
  (2026-06-10 s18: `tools/export_gltf.py` character mode consumes the
  same path — `--gsdump/--p2s` group primitives per CLD-masked TEX0 key
  and embed the build_texture_blob RGBA output as per-material PNGs;
  validated 51/51 on f00_id3b + office dump, 68/68 on f18_id94 seg 1 +
  save state 01.) **EMDL v2** ("EMD2"):
  header gains tex_count; tex table {w, h, byte_offset}; vertex = pos3,
  nrm3, uv2, bone, tex (10 words); RGBA8 texel blob at EOF. Export of
  f00_id3b + clip 346: 2,598 verts, 3,170 tris, 51 textures, 432 KB
  texels.
- Port (extermination-port): em_model loads EMD2 (+bounds checks);
  em_gfx mesh API takes the texture table; the Metal backend packs all
  textures into a `texture2d_array` whose slices hold each texture TILED
  to the common max pow-2 size (128x128 here) so sampler REPEAT
  reproduces GS wrap for every sub-size at any UV; per-texture float2
  UV scale in buffer 3; skinning shader passes scaled UV + [[flat]]
  slice; fragment = texture * directional light. Untextured verts
  (tex=~0) keep the flat-grey path; meshes with no textures bind a 1x1
  white slice so the PSO stays valid.

Remaining (nice-to-have): per-vertex lighting RGBA from the kernel's
light matrix (qw 1013..1016) instead of the port's stand-in directional
light; alpha (PSMT4 entries carry GS alpha — currently exported but
drawn opaque); the gun (separate model attached to a hand node).

## ENGINE FRAME ANATOMY — main loop, task dispatch, vsync, input (2026-06-09, live session)

Live PCSX2 DebugServer analysis (PC sampling located the loop; full native
disasm + live table reads + three lockstep per-frame counters carried the
proof: 0x00810E90 vsync count, 0x70003B64 main-loop count, 0x00810750 /
0x70003B68 gameplay count). This section is the semantic skeleton for the
native port's game loop.

### Main loop: `func_001AAE40` (924 B, never returns; called from crt0 tail)

INIT (once): engine bring-up (func_001AB1E0 → func_0010E088/00112F98/
001138D8, polls func_00110508(0x0027DB70)); audio init (func_001FEE60,
func_001FB210, func_001F9820); T0_MODE=0x83; pad init cluster
(func_001AB370, func_001CCCC0, func_001CCBD0(0,0x3FFF,0), func_001AB430);
**func_001F9780 creates THREAD 3** (entry func_001FB0C0, prio 2, TID at
0x00282184; main thread demoted to prio 5) — the audio/SIF-RPC pump:
`loop { SleepThread(); while (func_001192D0()); func_001152B0(); }`;
**func_00101548(0x001AB140) = AddIntcHandler(2 /*VBLANK_S*/, handler, -1)**;
func_001AB650 zeroes the 3-slot frame-task table at 0x0028A750;
**func_001AB740(0, 0x001AB7E0) = task_register(slot 0, boot/flow task)**.

PER FRAME (ordered):
```
A  sw 0 -> 0x00810E98                clear vsync flag (loop top 0x001AAF28)
B  func_001D1AE0(frame_idx)          frame begin: select per-frame CPU packet
                                     arena (gp-0x7D00 stream: +0x9C idx, +0x10
                                     cursor; base 0x0028F700, ~0x95800/frame)
C  func_001B57E0                     INPUT read/unpack (see Input below)
D  func_001AEBE0                     screen-fade machine (0x0028A8D0)
E  func_001AB6A0                     TASK DISPATCH: jalr *(slot+4) over the
                                     3x0x20 table @0x0028A750 -> ALL game logic
F  func_001FCA10                     audio service
G  func_001AEE70                     transition/brightness machine
                                     (jr-table 0x0026DD50, 7 states)
H  func_001FB100                     audio service
I  func_001B5B70                     input post-process
J  v=func_00100A60(0,0)              VIF1/GIF/VU1 sync; if idle: conditional
                                     VU0/VU1 macro block (0x0011A9D8..0x0011B910)
K  func_001D7410                     gated *(gp-0x7768): GS-VRAM READBACK path —
                                     the ONLY caller of func_00100EB8 (@0x001D74C4);
                                     disabled in all live samples
L  func_001AB590                     DMA CHCR watchdog (D0/D1/D2 MOD bits)
M  if byte 0x00821058==1: func_00203350   audio
N  func_001D1C10(frame_idx)          frame-end render bookkeeping
O  func_001AEE70                     (second fade update)
P  WAIT @0x001AAFF0: poll 0x00810E98 until nonzero   (vsync)
Q  sw 0 -> 0x10000000                T0_COUNT reset
R  func_001AB4E0(spad 3B94/3B96)     display-offset apply
S  func_001015A8/func_00101810       PutDispEnv x2 (dispenv @0x810F00/0x811070
                                     and 0x810F80/0x8110F0, FIELD-indexed)
T  func_0010BAA0(0)                  GS register apply
U  func_00100550(0x00810EA0+idx*0x28) per-frame GS env (double-buffered)
V  func_001D2300                     render frame-flip bookkeeping
W  frame_idx ^= 1 (sh 0x00810E80); func_001D2580; (0x70003B64)++ -> A
```

### Vsync ISR — RESOLVED

`func_001AB140` (registered above): di-guarded; **increments 0x00810E98**
(the wait flag) and 0x00810E90 (lifetime vsync counter); samples GS CSR
0x12001000 bit 13 (FIELD) -> halfword 0x00810E88 (drives the R-S dispenv
selection); **iWakeupThread(TID3)** — audio pump runs once per vsync.
Syscall stubs verified: 0x0010B740 SleepThread, 0x0010B760 iWakeupThread,
0x0010B850 iSignalSema, 0x0010B860 WaitSema, 0x0010B500 AddIntcHandler.
Threads: TID1 main (prio 5, busy-polls); TID3 audio pump (prio 2);
TID2 sema-wait worker (entry 0x00111680, created at 0x001117C4 — CD/
streaming service); one SDK-internal thread (entry 0x0010C5C8, prio 0).

### Task dispatch + the live game-update chain

`func_001AB6A0` iterates the 3-slot table @0x0028A750 (byte+0 state:
1/4 -> 2; 2 = run; 0 = skip; word+4 = fn; slot ptr parked in spad
0x70003B6C; `jalr` at 0x001AB704). Live: slot 0 = **func_001ACEC0** (game
task; replaced boot task 0x001AB7E0), slot 2 = 0x001FF0D0 dormant audio.

Chain (state bytes live 03/01/01 at task+8/+9/+0xB):
```
func_001ACEC0  game task machine (byte+8)
 └ func_001AD250  sub-machine (jr-table 0x0026DCB0, 6 states)
   └ func_001AD4D0 = j func_001AE040 (trampoline — explains its "0 callers")
     └ func_001AE040  in-game frame machine (byte+0xB, jr-table 0x0026DD30)
       state 1: func_001AFCF0 (per-frame flag reset), func_0018AB00
       (difficulty map), func_001B07C0(1) (player placement vs area spawn
       tables 0x0024D650, area ids @0x810700), func_001C1DC0 (camera),
       func_0018D7B0/func_0018C0D0(0x008101E0,1) (HUD/weapon context),
       func_001AEE10(4,0), func_001FAE70(0), func_001C5C50,
       v=func_001AE7E0 (end-of-level/game-over poll),
       then selector spad 0x70003B8D: -> func_001AE5E0 (gameplay frame)
       or func_001AE6B0 (cutscene variant; polls buttons 0x0900 @0x810E74)
```
**func_001AE5E0 — THE GAMEPLAY FRAME** (counters 0x00810750/0x70003B68++):
```
func_001CB590(0x008102B0, 0x320, byte 0x008102B9)  actor-context begin
                                  (sets the D_00275B40 node-table base)
func_0015BCF0(*(gp-0x782C)=0x008102B0)   PLAYER ACTOR UPDATE
func_001CB5A0                            actor-context end
func_001D1C50                            RENDER CHAIN BUILD (-> func_001D2830/
                                         7C30/30A0 walkers -> VIF1 kick via
                                         func_00101BB8/func_00101FE0)
func_001C1D00(0x008101D0)                camera apply
func_001AFD70(0); func_0015C160; func_001F0360     world services
func_001CB590(0x008101E0, 0xD0, 0)       HUD/second context begin
func_0018B9C0(0x008102B0)                view-target update (0x008105F0)
func_001CB5A0; func_001AAD00; func_001D1EA0(1)     close-out
```
Idle-room census: ONE live actor, the player struct @0x008102B0 (size
0x320). func_0015BCF0: per-actor spine — pos qword copies (func_00102948),
state/AI via func_0015BA50, anim-evaluator SELECTION among func_001C6DA0 /
func_001C68C0 / func_001C6960 via per-anim-id table @0x00248C90, physics
(func_0015CF90/func_0015CBA0/func_00187350), death check (Y < -200.0f ->
state 6), sound triggers (func_0011A070), pos mirror to spad 0x70003B40.
Multi-actor anim dispatchers func_0017A130/func_00148B40 have zero static
AND zero live-overlay callers — reached only via actor-struct fn pointers
when NPCs exist. Area/room logic installs at scene init: func_001AFCA0 ->
func_001D0660 -> func_001E7780 (area-ID dispatch to 17 overlay entries).

Bone-publish caller chain (player): main loop -> E -> func_001ACEC0 ->
func_001AD250 -> func_001AE040 -> func_001AE5E0 -> func_0015BCF0 ->
func_0015BA50 -> func_0015B130/func_0015B530 -> func_001612D0/
func_001837B0 -> func_0017C030 -> func_0017B660 -> publish into the
0x287F40 pair; anim eval also called directly from func_0015BCF0.

### Input path — RESOLVED (high confidence)

`func_001B57E0` (step C) -> `func_001B5F40(0x00810E70, 0x00810E40)`:
0x00810E40 = raw libpad RPC landing buffer, unpacked to the FRAME INPUT
BLOCK 0x00810E60..0x00810E7B — analog bytes @0x810E64/65 (0x80-centered
on failure), button halfwords @0x810E70/72/74/76/78/7A (current/pressed/
released triples). Consumers verified: func_001AE6B0 (mask 0x0900 skip),
pause bytes 0x008106B8/B9. Pad init: func_001CCCC0 / func_001CCBD0
(0,0x3FFF,0). The port's em_input replaces exactly this block.

### Ranked decomp targets for the port's game loop

1. func_001AAE40 (main loop, 924 B — mechanical now)
2. func_001AE040 + func_001ACEC0/func_001AD250 (frame state machines;
   jr-tables 0x0026DD30/0x0026DCB0/0x0026DC50 mapped)
3. func_0015BCF0 + func_0015BA50 (actor spine; struct fields +0x20C anim
   id, +0x2F3/+0x303 mode bytes, +0xA0/B0 pos)
4. func_001D1C50 (render-chain head)
5. task system: func_001AB6A0/740/650/7E0 (~800 B total)
6. func_001AB140 + func_00101548 (vsync ISR contract)
7. func_001B5F40 (pad unpack ABI)
8. func_001AE5E0/func_001AE6B0 (gameplay frame variants)
9. func_001D1AE0/func_001D1C10/func_001D2300 (packet-arena model)
10. func_001B07C0 (player placement / area spawn tables 0x0024D650)

Thin spots (stated plainly): the conditional VU block (0x0011A9D8..
0x0011B910) classified by gating/address-cluster only; func_001FCA10/
func_001FB100/func_00203350 are "audio service" by subsystem range;
TID2's exact role inferred from creation site; func_0018D7B0/func_0018C0D0
not disassembled.

## LEVEL RENDER MESH — the drawn level is NOT the id 0x44 file (2026-06-09, session 6)

> **2026-06-09 s8 CORRECTIONS** (see "LEVEL RENDER MESH v2" below): the
> record row order documented here is off by one row — the true order is
> the character order [TEX0][ST][color][pos+W] (the file starts
> mid-record); and the id 0x44 file DOES carry a second static render
> section in its tail (the "related files" paragraph below is stale).

The engine does not draw the `id 0x44` static level geometry. Each level
chunk carries a separate **VU1-ready render mesh** file, and that is what
streams through the 32-record VU1 kernel blocks seen in the GS dump. For
the office scene (the live GS-dump frame) the level is **chunk06.n1** and
its render mesh is `chunk06.n1/f03_id43.bin` (the file id varies per
level — chunk15's and chunk17's `id 0x43` files are SShd sound banks, so
the id is not a role tag here).

### Record format (64 bytes, four 16-byte rows)

| Row | Contents |
|---|---|
| `+0x00` | vec4 world-space position `(x, y, z, w)`, `|w| ~= 1.0`. W read as an int carries the SAME flag bits as the character kernel: bits 0..9 = VU1 dmem matrix-slot address (0 = static world, nonzero = movable sub-object e.g. doors), **bit 15 = ADC** — set on a strip's priming vertices and on padding, clear on triangle-kicking vertices. |
| `+0x10` | the draw's complete **TEX0 register value, verbatim** (TBP0/TBW/PSM/TW/TH/CBP), + 8 zero bytes. |
| `+0x20` | vec4 `(u, v, 1.0, 0.0)` — normalized per-texture ST, REPEAT semantics (NOT sheet-atlas coordinates). |
| `+0x30` | vec4 **baked vertex color** `(r, g, b, 1.0)` — the level's baked lighting — or a unit normal `(nx,ny,nz, 0.0)` on some records (same `|xyz|~=1` disambiguation as id44 geometry). |

Framing: fixed **0x880-byte blocks** = a 0x80-byte header (`01 00 00 00`
word quad + AABB rows) + 32 records; blocks are padded by duplicating the
last record (the model-format idiom). A tolerant walker that validates
`|pos.w|~=1` + TEX0-row zero tail + ST-row `(1.0, 0.0)` and resyncs by 16
bytes handles all of it. Decoder: `tools/export_level.py`.

### Why this closes level texture COLOR

The per-record TEX0 is disc-static exactly like the character mesh: 84 of
the 85 level TEX0 pairings of the office GS-dump frame appear byte-exact
(CLD-masked) in `f03_id43.bin` — so the level inherits the whole
character texture pipeline (PSMT4 + per-texture 16-entry CLUT resolved
from the dump's VRAM snapshot). The office mesh also references one
PSMCT32 (psm=0) texture (the window/backdrop) — `export_level.py` reads
it straight out of VRAM — and a few PSMT8 ones.

The dump's level textures are a sequential **pack**: tbp0 descends
through size classes (128x128 down to 32x16) with deltas equal to each
texture's block footprint, CLUTs packed densely behind (CBP 11912..12151
for the office; tiny textures carry their CLUT adjacent, cbp = tbp0+1
or +3). The old "512-wide PSMT8 atlas sheet" reading of the level sheet
uploads is therefore a misinterpretation: that VRAM region is the packed
PSMT4 texture + CLUT array; reading it as PSMT8 just happens to produce
structured-looking grayscale.

### GS-dump streaming behavior (useful for future cross-validation)

- The VU1 kernel emits **every** block, visible or not: culled/off-screen
  blocks stream as runs of identical parked vertices with ADC set (which
  is why per-TEX0 vertex totals in a dump roughly match file totals).
- A `.gs` dump spans multiple VSyncs — per-TEX0 vertex totals are ~2-3x a
  single frame's.
- Clipped triangles re-enter the stream as clipper output (guard-band
  coordinates, x/y = 4088.0).
- `parse_gsdump.py` reads the PACKED XYZ2 ADC bit from byte 15; the ADC
  bit is bit 111 = byte 13 bit 7 (kick counts in its tables are slightly
  off; pairing tables unaffected).

### Scene identification (and a trap)

The only in-game save state on disk is the SNOW level (chunk15) — EE-RAM
residency from it identifies the wrong level for the office dump. The
office was pinned three ways: (1) TEX0-key intersection — f03_id43
uniquely covers 84/85 of the dump's level pairings; (2) its bbox
X[-25,120] Y[-10,50] Z[-305,50] contains the live character position
(107.4, 0, -184) with floor y=0 there; (3) recognizable office textures
(lockers, binders, keypads) decode from the dump VRAM via its TEX0s.
For level-vs-dump comparisons, only TEX0 *content* (VRAM bytes) and
key intersections are trustworthy — TEX0 VALUES alias heavily across
levels because every level reuses the same VRAM slot allocator.

### Related files, still open

- `chunk06.n1/f02_id44.bin` (the id 0x44 static mesh) is RAM-resident
  byte-identical at runtime but its markers are NOT TEX0s and none of the
  dump's TEX0s exist anywhere in EE RAM as marker rows — the id44 file is
  not the drawn geometry; likely collision/visibility data. Its
  marker→texture translation (per-level material table) stays undecoded.
- `chunk27/f01_id37.bin` is an object-space **model library** (~21.8k
  records, 235 TEX0 keys, bbox ±25 units, multi-scene resident) — the
  props/pickups drawn in the frame. Placing them needs runtime entity
  data; not exported yet.
- EMDL v2 header `reserved` is now a **flags** word: bit 0 = "the normal
  slot carries a baked RGB vertex color" (written by export_level.py,
  consumed by the port's shader as texture*color GS-modulate).

## COLLISION WORLD — id 0x44 CONFIRMED as the collision/world-query data (2026-06-09, session 7)

The "the engine never draws the id 0x44 file" puzzle is resolved: **the
id 0x44 file is the level's collision world**, and the `level_world`
cluster (0x19Axxx–0x1A7xxx) is the collision/query library. Confirmed
three independent ways (static .s reading + disc-file decoding + EE-RAM
dumps from the on-disk save states; no live debugger used).

### 1. Runtime query API (the hubs all 4 reach from the actor spine)

All four hub functions stage a query **segment** in scratchpad
(`0x70003190` = start vec4, `0x700031A0` = end) and dispatch over a
caller mask, one bit per collision SET; each set test clamps the
segment end to its nearest hit, so the final result is the closest hit
across sets. Return = bitfield of the set that produced the final hit
(also stored to `0x700031D8`):

- `func_0019A570(float* from, float* to, u8 set_mask, u16 param)` —
  plain segment/ray query. mask bit0 → `func_001A6440` (movable-object
  hulls), bit1 → `func_001A0B10` (static cell/n-gon world), bit2 →
  `func_0019D330` (s16-grid heightfield world).
- `func_0019AD00(actor*, float* target, u32 mask)` — actor move-probe:
  start = actor position (+0xB0/+0xB8 = x/z), end extended 0.01 past
  the target (`f12 = 0x3C23D70A`); on hit with mask bit31 set it ADDS
  the hit-minus-end delta back into actor x/z — i.e. **collide-and-
  slide response**. Consults actor +0x00 bit0, +0x02 & 0x1F (matrix /
  world slot), +0x52 flags; object set via `func_001A7280`, entity set
  via `func_0019FE50(actor->+0x14)`, plus `func_0019CB60`.
- `func_0019AFE0(actor*, float* from, float* to, u32 mask)` — same
  shape, explicit segment (736 B twin of 0019AD00).
- Scratchpad result block: hit point `0x700031B0`, hit-minus-end delta
  `0x700031C0`, result record ptr `0x700031D0`, hit object `0x700031D4`,
  hit kind `0x700031D8`, hit id `0x7000324E`, surface attr `0x700030CC`,
  poly id word `0x700030D0`, surface-type byte staged at `0x70003B88`
  (values 0x50..0x59 are conditional/one-way surfaces, special-cased in
  func_001A0B10 against `0x7000324E`).

Caller linkage (tools/callgraph.py): the actor-update spine reaches all
four hubs — `func_0015BCF0 → func_0015BA50 → func_0015B130 →
{func_0016EBA0 → func_0019AD00, func_001647D0 → func_0019AFE0,
func_00161020 → func_00175900 → func_0019AB20, func_00162DB0 →
func_0017D080 → func_0019A570}`. 41/36/30/20 distinct callers — this is
THE gameplay collision API.

### 2. Static world: cells + convex n-gons, stored VERBATIM in the id 0x44 file

`func_001A0B10` walks a **cell list** pointed to by SPR `0x70003250`:
`u32 count` + count u32 entries (top bits 0xC0000000 = runtime
active/stream flags — on disc 0xC0 on the first entry, cleared/set in
place at runtime; low 30 bits = offset from list base; 0 = empty).
Cell = 6-float world AABB + `s16 prim_count` @+0x18 + prims @+0x1C.
Prim header u16: type = hdr & 0xF000, wide = hdr & 0x800, count byte
@+2. Type 0x1000 (`func_001A4030`) = **convex planar n-gon**: plane
normal+d @+4, N vec3f verts @+0x14, then edge data (narrow: edge
normals, 0x14+0x18N total; wide: edge vectors + edge normals,
0x24+0x30N). Types 0x2000 (0x1C B, `func_001A50A0`) / 0x4000
(0x18/0x2C B, `func_001A5C30`) are small prims, not yet decoded; type
0x8000 (0x14/0x24 B) is skipped here — it belongs to the s16-grid path.

**RAM proof (save state 03, chunk10.n0 resident):** the per-level
world-section directory at bss `0x28A598` holds 5 pointers; [0],[2],[4]
point INTO the resident `f02_id44.bin` image (file offsets 0x138000,
0x164000, 0x15A800), [1],[3] into `f10_id7a.bin`. [4] is the cell list:
51 cells, **byte-identical to the disc file except single-bit
0x40000000 entry-flag fixups** (1 of 51 words differed). A frozen
in-progress query sat in scratchpad: vertical probe (-255.75, 110→310,
-715.23) hit y=120.000 with `0x700031D0` pointing at file offset
0x14FDDC inside section [0] — s16 rows (433,432,432,432,431,432 ≈
120×3.6) → the **s16-quantized grid section produced a live floor hit**
(kind 4 = `func_0019D330`, which calls `func_0019F1A0` = per-axis
binary search over sorted s16 node tables (32 s16/node) + a stride-12
float boundary array, then `func_0019ED80` for the surface test).

### 3. Geometry: id 0x44 n-gons are SIMPLIFIED, textureless, and coincide with the render mesh

`tools/collision_probe.py compare extract/chunk06.n1 --at 107.4,-184`
(office room): cell list @0x7E800 (18 cells), 84 n-gons / 332 verts,
bbox X[-3.6,120.5] Y[0,17.8] Z[-296,2.4] — fully inside the render
mesh's X[-25,120] Y[-10,50] Z[-305,50]; 43% of collision verts lie
within 2u of a render vertex (median 0.77u); ~84 polys vs ~10k render
records (two orders of magnitude simpler); records carry plane + edge
math and NO uv/TEX0/color — pure collision, not drawable geometry. The
floor under the character is owned by the s16-grid section (the cell
n-gons there are stairs/ramps/prop hulls; cells 13–17 are repeated
small template boxes near the origin). Same decode works on chunk10.n0
(247 n-gons), chunk15 (84), chunk04.n0 (304, via loose-signature scan —
its cell-list header form differs).

### 4. Movable-object hulls (doors etc.)

Objects registered in the gp table `D_00275B8C` (ptr) / `D_00275B94`
(s16 count) carry a polygon list at +0x58: `u32 count` (s16 @+0xA ==
-2 → skip first record), then variable records: 3 layer bytes (masked
against obj +0x5C/5D/5E) + matrix-index byte, u16 size = 0x18+24N,
s16 N (<0 = disabled), plane normal @+8, plane d @+0x14, N vec3f verts
@+0x18, N vec3f edge normals after. Matrix index selects
`*(obj+0x110+idx*4)+0x90` (4x4) — sub-objects move/rotate. Walked by
`func_001A6440(mask)` / `func_001A6AD0`: world-transform plane & verts,
ray-plane t, per-axis interval check, then point-vs-edge-normal inside
test (eps 1e-5); mask bit 0x40 enforces front-facing only
(dot(n,dir) ≤ -1e-5). Verified live: 5-poly convex hull (a 11×2.5×3
prism) shared by two door-like objects at (-134, 61, -769), layout
checks 5/5.

### Port-facing summary

Collision query API for the native port:
`int query_segment(vec3 from, vec3 to, u8 set_mask, u16 id)` →
0 = no hit / set bit of nearest hit; outputs: hit point, hit normal,
surface attr byte, poly id, hit object. Plus
`int actor_move_probe(Actor*, vec3 target, u32 mask)` with bit31 =
apply slide correction. Sets: movable hulls (object list), static
n-gon cells, s16 heightfield grid. Tool: `tools/collision_probe.py`
(decode / compare / ram). Open: exact s16-grid decode (scale ≈ 1/3.6?,
node tables), prim types 0x2000/0x4000, section-directory loader
(who fills 0x28A598), and section [2] (header 0x376/0xE00/0x4E80) role.

_Last updated: 2026-06-09 (session 7) — verdict: **id 0x44 = collision world, CONFIRMED**._
_(s8 amendment: the file's TAIL additionally carries a static render-mesh
section — see "LEVEL RENDER MESH v2"; the collision verdict for the
cell/grid sections stands.)_

## CHUNK27 MODEL LIBRARY + LIVE PLACEMENT DECODE — it's the EQUIPMENT library; placement algebra solved (2026-06-09, session 7b)

Live PCSX2 session in the office scene (player idle at (105.9, 0, -184)).
Goal was "place the chunk27 props in the port"; the decode succeeded and
**corrected the premise**: `chunk27/f01_id37.bin` is the **player
equipment / pickup model library**, not room furniture. The office desk
items (books, papers, binders) are baked into the level render mesh
(`f03_id43`) and already render in the port. Result in the port: the
soldier now holds his **rifle** (a six-model composite) with his **knife**
on the hip — `tools/export_props.py` → `assets/scene/01_props.emdl`.

### Library file format (f01_id37.bin, 126 models)

- `+0x00` u32 model count (0x7E); `+0x04` u32 offset[count] (byte offsets,
  0x80-aligned, terminator 0xFFFFFFFF after the last entry).
- Each offset → a standalone **raw mesh blob in exactly the chunk28
  character format**: header `{n_blocks, total_qwc, n_nodes, size}`,
  first `STCYCL(4,4)+UNPACK V4-32` at +0x48, then 130-qw VIF blocks of
  32 × 64-byte records `[TEX0][ST][normal][pos+W]` (record order of the
  character/object meshes, NOT the level order).
  `export_native._walk_blob_blocks` parses all 126 unchanged.
- Most models are `n_nodes=1`, vertices select dmem matrix slot 0
  (`wbits & 0x3FF == 0`).

### Runtime residency + the loaded-asset slot table

- Library resident at EE `0x00BAA1C0` (whole file, byte-identical).
- The pointer lives in a **loaded-asset slot table around `0x0028A4A0..0x0028A5B0`**:
  u32 pointers to every resident asset base (chunk blobs at
  0x012Axxxx.., the library at +0x16C entry `0x0028A56C`, the level
  region pointers 0x00D191C0/0x00D1B9C0/0x00D1C1C0, …). The **player
  character mesh blob** (21 nodes) is resident at `0x00D1C1C0` and is
  REF'd 6–7×/frame as a whole 0x4BAA-qw stream.

### The per-model DRAW UNIT (packet arena, live frame at 0x2994xx..0x2A1xxx)

One unit per placed model, built fresh each frame:

```
CNT 9qw   VIF NOP+FLUSH, STCYCL(1,1), UNPACK V4-32 8qw -> VU1 dmem 0:
          qw 0..3 = MVP matrix (columns = axis images), qw 4..7 = light rows
          (tag words 2-3 hold two per-unit floats — sort keys, TTE not set)
REF 9qw  @0x00815360   shared GS/viewport env
REF 8qw  @0x00816440   shared
REF 1qw  @0x00814220   shared
CALL     @0x0023C750   kernel-kick packet (MSCAL of the 0x0023C780 kernel)
REF Nqw  @ lib_base + offset[model] + 0x40    the model's VIF block stream
CNT 5qw   FLUSH + UNPACK 4qw -> dmem 0x3F5: GS constants (58.0/47.0 rows,
          0x4B00xxxx xyz-offset row)
```

Multi-node batches use a bigger palette CNT (e.g. 105 qw = 13 sets of
8 qw to dmem 0) and the vertices' `wbits & 0x3FF` select `8*slot`. The
**level's movable sub-objects** (doors etc., streamed from the level
region at 0x0155D900) are drawn exactly this way; four of the 13 live
sets were byte-identical — see anchor below.

### Placement recovery (the algebra that worked)

- The unit matrix is `M = K · W` (K = shared camera/clip transform of the
  0x23C750 kernel; qwords are matrix COLUMNS; `clip = Σ col_i · obj_i`).
- The LEVEL render mesh streams world-space geometry through its own
  kernel (CALL @0x00237180) with upload `K_L` at the level chain
  (0x297410 live). Live byte-identities: `K.col0 = -K_L.col2`,
  `K.col1 = K_L.col1`, `K.col2 = K_L.col0` — same camera, permuted axes.
- **`W = K_L⁻¹ · M` gives the ABSOLUTE world placement.** Validation on
  every live unit: bottom row = (0,0,0,1) exactly, R orthonormal
  (|scale-1| < 0.002), translations inside the room at the player.
- Live office placements: models **47/48/49/50/56/64 = the rifle**
  (six parts, one shared transform, in the hands at
  (104.57, 12.93, -183.36), tilted idle hold); **model 106 = the knife**
  on the hip at (106.11, 9.18, -181.97) (axis-mirrored matrix);
  models **20/21 = glow billboards** (non-rigid: scales (1.6, 4, 1.6) and
  a rank-2 variant — skipped in the export). The 13-set sub-object
  palette's identity-rotation set decodes to a door-like part at
  (75, 0, -188.2) with yaw -90°.
- Cross-check vs the GS dump: the office dump's 36 TEX0 keys not
  attributable to level or character are covered 30/30 by exactly these
  equipment models (+4 PSMCT32 UI/backdrop keys + 2 strays) — the dump
  frame, like the live frame, draws **no chunk27 room furniture**.
  Earlier "37 library models present in the dump" was TEX0-key aliasing
  (shared textures between library models).

### Tool + port

`tools/export_props.py`: library decode + embedded live placement table
(`--placements` JSON to override) → world-baked static EMDL v2
(identity palette, flags bit 0, normals → lit baked colors, textures via
the GS-dump VRAM machinery; all 29 used pairings resolve). Output
`assets/scene/01_props.emdl`; EM_CAPTURE shows the rifle in the
character's hands and the knife on the hip in the office.

Honest limits: the rifle/knife are baked STATIC at the captured idle
pose (the real engine recomputes per-frame hand-bone transforms —
proper attachment needs the bone-publish chain); the billboard glows
are skipped; pickups elsewhere in the level (models 20/21's parents,
weapon pickups) appear only when their rooms are active, so their
placements are recoverable the same way once visited live.

## LEVEL RENDER MESH v2 — record phase fixed, id44 render section, live placements, weapon attachment (2026-06-09, session 8)

Live DMA-chain decode of the office frame (PCSX2 DebugServer, paused at
the vsync wait so the whole frame's packet arena is built). Four results,
two of them corrections to sessions 6/7b.

### 1. CORRECTION — the level record row order is the CHARACTER order

The session-6 reading ([pos][TEX0][ST][color]) was an off-by-one-row
artifact: `f03_id43.bin` simply *begins mid-record* (a stray pos row at
offset 0; the first full record starts at +0x10). The resident stream
(REF'd blocks at runtime) shows the true order — **[TEX0][ST][color/
normal][pos+W]**, identical to the character/object meshes. The old
walker paired every position with the NEXT record's TEX0/UV/color, which
smeared textures and mangled strip assembly in the port. Position-W flag
bits (verified against the level VU1 kernel's PACKED XYZ2 output layout —
the W word lands in output word 3, so file bit 15 == GS ADC bit 111):

- bit 15 = ADC, verbatim (SET = prime/padding, CLEAR = kick a triangle)
- bit 14 = strip parity / winding (mirrored in the float's sign bit)
- bit 13 = per-vertex flag, varies per strip, NOT topology (undecoded)
- bits 0..9 = dmem matrix-slot address (slot = bits >> 3)

Triangle assembly is plain GS tristrip semantics per VIF block: every
record enters the strip queue, kicks emit, **TEX0 may change mid-strip**
(the triangle belongs to its kick vertex's TEX0) — do NOT break strips on
TEX0 changes or ADC runs. ~10% of the room's triangles were lost to those
two bogus break rules; the rest of the old export's damage was the phase.

### 2. CORRECTION — the id 0x44 file ALSO holds a render-mesh section

`chunk06.n1/f02_id44.bin` is the collision world (s7, still true for its
cell/grid sections) **but its tail [0x86B40, 0x173800) is a second static
render-mesh section** — 448 VIF blocks, same record format, slot 0, world
space, X[37,120] (the WESTERN part of the area incl. the office's x=100
wall). The live static level draw REFs blocks from BOTH files; runtime
also keeps color-modified copies of some blocks (dynamic lighting) in a
separate arena (the s6 "id44 markers are not TEX0s" check failed only
because of the record-phase bug). The office room alone: f03 contributes
the east part (x 100..120), f02's render section the west.

### 3. Level region map + live placements (chunk06.n1, office frame)

`f03_id43.bin` is NOT all world-space static. Live frame structure (DMA
chain, CALL targets are the kernel-kick packets):

- `[0x000000,0x0820C8)` static world, slot 0 — drawn via the LEVEL kernel
  (CALL 0x237180) with camera K_L (one 8-qw set to dmem 0).
- `[0x088840,0x099C80)` a 13-slot SUB-OBJECT assembly (one REF 4420 qw),
  drawn via the OBJECT kernel (CALL 0x23C750) with a 105-qw CNT = 13
  matrix sets to dmem 0; records pick sets by their slot bits. This is
  the corridor door at (75, 0, -188.2). W = K_L^-1 * M per set
  (orthonormal, bottom row 0001 — same algebra as s7b).
- Standalone object blobs drawn via the object kernel with per-unit
  1-2-set palettes: 0xA05C0 (520 qw, 2 slots, double door at
  (109,0,-252)/(101,9,-252)), 0xA3040 and 0xA3940 (0x82 qw each, drawn
  TWICE each = two instances, wall fixtures x~116 z~-264..-290), 0xA8440
  (1040 qw, lockers at (116.2, 8, -184)).
- Remaining object-space regions ([0x820C8,0x88840) non-record data,
  [0x99C80,0xA05C0) a second 3-slot door assembly, [0xA41C0,0xA8440),
  [0xAC540,end)) are NOT drawn in this frame — other door/object states;
  placements recoverable the same way when live.

The static draw also re-kicks the sub-object region after CALL 0x2354A0
(level-kernel re-upload packet; alpha env REF 0x816540) — a second pass,
same palette. `tools/export_level.py` embeds the region map + matrices.

### 4. Weapon attachment — equipment draw matrix == bone matrix

The player skin draw unit (REF 19370 qw @ the resident player blob) has a
21-set palette = the player's 21 node matrices (M-form). The held
equipment units' matrices are **byte-identical** to specific bone sets:

- models 47, 48, 49, 50, 56, 64 (rifle parts) == bone 4 (right hand)
- model 48 drawn a second time == bone 14 (left hand)
- model 106 (knife) is NOT attached this frame — it is a floor pickup at
  world (115.0, 1.5, -269.3)

So equipment parenting is identity-offset node attachment: merging the
library model vertices into the player EMDL bound to nodes 4/14
(positions kept model-local = bone-local) animates them with the hands.
`tools/export_props.py --attach` does exactly that; the no-flag mode now
exports placed world pickups (model 106) instead of baking held weapons
at a world pose (the s7b approach, now superseded).

### Port side

`assets/scene/00_level.emdl` 5743 verts / 3800 tris / 92 textures (was
4002/2041/85 with the phase bug: missing west half, smeared UVs);
`assets/player.emdl` 3363 verts / 3686 tris (rifle + offhand item
attached, +516 tris); `assets/scene/01_props.emdl` = the knife pickup.
Metal backend: texture alpha now honored — alpha-test discard (<0.5) for
cutout texels (8 of 92 level textures carry alpha: grates, glass) +
standard alpha blending; winding is normalized (kick vertex first =
provoking vertex, bit-14 parity decides the flip) but the port still
draws double-sided until GS cull state is translated.

Open: bit 13 meaning; the un-drawn object regions' placements; dynamic
block-color (lit-copy) path; per-room residency layout of the two mesh
files (the engine loads them as multiple separately-based pieces).

## LEVEL OBJECTS COMPLETE + ATTACHMENT CORRECTION — placement table found, node 14 = knife holster (2026-06-10, session 9)

Live PCSX2 session (DebugServer; player teleported around chunk06.n1 by
writing the actor pos qwords at 0x810350/0x810360; camera steered by
writing the camera block eye/target at 0x8101F0/0x810200). Frames decoded
by scanning the per-frame packet arenas (two ~850KB regions around
0x28F000.. and 0x480000..) for matrix-upload CNT tags + their REF/CALL
tails, then W = K_L^-1 * M as in s7b/s8. Three s8 results corrected, the
remaining office/west-room objects recovered, and the engine's object
PLACEMENT TABLE located.

### 1. CORRECTION — node 14 is the hip-HOLSTER node; the knife rides it

In every s9 frame (three player positions, walking and idle) the
equipment draw units are:

- models 47/48/49/50/56/64 (rifle) -> bone 4, RAW BYTE-EXACT against the
  player skin unit's 21-set palette;
- model 106 (knife) -> bone 14, RAW BYTE-EXACT, every frame — the knife
  is always drawn HOLSTERED at the hip while the rifle is held.

s8's "model 48 drawn a second time -> node 14 (left hand)" was wrong on
both counts: the occasional second REF of model 48 is a second PASS of
the same draw unit (same matrix, LVL2-style re-kick), and node 14 is the
holster. Binding 48 to node 14 produced the port's phantom barrel
floating by the leg. Beware when matching: a frame contains up to THREE
21-set player-blob units — only the primary skin unit's palette matches
equipment; the others (shadow/secondary passes) have different matrices.

### 2. The live object PLACEMENT TABLE (EE 0x828170.., 0x28-byte entries)

Searching EE RAM for the recovered W translations finds the engine's
placement table for the loaded area: entries
`{u32 type_word, f32 x, y, z, 0, f32 yaw, 0, ptr class/blob, u32 C, u32 D}`.
type_word & 0xFFFF: 4 = placed object/door (ptr -> per-object record),
0xB = item pickup (all share class ptr 0x1C4820; the model varies by
item), 5/8/0xD/0x52 = other actor/trigger kinds. The five SE-room type-0xB
entries are exactly the s8 "wall fixtures + knife pickup" draws — they
are ITEM PICKUPS (A3040/A3940 ammo-box blobs at (116.3,9.4,-266.6) /
(116.6,1.5,-264.2) / (115.6,9.4,-280.1) / (116.3,1.5,-289.9), knife
model 106 at (115.0,1.5,-269.3)). Doors and stations are type 4. This
table is the authoritative source for object placements — no camera
hacking needed once an area is loaded.

Object draws are selected per frame (nearest door instances win: the
A05C0 double-door blob draws at the (109,-252.2) OR the (57,-220.5)
doorway depending on player position), so single-frame captures
under-count instances; the table has them all.

### 3. Region map completed (chunk06.n1/f03_id43.bin)

New live-recovered placements (all orthonormal, bottom row 0001):

- `[0x9A140,0x9A960)` 3-slot door CONTROL PANEL — all three slots at the
  corridor door (75, 0, -188.2); drawn in a late-pass call packet
  together with 2-tri glow quads (library models 110/111/112).
- `[0x9AAC0,0xA0420)` 3-slot wall STATION (ammo/refill unit) at
  (57.5, 15, -292.6) / (60.0, 14.9, -296.0) / (60.8, 15.35, -290.0).
- A05C0 double door, SECOND instance (west doorway):
  (57.0, 0, -220.5) / (57.25, 9, -228.19) — this was the port's
  "doorway renders as a bare hole".
- `[0xA2740,0xA2F60)` supply crate, TWO instances:
  (75.7, 0, -302.0) yaw 0.28 and (82.5, 0, -302.4) yaw -0.04.
- `[0xA4240,0xA8340)` table-top DEVICE ("orange battery bank") at
  (80.1, 8.2, -244.0) — sits on the long table in the west room.

All embedded in `tools/export_level.py` (level now 6451 verts / 4205
tris / 115 textures). Glow/billboard library models (20/21/110-118,
2-tri overlays or non-rigid) remain unexported — noted honestly; they
need a billboard path in the port. The only remaining un-drawn f03
content is the non-record region [0x820C8,0x88840) and the 0x2C0-byte
tail at [0xAC540,end).

### 4. Misc engine notes (s9)

- The frame's draw lists live in 4 bump arenas; the double-buffered
  chain heads are the tag pair at 0x4835C0/0x4835E0 (NEXT -> 0x488C80 /
  0x28F740). The geometry units are easiest to recover by scanning the
  arenas for `CNT qwc=8n+1` whose payload starts with UNPACK V4-32 ->
  dmem 0, then collecting the REF/CALL tail (model stream REFs come
  AFTER the kernel CALL).
- Static world streams from BOTH n1 files AND the neighbour zone files
  (chunk06.n0/.n2) once their geometry is resident — REF'd blocks match
  those files byte-for-byte (no runtime color modification observed in
  s9 frames; the s8 "lit copy" note stays open).
- A 21-set unit REFing chunk-blob 0x12D1A00 draws SCALED TO ZERO at
  (114.7, 17.9, -206.1) — a hidden/pending actor (office ceiling),
  invisible by construction.
- Camera block 0x8101D0: +0x20 eye, +0x30 target, +0x44 yaw (the actor
  struct's +0x218 float mirrors it). Writing eye/target works for a
  frame but draw-list selection is position-, not camera-, driven.

## CAMERA SYSTEM — isolated and characterized (2026-06-10, session 10)

Static .s analysis of the local splat tree, cross-checked against EE-RAM +
scratchpad from save states 01 (area 0x1100) and 03 (snow, area 0x0600) via
`tools/camera_probe.py`. This closes the "Camera: NOT yet isolated" entry in
SUBSYSTEMS.md. The camera TU lives at **0x18B9C0–0x199xxx** — inside the
cluster mislabeled `init_io` (everything below 0x18B9C0 there really is
boot/IOP; 0x18B9C0+ is the camera system).

### Corrections to earlier sections

- `func_001C1D00(0x008101D0)` is NOT "camera apply": it is a **once-per-area
  render-env init** (byte 0x008101D0+0 is a once-flag; area 0x1500 gets
  special GS regs via func_001E2260, then func_001E0CF0/func_001D5370).
  0x008101D0 itself is just that 0x10-byte flag block; the camera struct
  proper starts at 0x008101E0 (the s9 live note "0x8101D0: +0x20 eye,
  +0x30 target" is the same memory, offset by the 0x10 header).
- `func_001C1DC0` ("camera/matrix update" in the frame anatomy) is the
  per-frame **render-env updater**, not camera math: render-chain channel
  enables (func_001D2830 over channels 0..2, 0x20..0x25), camera-object
  publish (func_001C1E70 → func_001D52E0: looks up object 0 of the table at
  `*D_0028A5A0` via func_001C6120, copies +0x08..0x1C / ptr +0x20 into
  render-ctx +0x150../+0x140), per-area **fog** (func_001C1E80 →
  func_001D8FD0), weather-particle selection by func_001B0070 flag bits
  (func_001C1EA0 → D_00250F00/F10/F20 via func_001EFD20), and per-area
  channel masks (func_001C1F50).
- `func_0018B9C0` is the **camera state machine top**; its a0 is
  `*D_00275B44` = the context base set by the preceding
  `func_001CB590(0x008101E0, 0xD0, 0)` — NOT 0x008102B0.
  `func_0018D7B0`/`func_0018C0D0` are camera functions, not "HUD/weapon
  context". `func_00195130` (SUBSYSTEMS "area init outlier") is the
  **mode-0 area-camera director**. `func_0022EEF0` (labeled sdk_gs) is the
  **scope/sniper camera** (variable zoom = 224.0/x via func_001D25F0).

### 1. The camera struct: 0x008101E0, size 0xD0

Live-verified fields (states 01/03):

| Offset | Type | Meaning |
|---|---|---|
| +0x00 | u8 | state: 0 = init (one-shot setup in func_0018B9C0), 1 = run |
| +0x01 | u8 | sub-state (0→1 ramp on first run frame; zeroes timer +0x08) |
| +0x04 | u8 | top mode: 0 = normal play, 1/2 = frozen (commit only), 3 = scope/sniper (func_0022EEF0) |
| +0x05 | u8 | dispatch table select: 0 = "cut" jtbl_0026D950, 1 = "smooth" jtbl_0026D910 |
| +0x06 | u8 | **camera MODE 0..15** (≥16 → func_00195130 too); live = 0 |
| +0x07 | u8 | follow-solver result/hit byte (set by func_0018D7B0) |
| +0x08 | u16 | mode timer (zeroed on entry) |
| +0x0C | f32 | per-area param, init-copied from +0x64 (−46.8 live, both states) |
| +0x10 | vec4 | **desired EYE** (world; live (251.3, 248.9, 170.3) ≈ 33 u from player, 19 u above ground) |
| +0x20 | vec4 | **desired TARGET** (live = player x/z exactly, y = ground + 17) |
| +0x30 | vec4 | vertical offset accumulator (0, y, 0, 1) |
| +0x44 | f32 | yaw eye→target (func_001B1240 heading; tracks +0x9C) |
| +0x50,+0x54 | f32 | mode workspace (init −200.0 / 1000.0) |
| +0x5C | f32 | 2.0 (init constant) |
| +0x60,+0x64 | f32 | per-area params (440.0 / −46.8 live) |
| +0x8B | u8 | flag byte OR'd each frame from spad 0x700031F0 (player collision result) |
| +0x8C | f32 | target height offset above player Y, default 6.0 (aim mode, player state 5) |
| +0x9C | f32 | yaw copy written by commit func_0018C0D0 |
| +0xB0 | vec4 | normalized forward (copy of D_00810600) |

**Global camera vector pool 0x008105D0..0x008106A0** (the real per-frame
camera output, smoothed from the struct):

- `0x008105D0` vec4 **actual EYE** (w=1) — chased toward struct+0x10
- `0x008105E0` vec4 **actual TARGET** (w=1) — copy of struct+0x20 (func_0018C0C0)
- `0x008105F0` vec4 **UP vector = (0,−1,0,1)** — Y-DOWN view convention, set
  once at camera init (this is what func_0018B9C0 writes there; the frame
  anatomy's "view-target update writing vectors at 0x008105F0" was loose)
- `0x00810600` vec4 normalized forward; `0x00810610` **view (look-at)
  matrix**; `0x00810650` its **transpose** (func_00102798)
- scalars: `0x810690` desired horiz eye↔target dist, `0x810694` |Δy|,
  `0x810698` eye height above `D_00810354` (player ground Y), `0x81069C`
  actual horiz dist, `0x8106A0` atan2 angle of forward

### 2. Update rule — clamped proportional chase + collision-aware solver

Once per gameplay frame (inside func_001AE5E0, AFTER the render-chain build —
the renderer always consumes the PREVIOUS frame's matrices):

```
func_0018B9C0(cam = *D_00275B44 = 0x008101E0)
  state 0: init (vectors, up=(0,-1,0), mode=8 unless area 0x12 chunk 0,
           settle vs world: func_0018CE60 -> 2x func_0019A910 ray queries)
  state 1, top-mode 0:
    func_00191390(cam, player)       leaf pre-step
    func_0018BC20(cam, player)       MODE DISPATCH (table below)
    func_0018C0D0(cam, 1)            commit -> matrices
  top-mode 3: func_0022EEF0 (scope cam) ; top-modes 1/2: commit only
```

`func_0018BC20` dispatches byte+6 over **jtbl_0026D950** (cut, byte+5==0) /
**jtbl_0026D910** (smooth, byte+5==1), 16 entries (dumped from save-state RAM):

| Mode | Handler (cut table) | Notes |
|---|---|---|
| 0, ≥16 | **func_00195130** (4720 B) | DEFAULT: per-area camera director; 14× func_0018C4B0 + 10× func_0018C6A0 lerps, 12× func_0018D7B0, and a hardcoded `jal 0x823FE0` **overlay hook** — per-room camera logic lives in the area overlays (survival-horror room cameras) |
| 1 | func_00197D20 | |
| 2 | func_00198650 | |
| 3 | func_001936E0 | |
| 4, 7 | (nop) | |
| 5 | func_0018CA90 → sets mode 7 | one-shot reposition |
| 6 | wait `D_0028A9A0`==0 → mode 0 | timed hold |
| 8 | func_001914A0 + func_001DD980(eye, tgt) | init/fallback mode |
| 9 | func_00198CE0 | |
| 10 | func_00198D90 + enable chain ch3 | |
| 11 | func_00198F10 | |
| 12 | func_001963A0 | |
| 13 | func_00196CE0 | |
| 14 | func_00198AF0 | |
| 15 | func_00197390 | |

The smooth table maps to the same handlers; its default modes (0/9/11/14)
run an inline **player-relative follow**: chase target x/z toward player
+0xA0/+0xB0 (rate cap 0.8/frame), target y toward player.y + 15.0 (or
+cam[0x8C] = 6.0 in player state 5/aim), keyed on player state word +0x230.

**The two lerp primitives** (used everywhere, incl. by mode handlers):

- `func_0018C6A0(src, dst, max)` — horizontal chase, per axis (x and z):
  d = src−dst; if |d| ≤ 1.0: dst += d/4; else dst += sign(d)·min(|d|/6, max).
- `func_0018C4B0(vec, target_y, max)` — same for vec[1] (y), divisor 8.

So: **speed-limited proportional follow** (≈ exponential at rate 1/6 horiz,
1/8 vert per frame, hard cap, quarter-step snap inside 1.0 unit). No splines.

**Collision-aware: yes.** `func_0018D7B0(cam, style)` — the desired-eye
solver — picks collision-set mask **6** (static cell world + heightfield) or
**7** (+ movable hulls) and runs func_0018D330, then by style func_0018DD20
(6984 B full solver) / func_0018D910 / func_0018F870; all of these call
**func_0019A910**, a level_world segment-query hub (same TU family as the
documented func_0019A570; dispatches to func_0019D770 / func_001A1390 /
func_001A6AD0 = heightfield / cell-world / movable-hull walkers). Style 0
then smooth-chases global eye D_008105D0 toward desired cam+0x10 (max
4.0/frame); style 1 hard-copies desired→actual. Result byte → cam+0x07.

### 3. Projection path — commit to VU1 dmem 0

`func_0018C0D0(cam, flag)` (the commit, every frame):

1. forward = normalize(target − eye) (VU0: func_001028D0 sub, func_00102760
   normalize); horizontal length < 1e-3 clamped (degenerate guard).
2. view position = **eye + 4.0·forward** (near push, spad 0x700038C0;
   mode 0xA uses −1.0·forward; frozen modes copy eye as-is).
3. `func_00102CD0(D_00810610, pos, forward, up=D_008105F0)` — **look-at
   builder** (sceVu0CameraMatrix-style; memory rows are the operator's
   COLUMNS: clip = Σ col_i·v_i, translation row = −R·pos; verified
   numerically in both states). func_00102798 → transpose at D_00810650.
4. yaw bookkeeping → cam+0x9C, D_008106A0; forward → D_00810600, cam+0xB0.

`func_001D1C50` (render-chain head) passes D_00810610 to **func_001D2960**,
which builds in the per-frame render ctx (`*D_00275670`, live 0x811CC0):

- `ctx+0x2340` **projection P** from zoom s = ctx+0x2468, stored rows:
  (0.8s, 0, 0, 0), (0, 0.5s, 0, 0), (2048, 2048, 0.8996, 1),
  (0, 0, 1677721.5, 0). Default s = 480 (set via func_001D25F0; every
  static caller passes 0x43F00000; the scope camera and func_001D2590
  animate it): screen x = 384·x/z + 2048, y = 240·y/z + 2048 (GS center
  2048), w_clip = z_view, z = 0.8996·z + 1677721.5 (24-bit GS Z).
- `ctx+0x2380` = view copy; `ctx+0x23C0` = **K = P·V** — verified == P·V to
  1e-8 in both save states. This K is exactly the "MVP upload, CNT 9qw →
  VU1 dmem 0" of the draw-unit anatomy (per unit M = K·W).
- variants: ctx+0x2240/0x2280/0x22C0/0x2300 = altP·V (func_001D2D20 with
  explicit screen/near params — 1280×560, near 20, 3584², 2048²: offscreen/
  shadow/reflection passes); ctx+0x2220 guard-band clip constants;
  ctx+0x2410/20/30 frustum cull planes (normal = (±s, 0, −1023)/√(s²+1023²)).
- the LEVEL kernel's axis-permuted camera K_L is built separately by
  func_001D5370/func_001D5C80 (func_001D2D20(zoom, 4096, 4096, 0.1,
  16711680.0) × D_00810610 → spad 0x70003440) — explains the s7
  K↔K_L permutation identity.
- **Fog** (not projection): func_001D8FD0 reads the per-area 0x78-byte
  record from table `D_00251C50` (45 entries, keyed by area id
  D_00810700<<8|D_00810701; rec+4/+8 = fog near/far, rec+0xC/10/14 = RGB) →
  func_0021B970/func_0021BA80 → GS fog coefs A = 255·far/(far−near),
  B = −255/(far−near) at ctx+0xA0 (func_0021B920); func_001B0070 flag 0x80
  (night-vision) overrides with 0/110/black. Live: −209/304 (area 0x1100),
  −150/334 (area 0x0600).

### 4. Port contract — authentic camera natively

State: camera struct (0xD0, fields above) + globals eye/target/up
(D_008105D0/E0/F0) + zoom s (default 480) + per-area fog record.
At 60 Hz, in this order inside the gameplay frame:

1. (render) build V from LAST frame's eye/target: lookat(eye + 4·fwd, fwd,
   up = (0,−1,0)); P as above; K = P·V → every draw's matrix slot 0
   (M = K·W). For a native GL/Vulkan/Metal port remap GS screen → NDC:
   x_ndc = 0.8s·x/(z·half_w_gs) etc.; tan(half-hfov) = half_w_gs/(0.8s);
   screen Y is down; view +Z into the screen.
2. (logic) run the mode machine: handlers produce desired eye/target
   (cam+0x10/+0x20) — mode 0 delegates per-room logic to area overlay code;
   the generic follow chases target to player x/z (cap 0.8/frame) and
   y → player.y + 15 (or +6 aiming).
3. solve desired eye against the collision world (segment queries, set
   mask 6/7) — the id 0x44 file's cell/heightfield/hull sets.
4. smooth actual ← desired per axis: step = |Δ|≤1 ? Δ/4 :
   sign(Δ)·min(|Δ|/6 horiz | |Δ|/8 vert, cap 4.0); then commit (step 1
   inputs for next frame).
5. fog from the per-area table; zoom fixed 480 except scope (224/x) and
   scripted zoom lerps (func_001D2590).

Confidence: struct map, smoothing math, commit/look-at path, P/K
composition, fog — **high** (live-verified in both states; K = P·V to
1e-8). Mode-dispatch table — high; individual handler semantics — low
(only mode 0 traced). Collision-solver internals (func_0018DD20) — **high
as of s61: the full 6984-byte body is decoded — see "CAMERA WALL SOLVER
func_0018DD20 DECODED (s61)" below.** (superseded note kept for history:) medium
(mask + query hub verified; 6984 B body not read). Tool:
`tools/camera_probe.py`.

_Last updated: 2026-06-10 (session 10)._

## PLACEMENT TABLES ARE DISC DATA — OVERLAY/AREAxx.BIN record format + spawner decoded (2026-06-10, session 11)

Static analysis of the extracted `OVERLAY/` modules cross-checked against
EE RAM from save states 01 (area 0x1100) and 03 (snow, area 0x0600) via
`tools/parse_pcsx2_state.py`, plus the main-ELF spawner disassembly. This
closes s9's "table is the authoritative source" with the actual DISC
source: **no live capture is needed for object placements anymore.**
Tool: `tools/placements.py` (parser + scanner + JSON dump).

### 1. The disc source

The s9 "live placement table at EE 0x828170" is **static data inside the
area's overlay module**, loaded FLAT at the overlay arena base `0x823500`
(office table = `OVERLAY/AREA02.BIN` file offset 0x4C70 = vaddr 0x828170).
Live tables in both save states are **byte-identical** to the overlay
files — the engine never mutates them (mutable pickup/object state lives
in scratchpad words at `0x70003250`, see below).

Lookup chain (spawner `func_001B6990`, queried later by `func_0019C6F0`):

    desc  = *(u32*)(0x0024D7C0 + 4*D_00810700)   ; area number (code hi byte)
    table = ((u32**)desc)[D_00810701]            ; area sub-state (lo byte)

`D_0024D7C0` is a static per-area pointer array in main .data; each slot
points to a per-area DESCRIPTOR (in main .data for some areas, inside the
overlay for others) = an array of placement-table vaddrs indexed by story
sub-state (area code 0x0200/0x0201/0x0202 → desc[0..2]). Known tables:

- `AREA02.BIN` (the office level, chunk06.n1): desc @0x828C40 =
  `{0x827830, 0x828170, 0x8283D0}`. The s8/s9 captured office scene is
  **sub-state 1** (table 0x828170, 14 entries). States 0 and 2 are
  near-duplicate 58-entry lists for the area's other sub-states.
- `AREA06.BIN` (snow, save state 03): desc @0x275948 (main .data) =
  `{0x827AC0, 0x8283D0}`; state-0 table = 57 entries.
- `AREA11.BIN` (save state 01): table 0x82A3C0, 21 entries.

### 2. Record layout (0x28 bytes) — s9's read was shifted +8

Each table is a flat array of 0x28-byte records ending in a sentinel
record whose halfword +0x00 == 0x00FF. (s9 read the entries starting at
+0x08, so its "type word" was this record's `kind`.) Fields, with the
actor offsets `func_001B6990` copies them to:

    +0x00 u16 spawn_class  & 0xFF1F = class (4 generic actor, 5 double
                           door, 6 wall station, 8 door assembly, 2
                           link-special, ...); low-byte bits 5..7 = flags
                           (0x84/0x85/0x86 observed). 0xFF = END sentinel.
                           Low byte 0x0B = scripted/deferred spawn: the
                           area-load spawner SKIPS it (func_0019C6F0
                           queries those records on demand).
    +0x02 u8  model        object/model record selector → actor+0x03
                           (office: 0x36 control panel, 0x37 wall station,
                           0x2C battery device, 0x38 lockers, 0x03 door;
                           same model byte ↔ same behavior fn across areas)
    +0x03 u8  flags2       → actor+0x2E (door variants 0x81/0x02/...)
    +0x04 u16 param        → actor+0x0D; for kind-0xB pickups = ITEM TYPE
    +0x06 u16 uid          → actor+0x0E; high byte = per-area unique id =
                           index of the object's state word in scratchpad
                           0x70003250 (func_0019C6F0 tests bit 0x20000000,
                           sets/clears 0x40000000)
    +0x08 u16 kind         → actor+0x54 (s9's "type"): 4 = placed object/
                           door, 0xB = item pickup, 0xD/0xE = enemy
                           spawns, 0x46 fixture, 3/8/0xA/0x52 = others
    +0x0A u16 link         → actor+0x56; 0xFFFF none; doors: room/door
                           link id + flags (0x0200/0x0280 office doors)
    +0x0C f32 pos[3]       → actor+0xB0..B8 (world units)
    +0x18 f32 rot[3]       → actor+0xC0..C8, Euler radians, ry = yaw;
                           rx/rz almost always 0 (W = Ry·t verified
                           against every s8/s9 live matrix to ~1e-3)
    +0x24 u32 behavior     → actor+0x10: actor BEHAVIOR FUNCTION pointer
                           (main ELF or overlay): 0x1C4820 generic placed
                           prop/pickup state machine, 0x1BC350 door,
                           0x1551B0 enemy spawn, 0x159xxx per-fixture, ...

The s9 "class ptr" is therefore a function pointer, not a class record.
Multi-slot assemblies (13-slot corridor door, 3-slot panel/station,
2-slot double doors) have ONE record each: the table holds the BASE
placement (== live slot-0 matrix in every captured case); per-slot
articulation is runtime animation state. Validated: composing
`B·L0⁻¹·L_k` from the table base + office-door live slots reproduces the
WEST door's live slot matrices to 1e-2 — the articulation is door-local.

### 3. The office (115.0, 1.5, -269.3) "knife pickup" is an AMMO BOX

The office table's 7 pickups (kind 0xB, behavior 0x1C4820, model byte 0):

    item 0x0D, uids 2,3:   (115.6,9.4,-280.1) (116.3,1.5,-289.9)  → blob 0xA3940
    item 0x0C, uids 4,5,6: (116.3,9.4,-266.6) (116.6,1.5,-264.2)
                           (115.0,1.5,-269.3)                     → blob 0xA3040
    item 0x0B, uids 7,8:   (75.7,0,-302.0)    (82.5,0,-302.4)     → blob 0xA2740

The (115.0,1.5,-269.3) record is byte-identical in class/kind/item type
to the two ammo-box pickups beside it (item type 0x0C; differs only in
uid 6 and yaw 0.15) — it is a **third 0x0C ammo box**, drawn with the
same 0xA3040 blob. There is NO knife pickup in the disc data (matching
in-game observation); s9's "knife model 106 at (115.0,1.5,-269.3)" was a
frame-capture misattribution. Item-type → blob pairing (0x0B crate,
0x0C/0x0D ammo boxes) is consistent across both instances of each type.

### 4. export_level.py is now table-driven

`tools/export_level.py` builds the chunk06.n1 region placements from
`extract/OVERLAY/AREA02.BIN` (state-1 table) when present: single-matrix
instances (pickups, crates, battery, lockers) come straight from the
table (`Ry(yaw)+t`); multi-slot assemblies anchor the live-captured slot
sets on the table base (`B·L0⁻¹·L_k`). The embedded live matrices remain
as the no-overlay fallback. Office export: 6451→6475 verts, 4205→4217
tris (the third 0x0C ammo box), 115 textures; port-rendered OK
(EM_CAPTURE).

Open: rot[3] composition order when rx/rz ≠ 0 (none in the office table;
AREA11 has one rx=-0.314 entry); semantics of kinds 3/0xA/0xD/0xE/0x46/
0x52; the class-0x0B scripted-spawn records' trigger conditions; item-
type id → inventory-item name mapping (0x0B/0x0C/0x0D...).

## Pipeline generalizes beyond the player — 44-node creature exported (2026-06-10 s13)

First non-player character through the full EMDL pipeline: the
**chunk21/f17_id8f segment-1 44-node creature**, animated with its own
in-file id-0xd0 bank, rendered by the port next to the player
(evidence: `extermination-port/build/cap_enemy.png`; the temporary
`assets/scene/90_enemy.emdl` was removed after capture — default scene
unchanged; canonical export kept at `assets/enemy_test.emdl`).

**id-0xd0 clips use the standard map-A/B/C encoding.** All 30 in-file
0xd0 containers (0x64080..0xc4e30) decode with `parse_id74_prefix`
unchanged: every rot key is a unit quat to 4e-4 (the 11-bit-mantissa
truncation fingerprint). Composition (conjugate-quat locals, parent-table
world chaining) carries over verbatim.

**Hemisphere fix holds for 0xd0.** s7c-style scan over all 30 clips x 44
nodes x every baked frame: **0 single-frame world-rotation jumps > 90
deg**; worst step anywhere = 49.7 deg (clip 16 frame 58 node 41, a fast
snap). Mean joint step of the exported clip: 3.0 deg/frame.

**Clip survey (root XZ travel over the clip, 60 fps bake):** 9 clips are
in-place (< 0.05 u): container 0 @0x64080 (180 f, 0.003 u — the IDLE,
exported), 6, 11, 17, 18, 21 (286 f), 26, 19, 24. Locomotion clips
travel 3.1..89.9 u; clips 1/2/3 (@0x69b60/0x6d0e0/0x708b0) are exactly
54.0 u / 90 f each — a matched walk set. Clip 29 (@0xc4e30, 586 f,
5.3 u) is a long scripted/cinematic track.

**Exporter selectors (export_native.py).** `find_id74_headers` only
matches blob ids 0x74/0x2c, so in-file banks (0x70/0xd0/...) were
unreachable. New: `--rig-nodes N` switches container enumeration to the
id-agnostic whole-file scan (rig_probe.scan_anim_headers) filtered to
N-node parent tables — `--clip/--clips` then index only those; pick N =
segment max_slot + 1 (the encounter-package pairing rule). `--anim-hdr
OFFSET` pins one container by file offset (its offset becomes the EMDL
clip-table id). `--offset x,y,z` bakes a world translation into every
palette matrix (scene-file placement; the port poses scene EMDLs
verbatim at frame 0). Default (no selector) enumeration is unchanged —
player library indices like 346 keep meaning.

**Texture status: flat-lit (unresolvable from current captures).** The
creature's 61 TEX0 keys (CLD-masked qwords from the mesh) appear in NO
existing capture: 0/61 in the office GS dump's draw stream and 0/61 in
the EE RAM + VU1 dmem of all three save states — the chunk21 level was
never captured. The 30-node enemy (chunk12.n0/f13_id8b) is likewise
non-resident everywhere. Negative space mapped while checking: the
office dump's character keys are the player + equipment only. **Future
textured-NPC candidate: chunk15/f18_id94 segment 1 (21-node humanoid,
68 TEX0 keys) is 68/68 resident in save state 01** — a state-01-VRAM
texture source for that NPC needs only a .p2s-VRAM path in
build_texture_blob (clut_pair already reads it).

_Last updated: 2026-06-10 (session 13)._

## COLLISION "s16-GRID" SECTION FULLY DECODED — it is a rank-indexed polygon soup, NOT a heightfield (2026-06-10, session 14)

Static .s read of the remaining grid-path functions (`func_0019F1A0`,
`func_0019D330`, `func_0019ED80`, `func_00199C50`, `func_001A4030`) plus a
byte-level decode of `chunk06.n1/f02_id44.bin`. This closes the s7 open
item "exact s16-grid decode (scale ≈ 1/3.6?)" — **there is no quantized
height grid and no 1/3.6 scale.** The s16 values that looked like
quantized heights (433,432,… at the live result ptr) are *vertex indices*;
the apparent ×3.6 was coincidence. The section is a **convex-polygon soup
with full float precision** plus a sorted-rank acceleration index:

### Section layout (world-section directory [0]; office file @0x79000)

`func_00199C50` stages directory[0] (`D_0028A598`) into the scratchpad
query block and reveals the header (all offsets section-relative):

| Off | Field | SPR mirror |
|---|---|---|
| +0x00/+0x04 | vertex pool off (vec3f) / count | 0x700031FC |
| +0x08/+0x0C | edge-normal pool off (vec3f) / count | 0x70003200 |
| +0x10/+0x14 | vertex-index pool off (s16) / count | 0x70003204 |
| +0x18 | sorted rank tables off — **12** contiguous s16[N] (6 sort dirs + 6 range helpers) | 0x70003210/0x70003228 |
| +0x1C | form word (0xC = helper tables present) | — |
| +0x20/+0x24 | node array off / s16 node count N | 0x70003208/0x7000320C |

Node = **64 bytes**: +0x00 s16[6] boundary-vertex index per sort
direction; +0x0C s16[6] rank bounds (accel only); +0x18 u8 vert count;
+0x1A u8 **surface attr** (0x50..0x59 conditional vs query id 0x7000324E:
0x50 never, 0x51 id==0, 0x52 id==2, 0x53 not for id -1, 0x54+ always);
+0x1C u32 off into index pool; +0x20 u32 off into edge-normal pool;
+0x24 vec3f plane normal; +0x30 f32 plane d. The office section:
325 verts / 484 indices+edge normals / 121 nodes, all normals unit, all
ring verts on-plane, floor poly under the spawn solves to y = 0 exactly.

### Query semantics (shared by BOTH worlds)

`func_0019ED80` (grid node) and `func_001A4030` (cell n-gon) are the same
test: require dot(dir,n) ≤ −1e-5 (**front-facing only** — this is what
makes wall-sliding free), plane t, per-axis hit-in-interval, then convex
inside test dot(hit − v_k, edge_n_k) ≤ +1e-5; on accept the segment end is
clamped (nearest-hit semantics across sets). `func_001A4030` additionally
classifies the surface from ny²/(nx²+nz²) vs 0.49029/3.0 →
0x2000 wall / 0x1000 slope / 0x4000 floor / 0x800 down-slope / 0x8000
ceiling at SPR 0x700030CA, and stages the plane normal at 0x700030D4.
`func_0019F1A0` is a pure accel prune (per-direction binary search of the
segment-AABB ranks vs the node rank bounds). The camera-family hub
(`func_0019A910` → `func_0019D770`) reuses `func_0019ED80` — same facing
rule. **Wide cell n-gons (hdr bit 0x800) decoded:** plane@+4, verts@+0x14,
unnormalized in-plane edge normals (= cross(edge, n)) follow the verts,
then the entire record repeats (a second mirrored copy) — size
0x24 + 0x30·cnt accounted for.

### Tools / port

- `tools/export_collision.py` — bakes grid + cell worlds (narrow + wide
  n-gons) into **EMCL v1** (shared vert pool, per-poly plane/ring/edge
  normals/set bit/attr; format doc in the tool header). Office:
  205 polys (121 grid + 84 cell), validated floor y=0 at the spawn.
- Port `src/game/em_collision.[hc]` — EMCL loader + faithful
  `em_collision_segment_query` (func_0019A570 shape: mask bits, query id,
  conditional attrs, nearest-hit clamp, result block) and
  `em_collision_move_probe` (func_0019AD00: horizontal probe at target.y,
  0.01 extension, hit−target delta correction with mask bit31). Player
  movement + camera desired-eye solver (mask 6) now run these natively;
  EM_MOVE_TEST walks into the office z=−170 wall n-gon and asserts the
  stop (PASS), idle EM_CAPTURE byte-identical.

Still open: prim types 0x2000/0x4000 (small prims), type 0x8000 records,
section [2] role, who writes the 0x28A598 directory at level load, and the
movable-hull object set in the port (no native objects yet).

## First fully TEXTURED NPC — save-state VRAM as texture source (2026-06-10 s15)

The s13 candidate is delivered: **chunk15/f18_id94 segment 1 (21-node
humanoid) exported textured + animated** (`assets/npc_test.emdl`, 2789
verts / 3330 tris / 240-frame idle) and EM_CAPTURE-verified in the port
beside the player — a textured soldier NPC in camo fatigues, harness and
cap (`extermination-port/build/cap_npc.png`; the temporary
`assets/scene/90_npc.emdl` was removed after capture, default scene
re-verified byte-identical).

**.p2s VRAM is now a texture source.** `export_native.build_texture_blob`
accepts a save state (`--p2s`: a .p2s, a pre-extracted state dir, or a
bare gs.bin freeze blob) as the texel/CLUT source alongside the GS-dump
path: GS local memory = the 4 MB inside the GS freeze component at base
`len(gs.bin) − 0x400000 − 84` (gs_vram.read_localmem; the layout proven
2026-06-09). The PSMT4/PSMT8 + CLUT readers are clut_pair's, unchanged.
Residency is the caller's responsibility — the mesh's baked TEX0
TBP0/CBP are read as-is, so a state of the wrong level yields garbage.
Coverage here: **68/68 mesh TEX0 keys resolve from save state 01** (all
PSMT4; every 16-entry CLUT validates — alpha ≤ 0x80, ≥ 2 distinct RGB —
and every index plane is non-degenerate). `--attach` keeps requiring
`--gsdump` (that path uses export_level's PSMCT32-capable builder).

**The rig can live in a SIBLING file — and one file can hold several
same-node-count rigs.** f18_id94.bin itself contains NO animation
containers (scan_anim_headers: zero). The rig is in `chunk15/f12_id44.bin`
(the chunk's collision-id file is a multi-asset container here): 14
21-node + 18 20-node containers. The 14 21-node containers split into
TWO parent-table families — clips {0,2,3} vs {1,4,5,6,7,8,9,10,11,12,13}
— so the encounter-package rule (n == max_slot + 1 = 21) alone is
ambiguous. Disambiguation: pose the mesh at frame 0 of one clip per
family and score **cross-bone edge coherence** (mean posed length of the
388 mesh edges whose endpoints skin to different bones): matching family
0.99u, wrong family 3.62u. The matching rig's parents:
`[-1,0,0,2,2,4,4,3,3,3,8,9,5,6,6,10,11,12,13,15,16]`.

**Clip survey (all in-place — root XZ travel 0.000u on every container;
a scripted/cinematic NPC, fits the snow-level save state).** Matching
family: clip 5 near-frozen (7° max excursion), clips 6/7/8/9 gentle
gesturing loops (0.2–0.3°/frame, wrap ≤ 1.2°), 10/11 short actions
(60 f, ~1.3°/frame), 12/13 medium, 1/4 long scripted tracks (691/646 f;
clip 1 wraps badly at 125°). **Idle pick: clip 7** (240 f, 0.25° max
loop-wrap, 67.7° max excursion — visibly alive, loops cleanly).

**Discontinuity scan (s7c-style, all 14 21-node clips × 21 nodes × every
baked frame): 0 single-frame world-rotation jumps > 90°**; worst step
anywhere = 63.7° (clip 0 frame 14 node 17). Hemisphere fix holds for
this bank too. verify_all --no-container all-PASS.

_Last updated: 2026-06-10 (session 15)._

## Music cue table — SOLVED (2026-06-10 s15): how the engine selects MUSIC.DAT tracks

The engine's stream cue tables are pinned, validated against live RAM, and
wired into the exporter. The "55 tracks" of the silence-split decode were an
approximation; the game addresses **67 music cues** (and **178 voice cues**)
through fixed tables in the boot ELF.

### Engine code path (boot ELF, stream_music cluster 0x1FA)

- `sub_O_STREAM_MUSIC_DAT_1` (0x001FA6E0) — boot-time LBA resolve: calls
  `sceCdSearchFile` (`func_00111C28`) on `\STREAM\MUSIC.DAT;1`
  (string at `D_0026EBB0`) and `\STREAM\VOICE.DAT;1` (`D_0026EBD0`) and
  stores word 0 of the result (the file's first LSN) to **`D_00282188`**
  (music) / **`D_0028218C`** (voice). Retries in a spin loop until found.
- **`func_001FA790(channel, cueId)` — the cue starter.** channel 0 = music,
  nonzero = voice (channels 1/2 are used for dialogue via the 16-deep
  pending-voice ring `D_00281CF0` drained by `func_001FA5F0`). It indexes
  the cue table (16 bytes/entry, below), adds `start_sector` to the file's
  base LSN, and fills the per-channel streamer block (stride 0x60):
  `D_00281FFC[ch]` start = `D_00281FF4[ch]` current sector,
  `D_00281FF8[ch]` end sector (`start + (byte_len+0x7FF)>>11`),
  `D_00281FF0[ch]` ← entry flag, `D_00282178[ch]` ← cue id,
  `D_00282154[ch]` ← state 1 (2 = streaming, observed live), then kicks the
  first block read via `func_001FABF0`. It also computes a duration
  estimate: `60.0 * (byte_len>>11) * 0.0746514 / 2 - 30` frames for music
  (`/2` because the stream is stereo-interleaved; 0.0746514 s = one
  2048-byte sector = 128 VAG frames = 3584 samples @ 48 kHz — independent
  confirmation of both the 48 kHz rate and the 64-frame interleave).
- Public API above it: **`func_001FB0B0(cue)`** = write the current-BGM
  global **`D_00810D38`** + tail-call `func_001FAE70(1)` (fade-out then
  start); **`func_001FAE70(fade)`** (re)starts whatever `D_00810D38` holds.
  `func_001FAE70` also hosts a hardcoded override: when `D_008104E4 == 1`
  (and not area 0x15 / BGM 0xB/0xC/0x17), it plays **cue 0x18 (24)** at
  volume 0x40 instead — an "alert/event mode" music swap.
  `func_001FAFD0` only applies its logic when the playing cue is **< 29** —
  the engine itself treats cues 1..28 as BGM and 29+ as scripted streams.
- Script/event command path: `func_001B6D70` (7-way opcode dispatch via
  `jtbl_0026DF00`) — opcode arms call `func_001FAE70(cmd+0x18)`,
  `func_001FA790(0, cmd+0x18)`, `func_001FABB0` (stop), play_sound, etc.
  So cutscene scripts carry cue ids as data.

### Cue table layout (PINNED)

    MUSIC cue table  vram 0x0025DD30 (D_0025DD30), 68 entries x 16 bytes
    VOICE cue table  vram 0x0025E170 (D_0025E170), 179 entries x 16 bytes
    entry: +0x00 u32 start_sector   (relative to the file's first LSN)
           +0x04 u32 start_byte     (== start_sector * 2048, redundant)
           +0x08 u32 byte_len
           +0x0C u32 flag           (music: 1 on cues 3-16 and 20-26 = the
                                     looping in-level BGM set; 0 elsewhere)

  Entry 0 of each table is null; real cue ids are 1..67 (music) and
  1..178 (voice). The music table ends exactly where the voice table
  begins; the last voice cue ends exactly at VOICE.DAT's EOF. Cue spans
  tile each file completely (sector-aligned, no gaps beyond padding).

### Live validation (PCSX2 save state 01)

State 01 (area 11): `D_00282178[0]` = cue **25**, `D_00282154[0]` = 2
(streaming), start sector 720927 − MUSIC.DAT LSN 645393 = **75534 =
0x1270E**, exactly cue 25's `start_sector`; the end sector matches
`start + len>>11` (0x644 sectors). States 02/03 (areas 0/6) had no music
streaming (`D_00282154[0]` = 0) — consistent with what those scenes sound
like (silent exploration).

### Area/event → cue mapping

Two sources:

1. **`D_0026EC60`** — area/trigger → music cue table, 16-byte rows
   `{u32 area_id, u32 0, u32 trigger_id, u32 cue}`, terminated by
   `area_id == 0xFFFFFFFF` (36 rows). Scanned by `func_001FD4C0` against
   the current-area byte `D_00810700`; on match it stops the current
   stream and plays the row's cue. Covers cues 29–66 (the
   cutscene/scripted streams).
2. **Overlay call sites** (constants in per-area overlay code):
   `func_1FB0B0` BGM constants and direct `func_1FA790(0, cue)` stings.

Combined cue → old `track_NN` (silence-split index) → known use:

| cue | dur (s) | loop | old track | known use |
|----:|--------:|:----:|:---------:|-----------|
| 1 | 212.9 |  | track_00 | End Credits (official-soundtrack rip match) |
| 2 | 198.2 |  | track_01 |  |
| 3 | 160.2 | Y | track_02 |  |
| 4 | 186.6 | Y | track_03 |  |
| 5 | 217.8 | Y | track_04 |  |
| 6 | 222.4 | Y | track_05 |  |
| 7 | 182.8 | Y | track_06 |  |
| 8 | 198.8 | Y | track_07 |  |
| 9 | 90.7 | Y | track_08 | AREA07+AREA08 BGM (overlay func_1FB0B0 const) |
| 10 | 99.8 | Y | track_08 |  |
| 11 | 87.8 | Y | track_08 | AREA17 BGM |
| 12 | 90.3 | Y | track_08 | AREA21 BGM |
| 13 | 91.9 | Y | track_08 |  |
| 14 | 96.9 | Y | track_09 |  |
| 15 | 77.1 | Y | track_09 | AREA04 BGM |
| 16 | 81.5 | Y | track_09 |  |
| 17 | 7.9 |  | track_09 | AREA08/AREA21 event sting (func_1FA790 const) |
| 18 | 8.1 |  | track_10 | AREA13 event sting |
| 19 | 10.0 |  | track_11 | AREA02 event sting |
| 20 | 104.5 | Y | track_12 | AREA01 BGM |
| 21 | 101.3 | Y | track_13 | AREA06 BGM |
| 22 | 117.9 | Y | track_14 |  |
| 23 | 90.3 | Y | track_15 | AREA00 BGM |
| 24 | 84.0 | Y | track_16 | alert-mode override (func_001FAE70, D_008104E4==1) |
| 25 | 59.9 | Y | track_16 | area 11 BGM (live save state 01) |
| 26 | 60.5 | Y | track_16 |  |
| 27 | 9.1 |  | track_16 | frame_main func_001AD4E0 (alongside a movie call) |
| 28 | 10.1 |  | track_17 |  |
| 29 | 26.5 |  | track_18 | D_0026EC60 A11/t0 |
| 30 | 118.9 |  | track_19 | D_0026EC60 A01/t0x58 |
| 31 | 18.2 |  | track_20 |  |
| 32 | 19.1 |  | track_20 | D_0026EC60 A04/t0 |
| 33 | 52.1 |  | track_21 | D_0026EC60 A04/t7 |
| 34 | 141.6 |  | track_22 | D_0026EC60 A04/t0x2c |
| 35 | 24.5 |  | track_23 | D_0026EC60 A04/t0x8f |
| 36 | 108.9 |  | track_24 | D_0026EC60 A13/t0 |
| 37 | 24.7 |  | track_25 | D_0026EC60 A19/t0 |
| 38 | 117.9 |  | track_26 | D_0026EC60 A19/t5 |
| 39 | 160.6 |  | track_27 | D_0026EC60 A15/t0 |
| 40 | 13.7 |  | track_28 | D_0026EC60 A15/t0x5f |
| 41 | 44.7 |  | track_29 | D_0026EC60 A15/t0x64 |
| 42 | 156.2 |  | track_30 | D_0026EC60 A15/t0x81 |
| 43 | 75.1 |  | track_31 | D_0026EC60 A15/t0x117 |
| 44 | 137.8 |  | track_32 | D_0026EC60 A04/t0x92 |
| 45 | 14.3 |  | track_33 | D_0026EC60 A00/t0x31 |
| 46 | 130.5 |  | track_34 | D_0026EC60 A15/t0x150 |
| 47 | 21.7 |  | track_35 | D_0026EC60 A00/t0x32 |
| 48 | 24.0 |  | track_36 | D_0026EC60 A14/t0 |
| 49 | 46.8 |  | track_37 | D_0026EC60 A17/t0 |
| 50 | 32.3 |  | track_38 | D_0026EC60 A17/t0x15 |
| 51 | 150.2 |  | track_39 | D_0026EC60 A17/t0x16 |
| 52 | 83.2 |  | track_40 | D_0026EC60 A17/t0x5f |
| 53 | 16.6 |  | track_41 | D_0026EC60 A17/t0x82 |
| 54 | 120.7 |  | track_42 | D_0026EC60 A11/t0x19 |
| 55 | 23.9 |  | track_43 | D_0026EC60 A16/t2 |
| 56 | 158.1 |  | track_44 | D_0026EC60 A16/t3 |
| 57 | 25.9 |  | track_45 | D_0026EC60 A20/t0x19 |
| 58 | 23.0 |  | track_46 |  |
| 59 | 35.8 |  | track_46 | D_0026EC60 A21/t0 |
| 60 | 39.4 |  | track_47 | D_0026EC60 A21/t0x17 |
| 61 | 49.9 |  | track_48 | D_0026EC60 A21/t0x24 |
| 62 | 316.7 |  | track_49 | D_0026EC60 A21/t0x2b |
| 63 | 25.1 |  | track_50 | D_0026EC60 A11/t0x66 |
| 64 | 31.1 |  | track_51 | D_0026EC60 A01/t0xa5 |
| 65 | 46.7 |  | track_52 | D_0026EC60 A14/t1 |
| 66 | 129.6 |  | track_53 | D_0026EC60 A14/t0x20 |
| 67 | 1.0 |  | track_54 | real table entry (1 s), not a decode artifact |

The silence split merged adjacent cues 12 times (9–13, 14–17, 24–27,
31–32, 58–59), which is exactly 67 − 12 = 55. This supersedes the earlier
"MUSIC.DAT track listing" reading: the bimodal 25/30 guess maps onto the
engine's real partition — non-looping pieces 1–2 + looping BGM 3–16/20–26
+ stings 17–19/27–28 (cues ≤ 28, `func_001FAFD0`'s BGM range) vs scripted
streams 29–67. `clip_0054`/`track_54` is cue 67, a genuine (tiny) entry.

### Corrections / open items

- `tools/audio_export.py` `music`/`voice` now read the cue tables from the
  user's local boot ELF (`--elf`, default `elf/SCUS_971.12.elf`) and emit
  exact per-cue WAVs named `cue_NNN.wav`; the silence split remains as the
  no-ELF fallback. Verified: cue split is sample-identical to the old
  output over aligned ranges and the fallback still reproduces 55/116.
- **Open:** who writes `D_00810D38` (initial per-area BGM) at level load —
  zeroed in `func_001AD740`/`func_001ADF00` and set by overlay
  `func_1FB0B0` calls in 8 areas, but several areas' base BGM (cues 2–8,
  10, 13–14, 16, 22, 26, 28) have no caller found yet; likely set
  data-driven from area config or by overlay code paths not yet read.
- **Open:** entry flag semantics beyond "looping BGM set" (it is stored to
  `D_00281FF0[ch]` and read back by the streamer refill; behaviour-level
  confirmation that it gates seamless loop-on-end still pending).

## SECOND SCENE END-TO-END: the chunk15 snow level (AREA11, save state 01) (2026-06-10, s16)

The whole scene pipeline (render mesh -> EMDL, collision -> EMCL, textures
from a VRAM snapshot, placements from the OVERLAY table) is proven on a
SECOND level: the outdoor snow level **chunk15**, rendered and walkable in
the native port (`EM_SCENE=assets/scene_snow`).

### Level identity — disambiguating the two "snow" states

`chunk15` = **AREA 0x0B (AREA11.BIN), save state 01** — pinned live: state
01's EE `D_00810700/701` = 0x0B/0x00, placement desc `0x24D7C0[0xB]` ->
0x2759AC -> table **0x82A3C0** (21 entries, = s11's AREA11 entry), player
actor at **(218.592, 229.85, 201.789)** inside chunk15's main-zone bbox,
and every chunk15 file EE-RAM-resident in state 01. The s10/s11 phrase
"state 03 (snow, area 0x0600)" refers to a DIFFERENT (also snowy) level —
AREA06 is NOT chunk15's table; both are snow scenes (the game is set in
Antarctica), which is what the older "state 01 = gameplay snow" notes
meant. The task-relevant mapping: **chunk15 <-> AREA11 <-> save state 01.**

### Multi-zone level layout (differs from the office!)

The chunk's files load CONTIGUOUSLY into EE RAM (state 01 map: f06 @
0x13A1740 ... f07 @0x13B9F40 ... f12 @0x13F3F40 ... f18 @0x1842740, each
file starting exactly at the previous one's end; f00, the SShd bank, loads
elsewhere). Consequences:

- **Render meshes**: no single id-0x43 "render file role" — chunk15's id
  0x43 is its sound bank. The drawn level is SIX files of standard 64-byte
  render records: `f12_id44.bin` tail [0x127BD0..0x2D6FF0] (main zone,
  X[33,408], slot 0), `f13_id50/f14_id5a/f15_id47/f16_id88` (whole-file
  static east zones, X up to 621), and `f17_id93` (the movable/sub-object
  set: 5247/5428 records carry nonzero matrix-slot bits 3..14, but the
  positions are WORLD-space — at-rest articulation = bake as-is, unlike
  the office's object-space door assemblies). `f05_id97`/`f06_id98` are
  OBJECT-space multi-slot assemblies (bbox +-50, slots 1..12) — placed at
  runtime, NOT exported (no slot matrices available without a live frame).
- **Collision SPANS FILE BOUNDARIES**: the grid section header sits at
  `f07_id52.bin+0x1800` and its pools run through f08/f09/f10/f11 into
  f12 (live directory[0] @0x28A598 = 0x13BB740 = f07+0x1800; node array =
  f12+0x8E2C). `tools/export_collision.py` now accepts multiple files and
  decodes the byte-concatenation. Grid = **3503 verts / 3099 nodes**; the
  84 cell n-gons live in f12 as before. Floor probe at the live player XZ
  -> y = 229.85 — exactly the live actor Y. **165 grid quads are warped**
  (3 verts on the stored plane, 4th up to ~20u off): legitimate outdoor
  terrain data (the engine keeps one plane per node); the validator now
  counts these instead of failing.
- **Texture residency is whole-level**: all 290 zone-file TEX0 keys (96/
  51/41/43/24/35, ALL PSMT4) resolve from state 01's VRAM with healthy
  16-color CLUTs — `export_level.py --p2s` (the s15 export_native path,
  now in the level exporter too).

### No framebuffer in a HW-renderer .p2s (negative, useful)

State 01's GS freeze holds NO rasterized frame: the FRAME-reg page (56)
is zeroed; occupied VRAM pages (216-292, 300-308, 336-444) are exactly
the texture+CLUT pack. With the hardware renderer the rendered image
lives in GPU textures, not GS local memory — a .p2s yields textures but
never screenshots. (The old reference screenshot /tmp/cap2/Screenshot.png
is deleted; capture comparisons for this scene are vs the documented
description: soldier at a snowy gate, night palette.)

### Port wiring + what did NOT generalize

- `EM_SCENE=<dir>` (port main.c): switches scenes by staging a symlink
  shadow of assets/ with `<dir>` linked as "scene" + chdir. Default is
  untouched (office capture byte-identical). Scene dirs must name their
  collision `office.emcl` (the loader's compile-time constant) — a real
  per-scene name needs a src/game change (fenced this session).
- The port's spawn is compile-time (kPlayerPos, office coords), so the
  snow scene is baked with `--offset -111.192,-229.85,-385.789`
  (export_level + export_collision), anchoring the live state-01 player
  position onto kPlayerPos with floor y=0 at the spawn. Re-export without
  --offset once the port reads the engine spawn tables (func_001B07C0 /
  0x24D650).
- NOT exported: f05/f06 object-space assemblies (need live slot matrices
  or the spawner's model-byte -> mesh-blob binding), placement-record
  props/pickups/enemies (AREA11's 21 records parse fine via
  placements.py, but the model byte -> render-blob mapping that s9
  recovered for the office from live captures is unknown for this area),
  and snow particle effects. The office's hand-mapped movable regions
  did not generalize — chunk15 simply doesn't need them (its movables
  are world-space at rest), but a third level might.

**s18 supersession note**: the two port-side limitations above are gone —
the SCENE MANIFEST (`scene.txt`, written by the exporters; spawn /
collision filename / optional bgm, parsed by `em_game.c` at boot) gives
every scene its own spawn and collision name, and both exporters dropped
`--offset`: the snow scene is re-baked in TRUE world coordinates (spawn
`218.592 229.85 201.789`, collision `snow.emcl`). See PROGRESS s18.

_Last updated: 2026-06-10 (session 18: scene-manifest supersession note;
content session 16)._

## FIRST INTERACTIVE OBJECTS — pickup + door state machines, actor registry/tick decoded (2026-06-10, session 15)

Static disassembly walk of the two placement-table behavior functions from
s11 (`func_001C4820` prop/pickup, `func_001BC350` door), the spawner's
registry linkage, and the player-side trigger scans. This is the semantic
contract for the port's first interactive objects. All offsets are into the
0x2F0-byte actor struct unless noted.

### 1. The actor registry (closes "spawn/tick linkage")

**Pool:** 256 actors x 0x2F0 bytes at `D_007A5640`, initialized by
`func_001AF8E0` (memset each, chain free list through +0x1C; free head
`D_00275BC4`, free count `D_00275BC8` halfword = 0x100). A second static
24-slot actor array lives at `D_0028B020` (ticked separately by
`func_001B0000`; persistent/global actors).

**Alloc `func_001AFA90(cls)`** (called by the spawner and by scripted
spawners like `func_001C5C50`): refuses class 0xC when fewer than 10 free
(reserve for transient FX); pops the free list and initializes:
`+0x00 status=2`, `+0x02 class byte` (low 5 bits class, bits 5-7 flags —
bit 7 = "interactive"), `+0x14 self-pointer`, scale `+0x60..6C`=1.0 x4,
`+0x70/74`=0 `+0x78/7C`=1.0, `+0x80..8C`=1.0 x4 (anim scale), pos
`+0xB0..B8`=0 `+0xBC`=1.0, `+0x94`=-1 h, `+0x96`=0 h, `+0x99/9A/9C`=0;
links into the ACTIVE doubly-linked list via `func_001AFA50`
(head `D_00275BC0`, tail `D_00275BBC`, prev +0x18, next +0x1C); zeroes
`+0x30, +0x52..58 h, +0x5C=0x00010101`; class 2 additionally gets
`+0x9D = D_00810701` (area sub-state), `+0x9E = D_00810702`.

**Spawner `func_001B6990`** (s11, field map confirmed at instruction
level): walks the placement table, allocates per record (skipping
cls&0xFF==0x0B deferred records), copies model→+0x03,
flags2→+0x2E (u16), param lo→+0x0D, table index→+0x9A, uid→+0x0E (u16),
kind→+0x54, link→+0x56, pos→+0xB0/B4/B8, rot→+0xC0/C4/C8, and the
record's +0x24 BEHAVIOR fn→+0x10. One correction to s11's map: for
class-2 records uid is NOT copied to +0x0E; instead +0x9D=D_00810701 and
+0x9E=uid lo byte. If the pool empties the scan aborts.

**Tick `func_001AFD70(mode)`** — called from the gameplay frame
(`func_001AE5E0`, the "world services" slot after the player update):
walks the active list; mode 1 = only class 1, mode 2 = all except
class 1, mode 0 = all. Per actor: `func_001CB590(actor, 0x2F0,
bonecount@+0x09)` — this just publishes `D_00275B48=D_00275B44=actor`
("current actor" globals) and sizes the shared bone work array — then
clears byte +0x01 (per-frame visible flag) and calls the BEHAVIOR
POINTER `*(+0x10)(actor)`. Per-frame walk counter at spad 0x70003B8A.
`func_001CB5A0` ("context end") is a no-op. So the s11 placement table's
+0x24 pointer is invoked once per frame per actor, exactly here. The
player actor 0x008102B0 is NOT in this pool (updated separately by
`func_0015BCF0`).

**Free `func_001AFC10(actor)`** (behavior states 2/3): takes the
canonical pointer from +0x14, runs `func_001AF800` (bone-slot release) +
`func_001AFBC0` (active-list unlink), pushes onto the free list, clears
+0x00/04/08/0C, +0x36 h, +0x90 w, +0x98 b, and zeroes the 0x100-byte
tail +0x1F0..+0x2F0 (per-behavior scratch area).

**Per-frame category lists** (how other systems find actors without
walking the pool): when an actor is visible, `func_001B17A0`/
`func_001BC300` call `func_001B1B70`, which pushes `*(+0x14)` onto
downward-growing per-CLASS pointer lists rebuilt every frame:
class 1→`D_00275B70/78`, 2/0xA→`D_00275B54`-area, 4→`D_00275B80`
cursor/`D_00275B88` count (cap 0x80), 7→`D_00275B90`-area,
0xD→`D_00275BA0`-area, and — if class-flag 0x80 (interactive) —
additionally `func_001B1DE0` pushes onto the INTERACTIVE list
`D_00275B60` cursor/`D_00275B68` count (cap 0x20). At frame close-out
`func_001AAD00` double-buffer-swaps cursors/counts (published copies:
interactive `D_00275B5C/B64`, class-4 `D_00275B7C/B84`, ...), so
consumers always read last frame's complete list.

### 2. `func_001C4820` — generic placed-prop / item-pickup behavior

Tiny per-frame state machine on lifecycle byte +0x04:

- **0 INIT:** `func_001B0FD0` → `func_001B0EA0`: binds the model by ITEM
  TYPE byte +0x0D through the per-area model table `*(D_0028A59C)`
  (`func_001C6120` lookup → `func_001CA6E0` bind: stores model record to
  +0x44 and selects the +0x4C ANIM-MODE METHOD from `jtbl_0026E310` by
  the record's mode field — mode 0 = `func_001CAA00` default static);
  reads bone count (`func_001C6150`) → +0x0C, allocates one matrix slot
  per bone (`func_001AF780`) → pointer array +0x110, +0x09 = count.
  If the matrix pool is short (`D_00275BCC`), sets state 3 (self-free).
  On success state→1 and the dispatcher runs `func_001C6380`: build TRS
  world matrix at +0xD0 from pos/rot/scale and copy it into every bone
  slot (rigid prop). **MATCHED 100% readable C: src/func_001C6380.c.**
- **1 ACTIVE:** `func_001B17A0` (common prop frame): optional
  interaction-highlight hook when global mode `D_00810CA5`==6 for
  classes 7/8/0xA/2; then `func_001B1630(pos)` room/visibility test →
  byte +0x01; if visible `func_001B1B70` (category-list publish + draw
  submit). Then the +0x4C anim-mode method runs (bob/spin would live
  there; office pickups use the default static mode 0 — there is no
  bob/spin on these). NO gameplay logic in the pickup actor itself.
- **2/3 FREE:** `func_001AFC10` (despawn back to pool).

Readable-C attempt for the dispatcher reached 89.74% — delay-slot-fill
wall (analysis inline in src/func_001C4820.c; stays a .word stub).

**Pickup collection is NOT in the pickup behavior.** The office records
(AREA02 state-1 table dumped via tools/placements.py) show pickups are
class 0x0004 — WITHOUT interactive flag 0x80 — kind 0xB, model byte 0,
param = item type. The actual "take item" path is player-side and goes
through the published category lists (s15 found the consumers but not
yet the inventory write — see Open below).

### 3. `func_001BC350` — door behavior (class-5 double doors, model 3)

Outer lifecycle on +0x04 (0 INIT / 1 RUN / 2-3 FREE). INIT
(`func_001BBDA0` after `func_001B0F60` model-bind): door id +0x34 (u16)
= flags2 (+0x2E, from record +0x03), then +0x2E zeroed (reused as
"which side" latch); LINK halfword +0x56 bit 0x40 → uniform scale 1.5,
bit 0x80 → 2.0, else 1.0 (the s9 "double door is 2x scale" mystery —
office link 0x0280 vs 0x0200); status byte +0 = 1 (makes the door
eligible for the player's use-scan, below).

RUN: inner sub-state byte +0x05 through `jtbl_0026E1C0`:

```
0 CLOSED   model 0x15 ("security door"): unlock bitmask byte
           D_00810841[D_00810700] bit (1 << +0x34): set -> trigger
           func_001BBE40(self, blk, 0) -> sub 3; clear -> func_001BBE40
           (self, blk, 1) -> sub 1 (locked sequence). Other models:
           func_001BBE40(self, blk, 0) -> sub 3.
1 LOCKED-A func_001BC0E0 pump; when clip done queue script D_0024DBC0
           (func_001BA1A0) -> sub 2
2 LOCKED-B func_001BC0E0 pump; done -> +0x0B = 0 (re-arm), sub 0
3 OPENING  func_001BC0E0 pump (advance clip @1.0/frame -> blk+0xE; run
           queued script via func_001BA1F0); done -> sub 4
4 COMMIT   func_001BC240 [MATCHED 100% readable C]: advance clip, then
           func_001BC150 = THE TRANSITION COMMIT -> sub 5
5 CLOSE    func_001BC290: advance clip; once D_008106B8 (transition
           byte) returns 0: anim_clip_init(self, 0, 0.0, 0.0) reset,
           +0x0B = 0, sub 0
```

Every RUN frame ends in `func_001BC300` (already matched): evaluate the
door's multi-slot articulation via the generic keyframe evaluator
`func_001C68C0` (the s9 door-local slot animation IS skeletal animation
on the door's bones — no special articulation math), cull-test at
pos+(0,10,0), publish/draw, then the +0x4C method.

**Trigger (who arms +0x0B):** the player-side USE SCAN `func_00184BA0`
(callers: player locomotion states `func_001612D0`/`func_00160220`/
`func_0016DE40`): bails unless gameplay-frame selector spad 0x70003B8D
== 0; walks last frame's INTERACTIVE list (`D_00275B5C/B64`) filtering
status bit0 set, class flag 0x80, +0x0B == 0; per candidate
`func_00183EF0(player, actor)` — requires player action-state byte
+0x1F0 == 0x2D (pressing forward), a clear LOS query
(`func_0019A910`, mode 6; hit-flag 0x2000 blocks), distance^2 <= 144
(12.0 u), returns 2 (immediate) if < 4 (2.0 u), else facing-dot
threshold (~0.4) → nearest-candidate (distance via spad 0x70003B98).
Winner: `actor[+0x0B] = 4` and **spad 0x70003B8D = 3** — the gameplay
frame switches to the door-transit variant. So doors trigger on
walk-into (distance+facing), no button.

> **2026-06-11 s45 CORRECTION:** the LOS / dist^2<=144 / 2-u auto ring
> / facing-dot conditions above are func_00183EF0's CLASS-7 prefix.
> The CLASS-5 DOOR branch (read in full) measures from the DOORWAY
> CENTER (door_pos + 5*(-cos yaw, 0, +sin yaw) for models 3/0x15),
> radius desc D_002755F0 = {10.0, 8.0}, side test, then a pi/4
> through-door YAW gate — no LOS, no auto ring. See "DOOR USE SCAN +
> STAGING MATH DECODED" (s45).

> **2026-06-11 s58 CORRECTION (the trigger itself):** "doors trigger on
> walk-into (distance+facing), no button" is WRONG. func_00184BA0 is
> only ever CALLED on the USE-button press edge — all four call sites
> (func_00160220/func_001612D0/func_0016DE40 x3) gate on `D_00810E74 &
> *(u16*)0x70003B76` (press-edge mask & config "use" = 0x0040 CROSS),
> and the +0x1F0 == 0x2D check guards the CLASS-7 prefix ONLY (when the
> player IS in 0x2D, class-5 doors return 0). Doors arm exclusively on
> a CROSS press inside the class-5 window. See "DOOR TRIGGER IS THE
> CROSS PRESS EDGE" (s58).

**`func_001BBE40` (transit kickoff,** runs while +0x0B bit2 set**):**
computes which side the player is on (angle(player-door) vs door yaw,
threshold pi/2) → +0x2E side latch; writes a camera cue (front:
`D_0024DC14`=0x45/`D_0024DC54`=2/`D_0024DC8C`=90.0 or 0x43/0/70.0 +
`func_001BBD60(self, D_0024DC40)`; back via `D_0024DCD4/D_0024DD14`);
SNAPS player yaw +0xC4 to the door normal (yaw or yaw+pi); computes the
far-side point `door_pos -/+ 5.0*[cos,sin](side yaw)` (s45 trig-label
fix — originally written `[sin,cos]` from the swapped stub externs;
func_0011DE90 = cosf, func_0011E2A8 = sinf) adjusted by
player yaw into spad vec 0x700038A0 and issues a player MOVE-TO
(`func_00182F90`) — the door sequence WALKS THE PLAYER THROUGH;
queues door script `D_0024DE40` (side 0) / `D_0024DEC0` (side 1) on the
anim block and pumps it. Door scripts are command streams executed by
`func_001BA1F0`: 8-byte records `{u32 op|flags, u32 arg}`, opcode =
op&0xFFF dispatched through the function table `ftab_0024D880`
(opcode 0x18 = end/wait; flag bit31 = stop, bit30 = jump-to-arg ptr,
bit29 = arg-relative advance; spad 0x70003B91 gates a pause variant).
SOUNDS: no direct SShd call in the door fns — door audio is emitted by
script opcodes through ftab_0024D880 (table not yet itemized).

> **2026-06-10 s23 CORRECTIONS** (see "DOOR SCRIPTS DECODED" below):
> records are **0x40 bytes**, not 8; the script choice is **unlocked
> (D_0024DE40) vs locked (D_0024DEC0)** — the a2 mode arg — not side
> 0/1 (the side patches anim/clip/wait/sound values into the shared
> records instead); ftab_0024D880 is now fully itemized and the three
> scripts fully listed, door sounds = pair table D_0024DB80 indexed by
> link high byte + side.

**`func_001BC150` (transition commit):** door id +0x34 bit 0x80 set →
AREA CHANGE: `func_001B0C00(4)`, `D_008106B8=1`, and
`D_008106B5/B7/B6` = bytes 0/1/(2?3:0xFF) of record
`*(D_0024E140[D_00810700]) + 4*(+0x34 & 0x7F)` — the per-area DOOR
DESTINATION TABLE (next area / sub-state / entry point). Bit clear →
same-area room move: `func_001AEDE0(4,0)` (fade), `D_008106B8=2`,
`D_008106B7` = destination byte chosen by side latch +0x2E. The
flags2/uid of door records are therefore: low 7 bits = door id indexing
the area's destination table, bit 7 = "leads to another area".

**Door control panel (`func_001BD9F0`, office record[0] is the related
panel class):** its sub-state 1, when its script completes, follows its
active-list NEIGHBOR pointer +0x18 (the door spawned adjacently —
record order matters!), sets the partner's +0x0B = 1, points the view
target D_008105E0 at partner pos+(0,16,0), waits 0x3C frames (+0x28
countdown), then runs the same `func_001BC150` commit. I.e. panels
open/operate their adjacent door by arming the same flag byte.

### 4. The uid state words at 0x70003250 — refined

`0x70003250` (spad) holds a POINTER (= `*(D_0028A5A8)`, a per-area
loaded blob; count halfword cached at 0x7000324C, set up by
`func_00199C50`). Per-uid word at `ptr + 4*uid + 4`:
low bits = OFFSET (from ptr) of the object's COLLISION RECORD —
AABB {xmin,ymin,zmin,xmax,ymax,zmax} floats + halfword face list
(0xF000-coded face dir, walked by `func_0019BC40`/`func_0019F730`/
`func_0019FE50`/`func_001A0B10`/`func_001A2AE0`, the player/projectile
vs placed-object collision family, which iterate the published CLASS-4
list and gate on kind +0x54 < 0x51 and uid < count) — and flag bits:
0x40000000 (toggled by `func_0019C6F0`; e.g. collision on/off for
opened/removed objects), 0x20000000 (tested by it). `func_0019C6F0
(param|bit31, clear)` scans the placement table's leading class-0x0B
(deferred/scripted) records for a param match and tests 0x20000000 /
sets-clears 0x40000000 of that uid's word; it has NO main-ELF callers —
invoked from area overlay code (the scripted-spawn trigger path).
`func_00199DB0` returns an object's AABB center from the same records
(aim-assist/targeting). So: object MUTABLE state = these per-uid words;
the placement tables themselves are never written (s11 confirmed).

### 5. Port contracts

**Actor (entity) core — fields used by these systems:**
```
+0x00 u8  status (1 bits: 0=live/visible-eligible; alloc sets 2, door
          init sets 1)        +0x01 u8  visible-this-frame
+0x02 u8  class | flags (bit7 interactive, bit5/6 variants)
+0x03 u8  model byte          +0x04 u8  lifecycle state
+0x05 u8  behavior sub-state  +0x09 u8  bone count (ticked copy)
+0x0B u8  activation flags (bit0 panel-armed, bit2 player-use-armed)
+0x0C u8  bone count          +0x0D u8  param/item type
+0x0E u16 uid (hi byte = state-word index)
+0x10 fn* behavior (from placement record +0x24)
+0x14 ptr self (canonical pool pointer)
+0x18/+0x1C prev/next active links
+0x28 s16 timer (panel)       +0x2E u16 scratch (door: side latch)
+0x30 ptr anim/clip desc      +0x34 u16 door id (bit7 = area-change)
+0x44 ptr model record        +0x4C fn* anim-mode method
+0x54 u16 kind  +0x56 u16 link  +0x60 f32[4] scale
+0x80 f32[4] anim scale (door 1.0/1.5/2.0 from link bits 0x40/0x80)
+0x9A u8  placement-table index   +0x9D/+0x9E u8 (class 2 only)
+0xA0 q   player: position qword (player struct variant, 0x320)
+0xB0 f32[3]+1.0 position     +0xC0 f32[3] rotation (ry = yaw)
+0xD0 f32[16] world TRS       +0x110 ptr[] per-bone matrix slots
+0x1F0 u8[0x100] behavior-local scratch (door: anim block — +0xC byte
       clip-active, +0xE s16 clip time, +0x0 s32 script-active,
       +0x4 script cursor state, +0x8 script PC)
```

**Per-frame rule (port game loop):** after the player update, walk the
active actor list; for each: set current-actor global, clear
visible-flag, call behavior(actor). Behaviors publish themselves into
per-class + interactive lists consumed NEXT frame (double-buffered at
frame close). Free = unlink + push free-list + zero +0x1F0 tail.

**Pickup contract:** spawn from table records kind 0xB (class 4, param
= item type); INIT binds model by item type and stamps the placement
TRS into all bone slots; ACTIVE = draw-if-visible only; remove by
setting lifecycle 2/3. Persistence/collection state = per-uid word in
the area state blob (bit 0x40000000) — the inventory-write call site is
still open (below).

**Door contract:** spawn kind 4 class 5|0x80, model 3; closed door arms
when the player pushes into it (state 0x2D, LOS clear, dist <= 12,
facing-dot >= ~0.4, nearest wins) *(s58 CORRECTION: arms on a CROSS
press edge inside the s45 class-5 window — the use scan only runs on
`D_00810E74 & spad-3B76`; there is no walk-into door trigger, and the
0x2D/LOS/ring conditions were the class-7 prefix — see "DOOR TRIGGER IS
THE CROSS PRESS EDGE")* OR when its neighbor panel arms it;
model 0x15 doors additionally require unlock bit
`D_00810841[area] >> door_id` else they play the locked sequence and
re-arm; opening/closing = keyframe clip on the door skeleton at
1.0/frame; at clip end commit transition via the per-area destination
table `D_0024E140[area][door_id & 0x7F]` (bit7 = inter-area; writes
D_008106B5..B8 consumed by the area loader), walking the player to
`door_pos ± 5.0` along the door normal with yaw snapped.

### Open items

- The INVENTORY WRITE on item pickup: not in the pickup behavior, not in
  the door/use scan; candidates are the interaction-UI machine
  (`func_002149F0` cluster — its `func_00185420`/`func_00184D20` scans
  cover examine actions 0x1B-0x27 against class-4 models 0x14-0x2C, not
  kind-0xB items) or area-overlay handlers via the published class-4
  list. Needs one live breakpoint session on an office ammo-box take.
- ~~`ftab_0024D880` script opcode table — not itemized~~ CLOSED s23
  ("DOOR SCRIPTS DECODED"); door sounds = D_0024DB80 pair table.
- ~~Door scripts `D_0024DBC0/D_0024DE40/D_0024DEC0` and camera-cue block
  `D_0024DC14..D_0024DD14` not yet dumped/decoded~~ CLOSED s23 — the
  "camera-cue block" is runtime-patched fields inside the script records.
- `func_00183EF0` only fully read for the class-7 prefix; its class-5/8
  door variants (office doors are class 5) assumed symmetric — verify.

## PLAYER AURA / GLOW BILLBOARDS DECODED — additive glow path in the port (2026-06-10, session 17)

Closes the "billboard/glow quads (lib models 20/21/110-118) need a
port-side billboard path" open item (s9). Sources: offline arena scan of
save state 01's EE RAM (no live PCSX2 needed — the last built frame's
DMA chains are intact in the state) + the office GS dump (frame1.gs).

### 1. Geometry — the "billboard" library models

- **Models 20/21 are ±5-unit CUBES** (6 quads, axis normals, 32 records,
  `n_nodes=1`), not flat quads. Both sample ONE 16x16 PSMT8 texture
  (TEX0 key `0x041695113222E9`, tbp 0x22E9 / cbp 0x20B4): a bright
  border RING (lum 149..217) around a faint interior (lum ~5..15). The
  cube faces' UV box is **0.281..0.719 — interior only**, so each face
  adds only ~6% brightness: the aura is faint by construction.
- **Models 110–118 are single small QUADS** with positions baked in
  model space (e.g. 110: a ~1.8x1.8 tilted quad, full 0..1 UV — these
  DO sample the bright ring; PSMT4 64x64 for 110, the shared 16x16 key
  for 111–118). They are the control-panel/station glow overlays the s9
  door-panel pass draws. Model 114 is a 13-block 3-node composite (12
  distinct PSMT4 TEX0 keys) — not a glow, leave to the object pipeline.

### 2. GS state of every glow draw (office dump, all 4 captured frames)

```
PRIM  tristrip, IIP=1, TME=1, FGE=1, ABE=1
ALPHA A=0 B=2 C=2 D=1 FIX=0x80   -> Cv = Cs*(0x80/128) + Cd = Cs + Cd  (pure ADDITIVE)
TEST  ATE=1 ATST=0(NEVER) AFAIL=3(RGB_ONLY)  -> never writes A or Z
      ZTE=1 ZTST=2(GEQUAL)                   -> depth TEST stays ON
ZBUF  ZMSK=1                                 -> no depth write (belt & braces)
RGBAQ green, PULSING per frame: (1, 20..215, 1)/0x80 at the player;
      (0,110,0), (1,114,1), (1,48,1) on the panel instances; model 110
      (1, 103..132, 1)
```

### 3. Placement — models 20/21 are the PLAYER AURA

Arena scan of save state 01 (snow scene): the glow draw units go through
the LEVEL kernel (CALL `0x00237180`, matrix CNT 9qw with VIF in the tag
words, RGBAQ via an A+D CNT just before the model REF). With the camera
matrix K = P*V read from ctx+0x23C0 (camera_probe), **W = M · K⁻¹ (ROW-
vector convention — the memory qwords are rows here, unlike the s7b
column reading of the object kernel)** gives, for ALL model-20/21 units
in the frame:

```
W = diag(1.6, 4, 1.6), identity rotation (player yaw NOT inherited)
t = (player_root.x, player_root.y - 9.1, player_root.z)
```

byte-exact across both frame buffers (player root = skin palette bone 0
= actor +A0 = (218.592, 229.851, 201.789); glow at y 220.751). Model 21
gets a THIRD pass in a late chain (CALL 0x239C90 cluster) with the same
scale ROTATED ~80° about Y — a slowly counter-rotating second layer; the
s7b "rank-2 variant" reading was this rotated pass, mis-decoded. So the
aura = additive scaled cubes anchored at the root, NOT camera-facing
billboards; the depth test (GEQUAL, no Z write) clips the below-floor
half of the 40-unit-tall box, leaving a soft green column over the body.
(In the office placement table world: pickups get the same 20/21 pair —
"models 20/21's parents are pickups" from s7b refers to those instances;
the player carries his own.)

### 4. EMDL extension + port render path (both repos)

- **EMDL vertex flag** (EMD2/EMD3 compatible — old files carry 0):
  vertex bone word bits 24..31 = per-vertex flags; **bit 31 =
  BILLBOARD+ADDITIVE glow**. For flagged verts pos = anchor (bone-local)
  and the normal slot = camera-plane corner offset (x=right, y=up,
  world units). `export_native.write_emdl` passes the bits through;
  `export_props.GLOW_ATTACHMENTS` bakes models 20/21 as one camera-
  facing quad each (half-extents 8x20 = cube 5 x scale (1.6,4), anchor
  (0,-9.1,0) on node 0, face UV box, fixed mid-pulse tints (0,110,0)/128
  and (1,58,1)/128 premultiplied into DEDICATED texture copies —
  `finish_textures`). The pulse animation is NOT reproduced (fixed
  mid-pulse bake) and the camera-facing quad replaces the engine's
  additive cube + rotating second pass — the standard modern equivalent,
  same silhouette, stated honestly.
- **Port (`em_gfx_metal.m`)**: mesh creation partitions flagged
  triangles to the index-buffer tail; `draw_skinned` issues them as a
  second pass — additive PSO (ONE/ONE, dest alpha kept), depth test on /
  depth write off, shader mode bit 1 (no alpha-test cutout). Camera
  right/up are recovered in the vertex shader from viewproj rows 0/1
  (P*V rows = view rotation rows up to a positive projection scale).
- **Acceptance**: EM_CAPTURE diff vs a no-glow re-export: 8.7% of pixels
  change, GREEN CHANNEL ONLY, max delta +18/255, confined to the region
  around the player — glows visible but subtle, scene otherwise
  byte-identical. Move test PASS, verify_all --no-container all-PASS.
- Remaining: panel glows 110–118 (level side — export_level is the
  owner), pickup-instance auras in 01_props, the per-frame green pulse.

## INVENTORY LOCATED — static global block at 0x810C62..0x810CB5, byte-per-type count array (2026-06-10, session 18)

Live PCSX2 session (DebugServer + first end-to-end use of pad injection).
This closes the s15 open item "the INVENTORY WRITE on item pickup":
the inventory is a **static global block addressed absolutely**
(`lui at,0x0081` + fixed offsets — NOT reached through the player actor
or any heap object, which is why the actor-side scans never found it).

### Layout (all addresses absolute, BSS)

```
D_00810C62   u8   rounds in the CURRENT SPR4 magazine (0x1E = 30 = full mag)
D_00810C63   u8   magazine-equivalent counter, capped at 98; overflow is
                  folded back into the round reserve at 30 rounds : 1 mag
D_00810C64   u8[] ITEM COUNT ARRAY, indexed by item TYPE id (one byte per
                  type): count_of(type t) = *(u8*)(0x810C64 + t)
                    type 0x10 (SPR4 mag pack) -> 0x810C74   (= 4 in test save)
                    s11 office ammo types 0x0B/0x0C/0x0D -> 0x810C6F/70/71
                  observed nonzero in the test save: types 0x05,0x07,0x13,
                  0x17,0x1B = 1, 0x1E = 6, 0x1F = 1, 0x20 = 2, 0x21 = 1
D_00810CB4   s16  SPR4 reserve rounds (the "120" with the bullet icon on the
                  status screen)
```

**Display-verified:** poking `0x810CB4` while the status overlay was open
changed the rendered reserve immediately (120 -> 99, then 150) — the UI
re-reads the canonical value every frame. Restored afterward.

### The code that proves it

- **Add-ammo / pickup switch** `0x001C4100..`: `switch(item_type a0,
  amount a1)` with cases 0x0F..0x1D. Case 0x10 (`0x001C4190`):
  `count[0x810C64 + type] += a1`; `0x810C63 += a1`;
  `0x810CB4 += 30*a1`; if `0x810C62 == 0` set it to 30 (auto-fill mag);
  if `0x810C63 >= 99` fold the excess into the reserve (x30) and clamp
  to 98. This is the long-sought **inventory write site** — each SPR4
  magazine pack adds 30 rounds.
- **Reload routine** `0x0017B300(_, mode)`: mode 0 = fill mag from
  reserve only if mag empty; mode 1 = unconditional; else top-up
  (`need = 30 - mag`); mag = min(30, reserve). Confirms 0x810C62
  semantics and the 30-round magazine.
- **Consume path** `~0x00170D40`: `lh/addiu -1/sh` on `0x810CB4`
  (reserve decremented per shot) with a coupled `sb` to `0x810C62`.
- 22 code references to immediate `0x0CB4` total (0x157E6C, 0x170D48/54,
  0x170ED0/DC, 0x1710D8/E4, 0x17B30C..F8 cluster, 0x1AF424,
  0x1C41CC..4230, 0x209A44, 0x211A00, 0x212098, 0x212434) — a ready-made
  worklist for naming the weapon/ammo functions.

### Player stats (same page, separate from the count array)

```
player actor 0x008102B0:
  +0x220 (0x008104D0)  float  HEALTH    (75.0 = "75/100")
  +0x228 (0x008104D8)  float  INFECTION (60.0 = "60%")
```

### Scratchpad checkpoint copy (NOT canonical)

`0x70003240`: u16 cluster `[22, 4, 120, 93, 50, 38, 18]` — contains
mag-pack count (4) and reserve (120) but writing it does NOT affect the
live game or the UI; it sits immediately before the s17 persistence
pointer (`0x70003250`) and is evidently a save/checkpoint snapshot of
the inventory+stats. Layout differs from the live block.

### Status screen, depot, and open questions

- Status overlay (Triangle): "04/06 SPR4" + bullet-icon 120 + battery
  bar + health 75/100 + infection 60%. The "04/06" pair is **not**
  reserve/30 (held at 04 with reserve forced to 99 and 150), not
  0x810C63, not 0x810C74, not the overlay-arena `04 00 06 00` records at
  0x8278D0/0x828470 (poked all, incl. across a close/reopen) — RESOLVED
  by the user (2026-06-10): the "04/06" display is the BATTERY level
  (current/max), not a magazine or weapon counter. The battery storage
  address is still unlocated (candidates: the 240-cap tick struct below).
- The green wall unit ("DEPOSIT") in the office save: pressing Cross
  produces a sub-frame interaction cycle — scratchpad u16 `0x70003B88`
  pulses 0 -> 4 or 3 -> 0 within ONE frame (caught only by the
  DebugServer watch_change 5 ms poller). It never changed the inventory
  in any of ~6 cycles even with ammo at 120 and the mag at 4. USER
  CORRECTION (2026-06-10): the station IS an ammo refill — the agent was
  interacting with it incorrectly (the instant sub-frame cycle at 35%
  emu speed skipped the menu; a proper open->confirm sequence works).
  Re-verify the inventory-write path through it once the fast build
  lands. The 10-second timers it starts
  (0x8102DC and 0x8104BC counted 0x15D=349 down to 0 at ~35/s) are
  display/popup timers, not ammo. Also OPEN.
- A regen/refill-shaped tick at `0x001418F0` operates on a struct
  passed in `a1` (+0x70 s16 capped to 0xF0=240, +0x78 s16 tick counter,
  threshold from a table at gp-0x7FA8 indexed by `*(u8*)0x0081050C & 3`)
  — 240 = 8 mags x 30; owner struct not yet identified (battery?).
  RESOLVED 2026-06-10 s21: **NOT the player battery** (that is
  `0x810CB2/0x810CB7`, see the BATTERY section below). Its `a0` is an
  actor with the same +0xA light-flag convention as the player actor;
  its three static call sites (`0x0013D908`, `0x0013DA28`, `0x0013DF28`,
  in an entity-update chain s1=actor/s0=sub-struct) never executed in
  the office save room — some entity's own light/regen, not the suit
  battery. Beware 0xF0=240 numerology: 0xF0 also appears as a message
  id in the depot UI (`0x00214B38`).

### Tooling notes (first live use)

- **Pad injection works end-to-end**: raw TCP `pad_press`/`pad_set` on
  port 21512 drove the depot interaction, the status overlay
  (Triangle), weapon draw (hold R1 via pad_set), and analog walking
  (lx/ly) — at 20-40% emu speed use pad_set with real-time sleeps;
  short pad_press taps both opened AND auto-confirmed the depot dialog
  within one hold (it cycles in <1 s game time), so separate
  open/confirm presses need pauses between them.
- **watch_change (5 ms poll + auto-pause) works** and catches sub-frame
  scratchpad transients. **MCP watchpoints did NOT fire at all** this
  session (`write` AND `onchange`, 0 hits even on an address rewritten
  every frame, interpreter EE) — treat pcsx2_set_watchpoint as broken
  in this build; use watch_change or find_pattern+disasm instead.

## DOORS IN THE PORT — blob anatomy, sealed-room collision, clip hunt NEGATIVE (2026-06-10, session 20)

Exporter+port session: the office double doors became the port's first
native interactive objects (`export_props.py --doors` + port
`src/game/em_door.{h,c}`). Three engine findings fell out:

### 1. Double-door blob anatomy (RGN_DOOR @0xA05C0, model 3)

The "2-slot double door" is NOT two leaves. Per-slot extents of the
door-local records (local_k = L0^-1 * L_k of the captured closed pose):

    slot 0 (48 recs): (-9.0, 0.0, -0.5)..(0.0, 21.0, 0.0)  — the FULL
           9x21 door panel, hinge edge at local x = 0 (= the placement
           origin; the table pos is the panel's hinge corner)
    slot 1 (80 recs): model-space ~1.9 x 0.4 x 1.3 fixture placed at
           local (-7.689, 9.0, -0.252) — a lock/handle/mechanism part

So door articulation animates one rigid panel + one small fixture.
Office door records (placements.py, AREA02 state 1): west door uid
0x0A00 fl=0x81 (door id 1, bit7 = AREA-CHANGE) link 0x0200 at
(57, 0, -220.5) yaw -pi/2; office door uid 0x0900 fl=0x02 (same-area
room move) link 0x0280 (anim scale 2.0) at (109, 0, -252.2) yaw 0.
Both captured closed poses are unit-scale — the link-bit scale does not
affect the rest pose.

### 2. The grid world is SEALED ROOM BOXES — doorways have no static opening

Office EMCL probes (correct plane convention: n.p = d, NOT n.p + d = 0):

- a full-height grid boundary plane at x = 60 spans z -252.9..-167.3,
  y -3.1..37.9 — 3 u EAST of the west door at x = 57;
- the office door at z = -252.2 sits BETWEEN two boundary planes at
  z = -250 (n +z) and z = -255 (n -z), each room with its own floor poly.

Free movement can NEVER cross a doorway: the engine's room transitions
work exactly as s17 documented — the use scan arms the door and
`func_001BBE40`'s MOVE-TO walks the player to `door_pos -/+ 5.0 *
[cos,sin](yaw)` (s45 trig-label fix — was written `[sin,cos]` under
the swapped stub externs), i.e. just past the far boundary plane (the +-5.0
matches the plane spacing). The walk-through is the ONLY way across.
Consequence for the use scan's LOS gate (mode 6, hit-flag 0x2000
blocks): a straight player->door segment ALWAYS crosses the near
boundary plane, so either those planes carry something that exempts
them or func_00183EF0's class-5 path differs from the read class-7
prefix (s17 open item). The port exempts LOS hits within 6 u of the
door origin (the doorway pocket) — FLAGGED approximation in em_door.c.

### 3. Door open/close clip — hunt NEGATIVE; a global 3-node object bank exists

> **2026-06-10 s30 CORRECTION — FOUND, in THIS bank** (see "DOOR CLIPS
> FOUND"): f02_id39's leading directory has 16 entries; the "5 clips"
> below are directory ids 9/12/13/14/15 (the only blob-id-0x74
> containers). Door clips 0-3 are blob-id-0x28 containers (directory
> ids 0-3) that find_id74_headers skips — clip 0's frame 0 matches the
> closed slot-1 local EXACTLY. The placeholder is retired.

`chunk27/f02_id39.bin` is a 5-clip id 0x74 bank, 3-node chain rigs
(parents -1,0,1), RESIDENT at EE 0xd1a750 in save states of three
different areas (globally loaded, like the chunk27 model library):
clips of 360/90/40/30/30 frames; the 90/40/30/30 set moves two nodes
vertically between a "together" pose (~y 11-12 both) and an "apart"
pose (y 20-22 / y 10) — some vertical two-part object (not yet
identified); the 360-frame clip slowly rotates a node at (-5.1, 0.2, 0)
(dial/beacon-like). NONE of the rest poses matches the double door's
closed slot-1 local (-7.69, 9.0, -0.25), and no other decodable id 0x74
container in the extraction has a 2-4-node rig that does
(`export_props.find_door_clip` re-runs the hunt on every export). The
double door's clip therefore remains UNLOCATED — the port plays an
honestly-flagged placeholder 90-degree hinge swing (60 frames at the
engine's 1.0/frame rate) until it surfaces; an EMDL re-exported with a
real clip (frame_count > 1) is picked up automatically.

### 4. Port implementation (extermination-port)

- Manifest doors section: `door <file> <x> <y> <z> <yaw> <radius>` per
  instance (radius = the s17 use-scan 12.0); door EMDLs live in
  `<scene>/doors/` so the static scene loader does not double-draw them;
  `00_level.emdl` is rebaked without the RGN_DOOR replays in the same
  `--doors` run.
- `em_door.c` mirrors the s17 machine with engine sub-state numbering
  (0 CLOSED -> 3 OPENING -> 4 OPEN -> 5 CLOSING; locked subs 1/2 not yet
  ported), 1.0 frame/tick clip advance, kickoff side-latch + far-point
  MOVE-TO (collision-free glide in player_move = the selector-3 transit
  variant), closed-door AABB hull on the movable-hull set (mask bit 0;
  suppressed only when fully OPEN), and the use scan (dist^2 <= 144,
  facing-dot >= 0.4, auto < 2 u, nearest wins, +0x0B = 4). PORT
  DEVIATION (flagged): trigger needs CROSS outside the 2 u auto ring
  (engine = walk-into via action-state 0x2D) *(s58: NOT a deviation —
  the engine trigger IS the CROSS press edge; the walk-into/0x2D claim
  was the s17 misread)*; OPEN holds on a timeout
  instead of committing an area/room transition (no native area loader).
- `EM_DOOR_TEST=1` end-to-end self-test (deterministic, real input API):
  blocked at the x = 60 boundary while CLOSED (min x 60.010), CROSS at
  dist 5.4 -> OPENING, transit carries the player to (52.16, 0, -220.59)
  through both planes, door reaches OPEN — PASS. Office EM_CAPTURE with
  a doors-less manifest + old assets is byte-identical to the
  pre-change build; EM_MOVE_TEST / test-input / verify_all green.

## BATTERY LOCATED — storage, display mapping, spend/recharge mechanics, L3 light (2026-06-10, session 21)

Live PCSX2 session (DebugServer + pad injection + screen observation).
The status overlay's "04/06" BATTERY readout is fully resolved.

### Storage (static globals, inside the inventory block)

```
0x810CB2  u16  battery CURRENT, in internal HALF-units (8 in test save)
0x810CB4  u16  SPR4 reserve ammo (known, unchanged)
0x810CB7  u8   battery MAX, half-units (0x0C = 12 in test save)
0x810C7F  u8   gate flag: nonzero = battery readout enabled on overlay
```

**Display mapping (display-verified): shown value = stored >> 1.**
8>>1 = "04", 12>>1 = "06". Poking `0x810CB2 = 4` with the overlay open
changed the text to "02/06" AND shrank the segment bar to 2 — live,
same frame. NOT 240-based, NOT 40-per-segment; the old 240-cap guess
is dead (see the func_001418F0 resolution above).

Overlay draw site (status screen module): `0x00209424`
`lh 0xCB2(at=0x810000); sra a0,v0,1` -> 2-digit draw via the number
formatter `0x001C5FB0(value, digits, flag)`; `0x00209460`
`lbu 0xCB7; sra 1` for the max; the "/" sprite from `0x00273568`
between them; whole readout gated on `0x810C7F != 0`. (Same drawer
reads `0x810CB4` at `0x00209A44` for the "120" reserve. The status
strings "BATTERY"/"HEALTH"/... live at `0x00273DB0` etc., pointer
table around `0x00267290`.)

### Mechanics — the battery is a spend/recharge CURRENCY, not a timer

- **New-game init `0x001AF380`**: zeroes both `0xCB2` and `0xCB7`
  (battery acquired as items later), sets `0x810C7B=1` and other
  inventory flags.
- **Pickups / capacity upgrades** (cases inside the inventory-add fn
  `0x001C41CC`): one item class adds `count*12` half-units and raises
  max to 0x0C (=06 displayed) if below (`0x001C43D0..444C`); another
  adds `count*36` and raises max to 0x24 (=18 displayed!)
  (`0x001C4450..44D0`); current is clamped to max after each add. So
  capacity can grow to 18 displayed units late-game.
- **Spending — powering devices** (interaction dispatcher case 6,
  `0x00214AE0`): device cost = `2 * (device->+0x34)` half-units,
  snapshot of current to `+0x12` of the UI struct; if current < cost ->
  "insufficient battery" path (message id 0xF0). Otherwise a
  progressive transfer loop (`0x00215654/0x002156A0`) drains
  **-2 half-units (= 1 displayed unit) per cycle** until the target is
  reached; the empty-threshold branch fires an event
  (`0x8106C5 = 0xFF`, scratch `0x70003B8D = 3`). Skippable via pad
  mask 0x0870 (Start/Tri/O/X in the swapped layout, `lhu 0x810E74`).
- **Recharging** (station/depot events, `0x002277D4 / 0x00227800 /
  0x00227884`): **+4 half-units (= 2 displayed) per cycle**, same
  progressive-transfer pattern.

### Shoulder light: L3 toggle, timed burst, NO battery cost

- **Toggle = L3** (button config TYPE A; it is the unlabeled 7th row of
  the BUTTON CONFIG screen — icons: X, O, Square, R1, R2, L1, L3).
  Verified by pad injection: each L3 tap flips player-actor byte
  `0x8102BA` (+0xA of actor `0x8102B0`) 0<->1. NOT R3 (R3/SELECT/L1
  all tested negative; SELECT opens the options screen).
- **The light is a timed burst, then auto-off**: on activation the
  actor timer `+0x28` (`0x8102D8`, s16) is loaded with 0x12C (300
  frames = 5 s) (`0x00161180`, gated on `actor->0x200 & 0x1000`); it
  down-counts per frame (`0x00161138`); at 0 the phase byte `+0x7`
  (`0x8102B7`) increments and anim/event id 0x15D=349 is set via
  `0x001749A0` (stores the id to `actor+0x20C`, blend 8.0) — the
  turn-off animation. Observed end-to-end: light flag returns to 0 by
  itself ~5-6 s game time after L3.
- **Zero battery drain**: `watch_change` armed on `0x810CB2` across a
  full on->auto-off cycle: 0 changes (and `0xCB2` never appeared in any
  4 KB page diff during light-on). In this build/room the shoulder
  light costs nothing; the 04/06 battery is exclusively the
  device-powering currency above. (Caveat: only tested in the lit
  office save room; re-check in a dark area if behavior ever looks
  battery-gated there.)
- The earlier "10-second timers" note: `0x8102DC`/`0x8104BC` getting
  0x15D is this same anim/event id 349 propagating, not a dedicated
  battery timer.

### Pad-state map (found en route; useful for all future input work)

```
0x810D40 / 0x810DC0  raw libpad buffers (status 00, mode 0x73,
                     buttons active-low at +2/+3, sticks +4..+7)
0x810E40             ptr -> 0x810D40 (processed-pad struct follows)
0x810E70  u16        HELD button mask, BYTE-SWAPPED layout:
                     value = (low_byte << 8) | high_byte, i.e.
                     SELECT=0x0100, L3=0x0200, R3=0x0400, START=0x0800,
                     d-pad U/R/D/L=0x1000/2000/4000/8000,
                     L2=0x01, R2=0x02, L1=0x04, R1=0x08,
                     TRI=0x10, O=0x20, X=0x40, SQ=0x80
0x810E72  u16        same value (latched copy)
0x810E74  u16        PRESSED-EDGE mask, same swapped layout
```

Handlers and the config-mapped action masks come from scratchpad
(`lhu 0x70003B76` and-ed with `0x810E74` at `0x00160234`); L3 is also
tested raw (`andi 0x0200`) by the reload paths `0x00170C9C` /
`0x00171060` (L3 doubles as reload when the weapon is drawn).

## WEAPON SYSTEM — full fire path, state machine, reload, port contract (2026-06-10, session 22)

Static walk of the whole trigger-press → damage chain (callgraph.py +
splat asm; no live session needed). Closes the s18 worklist on the
0x0CB4 references. **Headline verdict: the SPR4 rifle and the spread
sub-weapon are HITSCAN (one `func_0019A570` segment query per shot);
the missile and grenade sub-weapons spawn projectile ACTORS; tracers,
muzzle flashes and impacts are visual-only transient actors.** This also
confirms `level_world`/`func_0019A570` as the collision/ray system (the
SUBSYSTEMS.md step-6 hypothesis) — the spad result block is
`0x700031B0` hit point, `*0x700031D0` hit record (+0x24 vec4 normal,
+0x1A surface-type byte, +0x1C sub-part/hull word), `*0x700031D4` hit
ACTOR pointer.

### 0. Architecture — three kinds of actors

1. **Player actor `0x008102B0`** owns input, ammo, fire timing, fire
   animation (weapon modes of the +0x05 state byte).
2. **The GUN is a separate actor**, linked at player `+0x20` (global
   mirror `D_008102D0`). Behavior `func_0018A6B0` (installed via
   runtime pointer — zero static callers). Its `+0x03` model byte =
   weapon kind (0 = SPR4 chassis; 1/2/4 = other carried weapons). The
   player SM never spawns effects itself: it posts a **fire event** by
   writing `1` to the gun actor's `+0x2E` halfword; the gun behavior
   consumes (reads + clears) it next tick.
3. **Per-shot transients** are class-1 pool actors (this is why
   `func_001AFA90` keeps a 10-slot reserve for class allocations):
   impact marker (behavior `func_0018ABA0`), homing missile
   (`func_0018AF50`), ballistic grenade (`func_0018B3E0`).

### 1. Global state (extends the s18 inventory block)

```
D_00810C61  u8   FIRE-MODE select: 0 single / 1 burst-3 / 2 full-auto
                 (chooses fire sub-state family 10/20/30; also read by HUD)
D_00810C62  u8   SPR4 magazine (s18)        D_00810C63 u8 mag-equivalent
D_00810CB4  s16  SPR4 reserve = TOTAL rounds INCLUDING the mag — proven by
                 the reload routine, which sets mag = min(30, reserve)
                 WITHOUT subtracting (each shot decrements BOTH)
D_00810CA8/CAA/CAC/CAE/CB0/CB2  s16  per-sub-weapon ammo counters
                 (CAA: subs 1+2, CA8: sub 3, CAE: sub 4, CB0: sub 5 —
                 from which fire SM reads which; CB2 is UI-shared)
D_00810CA4  u8   AIM OPTION: 0 manual+target-cycling, 1 lock-on, 2 third mode
D_00810CA6  u8   current attachment/weapon id (HUD-wide)
D_00810525  u8   = player+0x275, current SUB-WEAPON 0..5 (global mirror)
D_008106E0/E4/E8 ptr  the three nearest valid TARGETS (= D_008106B0+0x30)
D_008102D0  ptr  = player+0x20, the gun actor
D_00810550  mtx  = player+0x2A0, per-frame HAND/AIM matrix (copied from
                 the camera/actor context *D_00275B40 ctx[0]+0x90)
D_008104A0/A1 u8 = player+0x1F0/1F1: anim id / anim-phase flag
D_008104E0  u32  = player+0x230, player context id (0xC = normal gameplay,
                 0x29 = second armed context)
D_008103D0  ptr  = player+0x120, gun draw-matrix slot (hand bone)
```

Player-actor weapon fields (under the +0x1F0 behavior-scratch region):
`+0x274` trigger latch · `+0x275` sub-weapon 0..5 · `+0x276` s16 fire
counter (+2/frame) · `+0x278/+0x27C` aim-blend pitch/yaw (0.5 = center)
· `+0x28` s16 burst counter · `+0x2A` s16 queued-shot flag · `+0x2E`
own request halfword · `+0x2F0` target-cycle index 0..2 · `+0x2F2` aim
latch · `+0x2F4` float fire interval (12.0 default = shot per 6 frames)
· `+0x317` aim-restore latch · `+0x318` mode byte (selects
beam-vs-flash drawer) · `+0x200` u32 config-mapped ACTION mask (bit
0x1000 = weapon-draw hold, R1 in default config).

### 2. Weapon-mode state machine (player side)

`func_0015B130` (per-frame player dispatch on +0x05 via
`jtbl_0026D3B0`) routes modes **0x1D/0x1E/0x1F/0x20** to the four
armed-stance tops `func_0016FCF0` / `func_001703E0` / `func_001729A0`
/ `func_00173000` (same shape; pairs 0x1D+0x1E and 0x1F+0x20 share the
aim-pose anim tables `D_00248B88` / `D_00248C68`, indexed by
sub-weapon ×2 → clip id). Each top switches on MAJOR state `+0x06`:

```
0     ENTER: reload-if-empty func_0017B300(·,0); on fresh draw
      (+0x317==0) anim 0x110 (draw) via clip arbiter func_001749A0,
      sound 0x162 @vol150 (func_0016F530, which also sets +0x1F1=1 and
      D_008106C7 voice-line latch); aim blends +0x278/+0x27C = 0.5
1     WAIT until action mask +0x200 bit 0x1000 confirms hold; then
      aim-pose anim from the stance table
2     AIM/FIRE loop: D_008106E0=0; func_0017ABA0 (pose);
      target acquisition func_00199220 (gated by D_00810CA4 / blink
      counter +0x2F0); then jr jtbl_0026D680[+0x275] → the per-sub-
      weapon FIRE SUB-MACHINE: 0→func_00170A60, 1→func_00171320,
      2→func_00171670, 3→func_00171B00, 4→func_00171E90, 5→func_001723D0
3     RELOAD-ANIM wait (anim 0x33; +0x01 visible-flag kept on)
0x63  aim-blend ramp-out setup (+0x28=8, per-frame deltas /8)
0x64  ramp countdown, then re-enter aim pose
0x65  HOLSTER: anim 0x111, sound 0x163, +0x317=0
0x66  wait R1 release + yaw smoothing (func_001B12B0 to +0x218 goal)
0x6E  hit-reaction exit (func_00174AC0, +0x23F>=2 → func_0017C440)
0x6F  forced exit (func_00178B90, +0x200 bit 0x8000)
```

Tail of every top: `func_001764E0` (look/torso), `+0xB4 -= 0.2`,
`func_00175900(·,1)` (anim commit), `func_001796C0`.

### 3. Fire sub-machine (func_00170A60 = rifle; the others are clones
per sub-weapon with their own ammo globals)

Sub-state byte `+0x07`, three families chosen at trigger-press by the
fire-mode byte `D_00810C61` (decimal 10/20/30 — CW constants 0xA/0x14/
0x1E): **10/11 = SEMI** (one shot per press), **20..23 = 3-ROUND
BURST** (`+0x28` counter, slti 3), **30..32 = FULL-AUTO** (interval
refresh while held). Common per-SHOT block (states 0xB/0x15/0x1F...):

```
*(gun+0x2E) = 1                      fire event -> gun actor
D_00810C62-- ; D_00810CB4--          mag AND reserve (s18 "consume path")
play 0x164 (anims 0x31/0x34) or 0x165 (0x32/0x35), vol 150.0
+0x2A queued-shot flag from held mask (D_00810E74 & *(u16*)0x70003B78)
```

Cadence: `+0x276 += 2` per frame, fires when `>= (int)+0x2F4`
(12.0 → 6 frames → 10 shots/s at 60 Hz). Dry mag: `func_0017B300(·,0)`;
fail (reserve empty) → click `0x169` (or func_001FB9F0(0x169,0x1000…));
success → anim **0x33 reload**, `+0x06++`, `+0x07=0`. **L3 (raw pad bit
0x200) = manual reload** → `func_0017B300(·,2)` top-up (see s22 pad-map
note). While aiming, the per-frame aim matrix `ctx[0]+0x90 → +0x2A0`
(=0x810550) and aim vector `ctx[0]+0xC0..C8 → +0x2D0..2D8` are
refreshed; if a target is locked (D_008106E0) `func_0017AF70` steers
the aim blends +0x27C/+0x278 toward it (per-stance gain constants,
clamp [0,1], max 0.02/frame).

### 4. Reload — func_0017B300(_, mode) — **MATCHED 100% readable C**

`src/func_0017B300.c` (this session). mode 0 = only if mag empty;
mode 1 = unconditional; else top-up (only if mag<30 AND reserve>mag).
All modes: `mag = min(30, reserve)`, NO reserve subtraction (reserve is
the total pool). Returns 0 = reloaded, 1 = nothing to do.

### 5. Target acquisition — func_00199220 (auto-aim)

Per aim frame: clears D_008106E0/E4/E8, walks the published per-class
enemy list (`gp D_00275B8C/B94`); a candidate must be: status!=0,
HP halfword `+0x34` != 0, `func_00183B80` targetable, distance < 260;
its aim point (`func_00183C40`, class-keyed jtbl_0026D7C0) projected by
the spad camera matrix `0x70003AC0` must fall in the screen cone —
lock-on mode (CA4==1): radius ≤ 50+55·spread; manual: |x| ≤ 66+50·s
AND |y| ≤ 45+45·s, where spread = gun scratch `+0x214` float; then TWO
rays: `func_0019A570(playerPos, target, 1, 0x20)` must hit THAT actor
(`*0x700031D4 == candidate`) and mode-6 world ray must be CLEAR (LOS).
Keeps the three nearest (E0 ≤ E4 ≤ E8). Tail draws the reticle
marker(s) via `func_001DD170(1, pos, 0, 0x80808080, 0)`.

### 6. Gun-actor behavior — func_0018A6B0 → func_00188630 (kind 0)

Lifecycle on +0x04 (0 init `func_0018A8D0` retry-bind, 1 run, 2/3
free); visible flag = player visible OR player anim == 0x33 (gun stays
drawn through reloads). Run dispatches on its own model byte +0x03:
0→`func_00188630`, 1→`func_00188A50`, 2→`func_00188DF0` (gated by
D_008106CC despawn flag), 4→`func_0018A1F0`.

`func_00188630` (the SPR4) every frame: copies the HAND BONE matrix
`ctx[0]+0x90` → `*(D_008103D0)+0x90` (the s8 "equipment draw matrix ==
bone matrix" mechanism); computes **muzzle pos** `+0xA0 =
M(0x810550)·(-3, tbl.y, 0)`, second point `+0xB0 = M·D_0024A220[idx]`,
**fire direction** `+0xC0 = normalize(B0−A0)`; idx = sub-weapon
(sub-0 remapped by aim option: CA4==0→row 7, CA4==2→row 6; rows are
16-byte vec4s in `D_0024A224/D_0024A220/D_0024A2A0/D_0024A300`).
While the player fire anim (0x31/0x32/0x34/0x35) is in phase
(+0x1F1==1): per-frame beam/flash drawer `func_001854E0` or
`func_00185760` by +0x318 (each raycasts then draws via
`func_001CD520`/`func_001E2BA0` — the laser-sight/muzzle-glow pass).
Then the FIRE EVENT (`+0x2E`, cleared on read) via `jtbl_0026D850`:

```
sub 0  func_001861C0 BULLET  + func_00187CC0 muzzle FX
       + shell eject func_001F4010(3, M·D_0024A300[idx])
sub 1  func_001869A0 spawn MISSILE actor (behavior func_0018AF50;
       +0x24 = locked target if ctx==0xC && CA4==1)   + rumble(0,0xE8,0xF,1)
sub 2  func_00186A60 SPREAD hitscan (rand spread)     + rumble(0,0xD8,0xC,1)
       + muzzle FX
sub 3  FX object func_001EFEB0(0x80000039, perp dir)  + rumble(0x65,5,1)
sub 4  func_001872C0 spawn GRENADE actor (func_0018B3E0;
       velocity +0x70 = dir, type byte from D_0081070B) + rumble(1,0xF8,0x12,1)
sub 5  (no fire arm)
```

`func_001B61C0` = pad-actuator rumble (libpad `func_00111018` on
`D_00810E40`).

### 7. The bullet — func_001861C0 (HITSCAN, evidence)

1. ENDPOINT: in player contexts 0xC/0x29 with aim option 0/1 —
   lock-on: D_008106E0's aim point; manual: shots CYCLE through
   E0/E4/E8 by the per-shot counter player+0x2F0 (the "3-target
   round-robin"); targeted rays overshoot the aim point by 5 units.
   No target: `muzzle + dir·260` (260 = max range, same constant as
   acquisition).
2. `func_0019A570(muzzle+0xA0, endpoint, mode 7, mask 0x20)`.
   MISS → tracer `func_001860A0(muzzle, endpoint)` only.
3. HIT: tracer to hit point; hit class 1 + victim class 2 (enemy) →
   **`func_001B41F0(victim, hitPoint, dir+0xC0, subPart, 5, 0)`** =
   the HIT APPLICATION: per-victim-model jtbl_0026DDF0 picks the
   blood/impact FX id (0x80000024/25/35…), writes **victim+0x36 =
   hit-reaction code and victim+0x70 = bullet direction vec4** (the
   victim's own behavior consumes these → flinch/damage/death; enemy
   HP is the +0x34 halfword the targeting loop tests). World hits:
   near-miss cone ≤ ~4.5° re-credits the locked target; else
   surface-type (+0x1A) keyed impact: types 2-4 → spark code 0x201,
   5 → FX 0x8000002C, 8 → FX 0x80000067.
4. ALWAYS spawns the impact-marker actor (class 1, model 3, behavior
   `func_0018ABA0`, +0x2E = impact code, pos = hit point, +0xC0 = hit
   normal) — decal/sparks/ricochet sound.

Enemy-side counterpart: `func_00146AF0` ("am I in the line of fire") —
checks player anim 0x31/0x32 + gun dir vector (or player yaw
D_00810374) against the vector to self for dodge AI.

`func_001E3630` is one more projectile/beam behavior (50-unit ray
steps, surface check 0x5B, impact FX 0x8000001A, tracer models via
obj_registry D_00253AA0/B30/BC0) with NO static spawner and no pointer
to it anywhere in the boot ELF or overlay binaries — likely reached via
a runtime-built pointer (enemy weapon?). Open.

### 8. Port contract (weapon system)

- **State**: the inventory block (s18) plus: fire-mode u8, sub-weapon
  ammo s16[6], aim option u8, current sub-weapon u8, 3-target array,
  aim-blend pair, fire counter/interval, burst counter, target-cycle
  index. All global/static — no heap.
- **Per frame (armed)**: (1) refresh hand matrix → muzzle pos + dir;
  (2) acquire up to 3 targets (cone test in SCREEN space + actor ray +
  world-LOS ray); (3) steer aim blends toward target #1 (≤0.02/frame);
  (4) advance fire counter by 2; if trigger state and counter ≥
  interval(12.0) → SHOT.
- **Per shot**: decrement mag AND reserve; post fire event to the gun;
  gun resolves: rifle = one ray (mode 7/mask 0x20, 260 range, endpoint
  = cycled target or dir·260), apply hit to victim (+0x36 code, +0x70
  dir, FX by victim model), spawn impact marker, tracer line, muzzle
  flash, shell eject, rumble; missile/grenade = spawn projectile actor
  with dir/velocity from the gun vectors.
- **Reload**: mag = min(30, reserve); reserve untouched; L3 = manual
  top-up; auto when dry; anim 0x33 gates firing.
- **Fire modes**: semi/burst-3/auto from D_00810C61; burst pause = 8
  ticks; auto = every 6 frames.
- **Sounds**: 0x162 draw, 0x163 holster, 0x164/0x165 fire, 0x169 dry
  click (all vol 150.0); anims 0x110 draw, 0x111 holster, 0x31/0x32 &
  0x34/0x35 fire/aim by stance, 0x33 reload.
- The port can replace the gun-actor event indirection with a direct
  call, but MUST keep the one-frame latency (fire event consumed the
  next gun tick) if animation/FX sync is to match.

### Matching notes (new idiom data, this session)

- `src/func_0017B300.c` matched 100% via: idiom-8 raw volatile casts
  (all three globals through `$at`); **NEW: a `(short)` cast on an
  int-cached volatile-short load reproduces CW's redundant
  `dsll32/dsra32` re-sign-extension before `slti`** (declaring the
  local `short` does NOT — mwcc knows lh extends); assigning a
  comparison back into the compared variable (`low = low < 30`) lands
  the slti result in CW's register; parking a value in the dead first
  PARAMETER register pins it to `$a0`; declaring a constant's variable
  first but materializing it between two guards makes its `li` sink
  into the second guard's delay slot (idiom-15 composition).
- `func_001869A0` / `func_001872C0` (missile/grenade spawns) reached
  90.4%/93.6% but are **wall #13** (mwcc fills the `beqz` slot with the
  safe `li v1,3`; CW leaves a nop) — stubs restored with analysis
  inline.

## ENEMY AI ARCHITECTURE — behavior inventory, crawler & leech state machines, damage system (2026-06-10, session 22)

Static analysis only (local splat `.s`, `tools/placements.py` over all 19
extracted `OVERLAY/AREAxx.BIN`). First systematic map of the
`entity_logic` cluster (SUBSYSTEMS 0x130–0x15A).

### 1. Behavior-pointer inventory (placement records, `+0x24`)

Scanning every overlay's sentinel-terminated placement tables (the s11
record format) yields **1011 placement records → 164 distinct behavior
function pointers**: **39 in the main ELF** (shared engine behaviors)
and **125 overlay-local** (per-area one-offs: scripted props, area
set-pieces; they live at 0x823xxx–0x82axxx and can call main-ELF
helpers or install main-ELF brains — AREA15's `0x826CF0` installs
`func_00153950`, see §5).

Main-ELF behaviors split into two families by address:

- **0x1BB–0x1E4 = interactive-object family** (already in FINDINGS s15):
  `0x001C4820` generic prop/pickup (107 recs, 15 areas), `0x001BC350`
  door (30), `0x001BB860` door variant (29), `0x001C2420` (45, all
  class 0x0B = scripted/deferred), `0x001C1A80` wall station (31),
  `0x001E3D90` (26, class 0x0D model 1 — a second generator type,
  areas 07/18/19/20), plus ~15 smaller prop handlers (0x001C4960,
  0x00158xxx doors…).
- **0x149–0x15A = creature family** (the actual enemy/NPC AI). The
  placement-table census, main-ELF creature behaviors:

| behavior | n | areas | placement profile |
|---|---|---|---|
| `func_0015A2C0` | 129 | 00,01,02,03,04,06,07,08,13,16,19,20 | class 0x0D, model 3 — **enemy GENERATOR** (spawn point), `kind` 0–6 picks a 20-byte config rec in `D_00248120` |
| `func_00156620` | 115 | 03,07,11,13,14,17,19,20,21,22 | class 4, kind 0x46, models 0x18/0xA/0xC — **destructible fixture** (egg/nest cluster), HP=1 |
| `func_001551B0` | 95 | 02,03,07,08,11,13,14,15,17,18,20,21,22 | class 4, kind 0xD/0xE, models 6 (82×)/0x1C/0x1E — **placed crawler** (most common placed enemy; characterized §3) |
| `func_00158D30` | 18 | 00,03,07,08,13,15,16,22 | class 8, model 2 |
| `func_00158BD0` | 17 | 00,02,03,07,08,16,21,22 | class 8, model 2 (sibling of the above) |
| `func_00159B90` | 11 | 10 areas | class 0x84, model 0x38 |
| `func_00159210` | 9 | 9 areas | class 0x84, model 0x2C/0x24 |
| `func_00159970` | 7 | 6 areas | class 0x84/0x86, model 0x37 |
| `func_00159620` | 5 | 5 areas | class 0x84, model 0x36 |
| `func_001581A0` / `func_001582E0` / `func_00158430` | 3/2/1 | — | class 0x44, model 0xE |
| `func_00158810` | 3 | 00,22 | class 0x86, models 0x12/0x13 |
| `func_00158EC0` | 3 | 02,16 | class 0x84, models 0x14/0x23 |
| `func_00156F30` | 3 | 03,22 | kind 0x46 fixture variant (also installed by weapon-cluster `func_00175640`) |
| `func_0015A070` / `func_00159E70` | 2/1 | — | minor |

Runtime-installed brains (never appear in placement tables):
`func_00153F10` (kind 0xD "leech", §4), `func_001546C0` (kind 0xE),
both installed by the generator helper `func_0015A200`;
`func_00153950` (AREA15 overlay enemy, §5); `func_00153540`
(installed by `func_00153290` ← `func_00152930`); `func_0015AAF0`
(installed by `func_0015AB00`). Crawler death also spawns children
with behavior pointers taken from per-area nest records (§3 state 2),
so the disc tables undercount live brains.

So: **~20 distinct creature behaviors engine-wide**, of which 5
account for >90% of placements; everything else is per-area flavor.

### 2. Shared actor lifecycle (creature family)

All creature behaviors switch first on `actor+0x04` (lifecycle):
`0` init → (`4` idle/dormant, where used) → `1` active → `2`
hurt/death sequence → `3` free (`func_001AFC10`). Sub-state in
`actor+0x05` (and `+0x07` inside state 2 for the crawler). Common
fields established this session:

| offset | meaning |
|---|---|
| `+0x03` | model/variant byte (from placement `model`) |
| `+0x04` / `+0x05` / `+0x07` | lifecycle state / sub-state / death sub-state |
| `+0x0A` | **group-alarm flag** (set by a damaged neighbor; wakes the pack) |
| `+0x28`, `+0x2A` | frame timers (attack/hop counters) |
| `+0x2E` | spawn counter (generator) |
| **`+0x34`** | **hit points** (s16). Crawler/fixture init = 1; leech init (`func_00154040`) = 10 |
| **`+0x36`** | **incoming-damage mailbox** (s16). Written by attackers; low bits = amount, high bits = weapon-type flags (`0x2000` tested by the fixture, `0x4000` set by the pair-pass below — e.g. `0x400A` = type 0x4000, amount 10). Behavior polls it in its tick, then clears it |
| `+0x4C` | per-actor render/post fn pointer (called at tick end) |
| `+0x52` | "on-surface" flag (floor-probe result at init) |
| `+0x70` | hit-source position (copied from attacker `+0xC0` by the pair pass, for knockback direction) |
| `+0xB0/B4/B8` | world position; `+0xC4` yaw |
| `+0x1F0` | anim context (clip id at `+0x1F0+0`, advanced via `anim_advance_time`; status bit `0x1000` = clip finished) |
| `+0x2D0..0x2EC` | crawler: 4 precomputed diagonal probe direction pairs (init) |

Ticks end with the common post-update `func_001B17A0` (visibility cull
via `func_001B1630` → `+0x01`, transform rebuild `func_001B1B70`,
special-area marker hook `func_001B1CE0` when `D_00810CA5 == 6`).

### 3. `func_001551B0` — the placed crawler (characterized state machine)

Variants by model byte: 6, 0x1C, 0x1E, 0x1F, 0x50 (the same set is
whitelisted in its group-alarm broadcast and in the projectile pair
pass `func_001A9000`).

- **state 0 INIT** — wait until `func_001B0FD0` (resource ready);
  `HP(+0x34)=1`; pick a base heading by variant (2.1213 or 4.5962
  rad), build the 4 diagonal probe vectors into `+0x2D0..0x2EC`;
  if placement flag `+0x0E` bit0: resolve the **per-area nest
  registry** — `D_0024A850[area]` (s16 index) + `D_0024D820[area]`
  (table ptr) → 0x2C-byte child records, filtered by
  `func_001B11E0(rec+0x2)` (model-resident check); floor probe
  `func_0019AB20(actor, pos, out=D_700038B0, 7)`: result 4 → not on
  a surface (`+0x52=0`). → **state 4**.
- **state 4 IDLE (on nest)** — poll `+0x36`: **any damage kills**
  (HP=1) → state 2, and *broadcast the alarm*: walk the live-actor
  list `D_00275BC0` (next ptr `+0x1C`) and set `+0x0A=1` on every
  actor with model ∈ {6,0x1C,0x1E,0x1F,0x50} and `+0x52 != 0`.
  If own `+0x0A` set (a neighbor was shot) → **state 1**, `+0x2A=6`.
  Otherwise idle: RNG-timed idle anims/chitters from anim table
  `D_002468B0` (sound 0x19C via `func_001FBD50`), then copy own root
  node matrix (`*D_00275B40[0]+0x90`) into `+0xD0` and call `+0x4C`.
- **state 1 ATTACK RUN** — sub 0 *steer*: probe the 4 diagonals
  (`func_0019AB20(...,7)`; result 2 + scratch `0x700031D4` nonzero =
  blocked); rotate the velocity vector ±3° (`func_00102B08` /
  `func_00102A60`, angle 0x3D56774F≈0.0524) away from blocked sides,
  or RNG heading if open; if ≥3 sides blocked and `+0x2A`
  exhausted → back to state 4. Variant 6 sets its hop timer from its
  height. → sub 1 *hop*: integrate velocity, vertical `+0x2C8` with
  gravity 0.052/tick; forward probe each hop; **probe result 4 (lost
  the surface) or attack timer expired → clear `+0x36` → state 2**
  (these things burst on/after the lunge — the suicide-attack path).
  Open: state 1 never polls `+0x36`; crawlers appear undamageable
  mid-lunge (one-frame windows aside) — verify live.
- **state 2 BURST/DEATH** (`+0x07` sub-machine) — sub 0: if it has a
  nest link, **spawn children**: for each registry record, allocate
  via `func_001AFA90` and copy rec → actor (model `+0x2`→`+0x3`,
  class, params, `pos += parent pos`, rot, **behavior fn `rec+0x28` →
  `actor+0x10`**). Per-variant gore: sounds 0x19D/0x19E
  (`func_001FC580`) + effect pairs `func_001EFD90`
  (0x8000000A+0x80000015 / 0x8000000B+0x80000014 /
  0x80000031+0x80000015 / 0x80000032+0x80000014). If killed by damage
  (`+0x36 != 0`) and variant ∈ {6, 0x1E}: knockback — the hit vector
  in scratch `D_700036E0` is RNG-rotated (90°/180°/270°), velocity set
  from it, death anim clip 0x22/0x29 from the `D_0028A56C` clip lib
  (`func_001C6120` + `func_001CA6E0`), `bone_init_default_1` → sub 1
  corpse-slide (4 probes at radius 6, settle) → state 3. Otherwise
  state 3 directly.
- **state 3 FREE** — variant 0x50 extra release `func_001B1190`, then
  `func_001AFC10`.

### 4. Generator + leech (the kind-0xD/0xE runtime enemies)

- **`func_0015A2C0` (generator, class 0x0D)** — most-placed behavior
  (129). State 0: `kind`(+0x54) indexes config `D_00248120` (20 B
  recs; field 0 ×2 → scratch `0x700038A0/A8` spawn offsets);
  `link`(+0x56) selects a **count table** — 1 → `D_002481B0`, 2 →
  `D_002481D0` (8 bytes/row, row = frame-RNG `0x70003B68 & 3`,
  column = global spawn counter `D_008106EC`/`D_008106ED & 7`,
  post-incremented); when the drawn count is 1 it immediately emits a
  pair of **kind-0xE** enemies (`func_0015A200(actor, 0xE, 0/1)`).
  State 1 active: idle loop (sound 0x42F every 128 frames, breathing
  anim phase `+0x20`/`+0x80`), and on its anim beat emits **kind-0xD**
  enemies (`func_0015A200(actor, 0xD, 0)`) until `+0x2E == 4`, with
  inter-spawn delay from `D_002481F0[RNG%3]`. State 2/3 →
  `func_001AFC10`.
- **`func_0015A200` (spawn helper)** — `func_001AFA90(2)` allocate,
  copy generator pos/uid, then **install the brain: `+0x10 =
  func_00153F10` (kind 0xD) or `func_001546C0` (kind 0xE)**.
- **`func_00153F10` (leech, kind 0xD)** — init `func_00154040`: bind
  model/rig 0x14 anim 0x13 (`func_001B10B0`), **HP=10**, initial yaw
  toward `(D_00810350, D_00810358)` (player X/Z mirror) via atan2
  `func_001B1240`, spawn sound 0x430. Active state runs only when the
  gameplay-frame selector spad `0x70003B8D` is 0 or 4. Brain
  `func_00154120` sub-machine:
  - sub 0 *approach*: proximity test `func_0019AA80(nodeA+0xC0,
    nodeB+0xC0, 0x20)` between node-table slots `+0x34`/`+0x40` of the
    current `D_00275B40` table (≤32 units; exact node identities
    unverified — likely leech body vs player attach node). On hit and
    player status `D_008102B0 == 1`: **latch onto the player** —
    `D_008102BF = 2` (latch type), `D_008104D4 = 5.0` (drain/damage
    magnitude), `D_008102B0 |= 2` (player "latched" status bit), and
    the relative vector → `D_00810320` (the on-player wiggle anchor).
    When the clip's 0x1000 bit fires → next anim
    (`func_00153ED0`), sub 1, `+0x28 = 0x78`.
  - sub 1 *stalk*: countdown; **homing** — yaw toward
    `(D_00810350, D_00810358)` smoothed at 0.0698 rad/tick
    (`func_001B12B0`).
  - sub 2 *windup*: on anim end → sub 3, snap yaw, sound 0x431.
  - sub 3 *lunge resolve*: ≤32-unit test again → latch with
    `D_008104D4 = 15.0` (the lunge hit hurts more) and → state 2;
    else radius-6 contact (`func_0019A570`) → state 2; else on anim
    end → state 3 (despawn).
  - state 2 *burst*: sound 0x434, gore effect `func_001EFE00
    (0x80000052)`, call `+0x4C`, → state 3.
  Open: nothing in the leech family reads `+0x36`/`+0x34` — leeches
  appear to die only by bursting on the player (and presumably the
  shake-off mechanic); HP=10 may be consumed by a handler not yet
  found. Verify live.

### 5. Damage application (how enemies get hurt)

Two producer paths write the victim's `+0x36` mailbox; consumers are
the behaviors themselves (no central HP system):

1. **Weapons (weapon cluster — see this session's "WEAPON SYSTEM"
   FINDINGS)** — melee `func_001735C0` writes
   `target(+0x18 of player struct)->+0x36 = dmg` directly (5 etc.)
   and sets the target's `+0x00` event byte, sounds 0x17D–0x17F;
   hitscan bullets apply through `func_001B41F0` (victim `+0x36` =
   damage code, `+0x70` = hit direction) — same mailbox+knockback
   contract as the pair pass below, confirming `+0x36`/`+0x70` as the
   universal enemy-damage interface.
2. **Pair-collision pass (gameplay frame)** — the actor system keeps
   two per-frame pointer lists, rebuilt each frame
   (`func_001AF8E0` allocates):
   - `D_00275B80` / count `D_00275B88` (max 0x80): **damage targets**,
     pushed by `func_001B1D20` (stores `obj+0x14`, the actor ptr);
   - `D_00275BA0` / count `D_00275BA8` (max 0x30): **hazards/
     projectiles**, pushed by `func_001B1DA0`.
   `func_001AAD00` (← `func_001AE5E0`, the gameplay frame; FINDINGS
   "ENGINE FRAME ANATOMY") runs `func_001A9000`: every hazard with
   class byte 5 × every live target with model ∈ {6, 0x1E, 0x1F, …}
   → sphere tests `func_001A8E80`/`func_001A8F40` (height tolerance
   ±6/8, radius² vs 8) → `victim+0x36 = 0x14` (20) and zero the spad
   loop counter `0x70003B88` (one victim per hazard per frame).
   `func_001A97B0` → `func_001A9480` is a second pass (swipe/flame):
   writes `victim+0x36 = 0x400A` (type 0x4000 | amount 10) and copies
   the attacker position `+0xC0` → `victim+0x70` (knockback source).
   The weapon agent's missile/grenade spawns (`func_001869A0` /
   `func_001872C0`, this session's weapon FINDINGS) allocate exactly
   the class-5 hazard actors this pass consumes.
3. **Consumption** — each behavior polls `+0x36` in its own tick:
   - HP=1 actors (crawler state 4, fixture `func_00156620` state 4):
     any nonzero value → death state (fixture also branches on the
     type bit 0x2000 → effects 0x80000013/0x8000002E, sound 0x1A0).
   - HP>1 actors: hurt-state helper — `func_00153B50` (state-2 handler
     of the `func_00153950` enemy) is the canonical form:
     `HP(+0x34) -= dmg(+0x36)`; `>0` → flinch (sound 0x7D4, anim clip
     0x37); `≤0` → death sub-state (sound 0x7D8, anim 0x34, kill flag
     `D_008107FB=1`). Its active state `func_00153A90` gates on the
     vulnerability global `D_0081077B` (nonzero → take the hit,
     zero → absorb: ping `+0x00=1`, clear `+0x36`) — a phase-gated
     (boss-like) enemy, installed only by AREA15's overlay stub at
     `0x826CF0`.
   - The s15 "death check Y<-200" in the player spine has **no enemy
     equivalent** — enemies die via the `+0x36` mailbox or scripted
     burst, not a kill-plane.

### Cross-references / open questions

- Generator config tables `D_00248120` (20 B × 7 kinds), count tables
  `D_002481B0`/`D_002481D0`, delay table `D_002481F0`, idle-anim table
  `D_002468B0`, nest registries `D_0024A850`+`D_0024D820` (0x2C-byte
  child-spawn records, per area) — all main-ELF .data, undocumented
  until now; worth dumping when the data segment is split.
- `func_0019AA80` (two-point radius test), `func_0019A570` (radius-6
  contact), `func_0019AB20` (directional surface probe; result codes:
  2 = blocked w/ detail in spad `0x700031D4`, 4 = no surface; out vec
  `D_700038B0`) are the collision-query family the AI senses with —
  same family as the s17 door LOS scan. None of the creature
  behaviors ray-test toward the *player* for vision; "seeing" the
  player is distance-only (leech ≤32 u) or alarm-driven (crawlers).
- Kind semantics refined vs s11: placement `kind` 0xD/0xE mark the
  crawler placements; the *generator's* `kind` (0–6) is a config
  index, not an enemy kind.
- Live-verify (PCSX2, future session): crawler invulnerability during
  state 1; leech HP consumption; node slots `+0x34`/`+0x40` identity
  in the `D_00275B40` table during enemy ticks.
- **s62 update — see "ENEMY CONDITION DECODE" below**: state-1
  invulnerability is now closed STATICALLY (never polls; defers/absorbs
  decoded), the crate trigger is damage-only (no proximity test exists),
  and the leech 32-u/radius-6/latch-magnitude values are verified off
  the instructions. Still open: leech HP consumption, node-slot
  identities.

## AREA TRANSITION LIFECYCLE — both modes captured live, end-to-end (2026-06-10, session 22)

Live PCSX2 capture of one complete **intra-area room move** (office double
door, id 2, both directions) and one complete **inter-area change** (west
door, id 1|0x80, area 02 → area 01), instrumented with the DebugServer's
`watch_change` poller (5 ms, auto-pause, old/new/cycle). Execution
breakpoints were NON-FUNCTIONAL in this DebugServer session (a BP on the
main-loop top never fired) — all ordering below is from data watchpoints
+ paused-state reads; writer functions are attributed statically.
Timing unit: vsyncs (NTSC field count at `0x00810E90`).

### 0. The global transition-state bytes (refined from s15/s17)

```
0x810700 u8 AREA            0x810701 u8 story SUB-STATE
0x810702 u8 ENTRY/room idx  0x810703 u8 area-currently-BUILT mirror
0x8106B5 u8 req: next area  0x8106B6 u8 req: next sub (0xFF -> 0)
0x8106B7 u8 req: entry idx  0x8106B8 u8 req: mode 0=idle 1=area 2=room
```

`0x810702` is not a "current room" — it is the **player-placement entry
index**, the index of the 0x30-byte record the player was last placed
from. The same value doubles as the room/visibility-zone id (the engine
showed "SUPPLY ROOM" after placing entry 3).

**Spawn-table chain (func_001B07C0, fully decoded from disasm):**

```
desc = *(u32*)(0x0024D650 + 4*area)        ; per-area (bss-resident for
tbl  = *(u32*)(desc + 4*sub)               ;  overlay-owned areas)
rec  = tbl + entry*0x30
rec: {f32 pos[3]; f32 yaw @+0xC; u32 flags @+0x10; u32 1 @+0x14;
      f32 @+0x18; u32 packed @+0x20; ...}   (pos+yaw verified live;
                                             +0x10.. likely camera init)
```
Override path: if byte `gp-0x7790` == 1, placement instead uses floats
`0x810710/14/18` (pos) + `0x810720/24/28` (rot) — an explicit
"place player here" request (scripted/cutscene placements).
Verified live: area-2 sub-1 tbl @`0x24B6B0` — entry 2 = (104,0,-245) yaw 0,
entry 3 = (104,0,-259) yaw pi (the two faces of the office door); area-1
sub-0 tbl @`0x24B1A0` — entry 5 = (39,0,-225) yaw -pi/2 (west face of the
west door). Entries flank their door, yaw facing AWAY (exit pose).

**Destination table (s17/s20, now byte-verified live):** record =
`*(D_0024E140[area]) + 4*(door_id & 0x7F)`, but its 4 bytes are read TWO
WAYS depending on door-id bit7 (`func_001BC150` disasm):

- bit7 SET (area change): `{next_area, entry_idx, has_sub, sub}` →
  B5=rec[0], B7=rec[1], B6 = rec[2] ? rec[3] : 0xFF (0xFF → sub 0).
  Area-2 door 1 = {01 05 00 00} → area 1, entry 5, sub 0. Return path
  area-1 door 3 = {02 01 01 01} → area 2, entry 1, sub 1 (symmetric).
- bit7 CLEAR (room move): `{entry_from_side0, entry_from_side1, -, -}` →
  B7 = rec[side_latch +0x2E], B8=2. Area-2 door 2 = {03 02 ..}: entered
  from north (side 0) → entry 3 (supply room), from south (side 1) →
  entry 2 (office). Both directions captured live.

### 1. Intra-area room move — captured timeline (door id 2, side 0)

```
vsync   +d   event
528519       door sub 0->3 OPENING (func_001BBE40 kickoff): side latch
             +0x2E computed (0=north); player SNAPPED to staging point
             (104.0, 0, door_z+5.0 = -247.2) — no gradual walk observed;
             door script D_0024DE40 queued on anim block +0x1F0
             (+0x00 active=1, +0x04 cursor=1, +0x08 PC=0x0024DE40)
528616  +97  clip done (script active = -1) -> sub 3->4
528617  + 1  sub-4 (func_001BC240) -> func_001BC150 COMMIT:
             func_001AEDE0(4,0) fade-out; B7=3 (rec[side 0]); B8=2
             -> sub 4->5 same frame
528681  +64  fade-out complete: 0x810702 <- B7 (2->3); player re-placed
             from spawn rec 3 = (104,0,-259) yaw pi  [same frame]
528682  + 1  B5..B8 cleared to 0
528682   0   door sees B8==0: clip reset, +0x0B=0, sub 5->0 (re-armed)
             (fade-in runs after, same 4-speed machine)
```
NOT touched: overlay arena (byte-identical), actor pool (same actor
slots; the door actor persists across the move), audio (no channel fade).
Reverse direction (side 1, captured earlier in-session): B7=2, player
placed at (104,0,-245) yaw 0 — symmetric.

### 2. Inter-area change — captured timeline (door id 1|0x80, area 2 -> 1)

```
vsync   +d   event
528813       door sub 0->3; side latch 1; same script PC 0x0024DE40
             observed; player snapped to staging (62.0, 0, -225.5)
             (= door east face + 5; door yaw -pi/2)
528890  +77  clip done -> sub 3->4
528891  + 1  COMMIT func_001BC150 bit7 path: func_001B0C00(4) =
             func_001AEDE0(4,0) fade-out + func_001FAD70(ch,4,1) x3
             audio-channel fades (ch 0/1/2 — inter-area kills audio;
             room moves do not); B5=01 B6=FF B7=05 B8=01 -> sub 4->5
528955  +64  fade-out complete, SWITCH BEGINS: 0x810700<-01 (area),
             0x810701<-00 (B6 0xFF->0), 0x810702<-05 (entry);
             player still at staging point
528958  + 3  overlay arena 0x823500 REWRITTEN: MWo3 "Area01.bin" flat
             load (header now ver=02, sizes 0x54C0/0x4300, end 0x82CD00);
             overlay init populates the per-area .bss descriptors
             (D_0024D650[1]=0x275500 spawn desc, D_0024D7C0[1]=0x2758D0
             placement desc)
528962  + 4  0x810703 <- 01 (built-area mirror updated)
   ...       ~290 vsyncs of asset streaming (textures/models/level mesh;
             dominates load time even on PCSX2's instant CD)
529254 +296  completion burst (within ~1 frame): ACTOR POOL freed and
             respawned from AREA01 placement tables (old area-2 door
             slots zeroed; free count changed); player placed from
             area-1 sub-0 spawn rec 5 = (39,0,-225) yaw -pi/2;
             B5..B8 cleared; fade-in
```
Totals: commit -> playable ≈ 363 vsyncs (~6 s; disc-speed dependent) for
the area change, vs 65 vsyncs for a room move. Fade-out is 64 vsyncs in
both modes (`func_001AEDE0(4,0)` / via `func_001B0C00(4)` — the 4 is the
fade speed).

### 3. Port contract — what an area loader must implement

1. **Request**: door commit writes the 4-byte request B5..B8 (above).
   Nothing else carries the transition.
2. **Both modes**: run fade-out (speed 4 ≈ 64 frames); door stays in
   sub 5 until it observes B8 == 0, then closes and re-arms — doors are
   reusable and persist across room moves.
3. **Room move (B8==2)**: at fade-out end, set entry byte, re-place the
   player from `spawn_tbl[area][sub][entry]` (pos + yaw), clear request
   next frame, fade in. KEEP: actors, overlay, audio, collision world.
4. **Area change (B8==1)**: additionally fade all audio channels at
   commit. At fade-out end: write area/sub/entry; load OVERLAY/AREAxx.BIN
   flat at the arena base; run its init (registers spawn/placement/
   destination descriptors); update the built-area mirror; stream the
   area's assets; FREE the whole actor pool and respawn it from the new
   area's placement table (s11 spawner); place the player from the new
   spawn table; clear request; fade in.
5. **Authoring data per area**: placement table (s11), destination table
   (4 B x doors, dual interpretation by bit7), spawn table (0x30 B x
   entries, per sub-state). All three live in/are selected by the
   overlay; spawn entries flank their door faces at ±5 with exit yaw.

### 4. Tooling + caveats (this session)

- DebugServer **execution breakpoints never fired** (tested against the
  main loop); MCP watchpoints also never paused. The raw TCP
  `watch_change` poller (5 ms) is the working auto-pause primitive —
  use it for any future transition/loader work.
- The player's authoritative position is NOT `0x810350` (actor +0xA0):
  writes there, to +0x100, and to the spad mirrors are all overwritten
  by the next frame. The locomotion state lives elsewhere (untracked);
  the engine itself moves the player only via the placement path above
  (and MOVE-TO). Finding the real store is an open item.
- The organic walk-into trigger (s17: action-state 0x2D + ring) did NOT
  reproduce under analog pad injection — the door never armed while the
  player pushed against the doorway plane. Both captures were started by
  writing the use-scan's own effect bytes (`door+0x0B = 4`,
  spad `0x70003B8D = 3`); everything downstream is organic engine
  behavior. The class-5 trigger conditions remain the s17/s20 open item
  (exec BPs being broken, the use-scan path could not be traced live).
  *(CLOSED s58: the non-reproduction is fully explained — the use scan
  only runs on a CROSS press edge, and the analog injection never
  pressed CROSS. See "DOOR TRIGGER IS THE CROSS PRESS EDGE".)*
- Door script: both captured transits queued `D_0024DE40` despite
  opposite side latches — s17's "side 1 -> D_0024DEC0" needs re-check.

## CRAWLER RESOLVED — model-table binding, the crate husk, and the real creature (the leech EMDL) (2026-06-10, session 23)

Resolved which mesh = the "placed crawler" `func_001551B0` (s22's most
common placed enemy) and shipped the first enemy EMDL to the port
(`extermination-port/assets/enemy_crawler.emdl`). Method: follow the
per-area model table from save state 01's EE RAM
(`tools/parse_pcsx2_state.py` → `*(0x0028A59C)`), content-match entries
against `extract/` files. Three corrections to s22 fell out.

### 1. Model-table resolution method (now a repeatable recipe)

- `func_001C6120(table, id)` = `table + (*(table + 4 + 4*id) & ~3)`
  (id-directory lookup; offset words, 0xFFFFFFFF = absent; word 0 =
  entry count).
- **Save state 01 = AREA11** (`D_00810700` = 0x0B): `*(D_0028A59C)` =
  `0x01335F40`, count 0x15 (model ids 0x00–0x14). The table + entries
  content-match `extract/chunk15/f05_id97.bin` **at file offset 0x5000**
  (RAM−file delta constant; entries verified byte-identical). Entries
  past the file's 0x70800 end (ids ≥ 0x13) continue in the next file
  loaded into the area arena.
- **Each model-table entry is a raw mesh blob** — the same format as
  `f00_id3b` (header `n_blocks/total_qwc/n_nodes/size`, 8 floats of
  bounds, first UNPACK at +0x48) — so a carved entry feeds
  `export_native.py --mesh` directly. AREA11 ids 0x00–0x14 interleave
  with `rig_probe`'s MESH_SIG segments of f05_id97 but are NOT 1:1 with
  X-separator segment indices; carve by table offsets, not by segment.

### 2. CORRECTION (s11/s22): creature mesh binds by PARAM, not the model byte

`func_001B0FD0` → `func_001B0EA0` (the crawler/pickup INIT bind) reads
**actor+0x0D = placement PARAM low byte** and looks it up in
`*(D_0028A59C)` (`lbu $a1, 0xD($a0)` at 0x001B0EB4). The placement
"model" byte (+0x03, the s22 6/0x1C/0x1E/0x1F/0x50 set) is only the
behavior VARIANT tag (heading constants, hop timer, gore-effect pick).
Crawler placements bind: AREA02 + AREA11 param 0x0D, AREA03 params
2/3/4, AREA07 param 0x0B, AREA13 params 4/0x29 — per-area skins.

### 3. The placed "crawler" renders as an INFECTED CRATE (husk)

AREA11 (and office AREA02) crawler param 0x0D → model-table id 0x0D =
**a 14×14×14 beveled CRATE**: 1 node, 6 blocks, 192 verts (177 welded,
90 tris), TEX0 PSMT4 128×128 TBP0 0x2E80 CBP 0x3759 (resident in state
01 VRAM; 16-color grey/brown box skin). AREA11 even stacks them (the
3-crate pile at x 214–228: y values exactly 14 apart). So the most
common "enemy" idles disguised as scenery; s22's state machine reads
as: wiggle/chitter in place (the "idle anim" is **procedural** — float
tables `D_002468B0/B4/B8` jitter the world-matrix x/z translation at
actor+0x100/+0x108; no skeletal clips exist: zero anim containers in
f05_id97/f12_id44 match a 1-node rig), hop at the player, burst.

**CORRECTION (s22 §3):** "death anim clip 0x22/0x29 from the
`D_0028A56C` clip lib" is actually a **MODEL REBIND**: `D_0028A56C` is
the global MODEL library (= `extract/chunk27/f01_id37.bin`,
content-matched at offset 0 in state 01 RAM `0x00BAA1C0`), and
`func_001C6120` + `func_001CA6E0` swap the actor's model record
(+0x44) to library entry 0x22 or 0x29 — the burst-husk/gib models.
`func_001CA6E0` = `func_001CA5E0(actor, rec, mode=0)`; the +0x4C method
is picked from `jtbl_0026E310` by the MODE ARGUMENT (not a record
field — refines s17's wording).

**Static/prop VU1 kernel W encoding** (new): single-node raw blobs keep
their one 7-qw matrix set at dmem qw 0; vertex W = ±1.0f with flag bits
14/15 only (bit 15 = strip restart, bit 14 = parity), node bits 0..9 =
0. `export_native.py` now accepts slot 0 for `n_nodes==1` blobs (packed
character meshes still reject slots < 2 as kernel scratch).

### 4. The real creature: kind-0xD leech = chunk03 f12_id14 + f11_id13

The thing that actually crawls (runtime brain `func_00153F10`, spawned
by generators and crate bursts) binds through a SECOND, **global
record-pointer array `D_0028A490`** (67 slots, indexed directly —
`func_001B10B0(actor, model_idx, anim_idx)`: model rec =
`D_0028A490[model_idx]` → +0x44, clip table = `D_0028A490[anim_idx]` →
+0x40, anim mode 9). **Slot index = source-file id**: slot 0x14 =
`extract/chunk03/f12_id14.bin` (mesh), slot 0x13 =
`extract/chunk03/f11_id13.bin` (clip bank) — both content-matched in
state 01 RAM at offset 0 (chunk03 is globally resident, so its textures
are in EVERY save state's VRAM).

- **Mesh** (raw blob): 12 blocks, 24 nodes, 180 welded verts, 284 tris,
  one PSMT4 128×128 texture (dark red/black striated flesh). Rig =
  segmented worm: parents `[-1,0,1,...,14, 14, 15×7]` — nodes 0–15 a
  pure chain (tail→head, ring diameters 4.7→7.0), 16 + 17–23 head
  appendages. The s14 pairing rule holds: clip-bank nodes 24 = mesh
  max_slot 23 + 1.
- **Clip bank**: 4 containers = clip ids 0–3. From `func_00154040` /
  `func_00154120`: **clip 1 = spawn/emerge** (90 f; init writes clip id
  1 to +0x1F0), **clip 0 = crawl/stalk loop** (239 f, in-place),
  **clip 2 = windup** (45 f), **clip 3 = lunge** (120 f, root travel
  42.2 u → 21.27 u/s; export converts to in-place). Baked-palette
  continuity is hemisphere-clean (max basis-vector deviation 0.18 ≈
  fast whip, no sign flips; max node jump 8.7 u/frame in the lunge).
- **Live scale**: `func_00154040` writes **0.5 → actor+0x80** (anim
  scale) — the in-game leech is HALF the authored size (~11 u long).
  The EMDL ships authored-size; the port applies actor scale.

### 5. Shipped + verified

`assets/enemy_crawler.emdl` (port repo, git-ignored): EMD3, 25 palette
slots, 180 verts/284 tris, 4 clips (494 frames @60), 1 texture resolved
from state 01 VRAM via `--p2s`. Verified in the port via a temp scene
copy (office scene + the leech at spawn+offset): `EM_SCENE` +
`EM_CAPTURE` frame-60 BMP shows the textured leech posed in the office;
temp scene removed, default scene untouched (default-scene capture
re-verified after cleanup). The carved crate husk
(`scratch/crawler_mesh_id0D.bin`, git-ignored) exports the same way if
the port wants the disguise prop.

Open: which clip the burst-spawned (vs generator-spawned) children
play first; the AREA11 nest-registry record layout (the s22 0x2C-byte
guess didn't parse cleanly against `D_0024D820[11]` — re-derive from
`func_001551B0` state 2 before relying on it). ~~kind-0xE sibling
(`func_001546C0`) model/anim slots~~ **RESOLVED s33 ("KIND-0xE
COMPANION RESOLVED"): D_0028A490 slot 0x15 = chunk03/f13_id15.bin, a
1-node 96-vert spike mesh — no clip bank, anim mode 0xA.**

## DOOR SCRIPTS DECODED — ftab_0024D880 itemized, all three door scripts listed (2026-06-10, session 23)

Closes the s17/s20/s22 open items "ftab_0024D880 not itemized", "door
scripts D_0024DBC0/DE40/DEC0 not decoded", and the s22 side→script
re-check. Static analysis only (handler .s + local-ELF data dumps).

### Interpreter contract — func_001BA1F0 (CORRECTIONS to s22)

Script records are **0x40 bytes**, not 8 (the PC at blk+0x8 advances by
0x40; handlers read args anywhere in the record). Record word +0x00 =
`op|flags` (opcode = op & 0xFFF), +0x04 = jump target. Flags: bit31
STOP (terminate after this record: script word = -1, +0x4 = 0), bit30
JUMP (PC = rec[+0x4] instead of PC += 0x40), bit29 = force
continue-same-frame after advance. Handler ABI:
`handler(actor, blk = actor+0x1F0, rec)` →
**0 = stay on record (waiting; yield), 1 = advance + yield,
2 = advance + continue this frame, 3 = abort script.**
Opcode 0x18 is additionally hard-coded as the end marker in the
pause/fast-forward variants (gated by spad byte 0x70003B91; blk+0xC =
pause phase). blk fields: +0x0 script-active (>0 = run, -1 = done),
+0x4 per-record cursor state (handlers' "first call" flag), +0x8 PC.

### ftab_0024D880 — the 27-entry opcode table (0x00..0x1A)

Entry 0x0A is a mid-function entry point (func_001B99F0+0x10), which is
why splat shows a raw word there. Sub-command = rec[+0x8] unless noted.

```
op  handler        role (sub-tables below for the door-relevant ones)
00  func_001B8FC0  camera cue: view pos D_008105D0 / target D_008105E0
                   setters + spline (func_001DD980); jtbl_0026E020/E050
01  func_001B94F0  scripted player WALK-TO (func_00182F90 move-to; dir→
                   clip block D_0024D8F0: dirs [0,1,3,2] → walk clips
                   [0x4C,0x4B,0x4E,0x4D]); jtbl_0026E080
02  func_001B9BA0  WAIT rec[+0xC] frames (float; copy to rec[+0x10] on
                   first call, -=1.0/frame, ret 1 when <= 0)
03  func_001B8AB0  camera path/offset via spad vec D_70003600/10
04  func_001B9C10  set actor rotation component / camera-angle globals
                   D_00810354/370-37C; jtbl_0026E0E0 (11 subs)
05  func_001B9CF0  rotate camera angles D_00810354.. toward targets
                   (func_001B12B0 step); jtbl_0026E110
06  func_001BA080  script FLAGS/COUNTERS on D_00810758 (event flags; the
                   door-panel behavior consumes these) / D_008107D8
                   (counters), index rec[+0x14]: sub 0/1 = flag 1/0xFF,
                   2 = wait counter != 0, 3 = counter = rec[+0x18],
                   4 = wait counter == rec[+0x18], 5/6 = ++/--
07  func_001B82D0  MODE CONTROL (enter/exit scripted-sequence state) —
                   see sub-table below; jtbl_0026DFE0
08  func_001B9FF0  rotate the OWNER actor's yaw +0xC4 toward the player
                   (atan2 func_001B1240 + step func_001B12B0, step =
                   rec[+0x24]); ret 1 when aligned (gate rec[+0x8]==1)
09  func_001B99F0  CALL NATIVE: jr rec[+0x4] (function gets the same
                   3 args and returns the script return code)
0A  001B9A00       PLAYER ANIM (mid-entry of 09's body) — sub-table below
0B  func_001B8020  OWNER (door/prop) ANIM + sound — sub-table below
0C  func_001B7D60  music/stream control (D_002821B0/B4/B8/BC,
                   func_001FC9B0); jtbl_0026DFA0
0D  func_001B7B30  cutscene camera placement on D_008101E0 (chase cam
                   func_0018CBD0 at fixed distances, view targets
                   D_008105D0/E0, FOV reset 480 func_001D25F0);
                   jtbl_0026DF70 (9 subs; door uses sub 5 = chase at
                   -20.0, cam mode flags 5/1, hold 0x78)
0E  func_001B7F90  rotate the PLAYER toward the owner actor (step
                   rec[+0x24]; gate rec[+0x8]==1); ret 1 when aligned
0F  func_001B7A30  voice/stream cue trigger: D_00275C78 = rec[+0x14],
                   D_00821058 = 1, wait completion (state machine on
                   blk+0x4); on rec[+0x8]==0 also func_001FAE70(1)
10  func_001B7840  screen fade control (func_001AED80/DB0/DE0/E10
                   family), wait D_0028A9A0; jtbl_0026DF40
11  func_001B7700  set/wait global byte pair D_008106CE/CF
12  func_001B76D0  func_001B1E20(rec[+0x14], rec[+0x18]); ret 1
13  func_001B7670  set/clear the script-pause gate spad 0x70003B91
                   (rec[+0x8]: 1 = set if clear, 0 = clear if set)
14  func_001BAC00  spawn scripted actor (func_001AFA90 alloc, behavior
                   func_001BB0E0, model/clips via D_0028A490)
15  func_001B6FA0  cutscene player-anim/orient compound (anim_clip_init
                   xN + camera angles D_00810374, D_008104A2/A8)
16  func_001B6E40  wait player move-to arrival (func_00182BF0); on
                   arrival spad 0x70003B8D = 3 (door-transit frame)
17  func_001B6D70  SOUND — sub-table below; jtbl_0026DF00
18  func_001B6BF0  script END/teardown — area-change
                   kick (func_001B0C00), rumble clear (func_001B6250),
                   music stop (func_001FBC50/func_001FABB0), player anim
                   reset (+0x40 = D_0028A580, +0x1F2 = 0), camera/FOV/
                   view-dir restore; also the interpreter's end marker
19  func_001B73A0  cutscene anim variant of 0x15 (advance + waits)
1A  func_001B6AE0  screen-wipe/effect pair func_001FD4C0(rec[+0x18]) +
                   func_00119828 (as in op 7 sub 9-C teardown)
```

**op 0x07 MODE CONTROL subs** (jtbl_0026DFE0; spad 0x70003B8D =
gameplay-frame selector, 0x70003B91 = pause gate, 0x70003B92 = "mode
entered" latch, D_008101E4 = HUD/control mode byte):
- 0/1/D: ENTER scripted mode — D_008101E4 = 2, spad3B8D = 2 (sub D: 1),
  zero spad3B84/3B91/D_008106F3; state 1 waits rec[+0x14]/spad3B8F then
  sets spad3B92 = 1, blk+0xC = 1, camera blend func_001D2610(0.0).
- 2/3/7/8: ENTER variant with fade (func_001AEB60(4)) + func_001BA510;
  state 1 re-aims camera at the player (func_001B81D0) for subs 7/8.
- 4: EXIT/RESTORE (the locked-door finisher): restore D_008106F3/
  D_008101E4/D_008106EF=0x50, fade-in func_001AEBA0(4), camera restore
  func_001CA770 + FOV 480 + view-dir reset D_008105F0..FC, clear
  spad3B8D/3B92/3B91 (+ pause-aware variant → ret 3 abort).
- 5: same as 4 but first sets event flag D_00810758[rec[+0x14]] = 0xFF.
- 6: same family, stores D_008107D8[rec[+0x14]] = rec[+0x18] byte first.
- 9-C: AREA-TRANSITION teardown — fade-out func_001AEDE0(4,0), stream
  func_001FD4C0(rec[+0x18]), 2x func_00119828; A/C also spad3B91 = 1.

**op 0x0A PLAYER ANIM subs** (jtbl_0026E0B0; target = player
D_008102B0): 0 = set anim id +0x1F2 = rec[+0x14], rate +0x1F8 =
rec[+0xC] (float), +0x1F4 = 1.0; 4 = sub 0 + bind clip container
+0x40 = D_0028A490[rec[+0x1C]] + mode +0x2F3 = 3; 1 = container +
id + mode 1, +0x1F4 = rec[+0xC], clear flags +0x200; 2 = RESET (id 0,
+0x20C = -1, mode 3, container = D_0028A580); 3 = wait player anim done
(+0x200 & 0x1000); 5 = move-to point in rec[+0x30] (rec[+0x34] -=
11.0); 6 = camera angle reset; 7 = id/rate/+0x1F4 from rec
+0x14/+0xC/+0x10; 8 = player re-init func_001798D0.

**op 0x0B OWNER ANIM subs** (jtbl_0026DFC0; target = script owner):
0 = anim_clip_init(owner, clip rec[+0x14], rate rec[+0xC], 0); 1 = wait
clip-end flag (blk+0xE & 0x1000); 2 = advance (anim_advance_time
rec[+0xC]) until flags & rec[+0x14]; 3 = advance + countdown rec[+0x14]
frames; 4 = bind container D_0028A490[rec[+0x1C]] + clip_init(rec
[+0x14], 0, 0); 5 = bind container + bind pose (bone_init_default_2);
6 = positional sound play_sound(owner pos, 300.0, id rec[+0x18]) +
clip_init(rec[+0x14], rec[+0xC], 0).

**op 0x17 SOUND subs** (jtbl_0026DF00): 0 = play_sound(owner, 300.0,
id rec[+0x18]); 1 = play_sound(radius rec[+0x20] float, id); 2 =
func_001FB9F0(id, 0x1000, 0x1000, 0x1000) raw submit; 3 =
func_001FBC50() stop; 4 = music cue func_001FA790(0, rec[+0x18]); 5 =
func_001FABB0() music stop; 6 = func_001FAE70(rec[+0x18]) cue restart.

### The three door scripts (traced with jumps; 0x40-byte records)

Two of the "camera-cue globals" from s17 are actually **runtime patch
points inside script records**: D_0024DC14/D_0024DCD4 = the op-0x0A
player-anim-id field (+0x14) of the records at 0x24DC00/0x24DCC0, and
D_0024DC54/D_0024DD14 = the op-0x0B door-clip-id field of the records
at 0x24DC40/0x24DD00; D_0024DC8C = the op-0x02 wait count at
0x24DC80+0xC. func_001BBE40 writes them before queuing.

```
D_0024DE40 (OPEN, queued by func_001BBE40 mode 0):
 @24DE40 op07 sub0           enter scripted mode (spad3B8D=2)
 @24DE80 op0D sub5 JMP→24DC00 chase camera -20.0, hold 0x78
 @24DC00 op0A sub0           PLAYER anim id [0x45 front | 0x43 back],
                             rate 1.0 (patched via D_0024DC14)
 @24DC40 op0B sub6           DOOR sound (D_0024DB80 pair, patched into
                             rec+0x18 by func_001BBD60) + DOOR clip
                             [2 front | 0 back] (patched via D_0024DC54)
 @24DC80 op02 STOP           wait [90.0 front | 70.0 back] frames, end

D_0024DEC0 (LOCKED TRY, queued by func_001BBE40 mode 1):
 @24DEC0 op07 sub2           enter scripted mode (fade variant)
 @24DF00 op09 → func_001BBBF0 native: place locked-look camera (view
                             target/pos from door pos ± 8/10/12/13
                             along door yaw and camera yaw D_00810374)
 @24DF40 op02 JMP→24DCC0     wait 0 (1 frame), jump to back chain
 @24DCC0 op0A sub0           PLAYER anim id [0x46 front | 0x44 back]
                             rate 1.0 (patched via D_0024DCD4)
 @24DD00 op0B sub0           DOOR clip [3 front | 1 back], no sound
                             (patched via D_0024DD14)
 @24DD40 op02                wait 60.0 frames
 @24DD80 op17 sub0           positional sound id 0x3F2 (locked rattle)
 @24DDC0 op09 → func_001BBAE0 native: locked-door VOICE OVER — music
                             duck D_002821B0=2/B4=1, VO line selected by
                             door link low 6 bits via jtbl_0026E1A0
 @24DE00 op0B sub1 STOP      wait door clip end, end script

D_0024DBC0 (LOCKED FINISH, queued by door sub-state 1 when the re-close
clip completes):
 @24DBC0 op07 sub4 STOP      exit scripted mode / restore camera+control
```

**s22 side→script RESOLVED (correction):** the script choice is
**unlocked vs locked** — `func_001BBE40(door, blk, mode)` queues
D_0024DE40 when mode = 0 (unlock-bit OK) and D_0024DEC0 when mode = 1
(locked attempt). The SIDE (front = player within pi/2 of door yaw →
latch +0x2E = 0; else back = 1) only selects which values are patched
into the shared records: player anim 0x45/0x43 (open) or 0x46/0x44
(locked), door clip 2/0 (open) or 3/1 (locked), wait 90/70 frames, and
the player yaw snap (front: door_yaw + pi; back: door_yaw). Both sides
run the same script bytes.

**Door clip indices** (door model's own container, bound at INIT):
0 = open toward back, 2 = open toward front, 1/3 = locked-jiggle
back/front variants. The OPENING pump func_001BC0E0 advances whichever
clip the script started.

**Door sounds — D_0024DB80** (closes "door sound-id mapping open"):
a halfword PAIR table `[front_id, back_id]` indexed by the door LINK
halfword's HIGH byte (link +0x56 >> 8 = sound-set selector; ids
0x3FB..0x40E, the door family in the s23 sound map).
`func_001BBD60(door, rec)` patches `rec[+0x18] = pair[side]` into the
op-0x0B sub-6 record before the script runs; `func_001BBD20(door,
side)` is the direct variant (tail-jumps play_sound at radius 300).
So the door LINK halfword fully decodes as: bits 0-5 locked-VO line
selector (func_001BBAE0), bit 6/7 door scale 1.5x/2.0x (s17), bits
8-15 sound-set index into D_0024DB80.

s26 dump of the table (13 used pairs, selectors 0..12): [0x3FB,0x3FB]
[0x3FC,0x3FC] [0x3FD,0x3FE] [0x3FF,0x400] [0x401,0x402] [0x403,0x404]
[0x405,0x405] [0x407,0x407] [0x409,0x409] [0x40A,0x40A] [0x40B,0x40B]
[0x40C,0x40C] [0x40D,0x40E]. OFFICE: both door placements (AREA02
state-1 records [5]/[6]) carry link 0x0200/0x0280 → selector 2 →
**front 0x3FD / back 0x3FE** (area 2.1 bank chunk04.n0: snd_0522 @
24572 Hz / snd_0523 @ 27581 Hz). `tools/gen_sfx_registry.py` recomputes
this from the user's local ELF + overlay and feeds the port (scene.txt
`doorsfx` line + sfx.txt registry).

**Related scripts in the same region** (not door-specific, recorded for
later): D_0024D900/D_0024D940... (op 7 / op 0D / op 17 / op 09 →
func_001BB400 / op 01 walk-to 35.0 / op 09 → func_001BB310 / op 02 wait
40 / op 09 → func_001BBAE0 / op 7 sub 4 STOP) — this is the door-PANEL
use sequence (the func_001BD9F0 class); its native calls place the
panel camera and arm the partner door.

### Port contract (door sequence, script-accurate)

Implement the interpreter (0x40-byte records, return-code semantics
above) + opcodes {2, 7 subs 0/2/4, 9, A sub 0, B subs 0/1/6, D sub 5,
17 sub 0} and the two native callbacks, and the entire door experience
(camera, player walk-through, anims, sounds, locked VO) reproduces from
the original script bytes at 0x24DBC0-0x24DF80. Patch points: player
anim id, door clip id, wait count, sound id — all written by
func_001BBE40/func_001BBD60 before queuing.

## ANIM ID MAPPING — id → clip container resolution + aim/door/reload clip ids (2026-06-10, session 23)

Closes "how does anim id 0x110/0x33 pick a clip in the 455-clip
library". Short answer: **the anim id IS the container index** — there
is no remap table; D_00248C90 is a per-id PROPERTY table, not a remap.

### Resolution chain (player, and any actor with anim mode 0/3/4)

1. An anim REQUEST writes the id halfword to actor+0x1F2 (weapon code
   via the arbiters func_001749A0/anim_clip_arbiter@0x1749F0, scripts
   via op 0x0A, defaults via func_00182D70) and the rate float to
   +0x1F8.
2. The per-frame commit `func_00183090` (modes 1/3: bind-pose via
   bone_init_default_2 instead): if +0x1F2 != +0x20C (current id),
   copy it and call `anim_clip_init(actor, id, rate +0x1F8, 0.0)`.
3. `anim_clip_init` stores (id | 0x8000) at +0x2C and resolves the clip
   via `anim_clip_resolve(container = actor+0x40, id)` →
   `func_001C6120`:
   **`clip = lib + (((u32 *)lib)[1 + (id & 0x7FFF)] & ~3)`**
   — the container file's leading u32 table maps index → byte offset
   (low 2 bits are flag bits). Same accessor the model library uses
   (s22 crawler rebind).
4. The container pointer +0x40 comes from the global 67-slot
   record-pointer array `D_0028A490[slot]` (slot = source-file id,
   s22b). The player default `D_0028A580` **is** `D_0028A490[0x3C]`
   (0x28A490 + 4*0x3C = 0x28A580) = `chunk28/f01_id3c.bin`, the
   455-container clip library → **player anim id = container index in
   that file, 0..0x1C6.**

So: reload 0x33 = container 51, draw 0x110 = container 272, holster
0x111 = 273, ~~fire 0x31/0x32/0x34/0x35 = 49/50/52/53~~ **(CORRECTED
s25, "FIRE ANIM MECHANISM": 0x31/0x32/0x34/0x35 are armed-stance ACTION
CODES at +0x1F0, never clip requests — containers 49/50/52/53 are
unrelated locomotion-class clips; the fire recoil is a fire-counter
replay of the aim-ladder clip)**, door-open
front/back 0x45/0x43 = 69/67, door-locked front/back 0x46/0x44 = 70/68,
scripted walk set 0x4B-0x4E = 75-78, basic locomotion 1/2/3.

### D_00248C90 — the per-anim-id PROPERTY table (455 rows x 12 bytes)

Row = `{s16 mode, s16 frameA, s16 frameB, s16 pad, f32 rate_scale}`:
- **mode** (+0x0): consumed by the actor spine func_0015BCF0 (and
  func_00182DF0): nonzero → evaluator anim_eval_skeleton (root-motion-
  aware), zero → func_001C68C0 (generic in-place). Door interaction ids
  0x43-0x46 are mode 0 (the MOVE-TO walks the player; the clip is
  evaluated in place). Actor anim-mode byte +0x2F3 overrides: 3/4 →
  func_001C68C0, other nonzero → func_001C6960.
- **frameA/frameB** (+0x2/+0x4): FOOTSTEP trigger frames — per-frame
  func_00187350 fires func_00182430/func_00187EE0 (step sound + decal)
  when the clip time +0x3C crosses them (walk id 1: frames 72/21; run
  id 2: 26/3; scripted walks 0x4C: 135/24, 0x4D: 41/9, 0x4E: 20/6).
  Zero = no triggers (all aim/door/weapon ids).
- **rate_scale** (+0x8): copied to actor+0x34 every frame by
  func_0015BA50 — per-id playback speed (1.0 almost everywhere; the
  draw anim 0x110 runs at 1.4).

### Aim poses (the "aim idle / aim walk" question)

There are NO separate aim-idle/aim-walk clip families. The aim pose is
a per-sub-weapon POSE CLIP selected from two stance tables (s22's
"stance table" pinned):
- `D_00248B88[player+0x275]` (halfword entries; +0x275 = current
  sub-weapon 0..5, +8 for the second weapon bank) — used when the
  player locomotion sub-state +0x05 is 0x1D/0x1E:
  sub-weapon 0..5 → clips 0x112, 0x11C, 0x11C, 0x127, 0x131, 0x13B;
  bank 8..13 → 0x11B, 0x125, 0x126, 0x130, 0x13A, 0x144;
  rows 20-28 → 0x18A..0x192, rows 36-39 → 0x194..0x197.
- `D_00248C68[player+0x275]` — every other aiming state:
  0..5 → 0x18A, 0x194, 0x194, 0x19F, 0x1A9, 0x1B3; 8..13 → 0x193,
  0x19D, 0x19E, 0x1A8, 0x1B2, 0x1BC.
The +9/+10 spacing between consecutive ids and the second bank = base
family + 9 says each weapon owns a LADDER of consecutive pose
containers (aim-pitch steps blended by the aim pitch +0x278); all have
property mode 1, no footstep frames, rate 1.0. Selection code:
func_0016FCF0/func_001703E0/func_001729A0/func_00173000 (the four
weapon-mode tops) → func_001749A0 arbiter → anim_eval_skeleton.

### Port rule

`clip_for(actor) = container_file[offset_table[anim_id]]` where
container_file is the actor's bound library (player:
chunk28/f01_id3c). Apply rate_scale from the property table, fire
footstep events at frameA/frameB, and pick the evaluator by property
mode (0 = in-place). ~~The exporter's container indices already match
anim ids 1:1 — `export_native.py --anim chunk28/f01_id3c.bin --clip
0x110` IS the draw animation.~~ **WRONG — corrected 2026-06-11 s45:
the historic scan enumeration skips 4 non-sentinel containers and is
shifted by up to +3 above id 53 (the door "open" exports were the
LOCKED tries). export_native now resolves `--clip/--clips` through
the file's leading directory (anim_directory), after which the claim
holds. See "DOOR USE SCAN + STAGING MATH ... ANIM-ID DIRECTORY FIX".**

## LASER SIGHT DECODED — beam rule, colors, dot, and the aim camera (2026-06-10, session 23)

Live PCSX2 aim capture (R1 hold, AREA02 storage room) + native disasm of
the gun actor's per-frame laser drawers. The s22 weapon write-up's
"beam/flash drawer func_001854E0 or func_00185760 by +0x318" is now
fully characterized; the anim gate is the AIM-POSE pair, so the laser
runs the whole time the player aims, not only during fire frames.

### Reference (live capture)

- Holding R1 draws the rifle into the shouldered aim pose and the camera
  cuts to a close over-shoulder AIM camera: camera struct +0x06 mode
  byte goes 0 → **1** (handler `func_00197D20`, smooth table), top mode
  +0x04 stays 0 (NOT the scope cam 3); +0x8C target-height offset read
  **2.0** live in this area (the documented default is 6.0).
- The laser: a **thin red beam from the rifle muzzle to the world hit
  point**, visibly dashed/shimmering along its length, terminated by a
  **bright soft red glow dot** several times wider than the beam. On the
  bright wall at point-blank only the dot flare is prominent; in dark
  rooms the beam reads clearly (additive).
- Live gun actor (ptr `D_008102D0` = 0x007a8540 this session): muzzle
  +0xA0 = (106.79, 15.22, −292.80), aim dir +0xC0 = (−0.597, 0.001,
  −0.802) — refreshed per frame from the hand matrix exactly as s22
  documented. Player +0x05 = 0x1D, major +0x06 = 2, +0x318 = 0.

### Selection (func_00188630, the SPR4 gun tick)

Gated on the player anim pair (D_008104A0/A1 = player +0x1F0/+0x1F1):

```
anim ∈ {0x31, 0x34} (aim-pose ladder) && phase==1 && D_008105A2 != 0:
    +0x318 == 0 → func_00185760 (DOT + BEAM)   ← the normal aim laser
    else        → func_001854E0 (DOT only)
anim ∈ {0x32, 0x35} (the other stance pair) && phase==1:
    +0x318 == 0 → func_001854E0 (DOT only)
    else        → func_00185760 (DOT + BEAM)
```

### The raycast (both drawers, identical head)

`spad38B0 = muzzle (gun+0xA0)`; `spad38A0 = muzzle + dir(gun+0xC0)*260.0`;
**`func_0019A570(muzzle, end, mode 7, mask 0x20)` — the exact bullet
query.** On hit: endpoint = spad 0x700031B0 hit point, and if the hit
ACTOR (*0x700031D4) passes `func_00183AC0` its **+0x0A byte is set to
0x80 — a "laser on me" tag** (enemy AI input). The clipped endpoint is
stored at gun+0x200 (s0=gun+0x1F0, s0+0x10) with s0+0x20 = 1 (laser-hit
flag); gun+0x1F0 itself is the beam-start vec4. On miss the laser still
draws to the full 260-unit endpoint. func_001854E0 additionally writes
s0+0x24 = intensity: 1.0 for len ≤ 20, then linear (240−(len−20))/240
to 0 at 260 (a distance fade consumed elsewhere).

### The DOT (func_001CD520 billboard at the endpoint)

`func_001CD520(0, 2, endpoint, 0x20045BA5_154222DC, w=3.0, h=3.0, 2.0,
RGBA)` — additive billboard sprite:
- func_001854E0: R = 0x40 + (rand>>15 & 0x1F), G = B = 0, A = 0x80
- func_00185760 unlocked: R = 0x50 + flicker, G = B = 0, A = 0x80; size 3.0
- func_00185760 LOCKED (D_008106E0 nonzero): R/G/B = 0x70/0x40/0x20 +
  flicker, A = 0x80, size **5.0** (warm/orange dot when locked on)
(GS color scale: 0x80 = 1.0 → unlocked dot red ≈ 0.63..0.86.)

### The BEAM (func_00185760 tail → func_001E2BA0)

`func_001E2BA0(start=gun+0x1F0, end=spad38A0, colorf4, 260.0)` with
base color **(0.7, 0, 0, 1.0)** unlocked / **(1.0, 0.6, 0.2, 1.0)**
locked. Inside (728 B, fully read):

- the beam is **32 consecutive GS LINE segments** start → end
  (step = delta/32), drawn via `func_001E2800(2, seg, colorPrev,
  colorCur)` with both ends vclip-tested against the current clip
  matrix (func_001CD370(2));
- per-segment color = base × **sin-wave flicker**: phase starts RANDOM
  each frame (rand/2^31 × 2π) and advances **(0.1 × len)/4.0 rad per
  segment**; the float→GS-int conversion clamps negative sine to 0, so
  roughly half the segments go dark → the dashed shimmer;
- the previous-vertex color register starts ZERO, so segment 1 fades up
  from black; colors are per-vertex (gradient along each segment).

### Port rule (implemented in extermination-port s23)

Per AIM frame: one bullet-identical segment query muzzle → muzzle +
dir·260; clip at the hit; draw 32 additive, depth-tested (write-off)
segments with per-vertex color (0.7,0,0)×max(sin(φ),0), φ0 random,
Δφ = 0.025·len; 3.0-unit additive dot at the endpoint with
R = (0x50+rand5)/0x80. Beam width: GS LINE = 1 screen pixel at 512×448
(the port uses a ~0.12-unit axial-billboard quad). Aim camera: target
height offset = camera struct +0x8C (default 6.0; AREA02 live 2.0)
while the player is in the armed stance.

## GIB SET — the burst-death model entries of the chunk27 library, identified + exported (2026-06-10, session 24)

Follow-up to "CRAWLER RESOLVED": the crawler's damage-kill death REBINDS
the actor's model record (+0x44) to `D_0028A56C` library entry **0x22 or
0x29** (`func_001C6120` + `func_001CA6E0`). Surveying
`extract/chunk27/f01_id37.bin` around those ids — geometry stats +
TEX0-key sharing (`tools/export_props.py` survey, GS-dump-resolved
texels) — identifies the complete burst set. The discriminator is the
texture pairing: each husk's small chunk/shard meshes sample the SAME
TEX0 key as their husk, and nothing else nearby does.

| lib id | geometry | role |
|---|---|---|
| 0x22 | 160 v / 300 t, 14×14 footprint, 2.8 tall | **burst husk A** — the opened crate base (matches the 14×14×14 crate disguise of FINDINGS s23) |
| 0x1C/0x1D/0x1E | 1.5–3 u flat splinters | husk-A shards (share 0x22's 32×16 PSMT4 skin, TBP 0x22F9 — brown crate tones) |
| 0x29 | 100 v / 252 t, 14×14, 8 tall | **burst husk B** (the second rebind target) |
| 0x28 | same mesh family, exactly half size (7×7×4) | husk B at 0.5× (same skin) |
| 0x26/0x27 | ~2 u lumps | husk-B chunks (share 0x29's skin, TBP 0x229B — grey-cyan) |
| 0x1F/0x20/0x21 | same three shard shapes as 0x1C–0x1E | second shard triplet, own skin (TBP 0x22FD, dark brown) — the other variant's splinters |

NOT gibs, despite adjacency: 0x2C/0x2D/0x2E sample the additive
glow-billboard texture (key `0x041695113222E9`, the s7b player-aura
skin) — effect shells; 0x23/0x24 pair on their own 64×64 skin and
0x25/0x2A/0x2B pair elsewhere — unrelated pickups/props.

**Export:** `tools/export_props.py --gibs` (new mode) writes each entry
as a static 1-node EMDL v2 (model-local space, identity palette frame,
normals → baked colors, texels from the office GS dump — all 4 gib
skins resolve from frame1.gs since chunk27 is globally resident) into
`extermination-port/assets/gibs/gib_<id>.emdl` (git-ignored,
disc-derived, 11 files).

**Port (extermination-port s24):** `em_enemy.c` death now distinguishes
the engine's two state-2 arms: a LETHAL HIT (+0x36 nonzero — the
knockback arm, hit source +0x70) launches 3–5 gib instances from the
exported set with the documented knockback shape (hit vector RNG-rotated
by 90°/180°/270° + a flagged ±30° port jitter, 0.052/tick gravity arc,
floor-query landing, ~3 s rest then sink — the no-per-draw-alpha fade
stand-in); the contact/suicide burst (mailbox empty — the engine's
no-knockback arm) and missing gib assets keep the old sink placeholder.
Launch speeds/spin are flagged port constants; the gore particle-FX
pairs (func_001EFD90 ids) and nest-child spawns remain untranslated.

Open: which husk (0x22 vs 0x29) binds to which crawler variant (the
variant byte {6, 0x1E} gates the knockback, but the husk pick inside
func_001551B0 state 2 is not yet pinned); whether the engine ALSO
draws the shard/chunk meshes at burst time (the gore-FX effect ids may
reference them) — check live during a crate burst.

## STATUS SCREEN LAYOUT — full draw-chain decode of the Triangle/Start status screen (2026-06-10, session 25)

Live PCSX2 session (DebugServer + pad injection). Goal was a fidelity
reference for the port's `em_hud`. **Capture method:** a GS dump was NOT
obtainable — the DebugServer protocol has no dump command (verified
against the fork's `DebugServer.cpp` command list), Pine is not running
(so no save states either), and the one-shot GS-dump menu action needs
the GUI. Method used instead: drove the real screen live with pad
injection and decoded the entire draw chain statically (PCSX2 native
disasm), verifying inputs/state/strings/param-blocks from live RAM.
Everything below is from the code + live values, not from pixels; texture
images (icons/title/portrait) remain unresolved without a VRAM capture.

### Coordinate system & draw primitives (the UI module's vocabulary)

- UI canvas is **512x448**, y-down. GS coords: `x_gs = x + 0x700`
  (1792 = 2048−512/2), `y_gs = y/2 + 0x790` (1936 = 2048−224/2; y is
  halved — interlaced field space). Quad/sprite coords are passed as
  12.4 fixed (`float→fixed` via `*16.0` + `func_001281C0`).
- `func_00207F80(1, x0,y0,x1,y1, rgba)` — flat quad by corners (12.4 GS).
- `func_00207E40(1, x,y, w,h, rgba, tex_token:u64)` — textured sprite
  (w/h in canvas px; token = {0x2004xxxx fmt/page word, uv/rect word}).
- `func_001CBA50(1, x,y, glyph_w, glyph_h, str, style*)` — text
  (x,y integer GS-space). `func_001CC1E0` — tall-font variant (10x20).
  `func_001CC170(str)` returns pixel width (used for centering).
- `func_001C5FB0(value, digits, trim)` — number formatter (known);
  output assembled at buffer `0x2862C0` via `0x123168`/`0x122EF0`.
- `func_002082B0(1, block*)` — **annular arc segment** from a 0x60-byte
  param block: `+0x00/+0x04` x,y (GS*16, float), `+0x08/+0x0C` start/end
  angle (degrees, float), `+0x10/+0x14` inner radius, `+0x18/+0x1C`
  outer radius, `+0x20..0x5C` 4x RGBA (float4 each) gradient colors.
- `func_00207D00(1, mode)` — blend mode (0 normal, 1 additive-ish, 3 seen).
- Text styles (8 B records): `0x265510` white `0x80808080`; `0x265518`
  dark gray; `0x265520` dark red (96,8,16) = "INFECTED"; `0x265528` red
  (128,0,0) = health warning; `0x265530` gray (80,80,80) = profile rows;
  `0x265538` blue (0,96,206) = the name (same blue as the markers).

### Screen lifecycle & input (controller `func_0020CDC0`, ctx `0x810130`)

- **Triangle AND Start both open the status screen** (verified: identical
  memory diff — there is NO separate pause menu; this hub IS the menu).
- On open: camera matrix `0x810610` set to **identity** (UI camera — the
  3D scene becomes the status screen's rotating player model), flag
  `0x8106C4=1` (back to 0 on close), `0x810750` bit 0x40 set.
- ctx fields: `+0x01` state (jump table `0x2735B0`: 0 init, 1/2 hub,
  3 page view, 4 return-from-page, 5 close, 6 —), `+0x02` sub-state,
  `+0x10` ENTERED page (0x63 = none/returned), `+0x11` HOVERED page,
  `+0x34/+0x38` health/infection targets, `+0x3C/+0x3E` timers.
- **Count-up animation**: displayed health/infection live in display
  copies `0x810858`/`0x81085C` (floats) stepping ±1.0/frame toward the
  player-actor values (`0x8104D0`/`0x8104D8`).
- **Page hover = left stick** (`func_0020D930` mode 0): deflection > 0.8
  (double cmp via `func_00100130`), atan2 quadrant → `+0x11`:
  right→2, down→1, up→3, left→4 (sound id 5 via `func_001FB9F0(5,...)`
  on every change); resets to 0 when the stick releases (hover, not latch).
- **X enters** the hovered page (works with no hover too → page 0).
  Entering requests a disc-loaded asset module via `func_001FF080(0,id)`:
  page 0→0x1F, 1→0x1E, 2→0x2C, 3→0x24, 4→0x25, (5→0x26); loader task
  `0x1FF0D0`/`0x1FF1E0` reads header at byte offset `id*0x800`, loads
  sections to `0xB00000` (default base). Per-frame page VIEW code is in
  the main ELF: page 0→`func_0020EE50`, 1→`func_0020F950`,
  2→`func_00211970` (reads SPR4 reserve `0x810CB4` — equipment-flavored),
  3→`func_00214020`, 4/5→`func_002072C0`. Verified live: entered page 2,
  exited back to the hub (`+0x10` ← 0x63).
- **Exit to gameplay**: internal pad edge word `0x810E74` & `0x0810`
  (internal swapped layout; held word at `0x810E7A`, Triangle=0x0020
  there). Triangle closes from the hub.

### Main (hub) page — element inventory (positions in the 512x448 canvas)

Master drawer `func_00209DF0(ctx)`:

1. **Four page-tab panels** around the selector (textured strips via
   `func_00208750(0x10, verts, tex)`, tex `0x265540`, highlight variant
   `0x265570` for the hovered page).
2. **Page-selector diamond**, center (432,320): markers bottom
   (432,376)=p1, right (476,320)=p2, top (432,264)=p3, left (388,320)=p4.
   Each marker = white fading disc r0–16 (block `0x265270`) + ring r10–12
   (`0x2652D0`) + ring r14–16 (`0x265330`); ring colors alternate
   (0,128,255,128)/(0,64,64,128) **blue**, hovered → (0,240,0,128)/
   (0,200,0,128) **green**. Around the center: spinning double ring
   r50–56 / r56–62 (blocks `0x2651B0`/`0x265210`), ~88° cyan
   (41,159,189,128)↔transparent gradient arcs advancing +90°/update;
   plus a 16-particle sparkle emitter at (432,272) (`func_0020AC70`,
   particle array `0x821300`, tex/data `0x273580`).
3. **Title art**: textured 128x64 quad at (16,0) (token `..._9D421E50`).
4. **Profile block**: "DENNIS RILEY" 12 px blue at (16,56); gray 10 px
   rows "BIRTHDAY     :10.25.1981" (24,74), "HEIGHT/WEIGHT:5'11"/154lbs"
   (24,86), "NATIONALITY  :USA" (24,98); one small blue slant tick
   (4x6) at x=18 beside each row (y 78/90/102). Strings live at
   `0x273dc0..`; the label pointer table is `0x267290`:
   [INFECTED, INFECTION, HEALTH, BATTERY, SPR4, name, birthday, h/w,
   nationality]. (Nearby: "INFECTION 50+%/70+%/90+%" at `0x273d50..` —
   graded warnings used elsewhere.)
5. **Portrait**: textured 128x128 quad at (0,320) (token `..._DD421D40`);
   **help panel**: translucent gray (64,64,64,0x40) rect
   (128,336)-(384,432); per-page help-text line ids are written to
   `0x2821B4`/`0x2821B8`.
6. **Four 32x32 icons** (white, textured): (11,304), (484,304), (417,10),
   (417,406) — identity unresolved (tokens `..2186/..21F0/..2192/..21F4`).
7. **HEALTH** (`func_00208AD0(ctx, cx=208, cy=196)`):
   - label "HEALTH" 12 px white CENTERED on x=208 at y=118 (width via
     `func_001CC170`); 8x8 blue marker just left of the label (y 122).
   - **circular ring gauge** centered (208,197):
     - background full ring r36–56 (block `0x2653F0`, angles 180→540°),
       colors: normal (192,224,0,128)↔(224,128,24,128) yellow-green↔
       orange; health ≤ 35 → (160,0,0,128)↔(192,0,0,128) red;
     - health fill arc r24–56 (block `0x265390`) **light blue**
       (0,153,255,128), sweep = 360°·health/100 starting at 180°;
     - rotating 120° highlight (two 60° arcs r36–56, blocks
       `0x265450`/`0x2654B0`, transparent↔(80,80,80) gradient), advances
       12° per 2 frames → full revolution every 1 s; drawn in blend mode 1;
     - the rotating 3D player model renders inside the ring (UI camera).
   - value text 16 px at y=262: value at x=166, "/" at 202, max at 214 —
     "075 / 100". Reads `0x810858`; red style when health ≤ 60 or when
     flag `0x8104E4` is set; that same flag swaps the max string "100" →
     "60" (`0x273558`/`0x273560`) — infection-reduced max health.
8. **INFECTION** (text only — there is NO infection bar): reads
   `0x81085C`; if == 100 → "INFECTED" tall-font (10x20) dark red at
   (290,260); else label "INFECTION" 10x20 at (296,260) + value
   ("60" + "%" from `0x273570`) 16 px white at (296,288).
9. **BATTERY** (`func_00209280(ctx, 16, 118, tex_pair, mode=0)`), all
   gated on `0x810C7F != 0`:
   - 8x8 blue marker (0,96,206,128) at (16,120); "BATTERY" 12 px white
     at (28,118);
   - "04/06" 16 px white at (32,170): current `0x810CB2>>1` (2 digits) +
     "/" + max `0x810CB7>>1`;
   - **segment bar**: squares of 8x8 (mode 0), one per internal
     HALF-unit, 12 per row (additional rows wrap below for capacity
     > 12), drawn right-to-left from right edge x=104 at y=134, even
     squares x-staggered −1 px; per-square color steps along
     (163,54,160,128) magenta → (255,230,52,128) yellow in 1/12
     increments (gradient computed in scratchpad `0x700038A0`).
     NOTE: code says square count = half-units (8 squares in the test
     save); the s21 eyeball note "shrank to 2" when current=4 conflicts
     (code says 4) — needs a pixel capture to settle.
   - mode 1 variant (no marker/label, 12x12 squares, text at +0x46/+0x50)
     is used by `func_0020AE40` at (150,180) together with a HEALTH block
     at (438,110) — a different screen (page view/device UI) reusing the
     same widgets.
10. **SPR4 / ammo** (`func_00209860(ctx, 16, 190)`):
    - 8x8 blue marker at (16,192); "SPR4" 12 px white at (28,190);
    - bullet icon 24x24 (token `..2196`) at (16,262); reserve count
      (`0x810CB4`, 4 digits trimmed) 16 px white at (42,266);
    - **the magazine count is NOT shown anywhere on this screen**;
    - optional SECONDARY weapon row at y=286 (icon 24x24 at x=16, count
      16 px at ~42,290): keyed on `0x810CA4`==2 → count `0x810CB0`
      (icon `..21C2`); else switch `0x810CA6`: 1→`0x810CA8` (`..21A2`),
      2,3→`0x810CAA` (`..21A6`), 4→ fuel percent = `0x810CAE` +
      100·`0x810CAC` with "%" (`..21B2`), 0→ row hidden (test save: hidden).

### Port comparison — `extermination-port/src/game/em_hud.c` vs the real screen

The port's HUD is an honest placeholder (flagged as such in `em_hud.h`),
but nearly every visual decision differs from the real thing:

| aspect | port (`em_hud.c`) | real status screen |
|---|---|---|
| canvas | 640x448 | **512x448** (GS 0x700/0x790 offsets) |
| layout | corner-anchored HUD bars | composed page: profile col TL, ring gauge center, battery/ammo blocks left, infection text right, pager diamond right, portrait+help panel bottom |
| health | 180x10 linear bar, green→red lerp | **circular ring gauge** r24–56 @ (208,197): light-blue fill arc over yellow-green/orange ring, red ring ≤35, rotating 1 s highlight wedge, "075 / 100" text (red ≤60), max swaps to "/60" under flag `0x8104E4` |
| infection | 180x7 purple bar | **text only** — "INFECTION" + "NN%"; "INFECTED" dark red at 100; no bar exists |
| ammo | mag tick marks + reserve bar /240 | **no magazine display at all**; bullet icon + reserve number as text; optional secondary-weapon row; 240 is a port invention |
| battery | teal segments, count = display units | "04/06" text + 8x8 squares = **half-units** (8 squares for "04"), 12/row, magenta→yellow gradient, right-to-left, gated by `0x810C7F` |
| toggle | Triangle only | Triangle **or Start**; open/close animation states; count-up animation of displayed values |
| scene | world keeps rendering, dimmed | camera swaps to **identity UI camera** — the scene IS the status screen (rotating player model); not a dim over gameplay |
| text | none (no font) | white/gray/blue/red styled text everywhere; glyph cells 10x20 (tall), 12x12 (labels), 16x16 (numbers) |
| extras | — | pager diamond (stick hover + X), spinning cyan rings + sparkles, profile bio block, portrait, help panel, page sub-screens |

`em_hud.h` staleness: "battery storage address not yet located" — located
since s21 (`0x810CB2`/`0x810CB7`); the "gameplay keeps running underneath"
assumption is contradicted by the UI-camera swap (world view is replaced;
whether the world SIMULATION also halts is still unverified).

**Concrete fix list for the port (ordered):**
1. Switch the overlay canvas to 512x448 (or remap logical coords).
2. Move blocks to the real anchors: battery (16,118), SPR4 (16,190),
   health center (208,196), infection text (296,260), profile (16,56)+
   (24,74/86/98), portrait (0,320), help panel (128,336)-(384,432),
   pager diamond center (432,320).
3. Add an annular-arc primitive to em_gfx (params = the 0x60 block:
   center, angle pair, r_inner/r_outer, 4-color gradient) and rebuild
   HEALTH as the ring gauge (radii/angles/colors above), including the
   ≤35 red state and the 12°/2-frame highlight sweep.
4. Battery: squares = current half-units (not display units), 8 px,
   12/row, right-to-left from x=104 y=134, magenta→yellow 1/12 gradient
   steps, −1 px stagger on even squares; render "04/06" once a font lands.
5. Ammo: drop the magazine ticks from the status screen (keep mag state
   for a future in-page view); show reserve as icon + number at
   (16,262)/(42,266); add the secondary-row dispatch when weapons land.
6. Infection: remove the purple bar; text-only ("INFECTION NN%" /
   "INFECTED"), tall font; red INFECTED state at 100.
7. Open on Start as well as Triangle; add the ±1/frame count-up of
   displayed health/infection (mirror `0x810858`/`0x81085C` semantics).
8. Render the player model at the ring center with an identity UI camera
   instead of dimming the live scene (and stop simulating the world while
   open, pending verification of the engine's behavior).
9. Font renderer is the single biggest fidelity unlock (labels, numbers,
   profile text are the screen's core); glyph cell sizes 10x20/12/16.
10. Refresh `em_hud.h` notes: battery addresses, canvas size, camera
    behavior, and the EmPlayerStatus battery fields = half-units.

### Tooling gotchas (this session)

- `pcsx2_step` leaves TEMPORARY breakpoints at the stepped addresses;
  they re-trigger on every `continue` (the VM "freezes" advancing ~1
  cycle per resume). `pcsx2_clear_all_breakpoints` fixes it. Also
  `pcsx2_read_registers` pauses the VM as a side effect — re-`continue`
  after using it. Both together mimicked a soft-lock convincingly.
- Status-screen page exploration is save-safe: hover/enter/exit changed
  no inventory or player state (verified byte-identical `0x810C60..`
  block, health/infection floats after full restore).

### Open items

- Page sub-screen content layouts (`func_0020EE50`/`0x20F950`/`0x211970`/
  `0x214020`/`0x2072C0`) — each needs its own decode pass; page names
  unconfirmed (page 2 reads the ammo reserve → equipment-flavored).
- Icon/title/portrait texture images need a VRAM capture taken while the
  screen is open (GS dump via the GUI menu action, or a software-renderer
  save state once Pine is available).
- Battery-bar square count: code (half-units) vs the s21 eyeball note
  ("2 segments at current=4") — settle with pixels.
- Does the world simulation pause while the screen is open? (UI camera
  swap is confirmed; actor updates not yet checked.)

## FIRE ANIM MECHANISM — no fire clips; the recoil is a counter-driven replay of the aim-ladder clip (2026-06-10, session 25)

Static walk of the per-shot anim path (fire SM `func_00170A60` + stance
helper `func_0016F600` + publisher `bone_matrix_publish`/`anim_matrix_
dispatch` asm), property-table extraction, and clip-library inspection.
**CORRECTS the s23 "ANIM ID MAPPING" line "fire 0x31/0x32/0x34/0x35 =
containers 49/50/52/53": those four values are never requested as clip
ids. The engine has NO separate fire clips.**

### 0x31/0x32/0x34/0x35 are armed-stance ACTION CODES (+0x1F0)

`func_0016F600` (and the stance tops) set player `+0x1F0` 1:1 from the
stance byte `+0x05` on stance entry/switch:

```
+0x05 = 0x1D -> +0x1F0 = 0x31      0x1E -> 0x32
        0x1F ->          0x34      0x20 -> 0x35
```

The code is held for the WHOLE stance (this is why the s23 live laser
capture read +0x1F0 = 0x31 during plain aiming). It is fire-mode
INDEPENDENT — the semi/burst/auto families never write it; their
per-shot blocks only READ it to pick the fire sound (0x164 for
0x31/0x34, 0x165 for 0x32/0x35 — the s22 pairing) and the laser
drawers/dodge AI key on it. The reload code 0x33 happens to coincide
with the real reload clip id 0x33 because the top's state 3 requests
that clip separately — the coincidence misled s23 into mapping the fire
codes to containers 49/50/52/53.

### What containers 49/50/52/53 actually are (library-verified)

Exported from chunk28/f01_id3c on the 21-node player rig: 49 = 110
frames with 17.2 u root travel (in-place natural speed 9.48 u/s), 50 =
79 fr / 8.5 u travel, 52 = 33 fr, 53 = 60 fr — long locomotion-class
clips, NOT recoil snaps (true weapon clips all have zero root travel).
Identity unknown; NOT appended to the port's player.emdl.

### The real recoil: bone_matrix_publish re-seeds the aim clip with the fire counter

`bone_matrix_publish` (func_00179BC0), called per slot from
`anim_matrix_dispatch` (func_0017A130) with the aim-ladder clip id from
`anim_slot_index` (func_0017A0B0):

```
if (+0x1F0 in {0x31, 0x34})
    anim_clip_arbiter(actor, ladder_clip, f12=0, f13=(float)lh(+0x276))
else
    anim_clip_arbiter(actor, ladder_clip, f12=0, f13=1.0)
```

`anim_clip_init`'s f13 is the SAMPLE TIME passed to anim_sample_bones —
so in stances 0x1D/0x1F the committed aim-pose clip is evaluated at
frame = the FIRE COUNTER (+0x276): 0 written at every shot (and at
stance entry), +2/frame, shot gate at >= interval 12. Each shot
therefore replays the aim clip from frame 0 at 2 clip-frames/tick; the
RECOIL SNAP IS BAKED INTO THE CLIP'S FRONT FRAMES. Measured on clip
0x112 (SPR4 stance-A ladder base, 25 fr): max adjacent-frame bone
rotation 4.0 deg/frame over frames 0-4, decaying 1.9 -> 1.4 -> 1.2 to a
0.9 deg/frame static tail — a snap settling into the rest pose. 25 fr @
2/tick = 12.5 ticks to settle; at the 6-tick full-auto cadence the
counter resets at frame 12, so sustained fire only ever replays the
front half (engine-inherent overlap). Stances 0x1E/0x20 (codes
0x32/0x35) sample at constant 1.0 — no counter recoil.

### Aim-ladder tables (pinned; supersedes the halfword-table reading)

`anim_slot_index`: stance {0x1D,0x1E} -> `D_00248B70[sub]` (POINTER per
sub-weapon), else `D_00248C50[sub]`; the pointed-to halfword row is a
9-step pitch/yaw ladder (dispatch samples steps 0..2 by pitch +0x278
and 4/7/8 by yaw +0x27C, blending two buffers via func_00179CA0):

```
D_00248B70: sub 0 -> 0x112..0x11A   1,2 -> 0x11C..0x124   3 -> 0x127..0x12F
            4 -> 0x131..0x139       5 -> 0x13B..0x143
D_00248C50: sub 0 -> 0x18A..0x192   1,2 -> 0x194..0x19C   3 -> 0x19F..0x1A7
            4 -> 0x1A9..0x1B1       5 -> 0x1B3..0x1BB
```

(The s23 "D_00248B88/D_00248C68 halfword stance tables" used by the
tops' state-1 entry request remain correct — same clip families.)

### D_00248C90 property rows (extracted, weapon ids)

`{s16 mode, s16 frameA, s16 frameB, s16 pad, f32 rate_scale}`:
0x31/0x32/0x33/0x34/0x35/0x111/0x112 all mode 1 (0x35: mode 0), no
footstep frames, **rate 1.0**; 0x110 rate **1.4** (re-confirmed). The
counter-driven recoil path does NOT consume rate_scale — the playhead
step is the counter's +2/frame.

Extraction note: in `elf/SCUS_971.12.elf` the LOAD segment starts at
file offset **0x94** (readelf PHDR), not the boot ELF's 0x300 —
`file_off = vram - 0x100000 + 0x94` for that copy. Verified against six
anchors (walk 72/21, run 26/3, scripted walks 135/24 41/9 20/6, draw
1.4).

### Port translation (extermination-port s25)

`em_game_anim_hold_restart(0x112, 2.0)` per shot — rewinds the
committed aim hold to frame 0 at 2 frames/tick, clamping back into the
hold (em_weapon.c "FIRE RECOIL" block). Laser stays on through the
recoil (the gate is the stance code, held all aim — consistent with the
s23 laser finding). Verified: weapon self-test asserts mid-replay after
shots 1/5 and the clamp before holster; EM_CAPTURE_AIM=2 (aim + one
shot) vs =1 differ by 3.85% of pixels at the mid-recoil frame and are
pixel-identical 22 ticks later (the settle).

## UI FONT — glyph rule, storage, and export (2026-06-10, session 26)

Full decode of the text draw chain flagged in the s25 status-screen audit
(`func_001CBA50` small-font text / `func_001CC1E0` tall-font text). The
headline: **the engine has no VRAM-resident font sheet.** Text streams
1bpp glyphs from EE RAM into a PSMT4 staging strip in GS VRAM and draws
one batched sprite per string segment. Checked the office GS dump at the
strip address — only stale non-text noise (gameplay frames draw no text),
so the EE-RAM bitmaps are the authoritative (and complete) source.

### Storage

Pointer block at `0x0028A490` (filled by the boot asset loader; in the
test states all four point into the disc-module region at `0xB00000`):

| slot | points to | layout |
|---|---|---|
| `[0x0028A490]` | tall font (0xB00000) | 1bpp, **30 B/glyph = 12x20**, 409 slots |
| `[0x0028A494]` | small font (0xB03000) | 1bpp, **32 B/glyph = 16x16**, 256 slots |
| `[0x0028A498]` | string-table file (0xB05000) | used by `func_001FCB90`/`func_001FCBD0` |
| `[0x0028A49C]` | (0xB0B800) | unexamined |

### Glyph rule (func_001CBA50 / func_001CC1E0 / func_001CC8A0)

- `glyph_index = ascii - 0x20`; chars `< 0x20` draw glyph 0 (blank) —
  tall advances 0 for them, small still advances a full cell.
- `'$'` (0x24) remaps to extended glyph **0x89 tall / 0x87 small**.
- Byte offset = `index*30` (tall) / `index*32` (small).
- 1bpp→PSMT4 expansion via the 16-entry u16 LUT at `0x0026E350`:
  `LUT[n]` spreads bit *i* of nibble *n* into texel nibble *i* — within a
  nibble **bit 0 is the leftmost pixel**; nibbles consumed low-then-high.
- Small glyph: 32 bytes linear = 16 rows x 16 px (2 B/row).
- Tall glyph: 3 bytes per TWO 12-px rows (`b0.lo b0.hi b1.lo` = row N,
  `b1.hi b2.lo b2.hi` = row N+1), 10 iterations = 20 rows.

### Draw path

Per character the glyph is expanded and HOST→LOCAL-uploaded
(`func_001CCE80` builds BITBLTBUF/TRXPOS/TRXREG/TRXDIR + IMAGE tag) into
a staging strip at **GS block 0x1B00, DBW 8 (512 texels wide), PSMT4**
(TRXREG 16x16 small / 32x32 tall window). Cells advance horizontally —
fixed 16 texels per small glyph, **proportional advance** per tall glyph
— and at 512 texels (32 small glyphs) the batch flushes as ONE textured
sprite (`func_001CBC20` small / `func_001CC3B0` tall): UV (0,0)..(Σadv,
16|20) texels over the strip, screen w = Σadv (tall draws 1:1; small
GS-scales its 16x16 cells to the caller's `glyph_w`/`glyph_h` — the
status screen passes 12x12 labels, 12x16 numbers). Because the strip is
written left-to-right with overlapping 32-wide windows, each tall glyph
shows exactly its first `advance` ink columns.

Texture state (prebuilt packets `D_00250FC0` style/TEX setup,
`D_002511C0` ALPHA pair): TEX0 = `0x20036CE5E5421B00` → TBP0 0x1B00,
TBW 8, PSMT4, TW 9/TH 7, TCC RGBA, **TFX modulate**, CBP 0x1B67 (CT32,
CSM1, CLD 1); TEX1 = `0x60` → **MMAG/MMIN linear (bilinear)**; ALPHA
`0x44` = standard `(Cs-Cd)*As+Cd` blend (additive `0x68`+FIX 0x80
variant in the same packet, selected per style byte +4..+7; style byte
+7 is a 0..15 sub-pixel y nudge OR'd into the sprite's XYZ).

Tall-font advance table (`func_001CBE10`, keyed by RAW char code before
the `'$'` remap; default **9**): space/`'`(0x60) 5, `!` `'` `I` `i` `l`
0xA1 4, `,` `j` 0x82 0x91 0x92 0xA6 6, `:` `J` 7, `(` `)` `.`→5, `/`
`;` `[` `\` `]` `f` 0x84 0x93 0x94 8, `r` 0x8B 0x9B 9, `M` `W` `m` `w`
12. (`.` is 5, `(`/`)` are 8.) The width helper `func_001CC170` sums
these — and `func_00208AD0` centers the "HEALTH" label with it even
though the label draws in the SMALL font at 12x12 cells, so the drawn
label sits slightly right of true center: a real engine quirk.

### Export & port

`tools/export_font.py` (new; macOS arm64 python3) reads an EE-RAM dump
(`parse_pcsx2_state.py` output), decodes both fonts via the rule above
and writes `font.emfn` — RGBA8 sheet + per-glyph `{u,v,w,h,advance}`
table; format documented in the script header and mirrored by the port
loader (`extermination-port/src/game/em_hud.c`). The port draws text
through a new textured-overlay primitive (`em_gfx_overlay_glyph`,
bilinear+modulate = the strip's GS state); with the asset absent it
falls back to the old placeholder rects (verified byte-identical to the
pre-font build). Verified vs the disasm: 'A'/'0'/'R'/'%' render
correctly from both fonts; status screen shows "HEALTH 075 / 100",
"BATTERY 04/06", "SPR4", "INFECTION 60%" in the real glyphs.

## OFFICE CRATE BLOB — the AREA02 crawler disguise identified, carved, exported (2026-06-10, session 26)

Closes the "CRAWLER RESOLVED" follow-up "also resolvable for the office
area": which intact model the OFFICE (AREA02) placed crawler renders
while idling disguised. Method = the s23 recipe, applied to the office.

### 1. The office model table (the s23 method, AREA02)

- Save states **08 / 09 / resume are all AREA02** (`D_00810700` = 2):
  `*(D_0028A59C)` = `0x015570C0`, count **0x10** (model ids 0x00–0x0F;
  identical table in all three states).
- Content match: the table + entries are byte-identical to
  **`extract/chunk06.n1/f03_id43.bin` at file offset `0x82000`**
  (RAM−file delta `0x14D50C0`; entry 0x0D at file `0xA3900` = RAM
  `0x15789C0`, verified byte-exact). So the per-area model table lives
  INSIDE the level RENDER file, past the region map of FINDINGS s10 —
  f03_id43 carries render regions AND the area's bind-by-param models.

### 2. CORRECTION (s23 §3): the office disguise is NOT the 14u crate

s23 extrapolated "AREA11 (and office AREA02) crawler param 0x0D →
model-table id 0x0D = a 14×14×14 beveled CRATE". The binding (param
0x0D → entry 0x0D) holds, but the office's entry 0x0D is a different
model — **a plain 6×4×5 cardboard BOX** (1 block, 1 node, 10 tris, 8
corner positions; bounds x ±3, y 0..4, z ±2.5, radius 5.59), skinned
with THREE 64×64 PSMT4 textures (TBP0 `0x2DF0`/`0x2D98`/`0x2D88`, CBP
`0x2F71`/`0x2F75`/`0x2F74` — per-face cardboard browns, all resident in
the office GS dump `frame1.gs`). The 14u beveled crate is the AREA11
skin only — "per-area skins", exactly as s23 §2 predicted from the
param table. (Office table survey, for reference: 0x05 = a 20×20
6-block prop sharing the AREA11 crate's size class; 0x07 = a 13-node
34-block character; 0x0B/0x0C are near-duplicates of 0x0D's size.)

### 3. Export + port

- `tools/export_props.py --crate` (new mode): carves a per-area
  model-table entry (`table_entry_offset` = the func_001C6120 lookup:
  `table + (dir[id] & ~3)`), defaults = the office table/entry above,
  and writes a static 1-node EMDL v2 (model-local, identity frame,
  texels from `--gsdump extract/gsdump/frame1.gs`) →
  `extermination-port/assets/enemy_crate.emdl` (20 welded verts, 10
  tris, 3 textures; git-ignored, disc-derived). `--crate-blob` exports
  a pre-carved raw blob instead (e.g. `scratch/crawler_mesh_id0D.bin`,
  the AREA11 14u crate, with `--p2s` state 01 as the texel source) —
  flagged as the non-office stand-in.
- **Port (extermination-port s26):** `em_enemy` gains the CRATE kind —
  `enemy crate x y z yaw` manifest lines; IDLE renders the crate EMDL
  with the documented PROCEDURAL jitter mechanism (deterministic x/z +
  yaw perturbation standing in for the D_002468B0/B4/B8 tables, which
  are not exported — amplitudes flagged); HP 1; a bullet hit or ~10-u
  player proximity (flagged trigger) BURSTS it: husk gibs through the
  s24 launcher + the real worm spawned at the crate position (emerge
  clip 1, yaw toward the player = the engine's leech init yaw).
  `EM_ENEMY_TEST=3` walks the player at a crate 25 u ahead and asserts
  burst-at-proximity, worm-at-crate-position, worm-closes-distance.

Open: the AREA11 14u crate's husk pairing says husk A (0x22, 14×14
footprint) is ITS burst shell — which husk (if either) the engine
rebinds for the office's 6×4×5 box is unpinned (the husk set is the
GLOBAL chunk27 library; check live during an office crate kill);
office entries 0x0B/0x0C (same bounds as 0x0D) are probably the other
office crawler param variants — unverified against placement params.

## OFFICE SUB-STATE-1 PLACEMENT CENSUS — scene.txt enemy emission (2026-06-10, session 27)

`tools/export_level.py` now emits the port's manifest `enemy` lines from
the AREA02 placement tables (s11 record format, s22 behavior census) for
the office scene. The census result reframes "the office's enemies":

### 1. The captured scene's table places NO enemies

The exported office scene is area SUB-STATE 1 (table `0x828170`, the
s8/s9 capture). Its **14 records contain zero enemy-class behaviors**:

| sub-state | table | records | crawlers (fn 0x001551B0) | generators (fn 0x0015A2C0) | misc creature-family | doors | pickups | deferred (cls 0x0B) | fixtures/props |
|---|---|---|---|---|---|---|---|---|---|
| 0 | 0x827830 | 58 | **17** | **8** | 3 | 2 | 0 | 4 | 24 |
| **1 (scene)** | **0x828170** | **14** | **0** | **0** | **0** | **2** | **7** | **0** | **5** |
| 2 | 0x8283D0 | 53 | **17** | **8** | 3 | 2 | 0 | 4 | 19 |

Sub-state 1 = the back-office intro rooms (control panel 0x36 +
companion prop, wall station 0x37, lockers 0x38, battery 0x2C, 2 double
doors, 7 item pickups). The area's crawlers belong to the OTHER story
states of the same floor: sub-states 0 and 2 each place 17 disguised
containers (model 6, **param 0x000D = the s23 cardboard-box model**) on
the main office floor (x −157..483, z −23..168) plus 8 class-0x0D
generator spawn points (kinds 0/1/3/5 indexing `D_00248120`). The two
sets differ only in uid numbering, door-link flags and minor prop
swaps — they are the "before/after" populations of the floor. The misc
creature-family records per table 0/2 are fns `0x1582E0` (class 0x44),
`0x158EC0` (class 0x84), `0x158BD0` (class 8).

CONSISTENT GEOMETRY CHECK: the sub-state-1 render export's bbox is
X[37,120] Z[−305,−143] — every sub-state-0/2 crawler position lies
OUTSIDE the captured scene's geometry volume. The crawler placements
genuinely belong to a different scene of the area, not to this one.

### 2. Emitter + port verification

- `export_level.py` rewrites a marker-delimited `# --- enemies ... ---`
  block in scene.txt: active `enemy crate x y z yaw` lines from the
  scene table's fn-0x1551B0 records (none, per the census), generator /
  creature-family records as comment lines (unimplemented natively),
  and — since the scene table is enemy-free — the 17 sub-state-0
  crawler placements as a documented COMMENTED TOGGLE (`#enemy crate`).
  Idempotent; preserves all foreign manifest lines.
- TOGGLE GATING (verified empirically): manifest enemies load at boot,
  BEFORE the EM_ENEMY_TEST scene-init spawn, and the self-tests assert
  on enemy slot 0 — with the 17 lines active, slot 0 is the (33,0,120)
  crate 313 u away and EM_ENEMY_TEST=1 FAILs (also: 17 > EM_ENEMY_MAX
  16, the last line's spawn is rejected). Hence commented-by-default.
- Captures: default-spawn capture with the toggle ON is BYTE-IDENTICAL
  to the no-enemies default (all crates sit beyond the back-office
  walls, outside the scene volume); a temporary floor-vantage spawn
  (33,0,60) shows the cardboard crate EMDL rendered at the authentic
  (33,0,106..168) table positions, floating in the void where this
  scene has no geometry.
- The faithful default scene therefore HAS no enemies, and the
  byte-identical capture baseline does NOT move. All self-tests pass
  on the default: EM_ENEMY_TEST 1/2/3, EM_DOOR_TEST, EM_MOVE_TEST,
  EM_WEAPON_TEST.

Open: which engine event flips `D_00810701` from 0/1/2 for area 02
(story scripting — would let a future port scene load the populated
floor with its own geometry once sub-state-0's render file region map
is recovered).

## STATUS SCREEN UI TEXTURES — decor sprites resolved + exported; module loader format decoded (2026-06-10, session 26)

Closes the session-25 open item "icon/title/portrait texture images need
a VRAM capture": **no new capture was needed — the hub's decor textures
are RESIDENT in GS VRAM in an ordinary gameplay save state** (the s21
state-01 dump). Decoded straight out of `gs.bin` (local-memory base =
`len - 0x400000 - 84`, the gs_vram.py rule) using the TEX0 tokens inlined
in the hub master drawer `func_00209DF0` (+ `func_00209860` for the
ammo icon). New exporter `tools/export_ui.py` → `ui.emui` for the port.

### The sprite set (all PSMT4, TBW 8, 16-entry CSM1 CT32 CLUTs)

| sprite | TEX0 lo_hi | TBP | CBP | texels | canvas draw |
|---|---|---|---|---|---|
| title "MAIN" | `0x9D421E50_200453A5` | 0x1E50 | 0x229D | 128x64 | (16,0) 1:1 |
| button legend | `0xDD421D40_20045505` | 0x1D40 | 0x22A8 | 128x128 | (0,320) 1:1 |
| page arrow left | `0x55422186_20045EC5` | 0x2186 | 0x22F6 | 32x32 | (11,304) |
| page arrow right | `0x554221F0_20045EC5` | 0x21F0 | 0x22F6 | 32x32 | (484,304) |
| page arrow up | `0x55422192_20045EC5` | 0x2192 | 0x22F6 | 32x32 | (417,10) |
| page arrow down | `0x554221F4_20045EC5` | 0x21F4 | 0x22F6 | 32x32 | (417,406) |
| SPR4 bullet | `0x55422196_20045185` | 0x2196 | 0x228C | 32x32 | (16,262) **24x24** |

- Textures are stored **vertically flipped** (the engine's sprite UVs
  flip V on draw); the exporter un-flips so the sheet is screen-oriented.
- **Identities settled by pixels:** the (16,0) "title art" reads
  **"MAIN"** (the hub page's title — page sub-screens presumably swap
  it); the 128x128 quad at (0,320) the s25 audit provisionally called
  the "portrait" is actually the **button legend** (cross OK / circle
  BACK / triangle EXIT — there is no Dennis portrait on the hub); the
  four unresolved 32x32 icons are **double-chevron page arrows** pointing
  out of each screen edge (left/right/up/down — they pair with the pager
  diamond). The s25 audit's "help panel" is a flat translucent rect
  (no texture). The four icons draw in blend mode 3 (`func_00207D00(1,3)`),
  the title/legend in mode 0.
- All four 32x32 arrows share CLUT 0x22F6; the bullet icon (red shell,
  `func_00209860`) GS-scales its 32x32 texture to a 24x24 sprite.

### Where they come from: INDEX.IDX IS the UI-module package format

Decoding `func_001FF1E0` (the loader task behind `func_001FF080(0,id)`)
shows the "asset module" system reads **INDEX.IDX sector `id` directly**
as a module header (file handle `D_0028A480` = `{LBA 0x9D7D0, size
0x1C000}` = INDEX.IDX; section reads go through `D_0028A488` =
`{0x801D8, 0xEAFC000}` = DATA.DAT). So the status-screen "module ids"
(hub pages 0x1F/0x1E/0x2C/0x24/0x25/0x26, secondary-weapon icon modules
0x32–0x35 from `func_001FEF70`) are **DATA.DAT chunk indices**. Header
fields used by the loader (offsets in the 0x800-byte sector):

```
+0x04 u32 chunk data offset in DATA.DAT     +0x10 u32 count B (resident GS packets)
+0x08 u32 resident size bound               +0x14 u32 resident region offset
+0x0E u16 count A (transient GS packets)    +0x1C u32 count C (pointer-slot entries)
+0x20 (A+B) x {u32 off, u32 size}           then C x {u24 offset, u8 slot}
```

Group-A sections are loaded and DMA'd to VIF1/GS (`func_00200830`) then
discarded; the resident region loads to the asset heap (`D_0028A734`,
boot value 0xB00000) and its leading B sections are ALSO DMA'd (resident
GS packets); the C entries fill the **asset pointer-slot array at
`D_0028A490`** (`slot*4`): the s26 font slots 0–3 come from **chunk 0x00**
(slots 0–5), chunk 0x1B fills slots 0x35/0x37/0x39/0x3A, chunk 0x03
slots 8–52, etc. Every status-screen page chunk (0x1F–0x31) is one
transient GS packet = that page's texture upload; the hub decor set
above is boot-resident (uploaded once, never re-streamed — which is why
a plain gameplay state contains it).

### Export + port (extermination-port s26)

- `tools/export_ui.py` (macOS arm64 python3; reads the user's own local
  `gs.bin` state dump) decodes the 7 sprites + CLUTs, un-flips, packs
  them into one 272x144 RGBA8 sheet with 2-px gutters and writes
  `assets/ui.emui` — header + per-sprite `{sheet u,v,w,h; canvas x,y;
  draw dw,dh}` records + sheet (format documented in the script header;
  the audited canvas anchors TRAVEL WITH the asset). `*.emui` added to
  `.gitignore`.
- Port: `em_gfx` overlay texturing extended to **two slots** (font strip
  + UI decor sheet; `em_gfx_overlay_sprite` mirrors `_glyph`); flush
  order untextured → decor → text keeps the engine's hub layering
  (icons over the diamond rings, text over everything). `em_hud` draws
  the full decor pass when `ui.emui` is present: the 7 sprites at their
  recorded anchors, the pager-diamond markers (white fading disc r0–16 +
  blue gradient rings r10–12/r14–16 at (432,376)/(476,320)/(432,264)/
  (388,320), the live 0x265270/0x2652D0/0x265330 arc-block colors), and
  the profile bio block ("DENNIS RILEY" 12x16 blue, three 10x10 gray
  rows, 4x6 blue ticks — strings from the 0x267290 label table).
  Verified: with `ui.emui` absent the EM_HUD_FORCE and default captures
  are BYTE-IDENTICAL to the pre-decor build; with it present the forced
  capture shows the legible "MAIN" title, legend, arrows, diamond and
  profile rows at the audited anchors.

Open: hub page-tab strips (untextured gradient quads via
`func_00208750`, color records 0x265540/0x265570) and the spinning cyan
double ring + sparkle emitter (animated; cadence unverified) are still
not composed in the port; hover-green marker state needs page-navigation
input; page sub-screen texture sets (chunks 0x1F–0x31) are exportable
with the same recipe once their layouts are decoded.

## AREA02 SUB-STATE 0 GEOMETRY MAPPED — the main floor exported end-to-end (2026-06-10, session 28)

The office area's MAIN-FLOOR story beat (placement table 0x827830, 58
records — s27's "sub-state 0") now has its render geometry, collision,
model table, doors, spawn and textures pinned and exported as
`extermination-port/assets/scene_office0/` (EM_SCENE-loadable, 16 active
crawler crates). Closes the s27 open item "sub-state-0's render file
region map".

### 1. Sub-state -> chunk leaf mapping (the geometry was never in n1)

The s27 prompt assumed the floor hid inside chunk06.n1's files; it does
not. **The area's three story sub-states are the three chunk06 nested
leaves**, pinned by the soundmap's `area_scene_map` ((2,0) -> chunk06.n0,
(2,2) -> chunk06.n2; (2,1) ties broadly but chunk06.n1 is live-pinned as
the captured scene): n0 = sub-state 0, n1 = 1 (back office), n2 = 2.
Render-record survey of every chunk06* file (walk_records, per-section
slot histogram + slot-0 bbox):

| chunk06.n0 file | size | render content (world space, slot 0) |
|---|---|---|
| f00_id42 / f01_id46 | 0x28800 / 0x16000 | none (no records; f01 carries no GS uploads either — roles TBD) |
| f02_id44 | 0x1EA000 | collision world (head; grid section @0x10C800, 1863 verts / 1007 nodes, **no cell-list section** — unlike n1) + **render tail [0x14FCD0,0x1EA000) X[-349,35] Z[-210,201]** (west floor; same two-section shape as n1's id44, s8) + the leaf's two GS texture uploads |
| f03_id43 | 0x13D800 | **whole-file static world [0x10,0x13D750) X[-35,546] Z[-195,185]** (east + center; all slot 0 — NO object regions and NO embedded model table, unlike n1's f03) |
| f04_id72 | 0x22000 | static far-east annex [0x10,0xD1D0) X[520,550]; **+0xD800 = the per-area MODEL TABLE** |
| f05_id41 | 0x14B800 | model-table entries ONLY (object-space; never export whole-file) |

chunk06.n2 (sub-state 2, surveyed not exported): render spans f01_id43
(X[-120,345]) + f02_id42 + f03_id46 + f06_id74 (east, to X 546) +
f00_id44 tail (west) + f07..f12 object/model files; f00_id44 head =
collision; model table @ concat 0x3E9000 (f07_id82+0x12000, count 27).

### 2. The n0 model table — directory across a file boundary, param binds

`func_001C6120`-shape table (u32 count; u32 dir[count]; entry = table +
(dir[i] & ~3)) at **concat offset 0x373800 = f04_id72+0xD800, count 27
(0x00..0x1A)**; the entries run past f04's end into f05_id41 — the leaf's
files load contiguously (the chunk15 rule), so tools must operate on the
byte-concatenation. Single-file scans MISS this table; that is why s28's
first scan found only n1's (fully inside f03_id43 @0x82000).

Binding validation (n1 cross-check): the n1 table's entries land exactly
on the s8/s9 live blob regions — 0x07=corridor door 0x88800, 0x08=panel,
0x09=station, **0x0A=double door 0xA0580**, 0x0B/0x0C/0x0D=pickup blobs
(= pickup PARAMS 0xB/0xC/0xD), 0x0E=battery, 0x0F=lockers. So placed
actors bind mesh by **param** across the board (the s23 creature rule
holds for doors/fixtures/pickups too; the "model" byte is a variant
selector).

**CORRECTION-of-scope for s26's crate**: the n0 (and n2) table's entry
0x0D — the disguise the 17 ACTUALLY-PLACED office crawlers bind — is a
**14x14x14 crate (1 block, 32 recs)**, the AREA11 size class, NOT the
6x4x5 cardboard box. The s26 box is the SUB-STATE-1 table's 0x0D, and
sub-state 1 places no crawler at all. The port's global
`enemy_crate.emdl` (the box) therefore stands in for the office0 scene's
crates — flagged, re-carve from the n0 table when the crate kind grows
per-scene models.

Doors (class 5): param 0x16 (model 0x15, fn 0x001BC350, link 0x0200) at
(-30.5, 0, -187.3) — a 9x21 single leaf + handle sub-node; param 0x19
(model 0x17, fn 0x001BB860, link 0x0605) at (440.2, 15, 109.9) — 3 nodes,
two stacked 10x21 sliding panels. Carved model-local with ALL slots at
identity (their rest offsets are runtime palette state; no live capture
of this sub-state exists — n1's door slots prove offsets can be nonzero,
so the leaves may sit slightly off until captured).

### 3. The engine spawn tables live in the BOOT ELF (.data)

The s22 spawn-table chain (`0x24D650[area] -> desc[sub] -> rec*0x30`)
resolves statically for AREA02: **three 7-record copies at 0x24B2F4 /
0x24B444 / 0x24B594 (sub-states 0/1/2)**, record = {f32 pos[3]; f32 yaw;
u32 flags; u32 1; ...}. All three copies carry the SAME 7 entries:
[0] (-35,0,-178) y0 (flanks the west door), [1] (65,0,-225) y1.571,
[2]/(3] (104,0,-245/-259) the office door faces (s22 live), [4]
(439,15,99) ypi (flanks the east door), [5] **(40,0,-146) y-pi/2** — the
main-floor arrival, 3u from the sub-state-0 table's rec-0 trigger at
(43,3.5,-147) — and [6] (90,0,-170) ypi. (The s22 doc's "tbl @0x24B6B0"
is the NEXT table base; entry contents match at 0x24B444+, so the doc
vaddr was off by one table — the entries themselves were verified live
and stand.) Scene spawn = entry 5.

### 4. Textures with NO captured state: GS-upload replay (synthetic VRAM)

All AREA02 captures (GS dump frame1.gs, save states 08/09/resume) are
sub-state 1; the n0 leaf's 242 TEX0 keys overlap them ZERO percent (same
0x1FB0..0x34C8 block range, different content per sub-state). Resolution
instead comes from the disc's own GS upload packets
(`export_level.read_uploads_localmem`): replay BITBLTBUF/TRXPOS/TRXREG +
IMAGE payloads into a zeroed 4 MB buffer using PSMCT32 addressing
(`psmct32_word(x, y, ppr)` — NOTE its third arg is pages-per-row = DBW,
not pixels). Sources and coverage:

- `chunk06.n0/f02_id44.bin`: dbp 0x2A00 256x480 + dbp 0x3180 256x224
  (the leaf pack, blocks 0x2A00..0x3500) — 241/242 keys;
- `chunk27/f00_id35.bin`: dbp 0x1D00 256x480 (the global library pack,
  0x1D00..0x2480) — the remaining key.

**Replay validation: the chunk27 replay is byte-identical to the live
office GS dump's VRAM over all 1920 blocks** — coverage == residency, no
content heuristic needed (a 256-byte-block coverage map gates the read;
uncovered -> flat grey, counted). Scene result: **247/247 texture slots
resolved** (84+40+20 world, 98 placed, 5 doors), 0 flat fallbacks. This
generalizes: any leaf with its own upload packets can now be textured
without a save state (`--uploads`).

### 5. Scene export + verification (assets/scene_office0)

- Parts: 00_floor_east (8427v/4919t/84tex), 01_floor_west (3895/2261/40),
  02_annex_east (335/187/20), 03_placed (15520/11334/98; 21 instances of
  16 table entries — incl. the misc creature-family destructibles baked
  static, flagged), doors/door_m15+m17, office0.emcl (1007 grid polys;
  floor y=0 at the spawn and at every crawler position probed; the east
  platform solves y=15 under the y-15 placements — consistent).
- scene.txt: spawn 40 0 -146 -1.5708; **16 active `enemy crate` lines**
  (EM_ENEMY_MAX cap; the 17th, (476.9,15,68.8) — farthest from spawn —
  stays commented with the overflow note); 8 generator + 3 creature
  comments; 2 door lines (table positions, r=12); doorsfx 0x3FD/0x3FE
  (the west door's link family 2 = the s26 office pair; the EAST door's
  family-6 pair is unresolved — D_0024DB80 is BSS, needs a live read).
- Skipped, reported by the exporter: 4 deferred class-0x0B records, 3
  glow/billboard (param 0x7D), 2 env/camera, 1 param-0 record.
- Captures (EM_SCENE + temp spawn-override shadow): coherent textured
  industrial floor — concrete walls, pipe runs, grate floors, a freight
  conveyor crossing the floor, hazard-striped platform edges, shutter
  doors, the east freight elevator at y15, and organic infected growth
  masses on the west side; cardboard crates render at the authentic
  table positions (e.g. (33,0,106) beside the conveyor). Default scene
  byte-identical; EM_MOVE/DOOR/WEAPON/ENEMY 1-3 + make test-input PASS.

Open: n0 f00_id42/f01_id46 roles; the n0 cell-list absence (does the
engine run grid-only collision here?); live capture of sub-state 0 (door
leaf rest offsets, generator/crawler verification, the green aura tint);
the east door's sound family; per-scene crate models in the port.

## GAMEPLAY SOUND IDS PINNED LIVE — reload, footsteps, fire chain, door-close verdict (2026-06-10, session 29)

Live PCSX2 capture, storage room AREA02 (2.1). Method: exec breakpoint at
`func_001FB9F0` entry reading `$a0` (= sound id, both the raw path and the
`func_001FBD50` positional tail-call land here). Every action sound below
was read directly off the submit call; reload was replicated twice.

### RELOAD — id pinned (closes the s23/s26 open + the 0xF002 alias)

L3 tap with weapon drawn (mag 29/30, reserve > mag → `func_0017B300`
mode-2 top-up) →

    anim 0x33 committed at +0x1F2/+0x20C  (s23 mapping confirmed live)
    sound 0x163  at reload START   (same id as holster — 0x163 is a
                                    shared weapon-handling foley, not
                                    holster-specific)
    sound 0x168  at the MAG ACTION (~0.5 s in; the distinctive reload
                                    sound; global bank, snd_0351 @
                                    39006 Hz in the soundmap)

No third sound; mag refilled to 30, reserve untouched (total-pool
semantics re-confirmed). **The s26 registry alias `0xF002 → 0x166` was
wrong** — 0x166/0x167 remain unassigned weapon-bank neighbors. The
port registry now maps 0xF002 → 0x168 (regenerated this session).
*(s30: the 0xF002 placeholder is retired entirely — the port's
em_weapon plays the real pair 0x163 + 0x168 directly and
gen_sfx_registry.py dropped the alias plumbing.)*

### FOOTSTEPS — two-layer, surface-dependent + constant gear layer

Each footstep submits TWO positional ids back-to-back (~1.7k cycles
apart, same frame), matching the property-table trigger frames (72/21 →
two steps per walk cycle):

    layer 1 (SURFACE, alternating L/R pair, changes with floor material):
        storage-room floor A: 0x15 / 0x16   (snd_0042 / snd_0052)
        floor B (after crossing a material boundary mid-room):
                              0x1A / 0x1B   (snd_0050 / snd_0048)
    layer 2 (GEAR/cloth foley, constant across surfaces, alternates
             strictly per step): 0x139 / 0x13A  (snd_0308 / snd_0309)
    0x138 observed once (same family; likely a stop/scuff or run
    variant — unresolved which trigger)

Surface selection presumably keys off the collision hit record's
surface-type byte (+0x1A, s22) — the per-surface id table is NOT yet
located statically; only these two pairs are observed.

> **2026-06-10 s37 CORRECTION — mapping decoded (see "FOOTSTEP SURFACE
> TABLE")**: the surface attr DOES come from the hit record's +0x1A,
> but both observed pairs are the SAME material block 0x10 — the
> mid-walk change was a **gait change** (walk→run sub-base +5→+0xA),
> not a material boundary; and neither layer alternates strictly L/R
> (both add an independent rand 0..4; 0x138/0x139/0x13A are one
> randomized gear set 0x138-0x13C).

### FIRE chain re-verified live + two new ids

    CIRCLE (fire; config mask spad 0x70003B78 = 0x0020 swapped) →
        0x164 fire (mag 30→29, both s18 counters decrement)
        0x189 impact/ricochet  ~2 frames later (office wall hit)
        0x16A shell-casing     ~0.7 s later (2-event, snd_0347)
    SQUARE (config mask 0x3B74 = 0x0080) → 0x179 (2-event, snd_0436)
        with NO ammo use and no state change — sub-weapon/melee-class
        action sound, unidentified action (new open item)
    R1 draw 0x162 / release holster 0x163 — re-confirmed live.

Config-mask block at spad `0x70003B70` (u16 each, swapped layout):
3B70=0x0800, 3B72=0x0800, 3B74=0x0080 (SQ), 3B76=0x0040 (X, use),
3B78=0x0020 (O, FIRE), 3B7A=0x0010 (TRI), 3B7C=0x0008, 3B7E=0x0002.
So default config: CIRCLE = fire, X = use, L3 = reload (raw bit).

### DOOR CLOSE IS SILENT — engine verdict (closes the s17 question)

Static, confirmed against the live actors: the door close path has **no
sound call anywhere**: state 4 `func_001BC240` → `func_001BC150`
(callees: fade `func_001AEDE0`, area kick `func_001B0C00`), state 5
`func_001BC290` (callees: `anim_advance_time`, `anim_clip_init` only).
The complete door audio set is: open pair `D_0024DB80[link>>8][side]`
(op-0x0B sub-6 in the open script), locked rattle 0x3F2 + VO. The live
pool holds exactly TWO door actors in AREA02 s1 (pattern scan
`01 ?? 85 03` over the 256×0x2F0 pool at 0x7A5640): id 2 @(109,0,-252.2)
link 0x0280 and id 0x81 (bit7=inter-area) @(57,0,-220.5) link 0x0200 —
both selector 2 → front 0x3FD / back 0x3FE, as predicted by s26.
A scripted-open of door id 2 was forced via the s23 effect-byte method;
its sound fired during a tooling blackout (below) and was not read, but
the table+selector path needs no further live proof.

### Door/transit machine — new architecture details (from the recovery)

- `func_001BBE40` arm gate is **door `+0x0B` bit 2** (`andi 4`); once
  armed it unconditionally: patches the script records (anim id 0x45/0x43
  open Δ 0x46/0x44 locked, clip 2/0 / 3/1, wait 90.0/70.0), patches the
  sound id via `func_001BBD60`, snaps player yaw, **teleports the player
  via `func_00182F90`** (instant translate of +0xA0/+0xB0 AND the spad
  mirror — `0x70003B40` is the authoritative position mirror, settling
  the s23 "real store" open item: position writes must also hit 3B40 to
  stick), queues `D_0024DE40`/`D_0024DEC0` (`func_001BA1A0`) and runs one
  synchronous pump (`func_001BA1F0`).
- Per-frame script pumping happens from the DOOR's own SM states 1/3 via
  `func_001BC0E0` (clip advance + one `func_001BA1F0` step). States:
  0 closed → (BBE40 ok) 3 opening → 4 commit (`func_001BC240/150`) → 5
  re-close (`func_001BC290`; **gated on pause byte `D_00810841`-family
  `D_008106B8` == 0**) → 0.
- `spad 0x70003B8D` is the FRAME SELECTOR read by `func_001AE040` at
  0x001AE294: 0 → `func_001AE5E0` gameplay frame, nonzero →
  `func_001AE6B0` cutscene frame. Writing 3 by hand (the s23 method's
  second byte) flips the player into the transit hold; the door SM must
  independently see `+0x0B` bit 2 or the player waits forever.
- Player mode-0 top is `func_00161020` (jtbl_0026D3B0[0]) switching on
  +0x06 (0,1,2,0x63,0x64); sub-state 1 early-outs through the fade-wait
  halfword `0x28A9A0`, the action dispatcher `func_001607D0` (+0x1F0
  action byte 0x27,0x31-0x37), and the USE SCAN **`func_00160220`**
  (gate: `0x810E74 & spad3B76` = X edge — this is the interaction
  machine the s20 worklist wanted). Flashlight id 0x15D fires from this
  sub-state's idle timer path (0x12C-frame countdown at +0x28).

### Tooling postmortem — breakpoint decay → RPC wedge (REQUIRED READING for live sessions)

Extends the s23 caveat ("exec BPs never fired"). In this DebugServer
build exec breakpoints DO fire on a fresh PCSX2 launch — the whole id
capture above used them — but they **silently degrade after heavy use**
(hundreds of hits / add-remove cycles): first hits stop reporting, and
every address that EVER had a breakpoint keeps its blocks instrumented.
Consequences observed, in order:

1. Frames crawl (each gameplay frame takes seconds; main-loop counter
   `0x70003B64` near-frozen while the vsync ISR counter `0x810E90` runs
   at full turbo rate) — looks exactly like a soft-lock.
2. The audio thread (TID3, `func_001FB0C0`) backs up: the sound command
   ring cursors at `0x27F770` (write) / `0x27F774` (read) diverge by
   hundreds, and the SIF RPC client at `0x281B00` (data `0x277400`;
   +0x10 bit0 busy, +0x18 id) wedges busy permanently — the IOP-side
   sound server never recovers even after the EE side is cleaned.
3. **Fix that worked**: `pcsx2_clear_all_breakpoints` (restores full
   speed instantly) + drain the ring (read cursor := write cursor) +
   patch the RPC busy predicate `0x0010EA60` → `jr ra; li v0,0`
   (original words `0x8C850000 0x10A00009`) so the EE never blocks on
   the dead client. Game logic runs fine; audio output stays dead until
   PCSX2 restarts (EE-side sound submission still observable in memory).

Rules of thumb going forward: keep at most ONE breakpoint live, remove
it the moment a capture leg ends, and prefer `pcsx2_watch_change` +
`pcsx2_memory_diff` (snapshot the 48×0x78 channel array `D_0027E0C0`,
act, diff — channel +0x18 holds the sound id) for anything bulk.

### End-state of this session (honest damage report)

The VM was left RUNNING and playable-ish but NOT pristine:
- `0x0010EA60` patched (audio busy check bypassed) — **in-memory only;
  restart PCSX2 (or reload the ELF) to restore audio.**
- Audible audio dead for the session (IOP sound-RPC server wedged).
- Player: storage room ~(106.4, 0, -259.5) (walked from the parked spot
  (64.4, 0, -286.4)), yaw overwritten to pi at one point; locomotion and
  position are healthy, but a scripted-mode residue from the forced door
  transit still inhibits the weapon-draw action mask (+0x200 stays 0
  under R1) and locomotion anim requests — the next save-load or
  area transition resets the player struct and clears this.
- Door id 2: fully restored (closed, state 0, disarmed +0x0B=0).
- Mag 30, reserve 118 (two test shots). All breakpoints/watches/pad
  injection cleared.

## DOOR CLIPS FOUND — slot-0x39 bank pinned, bid-0x28 containers decoded, real swing shipped (2026-06-10, session 30)

Closes the s20 open item "double door open/close clip UNLOCATED" and
retires the port's flagged placeholder hinge swing. Static only (door
.s files + local ELF dumps + extract/); no emulator.

### 1. Container resolution chain (the s23 lead, run to ground)

The door OPEN script's anim record does NOT carry a container: the
op-0x0B record at 0x24DC40 is sub-6 (sound + `anim_clip_init(rec
[+0x14])` on the owner's EXISTING +0x40 container) and its +0x1C field
is 0 — only subs 4/5 read +0x1C (ELF dump of 0x24DBC0..0x24DF40
re-verified byte-exact against the s23 listing; default door-clip id
baked into both op-0x0B records is 1, patched to [2|0]/[3|1] by
func_001BBE40 before queuing). The bind happens at door INIT instead:

```
func_001BBDA0 (door INIT, s15)
  -> func_001B0F60(actor, 0)
       -> func_001B0EA0(actor): model rec = func_001C6120(
              *(D_0028A59C) per-area model table, actor+0xD) -> +0x44,
              node count -> +0xC, bone array alloc
       on success: actor+0x40 = D_0028A574        <- the clip container
                   bone_init_default_2(actor, 0)
```

`D_0028A574` = `0x28A490 + 4*0x39` = **`D_0028A490[slot 0x39]`** — the
asset pointer-slot array (s22b/s27). Slot 0x39 is filled at boot by
DATA.DAT chunk 0x1B (s27 loader decode: chunk 0x1B fills slots
0x35/0x37/0x39/0x3A = exactly extract/chunk27's four files) =
**`extract/chunk27/f02_id39.bin`** — the same globally-resident bank s20
characterized and rejected. Note `func_001B0F60` is the GENERIC placed-
object bind: every placed object that comes up this path gets the slot-
0x39 bank as its default clip container.

### 2. Why s20 missed it: the directory has 16 entries; door clips are blob-id 0x28

`anim_clip_resolve`/`func_001C6120` index the bank's OWN leading u32
directory (count 16, offsets 0x50..0x24D0, low 2 bits flags). s20's
hunt enumerated `find_id74_headers` scan order instead — and that
scanner only accepts blob ids 0x74/0x2c, so it saw just 5 containers
(directory ids 9/12/13/14/15: the 360/90/40/30/30-frame set, which is
NOT the door group). **Directory ids 0-8 are containers with blob id
0x28** — same header layout (n_nodes/clip_len at +0, sentinel 0xFFFE at
+4, blob id at +8, trn/scl table offsets at +0xC, parents at +0x20) and
same 20-bit-quat / 26-bit-vec3 key encoding; `export_native.
bake_id74_palettes(..., anim_hdr=off)` decodes them as-is (the
id-agnostic `scan_anim_containers` already finds all 16).

### 3. The door clips (directory ids 0-3, 2-node rigs, parents [-1,0])

Node 0 = the door panel (hinge at the placement origin), node 1 = the
slot-1 lock/mechanism fixture; **clip frame 0 = the live-captured
closed pose exactly** (node-1 local (-7.69, 9.00, -0.25) = L0^-1*L1 of
the captured A05C0 slots; clips 1-3 author it as -7.72).

| id | script role | frames | motion |
|----|-------------|--------|--------|
| 0 | open toward back | 150 | whole assembly (panel + fixture, shared yaw) swings about the origin Y hinge, ease-out: -42.8 deg by f40, -87.1 by f80, settles -87.5 |
| 1 | locked jiggle, back | 200 | panel STILL; the node-1 lock fixture rattles — rot-from-rest peaks 24.0 deg around f60-110, settles to 0 (times the script's 60-frame wait -> 0x3F2 rattle SFX) |
| 2 | open toward front | 150 | same swing, slightly sharper ease (-88.2 by f75, flat after); both open variants swing the SAME direction — the panel is one-way, the side only re-times it |
| 3 | locked jiggle, front | 200 | as clip 1, peak 15.8 deg |

Directory ids 4-7 (also bid 0x28, 2-node, 200 f) translate node 1 z
+-0.9 with zero rotation — small two-state slide, not referenced by the
door scripts (candidates: panel/station mechanism states — open).
Id 8 = 1-node 360-frame full Y rotation (s20's "dial/beacon"). Ids 9-15
= the old s20 set (the 3-node together/apart pair etc., still
unidentified — NOT door parts).

Hemisphere/continuity check across all four door clips: max basis-
vector deviation 0.070/frame (≈4 deg at the swing's fastest — no sign
flips, which would read ~2.0), max node translation jump 0.44 u/frame.
Clean.

### 4. Export + port pickup (tools/export_props.py --doors extended)

`find_door_clip` now resolves clips the way the engine does — the
file's leading directory, ids from the door scripts — and verifies each
(2-node rig, frame-0 node-1 translation within 1.0 u of the captured
closed slot-1 local) instead of hunting id-0x74 scan order. The door
EMDL (`assets/scene/doors/door_m03.emdl`, port repo, git-ignored) is
now EMD3 multi-clip: 700 palette frames @60, clip table ordered
open-first **[0, 2, 1, 3]** (the port's em_door.c plays table entry 0;
locked clips are reachable by engine id via `em_model_clip_index` when
the locked subs land). Verified by readback: frame 0 reproduces the
captured closed pose exactly (the port's closed-pose AABB hull is
unchanged), clip-0 end pose -87.5 deg, all within-clip continuity as
above. Port compatibility audited statically: `has_clip` keys off
header frame_count (700>1 ok), `em_model_palette_at(clip 0)` wraps at
the CLIP's 150 frames but `clip_t` is phase-capped at the script wait
(90/70) so no wrap is reachable; `door_clip_total`'s whole-buffer
`frame_count-1` only loosens that cap. The placeholder swing path
(`DOOR_SWING_*`) is now dead code whenever the EMDL is present — the
real motion IS a ~88 deg origin-hinge swing, so the placeholder reads
nearly identically; the win is the authored ease-out, the exact closed
pose, the lock-fixture motion, and the locked rattle now being
available.

Open (door clips):
- ~~scene_office0's door_m15/door_m17 (s28) still ship pose-only EMDLs;
  model-0x15 (13-slot corridor door) plays the same script clip ids
  through the same slot-0x39 2-node containers — how a 2-node clip
  drives a 13-slot palette needs the func_001C68C0 node->slot mapping
  read before exporting those.~~ CLOSED s32 ("OFFICE0 DOORS
  ARTICULATED"): the model RECORD carries the rig (parent + rest local
  per node); bones map 1:1 to clip nodes and unkeyed bones hold their
  model-rec rest. The office0 m15 blob is itself 2-node — both office0
  doors now ship articulated EMD3s.
- ~~directory ids 4-7 (z-slide pair) and 9-15 unidentified.~~ s32: all
  16 ids motion-classified (inventory table in "OFFICE0 DOORS
  ARTICULATED" §1); behavioral consumers of ids 4-15 still open.

## GENERATOR — func_0015A2C0 RESOLVED (2026-06-10, session 28)

Full .s read of the ENEMY GENERATOR (class 0x0D, model 3, 129
placements — the engine's most-placed creature behavior) plus its
callees, its trigger pair-pass and its data tables (decoded from the
pristine boot ELF locally; `elf/SCUS_971.12.elf` is a REBUILT binary —
read game data from `config/SCUS_971.12` only). This REVISES the
session-22 headline ("RNG count tables… emits up to 4 kind-0xD per
point"): the count tables are a MODE draw, emission is player-
proximity-gated, and the delays are 30/90 SECONDS, not frames.

### What it is

An organic infected-growth FLOOR PAD. It has no model-table binding,
no HP, no hit sphere — it idles, charges while the PLAYER STANDS ON
IT, and depending on a mode drawn at init either opens as a damaging
"breather" trap or births kind-0xD worms (the leech, brain
func_00153F10).

### Placement fields

- `kind` (+0x54) 0–6 → config rec `D_00248120 + kind*20` (decoded):

  | kind | X half-ext | Y ext | Z half-ext |
  |---|---|---|---|
  | 0 | 5  | 1.0 | 5  |
  | 1 | 15 | 1.0 | 15 |
  | 2 | 25 | 1.0 | 25 |
  | 3 | 10 | 1.0 | 15 |
  | 4 | 15 | 1.0 | 30 |
  | 5 | 15 | 1.0 | 10 |
  | 6 | 30 | 1.0 | 15 |

  (fields 3/4 zero in all 7 recs). The X/Z extents are BOTH the
  trigger box and, doubled into spad `0x700038A0/A8`, the visual pad
  footprint handed to func_001E9580.
- `link` (+0x56) selects the mode table: 0 = none (pad stays inert
  forever), 1 → `D_002481B0`, 2 → `D_002481D0`.
- `param` (+0x04 → actor +0xD): 0 = normal; 1 = the box pass fires
  event `func_00187EC0(7,0)` instead of `(6,mode)` and does NOT set
  the trigger flag (a "silent" pad variant).
- `uid` (+0x0E) = the per-instance render-buffer slot (below).

### INIT (state 0) — the mode draw

`+0x30` = config rec ptr; anim phase seed `+0x1F4` = RNG; visual
build func_001E9580(actor, uid). Then, link 1/2 only: draw ONE BYTE
from the count table — row = frame RNG (`0x70003B68`) & 3, column =
a GLOBAL per-link counter & 7 (`D_008106EC` for link 1, `D_008106ED`
for link 2, post-incremented) — and STORE IT BACK INTO +0x56 as the
runtime mode. Decoded tables (4 rows × 8 cols):

    D_002481B0 (link 1):        D_002481D0 (link 2):
      0 1 0 1 0 1 0 1             0 1 2 1 0 2 0 1
      1 1 0 0 0 1 0 1             1 2 1 0 2 2 0 2
      0 1 0 1 0 0 0 0             0 1 0 1 2 0 2 0
      1 0 1 1 0 1 0 1             2 2 1 2 0 1 0 1

  Mode semantics: 0 = inert pad, 1 = breather/trap, 2 = WORM EMITTER.
  Link-1 placements can never become worm emitters (table 1 is 0/1
  only; 16/32 active). If the drawn value is 1 the generator ALSO
  immediately emits a PAIR of kind-0xE enemies
  (`func_0015A200(actor, 0xE, 0)` and `(…, 1)` → brain func_001546C0,
  still uncharacterized).

### Trigger — actor +0x0A is "player inside my box", NOT the group alarm

Pair pass `func_001A8BE0(player)` (gameplay frames only, spad
`0x70003B8D == 0`, gated on `D_0028A9A0 == 0`): for every HAZARD-list
entry (`D_00275BA0` — the generator registers there, not on the
damage-target list) dispatch by model byte — 1 → func_001A8660, 3 →
**func_001A8840** (the generator), 5 → func_001A8970.

`func_001A8840(player, gen)`: box test |player+0xA0..A8 −
gen+0xB0..B8| against rec X / rec Z, Y against rec Y + 1.5. Inside,
param +0xD == 0: `gen+0x0A = 1` + event `func_00187EC0(6, mode)`;
and if the pad is OPEN (`+0x0B`, breather) and player event byte ==
1 and `D_00810707 != 1`: **player +0x22C = 5.0, player event = 3**
— the open pad hurts the standing player. The behavior consumes
+0x0A in its tick and clears it at tick end.

Second pass `func_001A8DA0`: every `D_00275BB0`-list actor ×
every model-3 hazard with +0xD == 0 → `func_001A8CE0` box test
(tighter Y +0.5) sets the OTHER actor's +0x0A — generators wake
nearby list-BB0 actors (list identity unverified; open).

### ACTIVE (state 1) — per mode (sub-state +0x05)

Every tick first renders via func_001E9E60(actor, uid), then:

- **mode 1 (breather/trap)** — sub 0 closed: morph phase `+0x80 =
  +0x20/100`; in-box → `+0x20 += 1`, at 100 → sub 1, `+0x0B = 1`
  (OPEN), `+0x20 = 60`, phase 1; out-of-box → `+0x20 = 0`. Sub 1
  open: sound 0x42F every 128 frames (`0x70003B64 & 0x7F`), phase =
  `+0x20/60`, particle fountain func_0015A750; in-box holds `+0x20 =
  60`; out-of-box decays −1/tick → 0 → closed (sub 0, +0x0B = 0,
  anim +0x1F0 = 0). The damage to the standing player comes from the
  pair pass above, not the behavior.
- **mode 2 (worm emitter)** — sub 0 charge: out-of-box → `+0x20 = 0`
  (consecutive-frames requirement); in-box → `+0x20 += 1`; when
  `+0x20+1 > 120` (the **121st consecutive in-box frame**) → spawn
  ONE kind-0xD worm `func_0015A200(actor, 0xD, 0)`; on alloc success
  `+0x2E++` (a full pool does NOT consume the cap — it retries after
  the delay). `+0x2E >= 4` → sub 2 **EXHAUSTED, permanent**; else
  sub 1 with `+0x20 = D_002481F0[((RNG>>16)*3)>>15]` — decoded
  delays **{1800, 3600, 5400} frames = 30/60/90 s** — counted down
  −1/tick in sub 1 WITHOUT needing the player, then sub 0 again.
  Mode 2 never writes +0x80 (the pad stays at rest pose).
- any other mode (0, or the drawn 0): inert — tail only.
- Tail every tick: func_001B17A0 (cull/transform), clear +0x0A.

States 2/3 → func_001AFC10 (free). **The behavior never reads
+0x34/+0x36** — and func_00183AC0 (the laser/bullet victim filter)
requires class&0x1F == 2, rejecting class 0x0D; the pair-pass model
whitelist skips model 3. The generator is INDESTRUCTIBLE; the only
terminal state is the 4-worm exhaustion.

### Spawn helper func_0015A200(parent, kind, idx)

`func_001AFA90(2)` alloc (NULL → return 0); child `+0x03 = kind`
(0xD/0xE — the model byte IS the kind), `+0x0D = parent kind(+0x54)`,
`+0x2E = idx` (the 0/1 pair index), `+0x9A = 0`, **pos = parent
+0xB0 verbatim (vec copy — worms emerge AT the generator origin, no
offsets)**, `+0xC0/C4/C8 = 0` (the leech brain init then yaws toward
the player and plays 0x430), `+0x20 = parent uid(+0x14)`, brain
`+0x10 = func_00153F10` (kind 0xD) / `func_001546C0` (kind 0xE).
Returns 1.

### Visual — procedural VU-morph, NOT a model-table entry

- `func_001E9580(actor, uid, sizes=spad 0x700038A0)` (init): builds a
  private pad-geometry buffer at `D_00275C1C + uid*0xA060`: world pos
  at buf+0x00, full footprint (2×recX, 2×recZ) at buf+0x30/0x34,
  wobble params at buf+0x40..0x50, then a per-AREA/ROOM shape/skin
  variant switch on `D_00810700<<8 | D_00810701` (cases 0x1300,
  0x803, 0x703, 0x702, 0x700, …). Texture select byte buf+0x5C.
- `func_001E9E60(actor, uid)` (every active tick): streams 6 ring
  segments × 8 morph rows from the buffer (0x200 stride), blending
  each vs the REST SHAPE vec `D_0026E9B0 = (24, 112, 24)` by factor
  `actor+0x80` (the breather phase — the pad visibly swells as it
  charges/opens), scale buf+0x1C × actor+0x8C; prim/tex word picked
  by buf+0x5C (0x20048CC1_55422242 / 0x20048E41_55422256); VU kernel
  packet `D_002345E0`. (The session-21 microcode table's
  "func_001E9E60 = kernel pair shipper" row described this upload
  path, not a generic shipper — this is the generator pad renderer.)
- `func_0015A750(actor)` (mode-1 open tick): particle fountain —
  jtbl_0026D390 per kind: count {2,3,5,3,5,3,4}, spread `D_00248280`
  = 0x18/0x20; per-particle params into D_00248200 block
  (rec X/Z fractions −5.0, 64×phase), drawn via func_001CFAE0/
  001CFBE0; advances the fountain phase +0x1F0 by 0.01 wrap 2→1.
- **Nothing to export**: no EMDL/model-table id. A native port must
  reimplement the morph (or stand in a placeholder).

### Office (AREA02) placements decoded

| table | recs | kinds | links |
|---|---|---|---|
| sub-state 0 @0x827830 | 50–57 (uid 0–7) | 5,0,0,1,5,1,1,1 | **all 0 → INERT** |
| sub-state 2 @0x8283D0 | 43–50 (uid 0–7) | 3,0,5,0,1,0,5,1 | 2,1,2,1,2,1,2,2 |

The main-floor story beat (sub-state 0, the exported scene_office0)
ships only DECORATIVE pads; the "after" population (sub-state 2)
arms 5 link-2 (worm-capable) + 3 link-1 pads on the same floor plus
the annex. All param 0, yaw 0, y = 0.01.

### Port status (extermination-port src/game/em_enemy.*)

`enemy generator x y z yaw [kind k] [link n]` implemented per the
above: decoded footprints/tables/charge/delays/cap, worm-at-origin
spawns through the normal crawler path, indestructible, separate
EM_GENERATOR_MAX pool (hazard-list semantics: not shootable, not
acquirable, excluded from alive counts). Flagged stand-ins: module
LCG for the draws, one-shot mailbox-5 trap hit, placeholder mound
visual (phase = Y swell), manifest
parsed by an em_enemy-side shim (em_game.c's kind table not touched).
The mode-1 kind-0xE pair is now TRANSLATED (port commit cd47b7e): a
mode-1 draw attaches the tendril-field pair per the "KIND-0xE
COMPANION RESOLVED" port contract — full SCAN/DEPLOY/HOLD/RETRACT
machine, LCG scatter, s16 bob/thrash integrator, 12 re-posed
assets/tendril.emdl draws per field (load-if-present), sound 0x42D on
trigger (unmapped in the generated sfx registry — flagged there), no
damage; room-tint→green blend + ramp alpha fade skipped (the
contract's sanctioned simplification). `EM_ENEMY_TEST=5` validates
trigger→deploy(8 ticks→296)→hold(12/12 valid)→retract→rescan with a
synthetic walker and a link-1 pad mode-forced to 1 (test-only).
`EM_ENEMY_TEST=4` validates charge→worm→kill→cap→exhaustion (delays
/60, test-only). scene_office0/scene.txt's 8 generator lines are now
ACTIVE with decoded kind/link (all link 0 → inert: zero enemy-count
impact; documented in the manifest), and export_level.py's enemy-block
emitter writes them directly (s33: fn-0x0015A2C0 placements →
`enemy generator x y z yaw kind k link n`; regeneration reproduces
the hand-activated set, so the manifest is round-trip safe).

### Open

- ~~func_001546C0 (kind 0xE, the mode-1 pair) — uncharacterized~~
  RESOLVED s33: tendril/spike field around the player, no damage —
  see "KIND-0xE COMPANION RESOLVED" below.
- `D_00275BB0` list identity (the second box pass's wake targets).
- func_00187EC0(6/7, …) event semantics (rumble? ambient cue?).
- The per-AREA/ROOM visual variants of func_001E9580 (case table).

## PLAYER STICK LOCOMOTION IDS — walk = anim id 1, run = anim id 2 (2026-06-10, session 31)

> **2026-06-11 s56 CORRECTION — every tier label below is ONE STEP LOW.**
> This section read only the locomotion ENTRY (locIdx = gait-1); the
> tier is then RAMPED UP by func_0017BC40 until D_00248870[locIdx]
> matches the func_00174AC0 target +0x240 — full stick SUSTAINS
> locIdx 3 = anim id 3 at 0.8 u/tick. Gait 1/2/3 = WALK id 1 / JOG
> id 2 / RUN id 3. See "LOCOMOTION TIER RAMP" (s56); the selection
> chain and table dumps below remain correct as the entry mechanics.

Closes the s30 flag "the port's locomotion ships clips 2/3 from the
s10 stride scan; the engine's default walk is id 1". Static decode of
the full stick → anim-id selection chain; the port now commits the
authentic ids.

### The selection chain (boot ELF, all static)

1. **Stick quantizer `func_001B5CC0(stickX, stickY)`**: r =
   sqrt((x−128)² + (y−128)²); returns **0** (r ≤ 48), **1** (r ≤ 88),
   **2** (r ≤ 122), **3** (else) — the GAIT byte.
2. The live pad reader `func_001B5940` stores it at processed-pad
   struct +0x17 = **`D_00810E57`** (base `D_00810E40`, the s21 pad
   map); demo playback overwrites the same byte from input-record
   byte +4 (`func_001B5BC0`, stream `D_0028A6AC`).
3. Player locomotion states copy it to **player +0x23F**
   (`func_00175390` / `func_001751A0` / `func_00174AC0`; the last
   compares it against a MAX of 3 — gait ∈ 0..3 always).
4. The locomotion top **`func_001612D0`** (vram 0x161450): locIdx
   `+0x25C` = gait − 1; **speed** `+0x38` = `D_00248870[locIdx]` =
   `{0.0, 0.1, 0.3, 0.8}` u/tick; **anim id** =
   `func_0017B490(actor, mode=1, family=+0x235, locIdx)` →
   `func_0017B460(mode, family*4 + locIdx)` =
   **`((s16**)D_00248AB0)[mode][family*4 + locIdx]`**
   (`jtbl_0026D700` routes modes 1/6 through the family*4 indexer;
   a flashlight-ish flag `func_001B0070()&4` substitutes locIdx+0x10).

### The mode-1 id table (D_00248AB0[1] = 0x248A10, ELF-dumped)

family 0 (unarmed): `{0, 1, 2, 3}`; family 1: `{0xA,0xB,0xC,0xD}`;
family 2: `{0x4B,0x4C,0x4D,0x4E}`; family 3: `{0x55,0x4C,0x4D,0x4E}`;
family 4 / +0x10 row: `{0x14,0x15,0x16,0x17}`.

### Verdict (family 0, the default)

| gait (stick ring) | locIdx | anim id | speed u/tick | D_00248C90 row |
|---|---|---|---|---|
| 1 (48 < r ≤ 88) | 0 | 0 | 0.0 | idle/turn-in-place, no steps |
| 2 (88 < r ≤ 122) | 1 | **1 = WALK** | 0.1 (= 6 u/s @60) | mode 1, steps **72/21**, rate 1.0 |
| 3 (r > 122, full) | 2 | **2 = RUN** | 0.3 | mode 1, steps **26/3**, rate 1.0 |
| (4 — unreachable) | 3 | 3 | 0.8 | steps 21/2 — sprint data slot the quantizer never selects |

The id-1 row's 72/21 is exactly the pair the s29 live capture metered
while stick-walking — the live and static reads now agree. The s10
stride scan had picked clips 2/3 ("walk/run") — i.e. the port walked
with the engine's RUN clip; corrected this session.

### Clip measurements (library chunk28/f01_id3c, exporter-baked)

- id 1 WALK: 120 frames, root travel 12.1 u → natural **6.11 u/s** at
  60 fps (the engine's own 0.1 u/tick = 6.0 u/s cross-checks);
  hemisphere-clean (max adjacent-frame rotation 5.4°).
- id 2 RUN: 45 frames, 17.6 u → 24.07 u/s (s10 numbers re-confirmed).
- id 3 sprint slot: 40 frames, 30.9 u → 47.57 u/s (stays exported,
  unused by stick locomotion).

### Port (extermination-port)

player.emdl re-exported with id 1 appended (recorded CLI + `,1`; old
export reproduced byte-identical first; superset byte-verified:
verts/indices/parents/tex/texels identical, old clip table + palette
exact prefixes, clip 1[977..+120] appended). em_game: CLIP_ID_WALK
2→1, CLIP_ID_RUN 3→2, WALK_CLIP_SPEED 24.07→6.11 (stride lock),
footstep frames walk 72/21 / run 26/3. Verified: instrumented run
shows crossings at exactly 72/21; default capture byte-identical;
mid-stride captures old-vs-new differ visibly (5.8% of pixels — the
id-1 walk reads as a longer, scissored stride with trailing push-off
vs the old jog-like clip-2 frame); move test PASS with 8 footstep-layer
plays (4 steps — was 6/3: the 120-frame cycle at the port's 15 u/s
stride lock steps every ~24 frames); all other self-tests PASS.

## OFFICE0 DOORS ARTICULATED — bank ids 4-15 classified, model-rec rigs, native slide decoded (2026-06-10, session 32)

Closes the s30 open items: the slot-0x39 bank's remaining directory ids
are motion-classified, the office0 door rigs are pinned, and BOTH
scene_office0 doors now ship articulated EMD3s. Static only (.s files +
rebuilt-ELF .data + extract/); no emulator.

### 1. chunk27/f02_id39.bin — full 16-entry directory inventory

Every container baked via `bake_id74_palettes(anim_hdr=dir[i]&~3)`;
motion = world-palette deltas from frame 0 (max |dT| per axis, rotation
angle vs frame-0 basis):

| id | rig (parents) | frames | motion |
|----|---------------|--------|--------|
| 0  | 2 [-1,0] | 150 | DOOR open-back: whole-rig -87.5° Y swing about origin, ease-out (s30) |
| 1  | 2 [-1,0] | 200 | DOOR locked-back: node-1 fixture rattle, 24.0° peak @f64, settles (s30) |
| 2  | 2 [-1,0] | 150 | DOOR open-front: -88.2° swing, sharper ease (s30) |
| 3  | 2 [-1,0] | 200 | DOOR locked-front: 15.8° peak @f68 (s30) |
| 4  | 2 [-1,0] | 200 | two-state z-flip: node 1 t0 z=-0.9 → +0.9 (dT +1.8), zero rotation |
| 5  | 2 [-1,0] | 200 | mirror of 4: z=+0.9 → -0.9 |
| 6  | 2 [-1,0] | 200 | same as 4 (second variant, z -0.9 → +0.9) |
| 7  | 2 [-1,0] | 200 | as 5 but settles -0.78 (z +0.9 → -0.78) |
| 8  | 1 [-1]   | 360 | full 360° Y rotation with wobble t (max dT (3.1,-7.9,11.6)) — s20's "dial/beacon" |
| 9  | 3 [-1,0,1] | 360 | node 2 only (rest (-5.1,0.2,0)): slow steady +100° over the whole clip — crank/valve |
| 10 | 2 [-1,0] | 300 | node 1 (rest at origin): +120° by f240, holds — lever/wheel A |
| 11 | 2 [-1,0] | 300 | node 1: +120.4° spread over all 300 f — lever/wheel B (re-timed pair of 10) |
| 12 | 3 [-1,0,1] | 90 | nodes 1+2 (rest y 12.3): 69.7° swing out @f36 AND node-1 y +9.7 lift, returns rot to 0 — arm/claw cycle |
| 13 | 3 [-1,0,1] | 40 | PURE TRANSLATION together: node-1 world y 22→12.6, node-2 y 10→12.6 (s20's "together/apart") |
| 14 | 3 [-1,0,1] | 30 | apart: node-1 y 10.8→20, node-2 y 10.8→10 (the 13-pair's return) |
| 15 | 3 [-1,0,1] | 30 | as 14 with overshoot wobble (12.6°/18.4° transient) |

Ids 4-7's ±0.9-u z-flip is far too small for any door panel — they are
two-state MECHANISM toggles (candidates: wall-station latch/button —
the wall-station brain func_001C1A80 comes up the same generic
func_001B0F60 bind and so holds this same container). Ids 9-15 =
servo/lever/arm motions, NOT door parts. NONE of 4-15 is referenced by
any main-ELF script: a full .data scan for 0x40-stride op-0x0B records
finds exactly the s23 m03 trio (0x24DC40 sub-6 id 1, 0x24DD00 sub-0
id 1, 0x24DE00 sub-1) — consumers of 4-15 are direct anim_clip_init
callers or overlay-local scripts (open).

### 2. The MODEL RECORD carries the rig — the s30 node->slot question answered

`bone_init_default_1` (called from both door INIT paths) reads the
actor's model record (+0x44, the per-area-table entry): node table at
`rec + *(rec+0xC)`, n_nodes = u32 at rec+8, 0x50-byte entries:

    +0x04  s16  parent index  -> bone+0x64
    +0x10  4x4  rest local (row-vector layout, translation in row 3)
                -> the bone's local matrix qwords

`func_001C9940` (the evaluator behind func_001C68C0) then composes
`world[b] = world[bone[b]+0x64] * local[b]` — bone+0x64 is the PARENT,
not a clip-channel remap. So clip channels bind to bones 1:1 BY INDEX;
a clip with fewer nodes than the rig leaves the extra bones at their
model-record rest (they still follow their parents). The s28 flag
"rest offsets are runtime state, needs live capture" was WRONG — the
closed pose is static data in the model record:

- param 0x16 (door_m15, west): 2 nodes [-1,0]; node-1 rest
  (-7.706, 9.023, -0.250). Mesh verts node-local, slots {0,1}
  (leaf X[-9,0]xY[0,21] hinged at origin + handle ~1.9x0.4x1.3).
- param 0x19 (door_m17, east): 3 nodes [-1,0,0]; panel rests (∓5,0,0).
  Slots {1,2} carry mesh (node 0 = motion root, no geometry); each
  panel 10x21 centered on its node → together they cover the 20-u
  doorway X[-10,10].

### 3. door_m15 = the m03 brain → bank ids 0/2/1/3 verified

The west door's behavior fn 0x001BC350 IS the m03 office-door brain
(INIT func_001BBDA0 → func_001B0F60 → +0x40 = slot-0x39 bank; scripts
D_0024DE40/D_0024DEC0; clip ids [2|0]/[3|1]). Closed-pose
verification, the s30 method with the model-rec rest standing in for
the live capture: bank clips 0-3 frame-0 node-1 translation
(-7.69/-7.72, 9.00, -0.25) matches the param-0x16 rest within 0.03 u,
rigs match exactly (2 nodes, [-1,0]). The m15 EMDL ships the same
open-first table [0, 2, 1, 3] as door_m03 (700 palette frames @60).

### 4. door_m17 = fn 0x001BB860: NO clip — native slide func_001BB400

The east-door variant brain binds NO clip container: its INIT
(func_001BB520 → func_001B0FD0) does model bind + bone_init_default_1
only (never touches +0x40). Its scripts (rebuilt-ELF .data, record
layout {u32 op|flags, u32 arg, u32 sub, f32 rate @+0xC, id @+0x14,
snd @+0x18, slot @+0x1C, f32 wait @+0x20}):

```
D_0024D900 (OPEN, queued via func_001BA1A0 from the trigger sub):
 @24D900 op07 sub0          enter scripted mode (rate field 120)
 @24D940 op0D sub5          chase-camera cue
 @24D980 op17 sub0          positional door sound (id patched into
                            rec+0x18 by func_001BBD60(self,D_0024D980))
 @24D9C0 op09 → func_001BB400   NATIVE SLIDE (pumped until done)
 @24DA00 op01 sub8 STOP     player walk-through/handoff (rate 35, id 2)

D_0024DA40 (LOCKED TRY): op07 sub2 (fade enter) → op09 func_001BB310
 (locked-look camera) → op02 wait 40 → op09 func_001BBAE0 (the SAME
 locked-door VO native as m03) → op07 sub4 STOP. No motion at all —
 sliding doors have NO locked-jiggle clip.
```

`func_001BB400` (the motion, keyed on the block's +0x3 = placement
flags2): twin branch (default) moves bone[1] +0x7C -= 0.2 and
bone[2] +0x7C += 0.2 per pump (bone+0x7C/0x80/0x84 = the keyed local
translation x/y/z per func_001C9940) until bone[1] passes -9.0;
flags2 0x3D/0x3E use -13.0; flags2 0x08/0x16 = single-leaf branch
(bone[0] only, -9.0). D_00275B40 = the pumped door's bone array. The
office0 east door's flags2 = 0x83 → twin, 9.0 u: each panel parts
9.0 u along door-local X (rest ∓5 → ∓14) at 0.2 u/frame = 45 ticks
(0.75 s), clearing the doorway exactly. The port EMDL bakes those
constants as ONE SYNTHESIZED 46-frame clip (id 0; frame 0 = rest =
closed) — flagged synthetic-from-constants, not disc keyframes.

### 5. Export + port verification (tools/export_props.py --doors-office0)

New mode writes both EMDLs into the scene (mesh node-local with
per-vertex slot bones, model-rec parents, GS-upload replay texels —
3/3 + 2/2 resolved, 0 fallbacks) and refreshes the manifest door lines
(byte-identical values to s28's). Verified:

- readback: m15 4-clip table [0,2,1,3], frame-0 node-1 t (-7.689, 9.0,
  -0.252), open ends ±87.5/88.2°, locked clips return to rest;
  m17 1 clip, panels ∓5 → ∓14, max per-frame step 0.2 u.
- captures (EM_SCENE shadow with spawn inside the 2-u auto-open ring):
  m15 frame-5 closed leaf / frame-100 leaf mid-swing ~85° (doorway
  open, corridor visible); m17 frame-5 closed cross-braced pair /
  frame-28 panels half-parted / frame-75 fully parted into the jambs.
  Door sfx fired on both (front 0x3FD west / back 0x3FE east-path).
- default scene: EM_CAPTURE byte-identical across runs (office assets
  untouched); EM_DOOR_TEST=1 (m03 doors) PASS; make test-input PASS.

Open (office0 doors):
- behavioral consumers of bank ids 4-15 (wall station func_001C1A80 is
  the prime candidate for 4-7/12-15 — its op/anim path unread).
- the east door's family-6 sound pair (D_0024DB80 is BSS — needs live;
  the port still plays the family-2 office pair from scene.txt).
- the port's em_door.c drives m17 through the m03 state machine
  (walk-through + fade transit); the engine's variant brain
  (func_001BB860 trigger sub func_001BB560, op01-sub8 handoff) differs
  in sequencing — acceptable until the variant lifecycle is read.
- m17 close: the engine re-closes via area re-entry/state, not a
  reverse script — the port runs the clip backwards (em_door CLOSING),
  which matches the native motion reversed.

## KIND-0xE COMPANION RESOLVED — the breather pad's TENDRIL FIELD (func_001546C0) (2026-06-10)

The "uncharacterized kind-0xE pair" a mode-1 (breather) generator pad
emits at init is not a creature: it is a STATIONARY AREA-EFFECT actor
that erupts a field of 12 tapering organic SPIKES/TENDRILS out of the
infested floor around the player whenever the player lingers near the
pad. It deals NO damage (the pad's pair-pass `func_001A8840` does
that), has NO senses beyond player distance, never moves, and cannot
be hurt — it is the breather trap's telegraph/dressing. All five
functions read statically (asm in build/asm; offsets vs the s17 actor
contract).

### Identity / spawn

- Spawned ONLY by the generator behavior `func_0015A2C0` init when the
  link-1/link-2 table draw lands mode 1: `func_0015A200(pad, 0xE, 0)` +
  `(pad, 0xE, 1)` — always a PAIR, once, at pad init. Exhaustive
  search: no other caller of `func_0015A200` in the boot ELF, no
  overlay references the helper or the brain (byte-scan of all
  `build/overlays/AREA*.BIN` for `jal 0x0015A200` / the brain address:
  0 hits). Worms (0xD) and tendril fields (0xE) are the only two
  dynamically-spawned generator children.
- Child fields from the spawn helper: model byte +0x03 = 0xE, +0x0D =
  PARENT pad kind (selects the D_00248120 footprint rec), +0x2E = pair
  index 0/1, pos +0xB0 = pad origin verbatim, +0x20 = parent's +0x14
  canonical POINTER (read live every tick), brain +0x10 =
  `func_001546C0`.

### Brain func_001546C0 (0x7C) — standard lifecycle dispatcher

state +0x04: 0 → `func_00154740(self, self+0x1F0)` init; 1 →
`func_001549C0(self, self+0x1F0)` tick; 2 → state=3; 3 →
`func_001AFC10` free; else return. The 0x100-byte actor tail +0x1F0
is the per-instance scratch (layout below).

### INIT func_00154740 (0x274)

1. Model bind `func_001CA5E0(self, *D_0028A4E4, mode 0xA)`.
   `D_0028A4E4` = `0x28A490 + 4*0x15` = **`D_0028A490[slot 0x15]` =
   `extract/chunk03/f13_id15.bin`** (globally resident, 8 KB) — this
   resolves the leech section's open item. Raw blob, header
   {n_blocks 3, qwc 0x186, **n_nodes 1**, size 0x18A0}: 96 verts,
   single rigid node, embedded texture blob at +0x18A0. Vertex
   profile = a TAPERING SPIKE: ring r≈1.6 at y=0 narrowing to a point
   at y≈9.9 (authored ~9.6 u tall, ~3 u wide at base). No clip bank
   (one node, no +0x40 bind); anim-mode method 0xA = `func_001CB1F0`
   → `func_001CB140(self, +0x44)` raw-blob draw submit.
2. Bone-count/matrix-slot boilerplate (same as func_001C4820 INIT):
   `func_001C6150(+0x44)` → +0x0C (=1), `func_001AF780` slot → +0x110,
   +0x09 = count; matrix pool short (`D_00275BCC`) → state 3 bail.
   `anim_bone_array_setup` + `bone_init_default_1`; state=1; **+0x00
   = 2** (class-2 byte write; the brain never publishes to the
   category lists — it skips func_001B17A0 entirely — so it is not
   shootable/acquirable in practice; mailbox +0x36 and HP +0x34 are
   NEVER read: indestructible, like its parent).
3. +0x30 = &D_00275450 (generic 2-float bounds rec {4.0, 14.0};
   defensive — the brain never reads it).
4. Per-room TINT from `D_00246800` (22 recs × 8 B: {u32 key =
   AREA<<8|ROOM; u8 c0,c1,c2,c3}; key match vs
   `D_00810700<<8|D_00810701`): scratch+0x00/04/08 = c0/c1/c2 as raw
   floats, scratch+0x0C = c3/128; actor RGB mult +0x80/84/88 =
   c0/c1/c2 ÷ 128 (defaults 1.0 if no key matches), +0x8C = c3/128
   (W/alpha; office AREA02 recs: (128,128,128,2), (128,102,122,2)…).
5. +0x38 = 1.0f; clear the 12 record valid-flags (+0x1F0+0x84+i*0xA).

### Scratch layout (actor +0x1F0, inside the 0x100 tail)

    +0x00..0x0B  f32×3  room tint c0..c2 (RAW 0..255 floats)
    +0x0C        f32    c3/128 (alpha/W target)
    +0x10/14/18  f32×3  deploy ANCHOR (player X, pad Y, player Z at trigger)
    +0x1C..0x78  12 × {f32 X, f32 Z}              tendril world positions
    +0x7C..0xF2  12 × {s16 phase, s16 vel, s16 ramp, s16 girth, s16 valid}
                 (stride 0xA)

### TICK func_001549C0 (0x540) — sub-state +0x05 machine

- **0 SCAN**: gate `func_00154460(self)`: gameplay frames only (spad
  `0x70003B8D`==0) AND player inside |Δx| ≤ 3·recX, |Δz| ≤ 3·recZ,
  |Δy| ≤ 3+recY of the PAD origin (rec = `D_00248120 + (+0x0D)*20`,
  i.e. **3× the parent pad's footprint**). On trigger: anchor =
  (player X, pad Y, player Z), timer +0x28 = 8, sub→1; then for each
  of 12 records: target = anchor + polar(r, θ) with θ uniform and
  **r = 5.5±2.0 u (pair idx 0) / 7.0±2.5 u (idx 1)** — the two pair
  members are concentric rings; valid flag +0x84 =
  `func_001545B0(self, X, Z)` = point inside the ELLIPSE of semi-axes
  0.92·recX / 0.92·recZ around the pad (heading via atan2
  func_001B1240, ellipse-radius-at-angle formula) — tendrils only
  erupt from the pad's own footprint; phase +0x7C = rand 48..127,
  vel +0x7E = 0, ramp +0x80 = 0, girth +0x82 = D_0026D320[(seed++)&3]
  (cycling table {0xB4,0xDA,0xFF,0x180}/256 = 0.70/0.85/1.00/1.50,
  random start row). If ANY record valid → **sound 0x42D** at range
  300 (`func_001FBD50`; soundmap: 88 ms squelch sfx/snd_0615.wav,
  every-area variant). Falls through to sub 1 same tick.
- **1 DEPLOY**: all 12 ramps += 37/tick clamp 300; +0x28-- → 0 → sub 2.
  (8 ticks ≈ 0.13 s rise.)
- **2 HOLD**: retract (timer 8, sub→3) if: non-gameplay frame, OR
  |playerY − padY| > 3+recY, OR player leaves the anchor: dist²XZ
  (player, anchor) ≥ 4.0 (idx 0) / 16.0 (idx 1) — i.e. the field
  stays up only while the player stands within 2 u / 4 u of where
  they triggered it.
- **3 RETRACT**: ramps −= 37/tick floor 0; +0x28-- → 0 → sub 4.
- **4 RESET**: sub = 0 (rescan; the field re-deploys indefinitely).
- **Tail every tick**: if parent ptr +0x20 != 0, blend own RGB toward
  GREEN as the pad opens: ph = parent+0x80 (breather phase 0..1),
  RGB = (base + (1−ph)·(roomC − base))/128 with base = (6, 92, 1) —
  room tint when closed → vivid green at full open. Then
  `func_001C6380` (TRS matrix rebuild) and, when sub != 0,
  `func_00154F00` (the field renderer).

### RENDER func_00154F00 (0x2A4) — 12 re-posed draws of one spike

Saves own pos; for each VALID record: bob physics on (phase +0x7C,
vel +0x7E): phase += vel; **pad open >0.5 (parent +0x80, flag s2):
vel −= 8/tick with a +28..41 kick whenever phase < 128 — violent
thrash; closed: vel −= 1/tick, +3..7 kick below 128, vel halved at
≥8 — gentle bob**; floor clamp: phase < 100 → 100, fresh vel 3..7.
Then per-instance TRS: scale X/Z (+0x60/+0x68) = girth/256, scale Y
(+0x64) = phase·ramp/65536 (deployed: ≈0.45 at rest-bob → ≈1.0
thrashing → spike stands 4.4..9.6 u), alpha +0x8C = scratch+0x0C
(faded in over ramp 0..16); pos.xz = record X/Z; `func_001C6380` +
the +0x4C method (func_001CB1F0 draw). Restores pos + matrix after
the loop. The 24 spikes of a pad pair are 24 re-posed submissions of
the SAME 96-vert mesh.

### Port contract (em_enemy "tendril field", companion of `enemy generator`)

- Spawn: when a generator pad rolls mode 1, spawn TWO tendril-field
  actors at the pad origin (pair idx 0/1), parent-linked. Kill them
  with the room (nothing else kills them).
- Per actor: 12 slots; on trigger (player in 3× pad footprint, pad Y
  band ±(3+recY)) scatter targets on ring r = 5.5±2.0 (idx 0) /
  7.0±2.5 (idx 1) around the player, REJECT points outside the
  0.92× pad ellipse; play sound 0x42D (range 300) if any survive.
- Deploy 8 ticks up (ramp 0→296/300), hold while player within
  2 u (idx 0) / 4 u (idx 1) of the trigger anchor, retract 8 ticks,
  rescan. No damage, no HP, no collision.
- Visual: spike mesh `chunk03/f13_id15.bin` (96 verts, 1 node,
  embedded texture; exportable as static EMDL — r1.6 base, 9.9 tall),
  12 instances, scale X/Z ∈ {0.70,0.85,1.00,1.50}, scale Y =
  phase·ramp/65536 with the bob/thrash integrator above; RGB mult =
  room tint → (6,92,1)/128 green as the parent pad's open phase → 1;
  alpha = roomC.w/128 faded in over the first 16 ramp units.
- Tunables table: trigger 3×footprint; rings 3.5–7.5/4.5–9.5;
  ellipse 0.92; ramp step 37, cap 300; hold radii 2/4 u; phase floor
  100, kicks 3..7 (idle) / 28..41 (open), gravity 1 (idle) / 8
  (open), vel damp ×0.5 at ≥8; girths {180,218,255,384}/256.

### Matching status (all five committed as stubs; 2 wall datums added)

- `func_001546C0`: hybrid asm-void stub stays byte-matching ("word").
  Readable-C `switch` reproduces 29/30 instructions but hits **wall
  #13**: CW leaves the beq(case 2) delay slot as nop, mwcc fills it
  with the next chain constant (`addiu v1,1`) — one-instruction-short
  body, every later branch off by one. Same family func_00153B50
  documented as unfixable.
- `func_00154460`: readable attempt 67.5% structural — three mwcc
  policy walls: (a) early-`return 0` sites — CW emits `b common-tail;
  v0=0 in slot` + nop after bc1t, mwcc fills the bc1t slot and inlines
  the epilogue at the first return; (b) table-address pair vs `lbu`
  kind-byte scheduling order; (c) final `return cond;` polarity (CW
  bc1t + dead addiu, mwcc bc1f). Recorded in the src stubs; not
  worth further attempts until a #13-family crack lands.

## SCENE_OFFICE0 CRATE CARVED — the n0 table's 0x0D shipped per-scene; husk verdict (2026-06-10, session 34)

Closes the s28 flag "per-scene crate models in the port": the 17 placed
crawlers of AREA02 sub-state 0 bind the **n0 leaf table's entry 0x0D**,
not the sub-state-1 cardboard box the port's global `enemy_crate.emdl`
carries.

### 1. The carve (export_props.py --crate-dir, new)

`--crate --crate-dir extract/chunk06.n0` switches the crate machinery to
the s28 concat view (`export_level.office0_concat`; the n0 table at
concat `0x373800` = f04_id72+0xD800 has entries crossing into f05_id41)
and to the s28 GS-upload-replay texel path (`office0_uploads`:
f02_id44 + chunk27/f00_id35; `--uploads` overrides). Entry 0x0D lands at
**concat 0x45E180 (inside f05_id41)**: 1 block, 1 node, size 0x860 → 23
welded verts, **12 tris, a plain 14×14×14 box** (bbox X[-7,7] Y[0,14]
Z[-7,7]), one **PSMT4 128×128, TBP0 0x2CA0 CBP 0x34F1**, resolved 1/1
from the replay (leaf pack dbp 0x2A00), 0 flat fallbacks.

**Wording correction (s28/commit 24cc7a0):** this is the AREA11 crate's
SIZE CLASS, not its mesh — the AREA11 entry is a 6-block/90-tri mesh
with real bevel geometry; the n0 entry is a 12-tri cube whose "bevel"
is painted into the skin. Texture (verified visually): dark horizontal
wooden planks, riveted metal corner straps along the edges, yellow
stenciled freight markings — reads as a wooden shipping crate, matching
the scene's industrial palette. (The default `--crate` n1 path
regression-checked byte-identical to the shipped global asset.)

### 2. Husk/gib verdict: the n0 table carries NO burst set

Full mesh-level survey of all 27 n0 entries (per-entry nodes/tris/bbox/
texture keys + SHA1 against the global library's gib set 0x1C–0x29):
**no n0 entry content-matches the global gib library, and none has the
14×14-footprint low-profile husk shape** (the only small props near
0x0D: 0x0C = a 7×8×3 4-texture fixture, 0x12/0x13/0x14 = small clutter).
So the office0 crates' burst husks/shards are the GLOBAL chunk27 models
(0x22/0x29 + shards), exactly as the rebind path requires — D_0028A56C
is the area-independent global library (s23) — and they are ALREADY
shipped as `assets/gibs/`. Nothing per-scene to carve.

### 3. Shipping: assets/scene_office0/props/enemy_crate.emdl + a one-line hook

The port (read-only this session) cannot pick a per-scene crate today:

- `em_enemy.c` loads the fixed global path `CRATE_ASSET =
  "assets/enemy_crate.emdl"` (line ~180; load at ~642) — no scene-local
  probe exists.
- A scene-dir-local EMDL canNOT simply sit at the scene top level:
  `scene_load` (em_game.c:556) slurps EVERY top-level `*.emdl` of
  `assets/scene` as static level geometry posed at frame 0 — the crate
  would render at the world origin as scenery. Subdirectories are not
  slurped (the `doors/` precedent), so the asset ships as
  **`assets/scene_office0/props/enemy_crate.emdl`** (git-ignored,
  disc-derived; generic name so the hook below stays scene-agnostic;
  the originally-suggested top-level `enemy_crate_n0.emdl` was rejected
  for the slurp reason).
- **The one-line port hook (documented, NOT applied):** in
  `em_enemy.c` `crate_mesh_get`, probe the scene-local path first —

      if (em_model_load(&s.crate_model, "assets/scene/props/enemy_crate.emdl") == 0 ||
          em_model_load(&s.crate_model, CRATE_ASSET) == 0) {

  (replaces the single `em_model_load(..., CRATE_ASSET)` condition).
  Under the EM_SCENE shadow stage `assets/scene` IS the active scene
  directory, so every scene may carry `props/enemy_crate.emdl`; scenes
  without one (and the default scene) fall back to the global box —
  byte-identical default behavior. Both crates are 1-bone, so the
  CRATE_BONE_MAX path needs no change.

### 4. Verification

- Visual (temp scene copy, EM_SCENE + spawn-override; cleaned up): the
  carved EMDL slurped at the origin renders beside the player as a
  dark wooden plank crate with metal-strapped edges at the correct 14-u
  scale (~chest height on the player) on the office0 conveyor floor;
  the green tint at its base is the player's additive aura billboard,
  not a texture fault.
- **Default and office0 captures are byte-identical with the asset
  absent vs present** — honestly: nothing loads `props/` today, so the
  asset is inert until the hook lands; the identity checks confirm
  shipping it changes nothing.
- make test-input + EM_MOVE_TEST/EM_DOOR_TEST/EM_WEAPON_TEST/
  EM_ENEMY_TEST=1..4 all PASS.

Open: apply the crate hook port-side; the n2 sub-state's table (concat
0x3E9000, count 27) almost certainly carries the same entry 0x0D —
carve with `--crate-dir --crate-table-off` when that scene exports.

## TENDRIL SPIKE ASSET SHIPPED — chunk03/f13_id15 exported; untextured by design (2026-06-10, session 35)

The s33 ("KIND-0xE COMPANION RESOLVED", commit 663d358) spike mesh is
now a port asset: **`assets/tendril.emdl`** (port repo, git-ignored) —
static 1-node EMD2, 96 records → 31 welded verts / 54 tris, local bbox
X[-1.39,1.39] Y[-0.02,9.65] Z[-1.63,1.63] (the authored ~9.6-u tapering
spike on an r≈1.6 base ring). Recorded CLI (export_props.py docstring):
`--crate --crate-blob extract/chunk03/f13_id15.bin --out
../extermination-port/assets/tendril.emdl`.

### Texture verdict — 0 textures, honestly (s33 correction)

The blob references NO texture: all 96 records carry TEX0 qword 0 and
UV (0,0); the attr rows are unit normals; and the +0x18A0 tail is the
1-node MODEL RECORD (parent -1, identity 4×4 rest) + zero pad to
0x2000 — **s33's "embedded texture blob at +0x18A0" was wrong**. A
GS-upload packet scan of the file (export_level.read_uploads_localmem)
finds 0 transfers. Both candidate texel sources are therefore moot:
the export is byte-identical with and without `--p2s scratch/state01`
(and `--gsdump` would be too) — coverage is 0/0 referenced/resolved,
not a fallback. This is consistent with the s33 render contract: the
engine colors the spike entirely through the actor RGB multiplier
(room tint → (6,92,1)/128 green with the pad's open phase) over the
normal-lit mesh; the EMDL bakes the standard normals→grayscale
stand-in light.

### Verification (port, read-only; asset-side only)

- Temp scene copy (default office scene + the spike placed on open
  floor at (99, 0, -179), 12 u left/front of spawn, via a one-off
  translated-palette EMDL): `EM_SCENE` + `EM_CAPTURE` frame-60 BMP
  shows a tall slender SPIKE rising from the floor beside the player —
  flared faceted base narrowing sharply, then a thin column tapering
  to a point at ~9.6 u; smooth-shaded neutral grey (the runtime green
  tint is the renderer's job). Reads exactly as the s33 "tapering
  organic spike/stalagmite". Temp scene removed after capture.
- Default capture byte-identical before vs after shipping the asset
  (nothing loads `assets/tendril.emdl` today — it is inert until the
  tendril-field renderer lands; stated honestly).
- `make test-input` PASS; EM_MOVE_TEST / EM_DOOR_TEST / EM_WEAPON_TEST
  / EM_ENEMY_TEST=1..4 all PASS.

### Port-side work remaining (em_enemy "tendril field", per the 663d358 contract)

All engine constants already pinned in s33's "Port contract" block;
nothing else needs decomp work:

- Spawn: a generator pad rolling mode 1 spawns the PAIR (idx 0/1) at
  the pad origin, parent-linked; dies only with the room.
- 12 instances per actor: trigger scan (player within 3× pad
  footprint, ±(3+recY) Y band), ring scatter r = 5.5±2.0 (idx 0) /
  7.0±2.5 (idx 1) around the player, reject outside the 0.92× pad
  ellipse, sound 0x42D range 300 if any survive.
- Ramp/bob/thrash scale-Y animation: deploy ramp +37/tick cap 300,
  hold while player within 2/4 u of the anchor, retract −37/tick;
  scale Y = phase·ramp/65536 with the bob integrator (floor 100,
  kicks 3..7 idle / 28..41 pad-open, gravity 1/8, vel ×0.5 at ≥8);
  scale X/Z = girth {0.70, 0.85, 1.00, 1.50} cycling.
- Room-tint blend: RGB mult = room tint → (6,92,1)/128 green as the
  parent pad's open phase → 1; alpha = roomC.w/128 faded in over the
  first 16 ramp units. 24 spikes of a pair = 24 re-posed draws of
  this one EMDL.

## KNIFE/MELEE DECODED — SQUARE action identified, both attack machines characterized (2026-06-10, session 36)

Static decode (splat asm of the boot ELF + boot-ELF `.data` reads; no
live session — PCSX2 MCP unavailable). **Retires the s29 open item
"0x179 / SQUARE = unidentified action"** and characterizes the knife
attacks end-to-end. Port: knife attacks shipped in
`extermination-port` (em_weapon.c melee section, `EM_MELEE_TEST=1`).

### 1. Dispatch — the knife is on TWO buttons, not tap-vs-hold

The action machine `func_001607D0` (mode-0 sub-state 1, after the
draw-mask checks) dispatches from the unarmed action codes `+0x1F0`
0..7 (idle + the movement actions, via `func_0017C370` stop-move):

```
held  E70 & spad3B7E (R2)      -> mode 0x1E, code 0x32   (armed stance)
held  E70 & spad3B7C (R1)      -> mode 0x1D, code 0x31   (armed stance)
press E74 & spad3B78 (CIRCLE)  -> mode 0x21, code 0x36   KNIFE LIGHT COMBO
press E74 & spad3B74 (SQUARE)  -> mode 0x22, code 0x37   KNIFE HEAVY STAB
```

`+0x1FC` (anim rate for the melee entry) is set to 1.0f at the
dispatcher head. Mode 0x21 -> `func_001735C0`, mode 0x22 ->
`func_00173E60` (jtbl_0026D3B0 indices 0x21/0x22; the s22 weapon
write-up's mode map extends cleanly). So: LIGHT melee is on the FIRE
button while unarmed; SQUARE is the HEAVY. There is no hold/tap split.

In the ARMED stances (codes 0x31/0x32/0x34/0x35) SQUARE instead calls
`func_0017A970(player, press?0:held?1)` = the SUB-WEAPON action,
switching on the attachment id `D_00810CA6`:

```
att 0: press toggles D_00810D3C: 0->1 plays sound 0x179 vol 300 +
       sets the voice latch D_008106C7; 1->0 silent.  <- the s29 capture
att 1: press -> func_0017A8B0(p,3)     att 2: press -> func_0017A8B0(p,1)
att 3: hold  -> func_0017A8B0(p,2)     att 4: hold  -> func_0017A8B0(p,1)
aim option D_00810CA4==2: press -> func_0017A8B0(p,5)
```

`D_00810D3C` is also read by the draw helper `func_0016F530`
(rifle draw: if att==0 && D3C -> replay 0x179 + D_008106C7=1) and
cleared by the inventory reset `func_001AF2C0`. **What the flag arms
is still OPEN** (a persistent attachment-0 mode; candidates: a
sub-weapon arm/laser mode toggle — needs a live session).

### 2. func_001735C0 — LIGHT 3-hit combo (player mode 0x21)

Major `+0x06`: 0 entry -> 1/2/3 = combo hits -> 0x50/0x51/0x52
hit-confirm recover -> 0x63/0x64 exit ramp (func_0017C440 at gait
+0x23F>=2, else func_0017C540 direct; 0x6E/0x6F-style hit/forced
exits = the same 0x63/0x64 handlers as the armed tops). Phase `+0x07`
within each hit: 0 start anim -> 1 wait blend (+0x200 bit 0x8000) ->
2 impact gate -> 3 release/chain -> 4 wait clip end (+0x200 bit
0x1000). Tail every tick: func_001764E0 (torso), +0xB4 -= 0.2,
func_00175900(.,1), func_001796C0 — identical to the armed tops.

Per-hit data (row idx = `+0x236`, see §5; all reads verified against
the original boot ELF `config/SCUS_971.12`):

```
                          row 0 (normal)      row 1 (alt context)
anim ids   D_00248690     0x10B 0x10C 0x10D   0x1BD 0x1BE 0x1BF
impact T   D_002486A0/A8/B0   24    26    41      25    28    41
release T  D_002486A4/AC/B4   20    15    30      20    16    34
chain T    D_002486D0/D4      19    19    (--)    19    19    (--)
damage     (immediate)         3     3     5   (same)
sound      (immediate)     0x17D 0x17E 0x17F  (same)
+0x25E marker              0x81  0x82  0x82   (same)
```

- At the impact gate (`+0x3C` vs T, c.le.s): target `+0x00 = 1`
  (event ping), target `+0x36 = damage` (the universal mailbox),
  positional sound vol 300 — ALL unconditional (range is the
  target's job, §4), `+0x25E = 0x80|n` (consumed by func_00187350's
  melee arm -> func_00182430(p, n), the step-effect dispatcher).
- COMBO: FIRE-button presses (E74 & spad3B78 — the same mask that
  started the combo) buffer in `+0x2E` from the impact phase on; at
  the chain gate the next hit starts through the clip arbiter
  func_001749A0 with f12=1.0 (blend), target `+0x00 = 2` (release).
- HIT CONFIRM: phase 3 polls target `+0x0A`; nonzero -> stop the
  sound handle `+0x302` (func_0011A070) and jump to recover 0x50.
  **A landed hit SKIPS the rest of the combo**; chaining only happens
  on whiffs.
- RECOVER 0x50/0x51: `+0x28 = 4` countdown, then anim 0x10F (row 0)
  / 0x1C1 (row 1) at blend 4.0; 0x52 waits clip end -> 0x63.

TIMING (OPEN): `+0x3C` is the up-counting clip time (the property-
table footstep semantics), under which `c.le.s(+0x3C, T)` passes on
the first impact-phase tick — impact ~right after the blend. But
hit 1's T=24 against its 50-frame clip only makes sense counting
DOWN (impact at frame 26, mid-swing); for hits 2/3/heavy T >= clip
length so both readings agree (immediate). Needs one live capture.
The port uses `impact = max(3, len - T)` (the down-count reading).

### 3. func_00173E60 — HEAVY stab (player mode 0x22)

Same skeleton, single attack: major 0 entry (anim halfword from
`D_002754A8[+0x236]`: row 0 = **0x10E** (20 fr), row 1 = 0x1C0; rate
+0x1FC=1.0) -> 1 wait blend -> 2 impact gate `D_00248700[idx]` = 43.0
(both rows): target event 1, **damage 0xF = 15**, sound **0x17F** vol
300, marker `+0x25E = 0x83` -> 3 release gate `D_00248704` = 29.0
(event 2) / hit-confirm (+0x0A) -> recover 0x50 -> 4 wait clip end ->
0x63. States 2/3 call `func_00173DD0` each tick: yaw `+0xC4` steered
toward the goal `+0x218` at `pi * D_002486F0[+0x23F] / 180` rad/tick
(D_002486F0 = {0, 0.5, 1.0, 2.0} by gait) — the heavy TRACKS while
swinging. Recover anim row 0 = 0x10F again (idx-0 sound... anim
branch at +0x236==0), row 1 = 0x1C1.

Clip lengths (id 0x74 library, exported s36): 0x10B = 50 fr,
0x10C = 25, 0x10D = 20, 0x10E = 20, 0x10F = 25 (property-table rates
all 1.0, trigger frames all 0 — like the aim/door ids).

### 4. Damage contract — the melee-target link player+0x18

Both machines write through `player+0x18` (global mirror
`D_008102C8`): `+0x00` event byte (1 = swing live, 2 = release),
`+0x36` damage mailbox, `+0x0A` read back as hit-confirm; state 0
clears `+0x0A` and parks the sound handle `+0x302 = 0xFF`. **No range
test exists in the player code** — enemy behaviors poll the link
themselves: `func_00219870` (creature, state 1 sub 0) reads
`(*D_008102C8)->+0x00` and runs its own `func_0019AA80(ctx, ctx+0x10,
0x40)` proximity/segment probe (the same query family as the leech's
0x20 grab sense). The +0x18 WRITER was not found statically (no
direct stores; likely established at player init via block copies) —
identity of the linked record (per-enemy target vs a fixed melee
agent record) is OPEN; the s17 note "writes target(+0x18)->+0x36
directly" stands. The +0x36/+0x0A semantics match the enemy-side
mailbox + group-alarm fields, so the port damages the resolved victim
through the same mailbox (reach stand-in: 12 u = the documented
use-scan hands-reach dist^2<=144, s17; cone 60 deg — both flagged).

### 5. +0x236 — the alternate melee row (anim ids 0x1BD..0x1C1)

Set to 1 by `func_001764E0` when the player's height sits beyond a
13.8-unit threshold against the floor-probe result (spad 0x700031B4
block) under a mode flag; cleared by `func_00179680`. Selects row 1
of every melee table (and the 0x1C1 recover). Context unverified
(elevated/hang/ladder family); the port ships row 0 only.

### 6. Knife visual — NO rebind found (flagged)

The knife (model 106) rides the hip HOLSTER node 14 (s9). Neither
melee machine nor their helpers (func_00173DD0 / func_001749A0 /
func_0017C370/C440/C540 / func_00174AC0 / func_00178B90) touches the
equipment draw slots (+0x120 family / D_008103D0), and no mode-0x21/
0x22 keyed attach-table switch was found in the main ELF. If the
engine shows the knife in hand during attacks, the rebind lives in
the equipment-draw selection (player-blob attach table) and needs a
LIVE melee-frame capture (the s9 method) to pin. The port leaves the
knife holstered during attacks (flagged note in em_weapon.h).

### 7. Sound ids (extend the s29 live table; soundmap-resolved)

```
0x17D  light hit 1 impact/swing   (snd_0432, 47745 Hz, global)
0x17E  light hit 2 impact/swing   (snd_0434, 53592 Hz, global)
0x17F  light hit 3 + HEAVY impact (snd_0430, 47745 Hz, global, 2 ev)
0x179  attachment-0 SQUARE toggle-ON (snd_0436 — the s29 capture)
```

`tools/gen_sfx_registry.py` office ids extended with all four
(19 ids, 0 unresolved).

### 8. Port (extermination-port, this session)

- `player.emdl` re-exported with the knife clips appended (superset
  discipline: old export reproduced byte-identical from the recorded
  CLI first, then `--clips ...,267,268,269,270,271`; old
  clips/palette verified as byte prefixes, default capture
  byte-identical).
- em_weapon.c gained the melee machines (engine states in comments),
  em_game.c the melee movement plant + `EM_MELEE_TEST=1` (two crates
  in knife reach but outside their proximity trigger: light-combo
  kill via the mailbox, hit-confirm recover 0x10F asserted, heavy
  kill, whiff-combo 0x10B->0x10C->0x10D chain asserted, no recover on
  whiff, 5 swings / 2 hits / 0 shots) — PASS; all prior self-tests
  PASS; SQUARE while aiming = the 0x179 toggle.

### Open items

- Live-verify the impact-frame reading of the +0x3C gates (§2).
- Identify what D_00810D3C arms (§1) and the +0x236 context (§5).
- The player+0x18 record's identity / writer (§4).
- ~~The melee rows' +0x25E -> func_00182430(p, 1..3) effect content.~~
  **(s37: resolved — func_00182430 is the surface-footstep mapper; the
  melee gates plant 0x80|n so the footwork plays a surface step at
  intensity n. See "FOOTSTEP SURFACE TABLE".)**
- Whether the knife visually rebinds to the hand mid-attack (§6).

## FOOTSTEP SURFACE TABLE — the surface→sound mapping fully decoded (2026-06-10, s37)

Static .s read of the per-frame footstep slice `func_00187350` and its
whole call tree (`func_00182430` mapper, `func_00179B90` variant rand,
`func_00187EE0` step decal/FX, `func_00175900` footing update,
`func_00187DC0/DE0/EA0` first-contact one-shots), cross-checked against
the office collision grid and the soundmap. **Closes the s29 open item
"the per-surface id table is NOT yet located statically" — there is no
data table.** The mapping is compiled-in immediates inside
`func_00182430`: a two-level switch (surface attr × gait) plus a random
variant. Headline formula, per footstep:

    surface_id = BLOCK(attr, depth) + GAIT_SUB(gait) + rand5()
    gear_id    = 0x138 + rand5()          (independent second rand5)
    both: positional play_sound(actor, id, 0) at volume 300

`rand5()` = `func_00179B90` = `(rand() & 7)`, values 5..7 folded to
0..2 — returns 0..4 with 0/1/2 twice as likely as 3/4.

### Where the surface attr comes from (actor +0x23A)

`func_00175900` (the footing update in the player spine, s17): after
the floor probe `func_0019B6C0` hits, it copies **the collision result
record's surface-attr byte +0x1A** (via the spad result pointer
`*0x700031D0` — for grid hits this points at the 64-byte poly node, s14)
into **actor +0x23A**, and the floor height `0x700031B4` into +0x250.
Not-grounded branch: a second probe `func_0019B8C0` (mask 7) supplies
the attr, else +0x23A = 0; **standing on a movable object** (+0x214,
type byte 2 masked ~0xE1 == 4) overrides by object subtype: subtype 2 →
attr 2; subtypes 0xA/0xC/0x18/0x2A/0x28 → attr 4.

First-contact latches (cleared whenever the probe misses):

- attr **0x5A** (latch +0x23D): `func_00187DC0` → play_sound(actor,
  **0x86**, 0) vol 300 (global bank: 4-WAV splash set 0069/0073/0132/
  0133 — puddle/wet-surface entry).
- attr **0x5B** (latch +0x23C): depth probe at floorY − 4.01 →
  **+0x23C = 1 (shallow) / 2 (deep)** — the WATER DEPTH state — then
  `func_00187DE0`: effect 0x80000016 at the hit point, loop level
  `func_001E8B90(pos, 5.0)`, and `func_001FB9F0(0xCA shallow / 0xDB
  deep, 0x1000 ×3)` (non-positional water-entry one-shot).
- attr **0x5C** (latch +0x23E): `func_00187EA0` =
  `func_001FB9F0(0xA8, 0x1000 ×3)`.

### The gait selector (mapper arg a1)

`func_00182430(actor, a1)` picks the sub-base from a1: **a1==3 → +0xA,
a1==2 → +5, else (0/1) → +0**. a1 = **actor +0x25C**:

- unarmed walk update `func_0016A8B0` RAMPS +0x25C ±1/frame toward the
  gait target **+0x23F** (the stick quantizer 0..3, s31) and uses it to
  index the speed table `D_00248640`;
- the armed locomotion top `func_001612D0` writes **gait − 1** (locIdx
  0..2) into the same byte.

So unarmed: walk (gait 2) → +5, run (gait 3) → +0xA, creep → +0; armed
locomotion tops out at a1==2 (+5). The melee machines bypass the gait
byte: their impact gates plant **+0x25E = 0x80|n** (s36) and
`func_00187350`'s 0x36/0x37 (and default) action paths call
`func_00182430(actor, n)` directly — footwork at scripted intensity.

### BLOCK(attr) — the material blocks (stride 0x11 = 17 ids)

Each material owns 17 consecutive ids: 3 gait sub-bases × 5 random
variants = 15, + 2 spare slots (base+0xF/+0x10 hold 3-/4-WAV random
sets in the banks — landing/scuff family, not fired by this path).

| attr (+0x23A)        | base  | ids (gait 0-1 / 2 / 3)           | global-bank WAVs (chunk00/f05 sfx) |
|----------------------|-------|----------------------------------|------------------------------------|
| 0 and any unmapped   | 0x10  | 0x10-14 / 0x15-19 / 0x1A-1E      | 0040,0055,0054,0053,0039 / 0042,0052,0051,0049,0047 / (level banks) |
| 1                    | 0x21  | 0x21-25 / 0x26-2A / 0x2B-2F      | 0037,0035,0036,0033,0032 / 0034,0030,0031,0028,- / - |
| 2                    | 0x32  | 0x32-36 / 0x37-3B / 0x3C-40      | 0016,0000,0012,0009,0007 / 0018,0017,0001,-,- / - |
| 3                    | 0x43  | 0x43-47 / 0x48-4C / 0x4D-51      | 0109,0107,0108,0105,0091 / 0106,0103,… / … |
| 4                    | 0x54  | 0x54-58 / 0x59-5D / 0x5E-62      | 0089,0090,0088,0087,0083 / 0086,… / …,0080,0081,0077 |
| 5                    | 0x65  | 0x65-69 / 0x6A-6E / 0x6F-73      | 0076,0074,0073,0071,0069 / - / -,0063,0061,0059,0058 |
| 6, 7                 | 0xA9  | 0xA9-AD / 0xAE-B2 / 0xB3-B7      | level banks only |
| 8                    | 0x87  | 0x87-8B / 0x8C-90 / 0x91-95      | 0131,0130,0129,… / … / 0122,0120,0119,0118,0116 |
| 0xD                  | 0xDC  | 0xDC-E0 / 0xE1-E5 / 0xE6-EA      | level banks only |
| 0xE                  | 0xED  | 0xED-F1 / 0xF2-F6 / 0xF7-FB      | level banks only |
| 0x5A (wet/puddle)    | 0x76  | 0x76-7A / 0x7B-7F / 0x80-84      | 2-event splash+step pairs (0150/0153, …) |
| 0x5B shallow (+0x23C==1) | 0xBA | 0xBA-BE / 0xBF-C3 / 0xC4-C8  | level banks only |
| 0x5B deep (+0x23C==2) | 0xCB | 0xCB-CF / 0xD0-D4 / 0xD5-D9      | level banks only |
| 0x5C                 | 0x98  | 0x98-9C / 0x9D-A1 / 0xA2-A6      | 0113,0192,… |

WAV resolution is per-loaded-bank (the s37 soundmap rule): the "-"
ids above simply have no record in the GLOBAL bank — area banks fill
them (s29's storage room resolved 0x1A→snd_0050, 0x1B→snd_0048).
The GEAR layer 0x138-0x13C (cloth foley, base 0x138 + rand5) resolves
globally: 0311/0308/0309/0310/0306.

### s29 CORRECTIONS

1. **"Floor A / floor B" was a GAIT change, not a material boundary.**
   0x15/0x16 = block 0x10 at gait 2 (walk); 0x1A/0x1B = the SAME block
   at gait 3 (run) — the player went from walk to run mid-room on one
   material. (a1==3 is only reachable unarmed → the capture was
   unarmed locomotion.) The actual material-change signature is a jump
   of the whole id by ±n·0x11.
2. **Neither layer alternates strictly L/R.** Both layers draw rand5()
   independently per step; the observed 0x15/0x16 and 0x139/0x13A
   "pairs" were the biased low values. 0x138 "observed once" = the
   same gear rand landing on 0.

### Per-step decal/FX layer (`func_00187EE0`, attrs again)

attr 0: while the wet-feet timer +0x212 is ticking → **wet FOOTPRINT
decal** `func_001F0460(1, foot)`, else dust effect 0x80000011;
attrs 1-4, 0xD, 0xE: none; 5 → effect 0x80000028; 6 → 0x80000005;
7 → 0x80000068; 8 → 0x80000066; 0x5A → 0x80000065 at floor Y;
0x5B → 0x8000001D (water ripple) at floor Y; 0x5C → 0x80000067 at
floor Y. Foot positions: the locomotion path passes skeleton globals
`*(D_00275B40)+0x44/+0x48` (+0xC0/+0x90 into each = L/R foot node);
the default mailbox path uses actor +0xB0/+0xD0.

Tail of `func_00187350` (every frame): **wet-feet timer** +0x212 = 120
while attr ∈ {6, 0x5B}, else decrements; **wade layer** while in water
(+0x23C ≠ 0, byte D_00810700 != 0x15, and moving): ripple effect
0x8000001D at the player + level `func_001E8B90(pos, 0.3·speed)`
(speed +0x38; halved-rate gate on spad 0x70003B68 frame counter & 3).

### Office cross-check (read-only, export_collision grid decode)

`extract/chunk06.n1/f02_id44.bin` grid section @0x79000 (121 nodes):
the floor poly under the live player position (105.9, −184) is
**attr 0 → block 0x10** — exactly the s29-observed block. Office
floor-attr census (ny>0.7): attr 0 ×11, 3 ×4, 4 ×9, 0xB ×7 (unmapped →
default block), plus singles 0x1F/0x37 (unmapped) and 0x50
(conditional-accept code, s14). No 0x5A/0x5B/0x5C in the office.

### Matching assessment (s37)

`func_00182430` (the mapper), `func_00187DC0`, `func_00187EA0`,
`func_00179B90` are already committed byte-equivalent as gated asm
(`word` form). `func_00187350` and `func_00187DE0` were assessed and
are **wall #13 blocked** (beq;nop sites whose fall-through candidates
are safe-to-speculate chain constants / lui — mwcc fills them): stubs
annotated with the analysis, no compile attempt burned.

### Port contract (the s30 floor-probe hook is ready)

- EMCL already carries the per-poly attr for GRID polys; **cell n-gons
  export attr 0** (the cell-prim attr byte location is still
  un-decoded — open). Office floors are grid polys, so the port is
  immediately correct there.
- Step fire (at the property-table frames, s25/s31):
  `id = block_base(attr, depth_state) + (gait==3 ? 10 : gait==2 ? 5 :
  0) + rand5(); gear = 0x138 + rand5();` both positional, vol 300.
  Unmapped attrs → block 0x10. rand5 = (r&7), 5..7→0..2.
- The port's current hardcoded walk pair stays correct for office walk
  (0x15+r) but must switch to 0x1A+r at run gait, and should drop the
  strict L/R alternation in favor of rand5.
- Water/wet extras when those materials appear in exported scenes:
  depth probe at floorY−4.01 → shallow/deep, entry one-shots 0x86/
  0xCA/0xDB/0xA8, wet-feet footprint decals for 120 frames after
  attrs 6/0x5B, ripple effect 0x1D + wade level while submerged.

### Open items (s37)

- The cell-prim surface-attr byte location (narrow/wide n-gon records;
  the result-ptr +0x1A read implies the byte exists in whatever record
  the cell path stages — possibly the wide header's +0x14..+0x23 span).
- Material semantics for attrs 1-8/0xD/0xE (concrete/metal/grate/…) —
  needs listening to the per-block WAV families or a live A/B.
- What byte `D_00810700` is (value 0x15 disables the wade layer and
  the +0x23C branches in func_00187350/func_00187DE0).
- The +0x212=120 pairing of attr 6 with 0x5B (wet-feet from a non-water
  material — blood/slime floor?).

## DOOR DESTINATIONS DECODED OFFLINE — static dest/spawn tables in the boot ELF; scene-switch goto links (2026-06-10, session 38)

Closes the s22 "authoring data per area" decode for AREA02 entirely
offline (boot ELF + extracted overlay; no live capture) and ships the
port's RUNTIME SCENE SWITCH on top of it. Everything below was
byte-cross-checked against the s22 live reads.

### 1. The transition authoring tables are STATIC in the boot ELF

ELF mapping note: the boot ELF's LOAD segment is file offset `0x94` →
vaddr `0x100000` (NOT 0x300 — the section header padding starts the
loadable content at `e_phoff`-adjacent 0x94; an earlier scratch decode
with +0x300 produced garbage tables shifted by 0x26C).

- **`D_0024E140`** (per-area door DESTINATION table pointers) is fully
  populated **statically** — all 16 slots point into main `.data`
  (0x24DF80..0x24E0xx), NOT overlay-filled. Notation refinement to
  s17/s20: `D_0024E140[area]` IS the table base; record =
  `D_0024E140[area] + 4*(door_id & 0x7F)`. Table length = next area's
  base (no stored count). AREA01 = `0x24DFA0`, AREA02 = `0x24DFC0`
  (4 records).
- **`D_0024D650`** (per-area spawn DESC) is statically populated for
  areas 0-3/7/8 (descs in `.data`, e.g. AREA02 → `0x24D610`); the
  `0x2755xx` slots (areas 1/4/6/10/11/13-15) are late-`.data` descs
  whose CONTENTS the area overlays fill at load (s22's "overlay init
  populates" — the pointer array itself is static). AREA02 desc
  `0x24D610` → sub-state spawn tables `{0x24B560, 0x24B6B0, 0x24B800}`
  (subs 0/1/2), 7 records of 0x30 each.
- Spawn record layout confirmed at +0: `{f32 pos[3], f32 yaw, u32
  flags?, u32, f32, ...}`. The s22 live-verified entries byte-match:
  sub-1 entry 2 = (104, 0, -245) yaw 0, entry 3 = (104, 0, -259) yaw
  pi; AREA01 sub-0 entry 5 = (39, 0, -225) yaw -pi/2.
- **All three AREA02 sub-state spawn tables carry IDENTICAL values**
  (three separate static copies) — the office floor's entry points
  don't vary by story state.

### 2. AREA02 door census (placement flags2 → dest record)

```
AREA02 dest table @0x24DFC0:
  door 0: {01 03 00 00}   door 1: {01 05 00 00}
  door 2: {03 02 00 00}   door 3: {04 00 00 00}

sub 0 (@0x827830): door id 0|0x80 model 0x15 (-30.5, 0, -187.3)
                     -> AREA 1 sub 0 entry 3 = (-25, 0, -197) yaw pi
                   door id 3|0x80 model 0x17 (440.2, 15, 109.9),
                     fn 0x001BB860 -> AREA 4 sub 0 entry 0
sub 1 (@0x828170): door id 1|0x80 (west m03, 57, 0, -220.5)
                     -> AREA 1 sub 0 entry 5  [s22 live-verified]
                   door id 2 (office m03, 109, 0, -252.2)
                     -> room move, entries {3, 2}  [s22 live-verified]
sub 2 (@0x8283D0): door id 0|0x80 (same as sub 0)
```

Cross-area records targeting AREA02 (the full 16-area scan): AREA01
door 1 → sub 0 entry 0 = (-35, 0, -178) yaw 0 (the m15 door's arrival
spawn); AREA01 door 3 → sub 1 entry 1 = (65, 0, -225) yaw pi/2 (the
west door's arrival — the s22 return path); plus 17 records from areas
4-15, almost all → sub 0 entry 1 (the elevator/hub entry).

### 3. NO real intra-area sub-state door link exists in AREA02

Every AREA02 door either changes AREA (ids 0/1/3 → areas 1/4) or room-
moves within its own sub-state (id 2). Sub-state 1 ↔ 0 transitions are
reached ONLY through other areas' doors (story routing). Generally:
dest records CAN encode same-area sub switches (`{02 xx 01 sub}` is
representable, and e.g. AREA03's table opens with `{03 01 01 01}` —
area 3 switching its own sub-state through a door), but AREA02 has
none.

### 4. Tooling + port (scene-switch machinery)

- `tools/export_level.py --door-goto DIR --sub N --exported
  sub=dir,... [--synthetic-link]`: annotates DIR/scene.txt door lines
  with `goto <sibling-scene-dir> <sx> <sy> <sz> <syaw>` from the
  decoded tables — emitted ONLY when the destination is an exported
  AREA02 sub-state; inter-area dests and room moves stay plain
  (documented in `# door-goto:` comment lines; idempotent).
  `--synthetic-link` wires the two exported office scenes' nearest
  doors (west m03 ↔ m15) with a FLAGGED synthetic goto — arrival =
  the target sub's REAL spawn entry nearest the partner door (sub-0
  entry 0 / sub-1 entry 1, exactly the records other areas use to
  arrive at those doors). Honest: only the door-to-door pairing is
  synthetic; the engine routes via AREA01 in between.
- Port (`extermination-port`): em_door goto tail → at fade-out black
  the commit posts a SCENE SWITCH (em_door_goto_pending) instead of
  the same-scene re-place; `em_game_scene_switch(dir)` frees the
  active scene (level meshes, collision, door + enemy pools — the
  engine's actor-pool free) and reloads everything from the new
  manifest WITHIN the running process; player/camera placed at the
  decoded arrival spawn while black; the render chain is re-recorded
  the same frame (the earlier-built chain points into freed tables);
  the transit-wide input lock + fade-in survive the door teardown
  (em_door_scene_clear). Player model, BGM, sfx registry persist
  (room-move audio semantics — the shipped links are intra-area).
  `EM_TRANSIT_TEST=1` exercises it end to end: PASS (scene → office0
  at frame 155 of the transit, player at (-35, 0, -178) yaw 0, new
  scene's 2 doors + 24 enemies + 1007-poly collision live, fade
  clean, default capture byte-identical).

> **2026-06-11 s45 CORRECTION:** the --synthetic-link west↔m15 wiring
> is REMOVED — AREA01 sub 0 (the drawbridge room, chunk05.n0) is now
> exported as scene_drawbridge, so every shipped goto is the REAL dest
> record (west door → A1.0 e5, office0 m15 → A1.0 e3, drawbridge doors
> 1/3 → the two office scenes). The user-confirmed office0 ACCESS path
> is the VENT (overlay-scripted; flagged synthetic door-line stand-in
> via --door-goto --vent). EM_TRANSIT_TEST now asserts the drawbridge
> arrival. See "DOOR USE SCAN + STAGING MATH DECODED ..." (s45).

### Open items (s38)

- The engine-side sub-state SWITCH mechanics for a same-area dest
  record (`{02 xx 01 sub}`-shaped): does the B8==1 loader skip the
  overlay reload when next_area == current? (AREA03's self-record
  suggests a fast path; needs one live capture in area 3.)
- AREA02 door 0 vs door 3 use models 0x15/0x17 (locked-door class —
  unlock bitmask D_00810841); the port's scene_office0 doors load
  their meshes but the locked sequence (subs 1/2) is still
  untranslated.
- The 0x2755xx spawn descs (overlay-filled areas): dump each AREAxx
  overlay's filler to complete the all-areas spawn map offline.

## STATUS SUB-PAGES — pager page identities, navigation remap, message bank, and texture export (2026-06-10, session 39)

Static decode of the five status-screen sub-pages flagged open in s25
("page sub-screen content layouts — each needs its own decode pass").
No emulator: page view .s files + the chunk00 message bank + GS-upload
replay of the page chunks. Port: page navigation skeleton shipped in
`extermination-port` (em_hud) with per-page texture sheets from the new
`tools/export_ui.py --page` mode.

### Page id → identity (pinned; names from the game's own message bank)

| page (+0x10) | hover | chunk | view fn | identity |
|---|---|---|---|---|
| 0 | 4 = left | 0x1F (extract/chunk31) | `func_0020EE50` | **ITEM SCREEN** — category hub; sub-modules 0x20 EQUIPMENT / 0x21 BATTERY / 0x22 EVENT / 0x23 HEALING (views `func_00214570`/`func_002149F0`/`func_00215870`/`func_002160B0`, states 4–7) |
| 1 | 3 = up | 0x1E (chunk30) | `func_0020F950` | **MAP SCREEN** — 11 area maps (owned-flags `0x810700+0x5B8[11]`, gate array `D_00810CB8[11]`); cursor `+0x12` skips unowned maps; preselects the CURRENT area from `D_008106CD` (&0xF map, >>4&3 floor `+0x14`); entering computes the player-blip offset from `D_00810350/0x810358` vs the per-map float-triplet tables `D_00265890[map][floor]` (scaled 2·0.10666 → `+0x28/+0x2C`); external preselect via `D_008106B0/B1` |
| 2 | 2 = right | 0x2C (chunk44) | `func_00211970` | **SPR4 SCREEN** — weapon customization hub; sub-modules 0x2D LOWER U.R.S. (`func_00218D90`) / 0x2E UPPER U.R.S. (`func_00217090`) / 0x2F SCOPE MOUNT (`func_00218640`) / 0x30 MULTIPLE ATTACHMENT (`func_002177B0`) / 0x31 SELECTOR SWITCH (`func_00217FA0`), states 4–8; plus the **magazine-refill flow** (entry request `D_008106B0`==5): counts the reserve display `+0x1E` up +3 per tick (sound 0x182 every 10 frames via the main-loop counter `0x70003B64`) to max = `D_00810C63`·30, then writes mag `D_00810C62`=30 and reserve `D_00810CB4`=max — reserve max is battery-capacity·30 |
| 3 | 1 = down | 0x24 (chunk36) | `func_00214020` | **DATABASE SCREEN** — full item catalog; record id `+0x1B` classed by id ranges (<0x20/<0x32/<0x48/<0x5D/else → category `+0x12` 0–4); record text via help group 0x64 → `func_001FCF90(line, D_00282244)` (the "Found:" entries); external open-at-record via `D_008106B0/B1` (the pickup → database flow) |
| 4/5 | — | 0x25/0x26 (chunk37/38) | `func_002072C0` → cb `func_00207350` | **PASSCODE KEYPADS** — NOT diamond-reachable; entered only via external request `D_008106C5` (≠2 → page 4, ==2 → page 5; 0xFF = cleared). 3x4 numeric pad: cursor `+0x28` (right +1, left −1, up −3, down +3, clamp 0xA=ENTER), X appends `D_00265010[cell]`+0x30 ASCII to the 16-byte buffer `+0x60` (max 8, sound 0x8C6; backspace Triangle-bit 0x20, 0x8C7; commit on cell 0xA, 0x8C8), then **strcmp (`func_00123020`) against `D_00275858[page-4]`**; match on page 4 sets unlock flag `D_00810845 |= 0x20`, mismatch → help line 2 + sound 0x8CB. Textures come from data tables `D_00275860[page-4]` (no inline TEX0 tokens — statically unexportable for now) |

### CORRECTIONS to the s25 "STATUS SCREEN LAYOUT" notes

- **X with no hover does NOT enter page 0** — `func_0020CDC0`
  .L0020D294 requires hover 1..4; hover 0 → error buzz `func_0020CD80`.
- **hover → entered page is a REMAP, not identity**: 1(down)→3,
  2(right)→2, 3(up)→1, 4(left)→0 (so the s25 live "entered page 2 from
  right-hover" was the identity case by coincidence).
- The hub close mask is 0x830 = Triangle | Start | **Circle**.
- s25's "(5→0x26)" module entry is real but only `D_008106C5`==2
  reaches it.

### The message bank: chunk00/f02_id02.bin = asset slot 2 (D_0028A498)

Help text resolves via `func_001FCB90(x, y, group=D_00282240,
line=D_002821B8)`: blob top header `{u32 base=0xA0, u32 ngroups=9,
u32 total, u32 dir=0x10}`, 16-byte group dir entries at +0x10 (first
u32 = group blob offset rel. base); each group blob `{u32 strbase,
u32 count}` + sequential NUL-separated strings — **line N = the Nth
string in physical order** (`func_001FE070` walks terminators).
Groups: 0 = hub (lines 0 DATABASE SCREEN / 1 ITEM SCREEN / 2 MAP
SCREEN / 3 "Dennis Infected" / 4–8 infection diary / 9 SPR4 SCREEN —
the hub help shows the page name per hover, or the infection-graded
diary line when idle, keyed on 100−infection thresholds 0x51/0x33/
0x1F at `func_0020CDC0` .L0020D1AC); 1 = ITEM categories (BATTERY/
EQUIPMENT/EVENT/MAIN MENU/HEALING); 2 = SPR4 components (SCOPE MOUNT/
MULTIPLE ATTACHMENT/LOWER U.R.S./UPPER U.R.S./SELECTOR SWITCH/MAIN
MENU); 5 = action prompts ("Reload SPR4 magazines…", battery-consume
confirms, "Yes           No"); 6 = the 11 MAP area names ("Underground
Tunnel", "A, B, and C Areas", "Supply Room", "Command Center"…);
3/4/7/8 = item names/descriptions + the "Found:" database variants
(64/64/41/42 lines).

### Page textures: TEX0 tokens are inlined raw, and the chunks replay

- Every page chunk uploads ONE PSMCT32 block at **dbp 0x1D00, dbw 4**
  (256 px wide; heights 96–480) — `read_uploads_localmem` replays it;
  the page textures are PSMT4/PSMT8 (+16/256-entry CSM1 CT32 CLUTs at
  CBPs inside the same upload), stored v-flipped like the hub set.
- The draw functions inline **raw 64-bit TEX0 values** (lui/ori +
  dsll32/or pairs): TBP=lo&0x3FFF, TBW=(lo>>14)&0x3F, PSM=(lo>>20)&0x3F
  (0x14/0x13), TW/TH log2 (TH spans the word boundary), CBP=(hi>>5)
  &0x3FFF. A register-tracking scan of each page's draw-function
  closure yields 17/18/35/22 tokens for pages 0–3 (page 4: 0 — data-
  driven). **All 92 decode against their chunk replays (100%)**;
  pixel-verified identities: "ITEM"/"MAP"/"SPR4" 128x64 title art
  (titles draw at canvas (8,0) — page-1's inline call GS 0x7080/
  0x7900), per-page OK/BACK/EXIT button legends, the 512x320 graph-
  paper map (2x 256x256 + 2x 256x64 bands), the DATABASE page's
  512x512 background (8x 256x128 tiles, 2x4), the SPR4 page's seven
  256x128 weapon-attachment renders (carbine/shotgun/grenade
  launchers/AT6/flamethrower/missile launcher), the map player-blip
  16x16, map icons, chevrons, "PAGE /" counter. A "DATABASE" art
  title was NOT found statically (likely data-driven via
  `func_00213F30`/`D_002659C0`).
- `tools/export_ui.py --page N|all` (macOS arm64, repo root; reads the
  user's own extract/) writes `assets/ui_pageN.emui` (.emui v1, same
  format as the hub sheet): records carry canvas anchors where known
  (titles asm-anchored; background tiles/legends composition-verified
  ASSUMED), else the x=-32768 sheet-only sentinel. Reported coverage:
  page 0 17/17 (1 anchored), 1 18/18 (6), 2 35/35 (3), 3 22/22 (10),
  4 0/0 (honest: keypad textures unreachable statically).

### Port (extermination-port s39) — page-navigation skeleton

em_hud: stick hover among the pager diamonds (deflection > 0.8,
quadrants → hover 1–4; hovered marker rings render the engine's green
state), X enters via the engine remap, Circle/Triangle exits a page to
the hub, hub closes on Triangle/Start/Circle. The entered page draws
its `ui_pageN.emui` anchored records (single UI texture slot re-
registered on hub↔page transitions — the engine's transient re-stream
shape); missing asset → flagged placeholder panel; every page carries
an amber "CONTENT TBD" strip (interiors not modeled). Capture hooks
`EM_HUD_PAGE=<0..3>` / `EM_HUD_HOVER=<1..4>` (with EM_HUD_FORCE=1).
Verified: `make test-input` + door/sfx self-tests PASS; default AND
EM_HUD_FORCE=1 hub captures **byte-identical** to the pre-nav build
(nav only changes on input); page captures show MAP's graph-paper map
+ legend, SPR4's title/carbine/legend, DATABASE's tiled background +
PAGE counter + legend, ITEM's title.

### Open items (s39)

- Page interior layouts (lists, cursors, the SPR4 component grid, the
  database record view) — each still needs its own draw-chain pass;
  the per-page draw helpers are mapped (see table) so the work is
  scoped.
- Sub-module texture sets (ITEM categories 0x20–0x23, SPR4 components
  0x2D–0x31) decode with the same recipe (verified spot checks: cyan
  selection banner, shared background art) — export once their views
  are modeled.
- Page background/legend anchors marked ASSUMED need one live capture
  (or the draw-call coordinate walk) to pin exactly; the DATABASE
  art title and the keypad textures need the data-table path
  (`D_002659C0`, `D_00275860`) decoded.
- The two keypad codes (`D_00275858[0/1]`) and the unlock flags'
  consumers — read the strings from the local ELF and tie to the
  METAL TAG / Maximum Security flow.

## MESSAGE BANK EXPORTED — full decode of chunk00/f02_id02.bin, group inventory, hub-help selection rule, port help/ITEM text (2026-06-10, session 42)

Static pass closing two s39 threads: the message bank is now fully
decoded and EXPORTED (`tools/export_ui.py --messages` →
`assets/messages.emsg`), and the port's em_hud consumes it — the hub
help panel shows the engine's real help line and the ITEM page renders
a basic real interior. No emulator needed.

### Bank on-disc layout (refines the s39 sketch; engine-verified)

`extract/chunk00/f02_id02.bin` = boot-chunk asset slot 2 (runtime
pointer `D_0028A498`), resolved by `func_001FCB90(x, y, group, line)`
→ `func_001FE070`. Exact layout (LE; offsets relative to the named
header):

- bank header `{u32 dir_base=0xA0, u32 ngroups=9, u32 total=0x6200,
  u32 dir_off=0x10}`; group dir at `dir_off`: 16-byte entries
  `{u32 group_off (rel dir_base), u32 group_off>>4, u32 size,
  u32 padded_size}`.
- group OUTER blob at `bank + dir_base + group_off`:
  `{u32 text_off, u32 line_count, u32 records_size, u32 0x10}` +
  line_count x 16-byte MARKUP entries `{u32 records_off (rel OUTER),
  ?, ?, u32 nrecords<<4}` (`func_001FE4B0`/`func_001FE4D0`; the
  16-byte control records — inline style/color/glyph-size runs, types
  2/3/4 in `func_001FE070` — live between this table and the text;
  groups 3/4 use them on 119 lines, all other groups are zero).
- group TEXT blob at `OUTER + text_off + records_size`
  (`func_001FE460`'s walk): `{u32 strbase (= 0x10 + 16*count),
  u32 line_count, u32 str_bytes, u32 1}` + per-line 16-byte entries
  `{u32 off, u32 off, u32 len, u32 len+1}` (`func_001FE480`: string N
  = TEXT + strbase + off), then the NUL-terminated ASCII strings.
  '\n' = in-entry line break; the line table agrees exactly with the
  s39 "Nth NUL-string" walk (exporter cross-checks both).

### Group inventory (content domains, full dump read)

| group | lines | domain |
|---|---|---|
| 0 | 10 | HUB HELP — page names (0 DATABASE / 1 ITEM / 2 MAP / 9 SPR4 SCREEN), 3 "Dennis Infected", 4–8 infection diary (graded) |
| 1 | 5 | ITEM screen category banners: BATTERY / EQUIPMENT / EVENT ITEMS, MAIN MENU, HEALING ITEMS (label + '\n' + description per line) |
| 2 | 6 | SPR4 screen component slots: SCOPE MOUNT, MULTIPLE ATTACHMENT, LOWER/UPPER U.R.S., SELECTOR SWITCH, MAIN MENU |
| 3 | 64 | ITEM CATALOG names+descriptions, indexed by record/type id: 0–15 weapon attachments, 16–22+63 `\` placeholders (the no-database ammo types), 23–29 gear/batteries (BATTERY PACK 6/18/24 GAUGE = lines 27/28/29), 30–34 healing, 35–44 event items (CARD KEYs, METAL TAGs, DVD-ROM, GARY'S BOMB…), 45–59 the 15 RECON DOGTAGs (named Marines), 60 MISSILE LAUNCHER, 61–62 fire modes. Carries inline markup records (color runs — e.g. card-reader glyphs) |
| 4 | 64 | same index space, the "Found:" PICKUP-POPUP / database variants (s39's help group 0x64 → `func_001FCF90`); also markup |
| 5 | 28 | ACTION PROMPTS / confirm dialogs — "Yes  No", magazine reload, battery-consume (2/4/6/16/24 units), MTS treatment, save-cost prompts |
| 6 | 11 | the 11 MAP area names, one line per map, '\n'-separated per-floor sub-names ("Underground Tunnel"… "Power Reactor Area") |
| 7 | 41 | MEMORY CARD / save-load system messages (slots, format, 88KB, corrupt-data flows) |
| 8 | 42 | OPTIONS screen labels (VIBRATION/SOUND/BRIGHTNESS/BUTTON CONFIG rows, action names, TYPE A/B/C) |

### Hub help-line selection — exact rule (func_0020CDC0, completes s39)

- Hovered diamond → the page-name line: hover 1 (down) → line 0
  DATABASE, 2 (right) → 9 SPR4, 3 (up) → 2 MAP, 4 (left) → 1 ITEM
  (`D_002821B4=1`, `D_002821B8=line`).
- Idle (hover 0) → `.L0020D1AC`: `v = 100 - (int)D_0081085C`
  (displayed infection). v == 100 → NO help line (`D_002821B4=0`);
  v ≥ 0x51 → line 4; ≥ 0x33 → 5; ≥ 0x1F → 6; ≥ 0xB → 7 (threshold
  not in the s39 note); > 0 → 8; else (infection 100) → 3 "Dennis
  Infected". I.e. infection 1–19 → 4, 20–49 → 5, 50–69 → 6, 70–89 →
  7, 90–99 → 8.
- Draw path `func_001FCA10` state 4: group ≠ 0x64 →
  `func_001FCB90(0x8A, 0xA8, group, line)` — canvas x 138, FIELD y
  168 = canvas y 336 (the help panel's top edge; panel is
  (128,336)-(384,432)). Group 0x64 (database records) instead draws
  at (0xA8, 0xBE) via `func_001FCF90`/`func_001FCF60`. '\n' line
  advance in `func_001FE070` = `(D_00264CD8 + D_00264CE0) >> 1` =
  (20+4)/2 = 12 field lines = **24 canvas px** (tall font).
  (Confirms the text functions' y is field space — e.g. the hub's
  "INFECTED" call passes GS 0x822/0x812 = canvas (290,260).)

### Export + port (extermination-port s42)

- `tools/export_ui.py --messages` (macOS arm64, repo root; reads the
  user's own `extract/chunk00/`) → `assets/messages.emsg` (.emsg v1,
  documented in the script docstring + the em_hud loader): flat
  group/line directory + offset table + NUL-terminated string blob;
  271 lines / 9 groups / ~12 KB text. Markup records are NOT exported
  (plain text only; the exporter reports the 119 affected lines).
- em_hud: loads `assets/messages.emsg` (missing/invalid = skip, no
  regression); the hub help panel draws the real engine help line per
  the rule above (tall font, (138,336), 24 px steps); the ITEM page
  renders a basic real interior — the group-1 category labels (row
  layout ASSUMED) plus the only item counts the port models, flagged:
  the carried battery pack (group-3 catalog name picked by capacity
  6/18/24 + charge cur/max) and "SPR4 MAGAZINE xN" (PORT label,
  N = reserve/30 derived — the engine's per-type count array
  `D_00810C64` / pack counter `D_00810C63` are untranslated). The
  page flag strip reads "PARTIAL: AMMO/BATTERY ONLY" there;
  "CONTENT TBD" elsewhere.
- Verified: `make test-input` + door/sfx self-tests PASS; default
  capture AND the EM_HUD_FORCE=1 hub capture with the bank absent are
  **byte-identical** to the pre-bank build; with the bank, the idle
  hub shows the infection-60 diary line 6 ("I feel dizzy…", 4 rows
  filling the help panel), EM_HUD_HOVER=4 shows "ITEM SCREEN", and
  EM_HUD_PAGE=0 shows the five category labels + the two count rows
  under the ITEM title art.

### Open items (s42)

- Inline markup records (groups 3/4): decode the type-2/3/4 payloads
  (style index / glyph size) and carry them in .emsg if the database/
  pickup views ever need styled runs.
- The other bank consumers (groups 5–8 prompts, map names, memory
  card, options) wire up as their screens/flows get modeled; group 6
  is ready for the MAP page interior pass.
- The engine's per-type inventory array (`D_00810C64`) is the next
  step for a faithful ITEM interior (real per-item rows + counts,
  category membership by type id).

## STATUS SCREEN BACKGROUND — the universal animated UI background decoded (2026-06-11, session 44)

Closes the s25 open question "what fills the screen behind the hub"
(dim? backdrop texture? the rotating-player room?): it is NONE of the
s25 guesses — every status/UI screen draws an **animated full-screen
background** through one universal drawer, over the black UI-camera
frame, BEFORE its panels. Static decode (asm walk + .data init + GS
dump pixel check); port ships it.

### The drawer: func_0020A7A0(tex0_token)

Called at the head of every UI view's draw chain with a **per-screen
128x64 PSMT4 tile** (raw 64-bit TEX0 token, same inline lui/ori//
dsll32/or idiom as the page tokens). Confirmed callers (21): the hub
(`func_0020CDC0` state 1/2, before `func_00209DF0`), the four pager
pages (`func_0020F170` ITEM frame, `func_00210A00` MAP, `func_002121A0`
SPR4, `func_00214020` DATABASE), the ITEM/SPR4 sub-views
(`func_00214570/2149F0/215870/2160B0`, `func_00217090/77B0/8640/8D90/
FA0`), and the non-status screens `func_00200A40/201000/201F70/202BA0/
202D10/22A650/225AC0` (title/options/memory-card family — same
mechanism, tokens not yet inventoried).

It composites THREE layers of the tile; per-layer state = three
0x20-byte blocks at `D_002655A0` (+0x00/+0x04 offset floats, +0x08
phase degrees, +0x0C pulse timer, +0x10..0x1C RGBA floats — colors
rewritten every frame to (96,96,96,64); alpha then *= sin(phase·π/180)
via func_0011E2A8). `.data` init: {0,0,90,0} / {0,0,90,0} / {0,...} —
**no per-open reset; state persists across opens**. Blend mode 0
(`func_00207D00(1,0)`), draws via `func_00207E40` (x,y 12.4 GS;
w/h canvas px).

- **Layer 0 — horizontal scroll tiling**: grid of 256x128-canvas-px
  quads (the 128x64 tile at 2x) at GS x 0x400..0xC00 step 0x100, field
  y 0x400..0xC00 step 0x40 (mostly offscreen; the visible residues are
  canvas x ≡ int(p0) mod 256, y ≡ 96 mod 128). p0 -= 0.5/frame, wrap
  <0 → 255 (drift LEFT 0.5 px/frame). Phase pinned at 90° → constant
  alpha 64 (= 50%); modulate (96,96,96) = 0.75.
- **Layer 1 — vertical scroll tiling, pulsed**: same grid; p1 -= 0.5
  FIELD lines/frame (1 canvas px), wrap 255; rows at y ≡ 96 + 2·int(p1)
  mod 128. Pulse: while timer ≥ 0 it decrements (phase was reset to 0
  → alpha 0, invisible); then phase += 0.25°/frame, alpha = 64·sin
  (12 s in / 12 s out breath); at phase ≥ 180 → timer = 0x3C +
  3·(rand()%0x3C) (60..237 frames, func_00122BB8), phase/p0/p1 reset.
- **Layer 2 — full-screen zoom burst**: same pulse timer; only while
  active (timer < 0): p0 += 0.3, p1 += 0.3 per frame and ONE quad at
  canvas (−p0, −p1) size (512+2·p0, 448+2·p1) — full screen expanding
  0.3 px/frame past every edge (0x200/0x1C0 = the canvas dims, x via
  16·(1792−p0), y via 16·(1936−p1/2)) — alpha 64·sin(phase).

Behind the layers there is NO additional draw — the frame is the
identity-UI-camera 3D pass (black + the rotating player model), so the
real background = black + this triple-layer tile shimmer.

### The tiles

- **Hub**: token `0x9D421E40_20045EE5` → TBP **0x1E40**, CBP **0x22F7**,
  PSMT4 128x64 TBW 8 — boot-resident, adjacent to the s26 decor set
  (title 0x1E50 / arrows CLUT 0x22F6); decodes from an ordinary
  gameplay-state gs.bin. Pixels: a **dark blue-gray circuit-board /
  schematic pattern** (fully opaque, peak brightness ~57/255) — tiled,
  scrolled and pulsed at 50% over black it reads as the menu's subtle
  animated "tech blueprint" backdrop.
- **Pages** (s39 SUPERSESSION — four "identity unknown" records were
  these): ITEM `0x9D421DB0_2003C8A5` (s39 "title_hi"), MAP
  `0x9D422050_20043C25` ("pan_b"), SPR4 `0x9D422300_20047945`
  ("title_b"), DATABASE `0x9D422200_200450A5` ("t2200" — its
  "background filler, identity unresolved" note is settled: it is the
  page's func_0020A7A0 tile; a "DATABASE" art title still does not
  exist statically).

### Export + port (extermination-port s44)

- `tools/export_ui.py`: hub mode adds the **backdrop** sprite (TBP
  0x1E40/CBP 0x22F7, sheet 272x144 → 272x212); page mode reflags the
  four tiles as `bg_anim`. New `.emui` sentinel: records with
  **x = y = -32767 are BACKDROP records** (vs -32768 plain sheet-only);
  dw/dh carry the engine's 2x tile size (256x128). Old assets simply
  lack the record (port falls back to its scene dim).
- Port (`em_hud.c background_render` + an em_gfx overlay BACKDROP
  queue that flushes FIRST, under the untextured panels): black base
  fill + the three layers, engine rates/phases/init values, rand →
  fixed-seed LCG (deterministic captures). Hub AND entered pages use
  their own sheet's record. Also exported `em_hud_is_open()` — the
  pause-gate query for em_game (the engine's open flag `0x8106C4`
  semantics; excludes the EM_HUD_FORCE render-only hook).
- Verified: default capture and the ui.emui-absent forced capture
  byte-identical to pre-change HEAD; forced hub/page captures show the
  animated background under every panel; repeat captures
  byte-identical; test-input + door/sfx/melee/transit PASS.

### Open

- The 7 non-status callers' tile tokens (title/options/memory-card
  screens) — same one-line decode each when those screens get modeled.
- Layer-1/2 pulse cadence is engine-exact but the engine's rand is
  time-seeded; the port's fixed seed is a flagged determinism choice.
- The rotating player model on the black UI scene stays the port's
  documented 3D-in-UI TODO.

_Last updated: 2026-06-11 (session 44)._

## MUZZLE GEOMETRY DECODED + FLASH FX PINNED; PLAYER-AURA VERDICT (2026-06-11, session 43)

Static decode pass closing the port's three weapon-fidelity bugs (laser
anchored at the stomach + aimed ~90 deg off, placeholder overlay
flash/crosshair squares, the permanent green "aura" square). Sources:
func_00188630 / func_00187CC0 / func_001F4F40 / func_001F5040 splat asm
+ the offset tables read from the local boot ELF.

### 1. The muzzle offset tables (boot ELF .data, read locally)

`func_00188630` computes, in the HAND-BONE frame M (player +0x90,
copied to the equipment blob `D_00810550` each gun tick):

```
ray origin  gun+0xA0 = M * (-3, tbl.y, 0, 1)
barrel tip  gun+0xB0 = M * D_0024A220[idx]
fire dir    gun+0xC0 = normalize(tip - origin)
laser start gun+0x1F0 = M_player * D_0024A2A0[sub]   (sub UNREMAPPED)
shell eject point      = M_player * D_0024A300[sub]  (func_001F4010(3,..))
```

idx = sub-weapon byte D_00810525, except sub 0 remaps on the aim
option D_00810CA4: option 0 -> row 7, option 2 -> row 6, else row 0.
Decoded rows (16-byte vec4s):

```
D_0024A220 (second point / barrel tip), rows 0..7:
  0 (5.4, 1, 0)        1 (4.886, .304, 0)  2 (6.6, .304, 0)
  3 (6.9, .524, 0)     4 (6.8, .4, 0)      5 (4.924, 1.348, 0)
  6 (4.74, .908, 0)    7 (6.0, 1.088, 0)   <- SPR4 manual aim
D_0024A2A0 (laser beam-draw start), rows 0..5:
  0 (3.6, .5, 0)  1 (2.864, 0, 0)  2 (5.056, -.064, 0)
  3 (6.36, -.048, 0)  4 (4.604, .024, 0)  5 (4.865, .627, .087)
D_0024A300 (shell eject), row 0 = (1.3, 0.8, 0)
```

Key consequence: both ray points share tbl.y and z = 0, so **the fire
direction is exactly the hand bone's local +X axis** — the rifle's
barrel axis. The camera-aim relationship lives in the ANIMATION (the
aim-pose ladder orients that axis along the aim yaw; the +0x278 pitch
step adds camera pitch). The ray origin (x = -3) sits inside the
receiver; the drawn laser starts at (3.6, .5, 0) on the barrel; the
muzzle FLASH anchors at the (6, 1.088, 0) tip (func_00187CC0 copies
gun+0xB0 into the FX actor).

### 2. Muzzle flash FX — func_00187CC0 -> func_001F4F40 -> func_001F5040

`func_00187CC0(gun)`: picks FX VARIANT a = (D_00810525==3 ?
(ctx ? 4 : 1) : (ctx ? 3 : 0)) where ctx = func_0015D2F0() in {2,0x82}
(the spread gun gets its own pair); spawns a class-0xC pool actor via
`func_001F4F40(a)` (behavior `func_001F5040`), copies the tip point
+0xB0 and the hand matrix (player+0x90 -> FX+0xD0), and stores a second
point FX+0x100 = tip + handRot*(0.2, 0.2, 0) with w=1.

`func_001F5040` (the FX behavior), variant 0 (SPR4):

- INIT: bind chunk27 library (D_0028A56C) model **0x0D**; scale vec
  +0x60 = 0.15 + 0.05*rand01 (uniform), scale VELOCITY +0x1F0.40 =
  0.15/component; rotation triple +0x80 = 0 (variants 3/4 start at
  -96.0). Aborts to state 3 if the pool headroom D_00275BCC is short.
- RUN tick t (halfword +0x28): t 0..2 -> rebind model **0x08** + a
  `func_001F4F90(self, 2.4)` line-burst pass (D_0026EA80/EAC0 point
  pairs through func_001CD940 — the radiating spark lines); t = 3 ->
  rebind model **0x07**; t >= 4 -> rotation lerps to -128 by
  0.35/tick; EVERY tick scale += vel, vel *= 0.8; **freed at t = 15**.
- Library geometry (export_props survey, model-local): 0x0D = radial
  puff X[0,.88] Y[-1.6,1.78] Z[+-1.81]; 0x08 and 0x07 = the forward
  star, X[0,4.9] (the +X barrel axis!) radial +-2.3 / +-2.2; all three
  sample ONE additive effect sheet (TEX0 key 0x457b5594220a0). Models
  0x0B/0x0E/0x0F are the other variants' shells (same sheet).

So the flash = a 16-tick additive billboard burst at the tip: bright
star ticks 0..3 (model 8 then 7), growing 0.15->~0.9 scale with a
0.8-decay velocity. PORT: drawn through the world-space beam/dot pass
(core glow + forward streak at the real tip, intensity decaying with
the engine's own 0.8^t; untextured stand-in for the sheet — flagged).

### 3. Player-aura verdict (the port's "green square")

Re-read of the s7b/s17 evidence for the port fidelity call: models
20/21 ARE drawn by the original engine at the player root (live arena
scan of save state 01 + all 4 office GS dump frames — two scenes), and
they are NOT a drop shadow: the blend is pure additive (ALPHA A=0 B=2
C=2 D=1 FIX=0x80, Cv = Cs + Cd — physically cannot darken), green-
pulsing, feet-anchored. In the real frame they contribute only a
~3..10% green shimmer (the cube faces UV the texture's FAINT INTERIOR,
lum 5..15/255, x G<=215/128). The port's single camera-facing quad
with the tint premultiplied into dedicated texels over-reads as a
solid permanent green sheet — so the bake is now GATED: export_props/
export_native `--no-glow` (player.emdl re-exported with it; the old
recorded CLI still reproduces the old bytes — verified byte-identical
before the change, superset discipline). No blob shadow is implemented:
the evidence says the engine draws no subtractive shadow primitive
under the player in these frames (none found in the s7b/s17 dumps);
re-adding the aura faithfully needs the pulse + additive-cube path
(open).

### 4. Port wiring (extermination-port, this session)

- `em_gfx_last_skinned_bone` (em_gfx.h + metal): every skinned draw
  copies its first 16 bone matrices; the chain draws the player LAST,
  so this publishes the player palette — the native analog of the
  engine's player+0x90 bone publish (one frame of latency by
  construction, like the fire-event mailbox).
- `em_weapon.c`: muzzle ray/laser/flash all hand-frame anchored per
  section 1 (node 4 = the rifle attach node; chest-height/yaw remains
  the flagged fallback for an EMDL without weapon clips). The overlay
  crosshair + hit-pulse + screen-space flash rects are REMOVED — the
  real game aims with the laser dot alone (s23 capture). The dot and
  flash draw as 3-layer concentric additive glows (untextured falloff
  stand-in, flagged).
- Verified: aim capture shows the laser leaving the rifle muzzle along
  the barrel to a soft wall dot, fire capture the muzzle flash at the
  tip; idle capture has no green square; make test-input + door/
  weapon/melee/transit/enemy 1-4/sfx self-tests all PASS. Default
  captures CHANGE by exactly the aura removal (expected, stated).

### Open items (s43)

- The flash line-burst pass (func_001F4F90's D_0026EA80 point pairs)
  and the effect-sheet texels: extract for a textured flash.
- The aim-ladder pitch step (+0x278) — vertical aim is still
  untranslated port-side; the laser inherits the level-pitch pose.
- Variants 1/3/4 of func_001F5040 (spread gun + the ctx pair) when
  those weapons land in the port.

## DOOR USE SCAN + STAGING MATH DECODED; ANIM-ID DIRECTORY FIX; DRAWBRIDGE ROOM EXPORTED (2026-06-11, session 45)

Closes three user-reported door-fidelity bugs from static decode (no
emulator): the hinge-vs-handle trigger/staging offset, the "open plays
the locked animation" anim-id bug, and the office main door's wrong
destination (the s38 synthetic link). Sources: func_00183EF0 full
class-5 read, the func_001BBE40 91.5% C stub re-verified against both
s22 live captures, the player clip library's leading directory, the
boot-ELF AREA01 tables, and OVERLAY/AREA01.BIN.

### 1. func_00183EF0 CLASS-5 (door) use-scan branch — full read

The s17 contract ("dist^2 <= 144, LOS, facing-dot ~0.4, 2-u auto ring")
was the CLASS-7 prefix only. The class-5 door branch (entered for
candidate class 5, model checked first):

```
models 3 / 0x15 (hinged m03 family; placement origin = the HINGE corner):
    center = door_pos + 5.0 * (-cos(yaw), 0, +sin(yaw))
other models (m17/m09 sliders; origin already the doorway center):
    center = door_pos
1. sqrt((px-cx)^2 + (pz-cz)^2)  <= desc[0]      ; horizontal, from CENTER
2. sqrt((py - door_y)^2)        <= desc[1]      ; vertical
3. side: |norm(atan2(player - door_pos) - yaw)| <= pi/2  -> FRONT
4. facing: |norm(player_yaw + (front ? pi : 0) - yaw)|   <= pi/4 -> hit
```

`desc` = actor +0x30 = `D_002755F0`, STATIC .data: **{10.0f, 8.0f,
0x102, ...}** — the door scan radius is **10.0 u from the doorway
center** (5 u from the hinge toward the free/handle edge), vertical
window 8.0. NO LOS query and NO 2-u auto ring exist in the class-5
path (both were class-7) — the port's flagged "LOS doorway pocket"
exemption matched nothing real and is retired. The class-5
other-model path (.L00184230) is identical with center = door_pos.
This is the user-reported "trigger is at the hinge, not the handle"
bug: the port measured from the placement origin (the hinge corner).

### 2. func_001BBE40 staging algebra — trig labels corrected, capture-exact

The C stub's extern annotations had sin/cos SWAPPED: **func_0011DE90 =
cosf, func_0011E2A8 = sinf** (pinned two ways: both s22 staging
captures, and func_00136630's forward step `x += v*E2A8(yaw); z +=
v*DE90(yaw)` with the engine's forward = (sin yaw, cos yaw)). The
decoded staging point:

```
pyaw = norm(door_yaw + (front ? pi : 0))        ; the player yaw snap
sx   = door_x - 5*cos(door_yaw) - 5*sin(pyaw)
sy   = player_y
sz   = door_z + 5*sin(door_yaw) - 5*cos(pyaw)
     = doorway CENTER - 5 * forward(pyaw)       ; 5 u on the player's side
```

Both s22 captures reproduce exactly: office door (109, -252.2) yaw 0
front -> (104, -247.2); west door (57, -220.5) yaw -pi/2 back ->
(62, -225.5). The spawn-table records flank the CENTER (office recs
2/3 = (104, -245)/(104, -259) vs center (104, -252.2)) — the engine
stages and re-places on the doorway centerline, toward the handle
side of the hinge. (src/func_001BBE40.c's comment block has been
updated to the corrected labels, along with func_001545B0.c's and the
s17/s20 staging prose above — annotation-only, the .o is byte-identical.)

### 3. Player anim ids are DIRECTORY indices — the exporter scan was shifted

`chunk28/f01_id3c.bin` has a **459-entry leading directory** (the
anim_clip_resolve/func_001C6120 path, s23) but `find_id74_headers`
enumerates only **455** containers: directory ids **54/94/115/375**
carry a non-sentinel +0x4 halfword (0x0035/0x005F/0x0072/0x0178
instead of 0xFFFE/0xFFFF — an undecoded header variant, blob id still
0x74) and are skipped, shifting every scan index >= 54 by up to +3.
Consequences in the shipped port assets (all fixed by resolving
through the directory — export_native.anim_directory, now the default
for files that have one):

- "--clips 69,67" (door open front/back 0x45/0x43) actually baked
  directory ids **70/68 = the LOCKED tries 0x46/0x44** — the exact
  user report "the player always does the locked animation". s23's
  script/id decode was right all along; its closing claim "the
  exporter's container indices already match anim ids 1:1" was wrong.
- "idle 346" actually baked directory id 349. The real dir-349 is the
  180-frame breathing idle the port has always shown; dir-346 is a
  30-frame gesture. em_game's CLIP_ID_IDLE is now 349 (still a
  flagged visual guess; the engine's default-idle id is open).
- the s23/s25 weapon/pager ids (0x110/0x111/0x112, 0x10B-0x10F) were
  authoritative engine ids — they now bake the RIGHT content (the old
  asset's versions were +3-shifted neighbors, subtly wrong).

**Door-clip motion verdict (directory-resolved bakes, 21-node rig):**

| dir id | engine id | frames | motion |
|--------|-----------|--------|--------|
| 67 | 0x43 open back  | 150 | still ~f48, step/turn to the door (~15 u by f96), then 18-29 u whole-body travel f120-150 — reach, push, WALK THROUGH |
| 69 | 0x45 open front | 150 | arm out ~5 u f12-84 (hold the handle), pull back f96, walk-through f108-150 (18.6 u) |
| 68 | 0x44 locked back  | 200 | limbs only, peaks 6.6 u, rattles f32-176, RETURNS TO REST (f192 ~0.6) — try and fail |
| 70 | 0x46 locked front | 200 | same shape, peaks 5.4 u, returns to rest |

The 150-frame opens cover the script wait (90/70) + the 64-frame fade
almost exactly (the walk-through tail plays during the fade-out).

### 4. AREA01 = the DRAWBRIDGE ROOM area; office main door fixed

The soundmap's area_scene_map pins **(1, 0) -> chunk05.n0** and
**(1, 7) -> chunk15 = the intro SNOW level**: the snow level is an
AREA01 sub-state, so AREA01 sub 0 is "the first room after the first
snow level" — the user-remembered drawbridge room. The office main
(west) door's REAL dest record (s38: door 1|0x80 -> {01 05 00 00} =
AREA01 sub 0 entry 5) therefore points at the drawbridge room; the
s38 --synthetic-link (west <-> m15) is REMOVED.

Offline decode of the area (all new):

- **Placement tables** (OVERLAY/AREA01.BIN — missed by placements.py
  --scan; found by hunting the door behavior fn pointers): TWO tables,
  0x82BD50 (54 recs) and 0x82C5F0 (32 recs), serving the area's 7
  sub-states. Table A carries an OVERLAY-BRAIN scripted door (record
  [12], fn 0x823580, door id 0, at the -1276 shaft bottom — likely
  the drawbridge cutscene) and link-0 INERT generators; table B has
  the same door as a plain fn 0x001BC350 record and ACTIVE generators
  -> table A = the first-visit story beat = SUB 0 (FLAGGED
  presumption). Doors in A: id 1|0x80 m15 (-20.5, 0, -192) link
  0x0200; id 2 m03 (60.5, 0.5, -559) room move; id 3|0x80 m03
  (50, 0, -220.5) yaw -pi/2 link 0x0200; id 4 m09 (128.6, 0, -610)
  room move; id 5|0x80 m09 (120, 60, -318.8); id 6|0x80 m03
  (-109.5, 60, -674.5).
- **Dest table** D_0024E140[1] = 0x24DFA0 (8 recs): door 1 ->
  {02 00 00 00} = AREA02 sub 0 entry 0 (-> office0, the m15 pair);
  door 3 -> {02 01 01 01} = AREA02 sub 1 entry 1 (-> the office
  scene, the west-door pair — the s22 return path); door 2 room move
  {01 02}; door 4 room move {09 08}; doors 5/6 -> areas 0x16/0x06.
- **Spawn table**: AREA01's desc (D_0024D650[1] = 0x275500) is BSS,
  but the SUB-0 table is STATIC at **0x24B1A0** (s22 live-verified
  entry 5). 10 entries; door 4's room-move entries 9/8 = (143,
  -609.7)/(115.5, -609.7) flank the door at (128.6, -610) — internal
  cross-check.
- **Leaf survey** (chunk05.n0): f02_id43 main world + 9 more
  world-zone files; f11_id4b is world only up to +0x56000 — the
  per-area MODEL TABLE sits there (concat 0x3F2000, 22 entries; door
  meshes param 5 = m03 2-node hinged, rest node 1 (-7.718, 9, -0.25);
  param 0xF = m15; param 0xB = m09 3-node [-1,0,0] origin-rest
  slider). f00_id44 = collision head + render tail [0x1523D0, end).
  f06_id4d has 29 records but only 1 kick — no assemblable triangles,
  skipped. Collision decodes over the LEAF CONCAT (grid section at
  concat 0x438800, 854 polys; floor probe at the arrival spawn = y 0).
  Textures: the s28 --uploads replay (leaf f00_id44 + chunk27 pack)
  resolves **100% of all zone/door textures** (0 fallbacks).
- **Export**: `export_level.py --drawbridge extract/chunk05.n0` ->
  assets/scene_drawbridge (11 zones + manifest spawn (39, 0, -225)
  yaw -pi/2 + doorsfx selector-2 pair + enemies block);
  `export_props.py --doors-drawbridge` (m03/m15 with the verified
  slot-0x39 bank clips [0,2,1,3]; m09 with the synthesized
  func_001BB400 slide); `export_collision.py chunk05.n0/f*.bin`.

### 5. The VENT (office0 access) — honest verdict

The engine reaches AREA02 sub 0 from the office room through a vent
crawl that is **overlay-SCRIPTED, not a placement object**: the sub-1
table's 14 records are fully identified (2 doors, 7 pickups, 5
fixtures — no vent, no class-0x0B), while the sub-0 table's record
[0] is a class-0x0B trigger at (43, 3.5, -147) right beside spawn
entry 5 (40, 0, -146) — the vent's EXIT end. The in-room mechanism
needs the AREA02 overlay code read (open). The port ships a FLAGGED
SYNTHETIC stand-in: a door line (`--door-goto --vent`) at the suite's
north-corridor wall (62.5, 0, -168.5) whose goto lands on office0's
real spawn entry 5; the mesh is a copy of door_m03 (no vent mesh is
decoded).

### 6. Port wiring + verification

- em_door.c: trigger = the decoded class-5 test (center point, radius
  desc[0]=10 via the manifest, |dy| <= 8, side + pi/4 facing; CROSS
  stays as the flagged action-state-0x2D stand-in *(s58: un-flagged —
  CROSS IS the engine trigger)*; LOS hack + auto
  ring deleted); staging/spawn = the exact func_001BBE40 algebra
  (hinged flag parsed from the exporter's door_mXX filename, flagged).
- Real goto graph: scene west door -> drawbridge e5; office0 m15 ->
  drawbridge e3; drawbridge door 1 -> office0 e0; drawbridge door 3 ->
  scene e1; vent (flagged) -> office0 e5. All five REAL records except
  the vent.
- EM_DOOR_TEST PASS (staging/warp now on the doorway centerline z
  -225.5 — the s22 captured values); EM_TRANSIT_TEST PASS (west door
  -> scene_drawbridge, arrival (39, 0, -225) yaw -pi/2, 6 doors, 854
  collision polys, 14 enemies); EM_MOVE/ENEMY/MELEE/WEAPON/test-input
  PASS; verify_all all-green. Default capture byte-identical (vent out
  of view; idle palette byte-identical under the directory fix) and
  deterministic. Captures: mid-open shows the player centered ON the
  doorway reaching at the panel; frame 145 shows the door swung open
  and the player stepping THROUGH — the open clip, not the locked try.

### Open items (s45)

- The 4 non-sentinel containers (player-library ids 54/94/115/375):
  header variant undecoded (the +0x4 halfword looks like a reference/
  alias id); bake refuses them explicitly.
- ~~The engine's true default-idle anim id~~ — CLOSED s46: base idle =
  id 0, fidget = 0x15D = 349 on a 300-frame timer ("PLAYER IDLE
  CYCLE" below).
- AREA01 sub-state -> table mapping (A vs B) is a flagged presumption;
  one live capture in area 1 would pin it. Same for the AREA01 subs'
  spawn tables other than sub 0.
- The office vent's overlay-scripted mechanism (AREA02 overlay code).
- The drawbridge crawlers are param 0x0004 — a different disguise
  model than the office cardboard box; the port's crate mesh is a
  flagged visual stand-in (carve the param-4 entry like s34's crate).

## PLAYER IDLE CYCLE — base id 0 + fidget 0x15D=349 on a 300-frame timer (2026-06-11, session 46)

Closes the s45 open item "the engine's true default-idle anim id".
Static decode of the player mode-0 top **func_00161020** (jtbl_0026D3B0
slot 0 of the +0x05 player-mode dispatch in func_0015B130).

### The state machine (+0x06 state byte)

- **State 0 (entry)**: speed +0x38 = 0, locIdx +0x25C = 0, request the
  BASE IDLE via `func_00174A50(actor, f12=12.0)` — which resolves
  `func_0017B490(actor, mode=0, family=+0x235, locIdx=0)` =
  `D_00248AB0[0][family]` (mode-0 row `{0, 0xA, 0x4B, 0x55, 0x14}`,
  stride 1): unarmed family 0 → **anim id 0 = container 0, the 80-frame
  breathing idle**. Timer +0x28 = **0x12C (300 frames = 5 s)**.
- **State 1, sub-byte +0x07 = 0**: when +0x236 == 0 and !(+0x235 & 1),
  the timer decrements; at 0 → `func_001749A0(actor, 0x15D, force=1)`
  with f12 = 8.0 — the IDLE FIDGET **anim id 0x15D = 349**, the
  180-frame look-around (directory id; the exporter's pre-fix scan
  called this container "346"). Sub-byte → 1.
- **Sub-byte 1**: waits for the clip-end flag (+0x200 & 0x1000), then
  sub-byte = 0, timer = 0x12C, re-request the base idle
  (`func_00174A50(8.0)`) — the clip re-inits at frame 0.
- Stick input (func_00174AC0 ≠ 0) leaves to the locomotion mode; state
  2/0x63/0x64 handle the walk-out/settle transitions (0x63/0x64 =
  re-idle settle: request base idle, wait clip-flag 0x8000, state 1).

So the standing player loops: breathing (id 0) for 300 frames →
look-around (349) once → breathing → … The f12 argument of the request
chain (12.0 entry / 8.0 cycle) rides anim_clip_init's +0x3C slot — a
transition/blend parameter, not the playback rate (the property table
D_00248C90 rates these ids 1.0); the port cross-fades 8 frames.

Port (extermination-port): em_game.c idle machine (CLIP_ID_IDLE 0,
CLIP_ID_FIDGET 349, IDLE_FIDGET_FRAMES 300); player.emdl re-exported
with directory ids `349,2,3,69,67,75,272,273,51,274,1,267..271,0`
(byte-superset of the s45 16-clip export — clip 0[1092..+80] appended;
old asset without id 0 degrades to the single-349 idle, cycle off).
Verified: fidget starts exactly at idle frame 300 (frame-290 vs -340
captures differ by 5.76% of pixels = the look-around pose; breathing
restarts after ~480); pose-only default-frame delta old-vs-new idle =
5.76% (same binary/scene).

## PLAYER WALL COLLISION RADIUS — 4.5 units, five radial probes (2026-06-11, session 46)

The player's wall "hitbox" is not the move segment: the walk integrator
**func_001764E0** (called from the idle top func_00161020 AND the walk
top func_001612D0) fires **five radial probes** every frame, at angles
`yaw + D_00248950` = **{0, +45, −45, +90, −90} deg**, in TWO passes:

1. **Ankle pass** (actor states +0x04==1/+0x05==1): local probe vector
   `(0, 0.05, 4.5, 1)` (stack-built constants 0x3D4CCCCD/0x40900000),
   rotated by Ry(yaw+angle) at the actor position → `func_0019AD00`
   mask **6** (static cells + grid). On a hit, the result poly header
   class (rec+0x1A & 0xFF00) decides: 0x1000 (n-gon) / 0x800 → push
   back; 0x2000 with rec byte+3 == 2 → push back; otherwise (mask-arg
   bit 2) a surface-angle test (func_0019A310 → atan: < 1.2217 rad
   (70°) or > 1.9198 rad (110°) pushes, the in-band slope is free).
2. **Chest pass** (always): from `(0, 4.01, 0)` toward
   `(0, 4.01, 4.5)` local (D_002488C0 = {0, 4.01, 4.5, 1}) via
   `func_0019AFE0` mask **7** (+ movable hulls — doors); hits set a
   per-direction bit in actor +0x314 and run the func_00176390
   responder.

The push-back is `actor x/z += (hit − end)` — the scratchpad
0x700031C0 hit-minus-end delta — i.e. the probe END rests ON the wall:
the effective standoff radius is **4.5 units**, and each iteration
re-reads the corrected +0xB0/+0xB8, so multiple probes accumulate.
Sliding falls out naturally (only wall-facing probes push, along their
own directions).

Port: em_game.c player_wall_probes (replaces the zero-radius pre-move
segment — the reason the player visually clipped halfway into walls);
ankle pass static-only, chest pass + em_door_probe. Move/door tests
updated to the radius geometry (wall rest = plane + 4.5: move test
−174.5, door test min-x 64.5) — all 12 port self-tests PASS.

### Camera wall response — observed-behavior note (port "CAMERA FIDELITY")

**SUPERSEDED s61** — func_0018DD20 is now fully decoded ("CAMERA WALL
SOLVER func_0018DD20 DECODED", end of file): the engine never lifts the
eye to clear a wall; the observed "rise" is constant-height PULL-IN
(the eye keeps its absolute height while the horizontal distance
collapses, so the view tilts down over the player's head). The original
note, kept for history:

Real-game observation (not yet decoded — the solver body func_0018DD20
is unread): a wall blocking the camera's sight line makes the camera
**RISE** above its default height until the line clears (you look down
at the top of the player), NOT pull in toward the player; pull-in
remains for the AIM camera. The player has **no free camera control**;
R1/L1 reorient the camera behind the player (R1 tracks the aim heading
while held), and an idle camera slowly auto-orients behind the player —
apparently only at default height, STOPPING if a wall blocks the
rotation path. Implemented in the port behind flagged PORT CONSTANTS
(em_game.c "CAMERA FIDELITY"); decoding func_0018DD20 and the mode
handlers should replace them with engine values. Per-room FIXED angles
are the cut-table mode-0 per-area director (func_00195130 + overlay
hook 0x823FE0, "CAMERA SYSTEM") — pending overlay decomp.

## WEAPON FIDELITY PASS — semi-family cadence decoded, L3 gate, flashlight identity, anim-id verdicts (2026-06-11, s47)

Static re-read of the rifle fire sub-machine `func_00170A60` (.s) plus a
read-only motion audit of the contested player clips against a fresh
fixed-resolver bake. Drives the extermination-port em_weapon fidelity
pass (per-note implementation list at the end).

### 1. SEMI family gate — the sub-state holds through the cadence

Corrects the port's old flat "pending" latch reading and refines the s23
summary. Sub-state byte `+0x07`, traced branch-by-branch:

```
0    TRIGGER WAIT.  +0x274 (trigger latch) set ->
       mag == 0 -> positional click 0x169 vol 300 ONLY (no reload here;
                   for fire-mode != 0 the state also advances by 1)
       mag != 0 -> family entry by D_00810C61: 0 -> 0xA, 1 -> 0x14,
                   2 -> 0x1E
     +0x274 clear -> raw L3 (D_00810E74 & 0x200) -> func_0017B300(.,2)
       (TOP-UP: only mag < 30 AND reserve > mag); success -> +0x06++,
       +0x1F0 = 0x33 (the RELOAD state). L3 is honored ONLY here and in
       the burst gap 0x17 — never mid-cadence.
0xA  SEMI SHOT: gun+0x2E = 1, mag--, reserve--, 0x164/0x165 by stance
     code, clear +0x2A, fall THROUGH into 0xB the same tick.
0xB  SEMI CADENCE: +0x276 += 2 at the head (so the shot tick itself
     counts); a PRESSED-edge of the fire button (E74 & spad3B78) is
     sampled into the +0x2A queue when counter >= int(+0x2F4) - 8
     (= every tick after the shot tick); at counter >= int(+0x2F4):
     counter = 0, then
       mag == 0 -> func_0017B300(.,1) UNCONDITIONAL reload -> RELOAD
                   (the dry-mag auto reload happens at the EXPIRY, not
                   on the next press; mode 0 lives at the stance ENTER,
                   not in the fire SM); reserve empty -> back to 0
       +0x2A    -> +0x274 = 1, +0x07-- (back to 0xA: the queued shot
                   fires the NEXT tick = exact 6-frame spacing)
       else     -> +0x07 = 0
0x14..0x17  BURST: 0x15 fire (sets +0x2F4 = 12.0 while +0x28 < 2),
     0x16 cadence (+0x28++ at expiry; < 3 -> refire via the same
     step-back; dry -> mode-1 reload, fail -> click via func_001FB9F0);
     0x17 gap (L3 honored).
0x1E..0x20  AUTO: 0x1E fire (sets +0x2F4 = 12.0) -> 0x1F cadence;
     still-latched trigger -> step back to 0x1E (refire next tick).
```

Headline: **SEMI is rate-gated exactly like full-auto** — one press can
never beat the 6-frame interval; a press during the cadence queues at
most ONE follow-up shot (+0x2A is a flag, not a counter). A press in
WAIT fires immediately (WAIT is only reachable a full cadence after the
last shot). This is the engine answer to the port-fidelity report "the
player can shoot as fast as they press the button".

### 2. Reload func_0017B300 — mode map confirmed + the top-up fill quirk

mode 0 = only if mag empty (stance ENTER), mode 1 = unconditional (the
cadence-expiry dry reload), else = top-up, gated mag < 30 AND
reserve > mag (the L3 manual reload — a FULL MAG IGNORES L3). Quirk in
the matched C: the top-up fill compares the reserve against the rounds
NEEDED (`reserve < 30 - mag ? reserve : 30`), not against 30 — with
30-mag <= reserve < 30 the engine writes mag = 30 although the pool
holds fewer. Port mirrors all three arms branch-for-branch.

### 3. FLASHLIGHT — D_00810D3C identity (user-attested) + port wiring

The s36 open item "what does the SQUARE-while-aiming toggle D_00810D3C
arm?" is settled by the user's gameplay knowledge of the real game: it
is the FLASHLIGHT (gun light) toggle. (Static cross-evidence: replayed
0x179 + voice latch on rifle draw; the s28b/s21 light decode is the
matching mechanics block — player +0xA flip, 300-frame (0x12C) auto-off
burst, turn-off anim/event id 0x15D, ZERO battery drain.) Port
(em_weapon.c): SQUARE in the AIM state toggles the light — ON plays
0x179 + arms the 300-frame burst, OFF is silent; the timer ticks every
frame in any stance; expiry turns the light off with the 0x15D switch
sound (920 ms, snd_0361 — added to gen_sfx_registry's office set, 31
ids 0 unresolved) and commits the 0x15D gesture only in the unarmed
idle (armed tops re-select their pose every frame — documented
simplification). Dynamic light RENDERING is a flagged port TODO.
Engine keeps D3C (gun light arm) and +0xA (shoulder light) as separate
bytes — the port unifies them in one flag until a live session
separates their visuals; the unarmed L3 light toggle (s21) is NOT yet
wired in the port (flagged).

### 4. ANIM-IDENTITY VERDICTS — the contested clips, baked + motion-audited

Method: read-only bake of directory ids 51, 267-271 (0x10B-0x10F),
272/273 (0x110/0x111), 274 (0x112) with the CURRENT (directory-fixed)
export_native.py to a scratch EMDL; per-node trajectory audit (root,
gun-mount node 4, hands 19/20, knife/holster node 14). Rig semantics:
parents [-1,0,1,1,2,3,3,2,2,2,8,9,5,6,5,10,11,12,13,15,16]; node 4 =
gun mount (leaf off chest), 19/20 = hands, 14 = hip-holster mount
(second child of thigh node 5), 17/18 = feet.

**True directory lengths vs the s36 table (which measured the
pre-directory-fix bake — shifted by +3 in this range):**

| id | true fr | s36 said | motion verdict |
|---|---|---|---|
| 0x33 (51) | 57 | 57 (unshifted) | **RELOAD, not a stagger**: root planted (zero XZ/Y), knife parked on the thigh, BOTH hands stay on the weapon (inter-hand 0.6-1.1 u) while the gun-mount dives chest->waist (y 12.8 -> 5.5, frames ~14-42, mag work low) and returns to ready by fr 56. The user's in-game "stagger" impression predates the directory-fixed re-export / honest-length windows. |
| 0x10B (267) | 35 | "50" | light hit 1: knife snaps from hip to hand in ~3 fr, wide chest-height slash (hand separation 3.8 -> 7.5 u), return to guard |
| 0x10C (268) | 35 | "25" | light hit 2: long crossing return-cut, starts extended (9.3 u) finishes tight (1.4 u), knife held high early |
| 0x10D (269) | 50 | "20" | light hit 3 FINISHER: overhead rise (knife y -> 15.0) then dive to 7.4, settle + re-holster tail |
| 0x10E (270) | 50 | "20" | HEAVY: fastest entry (1.6 u/fr at fr 1-4), knife drops low (y 6.1) then drives up through to overhead (y 17.2 at fr 15) — rising stab; long recover + re-holster |
| 0x10F (271) | 25 | 25 | hit-confirm recover: raised knife sweeps down and re-seats on the hip |
| 0x110 (272) | 20 | "25" | rifle DRAW/raise: both hands together lift chest 12.8 -> 14.2 |
| 0x111 (273) | 20 | "25" | HOLSTER: reverse lower, off-hand leaves the grip |
| 0x112 (274) | 25 | 25 | aim pose: near-static at y 14.2-14.4 (front-frame recoil micro-snap) |

**Knife visual RESOLVED (retires s36 §6 "no rebind found")**: there is
no rebind and none is needed — the hip-holster node 14 is itself KEYED
INTO THE HAND by every swing clip (rides hand node 20 at ~1.0 u for the
whole swing, hip distance ballooning to 9-15 u, re-seats at the end; in
idle/walk/reload it stays at the thigh, 3.9 u constant). The knife
model attached to node 14 swings with the attack automatically.

**Impact-gate timing RESOLVED (retires the s36 TIMING open item)**:
under the TRUE lengths the down-count reading `impact = len - T` is
self-consistent for all four attacks — impacts at fr 11/9/9 (light) and
7 (heavy), each before its release gate (15/20/20/21) and inside the
clip; the up-count reading would put every release before its impact.
The s36 contradiction ("T=24 on a 50-frame clip") was an artifact of
the shifted 50-frame length.

**Re-export status: NO further re-export needed.** The port's
player.emdl (re-baked 2026-06-11 by the s45 directory fix, canonical
CLI in export_native.py's header) is byte-identical per clip to the
fresh fixed-resolver bake for every contested id (51, 267-274 palettes
compared byte-wise). Note for the exporter owner: nothing to change;
this section is the requested verification record.

### 5. Port implementation (extermination-port, this session)

- em_weapon.c fire logic rewritten as the engine sub-state machine
  (§1): WAIT/SEMI/BURST/GAP/AUTO + next-tick chained shots; semi is now
  cadence-gated under mashing (user note 3 FIXED); dry-mag auto reload
  moved to the cadence expiry (mode 1); WAIT dry press = click only.
- weapon_reload mirrored branch-for-branch incl. the top-up quirk (§2);
  L3 honored only in WAIT/GAP (user note 1 — full mag ignores L3).
- FLASHLIGHT per §3 (user note 2); melee fallback lengths corrected to
  the true clip lengths, CIRCLE=light/SQUARE=heavy wiring re-verified
  NOT inverted (user note 4).
- NEW headless unit test `make test-weapon` (tests/weapon_fire_test.c,
  links em_weapon.c + stubs): semi mash = 11 shots/61 frames at exact
  6-frame gaps, single press = 1 shot, L3 gates (full/short/equal/
  quirk), expiry auto-reload, flashlight toggle + 300-frame burst,
  auto 31-frame hold = 6 rounds — PASS.
- All port self-tests PASS (weapon/melee/enemy 1-5/door/transit/sfx/
  move/pause/input); default capture byte-identical (6d23fe44...).

## STATUS-MENU UI SCENE DECODED — the rotating player model (2026-06-11, session 49)

Static decode of the status screen's 3D scene — what the identity UI
camera (s25) actually looks at: `func_0020CDC0` state 0's scene setup,
the menu-player static-actor behavior `func_0020E6F0`, the equipment
companion spawner `func_0020E250` and the matrix publisher
`func_0020EC80`. Closes the s44 open item "the rotating player model on
the black UI scene stays the port's documented 3D-in-UI TODO" — the
port now ships it.

### Scene setup (func_0020CDC0 state 0)

- On open the hub clears the 24-slot STATIC actor array `D_0028B020`
  (0x2F0 each, s15's second registry): `func_001AFEB0` frees every live
  slot (`func_001AF800`), `func_001AFE60` memsets all 24; `func_0020E020`
  zeroes the 16-slot sparkle pool `D_00821300`. The static array is the
  menu's private stage.
- `func_001AFF10` allocates the first free slot (status=2, scale rows
  1.0, +0x94=-1); the hub installs behavior **`func_0020E6F0`** at +0x10,
  then calls `func_0020E250`.
- `func_0020E250` spawns COMPANION actors (behavior `func_0020E460`) for
  the equipment display: base entries `func_0020E3A0(-1)` and `(0x10)`,
  plus weapon-conditionals (`D_00810CA4`==2 → 0xC; ==0 → 0xA +
  `D_00810CA6`; else `D_00810CA5/CA6/CA7`, and `D_00810CA6`==4 adds
  0x15 — the fuel tank). `func_0020E3A0` maps id+2 over
  `jtbl_002735D0` → GLOBAL model-library ids (0x2F..0x3D/0x40/0x6D);
  `func_0020E1E0` allocs a slot and binds the model from `D_0028A56C`
  (`func_001C6120` + `func_001CA6E0`), bone count → +0xC, source id →
  +0xD.
- `func_001B0000` ticks the static array every hub/page frame, BEFORE
  the input scan (`func_0020D930`) and the panel drawer
  (`func_00209DF0`) — so the 3D scene draws under the background tiles
  (func_0020A7A0, s44) and the panels.

### The menu player (behavior func_0020E6F0, 0x58C — state byte +0x04)

**State 0 (init):**

- model from the PLAYER VARIANT table: flag `D_008104E4`==0 →
  `D_00810C60` 1→`D_0028A588`, 2→`D_0028A58C`, else→`D_0028A57C`;
  flag==1 → 1→`D_0028A588`, 2→`D_0028A58C`, 0/else→`D_0028A590`;
  flag>1 → `D_0028A584`. Bind `func_001CA6E0`; bone count
  `func_001C6150` → +0xC/+0x09; bone matrix slots `func_001AF7C0` →
  +0x110[i]; `anim_bone_array_setup`.
- +0x40 = `D_0028A580` — the player clip library (chunk28/f01_id3c).
- clip on DISPLAYED health `D_00810858`: **> 35.0 → clip 0x1C2 (450),
  <= 35 → clip 0xA (10)** via `bone_init_default_2`; +0x0B records which.
  **Clip 0x1C2 is a SINGLE-FRAME container** (verified at the resolver:
  1 frame) — the healthy menu player is a STATIC STANCE; all motion is
  the turntable yaw. Clip 0xA is a 90-frame weak/low-health idle.
- `func_001CA5F0(actor, 0xB)`.
- infection tint vec +0x80 (vec4; alloc default 1.0) =
  `(-0.8, -1.0, -0.3, 0) * D_0081085C` (constants 0.01·(-80/-100/-30);
  infection display copy) — a GS 128-base color DELTA; ramp +0x38 = 1.0.
- rotation +0xC0 = **(0, pi, 0)**; position +0xB0 = `M[3] + 40.0*M[2] +
  7.4*M[0] + 2.4*M[1]` over the view-matrix columns at `D_00810610` —
  with the identity UI camera (set on open, s25) that is **view-space
  (x 7.4, y 2.4, z 40)**: right of axis, slightly below the eye line
  (GS y-down), 40 units in front. Scale is NEVER written (stays the
  alloc's 1.0). State → 1.

**State 1 (run, every frame):**

- ramp +0x38 breathes **1.0 ↔ 1.3 at ±0.01/frame** (direction flag
  +0x05) — 2 s full cycle;
- tint re-derived: +0x80 = -0.8·i·ramp, +0x84 = -1.0·i·ramp **clamped
  >= -127.0**, +0x88 = -0.3·i·ramp — at infection 100 the delta swings
  to ~(-80..-104, -100..-127, -30..-39): the pulsing dark blue-purple
  "infected" skin;
- **yaw +0xC4 += 0.01 rad/frame**, wrapped > pi → -= 2*pi: one full
  revolution every ~628 frames ≈ 10.5 s;
- clip swap-back only: displayed health back > 35 while +0x0B==1 →
  `anim_clip_init(actor, 0x1C2, 16.0, 0.0)`, +0x0B=0 (nothing swaps TO
  0xA mid-open — only init picks the low clip);
- `anim_advance_time(actor, 1.0)`; publish via `func_0020EC80`.

**States 2/3:** `func_001AFF90` — free the slot.

### The publisher (func_0020EC80)

World matrix built in scratchpad: rotX(+0xC0)·rotY(+0xC4)·rotZ(+0xC8)
(funcs `0x102B08/0x102BB0/0x102A60`) composed with **diag(-1,-1,-1)**
and an EXTRA rotY(pi), translation from +0xB0 (`func_001031E0`), copy
to spad 0x36A0, submit `func_001C69A0`. The basis flip + extra Y-pi
make the +0xC4=pi init face the camera upright in the y-down identity
view. When `D_008104E4`==1 it also writes a color triple
(3.2, -1.5, -0.6) to spad 0x700038A0 — the infected-variant override.

### Port (extermination-port s49)

- `assets/player.emdl` re-exported with the menu clips appended
  (`--clips ...,0,450,10`; byte-verified superset — verts/indices/
  textures/old palette+clip table identical prefixes; 450 bakes as
  1 frame, 10 as 90).
- em_game renders the UI-CAMERA 3D SCENE in place of the world whenever
  the status screen is visible AND the active sheet carries a BACKDROP
  record (em_hud_backdrop_ready): a black fullscreen BACKPLATE quad
  (drawn through em_gfx_draw_skinned_tinted black — the engine's black
  UI frame) + the player at the decoded transform (view-space
  (7.4, 2.4 down, 40), yaw pi + 0.01/frame, scale 1, menu pose 450 /
  low-health idle 10 at <= 35 displayed health, multiplicative
  approximation of the additive infection pulse). em_hud_scene_3d()
  tells the background drawer to skip its opaque base fill so the
  player sits between the black frame and the translucent tile layers
  — the engine's draw order exactly.
- **UI projection PINNED by anchor consistency**: the decoded 7.4-unit
  x offset at z 40 projects exactly onto the ring-gauge center column
  (canvas x 208 of 512, NDC -0.1875) iff the projection x scale is
  0.1875*40/7.4 = 1.01351 — i.e. **tan(fovy/2) = 0.74 at the GS 4:3
  frame (~73 deg vertical)**. The port locks this 4:3 projection for
  the UI scene (the frame stretches with the window exactly like the
  panel canvas), keeping the model registered on the ring at any
  window size. (The clean 0.74 suggests this IS the engine's UI/world
  projection constant — worth checking against func_001D2960's GS
  scale when that path is decoded.)
- Port lighting deviation (renderer stand-in, not engine): the gfx
  layer lights characters with a fixed directional stand-in
  (L = (0.4, 0.8, 0.45)); under the raw identity camera that leaves
  the menu player at the ambient floor (near-black on black). The
  port orbits the whole UI rig to the light's azimuth — the relative
  camera<->player transform (and therefore the framing) is unchanged,
  but the camera-facing side is the lit side.
- Engine values NOT carried: the equipment companion actors
  (func_0020E250's separate models; the port's player mesh already
  carries the attached weapon) and the engine's time-seeded pulse
  rand (the port background keeps its fixed-seed LCG).
- Spin phase is deterministic for captures: state re-inits on every
  open/force edge (yaw = pi), advancing 1 tick per rendered frame —
  EM_HUD_FORCE captures at frame N always sample yaw = pi + 0.01*N.

_Last updated: 2026-06-11 (session 49)._

## MODE-0 CAMERA DIRECTOR func_00195130 DECODED — fixed room cameras are MAIN-ELF data (2026-06-11, session 50)

Static decode of the mode-0 area-camera director (cut-table mode 0,
func_00195130, 0x1270 bytes) plus its helpers, driven by the exported-area
question "which rooms own a specified camera angle?" (the live observation:
in such rooms L1 won't reorient, and releasing the R1 aim INSTANTLY snaps
the camera back to the room's setting). Sources: local splat tree
(`build/asm/matchings/main/code/`), overlay splits
(`build/overlays/*/asm/`), region table bytes re-read from the local boot
ELF (LOAD file offset 0x94 → vaddr 0x100000).

### 1. Director structure

`func_00195130(cam = 0x008101E0, player)` — sub-machine on cam+0x01
(states 0..4; 0 arms 1; 2/3 are solver/commit-only arms; 4 is an AREA13
set-piece machine on cam+0x02 with three hardcoded eyes (669.3, 182.3,
1082.5) / (765.6, 279.1, 1103.4) / (773.6, 387.3, 1136.7)). State 1 (the
per-frame path) runs func_001916C0(cam, player, 0) (generic target/eye
follow pre-step), then a **hardcoded switch on the area byte D_00810700**
with cases ONLY for areas `0x00 0x04 0x06 0x08 0x0B 0x0D 0x0E 0x0F 0x11
0x13`; every other area — **including AREA01 and AREA02** — falls through
to the generic chase `func_001921D0(cam, player, 0)` (the s10 "smooth
follow": target→player cap 0.8/frame, y +15/+aim 6; its only area test is
an area-0 chunk-5/6 eye-z clamp at −1449.0). A case that fires writes the
desired EYE (cam+0x10/14/18) from **float immediates in the director's
code**, runs the solver func_0018D7B0(cam, 5), then steps the ACTUAL eye
D_008105D0 toward it with the chase primitives (func_0018C6A0 horizontal /
func_0018C4B0 vertical, case-specific caps 0.7–0.9) — or hard-copies it
via func_00102948 in some player states (e.g. area 0x0B state 7). The
desired TARGET keeps tracking the player in every fixed-camera case.

### 2. The trigger-volume table D_0024A5F0 (func_00194D10)

`func_00194D10(cam, player, idx)` = the region test: player XZ inside the
0x40-byte quad record `D_0024A5F0[idx]` (4 vec4 corners, walked by
func_001B1EA0 mode 0 — point-in-polygon, stride 0x10; all 3 shipped
records are axis-aligned rects) AND `|player.y − corner0.y| < 4.0`.
Records (ELF-verified, matches the splat data words):

| idx | XZ rect | y gate | binding | fixed eye (code immediates) |
|---|---|---|---|---|
| 0 | X[779.5, 816.5] Z[1230.5, 1279.5] | 160.6 | AREA13 (0x0D), entry≥8 path | (839.8, 198.0, 1217.3) |
| 1 | X[319.6, 339.0] Z[150.0, 182.0] | 289.8 | AREA11 (0x0B) — func_00230000 AIM target-height tweak (+12), not a fixed eye | — |
| 2 | X[−370, −340] Z[−620, −600] | 60.0 | **AREA06 (snow)** director case .L001952F8 | (−367.7, 90.0, −598.9), approach 0.7/frame (y first, then xz) |

Other fixed-camera sources in the director: per-player-state scripted
cameras `func_001944B0(cam, player, idx)` (states 0x1D..0x25 via
jtbl_0026DA50, eye tables D_0024A530/34/38 — ladder/scripted moments),
and per-sub-state hardcoded eyes in the area 0x08/0x0D/0x11/0x13 cases.

### 3. CORRECTION — the `jal 0x823FE0` "overlay hook"

The s10 note "per-room camera logic lives in the area overlays (jal
0x823FE0 hook)" is WRONG in its generality. The director's single
`jal 0x823FE0` (0x00195D18) is inside the **area 0x0D case only**, gated
`D_00810702 (entry idx) >= 8`; a nonzero return selects a second AREA13
fixed eye (801.3, 282.3, 1171.0). And in the shipped AREA13.BIN, vram
0x823FE0 is **mid-function** (inside the overlay's own region helper
0x823FA0: quad at 0x82E1C0 + player.y > 210 test, full prologue at
0x823FA0) — a call entering at 0x823FE0 would run `lq ra, 0(sp)` against
the caller's frame: dead or build-drifted code. (AREA02 happens to have a
function boundary at 0x823FE0 — its overlay brain dispatcher — pure
layout coincidence; nothing calls across.) Per-room fixed cameras are
main-ELF data, not an overlay delegate.

### 4. Exported-area verdict (the honest answer)

- **AREA02 (office, subs 0/1/2): NO fixed cameras** (s56 SUPERSEDED —
  true for the DIRECTOR only: the SPAWN-RECORD mechanism, s56, pins
  the supply-room corner camera at area 2 room 1 entry 3). No director case;
  the AREA02 overlay contains zero stores to the camera struct/pool
  (grep over 0x8101E6/0x8101F0/0x810200/0x8105D0/0x8105E0) — all chase.
- **AREA01 sub 0 (drawbridge): NO fixed cameras.** No director case; the
  overlay's only camera touch is the drawbridge-cutscene retarget —
  func_00102948 copies object+0xB0 into the ACTUAL TARGET D_008105E0
  (0x825C90 / 0x825D28), alongside a 0x8107DF=2 cutscene byte. An event
  camera, not a room camera.
- **AREA06 (snow, exported as scene_snow): ONE real region** —
  D_0024A5F0[2] above. Far from the exported spawn (218.6, 229.85,
  201.8) and at a different elevation (y gate 60), so it cannot cover
  the spawn.

### 5. Export + port contract (shipped this session)

`tools/export_level.py --camregions DIR --area N` rewrites a
marker-delimited block in DIR/scene.txt: real lines
`camregion x0 z0 x1 z1 ygate ex ey ez` (rect read from the user's local
ELF; eye = the decoded immediates) where a binding exists, else the
decode-verdict comment. Emitted: scene_snow = the real AREA06 region;
scene / scene_office0 / scene_drawbridge = verdict comments only.

Port (extermination-port em_game.c): inside a region (XZ rect +
|y−ygate|<4) the camera eye is PINNED to the spec (chase and wall solve
disabled), the target keeps tracking the player, the struct yaw becomes
the fixed sight-line heading (movement stays camera-relative), L1 and
the idle auto-orient are NO-OPS, and the R1 aim camera still runs — its
release re-pins the eye the SAME frame: the observed INSTANT snap-back
(no lerp). Noted deviation: the engine approaches the snow spec at
0.7 u/frame on entry; the port uses hard placement per the observed
"chase disabled" behavior. `EM_CAMREGION_TEST=1` proves the machinery
with a FLAGGED SYNTHETIC office region (the office has no real ones):
enter→eye at spec exactly, L1 no-op, aim moves the eye >6 u, release
snaps back within 2 frames — PASS; default capture byte-identical.

## FLASHLIGHT RENDER DECODE — the toggle draws NOTHING; the engine's whole vertex-lighting chain decoded instead (2026-06-11, s51)

Closes the s47 flagged TODO "decode how the engine renders the light when
ON (player +0xA / D_00810D3C)". Static-only session (no live capture);
method: exhaustive reader sweep of both flags across the full boot-ELF
.s tree + the 19 overlay trees, then a decode of the render-side light
pipeline those sweeps led into.

### 1. VERDICT — no render-side consumer exists

- **`D_00810D3C` (gun-light arm, SQUARE-in-aim)**: its only readers in
  the entire image are the toggle itself (`func_0017A970` att 0), the
  rifle-draw sound replay (`func_0016F530`: att==0 && D3C -> replay
  0x179 + voice latch), and the inventory reset (`func_001AF2C0`).
- **player `+0xA` (shoulder light, L3)**: no static reader goes through
  the player base symbol at all (`D_008102BA` never appears; register-
  tracked scan of `%lo(D_008102B0)`-derived bases finds zero `+0xA`
  loads). Every `lbu +0xA / andi 1` consumer is ENEMY-AI entity logic
  (0x128C10, 0x12A5D0, 0x12EB60, 0x1333F0, 0x138900, 0x138C20,
  0x13D850, 0x13D980, 0x1418F0, 0x1469B0, 0x147960, 0x147B50,
  0x14BB10): the light flag feeds DETECTION. `func_001418F0` (callers
  0x13D850/0x13D980/0x13DD40) is the clean read: player light ON ->
  awareness `+0x70 = 0xF0` instantly; light OFF -> atan2 to the camera
  (`D_00275B40->0x3C` pos), `func_001B3F10(player, angle, 12.5)` view-
  cone test, ramp `+0x78` against a difficulty threshold
  (`D_002753C8[D_0081050C & 3]`). The remaining flag plumbing is the
  s28b-decoded SM (`func_00161020`: 0x12C burst, off-anim 0x15D) plus
  the anim-row substitution (`func_001B0070()&4` -> locIdx+0x10
  light-carrying poses; bit 2 of the per-AREA flags word `D_008106C8`,
  loaded from the area table at `func_001B0250`, +0x1C of the entry).
- Overlays: no overlay references D_00810D3C, the player base, or the
  light-registration API below. **The flashlight's player-visible
  "light" in the real game is not a drawn beam/cone/sprite at all** —
  it is (a) the pose change, (b) the always-on camera light below, and
  (c) gameplay (enemies notice you). The port's visible cone is
  therefore a DOCUMENTED DEVIATION (see §4).

### 2. NEW — the per-actor VU1 light matrix chain (the s7b "light rows")

The 4-qw upload to VU1 dmem **0x3F5 = qw 1013..1016** (the skinning
kernel's lighting matrix) is built per actor per frame:

- `func_001C7420(actor, dmem=0x3F5, idx)` — emits the draw-unit CNT:
  4 qw from scratch `D_70003440` -> dmem 0x3F5, then the per-node
  7-qw {transform + normal-matrix} sets -> dmem 0 (all callers pass
  0x3F5; the multi-set level variant `func_001C7900` ditto via
  `func_001D88B0`).
- `func_001D89D0(actor, out3400, out3440, actor+0x80)` — THE BUILDER:
  - lighting-override mode `[D_00275670+0x246C]` (set by
    `func_001D8C20(mode)` from each draw class; modes 1/3/4/5/6 ->
    `func_001D8C30` jtbl 0x0026E520 special paths, menu/cutscene rigs).
  - light reference point: `actor+0x98` node idx (0xFF -> actor+0xB0
    position, else node world matrix `[actor+0x110[idx]]+0xC0`).
  - `func_001D8130` — copies the CURRENT ROOM's light rig into the
    working set `D_00817BC0` (ptr mirror `D_00275688`): rig source =
    `func_001D7B30` lookup of `(area<<8)|room` (`D_00810700/701`) in
    the 45-entry x 0x78 static table **`D_00251C50` = the per-room
    light-rig table** (3 directions + 3 colors + ambient, +0x1C..+0x70
    of the record).
  - `func_001D8340(actor, out, out, flag, point)` — composes 3
    directional rows + color rows. **Light slot 0 is special: when
    `flag` (= actor byte +0x2 bit 0x20) is set, its direction is
    replaced per-frame by the CAMERA VIEW DIRECTION** (transform of
    `D_00810610`, the camera lookat matrix written by the camera update
    `func_0018C0D0`); when clear, slot 0 is zeroed. Then up to **32
    DYNAMIC POINT LIGHTS** (slots at `[D_00275670]+0x220`, stride 0x80:
    +0x10 pos, +0x2C intensity, +0x20/0x40 color rows; constants
    `D_00253170/80`) contribute distance-attenuated terms
    (0.1 * intensity / max(dist,1), x10 dir / x2 color, via the
    `func_001D8270(actor)` gate).
  - `func_001D8690` folds direction rows into the actor frame
    (`actor+0x80`); actor flag +0x2 bit 0x40 adds a self-glow color
    term (`64 * actor[+0x80..0x8C]` onto matrix row 3).
- **`func_001D8BF0(actor, on)` = set/clear the camera-light flag** (+0x2
  bit 0x20). The PLAYER gets it ONCE at init (`func_001AF5C0`, which
  also zeroes the 0x320-byte player struct) — i.e. the protagonist
  carries an ALWAYS-ON camera-following fill light; NPC spawners
  (`func_001BA540/8E0/D40`, `func_001C1030`) set it on other
  characters. It is NEVER keyed to the flashlight flags.
- **Dynamic-light API**: `func_001D7BB0` clears the 32 slots each frame
  (caller `func_001D19E0`), then `func_001F68B0`/`func_001F6E40`
  register the room's PLACED lights (per-room lists, tables
  `D_0025D270/D_0025D2C0`, colors `D_0026EB70[type]`) and effects add
  theirs (`func_001EF9D0` -> `func_001D80E0/0x1D8100` presets;
  `func_001C50B0`, a behavior-installed flicker-light actor with
  per-room color cases keyed `(area<<8)|room`, random-walk intensity).
  Register = `func_001D7FA0(pos, color, type, ...)` -> slot handle;
  release = `func_001D80B0(handle)`.
- LEVEL geometry never goes through any of this: it streams baked
  vertex colors through its own kernel (s7/s13 findings stand).

### 3. Bonus identifications (en route)

- `D_008106C8` = the per-AREA FLAGS WORD (area-table +0x1C via
  `func_001B0250`; `func_001B0070` is its getter — bit 2 = "dark area"
  pose substitution, bit 0x80 fog/depth-fade, weather bits per s13).
- `func_001AF5C0` = player-actor init (zeroes 0x320 bytes, sets the
  +0x90 matrix mirrors, camera-light flag on).
- `func_0018C0D0` = camera commit: writes the lookat `D_00810610`
  (+ scratch eye/target `D_700038A0/D_700038C0`, yaw `D_008106A0`).
- `func_001551B0` = the alarm broadcaster that walks the actor list
  (`+0x1C` next) setting `+0xA = 1` on matching types — the s33
  "group-alarm" writer (NOT the player light; +0xA is per-behavior).

### 4. Port (extermination-port, same session)

`em_gfx_spot_light` (em_gfx.h + Metal backend): one per-frame forward
SPOT term in the skinned shader, applied to both the directional
stand-in path (characters, N.-L-wrapped) and the baked-vertex-color
LEVEL path (pure projected cone -> the light disc on the wall);
em_weapon sets it each update from the hand-frame muzzle ray while the
flashlight flag is on (`EM_CAPTURE_LIGHT=1` capture knob). Engine truth
+ deviation documented at the API and in em_weapon.h "RENDERING". The
off state is bit-exact: spot-off frames reproduce the historical
default capture hash (the o.pos expressions were kept byte-identical —
reworking them flipped rasterizer edge pixels, caught and reverted).
All port self-tests + make test-input/test-weapon PASS.

## WEAPON-VISUAL FIDELITY: FX SPRITE TEXTURES EXPORTED + TEXTURED BEAM PASS; FLASHLIGHT MODEL CORRECTED (2026-06-11, session 52)

User-fidelity pass against a reference capture of the original (while
aiming: a SHARP-EDGED light circle on the wall + a thin laser with a
small SMOOTH dot). Three fixes; the texture identities are new decode.

### 1. The weapon FX sprite textures, identified and exported (--fx)

`tools/export_props.py --fx` writes four raw-RGBA `.emtx` files (v1:
"EMTX", u32 version, u32 w, u32 h, RGBA8 rows top-down) into the
port's `assets/fx/`, texels resolved from the office GS dump (the
chunk27 sheets are globally VRAM-resident):

| file | TEX0 key (CLD-masked) | size | sampled by |
|---|---|---|---|
| laser_dot.emtx | `0x00045BA5154222DC` | 32x16 PSMT4 @ TBP 0x22DC | the laser drawers' func_001CD520 sprite call (s23b disasm literal `0x20045BA5_154222DC`) — a soft grey radial blob; the 2:1 image on the engine's SQUARE 3x3 quad squeezes to a round dot |
| flash_puff.emtx | `0x000457B5594220A0` | 64x32 PSMT4 @ TBP 0x20A0 | muzzle-flash models 0x0D (spawn puff) and 0x07 (tick-3+ star), full-frame |
| flash_star.emtx | `0x00045915594220A4` | 64x32 PSMT4 @ TBP 0x20A4 | model 0x08's 20 forward-streak records (X[0,4.9] — the +X star) |
| flash_ball.emtx | `0x00045935554221E6` | 32x32 PSMT4 @ TBP 0x21E6 | model 0x08's 12 muzzle-base records (X~0, +-1.3 YZ radial cross) |

CORRECTION to s43: "all three sample ONE additive effect sheet" was
incomplete — model 0x08 carries TWO additional private keys (above);
only 0x0D and 0x07 share the 0x20A0 sheet. The exporter reads the
flash keys from the models themselves (self-verifying: 0x0D/0x07 must
share one key, 0x08 exactly two, star-vs-ball split by +X extent) and
keeps the dot key as the disasm constant.

### 2. Port: textured world-space billboards (the real dot + flash)

- em_gfx (em_gfx.h + Metal): `EM_GFX_BEAM_TEX_MAX` (4) texture slots
  (`em_gfx_beam_texture_set`) + `em_gfx_beam_tex` (axial-billboard
  quad, u along the segment) and `em_gfx_beam_dot_tex` (camera-facing
  sprite) — same additive / depth-test-on / write-off state, same
  budget and flush as the untextured beams; textured records draw
  after the untextured set, grouped by slot (one runtime-compiled
  pos+color+uv pipeline). The untextured pass is kept expression-for-
  expression, so pre-texture frames stay byte-identical.
- em_weapon: loads the `.emtx` files lazily (fx_load; missing file =
  silent flat-color fallback). LASER DOT = ONE textured additive
  sprite at the endpoint modulated by the flickering engine red (the
  GS TFX-modulate draw) — replaces the 3-layer concentric flat-color
  squares the user flagged ("3 distinct shrinking squares"). MUZZLE
  FLASH = the engine's own model-per-tick schedule as textured
  billboards: spawn tick = 0xD puff (camera-facing), ticks 0..2 =
  model-8 star streak (axial, own sheet) + muzzle ball, tick 3+ =
  model-7 star on the puff sheet; the 0.8^t intensity decay stays as
  the flagged stand-in for the untranslated rotation lerp.

### 3. FLASHLIGHT MODEL CORRECTED (user-attested vs the original)

The port had hung the s28b 300-frame auto-off burst off the Square
toggle (light "ran out" after 5 s with a switch sound) — WRONG against
the real game: the gun light NEVER runs out, and light + laser appear
ONLY while aiming (not even during a reload). Corrected model:

- D_00810D3C is a PERSISTENT PREFERENCE: Square-in-aim toggles it
  (ON = sound 0x179, OFF = silent), NO timer, no battery; it survives
  holsters/re-draws (the engine replays it on the next rifle draw).
- The 300-frame burst + 0x15D expiry (sound + gesture) belong to the
  SEPARATE s28b shoulder-light stealth system (player +0xA family).
  The port KEEPS that code (em_weapon.c "SHOULDER-LIGHT BURST") but
  UNHOOKED — nothing arms it until the L3 input path is decoded;
  `em_weapon_flashlight_timer()` introspects it (always 0).
- RENDER GATE = the laser's: the spot term is set only in the AIM
  phase (w.laser_on), so light and laser appear/vanish together.
- The projected circle is SHARP per the reference: cone tightened to
  cos 11.5deg inner / cos 12.5deg outer (a crisp disc with a ~1-degree
  smoothstep rim) from the old 15->25-degree wash.

### 4. Verification

make test-weapon rewritten honestly (persistent preference: no
auto-off after 400 frames, no 0x15D, flag survives holster + re-draw,
manual OFF silent) — PASS; test-input + PAUSE/DOOR/MELEE/CAMREGION/
TRANSIT/ENEMY 1-4/SFX self-tests all PASS. Captures: default frame
byte-identical to the pre-change baseline (cmp exact); EM_CAPTURE_AIM
+ EM_CAPTURE_LIGHT shows the sharp-edged disc on the wall with the
crisp rim; the laser dot reads as one soft round red glow (the old
squares are gone); EM_CAPTURE_AIM=2 frames 59/60 show the textured
puff then the textured star streak at the muzzle. Reload/draw/holster
render neither light nor laser (shared gate, by construction).

_Last updated: 2026-06-11 (session 52)._

## CAMERA & AIM CONTROL DECODE WAVE — idle-orient timer, aim camera mode 1, manual aim steer, door camera cues (2026-06-11, session 53)

Static decode driven by four user fidelity notes against the real game
(idle reorient timing, aim up/down steering, aim camera placement, door
transit camera). All four mechanisms decoded from the local splat tree
and shipped natively in `extermination-port` (em_game.c). Helper leaves
pinned along the way: `func_0011E2A8` = sinf, `func_0011DE90` = cosf
(the 0.5-pitch special case in func_0017ABA0 proves the assignment),
`func_0011E620` = atan2f, `func_0011E748` = sqrtf, `func_0011DF78` =
fabsf, `func_001B5DC0(byte)` = |byte-0x80| deflection band through
rings 49/89/123 -> 0..3 (the gait quantizer's rings, single-axis).

### 1. IDLE AUTO-ORIENT — the real trigger is a 481-frame camera timer

`func_001921D0` (mode-0 generic chase), idle path (player states
1/0x26/0x27), tail block `.L00193448`:

- gate `(cam+0x07 & 9) == 0` (solver block bits clear), player state
  +0x230 in {1, 2}: `cam+0x08` (the mode timer) increments per frame,
  any other state resets it;
- at `>= 0x1E1` = **481 frames (~8.0 s)**: delta = normalize(player
  heading +0xC4 - cam yaw +0x44); if `|delta| > 0x3D567750 = 0.052368`
  (3 deg deadband) AND the rotation direction's solver wall bit is
  clear (delta < 0 needs ~(+0x07 & 4), else ~(& 2)): arm sub-state 2
  with `+0x48` = player heading, `+0x4C` = |D_0081069C| (actual horiz
  eye<->target distance), `+0x40` = 0.022222*|delta| min 0.0034907
  (stored; the orbit itself uses the fixed rate), `+0x03` = direction;
  timer resets either way.
- **481 = the 300-frame fidget timer + the 180-frame look-around clip
  0x15D** — the engine reorients exactly when the look-around idle
  ENDS, the user-observed anchor ("right around when the look-around
  idle plays, towards the END of that animation").

The orbit = director sub-state 2 -> `func_00193D90`: per frame
`func_001916C0(cam, player, 1)` target follow, yaw steps
**0.0034907 rad/frame (0.2 deg/frame)** toward +0x48
(func_001B12B0), `eye.xz = target.xz - (sin,cos)(yaw) * (+0x4C)`;
exits to sub-state 1 when aligned, when the player leaves states 1/2,
or on solver wall bits (0xD pre-direction-latch / 0xB after).

Also decoded en passant: `func_00191390` (the per-frame pre-step) is
the **+0x8C target-height table** — states 8/9/7/6/0x2C/0x2D -> 0,
0x13 -> 11, 0xF/4/2 -> -3, states 1/3 (and default) -> 6.0 — or 2.0
when the per-area param +0x64 == -31.2 (resolves the s23 "AREA02 live
read 2.0 vs documented 6.0"). `func_00191210` = an area-0x10-only eye-z
clamp (<= 507).

### 2. AIM CAMERA — mode 1 fully decoded (func_00197D20/740/870)

`func_00197D20` (cut-table mode 1) is a sub-machine on cam+0x01:
sub 0 entry saves the player pos to spad `D_70003040`; sub 1 (until
player +0x1F1 == 1 commits the aim pose) runs `func_00197740`; sub 2
(states 0xD = R1 stance / 0x2A = R2 stance) runs `func_00197870`;
states 0xC/0x29 (release) set **camera mode 2** (the blend-back
transition func_00198650) after re-saving the exit pos.

- **Entry** (func_00197740): rotation R = rotEuler(cam+0x30 — the
  chase camera's euler vec, (0, yaw, 0)); desired TARGET = player +
  R*(0, 19, 6), desired EYE = player + R*(0, 19, -30); actual target
  chases at 0.4/frame (both primitives), eye via solver style 2 +
  4.0/frame chase.
- **Steady** (func_00197870): base = player pos (state 0xD) or the
  ENTRY-SAVED pos (state 0x2A); aim dir = gun+0xC0 (the posed hand
  matrix +X). Desired TARGET = base + dir*16 + (0, 19, 0). Desired
  EYE = player + rotEuler(D_70003B50 = player rotation +0xC0, i.e.
  30 u behind the FACING — it tracks the turn-in-place)*(0,0,-30),
  with EYE.y = player.y + 19 - 30*dir.y where (-30*dir.y) clamps to
  >= -25 (f20 = 22 + that in [-3, 0]); EYE.y clamps into [player.y+2
  (areas with cam+0x5A bit 0x10: +11), player.y+30]; anti-close: horiz
  eye<->player dist < 7 raises EYE.y to player.y + 18 + f20 (a second
  arm re-places at -5 behind when cam+0x54 allows). Target chase 0.6,
  eye 4.0 (a2 != 0 = hard copies).
- **Dispatcher tail**: after the chase, if EYE.y > player.y + 23 and
  the horizontal eye<->player distance < 8, the eye is pushed out to
  EXACTLY 8 along its own heading (atan2/sin/cos — the min-distance
  clamp).

This retires the s23 "+0x8C aim height" port stand-in: mode 1 never
reads +0x8C; the over-shoulder look-down-the-barrel comes from the
target riding the aim line (dir*16) while the eye counter-moves
(-30*dir.y).

### 3. MANUAL AIM STEER — func_0017ABA0 (the +0x278/+0x27C writer)

Per aim frame (the stance tops; func_0017AF70 replaces it only with a
locked target D_008106E0):

- rate row by stance code +0x1F0: {0, 0.0025, 0.005, 0.015}/frame for
  0x31/0x34 (body-turn factor f20 = 1.0), {0, 0.0016667, 0.005,
  0.015} for 0x32/0x35 (f20 = 1.5); row index = func_001B5DC0 of the
  pad axis byte (D_00810E64 X / E65 Y);
- YAW (+0x27C): step = rate / sin(pi*(0.5 + 0.6*(pitch-0.5))) (the
  pitch == 0.5 case is special-cased to 1.0 — this pins func_0011E2A8
  as SIN); axis >= 0x80 increments toward 1.0, else decrements toward
  0; overflow past [0,1] clamps and rotates the BODY heading +0xC4 by
  the excess * f20 (sub-weapon 4: rate * 1.5);
- PITCH (+0x278): step = (rate * mult)/2; **axis >= 0x80 (stick DOWN)
  INCREMENTS — the INVERTED-Y the user attested ("W = down")**; mult =
  1.5 outside [0.3, 0.7] for 0x31/0x34, * 1.8 for sub-weapon 4; clamp
  [0, 1.0] for stances 0x31/0x32, [0, 0.75] for 0x34/0x35. Both blends
  reset to 0.5 at stance entry; +0x302 = 1 flags manual steering.

**The 9-step ladder measured from the data** (hand-bone +X fire
direction at the settled tail of each baked clip, sub-weapon 0 row
0x112..0x11A): 0x112 center (pitch +1.3 deg), 0x113 full UP +81.3,
0x114 full DOWN -78.7, 0x115/0x116 level yaw -+60 (model -X / +X),
0x117/0x118 up/down at -X, 0x119/0x11A up/down at +X — a full 3x3
pitch x yaw pose grid. (Exporter: the ladder ids 275..282 are now part
of the documented player.emdl bake.)

**R2 = the second armed stance**: `func_001607D0` dispatches HELD R2
(spad 3B7E) -> player mode 0x1E, action code 0x32 — same ladder
family, R2-family steer rates, dot-only laser (s23 table), no
fire-counter recoil, camera mode-1 states 0x2A/0x29 with the
entry-saved target base. The user-observed "R2 precision view" IS
mode 1 under stance 0x1E.

### 4. DOOR CAMERA CUES — op 0x0D sub 5 + func_001BBBF0

> **2026-06-11 s56 CORRECTION:** the OPEN-transit reading below is
> wrong on both axes — the cut Euler comes from spad 3B50 = the
> kickoff's SNAPPED pose (the THROUGH-DOOR axis, never the live camera
> heading), EYE.y = player.y + 19 (11 + f4 + f5) and TARGET.y =
> player.y + 13 (11 + f4, default params; −46.8 areas +17), both
> live-verified. See "SPAWN-RECORD FIXED CAMERAS + DOOR-CUT GEOMETRY
> RE-DERIVED" (s56).

- **OPEN transit** (script D_0024DE40 record 2 = op 0x0D sub 5,
  `func_001B7B30` -> `func_0018CBD0(cam, player, -20.0)`): desired
  TARGET = player pos with TARGET.y = player.y + 19 + f4 (f4/f5 = 6/2
  when the area param +0x64 == -46.8, else 2/6 — f4+f5 = 8 in both),
  desired EYE = player + rotEuler(cam+0x30)*(0, 0, -20) with EYE.y =
  player.y + 19 + f4 + f5 = +27 (too-close variants shave it); then
  solver style 5 AND style 1 — style 1 HARD-COPIES desired -> actual:
  **a cinematic CUT 20 u behind the player at +27**, and cam+0xA0 =
  0x78 makes the actual TARGET re-blend toward the desired one at
  <= 1.0 u/frame for 120 frames (func_001916C0's tail) while the
  player walks through. Sub 5's siblings: the same shape at dist
  -14 / cam+0x0C; sub 0 = scope-style zoom reset + view-dir reset.
- **LOCKED try** (record 2 of D_0024DEC0 = op 0x09 ->
  `func_001BBBF0(door)`): TARGET = door pos + (-8*cos(door yaw), +10,
  +8*sin(door yaw)) — **8 u to the door's LEFT = the HANDLE side, 10
  up**; EYE = TARGET - 13*(sin,cos)(camera yaw D_00810374) with EYE.y
  = door.y + 12 — the camera parked at the handle for the try
  animation; hard copies to D_008105E0/D0, returns advance. The finish
  script's op 0x07 sub 4 restores the saved camera (func_001CA770).

### 5. Port (extermination-port s53) + verification

All four shipped in em_game.c: the decoded idle-orient timer/orbit
(retires the 120-frame/0.4-rad-s port constants), mode-1 aim camera
(retires the +0x8C hack; runs inside camregions, instant region
snap-back kept), the manual steer + bilinear 3x3 ladder-pose blend
(player.emdl re-exported with clips 275..282; em_weapon's held 0x112
base is substituted at dispatch, so the hand-bone fire/laser ray
pitches automatically), R2-held stance 0x1E (camera + pose + steer;
em_weapon's dot-only laser noted as pending), and the door-transit
cinematic cut + 120-frame target re-blend (the locked-look math behind
EM_DOORCAM_LOCKED=1 as a flagged preview until em_door grows the
locked sequence — the unlocked west-door preview clips into the
doorway wall, so geometry verification waits for a real locked door).
Self-tests: ALL PASS including the new EM_AIM_TEST (inverted-Y, blend
clamps, full-down camera geometry, pose-pan-then-body-turn); door/
transit final-pos asserts updated for the parallel walk-out decode.
Default capture byte-identical (old vs re-exported player.emdl, cmp
exact). Captures: EM_CAPTURE_AIM=1 now frames down the barrel toward
the laser dot; =3/=4 show the up/down ladder poses with the
counter-moved eye; EM_CAPTURE_DOOR=1 shows the doorway cinematic.

## SCREEN-FADE BLEND + TRANSIT TWO-LOCK + ARRIVAL WALK-OUT DECODED (2026-06-11, session 54)

Closes three fidelity questions raised against the port: the fade's real
GS blend, why the menu opens "about halfway through" the arrival
walk-out while movement stays locked, and the walk-out itself.

### 1. The fade is a SUBTRACTIVE GS sprite, not a black alpha cover

The func_001AED80/DB0/DE0/E10 family arms the 0x0028A8E0
transition/brightness machine (per-frame writer func_001AEE70, jr-table
0x26DD50 — REAL table base; entries [0x1AEEB4, 0x1AEF10, 0x1AEF60,
0x1AEFB0, 0x1AF00C, 0x1AF00C, 0x1AF01C] for states 0..6). Header fields
(+0xC0 sub-state = D_0028A9A0, +0xC2 mode, +0xC3 state, +0xC4 level
0..255, +0xC6 step):

- func_001AED80(mode): level 0, state 0           (instant CLEAR)
- func_001AEDB0(mode): level 255, state 1, sub 2  (instant BLACK hold)
- func_001AEDE0(step, mode): state 3, sub 3       (FADE-OUT ramp)
- func_001AEE10(step, mode): state 2, sub 1       (FADE-IN ramp)
- fade-out completion: level=255, state 1, sub-state 2 — **D_0028A9A0
  == 2 is the "hold black" gate** func_001AE040 requires before running
  the commit executors (func_001AD010 room/area, func_001AD140);
  fade-in completion: level=0, state 0, sub-state 0.

The packet (init func_001AEA50, double-buffered 0x60/frame): VIF1
FLUSH/NOP/NOP/**DIRECT 5** → GIF tag NLOOP 1 EOP, PRE=1 PRIM **0x346**
(SPRITE, ABE on, FST, context 2), PACKED regs {A+D, RGBAQ, XYZF2,
XYZF2}:

- +0x20 A+D → **ALPHA_2** (reg 0x43):
  - mode 0: `0x00000080_000000A1` → A=Cd B=Cs C=FIX D=0, FIX=0x80 →
    **Cv = Cd − Cs** — saturating SUBTRACT of the source color;
  - mode 1: `0x00000080_00000068` → A=Cs B=0 C=FIX D=Cd → **Cv = Cs +
    Cd** — ADDITIVE (fade to white; used by the mode-1 callers).
- +0x30 RGBAQ: R=G=B = **the level** (rewritten per frame by
  func_001AEE70; A=0x80 unused since C=FIX).
- +0x40/+0x50 XYZF2: (0x7000,0x7900)-(0x9000,0x8700) 12.4 — fullscreen.

So the door fade subtracts the grey level from every pixel:
`out = max(0, pixel − level)` — shadows crush to black early, highlights
survive to the end ("exposure being pulled down", the user-observed
look). Port stand-in (standard alpha blend only): black quad with
**alpha = 1 − (1−l)²** — matches the engine's mean-luminance trajectory
for a uniform histogram and both endpoints; residual gap (no per-pixel
crush) documented in extermination-port em_frame.h; exact parity needs a
reverse-subtract blend op in the port overlay pass (gfx-owned).
**GAP CLOSED (2026-06-11 s55)**: the port overlay pass now carries the
exact op — `em_gfx_overlay_rect_sub` (Metal
MTLBlendOperationReverseSubtract, factors ONE/ONE on RGB, dst alpha
kept) draws a GREY fullscreen quad with r=g=b=level, per-pixel
identical to the GS sprite; the alpha stand-in and
`em_frame_fade_alpha()` are retired (D3D12/Vulkan skeletons carry a
no-op stub of the hook).

### 2. The transit "input lock" is TWO separate systems

**MENU**: gameplay state 1 polls func_001AE7E0 every frame; return 2
(via the C5/B0 request bytes OR internal-layout buttons 0x800 Triangle /
0x10 Start in D_00810E74, with D_008106B3 inhibit) opens the status
screen (func_001AE040 state 3, D_008106C4=1). It returns 0 — press
DROPPED — while: B8/B9 request pending, **fade machine D_0028A9A0 != 0**
(fade-out 3 / hold 2 / fade-in 1), or **scripted spad 0x70003B8D != 0**.
(Return 1 = the func_0022A650 screen on 0x100/SELECT or pad-mode byte
D_00810E50 != 4; return 3 = level-end via D_008106CE.)

**MOVEMENT**: the player actor state machine owns it. The room-move
commit chain: D_0028A9A0==2 → func_001AD010 (B8==2: entry byte ←B7,
task state 4) → state 4 runs **func_001AFCF0 (clears spads 0x3B8C/8D/8E/
93 — scripted mode ENDS here, under black)**, **func_001B07C0(1)**
(re-place from the spawn table; copies rec byte +0x14 → player +0x0E,
and +0x14 == 1 in every decoded record → **player state 5/1**),
func_001AEE10(4,0) fade-in, back to state 1.

### 3. The ARRIVAL WALK-OUT — player state 5/1 (func_00183250)

State-5 dispatcher func_0015B610 (head: 3B8D 0/4 → sub dispatch; 1-3 →
func_00182B30 arbitration — which returns 0 for 5/1, the abort path
that converts a walk-out to state 4/0 action 0x41 if scripted mode is
still active). Sub 1 = func_00183250, phases on +0x06:

- phase 0 (1 frame): locIdx +0x25C = 2, ramp +0x38 = 0.3 (D_00248870[2]
  u/tick), anim = func_0017B490(mode 1, family +0x235, locIdx 2) →
  D_00248AB0[1][family*4+2] (family 0 unarmed → id 2 = RUN; armed
  families → the 0x4B-0x4E scripted-walk row), blend 1.0
  (func_001749A0); timer +0x28 = 0x32.
- phase 1 (50 frames): clip plays, **no mover** (hidden under the
  64-frame fade-in).
- phase 2 (30 frames): mover func_00178B90 at the 0.3 u/tick ramp.
- phase 3 (30 frames): mover + ramp −= 0.0113636/frame (0x3C3A2E8C;
  hits 0 at ~26 → base-idle request, blend 12.0).
- exit: state 1/0/0, +0x1F0 = 0, 3B8D re-cleared. Every frame’s tail:
  pos.y −= 0.2 + func_00175900 floor settle.

Total ~111 frames, ~13.1 u of travel, stick never read. So on arrival:
**menu unlocks at fade-in completion (~64 f ≈ halfway), movement at
walk-out end (~111 f)** — two locks, exactly as observed in play.
(Other 3B8D writers, for the record: use-scan func_00184BA0 = 3,
func_001B0C60 = 3 + area kick, op16 func_001B6E40 = 3, op07
func_001B82D0 subs 0/1/D = 2 (D: 1) / sub 4 = 0, walk handlers
func_00183250/3F0/440 = 0 at their exits, func_001AFCF0 = 0.)

### Port (extermination-port, this session)

em_door_input_locked() split into em_door_movement_locked() (kickoff →
walk-out end) + em_door_menu_locked() (kickoff → fade-in end) +
em_door_walkout_active() (the 5/1 phases, surviving the goto scene
switch); em_hud gates Triangle/Start opens on the menu lock; the fade
rect draws em_frame_fade_alpha() = 1−(1−l)². Tests: EM_DOOR_TEST
(frame-290 two-lock witness; final pos 38.889 = spawn − 13.111 travel),
EM_TRANSIT_TEST (drawbridge arrival 25.889), EM_PAUSE_TEST door leg
(mid-fade press dropped / menu opens mid-walk-out / open menu freezes
the world / completion after resume) — all PASS; default capture
byte-identical.

Also this session (port-side fidelity, decode-adjacent):
- GAIT HOLD TIERS from the s31 quantizer table: Cmd = walk band 0.8,
  Option = the gait-1 TURN/creep ring 0.5 — gait 1 is the engine's
  slowest movement tier (zero translation; D_00248870 has no slower
  translating band, so "slowest" maps to turn-in-place, documented).
  *(s56 correction: gait 1 SUSTAINS the id-1 WALK at 0.1 u/tick — the
  zero-translation read was the entry transient; Cmd = JOG id 2,
  Option = WALK id 1, full = RUN id 3. See "LOCOMOTION TIER RAMP".)*
- WOODEN CRATE (s34 n0 entry 0x0D) promoted to the GLOBAL port default
  (user-confirmed fidelity). Honest table note: the cardboard model is
  the n1 (office sub-1) table's, but NO sub-1 placement record spawns a
  crate — no shipped scene genuinely binds cardboard.


## LOCOMOTION TIER RAMP — full stick = anim id 3, the s31/s54 gait labels were one tier low (2026-06-11, session 56)

USER PCSX2 REPORT (the oracle): "the player still WALKS by default;
should RUN — Cmd should give the anim the port played by default,
Option the one under that." Correct on all counts. Re-decode of the
gait chain found the missed mechanism; every prior tier label was one
step too slow.

### What s31 missed: the tier is RAMPED, not gait-1

The s31 chain (quantizer 0..3 -> +0x23F -> locIdx +0x25C = gait-1 ->
anim id row[locIdx]) read only the locomotion ENTRY. The full machine:

1. **func_00174AC0** is not a "max check": it switches on +0x23F and
   writes a TARGET SPEED `+0x240` = {0, 0.1, 0.3, **0.8**} u/tick for
   gaits 0/1/2/3 (immediates 0x3DCCCCCD/0x3E99999A/0x3F4CCCCD). Full
   stick targets 0.8 — the "sprint" speed.
2. **func_001612D0 state 2** (entry only): locIdx = gait-1, speed
   +0x38 = D_00248870[locIdx], ONE anim request, then state-- to 1 —
   the per-frame re-request the s31 read assumed does NOT happen.
3. **func_0017BC40 phase 1** (the ramp, jtbl_0026D720): sub 1
   accelerates +0x38 by `D_00248880[locIdx]` per frame and on crossing
   `D_00248874[locIdx]` (= D_00248870[locIdx+1]) does **+0x25C += 1**
   — a TIER PROMOTION; sub 2 mirrors it down via D_00248890 /
   D_0024886C (= row -1). Sub 0 compares +0x38 to the +0x240 target:
   the tier settles where `D_00248870[locIdx] == target`.
4. **anim_matrix_player** (from func_0017C030 phase 1) re-requests the
   anim every ramp frame: sub 0 -> row id[locIdx]; sub 1 (accel) ->
   id[locIdx+1] (blends TOWARD the next tier's clip); sub 2 ->
   id[locIdx-1]. At-tier locIdx==2 sets the anim rate +0x204 = 0.75
   (0x3F400000) — see the cross-check below.

So the SUSTAINED tier == the gait itself; gait-1 is only the entry:

| gait (ring) | sustained locIdx | anim id | speed u/tick (u/s) | D_00248C90 steps |
|---|---|---|---|---|
| 1 (48<r<=88) | 1 | **1 WALK** | 0.1 (6) | 72/21 |
| 2 (88<r<=122) | 2 | **2 JOG** | 0.3 (18) | 26/3 |
| 3 (full) | 3 | **3 RUN** | 0.8 (48) | 21/2 |

Tier 0 (id 0, speed 0 — turn-in-place) is the gait-1 entry transient
(~2 frames), not a sustained band. ELF dumps (rebuilt .data):
D_00248870 = {0, 0.1, 0.3, 0.8}; accel D_00248880 = {0.05, 0.05,
0.0625}; decel D_00248890 = {—, 0.05, 0.025, 0.022727}; full mode-1
row table D_00248AB0[1] families: 0 unarmed {0,1,2,3}, 1 {A,B,C,D},
2 {4B,4C,4D,4E}, 3 {55,4C,4D,4E}, +0x10 {14,15,16,17}; mode-0 idle row
{0,A,4B,55,14}; mode-6 (stop) family-0 row {0,0,4,5} — ids 4/5 = the
jog/run STOP-SKID anims (func_0017C030 phase 3 requests mode 6 at
locIdx 3; untranslated in the port, flagged). Family byte +0x235 is a
bitfield (bit0 armed — func_001B07C0 copies D_00810706; bit1 aim/
special — func_001756E0), so default unarmed = row 0.

Stick release: target 0 -> tiers <=2 stop INSTANTLY (phase 3, +0x38=0);
tier 3 first runs DOWN (phase 2, 0x17BECC): 0.03125 u/tick/frame decay
(x2 with carried gear +0x314 & 0x1F) to the tier-2 speed, then the
stop — the run carries ~9 u. Pivot turns (heading delta > 3/4 pi in
func_00174AC0) set phase 7 subs 3/4.

### Clip evidence (fixed-directory bakes, 2026-06-11)

| id | frames | root travel | natural u/s | feet swing | arm swing |
|---|---|---|---|---|---|
| 0 | 80 | 0 | 0 (breathing idle) | — | — |
| 1 | 120 | 12.13 u | 6.11 | 7.3 u | 3.3 u |
| 2 | 45 | 17.65 u | 24.07 | 8.4 u | 4.9 u |
| 3 | 40 | 30.92 u | 47.57 | 10.1 u | 6.8 u |

Cross-checks that pin the mapping: tier 2 drives clip 2 at 18/24.07 =
0.748 — exactly the engine's hard-coded at-tier rate 0.75; tier 3
drives clip 3 at 48/47.57 = 1.009 ~ 1.0. Clip 3 is the full arm-pump
15.5-u-stride RUN — the PCSX2 full-stick anim. **CURIOSITIES entry 2
("the unreachable sprint") is OVERTURNED**: the quantizer's clamp at
gait 3 caps the TARGET, but the tier ramp reaches locIdx 3 in normal
play — id 3 is the ordinary full-stick run, reached ~14 frames after
a standing start (0.3 entry + 8 accel frames).

(The s29/s30 live footstep data re-reads consistently: the 72/21
metering was a partial-stick WALK moment; the 0x15../0x1A.. surface
bands were tiers 2/3 — the sub-base key a1 IS +0x25C, unchanged.)

### Port (extermination-port, this session)

em_game.c: tier ramp implemented (entry gait-1 + accel/decel/promotion
+ tier-3 run-down; loco_tier/loco_upt = +0x25C/+0x38); clips
walk/jog/run = ids 1/2/3 (all already in player.emdl — no re-export
needed), stride-locked at 6/18/48 u/s; footstep frames per id (72/21,
26/3, 21/2) and sub-base a1 = tier; walk-out keeps 0.3 = tier 2 (now
honestly the JOG clip). em_input: Cmd 0.8 = JOG band, Option 0.5 =
WALK band (the "TURN tier" labels retired). EM_MOVE_TEST expectations
honestly recomputed (83.4, -174.5 wall stop; ramp 4.65 u + 0.8/f).
All self-tests PASS; office + drawbridge default captures
byte-identical (idle paths untouched).

## SLIDER DOOR BRAIN func_001BB860 DECODED — walk-into trigger, no fade, no player anim (2026-06-11, session 56)

USER PCSX2 REPORT: "sliding doors — the player just walks through; no
door-open animation plays." Decoded: that IS the variant brain's whole
design. Closes the s32 flag "the variant lifecycle differs in
sequencing — unread".

### The brain (per-frame, +0x05 state machine)

- **state 0 (armed)**: branch on placement flags2 +0x3 — {0x16, 0x17,
  0x3E} are LOCK-GATED: test `D_00810841[area] & (1 << door_id+0x34)`;
  bit set -> trigger sub with a2=0 (OPEN, state->2), clear -> a2=1
  (LOCKED try, state->1). All other flags2 (office 0x83, plain
  sliders) -> always a2=0 OPEN.
- **state 1** (locked-try pump func_001BB7C0 done): clears the use
  mailbox +0xB, re-arms state 0. **state 2** (open pump done) ->
  state 3 = func_001BC150 — the SAME transition COMMIT as m03 (dest
  table D_0024E140; the slider doors with id bit7 are area changes,
  plain ids are room moves). **state 4**: func_001BB7F0 wait ->
  re-arm state 0.
- Tail (every frame): dist(player) <= 20 sets the near flag +0x1, and
  class bit7 registers on the interactive list (func_001B1DE0) — the
  same use-scan plumbing as m03, so the TRIGGER is the standard +0xB
  bit-2 use-arm: **the player WALKING INTO the door** (state-0x2D
  push-into scan, FINDINGS "Door contract" — no button).
  *(s58 CORRECTION: the shared use-scan plumbing is right, the trigger
  conclusion inherited the s17 error — the use scan runs only on a
  CROSS press edge, so sliders too arm on CROSS, not walk-into. See
  "DOOR TRIGGER IS THE CROSS PRESS EDGE".)*

### The trigger sub func_001BB560 (the player's role)

Gate `+0xB & 4`, then:
1. SIDE: bearing(door->player) vs door yaw +0xC4 — within pi/2 =
   front, +0x2E = 0 (flags2 8/0x16 INVERT the latch — single-leaf
   variants mount reversed; flags2 8/0x16 are also func_001BB400's
   single-leaf motion branch).
2. PLAYER YAW SNAP: +0xC4 of the player block D_008102B0 = door yaw
   (+ pi on the front side) — pointing THROUGH the doorway.
3. PLAYER SNAP: staging = door_pos − **6.0** · (sin, cos)(new yaw)
   (y = player y) via **func_00182F90 = instant translate** (the m03
   kickoff's snap helper; 6.0 here vs the m03 family's 5.0).
4. Queue the OPEN script D_0024D900 (sound rec patched by
   func_001BBD60(self, D_0024D980)) or the LOCKED D_0024DA40 (camera +
   VO only — no motion, sliders have no jiggle clip). Area-0x16
   special case: D_0024E140-driven flag writes to D_008106C8 bit 0x80.

### The OPEN script = the observed behavior

op07 sub0 scripted-mode enter (input lock, **NO fade** — the locked
script's op07 sub2 is the fading one) -> op0D sub5 chase-camera cue ->
op17 sub0 ONE positional door sound -> op09 func_001BB400 native slide
(panels' keyed local translation 0.2 u/frame until ±9.0 — 45 ticks;
flags2 0x3D/0x3E use 13.0, 0x08/0x16 single leaf) -> **op01 sub8 =
scripted player WALK-THROUGH** (func_001B94F0 move-to family, walk
clip) -> STOP. There is NO player door-gesture anim anywhere and no
fade: walk at the door -> it parts -> you walk through. Exactly the
user's PCSX2 description.

### Port (extermination-port, this session)

em_door.c "SLIDER (m17/m09) VARIANT BRAIN": sliders (filename
door_m09/door_m17) leave the m03 machine — walk-into arming (stick
past the dead ring + the class-5 window; CROSS stays only on the m03
family), 6.0-u staging (walked via the established MOVE-TO deviation,
not snapped), slide clip pump with one doorsfx play, scripted
walk-through to the mirrored far point, then: goto sliders run the m03
fade+scene-switch commit (native interleave unread, flagged); plain
sliders end UNLOCKED on the far side, stay parted, and re-close
(reverse clip, the s32 flag) when the player leaves the radius + 2-u
hysteresis. Not ported (flagged): the D_00810841 lock gate + LOCKED
script, the op0D chase-camera cue, the single-leaf +0x2E inversion
(no flags2 in the manifest). EM_SLIDER_TEST=1 (scene_drawbridge,
door 4 m09 at (128.6, 0, -610)) asserts the whole sequence: no-button
arming, mid-slide park at (122.6, -610) with anim_active 0, fade
pinned 0, far-side landing (134.6, -610) unlocked, re-close — PASS.
Captures: closed approach / panels parted with the player standing /
player mid-walk-through.

*(s58 CORRECTION: the walk-into slider arming above is retired — the
use scan only runs on a CROSS press edge, for sliders exactly like the
m03 family; em_door.c now arms both on CROSS and EM_SLIDER_TEST
asserts push-without-button stays CLOSED. See "DOOR TRIGGER IS THE
CROSS PRESS EDGE".)*

_Last updated: 2026-06-11 (session 56)._

## WEAPON-VISUAL FIDELITY WAVE — true reload clip, semi fire interval, laser hide window, light-cone mesh, flash rotation (2026-06-11, session 53)

Six user-reported weapon defects run to ground: four were missing
ENGINE decodes (now pinned from the .s + boot-ELF tables and ported),
one an asset-identity error, one a port-side render placement bug.
Player struct base note used throughout: the live globals D_008104xx /
D_008105xx are the player struct at `0x008102B0` (gun ptr D_008102D0 =
player +0x20; D_008105A2 = +0x2F2, D_008105C8 = +0x318).

### 1. THE TRUE RELOAD CLIP IS 0x11B (283), NOT 0x33 — the "stagger" solved

The user's persistent "reload plays a stagger" (surviving the s45
directory fix) was an ID-IDENTITY error, the same code-vs-clip trap
s25 sprang for the fire codes: **0x33 is the +0x1F0 ACTION CODE; the
reload ANIM is a per-sub-weapon table pick.** The armed top's major
state 3 (`func_0016FCF0` -> `.L00170000`) runs `func_0016F600`, whose
reload entry requests TWO clips through func_001749A0:

```
stances 0x1D/0x1E: D_00248B88[sub] (blend 0)   then D_00248B98[sub] (blend 1)
stances 0x1F/0x20: D_00248C68[sub] (blend 0)   then D_00248C78[sub] (blend 1)
+ sound 0x163 (the s29-pinned reload-start foley — confirms the site)
```

ELF table reads (file_off = vram - 0x100000 + 0x94):

```
D_00248B88 stance-A entry:  274 284 284 295 305 315   (274 = 0x112 ✓)
D_00248B98 stance-A RELOAD: 283 293 294 304 314 324   <- sub 0 = 0x11B
D_00248C68 stance-B entry:  394 404 404 415 425 435   (394 = 0x18A ✓)
D_00248C78 stance-B RELOAD: 403 413 414 424 434 444
```

So each sub-weapon's clip family is entry/ladder 0x112..0x11A + reload
0x11B (283), then the next sub starts at 0x11C — the reload fills the
gap s25 left unexplained. Property row 0x11B: mode 1, rate 1.0.

Motion audits (fixed-resolver bakes, node 4 = gun mount, 19/20 hands):

- **clip 283 (0x11B), 60 fr** = the reload: root planted, gun STAYS
  SHOULDERED (mount y 14.2..14.4 the whole clip), the support hand
  leaves the grip for the mag work (inter-hand 2.5 -> 6.9 -> 2.1) — a
  shouldered tactical reload. Port capture confirms the visual.
- **clip 51 (0x33), 57 fr** = a KNOCKDOWN: the body folds toward the
  ground (root pose collapses, arm flung out) — the literal "stagger"
  the port played. CORRECTS the s47 motion-verdict row for id 51
  ("RELOAD, not a stagger... inter-hand 0.6-1.1" — that audit's
  numbers do not reproduce; the same bake measures inter-hand
  3.2->11.9 and a fold-to-horizontal root). Clip 51 stays exported
  (a real knockdown clip, future damage-reaction user).
- Runtime trace (EM_WEAPON_TRACE=1) had FIRST verified the port's
  request/commit chain was healthy (0x33=51 committed and played
  frames 1..56 every reload) — the bug was purely which container.

Port: WPN_ANIM_RELOAD = 0x11B; player.emdl re-exported with 283 in
--clips (canonical CLI updated in export_native.py's header). The
draw/reload requests are now HOLD-type (clamp the last frame until the
aim hold replaces them): the old play-once release left ~2 frames of
locomotion idle between clip end and the aim recommit — a full-pose
snap at every reload end (the stagger's second face).

### 2. SEMI FIRE INTERVAL — +0x2F4 = the ladder clip LENGTH (func_0017A8B0)

The "still no fire rate" report: the port ran every family at the flat
12.0 interval. Decode: **`func_0017A8B0`** (called by the action
machine func_001607D0 on EVERY fire-button press, and by the
sub-weapon action func_0017A970) sets `+0x274 = 1` (trigger latch) and
**`+0x2F4 = (float)func_001C61D0(lib, ladder_clip)`** — the FRAME
COUNT of the stance's aim-ladder base clip (func_001C61D0 returns the
container header halfword +2; confirmed in matched C). For the SPR4
ladder 0x112 (25 fr): counter +2/tick vs int(25) -> expiry 12 ticks
after the shot tick, chained shot the 13th — **semi = one round per 13
frames (~4.6/s), exactly the duration of the 12.5-tick recoil
replay.** The burst/auto FIRE states overwrite +0x2F4 = 12.0 per round
(the func_00170A60 0x15/0x1E stores already decoded in s47) — the
6-frame cadence is IN-BURST only. The s47 reading "+0x2F4 (12.0
default)" was the burst/auto store, not the semi default.

Consequence for the +0x2A queue window (s47): sampling from
`counter >= int(+0x2F4) - 8` = counter >= 17 = tick 8+ — presses in
the FIRST ~7 ticks of the semi cadence are DROPPED, not queued.
Port: per-press interval refresh from the honest 0x112 clip length
(fallback 25), burst/auto rounds store 12; mash can no longer beat 13
ticks (unit test 1a: 61-frame 2-frame mash = 5 shots at exact 13s;
plus an event-API mash leg through the real em_input pad model).

### 3. LASER HIDE WINDOW — player +0x2F2 (mirror D_008105A2) decoded

The user correction "the laser is SUPPOSED to disappear while firing"
is engine truth, and the gate is the THIRD term of the s23 gun-tick
selection (func_00188630): code in {0x31,0x34} AND phase +0x1F1 == 1
AND **+0x2F2 != 0**. The fire SM (func_00170A60) drives +0x2F2:

```
WAIT tick head (.L00170C08):       +0x2F2 = 1   (every tick)
every SHOT state (semi 0xA .L00170D14, burst 0x15, auto 0x1E):
                                   +0x2F2 = 0
cadence EXPIRY (burst .L00170F50 / auto .L00171158 — unconditional;
semi only on the QUEUED-shot path .L00170E4C):
                                   +0x2F2 = 1
stance-A draw entry (func_0016FCF0 +0x110 request site) also clears it.
```

So the laser hides from each shot tick until that shot's cadence
expiry; under sustained fire it BLINKS for exactly the expiry tick of
each chained round. (Also decoded in passing: the WAIT head re-copy
of the hand matrix to +0x2A0 gates on +0x2F2.) Port: w.laser_vis
mirrors the flag tick-for-tick; laser draws only in AIM with it set;
unit-tested (hidden through the cadence, expiry blink, WAIT visible).

### 4. LIGHT-CONE MESH FAMILY — chunk27 0x10/0x11/0x16 (+ flashlight geometry)

Survey of the global chunk27 library (s7b geometry-stats + TEX0-key
method) for the reference capture's visible beam: entries **0x10,
0x11, 0x16** are tessellated LIGHT-CONE shells — apex at the local
origin opening along +Z to radius 25.0 at z = 200 (half-angle
atan(25/200) = **7.13 deg**), 4 rings + apex fan, 224 verts; **0x12/
0x13** are the cheap 2-ring cross variants (32 v), **0x17/0x18** the
z=200 end-cap discs. All sample the additive glow sheet
0x041695113222E9 (the s7b player-aura texture) with a PLANAR XY
projection across the aperture: uv = 0.5 + (x,y)/25 * 0.219 — the
faint interior (lum 14-15/255), flat across the shell. Exported:
`export_props.py --cone` (new mode) -> assets/fx/light_cone.emdl
(entry 0x10, static 1-node EMDL + the glow texels from frame1.gs).

Port flashlight rework (all four user notes):
- cone angle ASSET-DERIVED: tan(theta) = 25/200 = 0.125 -> the spot
  disc at the ~30-u aim wall distance is 7.5 u = 2.5x the 3-u laser
  dot (the reference "2-3x dot"; the old ~12-deg cone read ~2x big).
  Inner cos 6.13 deg, outer cos 7.13 deg (1-deg rim inside the asset
  angle — the sharp disc).
- origin = the barrel TIP (gun+0xB0, the muzzle-flash anchor), not
  the in-receiver ray origin.
- spot term LEVEL-ONLY: the Metal shader adds it on the baked-vertex-
  color path alone; the directional character path takes none — the
  player/gun never catch their own light.
- the VISIBLE BEAM: the exported cone mesh drawn additively from the
  tip along the aim ray through a new beam-pass triangle queue
  (em_gfx_beam_tri_tex; depth test on / write off, cull none).
  Intensity gain 3.0 is PORT-TUNED (the engine stacks shells
  0x10/0x11/0x16; flagged).

### 5. MUZZLE-FLASH ROTATION LERP translated (s43 open item closed)

func_001F5040 `.L001F53A8` (tick >= 4, all variants): one scalar
`rot += (-128.0 - rot) * 0.35` per tick written to ALL THREE rotation
components (+0x80/84/88; variant 0 inits 0, variants 3/4 init -96.0 —
DEGREES). The dying star rolls ~45 deg in its first lerp tick and
settles toward -128. Port: em_gfx_beam_tex_roll rotates the axial
star quad's width vector around the gun axis (Rodrigues) by it — the
dominant visible component of the uniform Euler triple (flagged
interpretation). The 0.8^t INTENSITY decay stand-in is retired: the
engine writes NO color fade (the 0.8 decay is the scale velocity's);
the textured flash now draws at constant intensity and dies by the
model swap + scale spread + roll, vanishing at tick 15. Honest
remaining delta vs the original: the port's star is ONE axial quad
(the engine model is a multi-fin rosette), the func_001F4F90
line-burst spark pass (2.4-u point pairs) and tracer func_001860A0
stay untranslated, and the abrupt 16-tick end is engine truth.

### 6. Laser-dot clipping (port placement bug)

The endpoint dot billboard drew centered ON the wall plane — half of
it always failed the depth test. Port: the dot offsets HALF ITS SIZE
along the world hit normal (EmCollHit.normal; enemy hits back along
the ray) — engine sprites sit on the surface plane plus an offset.

### Port verification (extermination-port, this session)

All 16 self-tests PASS (input/weapon-unit incl. new mash+laser-window
+event-API legs / move / door / transit / weapon-scripted (schedule
re-anchored to the 13-tick cadence) / pause / aim / melee / sfx /
camregion / enemy 1-5); default capture byte-identical to a pristine
HEAD worktree build (73d393a3…) with the re-exported player.emdl.
Captures: EM_CAPTURE_AIM=1 laser+dot (offset off the wall);
EM_CAPTURE_AIM=2 mid-recoil frame shows the laser GONE with the flash
live; EM_CAPTURE_LIGHT=1 the sharp 7.13-deg disc + the visible cone
mesh from the muzzle, player/gun unlit; EM_WEAPON_TEST mid-reload
frame = the shouldered 0x11B reload (support hand at the pouch).

### Open items (s53)

- The semi interval for the OTHER sub-weapons (ladder lengths of subs
  1-5) — port carries sub 0 only.
- Stance-B (0x1E/0x20) reload clips 403+ and its dot-only laser.
- The engine's cone-mesh DRAW SITE (which behavior binds 0x10/0x11/
  0x16 — searchlights? the shoulder light in cutscenes?) — the port
  uses it for the flashlight deviation regardless.
- The +0x2A0 hand-matrix copy consumers beyond the laser (the WAIT
  head copy gated on +0x2F2).

## SPAWN-RECORD FIXED CAMERAS + DOOR-CUT GEOMETRY RE-DERIVED (2026-06-11, session 56)

Driven by three PCSX2 ground-truth reports against the port. Static
re-reads of `func_0018CBD0`/`func_001B7B30`/`func_001B0460`/
`func_001B07C0` plus a LIVE session (DebugServer watch-trap on the
camera pool) through the office double doors, both directions.

### 1. CORRECTION — op 0x0D sub 5 door cut (revises s53 "DOOR CAMERA CUES")

The s53 reading was wrong on both axes. Live-verified twice (cut
trapped via a cam+0xA0 change-watch, values read at the pause):

- The cue fires AFTER the kickoff snapped the player to the STAGING
  POINT with the THROUGH-DOOR yaw — and the kickoff path refreshes the
  spad pose snapshot `3B40/3B50` (writer: `func_001B07C0`'s
  player+0xB0/+0xC0 copy) with that snapped pose. `func_0018CBD0`
  builds its Euler from spad 3B50, so the cut direction is the DOOR
  AXIS — never the live camera heading.
- Heights: desired EYE = staging − 20·(sin,cos)(through yaw) with
  EYE.y = player.y + 11 + f4 + f5 = **+19** (both parameter sets;
  live: 19.0 exactly); desired TARGET = staging with TARGET.y =
  player.y + 11 + f4 = **+13** (param-default f4 = 2; the −46.8 areas
  use f4 = 6 → +17; live: 13.0 exactly). The extra `0.3·f20` term in
  the .s is the steep-pitch shave path — f20 is the CALLER's
  callee-saved value when the shave branch is skipped (live: 0).
  Both desired vectors hard-copy to the actual camera (solver style 5
  then 1), cam+0xA0 = 0x78.
- `func_001B7B30` sub map (jtbl_0026DF70): sub 0 fade gate, sub 1 →
  `func_001B0460(1)` (camera re-init from the entry record, below),
  subs 2/3 → `func_0018CBD0(cam, player, cam+0x0C)` (the PER-RECORD
  distance), sub 4 → dist −14, sub 5 → dist −20, sub 6 restore +
  yaw window, sub 7 compare, sub 8 → D_008101E4 = 1.
- ROOM-BOUNDARY RE-SEAT (live): when the walk-through crosses the
  doorway plane (the room move), the chase re-seats behind the
  player's through-door pose and the NORMAL solve runs — the door
  wall right behind the eye RISES it (live: parked at
  (104, 29, −250.4) over the 21-u doorframe, desired == actual).

### 2. NEW — SPAWN-RECORD FIXED CAMERAS (the supply-room corner camera)

The user-observed "director camera in the supply-room corner" that the
s50 director decode could not find. It is a SECOND fixed-camera
mechanism, keyed on room-ENTRY spawn records, not trigger volumes:

- Spawn records (`D_0024D650[area]` → per-room record tables, 0x30
  stride — the same records the re-place reads; area-2 room tables
  0x24B560/0x24B6B0/0x24B800, 7 records each) carry a CAMERA-INIT
  word at **+0x10**:
  - bit 7 of the low byte = FIXED-camera flag → cam+0x05 = 1
  - low 7 bits = camera mode byte → cam+0x06
  - word >> 8 = index into **`D_0024A8D0`**, a packed vec3 fixed-EYE
    table (stride 12)
  (+0x18 = the per-record camera distance → cam+0x0C and the fog/param
  copy D_00810244 — the source of the s53 "−31.2 areas" mystery value;
  +0x14 = the s54 walk-out byte.)
- **`func_001B0460(mode)`** = the camera re-init that consumes it,
  called on EVERY room entry: by the transition machinery/re-place
  (`func_001B07C0` tail), by op 0x0D sub 1, and by the scripted
  player states (`func_00157360`/`func_0016D130`/`func_0016DE40`,
  which set D_00810702 = entry idx first). Flagged record: desired
  EYE = D_0024A8D0[idx] hard-copied to desired AND actual (with the
  desired TARGET = player + 15, the chase target height), commit
  `func_0018C0D0(cam, 1)` — the camera SNAPS to the room spec at
  entry and stays pinned while cam+0x05 = 1 (L1/auto-orient dead, R1
  aim still runs — the same user-observed semantics as the director
  regions). Unflagged record: normal chase re-seat with mode byte →
  cam+0x06.
- **THE SUPPLY ROOM**: area 2 room 1 entry 3 (record 0x24B760: pos
  (104, 0, −259) yaw pi — the south side of the office double doors)
  has +0x10 = 0x380 → flag set, mode 0, eye idx 3 →
  **D_0024A8D0[3] = (116, 33, −300)** — live-verified: entering the
  supply room pins the camera there (desired == actual, eye static
  while the player walks the room); walking back through the doors
  re-inits from entry 2 (+0x10 = 0) → normal chase. D_00810702 is
  the ENTRY INDEX (set to 3/2 by the room move), not a room id.
- s50's "AREA02 has NO fixed cameras" stands for the DIRECTOR
  (func_00195130) only; the area verdict is superseded by this
  mechanism.

### 3. Menu-player lighting verdict (the s49 "black on black" report)

The status-menu turntable player WAS drawn by the port but at the
shader stand-in's 0.30 ambient floor — invisible on the black UI
backplate under the tile layers. Engine truth: the menu actor's draw
class 0xB (`func_001CA5F0` → `func_001CB480`) sets lighting-override
mode 2, and `func_001D89D0` special-cases only modes 1/3/4/5/6 — mode
2 runs the NORMAL character light path: the CURRENT room's rig from
D_00251C50 (office key 0x200: ambient (57,57,57)/128 ≈ 0.445 + two
directional lights (60,60,60) and (37,37,37)/128) with light slot 0
ZEROED (the static menu actor never gets the camera-light flag +0x2
bit 0x20 — func_001AFF10 zeroes the byte, func_0020CDC0 never sets
it). Port fix (em_game ui_scene_render): the existing forward spot
term doubles as a camera-anchored fill light scoped to the menu player
draw (cone edges < −1 = no cone falloff; the spot's N·(−L) IS a
camera-facing wrap), putting camera-facing normals at ~1.0 — the
engine-comparable readable turntable.

### 4. Exporter + port

- `tools/export_level.py --camregions` now also emits SPAWN-RECORD
  cameras (`CAMSPAWN_*`, sub-state aware via `--sub`; the captured
  office scene = room/sub 1, office0 = 0): flagged entries become
  scene.txt `camregion` lines with the rect spanning the room behind
  the entry's doorway plane (derived from the entry-record PAIR;
  behavior-identical to the entry-keyed pin for an enclosed room) —
  the office scene gains `camregion 64 -332 144 -252 0 116 33 -300`.
- Port (extermination-port): door cut corrected (staging + through-door
  axis + +19/+13, latched from the transit walk-to), the doorway-plane
  re-seat added (doorcam 3 → normal dispatch+solve = the engine's
  post-move rise), the menu-player fill light, EM_CAPTURE_SUPPLY
  capture knob. All self-tests PASS; default capture byte-identical
  vs the HEAD build.

_Last updated: 2026-06-11 (session 56)._

## LOCKED-DOOR SEQUENCE PORTED — try-anim verdict, lock census, the locked "VO" is a TEXT-ONLY radio message (2026-06-11, session 57)

Closes the s53 flag "EM_DOORCAM_LOCKED stays a preview until em_door
grows the locked sequence" and the s56 "D_00810841 lock gate not
ported" flag. Static decode (.s + local ELF/overlay data) + port
implementation; no emulator.

### 1. Anim verdict: 0x46/0x44 ARE CLIPS (directory ids), not codes

The code-vs-clip trap that bit fire (s25) and reload (s53) does NOT
apply to the locked script's op-0x0A ids. Directory-resolved bakes of
ids 70/68 (engine 0x46/0x44), 21-node player rig:

| dir id | frames | motion |
|--------|--------|--------|
| 68 (0x44 back)  | 200 | body stays (mean travel <= 0.8 u); limbs reach/jiggle, peak node dev 6.57 u @ f64, EXACT return to rest (f199 dev 0.00) |
| 70 (0x46 front) | 200 | same shape, peak 5.44 u @ f68, exact return to rest |
| 67/69 (opens)   | 150 | contrast: whole-body travel 25.2/14.5 u — walk-throughs |

Try-the-handle-and-fail gestures peaking exactly at the script's
60-frame rattle mark — the s45 table re-verified, safe to wire by id.
`player.emdl` re-exported with `,70,68` appended (canonical CLI in
export_native.py updated); byte-superset verified (verts/indices/
textures/old clip table + palette prefix identical).

### 2. func_001BBAE0 DECODED — the locked "VO" is the RADIO MESSAGE machine, text-only

Full .s read + ELF data (jtbl_0026E1A0 @0x26E1A0, D_00264DD0
@0x264DD0):

- Phase 0: music duck `D_002821B0 = 2` (machine MODE 2, func_001FCA10
  dispatch), `D_002821B4 = 1` (active), then `sel = link byte +0x56 &
  0x3F`; sel >= 6 -> done (no VO). jtbl_0026E1A0[sel] -> LINE WORD
  `0x8000000X` stored to `D_002821B8`, `D_002821BC = 0` (the mode-2
  pre-delay), blk+4 = 1. Mapping: sel 0->line 6, 1->0, 2->2, 3->8,
  4->0xA, 5->4.
- Phase 1: poll `D_002821B4 == 2` (set by func_001FCA10 mode 2 when
  func_001FDB80(0) reports the message done).
- The line word's **bit 31 selects the GLOBAL message table**
  `D_00264DD0[0]` = 0x272DF0 (per-AREA tables live at D_00264DD0
  [area+1] — func_001FD790 reads `lw 0x4(D_00264DD0 + area*4)`).
  8-byte line records `{u16 steps/duration, s16 voice_cue, u8 0xFF,
  u8 +4, u8 +5 wait_stream}`; a message = record pairs `{dur, -1, 0}`
  then `{0, .., +5=1}` (the +5 flag makes func_001FDB80 wait on the
  VOICE stream flags D_00282155/156 before completing).
- **Every locked-door line record's voice_cue is -1** (global recs
  0/2/4/8/A = {0x94=148f, -1}, rec 6 = {0x76=118f, -1}): the locked
  "VO" plays NO audio — it is a TYPEWRITER RADIO SUBTITLE. Text =
  bank slot 0x16 (`D_0028A4E8`, the bit-31 container in
  func_001FD950) string[line] via func_001FE480. A real voice cue
  (area-table path) would start via func_001FD580's
  `func_001FA5A0(rec+0x2)` — the VOICE.DAT stream.
- s23's "locked-door VOICE OVER" label is hereby corrected to
  "locked-door RADIO MESSAGE (text-only)".

### 3. Lock census of the exported scenes — two REAL locked doors

Gate recap: hinged func_001BC350 model 0x15 tests `D_00810841[area] &
(1 << door_id)`; slider func_001BB860 only for flags2 in {0x16, 0x17,
0x3E}. D_00810841 is BSS -> all lock-gated doors START LOCKED.
Placement dump (this session):

- AREA02 sub 0 [22]: m15, flags2 0x80 (id 0), link 0x0200 ->
  scene_office0's drawbridge door — LOCKED, VO sel 0 -> line 6 (118 f).
- AREA01 tblA [14]: m15, flags2 0x81 (id 1), link 0x0200 ->
  scene_drawbridge's office0 door — LOCKED, VO sel 0 -> line 6.
- All exported sliders (office m17 fl 0x83, drawbridge m09 fl
  0x04/0x85) are NOT lock-gated; no synthetic test door needed.

### 4. Tooling

- `export_level.py --door-locked DIR --area A --sub S --overlay ...`:
  idempotent `locked` token emission on the matched door lines +
  decode marker comments (`door <file> x y z yaw r locked [goto ..]`).
  Run on all three exported scenes (office = 0 locked, office0 +
  drawbridge = 1 each).
- `gen_sfx_registry.py`: locked rattle 0x3F2 added to the office
  preset (snd_0533, 30959 Hz — same WAV in every exported area's
  bank); `locked_door_census()` re-derives the gate + the VO verdict
  per lock-gated door from the user's local overlay/ELF and emits it
  as registry comments — and would emit a `lockedvo <id>` scene line
  if a locked line ever resolved to a real audio cue (none do).

### 5. Port (extermination-port, this session) + verification

em_door grows the LOCKED SEQUENCE (engine subs 1/2, header block "THE
LOCKED SEQUENCE"): manifest `locked` token -> gate at the use-arm,
kickoff mode 1 (same side-latch/snap/staging walk), locked-look camera
cut via the new em_door_locked_look() feed (em_game doorcam state 4 —
the EM_DOORCAM_LOCKED env preview is retired, the real sequence drives
the s53/s56 func_001BBBF0 geometry, with the op07-sub4 instant chase
restore at the finish), player try anim 0x46/0x44, the door EMDL's
locked-jiggle clip by ENGINE id (em_door now resolves op-0x0B clip ids
2/0 open and 3/1 locked through em_model_clip_index — retiring the
fixed clip-0 play), rattle 0x3F2 at the 60-frame mark, the lockedvo
slot (silent — engine truth), clip-end finish, re-arm CLOSED. No
fade, no warp; control + camera restore at the finish edge.
`em_door_unlock(i)` = the D_00810841 bit-set event (the engine's
door-panel/keycard scripts); em_door_is_locked/em_door_rattles
introspection.

EM_LOCKED_TEST=1 (EM_SCENE=assets/scene_drawbridge, the REAL m15
security door): refusal -> LOCKED_TRY with both locks, try anim 0x44,
locked-look camera geometry asserted ((-28.5, 10, -192) target /
(-28.5, 12, -205) eye), exactly one rattle, door re-armed CLOSED with
zero fade and the player held on the near side (parked at staging
-197.25, then the control restore lets the free-move wall separation
push the 4.5-u radius to ~-198.5 — never past the door plane), control
restored, then em_door_unlock + retry -> OPENING, open anim 0x43, the
real goto transit into scene_office0 — PASS. Full suite PASS (MOVE/
DOOR/TRANSIT/SLIDER/LOCKED/WEAPON/SFX/PAUSE/CAMREGION/AIM/MELEE/
ENEMY 1-4 + test-input + test-weapon). Default capture byte-identical
(old vs re-exported player.emdl under the same build, cmp exact).

Open (locked doors):
- The locked-slider script D_0024DA40's camera native func_001BB310
  is undecoded (the port approximates a locked slider with the hinged
  refusal minus anim/clip/rattle — FLAGGED, no exported placement
  exercises it).
- The radio TEXT machine (func_001FD950 typewriter + the slot-0x16
  bank strings + the D_00282155/156 stream-wait) is not ported — the
  locked refusal is silent where the engine shows the radio subtitle.
  Porting it would also serve every other radio message in the game.
- Which game events SET D_00810841 bits (the door-panel script's
  partner-door arm, s23) — the unlock authoring side is undecoded;
  em_door_unlock is the API stand-in.

_Last updated: 2026-06-11 (session 57)._

## PER-ROOM LIGHT RIGS FULLY DECODED — D_00251C50 record layout, camera fill, point-light fold, VU1 clamps; exported + ported (2026-06-11, session 57)

Static decode of the whole s51 per-actor lighting chain down to the
data: every byte of the rig record, the exact direction math, the
dynamic-light slot lifecycle and the kernel's clamp semantics. Source:
funcs 001D7B30/8130/8340/8690/89D0/7FA0/7BB0/7C30/8270,
001F6640/6760/6850/68B0, the VU0 helper identities (func_00102948 = qw
copy, 001028B8 = vadd, 00102900 = vmulx, 001026A0 = mat4xvec,
00102B08/A60/BB0 = RotX/Z/Y builders, 00102738 = DOT, 00102760 =
normalize) and the 62-qw skinning kernel at 0x23C780.
Tool: `tools/export_level.py --lightrig / --lightrig-dump`.

### 1. The rig table D_00251C50 (45 x 0x78, key (area<<8)|sub)

`func_001D7B30`: `func_001D2910(8)` active -> key forced 0xF00;
otherwise key = `D_00810700<<8 | D_00810701`. Linear search; **miss
falls back to ENTRY 0** (key 0x0000). Record:

    +0x00 u32  key
    +0x04/+08  fog near/far; +0x0C/10/14 fog RGB   (func_001D8FD0, s10)
    +0x18 f32  not read by the light path (fog family; raw in dumps)
    +0x1C f32  -> working set +0xB4 (paired with 0.0 at +0xB0; no
               reader found on the normal path)
    +0x20      3 light slots, 0x18 stride: { ANGLE ax, ANGLE ay,
               color r, g, b, w }
    +0x68/6C/70 ambient RGB        (all colors 0..128 GS-modulate scale)

**s51 correction: directions are stored as ANGLE PAIRS, not vectors.**
`func_001D8340` builds M = RotX(ax)·RotZ(az)·RotY(ay) (memory-row
form; az = working set +0x88/+0x98/+0xA8, never written by
func_001D8130 — BSS zero) and applies M^T to the base vector (0,1,0):

    dir = (sin ay · sin ax,  cos ax,  cos ay · sin ax)

so ax = tilt from +Y (world up), ay = azimuth. Most slot-2 entries
are near-+Y "from above" room lights. Slot 0's w (rec +0x34) = the
dir WEIGHT in the point-light fold (16.0 in 43/45 rigs, 64.0 in
0x400). Working-set map (D_00817BC0, ptr D_00275688, func_001D8130):
angles +0x80/+0x90/+0xA0, dir vectors +0xC0/+0xD0/+0xE0, colors
+0xF0/+0x100/+0x110, ambient +0x120 (alpha slot +0x12C = 128.0).

### 2. Slot 0 = the camera fill (and only slot 0)

`func_001D8340(actor, out3400, out3440, flag = actor[+0x2]&0x20,
point)`: flag CLEAR -> slot-0 color AND dir zeroed. Flag SET -> slot
0's angle vector is interpreted in CAMERA SPACE: the lookat
`D_00810610` is copied, TRANSPOSED (func_00102798), and applied to
M^T·(0,1,0) with w=0 — i.e. rotated view->world by the inverse view
rotation. Typical slot-0 camera vector (0.553, -0.249, -0.795) =
toward the camera (-z fwd), slightly from above/right — the engine's
over-the-shoulder fill on camera-facing normals. Slots 1/2 are static
world dirs every frame.

### 3. Dynamic point lights — the full slot lifecycle

- Registration `func_001D7FA0(pos, color4, type, f12, f13)` writes the
  STAGING array (ctx+0x1220, the next frame's +0x220 — the render ctx
  double-buffers): +0x0 = f12 (intensity MULTIPLIER), +0x4 = f13
  (ADDER), +0x10 pos, +0x20 color qword, +0x8 type, +0xC handle.
- `func_001D7BB0` (per frame): zeroes the 32 render slots + counters,
  then re-registers the room lamps (func_001F68B0) and ambient
  registrars (func_001F6E40).
- `func_001D7C30` (per frame): per ACTIVE slot, intensity ramp
  I' = max(0, adder + I·mult) and color ·= mult (placed lamps 1.0/0.0
  = constant; effect presets func_001D80E0 0.6/-1.0 and func_001D8100
  0.95/-0.05 = decays); then adopts new registrations into free slots —
  **color qword scaled x128 (func_00102900 vmulx.xyzw): the W LANE IS
  THE INTENSITY slot +0x2C**, so lamp I = D_0026EB70[type].w · 128.
  Type-1 (placed lamp) slots also random-walk two angles +-0.0314 rad
  into the +0x40 rotation matrix — a +-1.8 deg per-lamp direction
  FLICKER (key 0xF00 forces identity).
- Fold (func_001D8340 tail, gate func_001D8270: actor types {3, 8, 9,
  0xB, 0xD, 0x15, 0x16, 0x17, 0x3D, 0x3E} excluded, others only if
  [actor+0x44]+0x20 < 30.0 — small models): with toL = slotpos -
  actor's light point (node +0xC0 column or +0xB0):

      k    = 0.1 · I / max(dot(toL, toL), 1)     <- SQUARED distance
             (s51 said dist; func_00102738 is a dot of the
              UN-normalized offset)
      dir0 = normalize(dir0·w0 + sum M_flicker·(toL · 10k))
      col0 = col0 + sum slotcolor · 2k
      (sum bases D_00253170/80 = zero qwords)

  Slot 0 is REPURPOSED as the combined camera-fill + point-light
  light. With no lamps the fold is an identity (normalize(dir0·w0)).
- Per-room PLACED LAMP lists (`func_001F6760`): keys 0x0000/0001/0002
  (area 0), 0x0100 (drawbridge: 1 lamp), 0x0200 (office sub 0:
  8 lamps — the corridor wall lamps), 0x0E00, 0x1100, 0x1301 (+ a
  second list via func_001F6850); every other key -> none (office
  sub 1, snow have NO lamps). 0x28-byte records (+0x0 s16 marker >= 0,
  +0x4 s16 color type, +0xC/10/14 pos, +0x24 runtime handle);
  registration is story-flag gated in func_001F68B0 (e.g. 0x200 only
  while D_00810761 == 0xFF). Color types D_0026EB70 (vec4 · 128):
  type 0 (512, 128, 32) I=128; type 1 (1152, 288, 72) I=288; type 2
  (1659, 415, 104) I=415 — all warm orange; at the engine's k the
  glow saturates within ~5 u and fades ~1/d^2 (visible ~50 u).

### 4. Matrix build + kernel semantics (func_001D8690, VU1 0x23C780)

`func_001D8690(out3400, out3440, actorRGB = actor+0x80)`:
- out3400 (DIR matrix): columns 0..2 = dir0..2 (row 3 zero; column 3
  carries color0 — only the 3x3 is consumed via the per-node fold
  func_001D88B0 into each node's normal-matrix rows, so the kernel's
  N' = NM·n IS the intensity triple (I0, I1, I2)).
- out3440 (COLOR matrix, the 4qw at VU1 dmem 0x3F5): rows 0..2 =
  col_i · actorRGB (w = col_i.w · glow), row 3 = ambient · actorRGB +
  8388608.0 (2^23 int-bias trick; w = 2^23 + 64·glow) where glow =
  max(actor[+0x8C] - 1, 0); actor flag +0x2 bit 0x40 adds 64·actorRGB
  onto row 3 (the s51 self-glow).
- Kernel: `maxbcx vf12, vf11, vf00x` = **clamp I at 0** (no upper
  clamp), `mulAx/maddAy/maddAz/maddw` with the color matrix, then
  `minibcx vf14, vf13, vf17x` with vf17 = 2^23 + 255 = **clamp at
  255**. The result IS the GS vertex color: shade = tex·rgb/128
  (modulate), so 128 = identity and rigs can over-brighten to ~2x.

Verdict: per character vertex,
`rgb = min(amb + sum_i max(dot(dir_i, N), 0) · col_i, 255) / 128`.

### 5. Room selection = the key bytes, not geometry

The rig (and lamp list) key is (D_00810700<<8)|D_00810701 — the
AREA/SUB-STATE bytes. Sub-state flips are the s56 room moves /
transitions; there are no spatial room bounds in this system. For the
port each exported scene is one (area, sub) pair, so the active
scene's manifest rig IS the engine's room selection.

### 6. Exporter + port (extermination-port, same session)

- `tools/export_level.py --lightrig DIR --area A --sub S` writes a
  marker-delimited block into the scene manifest: `lightamb r g b`,
  `lightcam dx dy dz r g b w` (slot 0, camera-space), 2x `lightdir dx
  dy dz r g b`, and `lamp x y z r g b i` lines (x128 registration
  values). `--lightrig-dump` prints all 45 rigs + every lamp list.
  Exported: scene (0x201), scene_office0 (0x200, 8 lamps),
  scene_drawbridge (0x100, 1 lamp), scene_snow (0x600).
- Port: `em_gfx_char_rig` (em_gfx.h + Metal fragment rows) runs the
  kernel-exact composition on the character path; em_game's
  char_rig_build composes per actor draw (camera fill = PLAYER only
  per the decoded func_001D8BF0 gating; lamp fold at each draw's
  bone-0 translation; menu turntable = rig with slot 0 zeroed — the
  engine's mode-2 path, replacing the s56 camera-fill stand-in).
  Omitted, flagged: lamp flicker jitter, story-flag lamp gates, the
  func_001D8270 type exclusions (the port folds for all actor draws).
  The invented stand-in directional remains ONLY for rig-less scenes.
  The DEFAULT CAPTURE CHANGES (player lighting is now the office
  0x201 rig + camera fill instead of the invented light) — a
  deliberate fidelity change; level geometry stays byte-identical
  (baked vertex color, never rig-lit).

Open:
- rec +0x18 / +0x1C consumers (fog family / the +0xB0 working pair).
- The lighting-override rigs behind func_001D8C30 modes 1/3/4/5/6
  (menu/cutscene specials; jtbl 0x0026E520) — mode 2 (status menu)
  confirmed to run the normal path.
- Which actor types {3, 8, ...} the func_001D8270 exclusion list maps
  to (by name), and the engine event flipping the lamp gate bytes.

## PLAYER DAMAGE & DEATH PIPELINE — full decode (2026-06-11, session 58)

Closes PORT_DIFFERENCES C12/C14/P3 ("no damage/death/game-over
pipeline"). Static decode of the player hurt/death cluster — the 0x21B
-0x225 range FUNCTIONS.csv had as "area_logic" is the PLAYER HIT-
REACTION MACHINE (state-2 sub handlers + helpers).

### 1. The damage interface is TWO pending floats + a type byte

The player has NO +0x36 mailbox. Producers write the player actor
(0x008102B0) directly:

```
+0x224 (D_008104D4)  f32  pending HEALTH damage   (leech latch 5.0 /
                          lunge 15.0 — s22b's "drain magnitude")
+0x22C (D_008104DC)  f32  pending INFECTION damage (breather pad 5.0 —
                          the s33 "event 3" write INFECTS, not wounds)
+0x0F  (D_008102BF)  u8   damage TYPE byte (leech latch writes 2;
                          typed reactions 1..0xB = scripted/latch
                          paths, 0x63 = the infected-drain death)
+0x00                u8   event byte: 1 = normal/vulnerable, 2 = hit-
                          reacting/dying, 3 = pad event (s33)
+0x20E               s16  post-hit INVULNERABILITY countdown
+0x234 (0x8104E4)    u8   INFECTED latch (1 at infection 100; 2 mid-
                          infected-death). The SAME byte is the s39
                          "display max -> 60" flag (C14 closed)
+0x235               u8   bit 0 = low-health latch (health <= 35)
+0x236               u8   armed-stance flag (clip-variant select)
```

Every producer requires event == 1, so the player is immune from
hit-reaction start until +0x20E (armed at flinch END: 0x3C = 60
frames, 0x5A = 90 after a latch hit) counts out — i-frames are
producer-side, plus anim-id gates func_0021BB00/func_0021BC40 (the
current +0x1F0 in a blocked set drops typed/generic damage — door/
climb/scripted anims are damage-proof).

### 2. func_0021C440 — THE damage processor (3424 B)

Called at the head of player state 1 (func_0015B130) AND state 2
(func_0015B770) every frame, via func_0021C440's callers. Flow:

- health <= 0 already: clear pending, event = 2, return.
- +0xF typed paths 1..0xB: each picks a state-2 sub + marker anim
  (+0x1F0 = 0x3B/0x3C/0x3E/0x3F/0x40 — CATEGORY MARKERS, see below)
  with per-type specials (type 5/0xA/0xB zero health; type 7 anim-
  gated + sound 0x159; type 2 anim-blacklist + func_0021D640/6C0).
- latch ticks (+0x234 == 1 && event == 1 && +0x23A in {0x5B, 6}, or
  func_0021C3F0-gated +0x23B == 0xA): periodic +0x224 = 3.0/5.0/8.0
  re-arms with blood effect 0x8000001B/0x80000044 + knockback yaw
  into +0xC4 (atan2 of attacker facing + pi/2).
- GENERIC TAIL (the common path): func_0021BC40 anim gate, then
  func_0021C350 (health apply), func_0021C270 (infection apply),
  then route: health <= 0 -> death entry; else flinch entry (state 2,
  sub by context: 0 generic, 0x17/0x18 from aim subs 0x1D/0x1E,
  2/0x18 variants when infected; anim marker 0x3E).

**func_0021C350 (health apply)**: health -= +0x224, clear; <= 35 ->
+0x235 |= 1; <= 0 -> health = 0, event = 2.

**func_0021C270 (infection apply)**: infection += +0x22C, clear; >=
100 -> infection = 100, health = min(health, 60), +0x234 = 1 with
D_00810707 = 1 (re-infection inhibit — the s33 pad gate) and
D_008106F1 = 1, func_0021D4E0, sound 0x149 @ vol 300. "DENNIS
INFECTED" = NOT a death: a 60-HP cap + the passive drain below + the
pager line (s39 bank 0 line 3).

### 3. State 2 sub handlers (jtbl_0026D450, 26 subs)

- **sub 0 FLINCH (func_0021D800)**: rumble (0,0xC0,5); voice 0x152
  (health hit) / 0x153 (infection hit, +0x1F1 == 1) @300; RNG &1
  picks a flinch FAMILY, func_0021D1A0 the side: armed 0x56/0x57,
  unarmed 0x1E/0x1F vs 0x20/0x21, infected 0x1C7 (all clip ids =
  library directory ids; requested via func_001749A0 — the +0x1F0
  markers never play). Root-motion mover + gravity (-0.2) + floor
  settle while playing; clip end -> +0x20E = 60 (90 if +0x1F1 == 2),
  exit state 1 sub 7 anim 0xD (recover).
- **sub 1 DEATH (func_0021E240)**: phase 0 = rumble + sounds 0x146
  (voice) + 0x151 (body) @300, clip 0x2A (armed +0x236 -> 0x5C),
  both 130 f fall-to-ground (motion-audited: head y 14.6 -> 1.4);
  during the clip at frames-remaining 80 -> sound 0x156, 50 ->
  func_00182870(1), 16 -> func_0021D490 (thud 0x14E / infected
  0x14F @300) + rumble (1,0xEE,0x3C); root-motion mover throughout.
  Clip done -> func_0021D2E0.
- **sub 3 INFECTED DEATH (func_0021E830)**: clip 0x1C4 (300 f
  succumb sequence), gore effect 0x80000051, sounds 0x146 + 0x151;
  at clip time <= 160 -> +0x234 = 2, D_00810707 = 2,
  func_0015C1F0 (the full-transformation visual flip); end ->
  rumble + func_0021D2E0.
- **func_0021D2E0 (terminal)**: phase 0 = blood-pool effect
  0x80000043 at node-1 XZ (y = +0x250 + 0.1, scale 1.0), event = 2,
  health = 0, hold +0x28 = 0x78 (120 frames); phase 1 = countdown,
  at 0 (and +0xF != 0xB) -> **func_001AEDE0(4,0)** — the STANDARD
  fade-out; phase 2 = parked. Corpse settle (y -= 0.2 + floor) when
  called with mode 0.
- Subs 4/9 are entered by the pending-damage checks func_00181110/
  func_001821E0 (state-1 action subs poll them); subs 0xC-0x15 are
  the typed-reaction sequences (0x21Fxxx-0x225xxx, keyed on +0xD).

### 4. Passive ticks (state-1 tail)

- **func_0015D100** (gated on event bit 2 clear): hazard-room drain
  (room attr 4 + D_008106C8 & 0x60 + suit byte D_00810C7E == 0 ->
  health -= 1.0 / 360 frames) and the INFECTED drain (+0x234 != 0 ->
  health -= 2.0 / 240 frames + green effect 0x80000063). Drain death
  -> event 2 with +0xF = 0x63 -> the INFECTED death sub.
- **func_0015D000**: low-health HEARTBEAT = pad rumble only
  (func_001B61C0(0, 0xD0, 4, 0) every 121 frames at health <= 35;
  0xE0 every 61 at <= 10). No sound.
- Kill plane (spine): pos.y < -200 -> state 6 (func_0015D460): sub 0
  health = 0 + event = 0, sub 1 func_001AEDE0(4,0), sub 2 parked.
  No anim, no sound. State 6 is ONLY the kill plane; real deaths
  stay in state 2.

### 5. Anim-id MARKERS vs real clips

The damage paths write +0x1F0 = 0x3B/0x3C/0x3E (hit), 0x3F (infected
death), 0x40 (death) — but every state-2 sub handler immediately
requests its REAL clip through func_001749A0, so the markers exist
for the anim-id-keyed gates (invulnerability sets, the D_008106B3
menu inhibit in func_0015BA50's tail). Motion audit: container 64
(0x40) stays standing — it is not the death fall; 42 (0x2A) and 92
(0x5C) are.

### 6. GAME OVER — flow decoded to the screen launch; trigger OPEN

After the death fade-out the engine is at HOLD-BLACK (D_0028A9A0 == 2)
with the player machine parked; no B5..B8 request is posted. The
end-screen machinery: game task state 6 (func_001AE040, reached when
func_001AE7E0 returns 3 = D_008106CE != 0) stops audio and launches
the DATA.DAT screen module — func_001FF030(D_008106CF) when
D_008106CE == 2, else func_001FEFE0(D_008106CF) — busy-gated on
D_00275BD8, then resets to gameplay state 1. The ONLY D_008106CE
writer in the main ELF is func_001B7700, a POINTER-CALLED script op
(no static callers; rec +0x14 = screen id, +0x80 bank offset on one
arm). **OPEN: the dead-player -> D_008106CE/screen trigger was not
found statically** — candidates: an overlay-resident watcher, the
flow-task state 5 compositor (func_0021B180/550/840 — a three-layer
fade-in renderer + func_001FF080 launch), or func_0022A650. Needs a
live death capture in PCSX2 (watch D_008106CE/CF + D_00275BD8 +
task bytes at death).

### 7. Sound/effect id summary (all GLOBAL bank, vol 300)

```
0x152 flinch grunt        0x153 flinch grunt (infection hit)
0x146 death voice         0x151 death body foley
0x156 mid-fall cue (T-80) 0x14E ground thud (T-16; 0x14F infected)
0x149 infection-100 sting 0x14D infected-death extra (func_0021C200)
0x147 flinch settle extra 0x159 type-7 latched kill voice
FX: 0x8000001B/44 latch blood, 0x80000043 blood pool, 0x80000048
infected-death burst, 0x80000051 infected-death gore, 0x80000063
infected drain tick
```

### 8. Port (extermination-port, same session)

em_game.c "PLAYER DAMAGE & DEATH" block + em_hud: the generic-tail
processor, flinch/death/infected-death sequences with the decoded
clips/sounds/cues, infection mechanics (60-cap + drain + display-max
swap = C14), producer-side i-frames, kill plane, corpse hold + the
4-speed fade, and a FLAGGED game-over stand-in (black + "GAME OVER /
PRESS START" in the real UI font; START reloads the scene) until the
engine screen module + trigger are decoded. player.emdl re-exported
+10 clips (canonical CLI updated); sfx registry +11 ids.
EM_DEATH_TEST=1 (flinch -> death -> game over -> restart) PASS;
default capture byte-identical (code cmp-exact; asset append-only —
shared clip palettes verified byte-identical).

Open:
- the dead-player game-over trigger (above) + the game-over screen
  module id D_008106CF values / DATA.DAT chunk -> texture export.
- typed damage paths 1..0xB (producers = scripted/latch attackers
  not yet ported); the latch-tick fields +0x23A/+0x23B identities.
- func_0021D1A0 (flinch side select — direction test, port uses RNG).
- whether the infected latch survives an engine continue.

_Last updated: 2026-06-11 (session 58)._

## DOOR TRIGGER IS THE CROSS PRESS EDGE — use-scan callers decoded; the s17 "walk-into via 0x2D" contract is OVERTURNED (2026-06-11, session 58)

Static decode of the use-scan arming condition (the s20/s22 open item:
"the class-5 trigger conditions could not be traced live"). Verdict:
**doors — hinged m03/m15 AND m09/m17 sliders — arm ONLY on a CROSS
press edge inside the class-5 window. The engine has NO walk-into door
trigger.** PORT_DIFFERENCES #5 / K2 are closed in the opposite
direction from their framing: the port's CROSS gate was the engine
behavior all along.

### 1. Every use-scan call site is button-gated

`func_00184BA0` has exactly four callers — the player ground-state
handlers `func_00160220` (0x16024C), `func_001612D0` (0x1613E4) and
`func_0016DE40` (x3: 0x16E048/0x16E128/0x16E1F0) — and every one
gates the call identically:

```
lhu  v1, D_00810E74          ; pad press-EDGE mask (see 2)
lhu  v0, 0x70003B76          ; config-mask block "use" entry = 0x0040
and  v0, v1, v0
beqz v0, skip                ; no new CROSS press -> NO scan this frame
jal  func_00184BA0
```

spad `0x70003B76` is the s29-decoded config-mask block entry "X, use"
(default 0x0040). The scan body itself (func_00184BA0, read in full)
adds only: frame selector spad `0x70003B8D` == 0, fade-wait halfword
`D_0028A9A0` == 0, inhibit byte `D_008106EF` == 0, then the
interactive-list walk (status bit0, class flag 0x80, `+0x0B` == 0)
with `func_00183EF0` per candidate — return 2 = instant winner,
return 1 = nearest-by-dist² (spad 0x70003B98) wins. Winner:
`+0x0B = 4`, spad `3B8D = 3`. On success the caller (func_00160220)
runs func_001798D0 and puts the player in state 0x25/0.

### 2. D_00810E74 is the press-EDGE mask, not the held mask

`func_001B5BC0` (pad decode, per frame):

```
D_00810E72 = prev sample's inverted raw pad   (held, last frame)
D_00810E70 = (raw[0]<<8 | raw[1]) ^ 0xFFFF    (held, this frame)
D_00810E76 = old D_00810E74                   (prev edge)
D_00810E74 = E70 & ~E72                       (newly pressed = EDGE)
```

(Some earlier notes called E74 the "held mask" — wrong; E70 is held.
The s26 live note "0x810E74 & spad3B76 = X edge" had it right.)

### 3. The 0x2D check EXCLUDES doors — it guards the class-7 prefix

`func_00183EF0` head (0x183F14..0x183F4C): reads player `+0x1F0`,
candidate class byte `+0x02`.

- `+0x1F0 == 0x2D` (the push-into action code): ONLY `class & 0x1F
  == 7` proceeds (the LOS / dist² <= 144 / 2-u auto ring / facing-dot
  ~0.4 block — the s17 reading). Any other class, **including class-5
  doors, returns 0**.
- `+0x1F0 != 0x2D`: dispatch on candidate `+0x08` (jtbl_0026D810;
  +0x08 >= 6 and entry-0 both land at .L00184154) -> the class-5 DOOR
  branch (s45 geometry: doorway-center {10, 8} windows, side π/2,
  through-door facing π/4) and the other-class branches.

So s17's "doors trigger on walk-into (state 0x2D), no button" read the
class-7 prefix conditions AND attributed them to doors; in truth the
0x2D state is for class-7 interactables (pushing into panels etc.),
and a door can only arm while the player is NOT in 0x2D. This is also
why s22's analog-only pad injection never armed a door organically —
no CROSS edge was ever generated. The s56 slider-brain "walk-into, no
button" trigger claim inherited the same error (the slider plumbing IS
the same use scan — hence the same CROSS edge).

### 4. Corrections ledger

- s17 "Door contract" / "Trigger (who arms +0x0B)": walk-into →
  CROSS edge (blockquotes added in place).
- s22 open item "class-5 trigger conditions" — CLOSED.
- s56 slider TRIGGER bullet: walk-into → CROSS edge (blockquote).
- PORT_DIFFERENCES #5 + K2: the port REQUIRED no change to the hinged
  trigger; K9 sliders moved from stick-push arming to CROSS.

### 5. Port (extermination-port, this session)

em_door.c door_trigger_scan: the walk-into (stick past the dead ring)
slider arming is REMOVED — both families arm on `in->pressed &
EM_PAD_CROSS` (the port's pressed mask = exactly the D_00810E74
cur&~prev edge semantics) inside the class-5 window; comments rewritten
to the decoded contract (em_door.c/h, the "TRIGGER IS NOT A DEVIATION"
fidelity note). EM_SLIDER_TEST now asserts the negative AND the
positive: frame 13 inside the window pushing with no button = still
CLOSED + unlocked, frame 14 CROSS edge = OPENING; the rest of the
sequence unchanged — PASS. EM_DOOR_TEST keeps its frames-1..58
no-button approach (now engine-true twice over: no auto ring AND no
walk-into) and its frame-60 CROSS press — PASS. EM_LOCKED_TEST /
EM_TRANSIT_TEST / EM_PAUSE_TEST / EM_MOVE_TEST PASS unchanged; default
EM_CAPTURE byte-identical vs a HEAD-em_door build (cmp-exact).

Open:
- what WRITES action code 0x2D into player +0x1F0 (the class-7
  push-into arming — its writer was not located this session; doors
  do not need it).
- D_008106EF identity (a scan inhibit; observed set to 0x50 in a
  scripted-sequence context).

_Last updated: 2026-06-11 (session 58)._

## ENGINE PROJECTION EXACTLY DERIVED — near/far decoded bit-exact, field-space raster window, state01 pixel verification; PORT ADOPTED (2026-06-11, session 59)

Closes the s23b/CAMERA-SYSTEM §3 "native remap" sketch and the port's
TODO(projection) (PORT_DIFFERENCES D10/P6). Everything below is static
.s + savestate arithmetic, verified against the state01 rendered frame.

### 1. The P builder pair (func_001D2960 / func_001D2D20)

`func_001D2960` (the per-frame main-P builder, ctx+0x2340) stores the
literals directly: m00 = **0.8·s** (0x3F4CCCCD·zoom), m11 = **0.5·s**,
centers **2048.0** (0x45000000), w-column 1.0, and the Z-row pair
**bz = 0x3F664CB3** (0.8996078…) / **az = 0x49CCCCCC** (1677721.5) —
the values FINDINGS previously read live as "0.8996 / 1677721.5".

`func_001D2D20(m, zoom, w, h, near, far)` is the engine's own
PARAMETERIZED perspective builder (used for the offscreen/level
variants): m00 = zoom/(0.5·w), m11 = zoom/(0.5·h), m22 =
(far+near)/(far−near), m32 = −2·far·near/(far−near), m23 = 1 — a
GL-style Z row, unlike the main P's GS-Z row. Its callers pass
**far = 16711680.0 (0xFF0000)** and near 0.1 (level kernel) / 20
(offscreen passes).

### 2. The Z-row literals decode EXACTLY to near 0.1 / far 16711680

GS Z after the VU1 divide (confirmed live, see §4): z_gs = bz + az/z.

- **az = 1677721.5 = 0.1 · (2^24 − 1) exactly** → z_gs(0.1) = 2^24−1,
  the 24-bit GS-Z max: **near = 0.1**.
- **bz = f32(1 − az/16711680) = 0x3F664CB3 bit-exact** → z_gs(16711680)
  = 1.0: **far = 16711680 = 0xFF0000** — the same far literal the
  engine passes func_001D2D20. (Equivalently the SCE
  sceVu0ViewScreenMatrix form with zmin=1, zmax=2^24−1, n=0.1,
  f=16711680 reproduces both constants to f32 rounding.)

So the engine's frustum is near 0.1, far ≈ 16.7 MILLION units — an
effectively infinite far plane (fog + the ±(s,0,−1023) cull planes
bound the scene, never the far clip), reversed-hyperbolic onto the
24-bit Z buffer (nearer = larger z_gs).

### 3. X/Y: the raster window is FIELD-space in y — the exact fovs

The visible GS window (pinned by the UI module's decoded offsets
0x700/0x790) is x ∈ [1792, 2304] (512 px, half 256) and y ∈
[1936, 2160] (**224 field lines** = 448 display lines, half 112); both
centers are the P's 2048 offsets. Therefore on the 4:3 frame:

    ndc_x = (0.8s/256)(x/z) = (s/320)(x/z)   tan(hfov/2) = 320/s
    ndc_y = (0.5s/112)(y/z) = (s/224)(y/z)   tan(vfov/2) = 224/s

At s = 480: **hfov 67.38°, vfov 50.03°**. The tan ratio is 10/7, not
the square-pixel 4/3: the 0.8/0.5 row anisotropy bakes the
512x448→4:3 pixel aspect and leaves a real ~7% horizontal angular
compression (a sphere renders ~93% as wide as tall on the original
display) — a property of the original image, to be reproduced, not
corrected. The scope camera's documented "s = 224.0/x" is this same
model: x is tan(vfov/2), 224 the field half-height. (The s23b-era
phrasing "z = 0.8996·z + 1677721.5" was the pre-divide CLIP row; and
the old "tan(half-hfov) = half_w_gs/(0.8s)" sketch left half_w/half_h
unpinned — they are 256 and 112, NOT 224.)

### 4. Verification against the state01 rendered frame

From state01 ee.bin: K = P·V at ctx+0x23C0 (zoom 480). Projecting the
live player root (218.59, 229.85, 201.79) through the ENGINE's own K
and mapping GS→frame (x: [1792,2304]→640, y: [1936,2160]→480) gives
**(320.0, 441.1)** — visually confirmed to land between the player's
boots in the savestate's 640x480 screenshot (and the camera target
projects to exactly (320, 240), screen center, as it must). The live
z_gs at that point is 39638.7 = bz + az/42.33 to float precision —
proof the GS Z **is** divided by w (settling the linear-vs-hyperbolic
ambiguity).

### 5. Port adoption (extermination-port, this session)

- `em_math.h em_mat4_perspective_gs(m, s)`: m00 = s/320, m11 = s/224,
  near 0.1 / far 16711680 hyperbolic [0,1] depth (ordering identical
  to the GS's reversed encoding; clip planes exact). The old 50°-at-
  window-aspect + invented 0.5/500–800 planes are retired.
- Zoom s is wired through the camera (`EmCamera.zoom`, the native
  ctx+0x2468): default 480 at init; scope (224/tan) and scripted lerps
  have their field ready.
- The gfx backend letterboxes/pillarboxes every frame to the centered
  4:3 rect (viewport + scissor + in-frame clear; bars black) — the
  window no longer stretches the image; PCSX2-style presentation.
- `EM_PROJ_TEST=1`: the port chain (engine commit semantics +
  perspective_gs + 4:3 viewport mapping) reproduces the engine-K
  pixels of 5 state01 world points (incl. off-center, both axes) to
  **< 0.001 px** — PASS. All other self-tests PASS (the EM_SLIDER_TEST
  / EM_LOCKED_TEST failures present during the run also fail at a
  clean HEAD build and belong to the in-flight door-trigger rework,
  not the projection).
- Captures re-baselined (`scratch_noassets/captures_proj/`): vertical
  framing is essentially unchanged (old fovy 50° ≈ derived 50.03°, the
  old value was a good empirical pin), horizontal gains ~7% scene
  (hfov 63.7°→67.38° at 4:3), near 0.5→0.1 reveals close geometry,
  far 500/800→16.7M unclips distant geometry. EVERY capture's framing
  differs from pre-projection baselines — old baselines are obsolete.

Open (UI projection, s49 pin): the status screen keeps its empirical
tan(fovy/2) = 0.74 @ 4:3 projection. Under the engine s-model the same
decoded x-anchor implies a menu zoom s ≈ 324.3 → tan(vfov/2) = 0.691,
~7% tighter vertically than the pin (the pin assumed a square-pixel
4/3 fx/fy ratio the engine P does not have). Needs a live ctx+0x2468
read with the status screen open; until then the pinned look stands.

_Last updated: 2026-06-11 (session 59)._

## AUTO-AIM FULLY DECODED — func_00199220 screen-cone chain, the +0x2F0 round-robin, func_0017AF70 lock-steer rates; PORT TRANSLATED (2026-06-11, session 60)

Closes the s23 partial reading of the 3-target auto-aim (the port's
distance + 10° world-cone stand-in, PORT_DIFFERENCES H3). Full static
decode of the acquisition, the per-shot target cycle and the lock
steer; all three now run natively in `em_weapon.c`.

### 1. func_00199220 — acquisition, the exact math

Entry (a0 = player): copies the GUN muzzle `gun+0xA0` → spad `38A0`,
clears `D_008106E0/E4/E8`, seeds the three best distances f21/f22/f23
= 1000.0, then walks the published list `D_00275B8C` (count
`D_00275B94`). Per candidate, IN ORDER:

1. status byte != 0 → `func_00183B80` != 0 → HP `+0x34` != 0;
2. aim point `func_00183C40(cand, 0x700038B0)`; **distance from the
   PLAYER position `+0xA0`** (not the muzzle) to the aim point, f20 =
   sqrt; reject f20 >= 260.0 (`0x43820000`);
3. SCREEN CONE: aim point (w=1) transformed by the spad camera matrix
   `0x70003AC0` (`func_001026A0`). Reject 16/w < 0 (behind camera).
   Screen coords: `sx = x'/w − 2048`, `sy = 1.5·(y'/w − 2048)` — GS
   window pixels off the 2048 center (the visible half-extents are
   256 ×112 GS units; the 1.5 y-scale makes the y test span ±168).
   The mult/div-by-16 in the .s cancels (codegen artifact). Then by
   aim option `D_00810CA4`:
   - `== 1` (lock-on): radial — sqrt(sx²+sy²) ≤ 50 + 55·s;
   - else (manual 0 / mode 2): box — |sx| ≤ 66 + 50·s AND
     |sy| ≤ 45 + 45·s;
   where s = gun scratch `+0x214` float. **No instruction in the boot
   ELF ever writes gun+0x214** (exhaustive offset grep) — actor
   scratch is zero-initialized, so s = 0 and the cone is the fixed
   66×45 box / r=50 disc.
4. ACTOR RAY: endpoint = muzzle + (aim − muzzle)·1.2 (`0x3F99999A` —
   a 20% validation overshoot, distinct from the bullet's +5 u);
   `func_0019A570(muzzle, end, 1, 0x20)` must HIT and the result
   actor `*0x700031D4` must BE the candidate (an interposed actor
   disqualifies);
5. WORLD LOS: `func_0019A570(muzzle, 0x700031B0 = the actor ray's hit
   point, 6, 0)` must MISS (clear line).

Survivors run a 3-slot insertion sort on the step-2 distance: E0 ≤ E4
≤ E8 (`f21/f22/f23` shift with the pointers). Tail: reticle markers
`func_001DD170(1, aimPoint, 0, 0x80808080, 0)` — lock-on mode draws
E0's only, manual draws all three (loop over `D_008106B0+0x30..0x38`,
which IS E0/E4/E8).

### 2. WHO calls it — the stance split (corrects the s23 conflation)

`func_00199220` is called from stance tops **0x1E (func_001703E0) and
0x20 (func_00173000) only** — the R2 family. The R1 tops 0x1D
(func_0016FCF0) / 0x1F instead maintain E0 via the LASER-RAY lock
(`func_00185A10` acquire / `func_00185E30` revalidate — a ray down the
barrel, mask 0x20, same status/targetable/HP gates), cleared whenever
manual steering flags `+0x302` or sub-weapon != 0. The fire SM head
(func_00170A60, a1==0 callers = the 0x1D family) runs the lock steer
`func_0017AF70` when E0 != 0; the 0x1E top calls the SM with a1=1
(steer skipped — the R2 stance aims with the camera). The laser drawer
`func_00185760` flips to the warm arm purely on `D_008106E0 != 0`,
whichever system filled it.

### 3. The +0x2F0 round-robin — increment policy

Stance tops (all four), manual mode only (`D_00810CA4 == 0`): if the
trigger latch `+0x274` is set, `+0x2F0 = (+0x2F0 + 1) % 3` — BEFORE
the per-sub fire-SM dispatch. `+0x274` is SET only by `func_0017A8B0`
(the trigger-event handler that also stores the cadence `+0x2F4`),
which `func_001607D0` invokes on the fire-config bit: `D_00810E74`
edge for SEMI (C61==0), `D_00810E70` held for burst/auto — plus the
queued-semi refire arm (`.L00170E4C` sets it with `+0x2F2`). Every
shot state, the dry click and the release CLEAR it. Net: **one
advance per trigger event ≈ one per shot** (semi presses, queued
refires, auto-refire expiries); burst rounds 2/3 chain through the
step-back with the latch clear — a 3-round burst holds one slot.
Stance entry zeroes `+0x2F0`.

The bullet (`func_001861C0`, contexts 0xC/0x29): CA4==1 → E0 or
untargeted; CA4==0 → `+0x2F0` 1 → E4, 2 → E8 (EACH falling back to E0
when null), else E0; the chosen target's aim point is re-queried
(`func_00183C40`) at fire time, endpoint overshoots +5 u; no target →
muzzle + dir·260.

### 4. func_0017AF70 — lock steer, exact constants

Gate: `+0x2F2` (the laser-visible/aim latch — steering pauses through
each shot's cadence) and `D_008106E0`. Two constant sets by mode
`+0x05`: set A for 0x1D/0x1E, set B otherwise (0x1F/0x20):

```
              set A (0x1D/0x1E)        set B (0x1F/0x20)
yaw eps       3.78000e-4 (39C62E4D)    5.94000e-4 (3A1BB6AA)
yaw + side    1.0469040  (3F8600F3)    1.0458360  (3F85DDF4)   rad
yaw − side    1.0470290  (3F86050C)    1.0463070  (3F85ED63)   rad
pitch split   1.5693710  (3FC8E126)    1.5655510  (3FC863FA)   rad
pitch A       1.3957210  (3FB2A6FC)    1.3935290  (3FB25F29)   rad
pitch B       1.3972940  (3FB2DA88)    1.3981010  (3FB2F4F8)   rad
```

The yaw sides are the MEASURED ±60° ladder pose angles (59.98°/
59.99°), the pitch constants the ~80° pitch span (79.97°/80.06°) —
i.e. the steer maps angle error to blend units through the baked
ladder geometry. Desired yaw = `func_001B1240(muzzle, target.x,
target.z)` − heading `+0xC4`, wrapped (`func_001B1470`); current from
the gun dir `+0xC0` (atan2 `func_0011E620`). Desired blend =
blend − 0.5·(des − cur)/side (side picked by des vs eps; the engine's
y-down/blend-axis signs — a port translates magnitudes). Pitch:
desired `func_0017A800(muzzle, target)` vs current atan2(dir.y,
horiz); side split des ≤ 1.5694 is in practice always true (atan2
pitch < 90°) — pitch B is dead in flight. Both desired blends clamp
[0,1]. THEN: the (Δyaw, Δpitch) blend delta is normalized
(`func_00102760`) and stepped **0.02/frame** (`0x3CA3D70A`); |Δ| ≤
0.02 SNAPS to the desired pair (writes `+0x27C/+0x278` directly). The
delta magnitude is also published to spad `0x70003A20`.

### 5. Port translation (extermination-port, this session)

- `em_weapon.c weapon_acquire()` — the full chain each AIM tick;
  screen cone via the new `em_gfx_last_viewproj` (the gfx layer's
  published last-draw P*V = the engine's 0x70003AC0 read, one frame
  stale like the bone publish); sx = 256·ndc_x, |sy| = 168·|ndc_y|;
  spread fixed 0 (no writer, documented). New em_enemy queries:
  `em_enemy_targetable` / `em_enemy_aim_point`.
- Round-robin: `w.cycle` advances at the decoded trigger events
  (press accept / queued expiry / auto refire; burst rounds chain);
  the bullet picks slot[cycle] with the E0 fallbacks. The port's
  single stance merges the engine's 0x1D/0x1E split: acquisition =
  the 199220 3-slot chain, lock = slot 0 (documented conflation —
  the 0x1D laser-ray lock func_00185A10/E30 is not translated).
- `em_weapon_lock_steer()` — set-A constants, 0.02 step/snap;
  em_game's player_move applies it when the stick is idle (the
  engine's +0x302 manual-input lock drop).
- Laser lock color flip + 5.0-unit warm dot now ride the REAL lock
  state (slot 0) — the s23b warm arm (0x70/0x40/0x20 + rand5)/0x80.
- Tests: `make test-weapon` gained the 2-enemy ROUND-ROBIN section
  (slot fill nearest-first, victim sequence B,A,A,B over 4 semi
  presses, the exact 0.02 steer step, lock clear on stance drop);
  EM_WEAPON/EM_ENEMY 1–4/EM_AIM/EM_MELEE/EM_PROJ self-tests PASS
  (the kill run now pitches the aim down until the screen-cone lock
  fills — engine-true: a floor crawler under a level aim is OUTSIDE
  the cone; the old planar stand-in ignored height). Default capture
  byte-identical vs baseline.
- UNTRANSLATED, flagged: the reticle markers (func_001DD170), the
  lock-on aim option's radial cone selection (H16), the R2-stance
  set-B steer constants, the 0x1D laser-ray lock.

_Last updated: 2026-06-11 (session 60)._

## CAMERA WALL SOLVER func_0018DD20 DECODED — the "wall rise" is constant-height pull-in (2026-06-11, session 61)

Full static read of the 6984-byte solver body (`func_0018DD20.s`, all
1814 lines) plus its dispatcher `func_0018D7B0`, pre-pass
`func_0018D330`, and every caller of the dispatcher. Closes
PORT_DIFFERENCES item 8 / D3 (the port-invented rise model). Translated
verbatim into `extermination-port` `em_game.c cam_solver_0018DD20`.

### Who calls what — the style map (every `jal func_0018D7B0` audited)

`func_0018D7B0(cam, style)` picks mask **7** (movables in) only for
style 2, else mask **6**; always runs the pre-pass func_0018D330; then:

| style | solver | callers |
|---|---|---|
| **0** | **func_0018DD20** full path, then dispatcher chases ACTUAL eye D_008105D0 toward desired cam+0x10 (func_0018C6A0/func_0018C4B0, cap **4.0**/frame) | **the whole generic gameplay camera**: idle states 1/0x26/0x27 (func_001921D0 tail) and locomotion states 2/4/0xF (func_00230000) |
| 1 | func_0018DD20, then actual = desired HARD COPY | door-cut/cinematic placements (anim_frame_top_b, func_001921D0 state 0x2F) |
| 3, 4 | func_0018DD20 (style 3 takes a reduced branch: no waiver, simple pull-to-hit; caller chases at 0.8) | func_001921D0 cinematic/special states (6, 7, 9, 0x13, 0x14, 0x15, 0x18, 0xA/0x19, 0x2C/0x2D) |
| 5 | func_0018D910 (returns 0) | func_00195130 director fixed cams, func_00194DB0, func_00230000 fall-cam |
| 2, 6 | **func_0018F870** (mask 7) | **the AIM camera** func_00197D20 — a separate solver, still unread → "no rise while aiming" is STRUCTURAL |

Pre-pass func_0018D330 (always runs, outputs nothing the solver core
reads): cam+0x6D = func_0019B7D0 vertical query flag; cam+0x5A bits
(0x80 = ceiling-class within 200 above the head → cam+0x60 = its Y;
0x01/0x08 = a 20-u (aim: 9-u) ray from player+11 toward the eye hits
wall-class 0x2000 / ceiling-class 0x8800; aim adds 0x10 = +6-height ray
hits wall). Consumers unidentified — untranslated in the port, flagged.

### func_0018DD20 (cam a0, player a1, style a2, mask a3) — the decoded algorithm

Surface classes (rec+0x1A halfword, = the port's `surf_class`): wall
0x2000, ceiling-class 0x8800 (CEIL|STEEPDN), floor-class 0x5000
(FLOOR|SLOPE); the final floor-bound probe accepts **0x7000**
(FLOOR|SLOPE|**WALL** — engine-verbatim quirk).

1. **PRIMARY PROBE**: desired target (cam+0x20) → desired eye (cam+0x10)
   **extended 1.5 u** past the eye. No hit → step 4. On a hit:
   cam+0x58 = class halfword, cam+0x90 = wrap(atan2(n.x, n.z)),
   GLANCING := dot(horiz sight dir, horiz **raw** normal component)
   < 0.70710678 (0x3F34FDF4).
2. **HEAD-CLEAR WAIVER** (style != 3, and only while D_00810690 — the
   commit's desired horiz eye↔target distance, 1 frame stale — ≤
   fabsf(cam+0x0C) = 46.8 live): re-probe from (target.x, player.y +
   **17.5** [13.0 if cam+0x5C == 1.0], target.z) to the extended eye.
   CLEAR → the block is **waived** (low/waist-high walls never move the
   camera); a ceiling-class hit within 1 u of the eye also waives.
3. **FIRST-STAGE RESPONSE** (still blocked):
   - ceiling-class: eye = hit, y −= 1 (duck under), x/z += 0.5·sightdir;
     widen carried Y bounds; result bit 8.
   - wall: REVERSE probe ext-eye → target. Wall hit whose normal does
     NOT oppose the first (dot > −0.3, 0xBE99999A) = eye buried in a
     wedge → eye x/z = reverse hit + **4.0**·reverse-normal.
   - otherwise **PULL-IN: eye x/z = hit + 0.5·sightdir, HEIGHT KEPT**
     (sets NO result bits — a plain wall block returns 0). **This is
     the user-observed "rise": the eye keeps its absolute height
     (~19 u over ground) while the horizontal distance collapses, so
     the camera ends up over the player's head looking down**; the
     commit's +4·forward near-push carries the viewpoint off the wall.
     "Returns when clear" because the mode handler re-poses the desired
     eye from scratch every frame.
4. **SIDE STAGE** (clear or glancing only): probes **5.5 u**
   perpendicular to the sight line, both sides (glancing variant: start
   3.0 on the far side, end 1.5 toward the target + 5.5 near side;
   rejects: same-wall dot < −0.998 [0xBF7F7CEE], ceiling-case cross dot
   < −0.08 [0xBDA3D70A], hit within 1 u of the probe end; a confirm
   probe re-tests from the first hit point at eye height). Valid hit →
   candidate eye = (hit ∓ side vector) + 0.1·ray dir, i.e. ~5.4 u of
   lateral clearance [bits 2 left / 4 right / 8 ceiling-underside keeps
   the candidate's own Y / 1 other-class]; non-glancing response only
   for validation dot in (−0.3, 0.9). BOTH sides valid → eye x/z =
   midpoint of the two hit points (corridor centering). Engine quirks
   kept: the right side's ceiling-underside / other-class cases set
   bits 8/1 without 4, so those candidates are computed but never
   applied.
5. **FINAL Y POLICY + BOUNDS**: blocked by a NON-wall class → ceiling:
   eye.y = min(eye.y, hit.y − 1); floor-class: eye.y = **max(eye.y,
   hit.y + 1)** — the only literal rise in the solver (1 u over
   floor-class blockers). Clamp eye.y into the carried bounds cam+0x50/
   +0x54 (init −200/1000), then RE-PROBE them: from the eye pulled 1.5
   toward player+(0,11,0), 200 down (class 0x7000) → lower = floor +
   **17.0** [6.0 if cam+0x5C==1]; 200 up (0x8800) → upper = ceiling − 1,
   else eye + 200; lower forced to upper − 3 if crossed. Clamp again →
   bits **0x40** (floor-clamped) / **0x80** (ceiling-clamped). The eye
   can never sink below 17 u over the floor beneath it.
   Per-area specials: area 0x12 clamps eye.z to [169.5, 230.6] and has
   an alternate upward-bound probe from player+0xB0 (normal-down dot ≤
   0.2 → open); area 0x15 with eye.z > 260 forces lower bound 70.
6. Returns the bit byte → cam+0x07. **Idle auto-orient ties in** (s46):
   the idle timer resets on `+0x07 & 9` — bit 8 = ceiling involvement,
   bit 1 = side other-class; a plain wall pull-in (0) does NOT reset it.

Style 3 (cinematic) differences: skips steps 2/4's full shape — on any
0xD800-class hit it min/maxes cam+0x50/+0x54 with the hit Y (bits
8/0x10) and pulls the eye to hit + 0.5·sightdir on ALL THREE axes; a
plain wall pulls x/z only.

### Port verification (extermination-port, this session)

- `cam_solver_0018DD20` translated verbatim (style 0, mask 6); aim keeps
  its flagged pull-in stand-in for the unread func_0018F870 (now run
  over mask 7).
- EM_CAPTURE_RISE + EM_CAMERA_TRACE (office scene): running at the
  camera parks the desired eye at wall + 0.5 (z −250.5), height pinned
  19.00 across 300+ blocked frames while the target closes to 5 u →
  the frame-320 capture looks down at the top of the player's head —
  the observed behavior EMERGES from the decoded code. The new release
  phase (frames 360+: walk away) shows the block ceasing the frame the
  desired eye re-enters the room and the chase returning to full
  distance/height — "returns when clear" emerges too.
- Default EM_CAPTURE byte-identical vs HEAD (solver quiescent mid-room,
  as it must be: bounds floor+17 < eye 19 < ceiling−1, side walls > 5.5).
- Self-tests: input, weapon, move, door, proj, aim, camregion, transit,
  pause, melee, sfx, death — all PASS with the staged change (slider/
  locked FAIL pre-date this work at clean HEAD; enemy-test churn is the
  parallel enemy session's in-flight work).

## ENEMY CONDITION DECODE — crawler/crate, worm, leech triggers read off the disassembly (2026-06-11, session 62)

Static instruction-level read of `func_001551B0` (placed crawler/crate),
`func_00153F10` (worm brain) and `func_00154120` (leech sub-machine) from
the local splat `.s` — every wake/burst/damage condition below is the
literal branch logic, not inference. Refines and partially supersedes the
s22 "ENEMY AI ARCHITECTURE" section; written to kill the port's invented
constants (PORT_DIFFERENCES item 6 / rows J1–J4, J7 — now updated).

### 1. func_001551B0 (placed crawler / crate) — exact conditions

- **NO PROXIMITY SENSE EXISTS.** The whole function contains zero
  player-position reads: no `func_0019AA80`/`func_0019A570` call, no
  `D_00810350/58`, no `D_008102B0`. State 4 cannot wake on approach and
  state 1 cannot aim at or contact-test the player. The s22 open framing
  ("state-4 wake unpinned") is closed: damage and the group alarm are
  the ONLY triggers, full stop.
- **State 4 IDLE, in order each tick:**
  1. `+0x36 != 0` → event byte `+0x00 = 2`, state = 2 (HP is not even
     read — any nonzero kills), then the **alarm broadcast**: walk
     `D_00275BC0` (next `+0x1C`) and set `+0x0A = 1` on every actor with
     model byte ∈ {6, 0x1C, 0x1E, 0x1F, 0x50} AND `+0x52` (on-surface)
     nonzero — the WHOLE list, **no radius/distance term**; own `+0x0A`
     cleared. `+0x36` is NOT cleared here (state 2 reads it for the
     knockback arm).
  2. else own `+0x0A` set → clear it, `+0x2A = 6`, state = 1.
  3. else the disguise jitter (gated on placement flag `+0x0E` bit 0;
     the jitter runs in state 4 ONLY — an alarmed crate hops instead).
- **State 1 ATTACK is BLIND and never polls damage.** Sub 0 steer:
  `+0x2A--`; probe the 4 precomputed diagonals (`func_0019AB20`, mode 7;
  blocked = result 2 + spad `0x700031D4` nonzero); ≥3 blocked or both
  OPPOSITE diagonal pairs blocked → hold, and at `+0x2A == 0` → **back
  to state 4** (a pending `+0x36` then kills it on the next idle tick —
  mid-attack damage DEFERS). Else rotate the heading ±0.0524 rad
  (0x3D56774F) away from a blocked front side, or RNG-perturb the
  velocity components by `(rand/2^31 − 0.5)/60` (≈ ±1/120 rad) when
  open; hop timer `+0x28 = 30`; attack timer `+0x2A = 180` (0xB4) —
  variant 6 instead `+0x2A = pos.y × 60/12` (5× its height). Sub 0 runs
  ONCE: sub 1 never returns to it (one long leap, velocity 11.0-scaled
  / 1.4-normalized components, gravity decrement 0x3D54FDF4 =
  0.0519999). Sub 1 burst condition: forward probe result 4 (surface
  lost) OR `+0x2A < 0` → **`sh zero, 0x36` (mid-run damage is
  ABSORBED), `+0x2A = 0`, state = 2.** That store is the ONLY `+0x36`
  access in the entire attack state — the s22 "appears undamageable
  mid-lunge — verify live" open item is closed statically.
- **State 2 sub 0:** nest-child spawn gated on `+0x56 >= 0` (the LINK
  field; at INIT the registry resolve additionally needs `+0x0E` bit 0,
  and clears that bit if no child record's model is resident);
  registry = `D_0024A850[area]` (+1 if 0) + link → `D_0024D820[area]`
  table → 0x2C-byte records (terminator rec[0] == −1), each
  resident-filtered (`func_001B11E0(rec+0x2)`), alloc'd
  (`func_001AFA90(rec[4])`) and copied — model `+0x3`, `+0x2E` =
  rec[6]>>8, kind/link `+0x54/+0x56`, pos += parent, rot, **behavior
  `rec+0x28` → `+0x10`**. Then per-variant gore (6: 0x19D +
  0x8000000A/0x80000015; 0x1C/0x50: 0x19E + 0x8000000B/0x80000014;
  0x1E: 0x19D + 0x80000031/0x80000015; 0x1F: 0x19E +
  0x80000032/0x80000014). Knockback corpse-slide arm only when
  `+0x36 != 0` AND variant ∈ {6, 0x1E} (RNG 0/90/180/270° rotation of
  the D_700036E0 hit vector); otherwise straight to state 3.
- **INIT:** HP `+0x34 = 1`, `+0x00 = 1`, alarm cleared; base heading
  4.5962 (0x40931406 — variants 6/0x1E/default) or 2.1213 (0x4007C3B6 —
  0x1C/0x1F/0x50); 4 diagonal probe points = pos ± the rotated heading
  vector (90° steps); floor probe (target y−2, dir −3, mode 7): result
  4 → `+0x52 = 0` else 1.

### 2. func_00153F10 / func_00154040 (worm) — acquisition is unconditional

- **No idle state, no alarm read, no mailbox read.** State 0 init
  (`func_00154040`): bind rig 0x14 / anim 0x13, **HP `+0x34` = 10**,
  yaw = atan2 toward `(D_00810350, D_00810358)` (the player mirror),
  sound 0x430 → state 1. The worm is BORN ATTACKING — "target
  acquisition" is the init yaw plus the stalk homing; there is no
  distance gate of any kind.
- State 1 runs the brain only when spad `0x70003B8D` ∈ {0, 4} (the
  scripted/pause gate — the port's menu pause is the equivalent).
- State 2 burst: sub 0 = sound 0x434 (`func_001FBD50`, radius 300.0) +
  gore `func_001EFE00(0x80000052)` + call `+0x4C`; subs 1/2 are two
  linger ticks → state 3 → `func_001AFC10` release.
- **`+0x36`/`+0x34` are never read by the brain** — how a worm dies to
  weapons is still an OPEN ITEM (some unfound handler must consume the
  HP; verify live). The port keeps worms shootable via the canonical
  hurt-helper shape as a flagged stand-in.

### 3. func_00154120 (leech sub-machine) — the lunge, verified

Per tick: `anim_advance_time(rate 1.0)` first, then sub `+0x05`:

- **sub 0 APPROACH** — `func_0019AA80(slotA+0xC0, slotB+0xC0, 0x20)`:
  the 32-u radius test **VERIFIED** ($a2 = 0x20; slots `+0x34`/`+0x40`
  of `D_00275B40` — node identities still unverified). On hit AND
  player status `D_008102B0 == 1`: **latch** — `D_008102BF = 2`,
  `D_008104D4 = 5.0` (0x40A00000), status `|= 2`, relative vector →
  `D_00810320`. No state change — the latch is one-shot by the
  status==1 gate. Sub advances on the anim-done bit 0x1000 →
  `func_00153ED0` (next anim), sub 1, `+0x28 = 0x78` (120).
- **sub 1 STALK** — `+0x28--`; at 0 → next anim, sub 2. Every tick:
  homing — `func_001B1240` atan2 toward the player mirror, smoothed by
  `func_001B12B0` at **0.069813 rad/tick (0x3D8EFA35)** into `+0xC4`.
- **sub 2 WINDUP** — on anim end: next anim, **SNAP** `+0xC4` =
  atan2(player) (no smoothing), sound **0x431**, sub 3.
- **sub 3 LUNGE RESOLVE**, every tick of the lunge anim:
  1. the same 32-u node test → **state 2 burst**, and if status == 1
     also latch with **`D_008104D4 = 15.0`** (0x41700000 — "the lunge
     hurts more");
  2. else `func_0019A570(slotA+0xC0, slotB+0xC0, 6, 0)` — **radius-6
     contact** → state 2 burst (NO latch write on this arm);
  3. else anim done → **state 3 despawn** (released — no burst, no
     gore, no corpse).
- Post: `func_001C68C0`, `func_001B17A0`, `+0x4C` — every tick.

### 4. Port adoption (extermination-port, this session)

`em_enemy.c/h` now run the decoded machines: KIND_CRATE =
func_001551B0 (damage-only burst + list-wide alarm + the blind
suicide hop, exact mailbox windows incl. deferral/absorption),
KIND_CRAWLER = the worm (born attacking; approach/stalk/windup/lunge
with the decoded 120-t stalk, 0.0698 homing, yaw snap, 21.27 u/s
lunge, miss-despawn; lunge connect posts the decoded latch 15).
REMOVED inventions: the 32-u distance wake, the ~10-u crate proximity
burst, the 0x400A/10 lunge damage, the IDLE+ATTACK poll widening.
Remaining flagged stand-ins: worm shootability (open item above), the
latch/shake-off mechanic + 5.0 approach latch (untranslated), the
32-u arm folded into the radius-6 contact (node identities), ground
speeds 0.32 u/f, sub-0/2/3 windows = clip lengths 90/45/120 (the
engine anim-gates them; the bound-anim id chain is unverified), the
re-steering repeated hop (engine: one leap). Evidence:
EM_ENEMY_TEST 1–5, EM_MELEE_TEST (now also the no-radius broadcast
witness + the alarmed-crate timer burst) and EM_DEATH_TEST all PASS;
the default EM_CAPTURE is byte-identical vs HEAD.

_Last updated: 2026-06-11 (sessions 61–62)._
