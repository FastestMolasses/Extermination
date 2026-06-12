# Modding Extermination — The Current Surface

What you can actually change today, and with which tools. Two surfaces:

1. **Code modding** — edit matched C in `src/`, rebuild the boot ELF, repack
   your ISO, run in PCSX2. ~96.2% of game-code bytes are committed C.
2. **Asset modding via the native port** — export disc assets into our own
   open interchange formats (EMDL/EMCL/EMFN/EMUI), compose scenes with a
   plain-text manifest, and run them natively in the sibling
   `extermination-port/` (macOS Cocoa+Metal, zero dependencies).

There is **no asset repacker yet**: edited assets do not flow back into the
PS2 game. Asset modding today means the native port. Repacking
(`DATA.DAT`/`INDEX.IDX` rebuild) is a deferred roadmap item
(`docs/PROGRESS.md`, "Current roadmap" item 10).

**Legal frame (CLAUDE.md hard rules):** every exporter ingests only the
user's own disc dump / PCSX2 save states; all outputs land in git-ignored
directories (`extract/`, `assets/`, `models/`, `wav/`, …). Nothing
disc-derived is ever committed or redistributed.

---

## 1. Code modding (the decomp dev loop)

Setup (container, compiler, splat) is unchanged — follow
`textbook/12-how-to-contribute.md` and `textbook/05`–`08` once. The loop:

```bash
# macOS-arm64 — edit, then:
.venv/bin/python tools/decomp/build.py build && \
.venv/bin/python tools/decomp/link.py && \
.venv/bin/python tools/decomp/repack_iso.py
# boot Extermination-rebuilt.iso in PCSX2
```

Current state:

- The rebuilt boot ELF and all **19 overlays are byte-identical** to the
  original; the repacked ISO equals the original disc image.
- **Every function is a committed unit** in `src/` — either matched C
  (objdiff 100%) or an `INCLUDE_ASM` stub whose bytes come from locally
  assembled splat `.s` (`build/filler/`). Editing a stubbed function means
  decompiling it first (the contribute textbook covers the matching loop).
- 1247/1344 game functions matched at the last census (~96.2% of game-code
  bytes since). Mapping intent → function is much easier now:
  `docs/SUBSYSTEMS.md` (all 2,063 functions labeled by subsystem),
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
like `em_door` scan the file for their own keywords); a missing file or key
falls back to the built-in office defaults.

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
Shipped scenes: `assets/scene/` (captured office room, default),
`assets/scene_office0/` (AREA02 main floor), `assets/scene_snow/` (AREA11).
A scene's `props/enemy_crate.emdl` is probed before the global crate model.

---

## 5. Running and testing the port

```bash
# extermination-port/ (macOS-arm64)
make            # build/extermination
make run
make test-input # OS-free input-model unit test
```

Keyboard map (see `src/em_input.h`, which also documents the engine's real
default pad config): WASD = left stick, arrows = d-pad, K = CROSS
(USE/confirm), **L = CIRCLE (FIRE — the engine's real trigger)**,
I = TRIANGLE (status screen), J = SQUARE, Q/E = L1/R1 (R1 = weapon-draw
hold), U/O = L2/R2, **R = L3 (reload)**, Return = START, Tab = SELECT.

Environment variables (the test harness; all checked at boot):

| Variable | Effect |
|----------|--------|
| `EM_SCENE=<dir>` | load a scene directory instead of the default `assets/scene` |
| `EM_BGM=<wav>` | override the scene's music |
| `EM_CAPTURE=<path.bmp>` | BMP frame capture at gameplay frame 60 |
| `EM_CAPTURE_FRAME=<n>` | change the capture frame |
| `EM_CAPTURE_AIM=1\|2` | capture in aim pose (2 = after one shot, mid-recoil) |
| `EM_MOVE_TEST=1` | scripted walk self-test (asserts final position) |
| `EM_MOVE_LEGS=fwd,strafe` | resize the move-test legs (default 60,30) |
| `EM_MOVE_EXPECT=x,y,z` | expected final position for non-office scenes |
| `EM_DOOR_TEST=1` | door state-machine self-test (blocked → X → transit) |
| `EM_WEAPON_TEST=1` | draw/fire/reload/holster self-test (honest clip windows) |
| `EM_ENEMY_TEST=1..4` | 1 kill run, 2 contact run, 3 crate run, 4 generator run |
| `EM_ENEMY_GIBDEMO=<frame>` | lethal mailbox to enemy 0 at that frame (gib burst) |
| `EM_MELEE_TEST=1` / `EM_MELEE_DEBUG` | knife-vs-crate self-test / verbose |
| `EM_SFX_TEST=1` | SFX mixer one-shot test |
| `EM_HUD_FORCE=1` | force the status screen visible |
| `EM_AUDIO_TEST=1\|2` + `EM_AUDIO_FILE` | 440 Hz sine / stream a PCM16 WAV |
| `EM_INPUT_TEST=1` | input-event instrumentation |

Port modding conventions: the default capture must stay **byte-identical**
after asset/code changes that shouldn't alter the default scene, and the
self-tests above must PASS — that's the port's regression gate.

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

- **No repack into the PS2 game** — asset edits run in the port only.
- **Port scenes don't transition** — doors open and hold; the area-loader
  contract is decoded (FINDINGS "AREA TRANSITION LIFECYCLE") but not built.
- **Most creature behaviors** — only crawler/crate, leech, and generators
  are native; ~20 engine behaviors are censused but unimplemented.
- **Status sub-screens** — the hub is native; the X-entered pages are not.
- Footstep surfaces are fixed to floor-A pairs; button config is the
  engine default only. (All on the current roadmap in `docs/PROGRESS.md`.)

_Last updated: 2026-06-10 (through session ~s35). Reflects ~96.2%
matched game-code bytes, byte-identical ELF/overlays/ISO, and the
EMDL/EMCL/EMFN/EMUI native-port asset pipeline._
