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
  machinery and embeds native-size RGBA8 textures. **EMDL v2** ("EMD2"):
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
(only mode 0 traced). Collision-solver internals (func_0018DD20) — medium
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

_Last updated: 2026-06-10 (session 11)._
