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
- **Geometry / models** — **level geometry done; character/object models
  pending.** The `id 0x44` level-file format is reverse-engineered:
  `tools/extract_models.py` exports it to Wavefront OBJ (block-structured —
  MESH / SUBMESH / MATRIX / FILLER blocks; triangle-strip 64-byte vertex
  records). All 32 `id 0x44` files export to valid OBJ (verified: 19271 clean
  triangles in the largest, zero degenerate faces). **But the same MESH-format
  signature appears in ~330 other files** (many file ids) — almost certainly
  the character / enemy / prop models — and the tool does not yet decode them
  (format variants). See the open questions. `docs/FINDINGS.md` has detail.

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
- **Per-texture extraction** — the decoded sheets are correct but packed
  atlases; clean individual textures need UV rectangles from geometry data.
  Now unblocked: `extract_models.py` recovers per-vertex UVs; the remaining
  gap is binding a strip's marker key (m0/m1) to a specific texture packet.

### Next steps (roadmap)
- **Per-texture extraction** — the decoded texture sheets are correct but are
  packed atlases. Cutting them into clean individual textures needs each
  texture's UV rectangle/orientation from the geometry/draw data (so it
  depends on the geometry track below).
- Separate the 55 `MUSIC.DAT` tracks: **25 are the official soundtrack, the
  other 30 are cutscene audio** (per user, cross-referenced with an online
  soundtrack listing). Not yet labelled/split.
- **Geometry / models** — level geometry is decoded (`extract_models.py`).
  Remaining: (a) **character / object / prop models** — ~330 more files carry
  the MESH signature in undecoded format variant(s); adapt the tool to them;
  (b) strip marker → texture-packet binding (enables per-texture extraction);
  (c) MATRIX instance transforms (placed full-level scenes; also addresses the
  "missing pieces" — instanced geometry is currently exported once, unplaced).
- Stand up **Track A**: clone a reference template (`fmil95/recvx-decomp` —
  also CodeWarrior), install objdiff (native arm64), set up splat, build the
  x86_64-Linux `mwccps2` + wibo container, get one leaf function to 100%.

## Repo layout

- `tools/` — original tooling (committed).
  - `extract_data.py` — `DATA.DAT`/`INDEX.IDX` archive extractor.
  - `decode_sound.py` — VAG ADPCM decoder (SFX banks + VOICE/MUSIC streams).
  - `extract_textures.py` — GS texture-packet extractor (8-bit, grayscale).
  - `extract_models.py` — `id 0x44` geometry → Wavefront OBJ exporter.
- `docs/` — this folder: project state and findings (committed).
- Disc-derived outputs (`extract/`, `wav/`, `voice/`, `music/`, `iso/`, …) are
  git-ignored — each user regenerates them locally from their own disc.

## Maintaining this folder

- `docs/PROGRESS.md` (this file) — status, open questions, roadmap.
- `docs/FINDINGS.md` — technical reference for reverse-engineered formats.

Update both as work progresses so the next person/agent can continue cleanly.
