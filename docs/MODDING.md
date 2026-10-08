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
   for source builds, PNG/WAV/JSON/glTF editing, byte-sized growth, cue streams,
   new resident entries, and format limits.

The repacker supports reversible textures, variable-duration audio samples,
translations, music/voice cue streams and bounded same-topology native models.
Archive alignment padding is rebuilt around edited payload lengths. The audited
title-menu profile also supports logical texture-resolution and palette-count
changes within its original VRAM allocation. Other texture profiles, model
re-topology, arbitrary native-port EMDL/EMCL imports and movie re-encoding remain
unsupported. Opaque files and movies are preserved. Shareable mod packs contain
base-dependent deltas or explicitly declared entirely new work; players apply
them to their own original disc.

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
# In a PNG editor, recolor palette004.png to magenta; preserve alpha/dimensions.
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

## Translations, sound replacements and model tweaks

Use a new editor directory for each native leaf, and pack to a separate output.
The examples below all run on native arm64 macOS (the format commands also run
on Linux). Do not modify the native templates or layout manifests.

```sh
# Translations: edit table.json text, or segments for styled lines.
python3 -m tools.repack table-unpack \
  --input build/repack/loose/archive/chunk00/f02_id02.bin \
  --out build/repack/messages
python3 -m tools.repack table-pack --tree build/repack/messages \
  --out build/repack/messages-edited.bin
cp build/repack/messages-edited.bin build/repack/loose/archive/chunk00/f02_id02.bin

# Sound replacement: edit exported mono PCM16 WAVs and loops.json.
python3 -m tools.repack audio-unpack \
  --input build/repack/loose/archive/chunk04.n0/sound00.bin \
  --out build/repack/sound --rate 48000
python3 -m tools.repack audio-pack --tree build/repack/sound \
  --out build/repack/sound-edited.bin
cp build/repack/sound-edited.bin build/repack/loose/archive/chunk04.n0/sound00.bin

# Conservative shape, UV and normal edits: preserve glTF topology and _NATIVE_ID.
python3 -m tools.repack model-unpack \
  --input build/repack/loose/archive/chunk28/f00_id3b.bin \
  --out build/repack/player
python3 -m tools.repack model-pack --tree build/repack/player \
  --out build/repack/player-edited.bin
cp build/repack/player-edited.bin build/repack/loose/archive/chunk28/f00_id3b.bin
```

Translations may grow/shrink; the tool relocates string and style references.
The JSON uses native Latin-1 bytes, so a new language still needs supported font
glyphs. Edited runs must fit the original 128-byte scratch buffers including NUL;
visual line fit remains the author's responsibility. Sample duration may change,
with ADPCM-frame padding and tone/bank relocation. Keep the WAV rate/channel
format and adjust a loop start if shortening removes it. SPU memory and tone
addressing constrain growth; interior keyed sample aliases are refused.

Model positions stay inside existing joint bounds; identity skin nodes are a
native bone-local editing view. The accepted glTF interchange rules and full
re-topology design are in
[REPACK_MODELS.md](REPACK_MODELS.md).

For logical title texture upgrades, export its ten named views and a spec:

```sh
python3 -m tools.repack texture-upgrade-views --tree build/repack/loose \
  --out build/repack/title-upgrade
# Edit the PNGs and spec.json; PNG dimensions supply the new logical resolution.
python3 -m tools.repack texture-upgrade --tree build/repack/loose \
  --spec build/repack/title-upgrade/spec.json --quantize median-cut
.venv/bin/python -m tools.repack build-disc --tree build/repack/loose \
  --out build/repack/upgraded-title
```

For the demonstrated 2× selected NEW GAME, make `new-game-selected.png`
512×256 and set that view's `palette_size` to 256. Set all four
`background-*` views to 16 colors to free enough space. Keep the other five
menu views unchanged. `median-cut` explicitly permits color reduction and
reports changed pixels; use the default `exact` when no reduction is intended.
The original title allocation is a hard capacity limit, so simply doubling
every image is refused. Dimensions must be powers of two, at most 512, and
palettes contain 16 or 256 colors. The original sprite code's UV endpoint
overflows at 1024, which is refused explicitly. The output remains the same on-screen size.
The build regenerates the audited title compositor and all ten TEX0 references;
it does not change `src/`. Other atlases still require resolved consumer and
VRAM lifetimes before logical resizing. Physical index-sheet resizing remains
available separately with `texture-pack --resize-uploads`. See
[REPACK.md](REPACK.md) for the reference closure and capacity rules.

