# Extermination Decomp — Progress

Living status document for the Extermination (PS2) decompilation / modding
project. **Keep this current** — update it whenever a milestone is reached, a
finding changes, or the roadmap shifts. It is the entry point for anyone (a
person or an agent) picking up the project.

_Last updated: 2026-05-22_

## Project at a glance

- Goal: a matching decompilation of *Extermination* (PS2, **SCUS-97112**) and,
  long-term, a clean native PC port. See `CLAUDE.md` for the full charter and
  the non-negotiable legal rules.
- Two parallel tracks:
  - **Track A — matching decomp** of the game code. Not started yet.
  - **Track B — asset-format RE / modding tools.** Active. Delivers modding
    value early, before the code is matched.
- Current focus: Track B (asset formats).

## Status

### Done
- **Boot ELF characterized** — `SCUS_971.12`, Metrowerks CodeWarrior, no DWARF
  / no symbols (hardest matching tier). Details in `CLAUDE.md` "Target identity".
- **`DATA.DAT` / `INDEX.IDX` archive** fully reverse-engineered and validated
  (exact tiling). Extractor: `tools/extract_data.py`. 81 regions / 603 files,
  byte-exact, 100% coverage.
- **Asset taxonomy** mapped — 603 files classified by content.
- **SFX** — SShd sound banks decoded to WAV. `tools/decode_sound.py batch`.
  1146 references deduplicated to 241 unique sounds.
- **Dialogue** — `STREAM/VOICE.DAT` decoded: 116 mono clips.
- **Music** — `STREAM/MUSIC.DAT` decoded: 55 stereo tracks (interleaved VAG).
- **Textures** — pixel data decoded. GS texture-upload packets are 8-bit
  (PSMT8) images; `tools/extract_textures.py` finds 57 packets (28 standalone
  UI/common + ~29 embedded in `id 0x44` level files) and writes grayscale
  PNGs via the authoritative GS swizzle (proven correct). Open: the **color
  source** is unresolved — PSMT8 needs a CLUT, none was found in the data;
  it's either a grayscale ramp (color from vertex modulation) or an unlocated
  color CLUT. See the open questions.
- **Geometry / models** — **level geometry AND character/object/prop models
  done.** Both the `id 0x44` level-file format and the model-file variant are
  reverse-engineered: `tools/extract_models.py` exports both to Wavefront OBJ.
  Level files are separator-delimited blocks (MESH / SUBMESH / MATRIX /
  FILLER); model files use the same 64-byte vertex record but fixed-size
  padded blocks anchored by the MESH descriptor (with duplicated-record tail
  padding trimmed). A full `extract/` run exports **32 level + 328 model** OBJ
  files (≈1.18 M verts, ≈684 K triangles; 243 non-geometry files skipped).
  Verified: zero out-of-range faces, model-sized bounding boxes for model
  files, level result unchanged (19271 triangles). `docs/FINDINGS.md` has the
  format detail. **MATRIX instance transforms now decoded** — the level-file
  MATRIX blocks are a transform table plus the object-space geometry it
  instances; `extract_models.py --scene` bakes them and exports placed
  full-level OBJ scenes (32 levels, ≈331 K verts, zero outliers; default
  per-mesh export byte-identical and unchanged). Remaining geometry work:
  the skinning rig — **now located** in separate files and partially
  reversed (`--rig` / `--anim` modes; see `docs/FINDINGS.md`) — and
  material→texture binding.

