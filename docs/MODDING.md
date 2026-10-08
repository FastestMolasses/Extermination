# Modding Extermination — The Current Surface

What you can actually change today, and with which tools. Three surfaces:

1. **Code modding** — edit matched C in `src/`, rebuild the boot ELF, repack
   your ISO, run in PCSX2. Current coverage: `tools/verify_all.py` (match stage)
   and `docs/PROGRESS.md`.
2. **Asset modding via the native port** — export disc assets into our own
   open interchange formats (EMDL/EMCL/EMFN/EMUI), compose scenes with a
   plain-text manifest, and run them natively in the sibling
   `extermination-port/` (macOS Cocoa+Metal, zero dependencies).
3. **PS2 archive modding** — `python3 -m tools.repack` unpacks the user's
   disc into a lossless ISO tree and correctly labelled DATA/INDEX loose
   files, then rebuilds both containers and the ISO. See [REPACK.md](REPACK.md)
   for source builds, PNG/WAV/JSON editing, byte-sized growth, new resident
   entries, and format limits.

The repacker supports reversible textures, audio samples and message tables.
Archive alignment padding is rebuilt around exact edited payload lengths.
EMDL/EMCL/glTF conversion back to native model packets and cue-stream editing
remain future work; opaque files and stream movies are preserved.

**Legal frame (CLAUDE.md hard rules):** every exporter ingests only the
user's own disc dump / PCSX2 save states; all outputs land in git-ignored
directories (`extract/`, `assets/`, `models/`, `wav/`, …). Nothing
disc-derived is ever committed or redistributed.

---

## Quick PS2 asset-mod walkthrough

Run from the repack checkout on **native arm64 macOS**. The editors use Python;
the final source build needs the project's installed Apple container toolchain.
All disc-derived material remains local under ignored `build/repack/`.

```sh
export PYTHONDONTWRITEBYTECODE=1
python3 -m tools.repack unpack-disc \
  --iso '/Users/abe/Documents/PS2 Games/Extermination (USA).iso' \
  --out build/repack/loose
python3 -m tools.repack texture-unpack \
  --input build/repack/loose/archive/chunk01/transient00.bin \
  --out build/repack/title --preset title
# In a PNG editor, recolor palette004.png; preserve its alpha and dimensions.
python3 -m tools.repack texture-pack --tree build/repack/title \
  --out build/repack/title-edited.bin
cp build/repack/title-edited.bin build/repack/loose/archive/chunk01/transient00.bin
.venv/bin/python -m tools.repack build-disc \
  --tree build/repack/loose --out build/repack/my-mod
.venv/bin/python -m tools.repack proof-title \
  --iso build/repack/my-mod/Extermination.iso --out build/repack/my-mod-proof
```

`my-mod-proof/title.png` shows the cold-booted game. The proof command runs hidden
with private, disabled memory cards, saves only to private slot 16 and shuts down.
For a clean baseline, run `build-disc --require-original` before editing. Use a
fresh output directory per build. [REPACK.md](REPACK.md) gives WAV and table JSON
commands, exact limits, native format details, provenance and test results.
The matching source build still uses local unmatched assembly/data; it does not
claim the game is fully decompiled to C.

The 2026-10-08 proof used this workflow: the source-built baseline equaled the
original ISO, and the magenta NEW GAME mod changed only 43 disc bytes. Its hidden
cold boot reached the title screen; the before/after screenshots and receipts
are under `build/repack/`. The isolated mod tree and temporary edited ISO were
removed afterward, with canonical assets, memory cards and protected saves intact.

## 1. Code modding (the decomp dev loop)

Setup (container, compiler, splat) is unchanged — follow
`textbook/12-how-to-contribute.md` and `textbook/05`–`08` once. Intentional code
changes use a dedicated decomp checkout and its shared build-lock protocol.
The legacy patcher writes its ISO in place: pass an explicit private copy.
Do not run this loop in the repack worktree, whose default rebuilt-ISO path is
a link to the main checkout. Use the isolated asset-build command above there.

