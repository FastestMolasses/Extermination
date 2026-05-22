# Extermination Decomp — Progress

Living status document for the Extermination (PS2) decompilation / modding
project. **Keep this current** — update it whenever a milestone is reached, a
finding changes, or the roadmap shifts. With `docs/FINDINGS.md` it is the entry
point for anyone (a person or an agent) picking up the project.

_Last updated: 2026-05-22 (Track A pipeline complete — first 100% match)_

## Project at a glance

- Goal: a matching decompilation of *Extermination* (PS2, **SCUS-97112**) and,
  long-term, a clean native PC port that is moddable/extensible. See `CLAUDE.md`
  for the full charter and the non-negotiable legal rules.
- Two tracks:
  - **Track A — matching decompilation** of the game code. In progress — the
    build pipeline is complete and the first function is a 100% match.
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

### Done — Track A matching-decomp pipeline (the "hello world")

The decomp pipeline is stood up end to end and the first function is a verified
100% match. The M1 runs objdiff, splat and editing; an arm64 Linux container
runs the period-correct compiler.

- **Boot ELF** `SCUS_971.12` extracted from the user's ISO to
  `config/SCUS_971.12` (git-ignored); SHA1 verified. Entry `0x00100008`, `$gp`
  `0x0027D370`, one loadable PROGBITS section (file `0x300..0x175E00` → vram
  `0x00100000`).
- **splat** (0.40.1, in `.venv/`) disassembles the ELF via
  `config/SCUS_971.12.yaml` → 3015 per-function `.s` files in `build/`
  (git-ignored — disassembly of the original binary). 957 are leaf functions.
- **objdiff** v3.7.1 (native arm64 GUI + CLI) in `tools/bin/` (git-ignored).
- **Toolchain container** `exterm-toolchain` (arm64; Apple `container` CLI;
  `docker/Dockerfile`): Debian + `qemu-user` + MIPS binutils. The compiler
  `mwccmips.exe` (Metrowerks CodeWarrior 2.3.1.01 — the exact build named in the
  boot ELF's `.comment`) is a **32-bit Windows PE**. Apple's Rosetta and
  qemu-x86_64 user-mode both fail on 32-bit x86, so it runs under a genuine
  **32-bit wibo** (`tools/bin/wibo32`, cross-built by `docker/build-wibo.sh`)
  inside **qemu-i386**. The MIPS assembler is arm64-native — only the compiler
  is emulated.
- **First match — `func_001098D8`** (`return 1`): `src/func_001098D8.c`
  compiles to a **100% `.text` match** vs the original, confirmed by
  `objdiff-cli`. The pipeline is proven end to end.

Build flow (`tools/decomp/build.py`): `setup` runs splat + writes
`objdiff.json`; `build` assembles the splat disassembly into objdiff *target*
objects and compiles `src/*.c` into *base* objects via mwccmips; objdiff (or
`objdiff-cli`) diffs the two. The MIPS assembler needs `config/asm_prelude.inc`
(`.set noreorder`) so splat's literal delay slots assemble correctly.

Compiler source & legality: `mwccmips.exe` came from the public
`Adubbz/DCDecomp` repo (a Dark Cloud PS2 decomp using the identical compiler).
It is proprietary Metrowerks software — it lives in `tools/mwccps2/` and is
**never committed** (git-ignored).

Scouted near-match candidates for the next functions: `func_001031E0` (vec3
copy — 6 of 7 instructions, one FPU register off) and `func_00102638` (integer
bit op — a v0/v1 allocation swap). Closing these is compiler-flag / C-source
tuning — the normal substance of matching work.

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
as-is). The pipeline "hello world" is **done** — see "Done — Track A" above:
splat + objdiff + the mwccmips toolchain container all work, and
`func_001098D8` is a verified 100% match. Next: decompile and match more leaf
functions (`func_001031E0` / `func_00102638` are scouted and close), grow
`config/symbol_addrs.txt`, and work outward toward a partial runnable ELF.
The reference template studied for the pipeline is `fmil95/recvx-decomp`
(same CodeWarrior toolchain family).

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
  - `decomp/build.py` — Track A build driver (splat, compile, objdiff.json).
- `config/` — Track A build config (committed): `SCUS_971.12.yaml` (splat
  config), `symbol_addrs.txt` (hand-recovered symbol list), `asm_prelude.inc`
  (assembler directives for target objects). The boot ELF `config/SCUS_971.12`
  itself is git-ignored, as is the proprietary compiler under `tools/mwccps2/`.
- `src/`, `include/` — decompiled C and headers (committed).
- `docker/` — toolchain container (committed): `Dockerfile` (arm64 image: Debian
  + qemu-user + MIPS binutils, built with Apple's `container` CLI) and
  `build-wibo.sh` (cross-builds the 32-bit wibo).
- `docs/` — project state and findings (committed): `PROGRESS.md` (this file),
  `FINDINGS.md` (technical format reference), `track-a-kickoff.md` (Track A
  starter prompt).
- `CLAUDE.md` — project charter, legal rules, target identity, toolchain
  conventions, end-state build architecture.
- Disc-derived material is **git-ignored** — the ISO, `extract/`, `wav/`,
  `voice/`, `music/`, `textures/`, `subtextures/`, `models/`, the boot ELF, and
  the Track A build artifacts (`build/` splat disassembly, `elf/`, `.venv/`,
  `tools/bin/`). Each user regenerates these locally from their own legal disc.

## Maintaining the docs

- `docs/PROGRESS.md` (this file) — status, open questions, roadmap.
- `docs/FINDINGS.md` — technical reference for every reverse-engineered format.
- Update both in the same session as work progresses, so the next person or
  agent can continue cleanly (required by `CLAUDE.md`).