The 2×/256-color example was distributed as a delta pack, applied through a fresh
source build and cold-booted in hidden PCSX2. The title screenshot and read-only
loaded sampler/compositor checks are under `build/repack/texture-upgrade/pcsx2/`.
The four backgrounds show the intentional 16-color reduction; the other five
menu views remain unchanged. Original assets and protected saves stayed intact.

Music and voice use a separate bundle because their cue tables live in the boot:

```sh
python3 -m tools.repack stream-unpack \
  --input build/repack/loose/iso/files/STREAM/MUSIC.DAT \
  --elf build/repack/loose/iso/files/SCUS_971.12 \
  --cue 63 --out build/repack/music
# Replace cue_063.wav with stereo PCM16, 48000Hz; its duration may change.
python3 -m tools.repack stream-pack --tree build/repack/music \
  --out build/repack/music-bundle
.venv/bin/python -m tools.repack build-disc --tree build/repack/loose \
  --stream-bundle build/repack/music-bundle --out build/repack/my-stream-mod
```

For dialogue substitute VOICE.DAT and a voice cue ID (for example 143), keeping
mono PCM16 at 48000 Hz; repeat `--stream-bundle` to combine music and voice.
Unchanged WAVs preserve their original ADPCM bytes. Non-looping edits receive a
silent guard for the original playback timer; loops keep only sector padding.
Cue IDs and loop flags remain fixed, and script/subtitle timing is not rewritten.

Build all ordinary asset edits with `build-disc` as in the first walkthrough.
For an original-game player proof, rebuild after applying the model edit, then
cold boot and capture New Game:

```sh
.venv/bin/python -m tools.repack build-disc --tree build/repack/loose \
  --out build/repack/my-model-mod
.venv/bin/python -m tools.repack proof-gameplay \
  --iso build/repack/my-model-mod/Extermination.iso \
  --model build/repack/player-edited.bin --status-model \
  --out build/repack/game-proof
```

This verifies edited native vertex ranges through the live resource pointer and
captures the opening and status-screen model. The opening scene attaches a
separate face, so a head edit should be compared in the status screen. Add
`--voice-route` for the longer controller-only route to the first voiced
conversation. It waits for the player and fade to release movement, and exits
after two sound-state captures of cue 143. Both proof commands use the shared lock,
private disabled memory cards, private slot 16, and confirmed emulator shutdown.

The follow-up proof used a 20% smaller base head, a shorter music cue and a
longer dialogue cue. The STATUS comparison changed only 618 pixels around the
head. Both replacement streams passed active/advancing SPU playback checks on
the source-built disc; [REPACK.md](REPACK.md) gives the independent verifier
commands and retained receipts. Movies remain unchanged.

## Sharing a mod and installing someone else's

Finish native edits in a loose tree, keeping PNG/WAV/JSON/glTF editor projects
outside that tree. Make a pack against the exact original disc:

```sh
ISO='/Users/abe/Documents/PS2 Games/Extermination (USA).iso'
export PYTHONDONTWRITEBYTECODE=1
python3 -m tools.repack make-modpack --iso "$ISO" --tree build/repack/loose \
  --out build/repack/my-mod.emmod
python3 -m tools.repack verify-modpack --iso "$ISO" \
  --pack build/repack/my-mod.emmod
```

Share **only `my-mod.emmod`**, not the edited ISO, loose tree, exported images,
templates or stream bundles. The pack pins the original ISO and touched-file
hashes. Existing assets become COPY/XOR/INSERT deltas requiring original bytes;
the title's layout descriptor travels with its delta. For music or voice, add
`--stream-bundle build/repack/music-bundle` to `make-modpack` (repeat for both).
Only the changed cue lengths are distributed; original cue rows stay local.