```bash
# macOS-arm64, dedicated decomp checkout — after editing and acquiring its lock:
mkdir -p build/repack
cp '/Users/abe/Documents/PS2 Games/Extermination (USA).iso' build/repack/code-mod.iso
.venv/bin/python tools/decomp/build.py build && \
.venv/bin/python tools/decomp/link.py && \
.venv/bin/python tools/decomp/repack_iso.py --iso build/repack/code-mod.iso
# Release the lock; use the private code-mod.iso for testing.
```

Current state:

- The build's verification gate checks the boot ELF and all **19 overlays**
  against the original. Do not infer whole-ISO equality from that gate:
  the 2026-10-07 repacker audit found the supplied `Extermination-rebuilt.iso`
  differed from the user's original image only within `SCUS_971.12`
  (1,246,308 differing bytes). [REPACK.md](REPACK.md) records both hashes.
  Use the original disc image as the lossless rebuild reference.
- Boot functions can link from ordinary C, inline assembly, or explicitly
  retained original assembly for stubs, near misses and source-missing slots.
  Use `tools/decomp/audit_link_provenance.py` to inspect actual link routes.
  Editing a stubbed function means decompiling it first (the contribute
  textbook covers the matching loop).
- Current matched/near-miss counts: `tools/verify_all.py` (after a fresh
  `tools/decomp/build.py build`) and `docs/PROGRESS.md`. Mapping intent →
  function: `docs/SUBSYSTEMS.md` (game functions labeled by subsystem),
  `docs/FUNCTIONS.csv` (the claimable index), `config/symbol_addrs.txt`
  (named symbols), `tools/callgraph.py`.

Regression gate after any change:

```bash
.venv/bin/python tools/verify_all.py                  # full (container)
.venv/bin/python tools/verify_all.py --no-container   # fast host subset
.venv/bin/python tools/verify_all.py --only match,selftest
```

Stages: `boot-elf` (rebuilt ELF byte-identical), `overlays` (19/19),
`match` (matched-code % vs a floor), `gltf` (exporter validates),
`selftest` (anim decoder round-trip), `gs-offset` (save-state VRAM-base
formula). Exit 0 = all good.

---

## 2. Asset exporters (`tools/`, pure Python, macOS-arm64)

All read the user's local `extract/` tree (from `extract_data.py`), plus —
for textures — a GS dump, a PCSX2 `.p2s` save state, or the disc's own GS
upload packets. `docs/FINDINGS.md` documents every format.

