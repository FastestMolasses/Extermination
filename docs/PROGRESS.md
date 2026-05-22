# Extermination Decomp — Progress

Living status document for the Extermination (PS2) decompilation / modding
project. **Keep this current** — update it whenever a milestone is reached, a
finding changes, or the roadmap shifts. With `docs/FINDINGS.md` it is the entry
point for anyone (a person or an agent) picking up the project.

_Last updated: 2026-05-22_

## Project at a glance

- Goal: a matching decompilation of *Extermination* (PS2, **SCUS-97112**) and,
  long-term, a clean native PC port that is moddable/extensible. See `CLAUDE.md`
  for the full charter and the non-negotiable legal rules.
- Two tracks:
  - **Track A — matching decompilation** of the game code. Not started.
  - **Track B — asset-format RE / modding tools.** Broadly complete.
- **Current focus: Track A.** The near-term goal is a runnable, developer-style
  build — the decompiled code compiles and runs with the game's own assets, the
  way you would if developing the game. Asset *repacking* is explicitly
  deferred to late in the project (it is not needed for the dev loop — the
  original `DATA.DAT` is used as-is).

## Status

### Done — Track B asset extraction

All asset extractors are committed in `tools/` (original code, Python 3 stdlib,
native arm64). Disc-derived *output* is git-ignored. `docs/FINDINGS.md` has the
full format detail for everything below.

- **Boot ELF characterized** — `SCUS_971.12`, Metrowerks CodeWarrior (`mwccps2`
  2.3.1.01), **no DWARF / no symbols** (hardest matching tier). Pinned in
  `CLAUDE.md` "Target identity".
- **`DATA.DAT` / `INDEX.IDX` archive** — `tools/extract_data.py`. Reverse-
  engineered and validated by exact tiling: 81 regions / 603 files, byte-exact,
  100% coverage. Asset taxonomy mapped (603 files by content).
- **Audio** — `tools/decode_sound.py`. All PS2 VAG ADPCM.
  - SFX: SShd sound banks → 241 unique sounds (deduped from 1146 references).
  - Dialogue: `STREAM/VOICE.DAT` → 116 mono clips.
  - Music: `STREAM/MUSIC.DAT` → 55 stereo tracks (interleaved VAG).
  - Streamed audio is 48000 Hz (End Credits matches an official-soundtrack rip).
- **Textures** — `tools/extract_textures.py` (sheet extractor) and
  `tools/extract_subtextures.py` (per-material). 8-bit PSMT8, decoded with the
  authoritative GS swizzle (proven correct). The geometry marker→texture
  binding is cracked; 2274 per-material PNGs export. Output is grayscale (color
  source unresolved — see open questions).
- **Geometry & models** — `tools/extract_models.py`. Level geometry (`id 0x44`
  files) and ~328 character/object/prop models → Wavefront OBJ. Modes:
  `--scene` (apply MATRIX instance transforms → placed full-level scenes),
  `--rig` (dump the skeleton-transform files), `--anim` (export vertex-
  animation pose sequences).

### Open questions (most need the decompiled engine — i.e. Track A)

- **Texture color source.** PSMT8 always samples through a CLUT, but no CLUT
  data was found in `DATA.DAT` or the boot ELF; the 8-bit values are luminance-
  ordered. Either a grayscale-ramp CLUT (color from vertex-color modulation) or
  an unlocated color CLUT. The engine's GS `TEX0` setup will resolve it.
- **Cross-file texture residency.** ~631 materials bind to a texture uploaded
  by a different file (common/UI packets); needs the engine's VRAM map.
- **`extract_textures.py`** only matches `07 XX 00 60` packets, missing the
  `07../10` DMA-tag level sheets (`extract_subtextures.py` handles them);
  broaden or consolidate the two tools.
- **Rig payload.** The 112-byte per-record rig transform is VIF-packed; a
  faithful decode needs the VU1 microcode. Bone parent hierarchy not isolated.
- **MATRIX `--scene`.** Role of the repeated identity transforms, and whether
  transforms are absolute or parent-composed — confirm from engine code.
- **Audio.** SFX-bank rate unconfirmed (provisionally 22050). Clip splitting is
  heuristic (silence gaps) — no per-clip index found.
- **`OVERLAY/`** (`AREA*.BIN`, `MWo3` overlay modules) not yet scanned for
  textures/geometry.

### Roadmap

**1. Track A — matching decompilation (CURRENT PRIORITY).**
Goal: a runnable developer build — compile the decompiled code and run the game
with its own assets (no repacking needed; the original `DATA.DAT` is used
as-is). First milestone — the pipeline "hello world":
clone a CodeWarrior reference template (`fmil95/recvx-decomp`), install objdiff
(native arm64 macOS), set up splat, build the x86_64-Linux `mwccps2` + wibo
compiler container, and drive one trivial leaf function to a 100% objdiff
match. See `CLAUDE.md` for toolchain conventions and the environment (M1 macOS
primary; x86_64 Linux available). A ready-to-paste kickoff prompt for a fresh
session is in `docs/track-a-kickoff.md`.

**2. Asset polish (minor; several items need Track A's decompiled engine).**
The open questions above — texture color/CLUT, cross-file texture residency,
`extract_textures.py` `07../10` coverage, rig VIF payload + bone hierarchy,
MATRIX `--scene` confirmation, SFX sample rate, labelling the 25 soundtrack vs
30 cutscene `MUSIC.DAT` tracks, scanning `OVERLAY/`.

**3. Asset repackers + moddable build (DEFERRED to late in the project).**
Repackers that rebuild `DATA.DAT`/`INDEX.IDX` and the streams byte-identically
from loose extracted files, so the game can be rebuilt from editable assets.
The archive is reversed precisely enough (exact tiling) that byte-identical
repacking is feasible. Not needed for the dev loop. See `CLAUDE.md` "End-state
build architecture".

## Repo layout

- `tools/` — original tooling (committed; Python 3 stdlib, native arm64):
  - `extract_data.py` — `DATA.DAT`/`INDEX.IDX` archive extractor.
  - `decode_sound.py` — VAG ADPCM decoder (SFX banks + VOICE/MUSIC streams).
  - `extract_textures.py` — GS texture-packet → grayscale PNG sheet extractor.
  - `extract_subtextures.py` — per-material texture extraction (marker→packet
    binding + UV crop).
  - `extract_models.py` — geometry → Wavefront OBJ (level + models); `--scene`
    places full levels, `--rig`/`--anim` dump rig & poses.
- `docs/` — project state and findings (committed): `PROGRESS.md` (this file),
  `FINDINGS.md` (technical format reference), `track-a-kickoff.md` (Track A
  starter prompt).
- `CLAUDE.md` — project charter, legal rules, target identity, toolchain
  conventions, end-state build architecture.
- Disc-derived material is **git-ignored** — the ISO, `extract/`, `wav/`,
  `voice/`, `music/`, `textures/`, `subtextures/`, `models/`, etc. Each user
  regenerates these locally from their own legal disc.

## Maintaining the docs

- `docs/PROGRESS.md` (this file) — status, open questions, roadmap.
- `docs/FINDINGS.md` — technical reference for every reverse-engineered format.
- Update both in the same session as work progresses, so the next person or
  agent can continue cleanly (required by `CLAUDE.md`).