For an entirely new asset or archive entry, repeat `--authored archive/PATH`
using each actual loose path. This explicitly declares the **entire** FULL
payload was created from scratch. Ordinary edits of existing art or sound must
use deltas. The scanner rejects any 65-byte original run outside validated delta
instruction fields, including literal and residual content and ZIP framing.
This check cannot establish authorship or identify transformed copies; it does
not turn an inaccurate authorship declaration into permission to share content.

Players place downloaded packs under `build/repack/`, then use their own disc:

```sh
ISO='/path/to/my/original/Extermination (USA).iso'
python3 -m tools.repack verify-modpack --iso "$ISO" \
  --pack build/repack/my-mod.emmod --pack build/repack/another-mod.emmod
.venv/bin/python -m tools.repack apply-modpack --iso "$ISO" \
  --pack build/repack/my-mod.emmod --pack build/repack/another-mod.emmod \
  --out build/repack/installed
```

Omit the second `--pack` for one mod. The playable output is
`build/repack/installed/disc/Extermination.iso`. Applying packs runs the fresh
boot/19-overlay source build and requires the project's local compiler setup;
the byte scanner needs native host `cc`. Both commands validate before
compilation, and conflicts name the shared target and both packs. Nothing wins
silently. To add a mod later, run again with the original ISO, all desired packs
and a new output directory. Modified ISOs are not accepted as a new base.

## Taking a disc texture mod into the native port

The port loads its exported assets. It does not automatically consume a new ISO.
The helper below is specifically the magenta-title showcase fixture from the
first walkthrough: it requires an increase in magenta pixels. It runs existing
exporters against that source-built disc, uses a private copy of support assets,
and runs the existing headless title test. Other colours need their own visual
comparison rather than this fixture's magenta assertion:

```sh
.venv/bin/python -m tools.repack.port_proof \
  --iso build/repack/my-mod/Extermination.iso --out build/repack/native-proof
```

It reads the existing sibling port executable and shared assets without changing
them. Disc staging, legacy extraction, exported assets, native save directory,
logs and frame capture all stay under `build/repack/native-proof/`. The capture
is `capture/title.png`; the receipt records which support assets were copied and
which were freshly exported. The proven mod is the magenta title texture. This
is a startup/title proof, not a full re-export or gameplay proof for every mod.
Other types need the corresponding port exporters from its `docs/STARTUP.md`,
pointed explicitly at scratch inputs and outputs.

Current port limitation: `export_streams.py` and `export_module_loader.py` pin the
whole boot ELF hash. A cue-table patch from a resized stream is rejected before
export. Using an original ELF with relocated stream data would produce stale
cue metadata, so the helper does not substitute one. The texture showcase keeps
the original matching boot and exports successfully. Supporting cue-patched boots
requires a port-side change; the port was left read-only. Logical title upgrades
also change the boot and require the port to consume the new texture layout.
[REPACK_PORT.md](REPACK_PORT.md) gives maintainers the proposed acceptance,
export dependency, stream coverage and loader/cache design. It is design only;
the current port does not install `.emmod` packs. [REPACK.md](REPACK.md) records
commands, actual failure messages and proof receipts.

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

- **Bounded inverse model conversion only** — use the repacker's same-topology
  glTF view; arbitrary Blender/glTF/EMDL scenes, new topology/materials, blended
  weights and expanded culling bounds still need a full native packet encoder.
- **Textures outside the audited title profile and movies** — logical upgrades
  need a complete consumer/VRAM audit per profile; no MPEG-2/PSS encoder is
  implemented. Boot patches also meet the port exporter restriction above.
- **Runtime proof here covers the title and first-level PS2 route.**
  Port coverage and remaining work are tracked in its `docs/STARTUP.md`,
  `docs/FIDELITY_FEATURES.md` and area-specific audits; this walkthrough does
  not establish mod compatibility for every supported area.

_Reviewed 2026-10-08: shareable mod packs, audited title texture upgrades,
variable-size formats, music/voice bundles, same-topology models and isolated
PS2/native-port proofs; port mod acceptance remains a design._