| Tool | Produces | Key flags |
|------|----------|-----------|
| `export_native.py` | Characters/creatures → **EMDL** | `--mesh` file, `--anim` clip library, `--clips id,…` (append clips; EMD3), `--clip`/`--live` (single palette), `--segment` n (multi-model packages), `--rig-nodes` n + `--anim-hdr` off (in-file clip banks), `--attach` (merge held equipment; needs `--gsdump`) + `--no-glow` (skip the aura quads — recommended for player.emdl, s43), `--library`, `--offset`, textures via `--gsdump` or `--p2s`, `--out` |
| `export_level.py` | Level render meshes → scene **EMDL** parts + `scene.txt` lines | `--level` files…, textures via `--gsdump` / `--p2s` / `--uploads` (replay the disc's GS upload packets — no save state needed), `--spawn x,y,z[,yaw]`, `--bgm`, `--office0-placed`, `--office0-doors`, `--pickups`, `--examine` (s69: the manifest examine/examinetext block — use-scan examine objects: CROSS -> input pause + radio line + camera cue; values read from your ELF/overlay/extracted id-0x41 text banks), `--camregions`, `--lightrig`; emits the manifest `enemy` block (crates + generators with decoded kind/link, others as comments) |
| `export_props.py` | Doors, gibs, crates, attachments → **EMDL** | `--doors` (m03 swing doors + `door` manifest lines), `--doors-office0` (m15/m17 rigs incl. the native-slide synthesis), `--gibs`/`--gibs-outdir`, `--crate` / `--crate-table` / `--crate-dir` leaf (concat view) / `--crate-blob` / `--crate-id`, `--attach` + `--no-glow` (skip the aura quads, s43), `--placements`, `--library`, textures via `--uploads`/`--p2s`/`--gsdump` |
| `export_collision.py` | id-0x44 collision → **EMCL v1** | `id44 <file…> -o out.emcl [--at off]` — accepts multiple files (sections can span a chunk's concatenation, s16); records its filename as the manifest `collision` line |
| `export_gltf.py` | Blender-ready `.glb` | character mode: `--mesh [--segment] [--anim] [--clips] [--fps]`, full-COLOR textures via `--gsdump`/`--p2s` (s18); `level` subcommand: `--level` or `--all-levels --out-dir` |
| `audio_export.py` | WAVs + the sound-id map | `music` / `voice` (split on the engine cue tables read from your local ELF, `--elf`), `sfx` (tone-record-driven: 533 samples → 1206 engine-exact-rate WAVs), `soundmap` (full id → bank → tone → WAV chain → `soundmap.json`), `detect-interleave` |
| `gen_sfx_registry.py` | The port's `assets/sfx/sfx.txt` | `ids…` or `--scene office` (area 2.1 preset: door pair from your placement data; prints the matching `doorsfx` line), `-o` |
| `export_font.py` | UI fonts → **EMFN** | `--ee` EE-RAM dump (via `parse_pcsx2_state.py`) `--out font.emfn [--preview png]` |
| `export_ui.py` | Status-screen decor → **EMUI** | `--gs` gs.bin state dump `--out ui.emui [--png]` |

Supporting tools: `extract_data.py` (DATA.DAT/INDEX.IDX → `extract/`, 100%
coverage), `placements.py` (AREAxx.BIN placement tables → JSON),
`parse_pcsx2_state.py` (.p2s → EE RAM / GS VRAM dumps), `collision_probe.py`,
`camera_probe.py`, `extract_models.py` / `extract_textures.py` /
`extract_subtextures.py` (older OBJ/PNG surface — still works; glTF/EMDL is
richer).

---

## 3. The native interchange formats

All are **our own formats** (open, documented in the producer's header and
the port's loader header); files are disc-derived and git-ignored.

- **EMDL** (`EMD2`/`EMD3`) — skinned/static model + baked world-matrix
  palettes + RGBA8 textures. Header (magic, bone/vert/index/frame/tex
  counts, fps, flags), parents, texture table, clip table (EMD3: per-clip
  `{source container id, first_frame, frame_count, fps}`), vertices
  `{pos, normal-or-baked-color, uv, bone|flags, tex}` (bit 31 = billboard
  +additive glow), indices, palette `[frame][bone][16]`, texels.
  Producer: `export_native.py`; loader: port `src/em_model.h` (EMD2 loads
  as a single whole-range clip).
- **EMCL v1** — collision world: shared vertex pool + convex-polygon records
  (plane, ring, outward edge normals) tagged with the engine's collision-set
  bit (grid vs cell worlds). Producer: `export_collision.py`; consumer:
  port `src/game/em_collision.[hc]` (faithful `segment_query` /
  `move_probe`, set-mask semantics of func_0019A570/func_0019AD00).
- **EMFN** — UI font: RGBA8 sheet + per-glyph `{u,v,w,h,advance}` table
  (both engine fonts: tall 12x20 proportional, small 16x16). Producer:
  `export_font.py` (format in its header); consumer: `em_hud_text`.
- **EMUI** — status-screen decor: 272x144 RGBA8 sheet + per-sprite records
  carrying sheet UVs and the audited 512x448 canvas anchors. Producer:
  `export_ui.py`; consumer: `em_hud`'s decor pass.
- **`sfx.txt`** — the port's sound registry: `<id-hex> <abs-wav-path>` lines
  (+ provenance comments). Producer: `gen_sfx_registry.py` from
  `soundmap.json`; ids are the **engine's own sound ids**.

---

## 4. Scene manifests (`scene.txt`)

Each scene directory under the port's `assets/` is: top-level `*.emdl`
(static geometry, loaded alphabetically), an `.emcl`, optional `doors/` and
`props/` subdirs, and a `scene.txt` read by `em_game.c` at boot. Plain
"keyword value" lines; `#` comments; unknown keywords are skipped (modules
like `em_door` scan the file for their own keywords).

```
spawn <x> <y> <z> <yaw>          # player spawn, true world coords; yaw rad, 0 = +Z
collision <file.emcl>            # collision world inside the scene dir
bgm <file.wav>                   # optional looping music cue (EM_BGM overrides)
doorsfx <front-id> <back-id>     # global door sound pair (engine ids, e.g. 0x3FD 0x3FE)
door <file.emdl> <x> <y> <z> <yaw> <radius>      # interactive door instance
enemy crate <x> <y> <z> <yaw>                    # disguised-crate crawler
enemy crawler <x> <y> <z> <yaw>                  # bare leech crawler
enemy generator <x> <y> <z> <yaw> kind <k> link <n>   # worm-emitter pad
                                  # kind = D_00248120 footprint config 0-6,
                                  # link = mode selector (0 inert, 1/2 emitting)
```

Notes: manifest enemies fill the `EM_ENEMY_MAX` (16) crawler pool —
exporters comment out overflow farthest-from-spawn; generators live in
their own `EM_GENERATOR_MAX` pool. The exporters write these lines
(`export_level.py` spawn/bgm/enemy block, `export_collision.py` collision,
`export_props.py` door lines, `gen_sfx_registry.py` prints `doorsfx`).
The port's first-level work (AREA11) is driven by its own startup/level path and
docs (port `docs/FIRST_LEVEL_AUDIT.md`); which scene directories exist is whatever
your local asset export produced. A scene's `props/enemy_crate.emdl` is probed before the global crate model.

---

## 5. Running and testing the port

```bash
# extermination-port/ (macOS-arm64)
make            # build/extermination
make run
make test-input # OS-free input-model unit test
```

Keyboard map: see the port's `src/em_input.h`, which is the only authoritative
source (it lists every key-to-pad binding, including the right stick, and documents
the engine's real default pad config). This doc no longer copies the map, because a
copy goes stale.

Tests and verification: follow the port's `CLAUDE.md` ("Verification" and "Tests":
first-level scope, headless, ~10 s default runs, exhaustive sweeps behind
`EM_TEST_FULL=1`). The old `EM_*_TEST` office-scene fixtures that this section used
to list as the regression gate were retired in s87 and are not a gate.

---

## 6. Blender / DCC modding

`tools/export_gltf.py` exports standard glTF 2.0 `.glb`: skinned characters
(exact per-vertex bone binding, disc normals/UVs, all clips from the bound
library — the player's has 455) and whole placed level scenes. As of s18,
characters get **full-color textures** when given `--gsdump`/`--p2s`.
Levels without a texture source fall back to gray sheets.

```bash
.venv/bin/python tools/export_gltf.py \
    --mesh extract/chunk28/f00_id3b.bin \
    --p2s  <your save state>.p2s \
    --out  models/player.glb

.venv/bin/python tools/export_gltf.py level --all-levels --out-dir models
```

---

## 7. What's NOT moddable yet

- **No inverse native-port format conversion** — the archive repacker
  rebuilds native PS2 payloads, but Blender/glTF/EMDL edits still need an
  encoder into the original asset format. Container round trips do not
  establish that an arbitrary edited payload will load in the game.
- **The port covers the first level only** (and is being made exactly
  original there); what it reproduces and what is still missing is tracked in
  the port's `docs/FIDELITY_FEATURES.md` and `docs/FIRST_LEVEL_AUDIT.md`, not
  here.

_Reviewed 2026-10-08: source-disc builds, reversible PNG/WAV/JSON edits, archive
growth and the hidden original-game texture proof; port formats remain unchanged._