### Open questions / to verify
- **Sample rate.** Streamed audio (VOICE/MUSIC) = **48000 Hz** — strong
  evidence (End Credits matches an official-soundtrack rip's duration). SFX
  bank rate is still unconfirmed (decoded at 22050; may also be 48000).
  Re-confirm both from the decompiled audio engine.
- **Audio clip splitting** is heuristic (silence gaps) — no per-clip index was
  found for `VOICE.DAT` / `MUSIC.DAT`. An index may live in game code/overlays.
- **Embedded textures** — a few of the ~29 embedded-packet results look noisy
  (likely false-positive `07../60` signature matches in geometry data); the 28
  standalone sheets are the solid set. `OVERLAY/` not yet scanned for textures.
- **Texture color source.** PSMT8 needs a CLUT, but none was found in the
  game data. Either the runtime CLUT is a grayscale ramp (color comes from
  renderer vertex-color modulation) or there is an unlocated color CLUT. The
  decompiled draw code (GS `TEX0` setup) will resolve this.
- **Cross-file texture residency** — per-texture extraction works
  (`tools/extract_subtextures.py`, 2274 PNGs), but ~631 materials reference a
  texture uploaded by a *different* file (common/UI packets) not in the level
  dir. Resolving them needs the engine's cross-file VRAM map.
- **`extract_textures.py` misses `07../10` packets** — its signature only
  matches `07 XX 00 60`, so it never extracted the `07../10` DMA-tag level
  sheets. `extract_subtextures.py` handles them; `extract_textures.py` should
  be broadened (or the two tools consolidated).

### Next steps (roadmap)
- Separate the 55 `MUSIC.DAT` tracks: **25 are the official soundtrack, the
  other 30 are cutscene audio** (per user, cross-referenced with an online
  soundtrack listing). Not yet labelled/split.
- **Geometry / models** — level geometry, character/object/prop models, and
  the level-file MATRIX instance transforms are all decoded
  (`extract_models.py`; `--scene` exports placed full-level OBJ scenes).
  Remaining: (a) **skinning / animation** — the rig is located (separate
  0x78-byte-record files; plus per-frame vertex-animation pose sets), exposed
  by `--rig`/`--anim`; still to do is the VIF-packed transform payload decode
  (needs VU1 microcode) and the bone parent hierarchy; (b) confirm the MATRIX
  `--scene` open questions from the engine code — the role of repeated identity
  transforms and whether transforms are absolute or composed with a parent
  node (see `docs/FINDINGS.md`).
- **Per-texture extraction — done.** The strip marker→texture binding is
  cracked (`m0` carries a `sheet_field` GS VRAM address); `extract_subtextures.py`
  crops per-material textures (2274 PNGs). Remaining: the ~631 cross-file-bound
  materials (see open questions) and broadening `extract_textures.py` to
  `07../10` packets.
- **Asset repackers + moddable build** — the end goal is that a user builds the
  game from loose extracted assets (see `CLAUDE.md` "End-state build
  architecture"). Each `extract_*` tool needs a matching repacker that rebuilds
  the original container (`DATA.DAT`/`INDEX.IDX`, the streams) **byte-identically**
  from the extracted files. The archive format is reversed precisely enough
  (exact tiling) that byte-identical repacking is feasible. Note: the matching
  build repacks the *raw* extracted files; the human-friendly views (WAV/PNG/OBJ)
  are for inspection/modding and need re-encoders to put edits back.
- Stand up **Track A**: clone a reference template (`fmil95/recvx-decomp` —
  also CodeWarrior), install objdiff (native arm64), set up splat, build the
  x86_64-Linux `mwccps2` + wibo container, get one leaf function to 100%.

## Repo layout

- `tools/` — original tooling (committed).
  - `extract_data.py` — `DATA.DAT`/`INDEX.IDX` archive extractor.
  - `decode_sound.py` — VAG ADPCM decoder (SFX banks + VOICE/MUSIC streams).
  - `extract_textures.py` — GS texture-packet extractor (8-bit, grayscale).
  - `extract_models.py` — geometry → Wavefront OBJ (level + character/object
    models); `--scene` places full levels; `--rig`/`--anim` dump rig & poses.
  - `extract_subtextures.py` — per-material texture extraction (marker→packet
    binding + UV crop).
- `docs/` — this folder: project state and findings (committed).
- Disc-derived outputs (`extract/`, `wav/`, `voice/`, `music/`, `iso/`, …) are
  git-ignored — each user regenerates them locally from their own disc.

## Maintaining this folder

- `docs/PROGRESS.md` (this file) — status, open questions, roadmap.
- `docs/FINDINGS.md` — technical reference for reverse-engineered formats.

Update both as work progresses so the next person/agent can continue cleanly.
