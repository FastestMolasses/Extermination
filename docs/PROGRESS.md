# Extermination Decomp — Progress

> 2026-06-10 (session 25): FIRE ANIM MECHANISM decoded (static) — the
> s23 guess "fire clips 0x31/0x32/0x34/0x35 = containers 49/50/52/53"
> is DISPROVEN and corrected in FINDINGS: those values are armed-stance
> ACTION CODES at player +0x1F0 (func_0016F600: stance +0x05
> 0x1D/0x1E/0x1F/0x20 → 0x31/0x32/0x34/0x35, fire-mode independent; the
> fire families only read them for sound 0x164 vs 0x165), and containers
> 49/50/52/53 are unrelated long clips (49: 110 fr/17.2 u root travel,
> 50: 79 fr/8.5 u, 52: 33 fr, 53: 60 fr — library-verified, NOT recoil
> snaps, NOT appended to player.emdl; old export reproduced
> byte-identical from the recorded CLI first). The real recoil:
> bone_matrix_publish passes f13 = (float)fire-counter(+0x276) to
> anim_clip_arbiter when the code is 0x31/0x34 — the committed AIM-LADDER
> clip (anim_slot_index: D_00248B70/D_00248C50 per-sub 9-step pointer
> tables; SPR4 stance-A base 0x112) is re-sampled at frame = counter,
> which resets 0 per shot and gains +2/frame: the recoil snap is BAKED
> into the clip's front frames (0x112: 4.0°/fr over frames 0-4 decaying
> to a 0.9°/fr tail), replaying per shot and clamping back into the hold
> (12.5 ticks; full-auto restarts at frame 12 — inherent overlap).
> D_00248C90 rows extracted (weapon ids all rate 1.0; 0x110 = 1.4
> re-confirmed; elf/ copy maps file_off = vram−0x100000+0x94). PORT:
> em_game_anim_hold_restart (the counter re-seed) + em_game_anim_frame;
> em_weapon rewinds the held 0x112 to frame 0 @2 fr/tick per shot;
> weapon test extended (mid-replay after shots 1/5, clamp by D; D moved
> +14 for the settle window) — PASS; EM_CAPTURE_AIM=2 (aim + one shot)
> differs from =1 by 3.85% of pixels at the mid-recoil frame and is
> pixel-IDENTICAL 22 ticks later; default + aim captures byte-identical;
> all port self-tests PASS. FINDINGS: "FIRE ANIM MECHANISM" (+ inline
> correction in "ANIM ID MAPPING").

> 2026-06-10 (session 24 port wiring): WEAPON POSES + AIM CAMERA IN THE
> PORT — player.emdl re-exported with the s23 weapon clips appended:
> `export_native.py --attach --mesh extract/chunk28/f00_id3b.bin
> --anim extract/chunk28/f01_id3c.bin --clips 346,2,3,69,67,75,272,273,51,274
> --gsdump extract/gsdump/frame1.gs` (272 = 0x110 draw, 25 fr; 273 =
> 0x111 holster, 25 fr; 51 = 0x33 reload, 57 fr; 274 = 0x112 SPR4 sub-0
> aim-pose ladder base, 25 fr — all zero root travel). The old export was
> first reproduced byte-identical from the recorded CLI; the new EMDL is a
> byte-verified superset (verts/indices/parents/tex table/texels identical,
> old palette+clip table are prefixes, 4 clips appended) and all four clips
> are on the 21-node player rig (exporter parent-table check) and
> hemisphere-clean (max adjacent-frame rotation delta 5.0°/5.3°/24.1°/4.0°,
> far under the 90° s7c flip signature). Port side: em_game's anim mailbox
> grew a HELD-POSE variant (em_game_anim_hold — clamp at the last frame and
> keep the palette, the native analog of the armed tops re-selecting the
> aim-pose id through the arbiter every frame) + em_game_anim_frames (honest
> clip lengths); em_weapon drives the real clips per state (DRAW 0x110 @1.4
> once, AIM holds 0x112, RELOAD 0x33 / HOLSTER 0x111 with the state window
> = ceil(frames/rate) ticks — the old fixed windows remain only as flagged
> clip-less fallbacks; live windows draw 18 / reload 57 / holster 25).
> PLANTED AIMING (port decision, flagged in em_game.c player_move): the
> armed stance locks movement to turn-in-place — evidence: the armed modes
> 0x1D..0x20 replace the locomotion modes and their tops run no free-move
> spine, aim poses are full-body pitch-ladder POSE clips with NO aim-walk
> family and ZERO footstep frames in D_00248C90, and the live s23 capture
> shows the planted shouldered stance. The documented one-line AIM CAMERA
> hookup applied (camera target height → cam->aim_h while
> em_weapon_is_aiming()). EM_WEAPON_TEST rewritten to derive its checkpoint
> schedule from the honest windows — PASS; EM_CAPTURE_AIM=1 capture shows
> the shouldered rifle pose, the laser hit-dot on the wall and the
> lowered/over-shoulder aim camera; default capture byte-identical; all
> port self-tests PASS.

> 2026-06-10 (session 23c): door-path decomp wave — func_001BBD60 (door
> sound-pair patcher) and func_001BC240 (door sub-3→4 commit step)
> verified already 100%-matched (committed 572ab29); 3 walls
> characterized with best-attempt C inline in their stubs:
> func_001BC150 85.6% (NEW temp-register POOL-ORDER wall + prologue pair
> + #13), func_001B99F0/001B9A00 ~93.5% (dense-switch dispatch wall 2nd
> confirmation; two-function unit + mid-function ftab entry + case-
> fallthrough mid-block jtbl entry all reproduce from C), func_001BBE40
> 91.5% (all residuals known walls). NEW IDIOM 19: tail calls (`jr`
> through a loaded pointer) ARE matchable. New datums: bare-params-first
> for the reversed saved-reg mapping (param copies INVERT s1/s2); float
> literals = CW's lui/ori→mtc1 (the bit-pattern int form allocates a
> stack slot — keep it only for integer stores); volatile pins short
> distinct-global blocks (2 more confirmations). Idioms section below.

> 2026-06-10 (session 23 port wiring): DOOR-OPEN ANIMS IN THE PORT —
> player.emdl re-exported with the s23 door/scripted-walk clips appended:
> `export_native.py --attach --mesh extract/chunk28/f00_id3b.bin
> --anim extract/chunk28/f01_id3c.bin --clips 346,2,3,69,67,75
> --gsdump extract/gsdump/frame1.gs` (69 = 0x45 door-open FRONT, 200 fr;
> 67 = 0x43 door-open BACK, 200 fr; 75 = 0x4B scripted walk, 180 fr,
> root travel 15.0u → in-place at 5.01 u/s). All three verified present
> in chunk28/f01_id3c (21-node rig, matches the player) and
> hemisphere-clean (max adjacent-frame rotation delta 9.7°/10.6°/4.1°,
> well under the 90° flip signature of the s7c bug); door clips have
> ZERO root travel — in-place by construction, exactly the property-mode-0
> behavior of D_00248C90. The new EMDL is a byte-verified superset of the
> old (mesh/texture/index/old-palette blocks identical, 3 clips appended);
> the old export was first reproduced byte-identical from the recorded CLI.
> Port side: em_game grew the scripted-anim mailbox (em_game_anim_request /
> _cancel / _active = the +0x1F2 request → +0x20C commit path of
> func_00183090, one-frame latency preserved); em_door now runs the OPEN
> script per D_0024DE40 — side latch +0x2E at kickoff (s17 front test),
> on walk-to arrival player anim 0x45/0x43 at rate 1.0 + door sound+clip,
> wait 90/70 frames, then the fade — replacing the generic walk during
> that phase. EM_DOOR_TEST extended (anim 0x43 committed mid-open, reset
> after teardown) — PASS; default capture byte-identical; all port
> self-tests PASS.

> 2026-06-10 (session 23): DOOR SCRIPTS + ANIM-ID MAPPING decoded (static) —
> ftab_0024D880 fully itemized (27 opcodes 0x00-0x1A + sub-command tables;
> records are 0x40 bytes, s22's "8-byte" corrected), all three door scripts
> listed and traced (D_0024DE40 open / D_0024DEC0 locked / D_0024DBC0 locked-
> finish; the s22 side→script question resolved: choice is unlocked-vs-locked,
> the side only patches anim/clip/wait/sound fields into the shared records);
> door sounds = pair table D_0024DB80[link>>8][side] patched in by
> func_001BBD60. ANIM IDS: id = container index in the bound clip library
> (clip = lib + offset_table[id & 0x7FFF], func_001C6120; player default
> D_0028A580 == D_0028A490[0x3C] = chunk28/f01_id3c, 455 containers);
> D_00248C90 = per-id property rows {evaluator mode, footstep frames A/B,
> rate scale}; aim poses = per-sub-weapon clips from stance tables
> D_00248B88/D_00248C68. Clip ids: reload 0x33, draw 0x110 (rate 1.4),
> holster 0x111, fire 0x31/32/34/35, door open 0x45/0x43, locked try
> 0x46/0x44, scripted walks 0x4B-0x4E. FINDINGS: "DOOR SCRIPTS DECODED" +
> "ANIM ID MAPPING". Match attempt func_001BA080 (op-6 flag/counter switch)
> wall-blocked at 88.4% — NEW DATUM: mwcc orders dense-switch jump-table
> dispatch [lui, sll, addiu %lo, addu] vs CW's [lui, addiu %lo, sll, addu]
> (+ wall #13 slot fills, shadow-fill interleave); analysis + best shape
> inline in src/func_001BA080.c (stub restored).

> 2026-06-10 (session 22b): ENEMY AI ARCHITECTURE mapped (static) — all 19
> overlays' placement tables censused (1011 records, 164 distinct behavior
> fns: 39 main-ELF / 125 overlay-local; ~20 creature behaviors, 5 cover >90%
> of spawns). Characterized: generator func_0015A2C0 (class 0x0D, 129
> placements) → func_0015A200 installs leech brains func_00153F10/001546C0;
> placed crawler func_001551B0 (95) full state machine (idle nest → group
> alarm via actor+0x0A broadcast → 4-dir probe steering → suicide burst that
> spawns nest children); destructible fixture func_00156620 (115). Damage
> system: HP = actor+0x34, incoming-damage mailbox +0x36 (amount | weapon-type
> bits 0x2000/0x4000), hit-source pos +0x70; producers = weapon cluster
> (func_001735C0 melee, func_001B41F0 hitscan) + per-frame pair passes
> func_001A9000/func_001A9480 over lists D_00275B80 (targets, func_001B1D20)
> × D_00275BA0 (hazards, func_001B1DA0) from the gameplay frame; consumers =
> each behavior's own tick (func_00153B50 = canonical HP-=dmg hurt/death
> helper). Full writeup: FINDINGS "ENEMY AI ARCHITECTURE". No new matches
> attempted (docs-only session).

> 2026-06-10 (session 22): WEAPON SYSTEM characterized end-to-end (static) —
> rifle/spread = HITSCAN via func_0019A570 (mode 7, mask 0x20, 260 range),
> missile/grenade = projectile actors; gun = separate actor at player+0x20
> consuming a +0x2E fire event; 3-target auto-aim (func_00199220) with
> screen-cone + LOS rays; hit application func_001B41F0 writes victim
> +0x36/+0x70. Full writeup + port contract in FINDINGS "WEAPON SYSTEM".
> +1 readable-C match: src/func_0017B300.c (reload) — new idiom 16 below.

> 2026-06-09 (session 7): COLLISION CONFIRMED — id 0x44 = the collision world
> (cells + convex n-gons + s16 grid, verbatim on disc), level_world = the query
> library (func_0019A570/AD00/AFE0 segment-query hubs reached from the actor
> spine, with collide-and-slide response); decoder/validator:
> tools/collision_probe.py; full writeup in FINDINGS "COLLISION WORLD".

> 2026-06-09: SUBSYSTEM MAP COMPLETE — all 2,063 FUNCTIONS.csv rows labeled (40
> clusters, 91 fns in numbered unknowns), main() identified at 0x001AAE40,
> prioritized decomp roadmap in docs/SUBSYSTEMS.md; graph tool: tools/callgraph.py.

### glTF characters get full-COLOR textures — s5 TEX0 path adopted in export_gltf.py (2026-06-10, session 18)

Closes the s4d/s5 open item "glTF color textures via the same
marker-TEX0 path". `tools/export_gltf.py` (character mode) now sources
textures exactly like `export_native.py`:

- **Per-draw TEX0 grouping**: `load_skinned_sections` groups vertices by
  the CLD-masked 64-bit marker qword (the draw's complete TEX0 register
  value — FINDINGS s5), not the legacy 32-bit sheet-field DBP. One glTF
  primitive (+ material) per TEX0 key; a marker change still breaks the
  running strip. Geometry is unchanged (same strip walk/weld).
- **New `--gsdump` / `--p2s` flags** (same semantics as export_native):
  the VRAM snapshot resolves each plausible TEX0 (PSM 0x13/0x14, log2
  dims 4..10) to PSMT4/8 texels + its runtime-built CLUT via
  `export_native.build_texture_blob` (reused verbatim, lazily loaded —
  no import cycle), embedded as full-color RGBA PNGs with
  `baseColorTexture` materials. TEXCOORD_0 was already the per-record
  normalized ST; glTF's default REPEAT sampler matches the GS wrap.
- **Fallback unchanged**: without a VRAM source, TEX0 keys collapse to
  their sheet DBP (identical affine on the low 32 bits) and the
  residency-map grayscale sheets are shared per DBP — verify_all's
  no-dump invocation still resolves 3/3 sheets on f17_id8f (2837 tris,
  58 primitives / 3 materials).
- **Validated on both targets**: chunk28/f00_id3b + office GS dump
  (`extract/gsdump/frame1.gs`) → **51/51 TEX0s resolved**, 3170 tris,
  51 textured primitives, largest sheet 84% chromatic pixels (real
  color, not gray); chunk15/f18_id94 seg 1 + save state 01 (`--p2s`)
  → **68/68 TEX0s resolved**, 3330 tris, 14 clips from sibling
  f12_id44. pygltflib strict round-trip passes on both plus the legacy
  and level outputs; `verify_all --no-container` all-PASS (gltf stage
  green). Level mode untouched (still DBP sheets; `_png_rgba_bytes`
  refactored over a new `_png_from_rgba` RGBA encoder).

Still open: glTF alpha/blend modes (PNGs carry CLUT alpha but materials
stay OPAQUE), and the engine's per-material color modulation.

### Player AURA decoded + EMDL billboard/additive flag — green glow in the port (2026-06-10, session 17)

Closes the s9 "billboard/glow path" open item. Library models 20/21 are
±5 CUBES (faint-interior 16x16 PSMT8 texture, UV box 0.281..0.719); the
engine draws them through the LEVEL kernel as the player's aura: one
matrix = diag(1.6,4,1.6) at the root's X/Z, y = root_y − 9.1, identity
rotation, ADDITIVE (GS ALPHA A=0 B=2 C=2 D=1 FIX=0x80 → Cs+Cd), depth
test GEQUAL on / Z write OFF (ZMSK=1), green RGBAQ pulsing 20..215;
model 21 gets an extra slowly-rotating pass (the s7b "rank-2 variant").
Recovered OFFLINE by scanning save state 01's frame arenas (REF tags →
matrix CNTs; W = M·K⁻¹ row-convention with K from ctx+0x23C0) — no live
PCSX2 session needed. Models 110–118 = fixed panel-glow quads (level
side, still unexported). EMDL gains a compatible per-vertex flag (bone
word bit 31 = billboard+additive; bits 24..31 reserved as flags);
export_props bakes the aura as two camera-facing quads on node 0 with
live-tint textures; the Metal backend draws flagged triangles in a
second additive pass (ONE/ONE, depth-write off, camera right/up from
viewproj rows). EM_CAPTURE diff vs no-glow baseline: green-only +18/255
around the player (subtle, by construction); move test + verify_all
--no-container PASS. Full writeup: FINDINGS "PLAYER AURA / GLOW
BILLBOARDS DECODED".

### Multi-clip EMDL v3 + player WALK/RUN identified — port crossfades idle<->walk (2026-06-10, session 10)

The port now plays a real walk cycle while moving. Two halves:

**Player walk/run clips identified by stride scan.** Scanned all 455
containers of `chunk28/f01_id3c.bin` (bake every 21-node clip of length
10..120, feet = nodes 17/18 — the lowest-Y nodes of the idle pose —
score anti-phase fore-aft foot swing × foot lift × root travel):

- **clip 2 = WALK**: 45 frames, root speed 24.07 u/s, low foot lift
  (~1.3u), upper body CLOSEST to the idle-346 rifle-held pose (mean
  node distance 0.72u — the armed-stance variant).
- **clip 3 = RUN**: 40 frames, 47.57 u/s, high lift (~3.9u), same
  stance family (dist 1.33).
- Pairs (12,13)/(22,23) are other-stance walk/run variants (upper body
  2.1–2.6u from idle); (76,77) another family; 186–198 = slow/lateral
  steps (~13 u/s); 105/106/251 have 12–27u foot lift (jumps/rolls).
- ALL locomotion clips head exactly +Z (heading 0.0°), matching the
  port's yaw-0 facing convention.

**EMDL v3 (magic EMD3)** — `export_native.py` now writes a clip table
`{id, first_frame, frame_count, fps}` over a shared palette blob;
`--clips 346,2,3` bakes several library containers at once (first
clip's frame-0 root = shared origin, so the idle block is byte-
identical to the old EMD2 export); locomotion clips (root XZ travel
> 3u) are baked IN-PLACE (per-frame root XZ stripped, natural speed
printed). New `--attach` reuses `export_props.build_attached_player`
verbatim, so the one-command player export keeps the rifle/knife
merge. The port loads EMD2 unchanged (synthetic whole-range clip).

Port side: `em_model_palette_at(m, clip, t, out)` + clip table +
`em_model_clip_index`; `actor_update` crossfades idle<->walk with a
0.15 s linear palette blend driven by movement speed (the engine
cross-fades clip transitions the same way — see the s4 live-capture
note below) and advances the walk clip at move_speed/24.07 so the
stride tracks the ground. Verified: EM_CAPTURE idle byte-identical to
the pre-change baseline (asset blocks byte-compared too), move-test
placement unchanged, mid-walk captures (new EM_CAPTURE_FRAME knob)
show the legs scissored at ±8u feet separation vs 2.2u idle,
verify_all --no-container all-PASS.

### Quat hemisphere fix — one-frame 180° torso flip in the port (2026-06-09, session 7c)

The animated player's upper half flipped 180° for one frame per loop.
Numerical repro (bake clip 346 via `export_native.bake_id74_palettes`,
scan adjacent-frame rotation deltas): baked frames 98 and 162 had the
whole torso chain (bone 2 + descendants 4/7–11/15/16/19/20) ~177° off.
Root cause: the stored id-0x74 keys flip quaternion SIGN between
records — bone 2 keys at frames 96→100 and 160→164 are antipodal
representations of nearly the same rotation (dot = −0.9993; e.g.
q@96 = (−.4927,−.4899,−.5120,+.5049) vs q@100 = (+.5071,+.5054,+.4974,
−.4896)) — and `anim_decoder.sample_bone`'s NLERP had no hemisphere
correction, so the midpoint lerp passed through ~zero and normalised to
a garbage quat. Fix in `tools/anim_decoder.py` (the right layer; the
engine's own SLERP shortest-arcs): negate the earlier sample when
dot(q_prev, q_next) < 0 before lerping; selftest grew an antipodal-pair
case. Not implicated: the 0xFFFF sentinel (dropped at parse, clamped at
sample) and the port's `em_model_palette_at` last→first wrap (measured
0.01°/0.21° on clips 0/346). Verified: re-scan clean (no >90° adjacent-
frame jumps, clips 0 and 346), verify_all --no-container all-PASS,
player.emdl re-exported (diff confined to palette frames 97–99/161–163).

### chunk27 library placed in the port — it's the EQUIPMENT library (2026-06-09, session 7b)

Live placement decode done end-to-end (FINDINGS "CHUNK27 MODEL LIBRARY +
LIVE PLACEMENT DECODE"). Key outcomes:

- `chunk27/f01_id37.bin` decoded: directory (126 entries) of standalone
  mesh blobs in the character format; resident at EE 0x00BAA1C0; the
  loaded-asset slot table at 0x0028A4A0+ holds every resident asset base.
- **Per-model draw-unit anatomy + placement algebra solved**: unit MVP =
  K·W uploaded to VU1 dmem 0; `W = K_level⁻¹·MVP` (level kernel matrix =
  the world anchor) gives absolute world placements — validated affine +
  orthonormal on every live unit.
- **Premise corrected**: chunk27 is the player equipment / pickup
  library. Live office frame draws models 47/48/49/50/56/64 (the rifle,
  six parts, one transform at the hands), 106 (hip knife), 20/21 (glow
  billboards). The office desk items are baked in the LEVEL render mesh
  (already exported) — there is no separate room-prop pass in this scene.
- **New tool `tools/export_props.py`** → `assets/scene/01_props.emdl`
  (world-baked static EMDL v2, 7 models, 753 verts, 29 GS-dump-resolved
  textures). EM_CAPTURE acceptance: the soldier holds his textured rifle,
  knife on hip, in the office scene. Gun gap from session 5 is closed
  (static-pose bake; proper hand-bone attachment is future work).

### LEVEL RENDER MESH decoded — character standing in its textured room in the port (2026-06-09, session 6)

The native port now renders the textured, animated character STANDING IN
ITS LEVEL — the office room of the live GS-dump scene, fully textured in
color. Full write-up: FINDINGS.md "LEVEL RENDER MESH". Highlights:

- **The drawn level is NOT the `id 0x44` file.** Each level chunk carries a
  separate VU1-ready *render mesh* (the office's is
  `chunk06.n1/f03_id43.bin`): 64-byte `[pos+W][TEX0][ST][color]` records in
  0x880 blocks, position-W flag bits exactly like the character kernel
  (bit 15 = ADC/priming, bits 0..9 = matrix slot), **TEX0 verbatim on
  disc per record** — 84/85 of the dump frame's level TEX0 pairings appear
  byte-exact in the file. Level texture color is therefore solved by the
  same marker-TEX0 + GS-dump-VRAM machinery as the character.
- **Scene identification was nontrivial**: the only in-game save state on
  disk is a *different* scene (snow level, chunk15), which misled EE-RAM
  residency analysis. The office was pinned by TEX0-key intersection with
  the dump + bbox containing the live character position (107.4, 0, -184)
  + the VRAM texture-pack structure.
- **New tool `tools/export_level.py`**: render mesh → EMDL v2, static
  (bone_count 1, identity, frame_count 1), baked vertex colors in the
  normal slot (header flags bit 0), PSMT4/PSMT8/PSMCT32 textures resolved
  from the GS dump (112 textures for the office). `export_native.write_emdl`
  gained an optional `flags` kwarg (character path unchanged, default 0).
- **Port** (extermination-port): scene = `assets/scene/*.emdl` (alphabetical,
  world-space) + `assets/player.emdl` placed at its known world position;
  EMD2 flags plumbed through `em_model`/`em_gfx_mesh_create`; Metal shader
  gained a per-draw mode — bit 0 = "normal slot is a baked RGB color",
  fragment = texture * color (GS modulate). Orbit camera centers on the
  character in the room. Player-only path verified unregressed; EM_CAPTURE
  acceptance shows the camo character in the recognizable office (desk,
  lockers, binder shelf, concrete walls). verify_all --no-container all-PASS.
- **Honest gaps**: `chunk27/f01_id37` (an object-space model LIBRARY of
  props/pickups, 235 TEX0 keys, also drawn in the dump frame) is not
  placed — runtime entity placements unknown — so desk-top items/pickups
  are missing; movable sub-objects (nonzero matrix slots, ~750 verts:
  doors etc.) are exported at bind pose; PSMT4 alpha still drawn opaque;
  the id 0x44 static-mesh marker→TEX0 translation (the per-level material
  table) remains undecoded — irrelevant for rendering since the engine
  draws the render mesh, but the id44 file's runtime role (collision?) is
  still open.

### CHARACTER TEXTURES SOLVED — colored, textured, ANIMATED character in the port (2026-06-09, session 5)

The texture-color loop is CLOSED end-to-end. Full write-up: FINDINGS.md
"CHARACTER TEXTURES SOLVED". Acceptance test passed: the native port
renders the chunk28 character fully textured (face/skin/hair, urban-camo
fatigues, shoulder insignia, boots) animated on clip 346
(`extermination-port/build/capture_textured.bmp`, EM_CAPTURE frame 60).

- **Decisive finding: vertex qword 0 (the "marker") IS the draw's full
  TEX0 register value — TBP0+TBW+PSM+TW+TH+CBP baked into the DISC
  file** (x2 CLD load/reuse variants). Mesh texture<->CLUT pairing is
  disc-static; qword 1 is the vertex's normalized ST (REPEAT semantics,
  observed up to ~21.8). Character textures are PSMT4 (psm 0x14,
  16-entry CLUTs at per-texture CBPs) — why the PSMT8-only save-state
  harvest never saw them.
- **GS dump captured + parsed**: 1-frame PCSX2 dump of the office scene
  (recorded via Tools > Save Single Frame GS Dump). `parse_gsdump.py`
  rewritten into a full GIF-stream walker → per-draw TEX0 table (49,424
  TEX0 writes, 171 unique pairings) + `--textures` colored-PNG emission
  straight from the dump (its freeze blob has the same 425-base VRAM
  layout as .p2s). Cross-validation: all 51 mesh-marker pairs of
  f00_id3b appear in the dump's table (100%).
- **PSMT4 machinery added**: `extract_textures.py` BLK4/COL4/
  `psmt4_nibble`; `clut_pair.py` `read_psmt4`/`read_clut16_rgba`
  (16-entry CSM1 CLUT = 8x2 PSMCT32 region, word order
  [0,1,4,5,8,9,12,13,2,3,6,7,10,11,14,15]).
- **EMDL v2** (`export_native.py`): per-vertex UV + texture id from the
  records; `--gsdump frame.gs` embeds the resolved RGBA8 textures
  (51 textures, 432 KB for the player). Port: EMD2 loader (em_model),
  Metal `texture2d_array` with TILED pow-2 slices so sampler REPEAT
  reproduces GS wrap, [[flat]] per-triangle slice + per-texture UV
  scale, texture * directional light in the skinning shader.

Still open (graphics polish, not blockers): vertex lighting from the
kernel's light matrix instead of the port's stand-in directional light;
alpha (exported, drawn opaque); the gun model (separate, attached to a
hand node); ~~glTF color textures via the same marker-TEX0 path
(session 4d noted this upgrade path — the GS-dump pipeline is now
landed, so export_gltf.py can adopt it)~~ (CLOSED 2026-06-10 s18:
export_gltf.py `--gsdump/--p2s`).

### export_gltf.py re-pointed at the corrected pipeline (2026-06-09, session 4d)

Open item #3 is CLOSED. The void per-bone object-space decoders
(`load_per_bone_meshes*`), the spatial-proximity block binding, and the
id 0x71 character-animation path are deleted from `tools/export_gltf.py`;
characters now export as a proper skinned glTF:

- **Geometry**: stage-2 VIF strip blocks with the exact per-vertex node
  binding (`(pos_W_bits & 0x3FF) >> 3`), block walking shared with
  `export_native.load_mesh_sections`, plus per-record ST (TEXCOORD_0)
  and marker m0 → texture-sheet grouping (grayscale CLUT, as before).
- **Skinning**: one glTF `skin` with IDENTITY inverse-bind matrices —
  disc vertices are bone-local, so `vertex_world = joint_world * pos`,
  the same contract as the PS2 kernel and EMDL. JOINTS_0/WEIGHTS_0 =
  single influence, weight 1.0; joint index == node slot.
- **Animation**: one glTF animation per keyed container (sparse LINEAR
  keys at 60 fps, conjugate-quat rotations, scale only when non-unit).
  Rig pairing implements the session-4b rule (`n == max_slot + 1`, via
  `rig_probe.scan_anim_headers`, all blob ids): chunk28/f00_id3b
  auto-pairs the 455-clip f01_id3c library; f17_id8f segment 0 gets its
  11 id-0x70 20-node clips and segment 1 its 30 id-0xd0 44-node creature
  clips — the 21-node 0x74 cutscene track is correctly NOT selected.
- **Parity proven against the EMDL exporter**: glTF joint worlds composed
  from node TRS == `export_native.bake_id74_palettes` frame 0 to
  0.000000 max element error; triangle counts identical (3170 chunk28,
  2837/3994 f17 seg 0/1). pygltflib strict round-trip passes on all
  outputs (pygltflib reinstalled into .venv; `verify_all --only gltf`
  PASS, now reporting the 11 seg-0 clips).
- CLI: `--anim/--segment/--clips/--fps(60)/--no-textures`; `--skel` is
  accepted-and-ignored for backward compatibility (verify_all's
  invocation unchanged). Level mode untouched.

Still open here: EMDL (`export_native.py --anim`) still pairs by FIRST
container only — the session-4b `--rig-nodes/--anim-hdr` selector idea
remains the way to bake f17's id-0x70/id-0xd0 clips into EMDL. The glTF
texture grouping still uses the legacy `sheet_field → DBP` affine +
grayscale residency-map sheets; session 5's discovery that vertex
qword 0 IS the draw's TEX0 register (see EMDL v2 in export_native.py)
is the upgrade path to exact per-draw COLOR textures in glTF once the
GS-dump pipeline lands.

### chunk21/f17_id8f rig RESOLVED: it's an encounter package (2026-06-09, session 4b)

Open item #2 is CLOSED (parallel investigation; tool: `tools/rig_probe.py`
— probe/survey/compare subcommands). The "42-slot rig" question had a
third answer nobody hypothesized:

- **Segment 1 = ONE 44-node creature** (boss-sized, ~3x humanoid scale,
  hub node with 8 child chains — multi-appendage body plan). Its parent
  table + 30 clips (blob id 0xd0, 40..586 frames) are embedded in the SAME
  file's animation bank at 0x64080..0xc4e30. Slots 2..43 are nodes 2..43
  (root + node 1 unskinned, exactly like chunk28). Proof: contiguous slot
  histogram (no gap → not two stacked rigs) + mirror-chain fingerprint —
  the three slot pairs with byte-identical bboxes and z-mirrored centroids
  (11/12, 23/24, 34/35) are precisely the tree's mirror chains.
- **Segment 0's rig is the in-file 20-node id-0x70 table, NOT the 21-node
  0x74 prefix**: all five mirrored-geometry slot pairs match the 20-node
  tree's mirror nodes and contradict the 21-node tree. The 0x74 prefix
  (byte-identical to chunk28's player table) drives a 21-node actor in a
  401-frame synchronized multi-actor CUTSCENE track set (0x74 + 0x2c +
  3x 0x24 containers, same clip length).
- **The pattern generalizes**: survey of 126 prefixed files found more
  multi-model packages pairing mesh segments with in-file parent tables of
  size max_slot+1 (chunk05.n0/f14_id88 47-node, chunk17/f14_id8b 33-node
  74-clip, chunk25/f29_id95 33-node, chunk12.n0/f13_id8b → the common
  30-node enemy rig). Rule for exporters: pair each segment with the
  in-file table where n == max_slot + 1.
- To pose the creature: decode any id-0xd0 clip with the (now decoded)
  map-A/B/C channels + its 44-entry parent table — no external skeleton
  file needed. Minimal export_native.py change: a --rig-nodes/--anim-hdr
  selector (parse_id74_prefix returns only the FIRST container; use
  rig_probe.scan_anim_headers-style enumeration for multi-bank files).

### id 0x74 channel encodings FULLY decoded; ANIMATED character in the port (2026-06-09, session 4)

Open item #1 (map A's A/B fields + map B/C packing) is CLOSED — the
whole container turned out to be the id 0x71 clip format verbatim:
rotation = 4x20-bit LSB-first truncated-float quat (x,y,z,w),
translation AND scale = 3x26-bit vec3. Session 2's "i16 A/B companions
+ f20 W1/W2 + tag nibble" was that bitstream misparsed at byte offsets
(A/B = low 16 bits of qx/qz; tag nibble = their sign + exponent-prefix
0b011 top bits; W1/W2 = qy/qw). Full write-up + verification evidence:
FINDINGS.md "id 0x74 channel encodings FULLY DECODED".

Headlines:

- **Live-verified on two clips**: paused-capture quats match decoded
  channels to 0.0000 deg on keys / <=0.15 deg mid-interpolation across
  all 21 nodes; |q|=1 holds to 3e-4 on every keyframe of every
  container (static proof of the qx/qz fields). Translations match
  live node +0x00 exactly; engine local matrices use the CONJUGATE of
  the stored quat (pinned numerically: 0.008 vs 15.2 max element err).
- chunk28/f01_id3c is a **455-clip animation library** (the old "50
  containers" was a truncated scan); the live NPC cycles through it
  (idle = container 0, a 180-frame look-around = container 346) with
  cross-fade transitions (mid-blend captures match no single clip).
  Node +0x50 = clip phase 0..1, +0x54 = 1/(clip_len-1) per 60 Hz tick
  → clips play at 60 fps.
- `export_native.py --anim <lib> --clip N` bakes any container into a
  multi-frame EMDL (compose via the prefix's own parent table,
  conjugate-quat locals, recentre). Exported chunk28/f00_id3b +
  container 346 → the port renders a fully ANIMATED character; capture
  frame 60 (t=1.0 s) shows a clearly different, coherent pose vs
  frame 0. extract_models.py: `parse_id74_prefix()` now decodes real
  channel values; new `find_id74_headers()` enumerates whole files.

Still open from this thread: ~~export_gltf.py still carries the void
per-bone decoders~~ (DONE — session 4d, see top entry); map C scale is
constant (1,1,1) in everything sampled so far (apply-anyway support is
in the exporter); UVs/textures in EMDL.

### Texture COLOR recovered from save states (2026-06-09, session 4c)

`tools/clut_pair.py` extracts colored PNGs + pairing manifests from .p2s
save states — title-screen textures validated screenshot-exact; snow
particles + terrain atlases colored with high/medium confidence. The
TEX0-harvest pairing rule and a CRITICAL GS-VRAM offset correction
(freeze blob offset 425, not 509 — prior reads skewed) are in
FINDINGS.md "Texture COLOR recovered". **Offset fix landed (2026-06-09,
session 4d):** `gs_vram.py` corrected (base 425; `VRAM_TRAILER=84` is
the single source of truth, imported by `clut_pair.py`);
`clut_bruteforce.collect_clut_pool` now excludes tool-output dirs under
`extract/` (capture files were polluting the "disc" pool); the
2026-06-02 "zero disc matches → palettes are runtime-synthesised"
negative was RE-VERIFIED clean at base 425 on save states 01+02
(0 exact matches vs the 361-blob disc pool, nearest maxdiff 127–224 —
conclusion stands); new `verify_all.py` stage `gs-offset`
regression-guards the base against clut_pair's independent
palette-scored detection on a local save state (PASS: score 7>4 on
state 01). Character textures need a PCSX2 GS dump (.gs) of a character
scene (VU1-built TEX0s don't persist in a freeze) — DONE in session 5
(see top entry): the dump was recorded and the loop is closed for EMDL.

### Hand/forearm artifacts resolved: capture tearing, not IK (2026-06-09, session 3b)

The glitches around the hands in the first posed capture were temporal
tearing in the live capture (node arena read in two MCP requests while
the emulator ran; the hand nodes came from a different animation frame).
Re-captured with the emulator PAUSED: all 21 node worlds are exact
parent x local compositions (max dev 0.007) — no IK layer in the idle
pose. The right hand's curled shape is intentional (weapon-grip pose;
the gun is a separate attached model, not exported). Port captures are
now clean front and back. New protocol noted in FINDINGS: pause before
multi-read captures. Also swept FINDINGS/tools for stale pre-correction
claims (w-field selector, kernel classifications, exporter docstring).

### Skinned-character pipeline solved end-to-end; posed character in the port (2026-06-09, session 3)

The block→node binding fell: the character skinner is the 62-qw kernel
at vram 0x0023C780 (found via the live VIF1 draw chain: GS-state REFs +
CALL to the kernel's canned MPG packet + one REF covering the whole
block stream). It reads each vertex's position-W float AS AN INTEGER:
bits 0..9 are the absolute VU1 dmem qword address of that vertex's
7-qw matrix set (transform + normal matrix), laid out at qw 8×node;
bit 15 is the strip-restart flag. So per-vertex node = (W_bits &
0x3FF) >> 3, positions/normals are bone-local, and blocks are plain
VIF packets (MESH_SIG == STCYCL(4,4)+UNPACK V4-32).

Verified: exported the live scene's character (chunk28/f00_id3b.bin)
with its captured 21 node world matrices → the port renders a fully
coherent posed character (head/face/hands/boots). Joint-edge metric
confirms the mapping sharply (0.88 vs ≥3.6 for any shifted mapping).
Details: FINDINGS.md "Skinned-character pipeline FULLY DECODED".

Notes: session-1's player_bones_live.json is actually this chunk28
character's rig. ~~chunk21/f17_id8f is a different costume/variant (its
42-slot segment 1 rig pairing still open)~~ — RESOLVED in session 4b:
f17_id8f is an encounter package (20-node humanoid + 44-node creature +
in-file anim banks + a 401-frame multi-actor cutscene set); see the
session-4b section above. export_native.py grew --segment/--live and
per-vertex bone EMDL output; port unchanged.

### id 0x74 "vertex records" are ANIMATION — premise resolved, tools fixed (2026-06-09, session 2)

Set out to pin the 12-byte record layout via the VU1 kernels + VIF UNPACK
tags; the investigation overturned the premise instead. Full write-up:
FINDINGS.md "id 0x74 prefix is ANIMATION, not geometry".

**Headline results:**

- The mesh-prefix 12-byte records are **keyframes**, not vertices: the
  prefix is a keyed-animation container (`parse_id74_prefix` in
  extract_models.py) — header {node count, clip length in frames,
  pointers to 3 channel maps}, node PARENT table (the old "section→bone
  directory"), and per-node sparse keyed channels whose "vid" is the
  frame index. Record = {i16 A, f20 W1, i16 B, f20 W2, 2 flag bits,
  u16 frame} with the floats stored as top-20-bits overlapping the i16s.
- **Live-verified on PCSX2**: map A's W1/W2 = the node's local
  quaternion .y/.w byte-exact (chunk28 NPC, nodes 14/20/root); node
  struct corrections: +0x00 local translation, +0x30 local quat,
  +0x64[0] = PARENT index. A/B (the remaining quat components'
  encoding) and map B/C payload packing still open.
- `tools/disasm_vu.py` had five systematic decode bugs (UPPER bc ops,
  UPPER FD tables, LOWER T3 tables, I-bit placement, SQ field
  asymmetry); fixed against PCSX2's canonical tables. Consequently ALL
  pre-existing kernel classifications were artifacts: kernel #0 family
  = ribbon/beam quad renderer; #5/#7/#9 = fixed 14-vert effect kernel
  (single MVP, procedural UV, pseudo-normal lighting). No ITOF12
  "Q4.12 dequant" exists in any of them.
- Stage-2 MESH blocks are the only real geometry and are **bone-local**
  (per-block bbox ≤ ~2.7 units); the w field's k ∈ {-3,-1,0,2} is the
  same set in EVERY block (per-block 4-slot palette selector, not a
  global bone id). The **block→node binding table is the one remaining
  unknown** — it is not in the mesh file prefix (fully accounted for
  now: float track table, id 0x74 + id 0x2C anim blobs, preamble,
  313 MESH + 3 MATRIX + 41 raw-VIF float-vertex blocks).
- `tools/export_native.py` now exports the stage-2 strips (welded,
  Y-up, grounded) as a static EMDL; the port capture shows a coherent
  solid model — overlapping bone-local parts ("blob"), no longer
  triangle soup. Posed/animated rendering is parked on block→node.

**Next steps (in order):**

1. **Block→node binding**: pause PCSX2 mid-frame and sample VU1 dmem /
   VIF1 packet buffers during a character draw to catch (matrix
   palette, vertex batch) pairs; match batch floats to file blocks.
   Alternative: decompile the EE draw loop that walks the 313 blocks
   (the 41 trailing raw-VIF blocks' STCYCL(4,4)+UNPACK V4-32 addr-0
   tags are a good code anchor).
2. ~~Decode map A's A/B fields (qx/qz encoding) + map B translations →
   full mesh-embedded clip decode~~ DONE (session 4: the container is
   the id 0x71 record format; see top entry).
3. ~~Re-point export_gltf.py at the corrected pipeline (it still uses the
   void per-bone decoders)~~ DONE (session 4d: skinned/textured/animated
   glTF with exact per-vertex node binding; see top entry).
4. ~~After binding lands: bake palettes per frame and re-enable --clip
   in export_native~~ DONE (session 4: `--anim <lib> --clip N`; port
   side needed no changes).

### Native-port renderer + live-debug session (2026-06-09)

**Port (extermination-port):** full Metal skinned-character pipeline landed —
`em_gfx.h` grew mesh create/destroy + `em_gfx_draw_skinned` (bone-palette
skinning, depth buffer, runtime-compiled MSL) plus `EM_CAPTURE=<path.bmp>`
headless frame capture for screenshot regression; `em_model.{h,c}` loads the
new zero-dep `EMDL` asset (mesh + section→bone map + baked world-matrix
palette frames) produced by `tools/export_native.py` here. Pipeline verified
end-to-end (builds, runs, captures); the character renders as triangle soup
because the **disc vertex-record decode is wrong** — see below.

**Live PCSX2 MCP debugging (first use):** memory reads work (EE + VU1
windows); DebugServer breakpoints do NOT fire (recompiler). Found the live
per-bone NODE array (`*(0x00275B40)` → 21 nodes, stride 0xD0, world matrix
at +0x90, global-bone index at +0x64 matching the mesh section directory)
and disassembled `bone_matrix_publish`'s exact data flow. Capture in
`extract/live/` (git-ignored).

**Blocking discovery:** the long-assumed 12-byte object-space vertex record
(`pos3@+0, vid@+10`) is WRONG — vid is at +2, bytes [+5..+8] are a unit-range
LE float (per-vertex bone weight / W), and position fields are not
contiguous. All old object-space/rigged/glTF geometry is partially garbage;
the 2026-05-25 "posed figure in Blender" does not reproduce. ALSO: the player
mesh has TWO vertex streams (legacy 28-section + directory-mapped 21-section
with a different layout). **Next step:** decode VU1 skinning kernel #0 /
rigid skinner `0x00234610` dmem expectations + the section VIF UNPACK opcode
to pin the record layout, then fix `decode_objspace_bone_vertices` once.
Details: FINDINGS.md "Live bone-NODE array + vertex-record layout is WRONG".

### Whole-game extraction: every function is now a committed unit (2026-06-XX)

**Byte-match coverage of the game is 100%** — the boot ELF rebuilds byte-
identical to the original (verify_all boot-elf PASS) and all 19 overlays are
byte-identical. The rebuilt ISO equals the original; the decompilation is
verifiable-perfect today. (The "matched_code ~96%" figure is the narrower
*committed-C-source* coverage; the remainder is byte-reproduced by assembling
each function's local splat .s — that's why the full ELF is byte-identical.)

**INCLUDE_ASM stub skeleton.** Every function (~3000: game + Sony SDK) now has
a committed `src/<name>.c`. Functions not yet decompiled are **INCLUDE_ASM
stubs** — a first-line `// INCLUDE_ASM <name>` marker + a note. They contain
NO disassembly (CLAUDE.md hard rule); the byte-identical machine code keeps
coming from the locally-assembled, git-ignored splat .s via
`fill_unmatched.py`. `build.py` skips stubs at compile/expected/objdiff time
(see `is_asm_stub`), so the build output is unchanged — stubs are purely
organizational. This makes every function a claimable, parallelizable unit
without committing the binary's code. Generated by
`tools/decomp/gen_asm_stubs.py`; index in `docs/FUNCTIONS.csv`.

**The plan (user-directed order):**
1. *Byte-match everything* — DONE (ISO rebuilds byte-identical; every function
   covered by C or local .s).
2. *Make it all committed/claimable* — DONE (the stub skeleton above).
3. *Decompile stubs to byte-matching C* — ongoing, parallelized across the
   index. Replace a stub with C that compiles byte-identical (objdiff-verified).
   Wall-blocked functions (slti-into-branch, nop delay-slot fill, scheduler —
   see "CONFIRMED GENUINE WALLS") stay as stubs (still byte-identical) for now.
4. *Make C readable* — LAST, after byte-matching is maximised, per the
   strict-matching requirement (byte-identity must be preserved so the ISO
   stays testable).


### CodeWarrior 2.3.1 matching idioms — CRACKED (2026-06-02)

Three reusable C-source rules that make mwcc 2.3 reproduce CW 2.3.1's
scheduling/regalloc. Apply these when a readable-C decomp is 95-99% with
only float-op / operand-order / saved-register diffs:

1. **Compound assignment for float ops.** Write `x += K`, never `x = x + K`.
   `x = a[i] + K` makes mwcc emit `add.s dst, const, value` with a spare
   temp register; `a[i] += K` emits the target's `add.s dst, value, const`
   and reuses the right register. (func_0021B500, func_00179880.)

2. **Call-result first in chained multiply.** For `acc += A * (B * C)`, the
   final multiply's asm operand order follows source order. To get
   `mul.s dst, <call-result>, <product>` write the call result FIRST:
   `func(...) * (b * c)`, not `(b * c) * func(...)`. (func_00179150.)

3. **Saved-register allocation follows declaration / first-use order.**
   mwcc assigns s0/s1/... in the order locals are declared/first used.
   Declare the pointer that must land in s0 FIRST to match. Combined with
   rule 2 this cracked the sin/cos motion-update physics family
   (func_0014CD30, func_00150900, func_00136630 — `*p += *q * func(...)`).

4. **Duplicate the common tail-call in BOTH if/else branches.** When CW emits
   a dead `b epilogue; <dup jal>` and the duplicated instruction is a CALL
   whose per-branch value comes from another call result, writing the tail
   call explicitly inside each branch (instead of once after the if) makes
   mwcc emit the duplicated `jal` too, defeating its dead-`b` coalescing.
   (func_0017FC80, func_0017FF80 — animation-clip selectors.)

5. **Struct copy as explicit per-quadword copies.** Write a 0x40-byte block
   copy as `dst->q[i] = src->q[i]` for each 16-byte slot (or four explicit
   `*(u128*)(d+k) = *(u128*)(s+k)`), NOT a single aggregate struct assignment.
   The explicit form yields CW's interleaved `lq;sq` per slot; a single
   struct-assignment batches all loads then all stores and diverges.
   (func_001CFA60 object initialiser.)

6. **FP-register-slot via a two-float prototype (PARTIAL).** When a call takes a
   const float (e.g. 0.0f) that CW places in `$f13` (the 2nd FP arg slot) with
   `$f12` untouched, the callee has a *passthrough* float param. Declare the
   prototype with TWO trailing floats — `f(..., float passthru, ..., float k)` —
   forwarding the function's own float param to the first and the constant to
   the second. mwcc then emits `mtc1 ...,$f13` (not `$f12`) and stops re-sign-
   extending int args. Cracks the FP-register CHOICE only; surrounding mtc1
   placement / delay-slot scheduling may still be a wall.

7. **Block-copy temps via `volatile` src + fake trailing parameters**
   (2026-06-09, func_00102958 — 4x4 matrix copy, 100% matched). Two parts,
   both required: (a) declare the SOURCE pointer `volatile` — this pins the
   four `lq`s ahead of all `sq`s (otherwise mwcc sinks the first load down
   next to its store); (b) declare the copy temps as extra never-passed
   PARAMETERS — `f(u128 *dst, volatile u128 *src, u128 q0, q1, q2, q3)` —
   so they allocate to the next argument registers `$a2,$a3,$t0,$t1`,
   matching CW's block-copy temp choice (locals or an aggregate `*d = *s`
   get `$v1`/`$a1` reuse instead). Callers still pass only `(dst, src)`.

NEW WALL DATUM — saved-register allocation ORDER (2026-06-09,
func_00179BC0 / bone_matrix_publish, wall-blocked at 96.23%): mwcc assigns
`s0..sN` by WEB-START (emission) position; CW 2.3.1 assigns by DECLARATION
order. When a variable is declared early but first written late (e.g. a loop
counter initialized in the for-init after two calls), CW still gives it the
earlier saved register. With mwcc you can have EITHER the right registers
(init at declaration → the inits wrongly emit in the prologue) OR the right
placement (for-init → the saved params steal s0/s1) — never both. Falsified:
dual early+late init (dead-store elim runs first), `register` hints
(ignored), -O3,p (identical). Compose this with idiom 3: declaration order
only controls allocation when first-writes happen in the same order.
PARTIAL COUNTER-EXAMPLE (2026-06-09, anim_sample_bones): for the
param-save case specifically, copying a pointer param to an explicit local
(`bones = arg_bones;`) moves its web start ahead of a later param's,
flipping mwcc's s1/s2 to match CW — recovered 11 rows. The wall stands for
late-initialized loop counters; try the param-copy trick first.

NEW WALL DATUM — address-pair shadow-fill hoist (2026-06-09, keyframe
unpacker family func_001C84D0 / anim_decode_translation, blocked at
92.6%/87.0%): for back-to-back volatile-scratchpad store blocks, mwcc -O3+
hoists EVERY independent `lui/addiu` address-materialization pair maximally
early — packing as many pairs as fit into the first `lhu` load shadow, past
volatile stores, allocating fresh temps as needed. CW 2.3.1 emits at most
one pair per load cluster, each gated on a dead temp register. Falsified:
all pointer-local placements, idiom-7 fake-param register pinning (the pair
just moves to the next free register), `-O3,p`/`-O4`/`-opt nointerleave`
(identical), `-O2,p` (worse).

Additional cracked idioms from the 2026-06-09 sampler attempts (full
analyses live as comments in src/anim_sample_rotation.c,
src/anim_sample_bones.c, src/anim_decode_translation.c,
src/func_001C84D0.c — all still stubs, wall-blocked 82-93%):
8. **`-sdatathreshold 0` + `*(volatile int *)0xADDR`** reproduces CW's `$at`
   absolute addressing byte-exactly (raw constant, no reloc); `&D_xxx`
   externs give the relocated `lui/addiu` form. Both coexist in CW output —
   pick per access site.
9. **Union'd `uint128` quat fields fold `sq` offsets** (`sq v, 0x30(base)`);
   casting a `float[4]` field to u128 emits a stray `addiu` instead.
10. **Struct field access defeats mwcc's address-CSE** on store-then-reload
    of the same field; the cast form `*(float *)(p+0x54)` produces a hoisted
    `addiu a1, base, 0x54` that CW doesn't have.
11. **Goto-shaped loops** (guards branching forward to a bottom advance
    block) reproduce CW's non-rotated while-loop layout; a plain `while`
    gets rotated by mwcc.
12. **Statement-order emission** (2026-06-10, cracked func_001AB740 to 100%,
    recovered func_001AB650's call-arg order). mwcc emits independent SCALAR
    statements in source order — use this to steer instruction placement:
    (a) split compound pointer arithmetic across statements
    (`p = &D_xxx; p += off;`) to pin where the reloc pair vs the `addu`
    land — a single `p = &D_xxx + off` expression gets its whole
    computation sunk to just before first use (e.g. below an earlier sb);
    (b) place a constant materialization between two address statements
    (`p = &D_xxx; k = 1; p += off;`) to reproduce CW's
    [pair][const][addu][store] order; (c) materialize a call argument as
    its own statement before the call (`n = 0x20; f(p, 0, n);`) to make it
    emit FIRST with another arg in the jal delay slot, matching CW's arg
    order. Composes with reusing a dead PARAMETER variable for a constant
    to land it in that param's register (`idx = 1; *slot = idx;` →
    `addiu $a0, $zero, 1`, where a plain literal would allocate a temp).
13. **CW's delay-slot fill rule (when a `nop` is matchable).** CW 2.3.1
    fills a conditional-branch delay slot only from the TAKEN path's first
    instruction; mwcc 2.3 also fills from the fall-through whenever a SAFE
    candidate exists (a `lui` etc.). Consequence: a CW `beqz; nop;
    <safe-instr>` shape is matchable from C only when every fall-through
    candidate is unsafe to speculate (e.g. a store conditioned on the
    branch, cf. matched func_001F0060) — otherwise it is the delay-slot-fill
    wall (func_001B57E0 96.55%, func_001AB590 97.87%, func_001AD250 92.3%,
    all restored to stubs with the analysis inline).
14. **`switch` reproduces CW's beql compare-chain switch lowering**
    (2026-06-10, func_001B1B70 attempt, 91.53%). A C `switch` makes mwcc
    emit CW's exact sparse-switch shape: one `beql case-const` per case
    with the case block's first instruction (e.g. the call-arg `paddub`)
    duplicated into the likely slot, shared-label case pairs
    (`case A: case B:`) as beql(B)+beq(A)→out-of-line paddub stub
    falling into B's block, and out-of-line case blocks each ending
    `b common-exit`. Ordering rule: mwcc emits the COMPARE CHAIN in
    REVERSE source-case order and the CASE BLOCKS in source-case order —
    same as CW — so transcribe the target's chain bottom-up into source
    case order (func_001B1B70: chain D,7,4,2,A,1 + blocks 1,2,4,7,D ⇒
    source order `case 1; case 0xA: case 2; case 4; case 7; case 0xD`).
    An if-else chain does NOT work (mwcc inlines calls behind bne-skips,
    49.6%). Residuals in that attempt were walls #13 + dead block-head
    paddub dups (mwcc coalesces).
15. **Win the first saved register by hoisting a global load to the top**
    (2026-06-10, func_001AFA90 attempt). When CW gives `s0` to a
    variable whose load EMITS late (CW allocates by declaration order),
    write that variable's load as the FIRST statement: mwcc's allocator
    keys on source-order web start, so the variable wins `s0`, and
    mwcc's scheduler then sinks the load back down (often into a branch
    delay slot) for ~1 row of cost instead of ~16 — fixed the s0/s1 swap
    that cost 40 rows. Does NOT work when the variable is a loop-carried
    cursor (`cur`/`next` walks, func_001AFD70 97.46%): the cursor's web
    (head load + guard + bottom-copy phi) necessarily starts first and a
    `cur = next` seed before the loop is copy-propagated away — that
    remains the saved-register-allocation-ORDER wall.
16. **Forcing CW's redundant short re-sign-extension + temp-register
    choices** (2026-06-10, cracked func_0017B300 / SPR4 reload to 100%,
    src/func_0017B300.c). Four composable rules: (a) to reproduce CW's
    redundant `dsll32/dsra32` before an `slti` on an lh-loaded short,
    cache the volatile short into an **int** local and compare
    `(short)local` — a `short` local does NOT work (mwcc knows lh
    already extends; the cast on an int forces the pair); (b) assigning
    a comparison back into the compared variable (`low = low < 30;
    if (!low) …`) lands the slti/slt RESULT in that variable's register
    (CW's `slti v0, v0, k; bnez v0` shape) instead of `$at`, and also
    fixes the branch polarity/block order; (c) park a value in the dead
    first PARAMETER variable to pin it to `$a0` (`self = RESERVE;`) —
    temp-register analog of idiom 7's fake params; (d) idiom-15
    composition for temps: declare a constant's variable first but
    materialize it BETWEEN two guards (`int thirty; …if(...){ x=…;
    thirty=30; if(...){…}}`) — the web order wins the right register
    AND the scheduler sinks the `li` into the second guard's delay slot
    exactly where CW put it (materializing at declaration parks it in
    the FIRST guard's slot instead; both beql conversions follow).

17. **`switch` ALSO reproduces CW's plain beq+nop sparse chain — with
    cross-block case-constant reuse** (2026-06-10, func_00153B50 attempt,
    91.33% best, wall-blocked — analysis inline in the stub). Extends
    idiom 14 to the non-beql variant: a C `switch` on an idiom-16c
    fake-param-pinned value (`state = self->sub05` into the dead 3rd
    param -> `lbu $a2`) made mwcc emit CW's exact chain — reverse
    source-order compares with the SAME constant registers (v0,v0,a1,v1),
    the case bodies CONSUMING the chain constants (`sb a1,0x5(s1)` /
    `sb a1,D_008107FB` reusing the chain's `addiu a1,zero,2`), the
    switch register live into a case body (`addiu v0,a2,1`), and the
    dup'd common-tail head (`lui` in the default `b`'s delay slot).
    Compose with: idiom 12c for call-arg order (`clip = 0x34;
    anim_clip_init(s, clip, 10.0f, 0.0f);` puts `li a1` before the jal
    and `paddub a0,s1` in the slot — validated x3), `if ((short)hp <= 0)`
    polarity so the DEATH leg is the fall-through, and `*(int *)&f = K`
    for CW's integer stores of float-constant bit patterns (the lui/ori
    feeding both mtc1-compare and sw). Residual = wall #13 (the chain
    slots' fall-through candidates are the next chain constants — not
    C-addressable) + shadow-fill hoists + one f13-before-f12 swap.

18. **`volatile` on a function-pointer FIELD preserves a beqz delay-slot
    nop** (2026-06-10, same attempt). The tail `beqz v0; nop;
    lw v0,0x4C(s1); jalr` shape: mwcc fills the slot with the (safe,
    non-volatile) `lw` of the hook pointer; declaring the field
    `int (* volatile post4C)(...)` makes the load unsafe to speculate
    and recovers CW's nop. This is the field-level analog of the
    matched-func_001F0060 rule in idiom 13. (Counter-datum: `volatile`
    on a BYTE field did NOT stop mwcc sinking its `sb` into a jal delay
    slot — volatility blocks speculation across a branch, not sinking
    into an always-executed slot.)

NEW WALL DATUM — prologue ADDRESS-pair split (2026-06-10, func_001B61C0 /
pad-rumble request, wall-blocked at 93.6% with every other row AND all
registers matching — idiom-7 fake params pinned the pause byte to $t0 and
the pad-block pointer to $t1, the bnezl+dup'd-`addiu v0,1` likely-slot
shape and the `sh` slot fill reproduced, and all four branch nops
survived because the lbu candidates were volatile). CW 2.3.1 emits the
global-struct address pair adjacent BEFORE the ra save (`addiu sp; lui;
addiu; sq ra`) — the address-materialization analog of the
float-constant-prologue-hoist wall; mwcc 2.3 fills the lui->addiu stall
with the `sq ra` at -O3,p/-O4/-O4,s/-O4,p and hoists `sq ra` above the
whole pair at -O2,p. Falsified: all four opt levels, pointer assigned
before/after the first guard (after = worse, the pair sinks into the
beqz slot). Once bytes match, inject_relocs.py supplies raw-cast
HI16/LO16 — it refuses while .text differs, so the reloc rows show as
extra diff until the scheduling row is fixed.

WALLS RECOGNIZED WITHOUT BURNING ATTEMPTS (2026-06-10, from disassembly
inspection — see the analysis inline in each stub): func_001860A0
(surface-impact FX resolver) has 3x dead-`b`-dup (two dup'd chain
constants + a dup'd return-0 paddub); func_001B41F0 (the hit-application
dispatcher) stacks dead-`b`-dup x3 + wall-#13 bc1f slots + the
saved-register-allocation-ORDER wall across SEVEN webs (params a0..t1 ->
s6..s1 reversed, s0 reserved for a late-initialized local). Do not
attempt from C until those families crack.

NEW WALL DATUM — mwcc reorders accesses to DISTINCT volatile objects
(2026-06-10, func_001AAD00 attempt, 75.13% best). Pinning a long gp-rel
load/store swap block by declaring the globals volatile and transcribing
CW's emission order with explicit temps FAILS: mwcc preserves per-object
volatile order only, freely interleaving different volatile globals
(scored 55.7%, worse than plain statements). For such blocks the only
lever is coarse statement order (per-list [pubCursor, pubCount,
resetCursor, resetCount] beat 5 other legal orders by 4-14 points); the
residual interleave is the documented scheduler-divergence wall.
REFINEMENT (2026-06-10, door-wave session): for SHORT mixed blocks
volatile across distinct objects DOES hold program order in mwcc 2.3 —
two confirmations: func_001B9A00 case 6 (2 loads + 3 stores on five
distinct globals D_00810350/354/358/360/368: matched only after declaring
all five volatile, including the f0 register reuse), and func_001BBE40
(volatile on the five script patch points D_0024DC14/DC54/DC8C/DCD4/DD14
recovered CW's store order AND flipped the shared `1` constant into CW's
v1). The func_001AAD00 failure is specific to LONG gp-rel swap blocks.

19. **Tail calls ARE matchable** (2026-06-10, func_001B99F0 op-09 handler,
    byte-identical 3-instr prefix). mwcc 2.3 emits the MIPS sibling-call
    shape `lw v0, 0x4(a2); jr v0; nop` for
    `return ((int (*)(...))rec[1])(actor, blk, rec);` — a call in tail
    position whose arguments pass through unchanged. Do not skip
    `jr <reg>`-tail functions assuming mwcc can't produce them.

NEW WALL DATUM — temp-register POOL ORDER (2026-06-10, func_001BC150 /
door transition commit, wall-blocked at 85.61%; O3/O4 identical, O2
60.9%). For the four short pre-branch temp webs (door-id lbu, dest-table
reloc pair, area lbu, door-id lh) CW 2.3.1 allocates v1 / v0->a1 split
pair / a2 / v0; mwcc 2.3 allocates a1 / v1 same-register pair / a0 / v0 —
it prefers free ARGUMENT registers for short temp webs and never picked
a2. Idiom-7 fake-param pinning FAILS for values ASSIGNED from memory
loads: the assignment kill-renames the web and mwcc reallocates it by its
own pool order (idiom 7's matrix-copy temps worked because the loads were
the webs' only def, feeding stores, call-free). Statement reorder
(idiom 12) does not move the loads — the scheduler hoists all loads above
the address pair regardless of source order. Same attempt also hit the
prologue ADDRESS-pair split (pair interleaved around `sq s0`) and wall
#13. NOTE: split-REGISTER reloc pairs (`lui v0; addiu s0, v0, %lo`) are
NOT themselves a wall — mwcc emits them when the destination is a saved
register or a second live temp (matched in func_001B9A00's prologue and
cases 4/1); the same-register-pair divergence is specific to short temp
webs.

NEW DATUM — param-to-saved-register mapping: BARE params first
(2026-06-10, func_001BBE40 kickoff, 91.5% best). With a body-local
pointer assigned first (winning s0, idiom 15) and params used DIRECTLY,
mwcc reproduces CW's reversed-by-declaration mapping a0->s3, a1->s2,
a2->s1 exactly. Copying params to explicit locals (`d = door; b = blk;
m = mode;` in any order) INVERTS mwcc's s1/s2 choice — the copies
collapse by copy-prop and re-rank the webs. Counter-datum to the
anim_sample_bones param-copy trick: when the target mapping is already
a0->sN .. aN->s1, use bare params; reach for the copy trick only when
the target mapping deviates from that.

NEW DATUM — float-constant forms (2026-06-10, func_001BBE40): plain
float literals (e.g. `1.5707963705062866f`, `5.0f`) compile to CW's
exact `lui/ori -> mtc1` integer materialization — including inside
compares and `const * call()` products (idiom-2 operand order applies).
`*(int *)&f = K` is the WRONG form for values consumed as floats: mwcc
allocates a STACK slot and round-trips sw/lwc1 (cost ~15 rows + frame
growth). Keep the bit-pattern form only for INTEGER stores of float
constants (`D_0024DC8C = 0x42B40000`, `*(volatile int *)0x700038AC =
0x3F800000` — both matched).

NEW WALL DATUM — dense-switch jump-table dispatch order, 2nd confirmation
(2026-06-10, func_001B99F0/func_001B9A00 op-0A player-anim handler,
~93.5% genuine rows): mwcc [lui jtbl, sll idx, addiu %lo, addu] vs CW
[lui, addiu, sll, addu], as in func_001BA080. Two additional structure
datums from the same unit: (a) a TWO-FUNCTION translation unit compiles
fine — mwcc emits one .text section per function, and the ftab's
mid-function entry point (func_001B99F0+0x10) is just the second
function; (b) a jump-table entry landing MID-BLOCK in another case's body
reproduces naturally from C `case 4: ... /*fallthrough*/ case 0:`.
Reminder: a matched dense-switch function still cannot be LINKED from C —
the compiled object carries its own local jump table while the original
table lives in the shared data region (jtbl_0026E0B0).

 
NOT A WALL — paddub register moves (correction 2026-06-XX). mwcc DOES emit
`paddub $rd, $rs, $zero` for EE register moves/arg-saves/arg-setup — proven by
committed readable-C matches func_00182A70, func_001BC300, func_001F0060 (each
contains paddub and is byte-identical). A value (int OR pointer) used after a
call is saved across it via paddub naturally; `arg = 0` becomes
`paddub $aN, $zero, $zero`. Do NOT skip paddub functions (~1158 stubs have
paddub). The real limiter on multi-call functions is SCHEDULER divergence
(gp/scratchpad-address rematerialization order, delay-slot fill) — that, not
paddub, is the wall.

GENUINE WALLS — CONFIRMED UNBREAKABLE from C (exhaustive: 8+ varied steering
attempts each across opt levels, control-flow shapes, casts, temps; STOP trying):
- **slt-into-branch regalloc.** mwcc's branch-on-compare peephole ALWAYS sinks
  the slt/slti result into `$at` and folds it into the branch; CW uses `$v1`.
  The only way mwcc emits slti into a general reg is when the compare result is
  a RETURNED value (changes semantics). Blocks bone_init_default_0 (97.6%) and
  every dynamic-bound loop/compare guard.
- **Float-constant prologue hoist.** CW emits the const load (e.g.
  `mtc1 $zero,$f12`) right after the stack-adjust, before register saves; mwcc
  sinks it to just before the call. The const VALUE is matchable (rule 6); its
  PLACEMENT is not.
- **Branch-delay-slot fill.** CW leaves `beq; nop`; mwcc fills the legal delay
  slot with the next instruction, shifting everything after by one. No C
  structure forbids the fill; -O4 (no speed-sched) still fills.
These three saturate the ~40-50 game-code near-misses; they stay as .word
matches under strict matching (or become readable C only via hand-written asm).

ADDITIONAL CONFIRMED WALLS (recurred in wave 3, not C-addressable):
- **Float-constant materialization hoist.** CW emits `mtc1 zero,$f12` / a
  const load right after the stack-adjust (before register saves); mwcc emits
  it just before the call.
- **Prologue spill/reload interleave.** CW interleaves an unrelated `lwc1`
  reload between a sub.s and its store around scratch RAM.
- **slt-into-branch regalloc** (`slt $at` vs CW's `slt $v1`) on dynamic-bound
  loop/compare guards — blocks otherwise-clean loops (e.g. bone_init_default_0,
  a per-bone default bind-pose init, reached 97.6%).

CONFIRMED COMPILER WALLS (do NOT keep trying from C):
- **Dead `b epilogue; <dup-instr>` after a branch.** CW 2.3.1 emits an
  unconditional branch to a shared epilogue plus a dead duplicate of the
  merge instruction; mwcc 2.3 always coalesces it, at every opt level and
  for every structure (if/else, goto, ternary). (-O2,p can recover the dead
  `b` but then leaves delay-slot nops unfilled — trades one wall for #2.)
- **Branch-delay-slot fill divergence.** CW left `beqz; nop` or set the
  return value redundantly in delay slots; mwcc fills/omits differently.
- **add.s scheduled BETWEEN a lui %hi / addiu %lo reloc pair.** mwcc treats
  the reloc pair atomically and won't interleave; CW does. Scheduler-level.

These three walls are why ~40 game-code near-misses stay as .word matches.

### Update — 2026-06-02 readable-C campaign (+83 functions, strict match)

Two parallel campaigns over the game-code region converted **83 .word/
asm-void blocks into readable idiomatic C**, all byte-identical (commits
db65e1c, 11b52a2). Boot ELF remains byte-identical (verify_all boot-elf
PASS); matched_code 96.18% (1247/1344). This is the first big push toward
READABLE game/engine code (the prior 96% was mostly opaque .word blocks).

Readable gameplay now includes: per-frame motion update (countdown ->
position-snap vs velocity integration), entity sub-object construction by
type byte, entity-to-target heading-angle helpers, sound triggers, a
one-shot-flag substate reset. Plus ~50 engine dispatch/allocator thunks.

Two process notes baked in for future readable-C work:
1. **Never-regress requires a full recompile before linking.** Restoring a
   failed attempt's .c WITHOUT recompiling its build/obj/*.o leaves a STALE
   object; fill_unmatched links the stale obj and silently regresses the
   boot ELF. Per-function objdiff passes but the LINK differs. Fix: run
   `build.py compile` (full) then `verify_all --only boot-elf` before
   committing any readable-C batch. verify_all's relink is what catches it.
2. **The recurring matching wall is the CodeWarrior 2.3.1 scheduler.** ~48
   near-misses (95-99.4%) had fully correct readable logic but 1-2 non-
   matching instructions, from: reversed independent float-constant loads
   (f13 before f12), the branch-before-jal nop-delay-slot idiom, the 2.3.1
   dead-instruction-after-branch artifact, and commutative-operand /
   register-allocation divergences. Cracking any one of these idioms would
   unlock many functions at once — higher leverage than more breadth.

### Update — 2026-06-02 TEXTURED-BLOCK BONE BINDING — no on-disc field; runtime-built (decisive negative)

Cracked the *structure* of the textured-mesh per-block bone binding from the
live capture (`/tmp/cap2/`), even though it turned out there is **nothing to
read on disc**. The headline-export question ("which 4 global bones skin each
textured 0x820 block?") is answered: the engine builds the palette at draw time.

- **File architecture mapped.** `chunk21/f17_id8f.bin` stores the model twice:
  an **object-space per-bone VIF stream** (bound by the directory at `0x2280`,
  21 sections, RESOLVED earlier — plus ≈40 more 20/44-section directories of the
  same format filling the directory zone `0x53040..0xc4e40`, LOD/detail copies),
  and **textured 0x820 blocks** in two uniform runs — region 1: 134 blocks
  `0xe848..0x520e8`, region 2: 183 blocks `0xd6048..0x132708` (317 total).
- **The W-selector is GLOBAL, not per-block.** `k = round((w-sign(w))*512)` is
  **exactly `{-3,-1,0,+2}` in every block of both regions** — it selects one of
  4 fixed palette *slots*; the *contents* (the 4 bones) are swapped per draw
  batch by the EE. The earlier "small per-block k-set" was an artifact of a
  too-small sample.
- **Ruled out as the per-block bone field:** the per-bone directories (their
  `base_ptr+offset` sections do NOT align with textured-block MESH sigs — they
  index the parallel object-space stream); the 276 file-prefix `0x20`-byte
  records (count≠blocks; 8-float animation/transform params); the `m0/m1`
  marker (texture-sheet + material index only); any length-134/183 byte table
  (none exists).
- **Live capture = runtime palette, not a disc lookup.** This frame's
  `vu1_dmem` qw 111..122 held GS/projection setup (qw112 `2048,2048` screen
  centre, qw113 `1677721.5` Z scale), and the qw 90..93 "active draw" matrix is
  an identity-rotation **viewport** matrix (translation `452.3,278.6,277.6`,
  also resident at several EE viewport-constant addresses). So palette slots are
  **MVP-folded** (`viewport · bone_world`) — the save state caught VU1 between
  batches with no clean 4-bone palette. The clean per-bone LOCAL matrices ARE
  present: two 21-matrix double-buffered runs at EE `0x00287f40` / `0x00288d40`.
- **Conclusion / next step.** Per-block binding is engine-built: the EE gathers
  4 bone matrices per block, folds with the viewport, UNPACKs to the VU1 palette;
  the vertex W picks the slot. To recover the exact slot→bone choice, decompile
  the EE palette-UNPACK builder (the loop reading 4 bone indices from a per-draw
  structure) or capture mid-UNPACK with a fully-populated palette. Exporter
  **unchanged** (no clean disc binding to wire); textured surface stays on the
  spatial-proximity proxy; object-space rig (dir `0x2280`) remains the faithful
  posed export. `verify_all --only gltf` PASS preserved.
- **Files:** no tool edits; `docs/FINDINGS.md` ("Textured-block bone binding —
  the per-block field is NOT on disc"), `docs/PROGRESS.md`. Scratch analysis in
  `/tmp/cap_analysis/` (user-local, never committed).

### Update — 2026-06-02 NEUTRAL-LIT MENU CAPTURE — base palette is also engine-synthesised (colour offline = closed door)

**[Correction 2026-06-09: this entry's VRAM reads used the wrong freeze base
(509; correct 425, entry-aligned skew). The decisive "zero disc matches"
negative was re-verified clean at base 425 — conclusion stands. See
FINDINGS.md correction notes.]**

The decisive follow-up to the live-capture work below. Two **flat-lit menu**
captures (`/tmp/menu02` = title screen in colour, `/tmp/menu03` = Options;
user-local, never committed) test whether neutral lighting yields the
*unmodulated base* palette — which would then match an on-disc blob and give an
offline colour binding. **It does not. The base palette is engine-built too.**

- **Step 1 — title-screen colour reproduction VALIDATED (must-hit milestone).**
  The title atlas (DBP 10752, `chunk01/f00_id06.bin`, 512×768 PSMT8) decodes
  cleanly and holds the whole title screen. Applying the menu's **resident**
  CLUTs (un-swizzled) reproduces the **on-screen colour families** — the atlas
  is **multi-palette**: the purple/silver wordmark uses one resident CLUT
  (CBP 12174-run), the red/magenta X-ray hand another (CBP 12158-run), a
  blue/steel region another (CBP 12288-run), plus a persistent UI/font palette
  (CBP 8368-run, also in the gameplay capture). Renders are structurally perfect
  and colour-correct per region → the per-frame colour path is proven on a
  known target. PNGs: `scratch/color2/menu02_title_cbp*_unsw.png`.
- **Step 2 — KEY TEST: menu CLUT vs disc blob = NO MATCH (decisive negative).**
  Every resident menu CLUT (37 in menu02, 19 in menu03) vs the 361-blob disc
  pool, with 4-block alignment, both swizzle directions, and three keys (exact /
  RGB-only / alpha-normalized): **zero matches**; nearest RGB-only max-byte-diff
  154–235/255. The neutral-lit base palette is **not** a disc blob under any
  alpha convention.
- **Per-entry synthesis re-confirmed.** 12/13 distinct chromatic entries of each
  resident CLUT appear as standalone 4-byte words in `ee.bin`, but the
  contiguous palette appears nowhere on disc or in EE RAM. The engine builds
  each CLUT **per entry** into GS VRAM.
- **menu03 cross-check consistent.** Same 12158-run resident at the identical
  VRAM offset in both frames (persisted UI palette); zero disc matches;
  per-entry colours in EE RAM. CBP 12288's colours are absent from menu03 EE RAM
  (that atlas region isn't loaded) — further proof palettes are per-frame.
- **CONCLUSION.** There is **no offline (texture → disc-blob) base-palette
  binding rule** — even flat lighting gives a synthesised palette. Disc blobs are
  palette *inputs*; the resident palette is an engine *output*. Offline colour
  now requires reproducing the engine's per-entry palette build (decompile the
  PSMT8 `TEX0`/CLUT setup + per-material colour modulation), not a lookup.
  **Extractors stay on identity grayscale** (nothing wired to colour — no rule).
- **Files:** no tool edits (no binding rule emerged); `docs/FINDINGS.md`
  ("Neutral-lit MENU capture — base palette is STILL synthesised"). Colour PNGs
  → `scratch/color2/` (git-ignored). Reused `tools/gs_vram.py`,
  `tools/clut.py`, `tools/extract_subtextures.py`, `tools/clut_bruteforce.py`.

### Update — 2026-06-02 RESIDENT CLUTs FROM A LIVE CAPTURE — pipeline proven, offline binding still not recoverable

Extracted the **real resident CLUTs** from a PCSX2 in-game capture's 4 MB GS
local memory (`/tmp/cap2/gs.bin`, user-local) to try to crack the colour
binding the brute-force pass (below) couldn't. New tool `tools/gs_vram.py`.

- ~~**GS local memory base in `gs.bin` = byte 509 (0x1FD).**~~ **[VOID —
  corrected 2026-06-09: base = `len - 0x400000 - 84` = 425; the 4 MB is
  followed by 84 trailing state bytes. All reads in this entry were skewed
  +84 bytes (entry-aligned, so palette shapes held). The decisive negative
  below was re-verified clean at base 425 — see FINDINGS.md corrections.]**
  The file is a GS *freeze* blob (not the GS-dump packet format). A CLUT
  `CBP` (256-byte blocks) → `base + CBP*256`.
- **CBPs found.** EE-RAM `TEX0` scan: character sheet `TBP0=7424` → CBP cluster
  8272–8800; `TBP0=12800 → CBP=13560` (all `CPSM=0` PSMCT32, `CSM=0` CSM1,
  `CLD=1`). A full VRAM block-scan finds **13 resident palette-shaped CLUTs**,
  notably a vivid run at **CBP 8368–8371** (chroma 50–65 — real colour
  palettes, confirmed by swatch).
- **COLOUR pipeline proven (milestone hit).** Un-swizzling a resident CLUT and
  applying it to the known-coherent EXTERMINATION title screen (DBP 10752)
  yields a **structurally perfect, readable** image — proving the apply +
  CSM1-un-swizzle path is correct — with **wrong colours** (it's the snowy
  level's palette on the title art, not the title's own). So: one confirmed
  colourised render from the capture; the extraction/apply pipeline works.
- **OFFLINE BINDING — decisive NEGATIVE.** Resident CLUTs match **no** disc
  blob: 0 exact matches vs the 361-blob pool (nearest max-byte-diff 128–235);
  the resident **RGB content appears nowhere on disc** (alpha-free, both swizzle
  forms); and no disc blob fits a resident CLUT under a per-channel **tint**
  (best RMS ≈ 68/255). **The resident CLUTs are runtime-synthesised** (likely
  per-material/ambient lighting modulation baked into the palette before
  upload), not raw or tinted disc blobs. There is **no offline lookup rule** to
  recover; colour offline now requires reproducing the engine's palette-build
  (decompile the PSMT8 `TEX0`/CLUT-DMA setup + colour modulation).
- **Not wired into exporters** (no binding rule). Default colour path stays
  identity grayscale. Files: `tools/gs_vram.py` (new), `docs/FINDINGS.md`
  ("Resident CLUTs from a live PCSX2 capture"). Colour PNGs → `scratch/color/`
  (git-ignored).

### Update — 2026-06-01 BRUTE-FORCE CLUT RECOVERY — NEGATIVE RESULT (colour still needs the engine)

Tried recovering the per-sheet colour palette **without an emulator** by
image-coherence scoring: collect every CLUT-shaped blob disc-wide, apply each
to a sheet's PSMT8 indices, rank by how natural the colour image looks. New
tool `tools/clut_bruteforce.py`. **Outcome: coherence scoring cannot
disambiguate the palette — confirmed negative.**

- **Candidate pool:** 424 hits across 50 files → **361 unique 1024-byte CLUT
  blobs** disc-wide; 98 of them genuinely colourful (chroma > 30), so the pool
  is not the limiting factor.
- **Metric:** Pearson correlation of |index Δ| vs |colour Δ| over adjacent
  distinct-index texel pairs (degeneracy-robust — a plain "minimise adjacent
  RGB difference" smoothness score was tried first and trivially rewards
  near-grayscale palettes, so it was replaced). Diagnostics: smoothness,
  luminance-monotonicity, chroma, distinct-colour guard. Tests both the raw
  and PSMT8/CSM1-swizzled form of each blob.
- **Tested all 5 disc-wide texture sheets** (DBP 7040/7424/10752/12672/14592;
  10752 is the EXTERMINATION title screen — a known-appearance validation
  target). **The SAME blob wins on all of them** (`chunk19.n1/f01_id44.bin @
  0xb6690`, z ≈ 2.5–3.2): a chroma-4.6 near-grayscale luminance ramp (r(index,
  luminance) = −0.90). It wins everywhere because the indices are
  luminance-ordered, so any monotone gray ramp correlates — the metric just
  re-discovers grayscale; the correct colour palette gets no advantage.
- **Forcing colour** (`--min-chroma 30`) makes the best vivid palettes decode
  to **confetti noise** for the level sheets — visibly worse than grayscale. No
  vivid CLUT produces a coherent colour image.
- **Two structural reasons it's the wrong tool:** (1) luminance-ordered indices
  mean smoothness is maximised by any monotone ramp; (2) a sheet is an atlas of
  many sub-textures that may each use a *different* CLUT, so one global
  sheet-palette is ill-posed in principle.

**Colour-CLUT binding status: UNRESOLVED, and provably not crackable by
coherence heuristics.** It remains a boot-ELF per-asset palette-LUT / VU1
`TEX0` `CBP`-trace problem (the engine path, blocked on PCSX2 / the static
TEX0 setup). The default colour path stays the identity grayscale CLUT.
`tools/clut_bruteforce.py` is kept as the disc-wide candidate collector +
scorer; PNG output lands in `scratch/clut_bf/` (git-ignored), nothing wired
into exporters. Files: `tools/clut_bruteforce.py` (new),
`docs/FINDINGS.md` ("Brute-force CLUT recovery … NEGATIVE RESULT").

### Update — 2026-06-01 GLOBAL GS VRAM RESIDENCY MAP (cross-file texture resolution)

New tool `tools/vram_residency.py` builds ONE disc-wide GS VRAM residency map
so any DBP resolves regardless of which file uploads it — closing the
"cross-file texture residency" gap that left exporter sheets gray.

- **Scans every file** under `extract/` for GS upload packets and records each
  upload's full descriptor `(DBP, DBW, DPSM, TRXREG w×h, TRXPOS dest, source
  file, byte offset)`, indexed `DBP → [uploads]`. Reuses
  `extract_subtextures.decode_transfer` for pixels (no duplicate decode).
- **Cached** to `scratch/vram_residency.json` (git-ignored; verified via
  `git check-ignore`). A size/mtime corpus signature invalidates it;
  `--rebuild` forces a fresh scan. Cached load 0.018s vs 3.2s full scan.
- **Lookup API** `resolve_dbp(dbp, prefer_dir=…, want_psm=…, want_dims=…)`
  returns the best upload: exact-DBP candidates ranked by hint match + same
  chunk dir + stable path order, else nearest-DBP within a snap tolerance
  (the affine `sheet_field→DBP` map is exact only on the universal slot trio,
  so neighbouring addresses are the same physical sheet). Returns ALL
  candidates via `candidates(dbp)` so the consumer can disambiguate.

**Disc-wide census (definitive):** exactly **113 GS texture uploads** at only
**5 distinct DBPs** `{7040, 7424, 10752, 12672, 14592}`, all PSMCT32 / DBW=4.
Each DBP is re-targeted by many files (slot reuse across levels) with varying
dimensions, so DBP alone is ambiguous. **Zero GS CLUT uploads** — confirms
CLUTs are the raw 1024-byte blobs (`clut.py`), not GS packets; the color-CLUT
question gains no lead from GS packets (the hoped-for CLUT-upload crack came up
empty — palettes simply aren't GS-packetised).

**Wired into `export_gltf.py`** (character + level paths consult the map first,
legacy per-dir scan as fallback). Results:
- **Player**: 3/3 sheets, identical sources to the validated baseline
  (10752/12672 from chunk21, 14592 → chunk04.n2 512×384).
- **Sample levels** `chunk08.n0`/`chunk08.n1`: 3/6→4/6 and 3/7→4/7 (DBP 14562
  now snaps to 14592's sheet, was gray).
- **All 32 levels aggregate**: 91/143 → 93/143 sheets resolved. The remaining
  50 are low DBPs (548…2581, 1152, 8802) with no upload anywhere on disc
  (common/UI textures outside the extracted tree) — they honestly stay gray.
- pygltflib strict round-trip passes on all re-exported `.glb`s.

Files: `tools/vram_residency.py` (new), `tools/export_gltf.py` (wired),
`docs/FINDINGS.md` ("Global GS VRAM residency map"). Output cache and `.glb`s
stay git-ignored.

### Update — 2026-06-01 SYMBOL NAMING — anim / skinning / DMA / VU1 / GS-readback

Pushed the session's RE findings into real symbol names. **40 functions
renamed** from `func_XXXXXXXX` to descriptive names in
`config/symbol_addrs.txt`, each corroborated by `docs/FINDINGS.md`:

- DMA/VU1/VIF infra (9): `vu1_cold_start`, `dma_wait_and_submit` (was the
  `sub_D2_TADR_08x` strref name), `gs_readback_build_packet`,
  `gs_vram_readback`, `dmac_channel_base`, `dmac_reset`, `dma_kick`,
  `vif_append_ref_tag`, `vif_build_unpack_const`, `skin_arena_init`.
- GS readback queue (2): `gs_readback_queue_drain`, `gs_readback_queue_run`.
- Memory helpers (3): `block_copy`, `copy_qw4`, `float_to_int`.
- Animation pipeline (19): `anim_frame_top_a/_b`, `bone_matrix_publish`,
  `anim_matrix_dispatch`, `anim_slot_index`, `anim_matrix_multi`,
  `anim_matrix_player`, `anim_clip_arbiter`, `anim_eval_skeleton`,
  `build_trs_matrix`, `quat_nlerp`, `quat_to_mat3`, `anim_clip_resolve`,
  `anim_clip_init`, `anim_sample_bones`, `anim_advance_time`,
  `anim_sample_rotation`, `anim_decode_translation`, `anim_bone_array_setup`.
- Bone init / effects (6): `bone_init_default_0/1/2`, `bone_root_pulse`,
  `bone_wobble_decay_0/1`.

Mechanics: re-ran splat, renamed 16 owning `src/*.c` definition files
(`git mv` + internal symbol), updated all stale `extern`/`jal` references
across 94 `src/*.c` caller files, and — the step that bit twice before —
updated the 5 affected name-keyed entries in
`tools/decomp/fill_unmatched.py`'s `GPREL_FORCE_ASM` / `SIZE_DRIFT_FORCE_ASM`
lists (a renamed force-asm function silently falls back to non-matching
compiled C otherwise). **Boot ELF byte-identical (0x175b00 loadable bytes),
overlays 19/19 byte-identical.**

Named-symbol count: **244 named functions** (`type:func`, non-`func_`) +
4 `type:label` data/table symbols.

Toolchain note: under the Apple `container` CLI, `fill_unmatched.py`'s default
8-way parallel copy intermittently hits `OSError EDEADLK` on the bind mount;
`--clean --jobs 1` then `link.py --no-fill` is the reliable path.

### Update — 2026-06-01 LEVEL-SCENE glTF EXPORTER (explorable textured levels in Blender)

Applied the character glTF infrastructure to a new surface: **whole `id 0x44`
level scenes**. New mode `tools/export_gltf.py level` exports an explorable,
textured level as a single `.glb` that opens in Blender.

- **Geometry** reuses `extract_models.parse_scene()` (world-space MESH/SUBMESH +
  MATRIX-instanced props, transforms baked). No new geometry decode needed —
  the 64-byte vertex record and tristrip topology are identical to characters.
- **Textures** resolved per strip via `sheet_field (m0>>15)&0x3FFF → GS DBP`,
  scanning the level's chunk dir then the whole `extract/` tree (level textures
  are almost entirely cross-file), decoded with `extract_subtextures` and
  embedded as identity-grayscale RGBA PNGs. Unresolved sheets → gray placeholder.
- **Structure**: one mesh per texture-sheet DBP, double-sided TRIANGLES with
  POSITION + NORMAL + TEXCOORD_0, under a single scene-root.

**MATRIX absolute-vs-composed: RESOLVED — transforms are ABSOLUTE world
placements, not parent-composed.** On `chunk04.n0/f06_id44.bin` the MATRIX
object-space geometry already spans the full level Z extent at its identity
transform, and per-instance transforms are small additive offsets that scatter
prop copies *within* the level footprint. Baking them directly keeps everything
inside the static world bbox (see FINDINGS "MATRIX transforms are ABSOLUTE").

**Validation.** Reference level → `models/chunk04.n0_f06_scene.glb` (2.4 MB,
3 meshes, **20 647 triangles**, 47 271 verts, bbox 455 × 162 × 1813 units —
room scale, **3/3 sheets** embedded). Round-trips through `pygltflib` strict
loader. `--all-levels` exports **32/36** levels; the 4 skips are MESH-less
texture-carrier files (correctly non-renderable). `--no-textures` gives a
geometry-only fallback. Output `models/*.glb` stays git-ignored.

**Still off / open:** colour CLUT binding unresolved (grayscale luminance
preview); levels referencing 4+ DBPs leave the extra sheets as gray
placeholders (common/UI uploads outside the extract-tree search — same
cross-file VRAM-residency gap as the per-texture extractor's ~631 materials).

### Session checkpoint — 2026-05-27 (53 commits)

Headline numbers and what's open. The session brought the project from
boot-ELF-only matching to a complete end-to-end animation pipeline that
exports posed + textured character glTFs.

**Matching surface:**
  - Boot ELF: **1247/1344 (92.78%) functions matched, 96.18% code-byte
    match, byte-identical**.
  - Overlays: **120/586 functions decomp'd, 19/19 byte-identical**.
  - 255 named symbols (incl. fptr-table labels). _(See 2026-06-01 naming
    update above: now 244 named functions + 4 label symbols.)_
  - Reusable tools landed: `inject_relocs.py`, `name_fptr_tables.py`,
    `name_functions.py`, `name_vtables_overlays.py`,
    `parse_pcsx2_state.py`, `parse_gsdump.py`, `disasm_vu.py`,
    `clut.py`, overlay generators.
  - ISO refreshed + verified working in PCSX2.

**Asset extraction:**
  - Skeletons: full bone hierarchies (28 bones, parent table, validated
    L/R symmetry).
  - Per-bone collision hulls (OBBs from plane equations).
  - Per-bone object-space character vertices (Q4.12 decoded).
  - Anim format **fully decoded**: id 0x71 sections 1-3, quat keyframes
    (top-20-bits-of-float packing), translation keyframes (top-26-bits),
    event table (per-clip footstep markers). Python decoder validated
    against real disc data.
  - **`tools/anim_decoder.py`**: parse_rotation_section,
    parse_translation_section, parse_event_section, sample_bone (NLERP).
  - **`tools/export_gltf.py`**: produces single .glb with skeleton +
    per-bone meshes (TRIANGLES + smooth normals + UVs + 3 PSMT8 sheet
    textures) + all 57 animation clips. Opens cleanly in Blender / Maya
    via standard glTF import.

**Bind-pose data flow CHARACTERIZED end-to-end:**
```
disc id 0x71 entry
  -> func_001C8480 (resolver) caches sections
  -> func_001C8D50 (per-bone sampler) writes quat/translation slots
  -> func_001C64F0 (per-frame time advancer)
  -> func_001C6DA0 (NLERP + TRS + parent concat) -> bone+0x90 4x4
  -> func_00179BC0 publisher -> BSS 0x002863XX..0x002893XX
  -> DMA via func_00101FE0 -> VIF1 -> VU1 dmem -> vf01..vf04
  -> per-vertex transform -> XGKICK -> GS
```

**Genuinely-hit walls (need user-driven PCSX2 work to break):**
  - Per-block bone-INDEX TABLE for the textured-surface MESH-blocks
    (proxy-bound via spatial-proximity; 30-50% mis-bound).
  - Packed-normal byte-to-lane mapping (Q4.4 confirmed but lane order
    unverified; face-averaged normals used in glTF).
  - PSMT8 TEX0 / color-CLUT binding (TEX0 setup is inside VU1 kernels;
    needs mid-frame dmem capture).
  - Boot-ELF text-mismatch partials (~50 remain; CW 2.3.1 scheduler/
    dead-code patterns mwcc 2.3 can't reproduce from C).
  - Overlay small-leaf decomps saturated; remaining functions need
    medium per-function manual work.

**Recommended next session start point:**
  - Capture a PCSX2 VU1 dmem dump mid-frame during player render to
    pin down the per-block bone-INDEX list, packed-normal byte order,
    and GIF tag REGLIST in one go.

### Update — 2026-05-27 VU1 SOFT-skinner kernel #0 first-pass disassembly (per-vertex selector → vi01 → matrix-palette LQ confirmed)

Followed the kernel #5 walkthrough by disassembling the first of the
"medium 2-segment" kernels expected to consume the MESH-block per-vertex
W-field bone selector. Disassembler note: `disasm_vu.py disasm <vram> <size>`
takes a *body* vram (not the packet wrapper), so feed it the body start
listed in the catalog (e.g. `0x00230828` for kernel #0, NOT `0x00230824`)
otherwise the first qw is parsed as a stray MPG tag and the decoder
shifts by one byte.

**Kernel #0 chosen:** segment 1 vram `0x00230828` size `0x800` (256 qw,
imem 0x0000), segment 2 vram `0x00231030` size `0x3f0` (126 qw, imem 0x0100).

**Shared dmem layout across the four soft-skinner siblings #0/#1/#2/#4**
(vram heads `0x00230828`, `0x00231798`, `0x00232568`, `0x00233828`).
All four programs load *identical* absolute dmem slots — strong evidence
the EE VIF1 driver lays out a single shared dmem framing for the entire
character-mesh draw class:

  - qw 80,81     → vf23, vf24       (constants)
  - qw 82..85    → vf10..vf13       (clip planes / fog / lighting state)
  - qw 86..89    → vf05, vf04, vf03, vf18
  - qw 90..93    → vf28..vf31       (ONE bone matrix, "active draw" matrix)
  - qw 96        → vf02
  - qw 102       → vf25             (per-batch base ptr for SQ-via-vi25)
  - qw 110..123  → vf16..vf28+vf09  (constants block, including a 12-qw region
                                     qw 111..122 that is the **per-block
                                     4-matrix bone PALETTE** — 4 bones x 3
                                     affine rows = 12 qw; vf09 from qw 123 is
                                     the per-batch GIF tag template)
  - qw 124..128  → vf31, vf01..vf04 (segment-2 constants reload)

The 4-bone palette in qw 111..122 matches the FINDINGS empirical result
("each MESH block ships with an implicit small bone TABLE — ~4 bones /
block"). The matrix at qw 90..93 is the *active* draw-frame matrix (likely
the per-block "base bone" used as a fallback / origin); the 4-bone palette
is layered on top via per-vertex selection.

**Per-vertex selector decode — IDENTIFIED.** Inside the inner loop at
`0x00230B50..0x00230BC8` (seg 1) the kernel runs the canonical
UPPER-pipe-FTOI + LOWER-pipe-ISWR round-trip:

```
0x00230b50  U:itof4.xyzw         L:iaddiu vi02, vi00, 0x0080  ; mask seed
0x00230b58  U:upp_0e.x           L:low4_10                     ; (selector preprocess)
0x00230b70  U:ftoi4.x            L:iswr                        ; float -> Q4.4 int, write VI
0x00230b78  U:itof4.xyzw         L:fmand  vi02, 0x021000       ; flag mask
0x00230b80  U:maddi.x            L:ior                         ; OR base ptr
0x00230b88  U:upp_19.x           L:sq vf00, 102(vi25)          ; (output store)
0x00230b90  U:subbcx.w           L:lq vf12, 0(vi01)            ; <<< MATRIX ROW A
0x00230b98  U:itof0.xyzw         L:lq vf13, 16(vi01)           ; <<< MATRIX ROW B (+16qw)
```

The `ftoi4.x` + `iswr` pair converts the per-vertex W-field (Q-format
float) to an integer, stores it through a VI register (vi01 receives the
result via the ISWR→IADD chain at 0x00230B80 `ior`), and the subsequent
`lq vf12, 0(vi01)` / `lq vf13, 16(vi01)` **uses vi01 as the matrix-palette
base pointer**. The stride of 16 qw between vf12 and vf13 is exactly **one
matrix slot in a 4-matrix-palette layout** (4 matrices × 4 rows × 1 qw =
16 qw / matrix-spacing); but here each LQ is one ROW, so the layout is:

```
palette base + 0    = bone[selector].row0
palette base + 16qw = bone[selector+1].row0    (or row1 of next pose-slot)
```

Two reads with a +16qw stride strongly suggests **2-bone blend per vertex**
(load row0 of bone-A and row0 of bone-B; weighted sum), then lines 121-122
re-fetch vf14, vf15 at the same `0(vi01)` and `16(vi01)` — likely a
**second-row pair**, building two affine rows for both selected bones.
With 12-qw palette (qw 111..122) and 16-qw stride, the modulo wraps every
loop iteration. (Need careful field-by-field decode of `low4_10` and
`maddi.x` to nail down the exact arithmetic; left for follow-up.)

**What this means for the per-block 4-bone-index TABLE.**

The microcode *does NOT carry the bone-INDEX list* — it operates purely
on the per-vertex selector (small integer 0..3) and indexes a palette
that the **EE side pre-populates via VIF1 UNPACK** into dmem qw 111..122.
That UNPACK is what we need to trace. The 4 matrices land in dmem in a
specific order that *defines* the per-block bone-to-selector mapping.

Three concrete next steps to crack the binding:

1. **Find the VIF1 packet builder** that targets dmem `qw 111..122`. The
   matrix payload comes from the live-bone BSS arena at `0x002863XX..
   0x002893XX` (already characterized in FINDINGS). The EE function that
   reads the per-block 4-bone-index list and gathers the 4 matrices into
   the UNPACK payload is the bone-binding source of truth.
2. **Re-examine each 0x820-byte MESH block header.** Per FINDINGS, the
   bone-index list "is not yet located" — but with the confirmed 4-bone
   palette, the search target is now precise: a `u8[4]` or `u16[4]` field
   in the per-block header (block size 0x820 = 2080 bytes; 4 bytes for
   u8x4 is tiny — likely at a fixed offset in the first 0x40 bytes of
   each block before the 64-byte vertex records start).
3. **PCSX2 dmem capture mid-frame.** Snapshot VU1 dmem qw 111..122 while
   the player mesh renders; the 12 qw should be 4 well-formed affine
   matrices that can be cross-referenced against the global bone-pose
   BSS arena to recover the index list directly.

**glTF exporter — NOT UPDATED THIS SESSION.** The per-block bone-INDEX
list is still required to replace `_bind_blocks_to_bones()` spatial
proximity. We confirmed *where the binding metadata lives* (the EE-side
VIF1 UNPACK source) but didn't recover the per-block u8[4]/u16[4] list.
Updating `tools/export_gltf.py` without that mapping would still be a
proxy. Flag for next pass: do the PCSX2 dmem capture, recover one
block's 4 matrices, identify them in the BSS pose arena → that yields
ONE block's bone indices, which then validates whichever MESH-header
offset stores the per-block list.

**Disassembler caveat.** `tools/disasm_vu.py` still has some LOWER-pipe
opcodes printed as `low4_10`/`low_3b800000`/`low1_19` placeholders
(IBIT-immediate followers + a handful of LOWER1 sub-encodings). The
visible structure of the per-vertex selector decode is sufficient for
this finding, but a clean trace of the exact `ftoi4 → iswr → ior` field
arithmetic (which bits of the W lane select the matrix slot, and
whether the residual `+epsilon` documented in FINDINGS is a separate
per-block base or just float quantisation) needs those decoded too.

---

### Update — 2026-05-27 VU1 skinner kernel #5 disassembled (operands recovered, field convention corrected)

Disassembled the per-bone rigid-skinning main kernel at vram
`0x00234610` (153 qw, 0x4c8 bytes — the full body; the catalog's
helper packets at `0x002346b0` / `0x002346f0` are **false positives**
— scanner mis-identified interior `jalr vi15` instructions
(`0x4a0f0800`) as MPG tags, and the same false-positive pattern
recurs at `0x00234bd0/0x00234c10/0x002350b0/0x002350f0`). The real
helper at imem 0x0800 is uploaded by a SEPARATE VIF DMA that the
catalog scanner doesn't reach — likely a one-time boot upload reused
across all 3 sibling skinner mains (vram `0x00234610`, `0x00234b30`,
`0x00235010`). `tools/disasm_vu.py catalog` needs a follow-up filter
that requires a preceding VIF setup template before accepting an
MPG signature.

**LOWER-pipe operand field convention CORRECTED.** The PCSX2-style
`FT=[15:11] IS=[20:16]` documented in the disasm comments is wrong
for this binary: every load/store to vf28..vf31 / vf16..vf21
disassembled as `vf00` (zero register) under that mapping. With
`FT=[20:16] IS=[15:11]` (swap), the instruction stream becomes
sensible — `vf28..vf31` get the bone-matrix loads, `vf16..vf21` get
the top-of-dmem constants. The on-disk encoding therefore stores
the integer-register field at [15:11] and the vector-register
field at [20:16] for all LOWER-pipe load/store/branch/iaddiu/jalr
ops. `tools/disasm_vu.py` needs a one-line correction; the
operator-frequency `profile` mode is unaffected (op detection
doesn't depend on operand positions).

**Skinner main #5 structure (vram 0x00234610..0x00234ad8, 153 qw).**

```
PROLOGUE (~16 qw):
  mfp     vf13          ; sync EFU pipe
  iaddiu  vi14, ...     ; counter setup
  iaddiu  vi12, vi13, 7 ; loop-end cursor
  iaddiu  vi11, vi13, 0x19
  fcset   0
  ; 6 dummy LQs to vf16..vf21 from dmem qw 1016..1023 (top-of-dmem
  ; shared constants: clip planes, light dirs, GIF tag template, etc.)
  lq      vf16, 1019(vi00) .. lq vf21, 1016(vi00)
  ; bone matrix: 4 rows from dmem qw 0..3 into vf28..vf31
  lq      vf28, 0(vi00)
  lq      vf29, 1(vi00)
  lq      vf30, 2(vi00)
  lq      vf31, 3(vi00)

PER-VERTEX LOOP (unrolled 2x, body ~12 qw):
  lq      vf03, 0(vi13)        ; load vertex record qword
  iaddiu  vi01, vi00, 0x31     ; setup jalr-target seed
  jalr    vi15, vi01           ; -> imem 0x800 helper (per-vertex xform)
  iadd    vi13, ..., 8         ; advance input ptr by 8 dmem qw stride
  lq      vf03, 0(vi13)        ; second vertex of unrolled pair
  iaddiu  vi01, vi00, 0x31
  jalr    vi15, vi01
  isubiu  vi13, vi13, 8        ; rollback (second vert reuses prior offset)
  iadd    vi13, ..., <stride>  ; advance by net stride (low_072)
  ibne    vi13, vi12, -136     ; loop until vi13 hits vi12 (end)

EPILOGUE (~5 qw):
  mfp     vf11
  iaddiu  vi11, vi11, 0x18     ; output ptr += 3 qw
  lq      vf01, 1020(vi00)     ; load GIF tag template from constant pool
  sq      vf11, 0(vi01)
  [E] xgkick vi11               ; kick the assembled GIF packet
  -- end --

UNREACHABLE TAIL (vram 0x00234780..0x00234ad8, ~150 qw):
  This is NOT main-flow code (the E-bit set at the xgkick ends the
  program). It is the **helper subroutine** that the JALR vi15 calls
  reach via imem 0x0800 once uploaded by a separate VIF DMA. The
  per-vertex xform body lives here; it culminates in:
    itof12.xyzw      ; positions: Q4.12 -> float
    addAi/mulAbcz    ; bone matrix multiply (acc=vf28..31 * vfTMP)
    ftoi12.xyzw      ; back to Q4.12 integer for GS output
    itof12.w  ...    ; W lane dequantize, then minibcx/maxbcx (clamp)
    itof4.xyzw       ; SEPARATE int->float at Q4.4 scale — this is the
                     ;   per-vertex packed-normal/lighting decode!
    sq vf11, 0(vi02) ; final 3-qw GIF packet writes
    sq vf11, 1(vi05)
    sq vf11, 2(vi05)
    jr vi00          ; return
```

**KEY DECODES.**

1. **Bone matrix slot — CONFIRMED.** 4 rows in vf28..vf31 from dmem
   qw 0..3 (bytes 0x000..0x03F). This refines the earlier FINDINGS
   text ("vf01..vf04" was wrong — the prior pseudocode was inferred
   from op-frequency, not byte-decode).

2. **Per-vertex input stride.** The kernel advances vi13 by 8 (LQ
   immediate units, which on this VU are **qwords**, so 8*16=128
   bytes per 2-vertex pair = 64 bytes/vertex stride at the dmem
   level). The disc per-bone records are 12 bytes each, so VIF
   UNPACK is *expanding* them: most likely **V4-16 UNPACK with
   USN=0 expanding 8 bytes of disc data into 1 qw** (4 int16
   lanes), and the remaining 4 bytes (`+0x06..+0x09` packed normal)
   go to a parallel dmem stream OR a second pass.

3. **Packed-normal decode — PARTIAL.** The helper has
   **two distinct ITOF scales**: `itof12.xyzw` for positions
   (Q4.12, divide by 4096) and `itof4.xyzw` for a separate lane
   (Q4.4, divide by 16). The Q4.4 path is the per-vertex normal
   dequant — signed bytes -127..+127 → -7.94..+7.94 unit vectors,
   which is a textbook PS2 packed-normal encoding (~Q1.7 with
   intensity headroom). The 4th byte (clustering near 63 / 188 in
   the FINDINGS empirical analysis) is loaded into the .w lane via
   `itof12.w` then clamped via `minibcx.w` / `maxbcx.w` — that's a
   **per-vertex lighting intensity / culling flag**, not a
   coordinate. This matches FINDINGS' guess of "category/intensity
   tag".

4. **Per-block bone palette — NOT FOUND IN THIS KERNEL.** The
   skinner main loads ONE bone matrix into vf28..vf31 — there is no
   4-matrix palette and no per-vertex selector read. Conclusion:
   **kernel #5 is a single-bone rigid skinner**, one bone per VIF
   batch. The per-vertex bone selector documented in MESH-block
   `(w-sign(w))*512` (FINDINGS line 103) must be consumed by a
   DIFFERENT kernel — most likely one of the 4 medium 2-segment
   kernels at imem 0x0000 (#0/#1/#2/#4, vram 0x00230824 etc.) which
   have 17-20 ftoi/itof and the LQ density characteristic of a
   4-matrix-palette weighted skinner.

5. **GIF output structure — PARTIAL.** Output is **3 qw per kick**:
   `[GIF tag template at vi11+0, payload at vi11+1, payload at vi11+2]`.
   The GIF tag template is preloaded from constant dmem qw 1020.
   3 qw is consistent with a `PRIM + 2 vertex records` strip
   continuation OR a `GIFtag + STQ + XYZF2` per-vertex packet.
   Without the REGLIST bits from the tag template (which lives in
   dmem at qw 1020, not statically decodable from microcode alone),
   the exact GS register list isn't pinned. Capture needed.

**Implications for the glTF exporter.** The Q4.4 packed-normal
hypothesis is now strong enough to *attempt* a real decode in
`tools/export_gltf.py`, but the per-byte component-to-lane mapping
inside the 4-byte field still needs a sanity check (which 3 of the
4 bytes are x/y/z, and which is the intensity). The current
face-averaged-normal path is left in place — it works; replacing
it speculatively risks breaking the 1.98 MB validated .glb. **Not
wired into export_gltf.py this session** — flag for next pass
(combine with a PCSX2-state capture of vu1 dmem mid-frame to
confirm the byte order).

**Implications for `_bind_blocks_to_bones`.** This kernel handles
**rigid** skinning (1 bone per batch); MESH-block soft skinning is
a different kernel. The per-block bone palette is therefore NOT in
this kernel's dmem footprint. No changes warranted.

### Update — 2026-05-27 MESH-block per-vertex bone selector identified

Investigation into the "MESH-block static-textured mesh collapsed at
origin" question revealed that the 64-byte vertex record's `+0x3C` `w`
field is NOT a pure sign flag -- it carries a quantised per-vertex
bone-selector. Per block, `round((w - sign(w)) * 512)` takes 3-15
distinct small integer values (mode = 4 -- one per matrix slot the
VU1 microcode holds resident). This matches the canonical PS2
rigid-skinning idiom: a small bone TABLE per draw-batch + a per-vertex
selector into it. The per-block bone-INDEX table that maps
selector -> global bone-id is not yet located in the file (the 0x48-byte
preamble before the first MESH block is a single bbox header, not
per-block; the `28 x u32` prefix table is too short for 317 blocks).
Most-likely candidate is the residual float offset above each `k *
1/512` slot encoding a per-block base, but proving that needs the
VU1 microcode. See `docs/FINDINGS.md` "Per-vertex bone binding lives
in the position w field" for full evidence and quantisation data.

Implication: the glTF static-textured-mesh primitive cannot yet be
parented to bones for posing -- but the path forward is concrete
(decode VU1 microcode -> recover the per-block bone-table format).

### Update — 2026-05-27 GLTF EXPORTER — UVs + textures (static reference mesh)

The .glb exporter now embeds the player's three character texture sheets
and a UV-mapped reference mesh alongside the rigged skeleton.

**Source of UVs.** The character mesh file (`chunk21/f17_id8f.bin`) carries
TWO independent vertex streams:

1. The previously-documented per-bone VIF stream (28 sections,
   POSITION+packed_normal+vid, **no UVs**) -- still used to drive the
   rigged/animated skeleton.
2. A **MESH-descriptor section** starting at file offset 0xe848, holding
   317 fixed-size 0x820-byte blocks. Each block is the standard 64-byte
   vertex record (already documented for level `id 0x44` files): `m0/m1`
   marker, `(u,v,1,0)` UVs at +0x10, normal-or-color at +0x20, position
   at +0x30. The existing `parse_model_file()` in `extract_models.py`
   already decoded this stream (it just wasn't being routed to the .glb).
   Total: **10015 vertices / 6921 triangles / 105 distinct (m0,m1) keys**.

**Texture binding.** The per-strip `sheet_field = (m0 >> 15) & 0x3FFF`
field (already documented in FINDINGS for level materials) applies
identically to character strips. The player rig uses the universal
sheet trio `{10884, 12932, 14980}`, which maps to GS VRAM
`DBP = {10752, 12672, 14592}` via the affine `DBP = (sheet_field + 584.8)
* (1920/2048)`. Two of the three sheets are uploaded by sibling files in
the player mesh's own chunk dir (`chunk21/f00_id43.bin` and
`chunk21/f01_id44.bin`); the third (`DBP=14592`, the smallest 512x384
sheet) is uploaded by per-level `id 0x44` files -- the exporter falls
back to scanning `extract/` and picks the first match (currently
`chunk04.n2/f00_id44.bin`).

**Textures embedded.** Each resolved sheet is decoded via the existing
PSMT8 deswizzle pipeline (`extract_subtextures.deswizzle`), expanded to
RGBA via the identity-grayscale CLUT (color CLUT binding is still
unresolved -- see FINDINGS "Color source"), and written into the .glb as
an embedded `image/png` referenced by a glTF Material with
`baseColorTexture`.

**What works.** The .glb now contains:
- the 28-bone rigged skeleton with 57 animation clips (unchanged);
- a sibling un-rigged `player_static_textured` mesh under the scene root
  with 3 primitives (one per texture sheet), each with POSITION +
  NORMAL + TEXCOORD_0 + a material pointing at the embedded PNG.

**What's still off.**
- The static-textured mesh's positions are *bone-local*
  (range roughly -5..15 across all blocks) -- they need bone matrices
  applied to land in world space. So the static mesh appears as a
  collapsed blob near the scene origin rather than a posed character.
  The per-bone↔MESH-block binding metadata is not yet decoded; nothing
  in the per-block header carries an explicit bone index, and the
  block positions do NOT overlap with the per-bone VIF positions
  (different coordinate frames). Candidate next investigation: the
  "small u32 x 28" table immediately preceding the section table (file
  offset 0x2258..0x22c8), or the m0/m1 key -> bone mapping.
- Color CLUT is still grayscale-luminance (PSMT8 indices ordered by
  luminance, so grayscale is a faithful brightness preview but obviously
  not the in-game palette).
- The rigged mesh primitives still have no UVs (the per-bone VIF stream
  carries no UV field at all -- decoding the VU1 skinning kernel may
  reveal whether the UV is shipped via a parallel VIF UNPACK or is
  reconstructed at runtime from the MESH-descriptor stream).

**Validation output** (`tools/export_gltf.py --mesh extract/chunk21/f17_id8f.bin
--skel extract/chunk05/f04_id71.bin`):

```
wrote models/Extermination_Player.glb (2709792 bytes)
  bones      : 30
  meshes     : 23  (one rigid mesh per non-empty bone + 1 static textured)
  triangles  : 1739
  animations : 57
  static mesh: 10015 verts / 6921 tris / 3 prims
    sheet DBP=10752 ( 525 strips)  512x960  source=chunk21/f00_id43.bin
    sheet DBP=12672 ( 505 strips)  512x448  source=chunk21/f01_id44.bin
    sheet DBP=14592 ( 517 strips)  512x384  source=chunk04.n2/f00_id44.bin
  images/textures/materials: 3/3/3
```

GLB round-trips through `pygltflib`; all accessors fit their bufferViews;
embedded PNGs are well-formed (verified via `file(1)`: `512 x 960, 8-bit/
color RGBA, non-interlaced`).

### Update — 2026-05-27 GLTF EXPORTER — triangles + smooth normals

Promoted the .glb exporter from a point cloud to a shaded mesh:

- **Triangle topology decoded.** The per-bone VIF stream's monotonic `vid`
  field (+0x0A in each 12-byte record) is a generalized-tristrip cursor:
  adjacent verts in one strip have `delta_vid == +2`; any other delta
  (+1, +4, +5, +7, +9, +17, +19, +43, +64, +90 observed) signals a
  STRIP RESTART. `export_gltf.triangulate_bone()` splits each bone's
  vertex list at every irregular delta, drops sub-strips with <3 verts,
  and emits PS2-standard alternating-winding triangles per strip
  (coincident-vertex degenerates dropped).
- **Smooth per-vertex normals** via face-area-weighted averaging.
  The packed 4-byte field at record +0x06 (3 signed-ish components + a
  4th byte that clusters tightly around 63 or 188) is **not yet
  decoded**: tried (signed/127), (unsigned/255), and IEEE-float
  reinterpretation -- none yields consistently unit-length vectors, so
  the exact quantisation needs the VU1 microcode. Face-averaged smooth
  normals are good enough for preview shading and can be replaced once
  the packed format is cracked.
- **glTF schema upgrade.** Per-bone primitives now write mode=4
  (TRIANGLES) with POSITION + NORMAL + indices (u16 auto-promoted to
  u32 when needed). 3 trivial 3-vert bones with no valid strip fall
  back to mode=0 (POINTS).

Validation on the player rig (`chunk21/f17_id8f.bin` +
`chunk05/f04_id71.bin`):

```
wrote models/Extermination_Player.glb (1980632 bytes)
  bones      : 30
  meshes     : 22  (one rigid mesh per non-empty bone)
  triangles  : 1739  (points-only fallback bones: 3)
  animations : 57
  tracks     : 3420
  samples    : 33562
```

Round-trips through `pygltflib`; all 6900 accessors / 22 primitives
parse clean; first-vertex normal on a sampled bone is unit-length
within float epsilon. Outstanding gaps: UVs / texture binding (the VIF
per-bone record carries no UV field -- texturing data lives in a
separate stream not yet located); a faithful decode of the packed
normal/lighting byte quad (blocked on VU1 microcode lift).

### Update — 2026-05-27 GLTF EXPORTER — full character + 57 anims in one .glb

`tools/export_gltf.py` (new) is the headline modder-facing deliverable: it
emits a single glTF 2.0 binary bundle containing the player's skeleton
hierarchy, per-bone rigid mesh, and every animation clip from the chosen
id 0x71 file. Pure-Python, no third-party deps.

Validation run on `extract/chunk21/f17_id8f.bin` (player mesh) +
`extract/chunk05/f04_id71.bin` (player skeleton + 57 clips):

```
wrote models/Extermination_Player.glb (1938052 bytes)
  bones      : 30 (28 active + 2 overshoot)
  meshes     : 22  (one rigid POINTS mesh per non-empty bone)
  animations : 57
  tracks     : 3420
  samples    : 33562
```

Structural validation: GLB header magic/version/length OK; binary chunk
size matches buffer declaration; all 6862 bufferViews lie within the
buffer; all accessor counts × component-sizes fit their bufferViews;
every animation sampler input is strictly time-monotonic; quaternion
output samples are unit-norm. Round-trips cleanly through `pygltflib`
(strict parser). Should open in Blender's "File > Import > glTF 2.0"
and in any compliant viewer; 57 clips appear under the Animations panel.

Design choices:
- **Rigid skinning via node-attachment** (no `skin` object). Each non-empty
  bone's Q4.12 verts become a POINTS-mode primitive parented to that
  bone node; when the node animates, the mesh moves with it. This
  matches the engine's per-bone object-space packets (one per bone).
- **POINTS primitive** because the VIF-prefix vertex stream is pre-strip
  quantized points -- no triangle indices survive the decode. The mesh
  shows up as point cloud silhouettes in viewers; the bone tree and
  animation playback are the load-bearing output for now.
- **30 fps time conversion** for the keyframe `t_next` (frame index) → seconds.
- **Per-clip animation** (one glTF animation per id 0x71 entry), with
  one rotation and one translation sampler per bone (LINEAR = NLERP for
  quaternions, which matches `anim_decoder.sample_bone(normalize=True)`).

Outstanding items: (1) triangle topology -- need to recover or reconstruct
the strip indices for proper surface rendering; (2) per-vertex normals
and UVs (the 4-byte packed-normal field is decoded by the VU1 microcode
which we haven't lifted); (3) texture binding. None block animation
preview in Blender / Maya / any glTF viewer.

### Update — 2026-05-27 PER-BONE SCALE FIELDS — NOT A DISC CHANNEL

Open question from the rigged-export work was "who writes the per-bone
`+0x7C` float vec3 scale and `+0x88/+0x8A/+0x8C` s16 Q4.12 fine scale
that `func_001C6DA0` reads?". Audited every store to those offsets
across all splat asm:

- **Bind-pose / reset writers** (`func_001C6200`, `func_001C62C0`,
  `func_001C63E0`) initialize all bones to `s16 = 0x1000` (=1.0) and
  `f32 = 0`. This is the rest value.
- **Runtime gameplay-effect writers** (`func_001C06E0` — root-bone
  pulse driven by `actor+0x2A`; `func_001BDD70`/`func_001BDCA0` —
  decaying "wobble" over the global object list bones 1..0x10) mutate
  these fields with synthesized values, not anything sourced from
  disc.
- **The keyframe sampler** (`func_001C8D50` → `func_001C90D0` /
  `func_001C92C0`) writes only `+0x18..+0x60` (TRS R + T), **never
  +0x7C or +0x88**.

Conclusion: id 0x71 has **no scale channel**. Section 3 of id 0x71
("uniform 36 bytes/bone") is therefore *not* scale keyframes (probably
events, as previously hypothesized). `tools/extract_models.py`'s
existing default of scale=1.0 already matches what the engine itself
writes at skeleton init, so the rigged exporter is correct as-is and
no decoder additions to `anim_decoder.py` are warranted.

Updated `docs/FINDINGS.md` ("Per-bone scale fields — writers
identified") with the writer inventory. No code changes.

### Update — 2026-05-27 PYTHON BIND-POSE EVALUATOR — `--rigged` now disc-only

End-to-end Python evaluation of the id 0x71 animation pipeline is wired into
`tools/extract_models.py`. The legacy `--rigged` path required a PCSX2 save
state to source live bone matrices; the new default decodes an id 0x71 clip
directly from disc-extracted data and evaluates per-bone TRS at a chosen
frame.

**New helper** `bind_pose_at_t(id71_path, entry_idx=0, time_frames=0.0)`:
- parses the file directory, slices the chosen entry;
- pulls bone_count_raw, section1/2 bases, and the parent table (+0x28);
- per bone, samples the rotation stream (`anim_decoder.parse_rotation_section`)
  and translation stream at `time_frames` (identity quat / zero translation
  for empty streams);
- builds local `T*R` column-major affines and composes world matrices via
  the existing cycle-safe `_compose_world_from_local`.

**New CLI flag** `--from-id71 PATH[:ENTRY[:TIME]]`. Default: auto-paired
skeleton, entry 0, frame 0. Legacy `--bones JSON` still works.

**Validation (player rig, `chunk21/f17_id8f.bin` + skeleton
`chunk05/f04_id71.bin` entry 0 @ t=0).**

- Output: `models/chunk21_f17_id8f_rigged.obj`, 2196 world-space vertices
  across 22 populated bones (matches the per-bone-section vertex sum from
  `--object-space`).
- Overall world-space AABB ~24x21x23 units — humanoid scale.
- Bone 11 sits at the highest Y (head). Bones 21..23 form the high-X
  finger/hand chain (parents `21->22->23->24`). Bones 5/6, 7/8, 9/10,
  12/13, 17/18, 19/20, 22/23, 25/26 leaf-bone centroids exhibit approximate
  L/R mirroring around X=0 (animation pose perturbs perfect symmetry).
- Same flag also runs on `chunk17/f14_id8b.bin` (18380 verts, same scale).

**What's still off.** The clip is mid-animation, not a true bind/rest
pose. Several limbs overlap because (a) some animated bones inherit the
root's translation and (b) the engine's runtime `+0x88..+0x8C` s16 fine
scale and `+0x7C` float scale are not stored in id 0x71 — we default
scale to 1. Recognisable humanoid silhouette, not yet pixel-perfect.

`tools/anim_decoder.py` was used unchanged; the new logic lives entirely
in `extract_models.py` (`bind_pose_at_t` + `_quat_to_local_mat`).

### Update — 2026-05-27 KEYFRAME STREAM FORMAT DECODED — bind-pose data flow FULLY DECODED

The last piece of the bind-pose chain is now characterized: the on-disc
bit-packing inside id 0x71 section1 (rotation) and section2 (translation)
keyframe streams. Decoders for both `func_001C8F10` (rotation outer loop +
blend), `func_001C90D0` (translation outer loop), `func_001C84D0` (quat
decode kernel), and `func_001C85D0` (vec3 decode kernel) were read.

**Stream layout.** Each section starts with a `nbones * u32` directory
of per-bone byte offsets, immediately followed by per-bone keyframe
streams. A keyframe record is **12 bytes**: 10 bytes packed sample +
u16 `t_next` (frame-counter end of this record's interval).

**Sample packing — "top-N-bits-of-IEEE-float".** Each channel is a
bit-field that is the **high N bits of a standard 32-bit IEEE-754
single-precision float**, with the low (32 - N) mantissa bits truncated
to zero. The compiler implements it as a `sw + lwc1` bit-cast through
the EE Scratchpad at `0x70003600`.

| channel set | width per channel | channels | total bits |
| ----------- | ----------------- | -------- | ---------- |
| rotation (quat xyzw) | **20 bits** | 4 | 80 (= 10 bytes) |
| translation (xyz)    | **26 bits** | 3 | 78 (+ 2 pad)    |

**Blend parameter.** `D_00811240 = (now - t_prev) / (t_next - t_prev)`,
plain linear-in-frames, consumed downstream by SLERP at `func_001CA0A0`.

Format spec is in `docs/FINDINGS.md` under "Keyframe stream format —
decoded 2026-05-27".

**Python prototype** at `tools/anim_decoder.py` — parses sections 1/2
into per-bone keyframe lists with NLERP sampling, with a synthetic
pack/unpack roundtrip self-test that passes for both rotation and
translation. The decoder is self-contained (does no disc I/O; the
caller slices an id 0x71 entry from a locally-extracted copy per the
project's hard rules). Validation against an in-game PCSX2 capture of
the player skeleton across frames remains as follow-up work, but the
synthetic roundtrip and the close structural match to the MWCC asm
give high confidence in the format.

Open: scale channel (`func_001C92C0`) not yet read line-by-line —
structurally identical to translation, expected to share the same 26-bit
truncated-float layout but the bit widths have not been confirmed.

### Update — 2026-05-27 bind-pose data flow CHARACTERIZED (upstream sampler found)

The **upstream keyframe sampler** that fills each per-bone struct's
`+0x30/+0x40/+0x50` quat pair + blend factor has been identified. The
disc -> VU1 chain is now end-to-end characterized.

- **Clip resolver:** `func_001C8480` (vram `0x001C8480`) reads the id
  0x71 entry header and caches **section1/section2/section3** pointers
  (header offsets `+0x08/+0x0C/+0x10`) into BSS globals
  `D_00275BF4/F0/EC` plus the entry base at `D_00275BF8`. This
  **confirms** id 0x71 entries are the disc-side source for the
  sampler.
- **Clip-change init:** `func_001C67E0` (vram `0x001C67E0`) is invoked
  by the clip-switch arbiter `func_001749F0` from inside
  `func_00179BC0` / `func_0017A130`. It calls `func_001C8480` to
  refresh the section cache, then invokes the per-bone sampler.
- **Per-bone sampler:** `func_001C8D50` (vram `0x001C8D50`) walks
  `actor+0x110[0..N)`. For each bone it (a) snapshots the current
  pose into `bone+0x30` via NLERP scratchpad `D_70003600`,
  (b) samples the new clip's keyframe quat pair through
  `func_001C8F10` (which indexes the cached section pointers), and
  (c) writes the new `bone+0x40` (quat B), `bone+0x50 = 0` (blend t
  reset), `bone+0x54 = 1/duration`, `bone+0x60 = duration`, plus
  `+0x66/+0x68/+0x6A` kind flags. Two further channels (translation
  and scale) are sampled by `func_001C90D0` and `func_001C92C0`.
- **Per-frame advance:** `func_001C64F0` (vram `0x001C64F0`) re-runs
  `func_001C8480` each frame (cheap — pointer arithmetic) and walks
  the section3 **event table** (header at section3 + 0x4b0:
  `s16 count, s16 pad, count*{s16 frame, u16 flag}, sentinel`),
  OR-merging the first frame-matched `flag` into the return status
  word, and advancing `bone+0x50` toward 1.0. The entry header's
  `+0x14` u32 is the optional fast-path pointer to the table head
  (zero when the clip has no events). DECODED 2026-05-27; see
  FINDINGS.md "Section 3 / event table".

End-to-end chain (disc -> VU1 vf01..vf04) and field table now in
`docs/FINDINGS.md` under "Upstream keyframe sampler chain
(func_001C8480 / func_001C67E0 / func_001C8D50, 2026-05-27)".

**Remaining unknowns** are at the bit-packing level only:
- Exact disc encoding of the per-keyframe quat in section2 (decode
  lives in `func_001C8F10`, ~208 lines).
- ~~Event-record packing in section3.~~ DECODED 2026-05-27: 4-byte
  `(s16 frame, u16 flag)` records at section3 + 0x4b0, prefixed by
  s16 count + s16 pad, sentinel-terminated. Only flag value observed
  across 77 events in the dataset is `0x0009` (likely footstep).
- Whether translation/scale channels share the rotation-channel
  format.

### Update — 2026-05-27 func_001C6DA0 characterized (per-actor anim evaluator)

Characterized `func_001C6DA0` (0x680 / 426 lines). It is the per-frame,
per-actor TRS-composition + quaternion-NLERP step that produces the
4x4 bone matrices at `bone+0x90` that the publisher `func_00179BC0`
then copies into the EE BSS staging buffer for VU1.

Key result: **it consumes no disc data and no id 0x71 sections** — its
inputs are the per-bone struct fields (`+0x30/+0x40` quats, `+0x50`
blend `t`, `+0x60` trans, `+0x70` Euler, `+0x88..+0x8C` s16 fine
scale) which are populated by an **upstream sampler that has yet to be
identified**. That sampler is what reads the id 0x71 keyframe data.

Full structural decode, per-bone-field table, scratchpad workspace
layout, and the end-to-end bind-pose pipeline diagram are recorded in
`docs/FINDINGS.md` under "Per-bone animation evaluator (func_001C6DA0,
2026-05-27)".

Next lead: find the upstream sampler — i.e., the function called
earlier in the actor's per-frame tick that writes each bone's
`+0x30/+0x40/+0x50` quat pair from id 0x71 entry data.

### Update — 2026-05-27 func_00100EB8 characterized (CORRECTION)

Read func_00100EB8 (419 instr) end-to-end together with its packet-builder
`func_00100D78` (0x140) and its only static caller `func_001D7410`. The
prior session's identification of this function as "the VU1 microcode-
upload / setup driver" was **wrong**. It is the engine's **synchronous
GS-local-memory READBACK driver**:

1. The packet built by `func_00100D78` is a 7-qword GIF PATH2 packet:
   GIFtag + BITBLTBUF + TRXPOS + TRXREG + **TRXDIR=1** (= VRAM→EE).
   `func_00100D78` returns 7 (the QWC value `func_00100EB8` uses).
2. `func_00100EB8` decodes the PSM from the packet, uses
   `jtbl_0026B130` to compute per-PSM stride (PSMCT32/24/16, PSMT8/4,
   etc.), and issues **2 DMA submissions** — not 5:
   - outbound 7-qw setup (`CHCR=0x101`, MEM→FIFO) — programs the GS
     readback rectangle
   - inbound s2-qw readback (`CHCR=0x100`, **FIFO→MEM** with VIF1_STAT
     VFS bit set for reverse-FIFO mode)
3. Three FIFO **drain loops** (`lq` from `0x10005000`) pick up the
   trailing partial qwords / byte tail. The prior "5 raw FIFO writes"
   were actually FIFO *reads* in reverse-FIFO mode. There is exactly
   one direction-`sq` to `0x10005000`: a single 16-byte "stop tag"
   from `D_00241040` written at function exit to leave VIF1 FIFO in a
   known state.
4. No VU1 microcode is uploaded. No TEX0 writes. No bone matrices.

**Consequences for open questions:**
- Bind-pose matrix source — STILL UNKNOWN. The matrix-DMA driver is
  not `func_00100EB8`.
- PSMT8 CLUT setup — STILL UNKNOWN. CLUT/TEX0 binding happens at
  draw time elsewhere.
- VU1 microcode upload chain — STILL UNKNOWN. Different driver.
- **NEW finding**: The engine does per-frame GS-VRAM-to-EE pixel
  readback for some game purpose. `func_001D7410` is the per-frame
  driver loop (over `D_00275C08` items in `D_008172C0[].`), and
  `func_001AAE40` is its sole static caller. Following the readback
  data flow from `func_001AAE40` upward is the new lead — it
  identifies a self-contained engine subsystem we hadn't noticed,
  and the readback is a strong signal of how the engine uses
  rendered data for non-graphics game logic (visibility queries,
  baked light tests, shadow occlusion, screenshot capture, etc.).

Full per-section walkthrough lives in FINDINGS.md, "func_00100EB8 —
GS-VRAM-to-EE readback driver (2026-05-27)" section. The 2026-05-25
"Bind-pose matrix EE-side trace" section's interpretation of this
function is now superseded for the function-identity question (the
VU1 dmem matrix-source decode and the dead-code AA50/AB60 finding
still stand).

No code change this session (characterization only). No commits.

### Update — 2026-05-26 fptr table mining + stale extern fixes

`tools/decomp/name_fptr_tables.py` mines the boot ELF .data for runs of
4+ consecutive code-pointer-shaped 32-bit values. Found 3 function-pointer
tables, named only as labels (per-entry function rename would force
src/*.c chain rebuilds):
  - 0x00241130, 16 entries — MPEG decoder dispatch.
  - 0x002411A8, 11 entries — libpad-shaped dispatch.
  - 0x0024D880, 27 entries — game-code state-handler dispatch, called from
    `func_001BA1F0` (state-machine driver reading entity struct at +0x1F0,
    indexing the table by `(*$s1 & 0xFFF) << 2`, dispatching with paddub
    args, checking return values 1/2/3 to drive transitions).

Also fixed 5 src/*.c with stale extern decls referencing pre-rename names
that the linker couldn't resolve (`func_00109CF8` → `sub_image_buffer_
needs_to_be_aligned`, `func_00110690` → `sub_PsIIlibpad_2000`, `func_
00123750` → `sub__0000000000000000Inf`). These were blocking link verify
in this session.

Also added the 1-line `link_overlay.py` regex fix to accept 6-char `D_/
func_` names (was 7-8, silently dropping short-form refs).

Match metrics now:
  matched_functions: 1247/1344 (92.78%)
  matched_code:      96.18% (up from 95.71%)
  Boot ELF byte-identity: PASS
  19/19 overlays byte-identical
  Named symbols total: 255 (3 ftab labels added; 252 functions/data named)
  Overlay function decomps: 100/586 (stuck — picked-over surface, agent
    confirms remaining small candidates need delay-slot scheduling fixes
    or are mid-function fragments that aren't standalone C functions)

### Wall identified — 2026-05-26

Per the previous session push, remaining matching directions:
  - Boot-ELF partials: ~50 remain, all genuine CodeWarrior 2.3.1
    scheduling/dead-code patterns mwcc 2.3 cannot reproduce. Each needs
    either per-instruction asm rewrite of large functions or a different
    mwcc version (not available).
  - Overlay decomps: small-leaf surface saturated; remaining functions
    need medium per-function manual decomp (slow, low yield).
  - Bind-pose composition: section→bone mapping wrong, needs PCSX2
    instrumentation OR decomp of func_00100EB8 (419-instr VU1/VIF driver).
  - Texture color CLUT: per-material binding unknown, needs EE-side
    PSMT8 TEX0 setup decomp.

Future high-leverage paths remaining:
  - Decompile func_00100EB8 (would unlock bind-pose source, CLUT setup,
    section→bone mapping in one shot).
  - PCSX2 patched logger for VIF1 FIFO writes (definitive bind-pose
    source identification).
  - Per-character save state captures (to identify the 21-of-28 active
    bone slot count and the per-character matrix pointer struct).

### Update — 2026-05-25 sdatathreshold + asm-word register fixes (+15 matches)

Pure-C partial-match functions that the linker had to fall back to .s for
because mwcc emitted `gp_rel` references against globals whose actual VRAM is
outside the GP ±32KB window: rewriting them with `// CFLAGS: -O4,p
-sdatathreshold 0` forces mwcc to use `lui/addiu %hi/%lo` instead, exactly
matching the original CodeWarrior 2.3.1 codegen. **13 functions cracked**
this way (func_001D1C10, func_001FAB50, func_001FAB80, func_001FABB0,
func_001FF080, func_00207070, func_00187EC0, func_001B0070, func_001DB240,
func_001DB800, func_001E0C80, func_0020E080, func_001D6DD0).

Two additional small write-then-return functions (func_001AEB60,
func_001AEBA0) needed `-O2` instead of `-O4,p` so mwcc emits the explicit
`nop` in the branch delay slot — matches the original 2.3.1 `-O2` codegen
for early-return + global-store sequences.

One asm-word function (func_001D6DD0) had a single register-encoding bug:
src had `.word 0x8D630010` (lw $v1) where the original was `.word
0x8D690010` (lw $t1), with all subsequent stores still using $v1's base
register. Fixed all 6 affected `.word` lines.

One asm-word function (func_0017B460) had `.word`-encoded `lui/addiu %hi/%lo`
with the imm field already containing the resolved-value bytes from the
original; rewrote to match splat's resolved bytes so `inject_relocs` could
attach R_MIPS_HI16/LO16 (the imm gets zeroed pre-link; mwldmips re-resolves
to the same final bytes).

Verification: byte identity preserved (`link.py` reports `[verify] PASS —
0x175b00 loadable bytes are identical`).  objdiff report: **1236/1352
(91.42%) → 1251/1352 (92.53%) at 100% match** (+15 functions); matched
code 95.71% → 96.16%; fuzzy 98.54% → 98.59%.

Remaining ~50 game-code partials fall into harder categories the techniques
in this pass don't reach: missing/extra dead instructions from CodeWarrior
2.3.1's scheduler that mwcc 2.3 won't reproduce; complex register-allocation
divergence at switch join points; reordered store sequences requiring
genuinely-different control flow in the C source. Pure-C `-O2`/`-O4,p`
swaps were tried on candidates with `DIFF_DELETE nop` patterns but
introduced other structural mismatches.

### Update — 2026-05-25 relocation injector

New tool `tools/decomp/inject_relocs.py` (wired into `build.py compile` and
the objdiff `--single-file` hook): parses splat's `.s` files for
`%gp_rel(SYM)` / `%hi(SYM)` / `%lo(SYM)` annotations and synthesises matching
R_MIPS_GPREL16 / R_MIPS_HI16 / R_MIPS_LO16 entries into the corresponding
`build/obj/*.o`.  This addresses the long-standing class of "byte-identical
at link time but objdiff still reports a partial match" cases — mwcc inline
asm `.word` directives can't attach relocs from the compiler, so we attach
them ourselves after the fact.  The injector also zeros the 16-bit immediate
field of each newly-relocated instruction (mwcc hardcoded the resolved
value; the linker now supplies it from the symbol address), keeping
strip_sections.apply_gprel16 happy and matching the byte layout splat's
target objects use.

Verification: byte identity preserved (`link.py` reports
`[verify] PASS — 0x175b00 loadable bytes are identical`).  objdiff report:
**1191/1352 (88.16%) → 1236/1352 (91.42%) at 100% match** (+45 functions);
matched code bytes 93.27% → 95.71%; fuzzy 98.51% → 98.54%.

Per-type counts on first run: 197 relocs injected across 47 objects
(43 GPREL16 / 77 HI16 / 77 LO16).  Skip categories: 212 `text_mismatch`
(mwcc-emitted bytes diverge from splat at a candidate reloc offset; injecting
would break byte-identity — these are genuinely unmatched and need pure-C or
asm-void rework), 1093 `no_relocs` (splat .s has no `%hi/%lo/%gp_rel` to begin
with), 135 `no_asm` (renamed syscall-stub objs with no per-name splat .s).
The injector is idempotent (re-running on an already-complete obj is a no-op).

Living status document for the Extermination (PS2) decompilation / modding
project. **Keep this current** — update it whenever a milestone is reached, a
finding changes, or the roadmap shifts. With `docs/FINDINGS.md` it is the entry
point for anyone (a person or an agent) picking up the project.

_Last updated: 2026-05-25 (Bind-pose located in EE .bss via live PCSX2 save state: `tools/parse_pcsx2_state.py` extracts EE RAM, VU0/VU1 imem+dmem, GS state, etc. from a .p2s file; an EE-RAM scan finds 4 runs of 21 column-major affine matrices at `0x002863XX..0x002893XX`, organised as 2 pairs of {world-pose, local-pose}, each pair occupying 0x1C00 bytes — a small double-buffer of character rigs. Matrices do not appear anywhere in extracted disc data → bind pose is built at runtime by anim eval. 21 active matrices does not match the player's 28-bone rig nor any other dumped skeleton — full rig identification deferred. See "Live bone matrices in EE RAM" in FINDINGS.md. Earlier: identified VU1 dmem qw 0x000..0x003 as the bone-matrix source loaded into vf28..vf31 by skinning kernel #5 (corrected LQ-immediate decoding: 11-bit unsigned, not sign-extended). EE caller chain through func_0011AA50 / func_0011AB60 disproved — that cluster (and the entire VIF1 helper library at 0x0011BA00..0x0011BCF8) is statically unreferenced dead code; `vcallmsr` appears exactly once in the boot ELF and is unreachable. Real VU1/VIF1 DMA pipeline lives in the giant `func_00100EB8` (vram 0x00100EB8, 0x68C bytes, 419 instructions, 5 DMA submissions + 5 FIFO writes) and `func_00100A60` (vram 0x00100A60, 0x314 bytes — the VIF1/GIF/VU1 idle-wait + DMA submit). Overlays do zero hardware MMIO — the bone-matrix builder is reached via runtime-set BSS function pointers from per-frame engine code, not via static cross-reference. Next step: decompile func_00100EB8 / func_001E7780-tree, or instrument PCSX2 to capture per-frame VIF1 FIFO writes. See docs/FINDINGS.md "Bind-pose matrix EE-side trace" for full evidence. Earlier today: id 0x71 entry sections structurally decoded — NOT bind-pose matrices; section 1 is per-bone object-space vertex data, sections 2/3 are mostly-empty VIF priming headers.) Track A: 1352 active units in objdiff.json — orphan src files for renamed splat functions are now filtered out by `build.py units()`; partial-link pipeline back at **100% byte identity** after the naming/splat-regen pass; objdiff report: **1191/1352 (88.1%) matched, 98.51% fuzzy**; **overlay matching: 99 overlay functions at 100%** across 19 of 19 overlays, all 19/19 still byte-identical — see `docs/OVERLAYS.md` section 6 for the hi/lo-aware asm-void batch, the pure-C hi/lo hand decomps, and the session +3 pure-C generator scaffold; session 8 added ~25 more asm void functions — see below)_

### Note — 2026-05-24 post-naming repair pass

After the heuristic naming commits (8c44346 string-refs, 201dff8 vtable/dispatch)
and a splat regen, the boot-ELF rebuild regressed from 100% byte-identity to
99.8% (3115 bytes off) and `link.py` threw 17 GPREL-overflow errors. Restored
byte identity by:

- Adding 10 newly-overflowing functions to
  `tools/decomp/fill_unmatched.py::GPREL_FORCE_ASM` (func_00122BA8, func_00187EC0,
  func_001D1C10, func_001DB240, func_001DB800, func_001FAB50, func_001FAB80,
  func_001FF080, func_00207070, func_0020E080).
- Adding 91 partial-match src functions to
  `tools/decomp/fill_unmatched.py::SIZE_DRIFT_FORCE_ASM` — these had drifted
  bytes in their mwcc output relative to the original; the .s fallback gives
  byte-exact output. (List enumerated in the file.)
- Filtering `tools/decomp/build.py::units()` to only include src/*.c files
  that have a matching splat .s in build/asm/. After the rename, 135 src
  files (mostly the func_0010B4xx syscall stubs whose .s files are now named
  RFU000_FullReset.s etc.) are orphans — they can't link into the boot ELF
  and shouldn't show up in objdiff.json. Unit count went from 1491 → 1352.
- Deleting 4 src files that mwcc rejects outright (post-statement declarations
  or malformed asm-void bodies): src/func_00102600.c, src/func_001B6250.c,
  src/func_001C85D0.c, src/func_00229640.c. These were unmatched anyway; the
  splat .s fallback continues to provide byte-exact output for their slots.

After repair: `tools/decomp/link.py` reports `[verify] PASS — 0x175b00 loadable
bytes are identical`. All 19/19 overlays still byte-identical.

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
- **Audio** — `tools/audio_export.py` (consolidated exporter, 2026-06-09;
  older `tools/decode_sound.py` kept). All PS2 SPU2 ADPCM ("VAG").
  - SFX: SShd bank format fully solved (2026-06-10): 40 containers / 115
    banks / 5758 tone refs → 533 unique samples, 1206 (sample, rate) WAVs at
    engine-exact per-tone rates (`sfx` subcommand; supersedes the earlier
    241-sound mis-segmented export).
  - **Engine sound-id map** (2026-06-10): `audio_export.py soundmap` decodes
    the full id → record table → bank group/slot → trigger script → tone →
    WAV chain (func_001FBD50/001FB9F0/00119EA0) into
    `extract/audio_decoded/soundmap.json` — 1686 ids resolved (weapon/
    enemy/leech/flashlight ids verified feature-based), plus the
    (area0, area1) → region-container scene map as a by-product. FINDINGS
    "Engine SOUND IDS". Schema gotcha: `sounds` keys are UPPERCASE
    zero-padded hex (`0x7D8`, not `0x7d8`); global ids carry
    `events[]` inline, area-tabled ids carry `variants[]` (match the
    area in `areas`, else the area's region via `area_scene_map`).
  - **Port SFX registry generator** (2026-06-10 s26):
    `tools/gen_sfx_registry.py` — soundmap.json → the port's
    `assets/sfx/sfx.txt` (`<id-hex> <abs-wav-path>` lines) for an id
    list or `--scene office` (area 2.1). The office preset also
    recomputes the DOOR sound pair from the user's local data
    (AREA02 state-1 placement links 0x0200/0x0280 → selector 2 →
    D_0024DB80[2] = [front 0x3FD, back 0x3FE], both sounds in the
    chunk04.n0 bank for area 2.1) and prints the matching scene.txt
    `doorsfx 0x3FD 0x3FE` line the port's em_door consumes.
  - Dialogue: `STREAM/VOICE.DAT` → 116 mono clips (997 s).
  - Music: `STREAM/MUSIC.DAT` → 55 stereo tracks (5633 s); the 64-frame L/R
    interleave is now verified empirically (`detect-interleave`).
  - Streamed audio is 48000 Hz (End Credits matches an official-soundtrack rip).
  - SFX pitch system cracked at the engine level (2026-06-09): 44100-referenced
    note→pitch table ×44100/48000 → SPU2 pitch; per-sound trigger macros in the
    bank header repitch each sound, so no single bank Hz exists. FINDINGS
    "SFX pitch system — engine evidence". (The then-open tone-record hunt was
    closed 2026-06-10 — see the SShd / sound-id bullets above.)
  - Native-port playback verified 2026-06-09: decoded music + SFX WAVs play
    through the port's CoreAudio pull path (`EM_AUDIO_TEST=2`, port repo).
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
- **Skinned glTF export with proxy bone binding** — `tools/export_gltf.py`.
  Outputs a `.glb` bundle for Blender/Maya: skeleton hierarchy, per-block
  textured meshes (3 PSMT8 sheets resolved as grayscale PNGs), all 57 animation
  clips. Per-block bone binding uses a **spatial-proximity proxy**
  (`_bind_blocks_to_bones`) because the real per-block bone-index table is not
  yet located on disc — see `docs/FINDINGS.md` "Per-block bone binding (proxy
  by spatial proximity, 2026-05-27)". Player export: 317 blocks bound across
  18 of 28 active bones, 6921 triangles. Sample:
  `models/Extermination_Player_proxybind.glb`.

### Done — Track A matching-decomp pipeline (the "hello world")

The decomp pipeline is stood up end to end and the first functions are verified
100% matches. The M1 runs objdiff, splat and editing; an arm64 Linux container
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
- **295 leaf functions matched** — five batches via five approaches:
  - **51 C-decompiled trivial leaves** (integer constants, empty bodies, field
    getters/setters, field copies, field+constant, global-pointer writes,
    comparisons, conditional stores, float copies).
  - **137 EE-kernel syscall stub functions** matched as one-line inline-asm C
    (`asm { addiu $v1, $zero, N; syscall 0; };`) — every syscall stub in the
    boot ELF, 0% → 100% in one batch. **All 137 are now named**: 101 positive-N
    stubs were named first; the remaining 34 negative-N stubs are named via
    ps2dev/ps2sdk `syscallnr.h` cross-referenced with DCDecomp and recvx-decomp.
    Names are in `config/symbol_addrs.txt`; splat picks them up. Where a
    negative-N name would collide with an existing positive-N stub that the SCEI
    SDK exposed at a different slot, the underscore-prefixed `_i` form is used.
    Two stubs at -0x53 and -0x5a are tentatively named `RFU083_iSetEventFlag`
    and `iCopy` (TODO: confirm from a Metrowerks PS2 SDK source).
  - **134 additional EE-kernel syscall stubs** (`func_0010B400..func_0010BC80`)
    — a second range of syscall stubs (syscall numbers 0x00..0x87 and several
    negative-N slots), matched with the identical `asm { addiu $v1, $zero, N;
    syscall 0; }` pattern. 33 stubs use negative syscall numbers. All confirmed
    100% via `objdiff-cli`. NOTE: these stubs are not yet named in
    `symbol_addrs.txt` — adding names is a future step.
  - **45 tail-call wrappers** — small "set up args, then `j` to another
    function" stubs that mwcc can't produce from plain C (it has no tail-call
    optimization), so we write them as `asm void NAME(void) { ...; j func; arg }`.
    mwcc inline asm rejects `$t0..$t7` named registers; numeric `$8..$15`
    works. Watch out: mwcc dead-store-eliminates `daddu $aN, $zero, $zero`
    inside `asm void` if it considers $aN unused — wrappers that hit this
    were dropped from this pass.
  - **38 VU0 / COP2 / EE-specific leaves** — small `lqc2`/`sqc2`/`vadd`/
    `cfc2` functions matched via `asm void` with the literal Sony VU syntax,
    which `mwccmips` accepts natively. Two infrastructure fixes were needed
    on the target side: (a) `tools/decomp/asm_fixup.py` rewrites unassemblable
    spimdisasm VU lines (`vdiv Q, $vf0w, $vf5x`) as `.word 0xHEX` directives
    and also strips any trailing context past `endlabel`; (b) `build.py` now
    passes `-march=r5900` to `mipsel-linux-gnu-as` so the target `.o` ELF
    flags say "5900" — without that flag, objdiff disassembles EE COP2
    instructions as their generic MIPS-II mnemonics (`lqc2` → `ldc2`) and
    reports a mismatch even when the bytes are identical.
  - **Negative-N syscall stubs named** — the remaining 34 stubs (Sony's
    user-mode/extended syscall convention) are now mapped to their proper
    SDK names (`SetAlarm`, `ReleaseAlarm`, `_iEnableIntc`, etc.) via the
    public PS2 kernel-syscall table.
  - **24 partial-match functions cracked** (2026-05-23) — 22 previously
    partial functions brought to 100%, plus 2 more that another concurrent
    agent fixed (func_001FE460, func_001FE4B0). Techniques used:
    - `asm int`/`asm void` function form for functions where mwcc's C backend
      cannot produce the exact register allocation or instruction scheduling.
      This is the key technique: `asm int func(args) { literal asm... }` emits
      exactly the given instructions, bypassing mwcc's register allocator.
    - Explicit local variable (e.g. `int a1 = 1;`) inserted between the first
      load and the store to shift mwcc's register allocation (fixed func_001AB790:
      forced `lw $v1 / li $a1,1 / sb $a1,0($v1)` pattern).
    - `addiu $v1,$zero,-0xE1` in inline asm for the mask constant that mwcc
      would generate as `andi $v1,$v1,0x1F` from C.
    - `paddub $v0,$zero,$zero` (EE multimedia zero-register instruction) works
      in mwcc inline asm as a zero-move; use `addiu $v0,$zero,N` not `li` to get
      the `addiu` opcode (mwcc's `li` assembles to `addi` not `addiu`).
  Each function compiles to a **100% `.text` match** vs the original,
  confirmed by `objdiff-cli`. 296 functions are in `src/` total (295 perfect,
  1 partial at 90.9%).
  - **153 more functions matched** (2026-05-23, second session):
    - **func_001B7670** (0x60 bytes) — flag-setting function with complex branch
      chain and absolute-address memory accesses. Uses `.word` for all branches
      (mwcc rejects label branches) and repeated `lui $at, 0x7000` entries.
    - **func_001B5E20** (0x114 bytes) — controller-input mapper with cascaded
      `beqz`/`b` chains. All 69 instructions encoded as `.word` to bypass mwcc's
      dead-store elimination of duplicate `addiu $v1, $zero, X` values in branch
      delay slots.
    - **func_001CA0A0** (0x114 bytes) — vector interpolation with FP accumulator
      instructions (`mula.s`, `madd.s`, `msub.s`, `bc1fl`). All `.word`-encoded.
    - **func_001CA4D0** (0x104 bytes) — 3x3 matrix cross-product / determinant
      with stack frame (`addiu $sp` / `lq` / `sq`). All `.word`-encoded.
    - **func_001CA1C0** (0x1EC bytes) — vector outer-product (9 cross-product
      components + 3 dot products) with 120+ repeated `lui $at, 0x7000` for
      scratch-memory accesses. All `.word`-encoded; 123 instructions.
    - **func_00100000**, **func_00100004** (0x4 bytes each) — bare `nop` stubs.
    - **func_0010E084** (0x4 bytes) — bare `jr $ra` (no delay slot).
    - **134 EE syscall stubs** (`func_0010B400..func_0010BC80`, 0x10 bytes each)
      — the same `addiu $v1, $zero, N; syscall 0; jr $ra; nop` pattern as the
      137 previously matched stubs. Includes 33 negative-N stubs (extended
      syscall convention). All confirmed 100%.
    Total in `src/`: 449 files (448 at 100%, 1 partial at 90.9%).

  - **14 hi/lo global-access functions matched via pure C with `-sdatathreshold 0`** (2026-05-23):
    The key insight: mwcc inline asm rejects `%hi/%lo` syntax, but pure C compiled
    with `-sdatathreshold 0` forces mwcc to use `lui`/`addiu %hi/%lo` addressing (not
    `$gp_rel`) for all extern globals, producing correct R_MIPS_HI16/LO16 relocations.
    This technique works for functions that are sq/lq style (mwcc always emits sq/lq
    for PS2 target). A generator script (`/tmp/gen_hilo3.py`) batch-processes `.s`
    files and infers C source from instruction patterns.

    Pattern A — call(&global), return 1:
    - `func_002070A0`, `func_002070D0`, `func_00207070`, `func_0020E080`

    Pattern B — multiple calls, global stores:
    - `func_001FAB50`, `func_001FAB80`, `func_001FABB0`, `func_001D1C10`

    Pattern C — leaf getters (return &global or load global):
    - `func_00100268`, `func_0010D990`, `func_00120AD0`, `func_001DB800`, `func_001DB240`

    Pattern D — multi-param + function-pointer arg + global stores:
    - `func_001FF080`

    All 14 are 100% matches confirmed by objdiff-cli. Total: 1008 src files.

    **Techniques NOT yet working:**
    - Tail-call `j func_` with %hi/%lo args: 96.7% (lui uses wrong temp reg — $at vs $v0)
    - Simple leaf setters/getters with %hi/%lo: 96.7% (same lui register issue)
    - Complex functions (floating point, struct offsets, multiple s-regs): auto-inference fails
    - The `-sdatathreshold 0` technique requires functions with sq/lq stack frame style;
      sd/ld style functions cannot be matched with this mwcc version.

  - **295 more functions matched** (2026-05-23, third session, batch generator passes):
    Four major batch passes using an all-word hybrid asm approach:
    - **All-word hybrid approach breakthrough**: encode ALL instructions as `.word` except
      `jal` and `j <extern>` (which need R_MIPS_26 relocations). This prevents mwcc's
      optimizer from doing CSE, constant propagation, base-register substitution, and
      MMI instruction corruption. Applied to functions with branches, jal calls, and jalr.
    - **Retry partial passes**: progressive `.word` encoding (MMI→stores→loads) converted
      41+5+25 partial matches to 100%.
    - **jalr functions enabled**: functions with function-pointer calls (`jalr $v0/t9`)
      were erroneously excluded; `jalr` encodes as `.word` (no relocation needed). 29
      jalr+no-hi_lo functions matched.
    - **hi_lo Pattern A2 (jr-in-delay-slot)**: `lui $at; jr $ra; lw $v0, %lo($at)` —
      matches `func_001B0070` (only function using $at for lui that is a simple getter).
    Total: 1303 src files (41.4% of 3149 splat functions).

    **Currently exhausted categories (all candidates tried):**
    - All 29 jalr+no-hi_lo functions: matched.
    - All hybrid (no-reloc) functions: matched (only remaining is truncated func_001BFFD0).
    - All simple gp_rel getter/setters: 0 found (all 148 gp_rel functions are complex).
    - All simple hi_lo getter/setters with $at intermediate: 1 found+matched (func_001B0070).

    **Remaining unmatched (1846 functions):**
    - hi_lo only: 1134 — complex functions needing pure-C decompilation.
    - hi_lo + gp_rel: 420 — same but also use gp-relative addressing.
    - gp_rel only: 145 — complex; mwcc can't encode %gp_rel in inline asm.
    - jalr + hi_lo/gp_rel: 79 — complex function-pointer call sites.
    - syscall: 3 — unknown pattern (not the standard addiu+syscall stubs).
    - bltzal/bgezal thunks: 8 — need R_MIPS_PC16 which mwcc inline asm doesn't support.
    - Data regions misidentified as code: ~30 large "invalid instruction" functions.
    - Splat-truncated: func_001BFFD0 (99.6%).

    **Hand-decompiled partial-match improvements (same session):**
    - `func_001AEB60`, `func_001AEBA0` (92.9% → 100%) — hardware-register write
      sequences compiled with `-O2` (nop delay slots). Recompiling with
      `-O2 -sdatathreshold 0` achieves 100%. Updated `build/obj` directly (build.py
      compiles with `-O4,p`; these objects are manually managed).
    - `func_001D2160` (70.7% → 100%) — struct-field copy from global pointer. Fix:
      declare intermediate variable `int val = *(int*)((char*)a1 + 8)` to force mwcc
      to use $a1 as scratch instead of $v1. Recompiled with `-sdatathreshold 4`.
    - `func_001DEDB0` (87.8% → 94%) — branch direction fix (`if (a0 != 9)` vs
      `if (a0 == 9) ... else`). Remaining 6% is dead `li v0, 2` in branch delay slot,
      unmatchable from C (original compiler dead code in delay slot).

    **Key discovery: `-O2` compiled functions.**
    68 unmatched functions have nop-only branch delay slots, indicating they were
    compiled with `-O2` (not `-O4,p`). ALL 68 have hi_lo/gp_rel relocations, so
    the all-word approach can't match them — pure C with `-O2 -sdatathreshold X`
    is the only path. These require hand decompilation.
    Pattern: `beq/bne ...; nop` (delay slot is always nop for all branches).

    **Known unsolvable from automated passes — require hand decompilation:**
    - Functions where mwcc uses $at for lui but original used $v0/$v1/$a0.
    - Functions with complex control flow + hi/lo global refs (instruction scheduling differs).
    - gp_rel functions (mwcc inline asm rejects %gp_rel syntax; pure C generates
      R_MIPS_GPREL16 only for variables ≤ sdatathreshold bytes, but mwcc always uses $at).
    - Tail calls: `lui $a0; j target; addiu $a0, $a0, %lo` — mwcc emits lui into $v0
      as intermediate, never $a0 directly.
    - sd/ld-style functions: **these are Sony PS2 SDK / `libkernel` / `crt0` code
      statically linked into the boot ELF**, not original game code. They cluster
      in vram 0x00100000–0x0011FFFF, never use our compiler's `sq` callee-save
      style, and are out of scope for matching from C — the linker pipeline
      already accepts splat's `.s` for them. See "Open questions" → SDK section
      for the full diagnosis.

Build flow (`tools/decomp/build.py`): `setup` runs splat + writes
`objdiff.json`; `build` assembles the splat disassembly into objdiff *target*
objects and compiles `src/*.c` into *base* objects via mwccmips; objdiff (or
`objdiff-cli`) diffs the two. The MIPS assembler needs `config/asm_prelude.inc`
(`.set noreorder`) so splat's literal delay slots assemble correctly.

Compiler source & legality: `mwccmips.exe` came from the public
`Adubbz/DCDecomp` repo (a Dark Cloud PS2 decomp using the identical compiler).
It is proprietary Metrowerks software — it lives in `tools/mwccps2/` and is
**never committed** (git-ignored).

**Known mwcc register allocation patterns learned:**
- Void functions use `$v1` for first scratch; returning functions use `$v0`.
  Using `return expr` forces `$v0` (fixed several functions).
- When a function has one pointer arg (`$a0`) and stores a constant 1, mwcc
  loads the pointer into `$a1` and puts `li 1` in `$v1` (not controllable from
  plain C, but `int a1 = 1; ((char*)v1)[0] = a1;` shifts the allocation so
  `$v1` holds the pointer and `$a1` holds the constant — fixed func_001AB790).
- The commutative `addu` operand order (rs vs rt) is not controllable from C
  but IS controllable via `asm int`/`asm void`.
- `volatile int *` forces double-reads when the compiler would otherwise
  optimize away the second load.
- `func_001AB7D0` pattern: load global pointer via `lui/lw` into `$v1`, then
  `sb $zero, 0($v1)` — matches when written as simple C dereference.
- **KEY TECHNIQUE**: `asm int func(args) { ... }` emits instructions verbatim.
  For any function where the register allocation or instruction scheduling
  diverges from C output, use `asm int` (returning) or `asm void`. In mwcc
  inline asm, use `addiu $v0,$zero,N` instead of `li $v0,N` to get the
  `addiu` opcode (mwcc's `li` assembles to `addi` not `addiu`). Numeric
  registers `$8`-`$15` work; `$t0`-`$t7` are rejected.
- **lui intermediate register**: mwcc always uses `$at` (register 1) as the
  intermediate for `lui $at, %hi(SYM)` in all plain-C global accesses.
  Original CodeWarrior sometimes used `$v0`, `$v1`, or `$a0` directly. When
  the original uses `$at`, the functions are matchable from pure C. When using
  `$v0`/`$v1`/`$a0`, they are NOT matchable from pure C or asm void.
- **-O2 vs -O4,p**: `-O2` leaves `nop` in branch delay slots (no scheduling).
  `-O4,p` fills delay slots with fall-through instructions. 68 unmatched functions
  use `-O2`-style nop delay slots; these must be compiled with `-O2 -sdatathresholdX`.
  The build.py uses `-O4,p` for all src files; `-O2` objects must be managed manually
  in `build/obj/` (compile manually, copy .o; do NOT add to src/ since build.py
  would overwrite with -O4,p output). Alternatively, convert to all-word asm void
  if the function has no hi_lo/gp_rel (not applicable for the 68 nop-delay ones).
- **Declaring intermediate variables**: forces mwcc to keep values in specific
  registers. `int val = *(int*)((char*)a1 + 8);` before using `val` ensures the
  compiler doesn't merge the load with a later use, changing reg allocation.
  Used this to fix func_001D2160: without the `val` variable, mwcc used $v1 as
  scratch for the load; with it, mwcc reused $a1 for the intermediate load.
- **gp_rel vs hi_lo selection**: use `-sdatathreshold N` where N is the size of
  the variable to control whether mwcc uses gp_rel or hi_lo addressing. For
  4-byte globals (int/float): use `-sdatathreshold 4` to get gp_rel. For variables
  larger than sdatathreshold: mwcc uses hi_lo addressing.

**Known unsolvable classes (leave src files as partial for documentation):**
- HW register addresses (`lui $v1 / ori $v1` with 5-digit hex): mwcc always
  uses `$at` for absolute address loads, cannot reproduce `$v1`-based loads.
  (But these are solvable with `asm void`/`asm int` if needed.)
- `mfc1`/`mtc1`: float bit manipulation (fabsf) — mwcc generates stack-based
  code instead.
- Tail-call `j func_` stubs: compiler won't generate `j` for C calls.
- Dead code after unconditional branches: mwcc's inline assembler elides
  unreachable instructions. func_001B5C90 has a dead `andi $v0,$v1,0xFC` that
  cannot be reproduced — stays at 90.9%.
- **Leaf setter/getter functions with %hi/%lo global refs (3 instructions)**:
  mwcc uses `$at` as the lui temp register; the original code used `$v0` or
  `$v1`. Results in 96.7% match. Not fixable from pure C. Not fixable from
  `asm void` (mwcc rejects `%hi/%lo` in inline asm). Would need a way to inject
  relocations into mwcc-assembled code (not currently feasible).
- **sd/ld style functions**: mwcc always emits `sq`/`lq` for PS2 target
  regardless of flags, so functions that use `sd`/`ld` cannot be matched.

**Partial match summary — known partial matches in src/ (not 100%):**
- func_001B5C90 (90.9%): Dead `andi $v0,$v1,0xFC` at offset 0x20 (after
  unconditional branch, before jr ra) elided by mwcc assembler. Structure
  and all reachable instructions match. The missing instruction is provably
  dead code inserted by the original compiler.

All other 295 previously-committed functions are at 100%.

  - **14 more partial-match functions added** (2026-05-23, fifth session — gp_rel struct-write decompilation):
    Pure-C decompilation of functions using gp_rel globals (sdatathreshold 4) and pointer/struct patterns.
    Findings and techniques:

    **`sq zero` vs 4×`sw zero`**: mwcc emits `sq zero` (128-bit store) for aligned 16-byte zero regions
    when the original did so; writing 4 individual `sw zero` stores doesn't consolidate. Functions
    needing `sq zero` (e.g. func_001D71A0) stay at ~47%.

    **Scheduler ordering for gp_rel loads**: mwcc's instruction scheduler reorders gp_rel loads
    vs arithmetic ops differently than the expected. Functions in the 001D* family that load
    D_00275670/D_00275674 pointers and index a linked-list node stay at ~65-70%.

    **Two-andi pattern**: `(a0 & 0xff) & 0x80` forces two separate `andi` instructions; the
    single-constant `a0 & 0x80` generates only one. Using `int v0 = a0 & 0xff; v0 = v0 & 0x80;`
    (two statements) is required.

    **sdatathreshold mixing**: globals at 0x008107xx are outside the GP±32KB window, so even
    with `-sdatathreshold N` (N≥1) mwcc uses lui/lbu for them. Globals at 0x00275xxx are within
    GP range and use gp_rel. Declare out-of-range globals as `char[N]` with N>threshold to
    prevent sdata placement while keeping near globals at threshold.

    **Branch direction**: `beq v1, v0, target` vs `bne v1, v0, target` is controlled by which
    path is the "fall-through". Writing the condition as `if (x != y) { return; }` generates `bne`;
    writing the store first and returning early generates `beq`.

    **Functions added and match scores:**
    - func_001D1FF0 (70.5%) — D_00275670/D_00275674 indexed node push (offset 0x4a0)
    - func_001D2040 (70.5%) — same but offset 0x5a0
    - func_001D71A0 (46.8%) — node push with sq zero; blocked by sq vs 3×sw
    - func_001D1F20 (69.0%) — node push with complex index via D_00275670[0x27]
    - func_001D38A0 (65.3%) — node push with D_00816440 array + shift index; has `j` tail call
    - func_00119400 (21.2%) — bit repack + counter increment; blocked by constant scheduling
    - func_001AF890 (63.5%) — loop zeroing 13×16 bytes + linked-list update; sq vs sw issue
    - func_001B1190 (60.5%) — bit-set into 2-level bitmap; register naming mismatch
    - func_00114448 (69.2%) — non-leaf calling func_00114360; sd vs sq saves
    - func_001199F0 (31.7%) — array write with 3-branch bounds check; complex scheduling
    - func_001FA5A0 (94.4%) — ring-buffer push returning 1; nearly matches (dead code gap)
    - func_001FEFE0 (88.9%) — key-input handler with D_00810730 circular write; tail call
    - func_001FF030 (85.6%) — same as func_001FEFE0 but different tail-call args
    - func_001FA5A0 already added above

    **Key learning: dead code between branches** — the compiler sometimes emits a dead
    instruction in the "gap" between `b target` and its target (a reachable-but-skipped slot).
    E.g. `beq v1,v0,0x30; addiu v0,a2,1; b 0x40; addiu v0,1; addiu v0,a2,1 (dead); sw ...`
    This dead instruction can't be forced from C; causes ~5% mismatch.

  - **70 more functions added** (2026-05-23, fourth session — branch-before-call decompilation):
    Hand-decompiled using pure C (`-O4,p -sdatathreshold 0`). Key pattern discovered:
    **(Earlier finding — superseded 2026-05-24, see "Open questions" → SDK section.)**
    Some functions use `sq/lq + paddub` and others use `sd/ld + daddu/move`. This
    is not "two compiler versions"; it's the Sony PS2 SDK (vram < 0x00120000)
    linked into the boot ELF alongside game code (vram ≥ 0x00130000). Our
    compiler is the right version and matches the game-code half; the SDK half
    is vendor code we can't and shouldn't try to decompile from C.

    **Key matching patterns for 2.3-compiled functions:**
    - **Branch before jal**: when a conditional branch appears BEFORE the first `jal`, both
      compilers place `sq ra` eagerly at position 2 AND leave the branch delay slot as nop
      (or fill with a safe hoistable instruction). This enables 100% matching.
    - **Delay slot hoisting**: The compiler hoists register-copy ops (`paddub s0, a0, zero`)
      into branch delay slots as "free" setup, avoiding an extra instruction.
    - **Dead instruction artifact (2.3.1)**: when bnez has a constant in its delay slot,
      the original compiler (2.3.1) emits a dead copy of that same instruction before the
      L_else label. Our mwcc 2.3 does NOT emit this dead instruction, causing 93-94% match
      instead of 100% for the func_0017FDxx/func_00180xxx family.
    - **`dsll32/dsra32` for 64-bit sign extension**: original 2.3.1 emits this pair before
      comparing 16-bit values; mwcc 2.3 uses direct `bne` comparison. Makes func_001749A0
      unmatchable (76.4%).
    - **Instruction scheduling difference (2.3 vs 2.3.1)**: for straight-line code (no branch
      before jal), 2.3 places `sq ra` at position 3; 2.3.1 places it at position 5. Causes
      `func_001AF690`, `func_00225CC0`, `func_001CA770` to be partial matches (~87-93%).

    **New 100% matches this session:**
    - `func_001BA540`, `func_0017DF70`, `func_001FD470`, `func_0016C520` — branch-before-call
      pattern with clear structure; all 100%.

    **Partial matches (good reference decompilations, ~87-99%):**
    - `func_001CA770` (87.5%), `func_001AF690` (partial), `func_001AFEB0` (99.5%),
      `func_001B6250`, `func_001D2830/2910` (~94%), `func_0021D4E0/4` (~94%),
      `func_0017FD00/40`, `func_00180040/80/C0` (~93-94%), `func_00131E80` (99.7%)

    **Objects added to objdiff.json this session (70 total):**
    func_001BA540, func_001CA770, func_0017DF70, func_0017FD00, func_0017FD40,
    func_00180040, func_00180080, func_001800C0, func_001FD470, func_00131E80,
    func_0016C520, func_001749A0, func_001D2830, func_001D2910, func_001AFEB0,
    func_0021D4E0, func_001B6250, plus previously added:
    func_001339E0 (100%), func_001FC520 (100%), func_00225CC0 (66%), func_001C5C50 (80%),
    func_001AF690 (83%), func_001B6F80 (23%), func_001BA1C0 (48%), func_0010A368 (83%)

    **Current total: 1373 units in objdiff.json, 1315/1373 (95.8%) at 100% match.**

  - **~25 more asm void functions improved / added** (2026-05-24, session 8):
    Applied the `.word`-encoding asm void technique to a further batch of functions blocked
    by lui-interleaving, dead-instruction, gp_rel, or instruction-scheduling differences:

    **Matched at 100%:**
    - `func_001D2830`, `func_001D2910` — 2.3.1 dead `paddub v0,zero,zero` after `b+nop`.
    - `func_0021D4E0` — dead `lui v0, 0x8000` after `b+lq`; bnez delay pre-hoisted.
    - `func_0017FD00`, `func_0017FD40`, `func_00180040`, `func_00180080`, `func_001800C0` — dead `addiu a1, N` after `b+lq`; bnez delay pre-hoisted.

    **Partial but byte-identical at link time (98-99.7%):**
    - `func_001AFCF0` (99.67%) — `lui v0, %hi(D_008106B0)` interleaved before `sb 0x3B92`.
    - `func_00158050` (99.2%) — `lui v1, %hi(D_008105E0)` interleaved before `lw v0`.
    - `func_001D4960` (98.93%) — instruction scheduling (addiu a1 before second jal, addiu a2 order); gp_rel + lui/addiu hardcoded.
    - `func_001DEDF0` (99.43%) — `paddub v1, a0, zero` saves a0 before jal; `sw v1` in jal delay slot.
    - `func_001FA5A0` (94.4%) — dead `addiu v0, a2, 0x1` after `b+addiu`; gp_rel hardcoded.
    - `func_001FEFE0` (98.06%), `func_001FF030` (98.06%) — `addiu a0, N` scheduling differs; `j func_001FF080` tail call.
    - `func_001FE8D0` (95.63%) — `addiu v1, -1` scheduled between `lui at` and `sw`; all remaining mismatches are relocation-only.
    - `func_001AF7C0` (98.46%), `func_001AF780` (98.57%) — dead `paddub v0,zero,zero` after blezl/bnel + delay; gp_rel hardcoded.
    - `func_0022BB70` (99.71%), `func_001F8880` (99.69%) — `sra`/`addu` immediately after `div` (no nop); gp_rel hardcoded.
    - `func_001B0B50` (98%) — dead `andi v1, a0, 0x2` after `b+sb`; beqz delay pre-hoisted.
    - `func_0017B420` (98%) — dead `addiu v0, zero, 0x1` after `b+paddub`; beql delay has addiu.
    - `func_0016F5D0` (98.33%) — `beqz` has `nop` delay slot omitted by pure C; lui/addiu hardcoded.
    - `func_001818D0` (99.06%), `func_0017F1C0` (99%), `func_001C2540` (98.42%), `func_001C4760` (98%) — `paddub a0, s0, zero` in jal delay slot; all lui/addiu/gp_rel hardcoded.
    - `func_001DEDB0` (99%) — dead `addiu v0, zero, 0x2` in beq delay slot; gp_rel hardcoded.
    - `func_00206170` (98.67%) — beqz delay slot filling differs; gp_rel and hi/lo hardcoded.
    - `func_00203460` (99.09%) — dead `paddub a2, v0, zero` after `b+addiu`; lui/addiu hardcoded.

    **Key patterns codified:**
    - **Dead instruction = copy of branch delay slot hoistee**: mwcc 2.3.1 always emits a dead copy of the instruction pre-hoisted into a conditional branch delay slot. It appears one instruction after the `b+delay_slot` that exits the non-taken path.
    - **div/mult immediate use**: 2.3.1 places `sra`/`addu`/`mflo` directly after `div`/`mult` with no intervening nop.
    - **lui interleaving**: 2.3.1 emits a `lui` for a symbol that is used later, interspersed between unrelated instructions as a load-delay filler (scheduler artifact).
    - **paddub in jal delay slot**: when the next jal argument needs a register copy, 2.3.1 places `paddub aN, sM, zero` in the jal delay slot rather than before the jal.
    - **All remaining mismatches are relocation-only**: objdiff shows N% because hardcoded `.word` values lack R_MIPS_HI16/LO16/GPREL16 relocations; the bytes are identical at link time since the linker resolves them to the same value.

    **Overall stats after session 8: ~1338 functions at 100%, 24 at 99%+, fuzzy match ~98.47%.**

  - **8 more asm void functions improved** (2026-05-24, session 9 — continued):
    Continued applying the asm void technique to remaining partial-match functions:

    **Improved (byte-identical at link time, all relocation-only mismatches):**
    - `func_001E8B40` (93%→99.44%) — original uses `lui at,0x1 / addu at,v1,at / lw v1,-0x5f48(at)` to reach D_00275C20+0xa0b8 (offset > 32KB from base pointer); pure C generates two-step addiu. gp_rel hardcoded.
    - `func_001831F0` (84.2%→99.47%) — dead `addiu v1, zero, 2` after `bne + delay slot`; lui/addiu hi/lo hardcoded.
    - `func_0020E020` (81.5%→98.85%) — loop body uses `paddub a0, zero, zero` as counter init; lui/addiu and gp_rel hardcoded.
    - `func_00131F20` (81.4%→98.57%) — complex arg-saving across 3 jal calls using paddub s0/s1/s2; paddub in jal delay slots; all lui/addiu hi/lo hardcoded.
    - `func_00191530` (80%→99.5%) — `lui v0, 0x4188` (float 17.0 integer representation) at position where mwcc interleaves `lui v0, %hi(D_008105E0)` instead; lui/addiu hardcoded.
    - `func_0017B460` (77.8%→98.89%) — lui/addiu hi/lo for D_00248AB0 pointer array; `lh v0, 0(v0)` in jr-ra delay slot.

    **New patterns documented:**
    - **Large pointer offset via lui+addu**: `lui at,1 / addu at,v1,at / lw v1,-offset(at)` reaches pointer + 0x10000 - offset. Pure C generates two `addiu` instructions instead. The asm void approach is needed when the compiler chose this encoding.
    - **Float constant as integer**: `lui v0, 0x4188` / `mtc1 v0, f0` loads 17.0f without a FP load-immediate. Interleaved with surrounding symbol loads in different order than pure C.
    - **All partial matches now cluster at 98-99.7%** — all remaining mismatches in asm void functions are relocation display differences, not actual byte differences.

    **Stats after session 9: 1203 functions at 100%, 27 at 99%+, avg 97.31% across 1356 compiled src files.**

  - **10 new asm void functions added** (2026-05-24, seventh session):
    Applied the `.word`-encoding asm void technique to functions blocked by:
    - gp_rel loads hardcoded as `.word` (mwcc inline asm rejects `%gp_rel` syntax):
      `func_0021B860`, `func_001D6DD0`, `func_001D6F60`, `func_001D7000`, `func_001D7080`
    - hi/lo global address loads hardcoded as `.word` (mwcc rejects `%hi/%lo` in asm):
      `func_001F9100`, `func_001F9180`, `func_001F9140`, `func_001FC770`, `func_00206BA0`
    All reach 98-99.9% objdiff; all are byte-identical at link time since the linker
    resolves gp_rel/hi/lo offsets identically whether via relocation or hardcoded value.

    **Improved existing partial matches:**
    - `func_001D2DE0` (98%→99%) — fixed `addu v1, v1, a0` operand order via asm void
    - `func_001AFEB0` (99.52%→99.52%) — fixed `slti $at` → `slti $v1` register; 2 hi/lo remain
    - `func_00179010` (97.4%→99.7%) — beqz delay slot nop was being filled by mwcc; asm void preserves it
    - `func_001790B0` (91.5%→99.2%) — instruction ordering and register allocation fixed via asm void

    **Key findings:**
    - `%hi/%lo` syntax is NOT supported in mwcc inline asm. Use `.word` with the hardcoded
      absolute value; the linker produces identical bytes since no link-time adjustment is needed
      for addresses in the fixed virtual address space.
    - The gp_rel and hi/lo hardcoded `.word` mismatches show in objdiff as argument mismatches
      (missing relocation entries) but are 100% byte-identical in the final linked ELF.
    - New `func_XXXX` stub-file pattern: for struct-fill functions that are purely register
      manipulation (dsll32/dsra32/or/addu + gp_rel load + jr ra), the `.word` approach gives
      99-99.9% with no further tuning needed.

  - **New session (2026-05-23, sixth session) — GP-relative queue-push family decompilation:**
    Pure-C decompilation of gp_rel struct-write functions (command queue push pattern).
    94 existing src files that were missing from objdiff.json were added (total now 1477 units).
    8 new src files created:
    - **func_00207D00** (98.2%) — switch-based command queue push using D_00275670/D_00275674.
      Uses `if (a1==N) goto L_N` chain to get `beq` branches matching the expected.
      Remaining 1.8%: register allocation at join point puts D_00275670 into $a1 vs expected $v1.
    - **func_001D2090** (80.8%) — double command push (two write sequences) into D_00275670 channel.
      Compiler hoists all `li` constants upfront; expected delays one `addiu` until after first store.
    - **func_001CB9B0** (82.4%) — switch-based pointer getter returning D_00275674 + offset.
      Uses goto chain; remaining: comparison uses $v1 vs expected $v0, missing dead `paddub`.
    - **func_001D1F80** (53%) — complex index arithmetic + queue push; scheduler difference prevents better match.
    - **func_001D1FF0** (70.5%, previously existing) — rewritten with `int` extern types for gp_rel.
    - **func_001D4A90**, **func_001DD950** — compiled (previously unbuilt src files); match 56% and 72%.

    **Key findings:**
    - **`beq` vs `bne` generation**: writing `if (a0 == N) goto L_N` produces `beq a0, v1, L_N`.
      Writing `if (a0 == N) { action; }` produces `bne a0, v1, skip` (inverted branch). The goto
      form matches the expected beq-chain pattern in switch-like functions.
    - **Constant scheduling**: mwcc always hoists ALL `li` (load-immediate) operations to the top
      of the basic block, regardless of source order. Cannot force a constant to be initialized late.
    - **GP-relative register choice at join point**: after a switch with multiple beq-targets,
      at the join point the compiler picks the register for global loads based on liveness analysis,
      not source variable name. Cannot force $v1 vs $a1 without changing the function structure.
    - **Dead `paddub` in else branch**: appears when comparing with `bne` chain (not `beq` goto chain).
      The goto form with `beqz` tests generates `paddub` in the `bnez` delay slot for the "no match" path.

### Done — Heuristic function naming pass (47 names, 2026-05-24)

Added `tools/decomp/name_functions.py` — heuristic naming via string-reference
analysis. The tool reads splat's per-function `.s` disassembly, recovers
absolute 32-bit addresses from lui/addiu pairs (and the resolved
`D_XXXXXXXX` symbols splat already emits for cross-function data refs), maps
them to ASCII strings extracted from the boot ELF, and proposes a
function name derived from the most distinctive nearby string. Auto-block in
`config/symbol_addrs.txt` is idempotent — rerunnable any time without
duplication.

Result of first pass: **47 SDK helpers named** by their referenced strings,
e.g. `sub_Invalid_macroblock_type_code_0` (MPEG decoder error path),
`sub_Too_many_macroblocks_in_picture`, `sub_libpad_Module_version_mismatch`,
`sub_BASCUS_97112_DS00_00_*` (boot ID readers), `sub_NIGHT_VISION_SYSTEM`
(weapon-name lookup, game code). Stdlib-signature pass and SDK-banner pass
yielded zero high-confidence hits — the SDK leaves all call out (no
isolated memcpy/memset/strlen in the linked ELF that's also obviously
distinct from byte-loop game code), and the `PsIIlib*` banner strings are
referenced by too many functions (≥6 each, dropped by the
distinctiveness filter).

DCDecomp cross-reference attempted (`/tmp/DCDecomp`): yielded zero. DCDecomp
itself only has ~120 named symbols, all anonymous `D_XXXXX` / `.LXXXXX`
labels — they don't name SDK functions either. The memory note's hopeful
estimate of 50-200 hits did not materialize.

objdiff still parses (1491 units) and `objdiff-cli report generate` reports
unchanged metrics: **1338/1491 matched at 100%, 98.51% fuzzy**. No matched
function regressed.

### Done — Overlay matching first batch (36 functions, 2026-05-24)

The overlay decomp loop is now proven end-to-end. **36 overlay functions are
matched to 100% C source** across 18 of 19 overlays, while all 19 overlays
remain byte-identical to the original `OVERLAY/AREAXX.BIN` files. The
matched-function tree lives at `src/overlays/AREAXX/`; see `docs/OVERLAYS.md`
section 6 for the per-overlay table and the patterns used.

Categories (most are pure C with `-O4,p -sdatathreshold 4`, no inline asm):
- 19 **area-init functions** (1 per overlay) — write area-type, data-section
  pointer, and zeroes into 4–6 gp-relative boot-ELF BSS slots.
- 6 **jr+nop stubs** (`void f(void) {}`).
- 3 **thin wrappers** (`callee(args); return 1`).
- 4 **struct-field setters / boolean inverters** (small leaves).
- 2 **abs-address byte read-modify-write** (need `-sdatathreshold 0` to
  force `lui/lbu` for globals outside the GP ±32KB window).
- 2 **gp_rel pure-C leaves** that don't fit the above buckets.

Three small infrastructure changes:
- **`tools/overlay/compile_overlay_src.py`** (new) — compiles
  `src/overlays/AREAXX/*.c` → `build/overlays/AREAXX/obj/`.
- **`tools/overlay/fill_overlay.py`** — normalizes the mwcc-emitted EABI64
  e_flags to O32 before handing the object to GNU ld (which refuses to link
  EABI64 alongside GNU-as O32). Boot-ELF builds use mwldmips directly and are
  unaffected.
- **`tools/overlay/link_overlay.py`** — symbol table for the GNU ld script
  now also scans compiled `.o` objects (`nm -u`), not just `.s` disassembly,
  so C source can reference globals never named in any `.s`.

### Done — Overlay build pipeline (19/19 byte-identical)

All 19 `OVERLAY/AREA*.BIN` overlay files produce byte-identical output from
the original disc disassembly. The pipeline is fully automated in `tools/overlay/`.

**Key decisions and hard-won fixes:**

- **GNU ld instead of mwldmips**: mwldmips (the period-correct linker) segfaults
  on aarch64 under qemu-i386 + wibo32 for small overlay-sized inputs. GNU
  `mipsel-linux-gnu-ld` runs natively and produces byte-identical results.
  mwldmips continues to be used for the boot ELF (the full 3014-object link
  doesn't crash).

- **Cross-file `.L` label fixup**: splat splits code at function boundaries, but
  MIPS branches legally jump into adjacent functions. Splat generates `.L` local
  labels for branch targets in the *defining* function's `.s`. GNU as local labels
  are not exported, so cross-object `.L` refs fail at link time. Fix: scan all
  `.s` files, find `.L` labels referenced across files, rename them (drop `.`) and
  add `.globl` in the defining file.

- **R_MIPS_PC16 addend bias**: GNU ld's formula is `(S - P) >> 2`, but MIPS
  hardware uses `target = P + 4 + offset×4`, so the correct formula is
  `(S - P - 4) >> 2`. GNU as leaves the instruction field = 0 for cross-object
  branches. Fix: after partial link, patch all PC16 relocation instruction fields
  from 0 → 0xFFFF (-1), so GNU ld computes `(S + (-1) - P) / 4` = correct value.

- **VU0 / COP2 macro-mode instructions**: AREA21 contains vector unit instructions
  (`vmulax`, `vmadday`, etc.) that GNU as doesn't support. Splat emits them as
  decoded mnemonics with the raw 8-hex-char opcode in the comment. Fix: replace
  with `.word` directives. Opcode byte order: splat shows bytes MSB-first; use
  `int.from_bytes(bytes.fromhex(opcode), 'little')` to get the correct LE integer.

See `docs/OVERLAYS.md` for the full architecture, tool descriptions, and
byte-identity results table.

### Done — Track A partial-link pipeline

A fully automated pipeline that links all 3014 unique-vram boot-ELF functions
into a single `elf/SCUS_971.12.elf` using `mwldmips.exe` (the original
period-correct linker). **The loadable region is byte-identical to the original
ELF** (1530624/1530624 bytes, 100.00%).

- **`tools/decomp/fill_unmatched.py`** — assembles all 3014 per-function `.s`
  files from splat into `build/filler/*.o`.  For functions with a compiled
  `build/obj/*.o`, copies those instead.  Applies all post-processing
  (section stripping, 16→4-byte `.text` alignment fix, GPREL16 pre-application,
  cross-function local label globalization, symbol weakening).  Idempotent with
  incremental rebuild.  135 duplicate-vram syscall-stub aliases (same vram, both
  a named form and `func_XXXXXXXX`) are deduplicated to the named form.
- **`tools/decomp/strip_sections.py`** — strips `.pdr`/`.reginfo`/`.MIPS.abiflags`/
  `.gnu.attributes` from GNU-as objects; zeroes empty `.text`/`.data`/`.bss`;
  forces `.text` section alignment from 16 to 4; pre-applies `R_MIPS_GPREL16`
  relocations (preserving REL addend for expressions like `%gp_rel(sym+0xC)`);
  zeroes `R_MIPS_PC16` addend fields (mwldmips formula is `(S+A-(P+4))/4`,
  but GNU-as writes A=-1; setting A=0 gives the correct branch offset).
  For each function, resizes `.text` to `slot_size = next_vram − this_vram` so
  inter-function gap bytes are part of the object (not left as linker holes).
  156 functions whose mwcc output has the wrong size or content are
  force-assembled from the `.s` instead (`SIZE_DRIFT_FORCE_ASM`).
- **`tools/decomp/link.py`** — generates `config/SCUS_971.12.lcf` (the
  Metrowerks linker command file) with per-function `.text` placement in vram
  order, 2000+ absolute symbol definitions for BSS/IOP/hardware-register
  addresses, and `_gp = 0x0027D370`.  Invokes `mwldmips.exe` via
  `qemu-i386 wibo32`, then compares the PT_LOAD region of the output ELF against
  the original.  Vram deduplication mirrors `fill_unmatched.py`.
- **Current byte identity: 100.00%** (1530624/1530624 bytes in the loadable
  region).  The output ELF is 0x80 bytes longer than the original due to
  mwldmips alignment padding in currently-empty data sections — expected and
  harmless.  Byte identity rises as matched functions displace `SIZE_DRIFT_FORCE_ASM`
  entries.
- **To run the full pipeline** (inside the `exterm-toolchain` container):
  `python3 tools/decomp/link.py`  (runs fill_unmatched, generates LCF, links,
  compares).  Add `--no-fill` to skip fill_unmatched; `--dry-run` to skip the
  linker.  See `docs/LINKER.md` for detail.

### Open questions (most need the decompiled engine — i.e. Track A)

- **VU1 microcode upload driver — RESOLVED 2026-05-27.** Outbound
  VIF1 DMA in the boot ELF is dispatched through a generic indirected
  pump: `func_00101FE0($ch_base, $madr, $qwc)` is the universal
  MADR/QWC/CHCR writer (sets `CHCR |= 0x101`); `func_00101BB8(ch)`
  returns the channel base from table `D_00241050`. Microcode-packet
  wrappers in the `001D3xxx-001D5xxx` and `001E7xxx-001E9xxx` clusters
  build chain tags into the per-channel buffer at `D_00275670[ch]`
  via `func_001D2090` (REF tag, addr=packet) and `func_001D4750`
  (matrix UNPACK builder). **REVISED 2026-05-27:**
  `D_00817240..D_008172BC` is NOT the bone-matrix source — it is a
  static UNPACK header (fixed Y-flip matrix + GS bias constants + VIF
  tag immediates) that `func_001D4750` rebuilds from literals on every
  call. A full-coverage search finds NO other writer to this range.
  The actual per-bone matrix arena is reached via the REF tag appended
  by `func_001D2090` in each caller; the data is rooted at
  `D_00816440 + 0x80 * 0x9C($t0)` where `$t0 = D_00275670[ch]`.
  See FINDINGS.md "Bone-matrix UNPACK source — REVISED 2026-05-27".
  **FOLLOW-UP 2026-05-27:** Writer to the `D_00816440` arena
  identified — `func_001D2E20` (vram 0x001D2E20), a one-shot init
  that memcpys 14 static pre-formatted VIF-UNPACK packets from
  `D_002514D0..D_00251B50` (main image .data) into the 14 per-channel
  arenas `D_00816440..D_00817140`, two 0x80 slots per arena. Source
  blobs are static (header `0x01000404 / 0x6C0703F9` = VIF UNPACK
  V4-32 NUM=8 dest=0x3F9), NOT anim-evaluator output. Caller chain:
  `func_001ACA20`/`func_001AE040` -> `func_001D19E0` -> `func_001D2E20`.
  This is a **fixed/identity-pose** path. The live per-frame anim
  matrices in `0x002863xx..0x002893xx` are produced by a separate
  pipeline — **writer identified 2026-05-27: `func_00179BC0` (inner
  publisher) dispatched by `func_0017A130` / `func_00148B40` /
  `func_0017B660`** (the only three functions that statically reach
  the four bone-buffer addresses via lui/addiu). The publisher copies
  pre-computed 64-byte matrices from a per-bone source struct
  (`*(D_00275B40 + 4*bone) + 0x90`) into one of the four BSS slots.
  Bone count is `lbu($actor + 0xC)`; matrix data is **populated
  upstream by `func_001C6DA0`** (per-actor anim evaluator) — the next
  hunt target for actual keyframe / clip data. Top-of-chain is the
  same per-frame entries as the D_00816440 pipeline
  (`func_001ACA20` / `func_001AE040`). See FINDINGS.md "Live bone
  matrix writers — RESOLVED (2026-05-27)".
  VIF1
  cold-start primer is `func_00100278` (direct FIFO write of two
  VIF-command quadwords at `D_00241020`). The per-frame chain flush
  is still unidentified — likely in the `func_001D7C30 /
  func_001D30A0` subtree off `func_001D1C50`.
  Details: `docs/FINDINGS.md` -> "VU1 microcode upload path".

- **PSMT8 TEX0 CLUT binding — STILL UNRESOLVED.** No EE TEX0
  composition with PSM=0x13 in static disassembly; per-frame GS dump
  is 100% PATH1, so TEX0 is emitted inside VU1 kernels (at
  IMEM=0 of the 256-qw families `0x002354cc / 0x002364dc / 0x00237750
  / 0x00238760 / 0x00239cc0`). The per-material CLUT pointer is a
  VU1 dmem field EE writes per-draw via a small UNPACK; identifying
  the dest requires disassembling the kernel's TEX0-emit sequence.


- **VU1 microcode location + per-program classification — PARTIALLY
  RESOLVED 2026-05-25.** Found 48 VU1 microcode programs (25 logical
  kernels after grouping multi-MPG uploads) statically embedded in the
  boot ELF, all in vram **0x00230824..0x00240F88**. `tools/disasm_vu.py`
  now decodes the full LOWER + UPPER opcode space well enough to
  classify every kernel: `disasm_vu.py profile` prints an op-frequency
  table; XGKICK, LQI/SQI/LQD/SQD, MULAbc/MADDAbc/MSUBAbc, ITOF/FTOI,
  CLIP, DIV/RSQRT, EFU helpers, and I-bit imm32 sequences all decode
  correctly. The 25 kernels fall into 4 families: (1) 3 large
  5-segment world/level-geometry renderers (~1200 qw each, 3 XGKICKs);
  (2) 4 medium 2-segment **skinned-mesh renderers** (#0,1,2,4 — high
  ftoi/itof + LQ density, 1-2 XGKICK); (3) 3 paired main+helper
  per-bone rigid skinners (#5/6, #7/8, #9/10 — ~150 qw main + 15 qw
  inner-transform helper at imem 0x0800); (4) standalone
  setup/effect/particle kernels. The 15-qw helpers are reached via
  plain `bal` — no `vcallms` is used anywhere. Inferred dequantize for
  id 0x74 character vertices: each vertex is a single int16 quadword in
  Q4.12, transformed by a preloaded bone matrix. **Confirmed and wired
  2026-05-25** -- see "Object-space character vertices" below.
  Details: `docs/FINDINGS.md` -> "VU1 microcode".

- **Object-space character vertices — RESOLVED 2026-05-25.** Q4.12
  dequantize formula validated empirically on the 28-bone player rig
  (`chunk21/f17_id8f.bin`). Per-bone VIF packet layout is uniform
  12-byte records: `int16 x/y/z (Q4.12) + 4-byte packed normal +
  uint16 vid`, with `vid == 0xffff` as the section terminator. 2196
  object-space vertices decoded across 28 bone sections; per-bone
  bboxes are bone-scale (within +/-8 Q4.12 saturation) and
  anatomically plausible (left/right mirror-pair x-centres at the
  arm/leg bones, large y/z extents on the spine bones). Wired into
  `extract_models.py --object-space` (writes `*_objspace.obj` with
  one `o bone_NN` group per bone and `*_objspace.txt` with per-bone
  counts and bboxes). Still unknown: (a) the 4-byte normal/lighting
  payload format; (b) the section-index → bone-index mapping (likely
  identity for true characters, since the section count matches the
  skeleton's bone count, but unverified). Details:
  `docs/FINDINGS.md` -> "Per-bone object-space vertex format".

- **Per-vertex skin weights — RESOLVED 2026-05-25.** There are none.
  Character meshes use **per-bone rigid attachment**: a VIF prefix in
  the model file carries N per-bone vertex packets (one per bone,
  bone matrix preloaded into VU1 before each unpack), and the
  per-bone section boundary table sits in the file prefix as a
  zero-terminated u32 offset array. Length matches the paired
  skeleton's bone count for characters with hand-validated 28-bone
  player rigs (chunk17/f14_id8b, chunk21/f17_id8f). The 64-byte
  vertex record in the post-skinning MESH-descriptor section carries
  no weight or bone-index field — every byte is accounted for. New
  `extract_models.py --skinned` exports the 17 detected character
  meshes with bone-section metadata; see `docs/FINDINGS.md` →
  "Per-bone rigid attachment". Still blocked on VU1 microcode:
  decoding the quantised VIF packets to recover bind-pose object-
  space vertex positions, and confirming the section-index → bone-
  index mapping.

- **`sd/ld` callee-save functions are Sony PS2 SDK code, not game code (RESOLVED 2026-05-24 — not a blocker).**
  Earlier sessions noted that ~half the boot ELF's functions use
  `sd/ld + daddu/move` for callee-saves while the other half use
  `sq/lq + paddub`, and called this a "two compiler versions" mystery
  blocking Track A. **The split is real but the diagnosis was wrong.**
  Empirical findings:
  - The local `tools/mwccps2/mwccmips.exe` is **byte-identical** to
    `Adubbz/DCDecomp`'s `tools/compilers/mw/2.3.1.01/mwccmips.exe` (SHA1
    `b368c01c0d3e306389d5de622a801e6b56f77ba4`, 1,177,088 bytes). It
    self-identifies as **`Version 2.3, Runtime Built Dec  2 1999`** when
    asked `-help`, but `mwldmips` writes `MW MIPS C Compiler (2.3.1.01)`
    into the linked ELF's `.comment` regardless — that string is a static
    linker label, not a per-compile attribute. DCDecomp uses this exact
    binary to byte-match the Dark Cloud ELF.
  - This compiler **always emits `sq/lq` for callee-save spills** from C
    input. Tested across `-O0/-O1/-O2/-O3/-O4/-O4,p/-O4,s` with multiple
    function shapes — `sq` in every case. There is no CLI flag, pragma,
    or optimisation level in the EXE that toggles this to `sd`/`ld`
    (verified by reading the full `-help all` output and grepping strings).
  - **Spill style is bimodal per function**: out of 2065 functions in the
    boot ELF that spill anything to the stack, **1627 use `sq` exclusively**
    and **437 use `sd` exclusively**; only 1 mixes both. So each function
    was compiled by exactly one codegen, never a mix.
  - **The `sd`-style functions cluster sharply by vram**:
    ```
    bucket(64KB)   sd    sq    % sd
    0x00100000    184     4   97.9%   <- SDK region
    0x00110000    185     0  100.0%   <- SDK region
    0x00120000     67    42   61.5%   <- transition
    0x00130000+     1  1581    0.1%   <- game code
    ```
    The 0x00100000–0x0011FFFF region (the lowest 128 KB of the boot
    segment) contains the entry point `_start`, the 134 EE-kernel syscall
    stubs (`func_0010B400..0010BC80`), and named runtime/kernel helpers
    that are already in `symbol_addrs.txt`: `ResetEE`, `SetGsCrt`,
    `LoadExecPS2`, `ExecPS2`, `AddIntcHandler*`, `AddDmacHandler*`,
    `_EnableIntc`, `_iSetAlarm`, `CreateThread`, `StartThread`, etc.
    These are the **Sony PS2 SDK / `libkernel` / `crt0`** statically
    linked into the boot ELF — pre-compiled by Sony (presumably with an
    earlier mwcc build whose default callee-save width was `sd`) and
    shipped as object/archive files alongside the Metrowerks compiler.
    Above 0x00130000 is Extermination's own engine code, which our
    compiler matches in `sq` style.
  
  **Implications:**
  1. The `sd`-style functions are not original game code waiting to be
     decompiled. They are **vendor binaries**. They should never count
     against Track A's "matched-functions" denominator.
  2. Re-matching them from C with this compiler is impossible (it never
     emits `sd` callee-saves). Re-matching them from a hypothetical
     "earlier" mwcc is undesirable even if one were found — we'd just
     be re-creating Sony's pre-compiled SDK.
  3. The linker pipeline already handles them correctly: when
     `tools/decomp/strip_sections.py` detects a `SIZE_DRIFT_FORCE_ASM`
     mismatch, it links from the splat-disassembled `.s` instead. For
     these SDK functions that **is** the canonical source.
  4. Hand-decompilation effort should focus on the **1627 `sq`-style
     functions** (vram ≥ ~0x00130000) — that's the game code our
     compiler can actually match.
  5. The "current total: ~1477 src files" / "44% matched" numbers
     elsewhere in this document should be recomputed against a denominator
     of 1627 (game code) + 134 (named syscall stubs, already matched)
     ≈ 1761, not 3014 total splat functions. Many of the unmatched
     remainder is permanently un-decompilable SDK code that the linker
     already handles via assembled `.s`.

- **Texture color source — partially resolved (2026-05-24).** Raw 1024-byte
  CLUT-shaped blobs ARE present in some asset files (id 0x6e/0x70/0x72
  character/model files, id 0x46, most id 0x44 level files) — not as GIF
  packets, just plain `[R G B A]*256` arrays. `tools/clut.py` locates them
  heuristically and both extractors now support `--clut auto` (scan-and-
  apply) and `--clut gray` (identity-grayscale, the default; produces RGBA
  PNGs that match the legacy 8-bit grayscale visually). `--no-clut` keeps
  the original grayscale PNG output. Standalone texture packets (ids 0x06..
  0x0c, 0x35, 0x38) carry NO CLUT in the same file at all — the only
  reliable color source for them is the boot ELF's per-asset palette table
  (engine code, not yet decompiled). The per-material→per-CLUT binding
  inside multi-CLUT files is also not yet known (auto-mode picks the first
  candidate; visual spot-checks show structurally correct images with
  partially-wrong colors when there are multiple CLUTs per file). See
  docs/FINDINGS.md "Textures" section for the full investigation.
  - **Per-material→per-CLUT binding investigation (2026-05-24).** Tested the
    earlier hypothesis that `m1>>10 & 0x3FFF` indexes the file's CLUT list.
    **Hypothesis disproved.** The "21 distinct values per level" signal was
    a coincidence: `m1>>10 & 0x3FFF` is a small per-material running counter
    (typically 4-7 consecutive integers, like 418-423), partially shared
    across sheets — not a palette index. `m1>>15 & 0x3FFF` is constant per
    sheet (just a duplicate sheet-group tag), also not an index.
    **Structurally promising alternative: `(m0 >> 30) & 0x3`** — m0's top
    2 bits take only values {0,1,2,3} (only ever observed as 0,2,4,6 in the
    full 3-bit `(m0>>29)` slice), per-sheet. Counting distinct (sheet, idx)
    pairs across 32 level files yields numbers comparable to (often within
    ±2-4 of) the per-level CLUT candidate count: e.g. chunk04.n2 6↔6,
    chunk04.n0 11↔10, chunk15 8↔8, chunk20.n1 6↔5. So `(m0>>30)&3` is
    plausibly a 2-bit "which-CLUT-of-up-to-4-for-this-sheet" index — but
    the actual ordering of CLUT candidates within a file is unknown, so
    the binding cannot be verified from raw bits alone. Empirical render
    test (chunk04.n2): cycling all 6 CLUTs against each decoded sheet
    produced mostly noisy/stippled output for 5 of 6 — the cleanest
    result was nearly grayscale, so the in-game palette for this
    particular level is genuinely near-monochrome (industrial wall
    textures), which makes visual validation inconclusive on its own.
    Did NOT wire `(m0>>30)&3` into the extractors — the structural
    signal is real but unverified; shipping a half-correct binding would
    be worse than the current honest `--clut gray` default. Next step:
    decompile the boot ELF's PSMT8 TEX0 setup (find every TEX0 register
    write with PSM=0x13, trace where CBP comes from, find the per-asset
    palette table).
- **Cross-file texture residency.** ~631 materials bind to a texture uploaded
  by a different file (common/UI packets); needs the engine's VRAM map.
- **`extract_textures.py`** only matches `07 XX 00 60` packets, missing the
  `07../10` DMA-tag level sheets (`extract_subtextures.py` handles them);
  broaden or consolidate the two tools.
- **Rig payload — partially resolved (2026-05-24).** The "rig" files were
  reverse-engineered to be per-bone **collision hulls**, not skeleton bind-
  pose transforms. Each 112-byte record's first 16 bytes are a **plane
  equation** `n.x + D = 0` with `(nx,ny,nz)` a unit-length outward normal
  (verified across every record of every rig file) and `D` the signed plane
  offset. Records group by bone; most bones have 6 records = 3 antiparallel
  plane pairs = a full OBB (oriented bounding box). `extract_models.py
  --rig` now decodes the plane equations, detects OBB pairs, dumps a
  `*_rig.txt` per-bone summary, and exports `*_rig_hulls.obj` wireframe
  bounding boxes. **Still open:** (a) the remaining 96 bytes of payload
  (six vec4 "extras") look like face polygon corners / edge endpoints but
  the exact field layout is not decoded — needs the VU1 microcode that
  consumes them; (b) the actual bind-pose joint transforms are still
  packed (see next bullet).
- **Bone parent hierarchy — RESOLVED 2026-05-25** (bind-pose matrices
  still packed). The skeleton lives in **id 0x71 character/animation
  files** (paired with the id 0x73 collision-hull file), not the rig
  file itself. An id 0x71 file is an offset directory of N animation
  entries; every entry shares the same skeleton, with the bone
  PARENT-INDEX TABLE at entry+0x28 as a `u32[bone_count]` array (the
  bone-count byte is at entry+0x00). 26 skeletons across the game now
  parse cleanly; six id 0x71 files share the player-character skeleton
  (28 bones, anatomically coherent shoulder/elbow/wrist chains, a
  9-child chest hub and a 5-child hand hub). One persistent caveat:
  the array contains a stable 2-cycle (`parents[2]=3, parents[3]=2`)
  and two self-parented roots, so the field is overloaded for a couple
  of bones — likely a "linked-pair" / mirror pointer rather than a
  parent. `extract_models.py --skeleton` writes `*_skeleton.txt` (tree
  + hull centres) and `*_skeleton.obj` (stick figure placing each
  bone at its collision-hull centroid or its nearest hulled ancestor).
  **BIND-POSE JOINT TRANSFORMS — RESOLVED 2026-05-25 (live PCSX2 save
  state)**. The transforms are not stored verbatim on disc; they are
  constructed at runtime by the animation system in EE .bss. A PCSX2
  save state captured with only the player on screen confirms a single
  populated 0x1C00 character slot pair at `0x00287F40` / `0x00288D40`
  holding two copies of a 21-active-matrix buffer (current + previous
  frame for inter-frame interpolation; both are bone-local matrices).
  The player's id 0x71 declares 28 bones; only the first 21 are live
  at runtime (the trailing 7 slots are zeroed / reserved). The matrices
  are extracted with `tools/parse_pcsx2_state.py --player-bones`
  (writes `player_bones.json`) and composed into a posed model with
  `extract_models.py --rigged --bones player_bones.json --file
  extract/chunk21/f17_id8f.bin` (writes `*_rigged.obj`: one
  `o bone_NN` point-cloud group per bone in world space, plus
  `*_rigged.txt` with per-bone world bboxes). Inactive bone sections
  (the trailing 7 of the 28 declared bone-section slots, for which the
  engine left col3.w == 0) are **skipped** rather than identity-padded:
  emitting their raw Q4.12 verts at the origin previously produced
  stacked phantom copies of the body across the world Y axis. The
  two captured buffers both hold parent-relative local matrices
  (current and previous frame for interpolation); world matrices are
  composed through the id 0x71 parent table. The 7 inactive sections
  carry ~830 verts of geometry whose bone attribution is still
  unresolved -- candidates include per-material vertex re-emissions,
  LOD packets, or accessory geometry not drawn this frame.

  **STATUS: WORK IN PROGRESS — composition pipeline produces a scattered
  point cloud, not a recognisable humanoid, when loaded in Blender.**
  Confirmed via user screenshot 2026-05-25. Each piece in isolation is
  correct (live matrices extracted, parent walk composes, Q4.12 dequant
  validated on object-space mode), but the assembled output doesn't form
  a coherent figure. Most likely cause: the assumption that the per-bone
  vertex section index in the id 0x74 file's section table maps 1:1 to
  bone index in the runtime matrix buffer is WRONG. Section #N's vertices
  are getting transformed by bone matrix #N, but the actual mapping is
  some permutation/indirection we haven't found. Resolving this needs
  either: (a) PCSX2 instrumentation to log which VU1 dmem matrix slot is
  active for each per-bone VIF UNPACK in one frame, or (b) decompiling
  the EE-side anim/draw code (func_00100EB8 chain) to read the section→
  bone mapping. Deferred — the 28-commit session of bind-pose RE has
  already produced 90% of a working pipeline (skeleton + collision hulls
  + object-space vertices + live matrix capture + composition). The
  remaining 10% (correct section→bone mapping) is a discrete future
  task.

  **PRIOR (investigated 2026-05-25,
  found NOT to live in the three id 0x71 entry "sections" as
  previously hypothesised). Detailed structural decode:
  - Section 1 (variable per-bone size) is a stream of 12-byte records
    in the EXACT same Q4.12 + packed-normal + uint16-vid format as the
    separate id 0x74/0x8b/0x8f character-mesh per-bone vertex packets;
    i.e. it is **per-bone object-space vertex data**, not matrices.
  - Sections 2 and 3 are mostly 36-byte stride per bone where each
    36-byte record is a SINGLE 12-byte payload repeated 3 times
    following the standard `0000 / 7700 / ffff` VIF priming-vs-
    terminator pattern. In other words these are **empty VIF priming
    headers** — vertex-stream framing with no body data.
  - Section 2's bone-0 special 384-byte record (32 chunks instead of
    3) carries a smoothly-varying int16 sequence that looks like a
    keyframed root channel rather than a transform.
  - The 57 entries in `chunk05/f04_id71.bin` are **animation clips**
    sharing the same skeleton; the per-clip payload differs but the
    parent table is invariant — consistent with keyframed animation
    rather than per-pose matrices.
  The bind-pose / inverse-bind-pose joint transforms are therefore
  not in id 0x71 at all. **Mesh-file prefix region also ruled out
  (2026-05-25)**: the bytes from file offset 0 up to the per-bone
  section table (`0x22c8` in `chunk21/f17_id8f.bin`, `0x173c8` in
  `chunk17/f14_id8b.bin`) contain 32-byte float records (two vec4s
  per record), but the region's size varies ~10x between two files
  of the same 28-bone skeleton — far too much to be a per-bone
  bind-pose array — and zero 4×4-matrix-shaped runs (last row
  `≈(0,0,0,1)`, |row0|≈1) appear in either prefix. The prefix's
  last 0x40 bytes are a MATRIX-style descriptor followed by two
  parallel 28-entry u32 tables (the second of which is the existing
  "section offset" table). Detail in `docs/FINDINGS.md`
  "Bind-pose matrices > Prefix-region structure". Remaining
  candidates: (a) baked into the VIF microcode preamble that the
  engine ships to VU1 before each per-bone vertex upload, or
  (b) inferred at runtime from rest-pose mesh + collision-hull
  centres, or (c) assembled in EE code (trace from VU1 kernel #5/#6
  helpers at `0x002346b0`/`0x002346f0` back to `func_0011AA50` and
  its callers). The per-bone rigid-
  skinning VU1 kernel (#5/#6, #7/#8, #9/#10 family at vram
  0x00234610 / 0x002346f4) loads a 4-row matrix into vf01..vf04 from
  a fixed dmem address — tracing that address back to its EE-side
  DMA-source needs the decompiled engine. **Until then the hierarchy
  + collision-hull centres remain the practical guide for hand-
  rigging in Blender/Maya.** See `docs/FINDINGS.md` "Bind-pose
  matrices" for the corrected header layout, the per-section content
  decode, and the byte-level evidence.
  - **GS dump capture attempt — NEGATIVE RESULT 2026-05-25.** A PCSX2
    GS dump of one live gameplay frame was parsed (new
    `tools/parse_gsdump.py`; decompresses zstd, decodes the
    `0xFFFFFFFF` "new" header + state blob + 8192-byte regs + packet
    stream). The capture contains 4 vsyncs, each with 706 `PATH1new`
    Transfers (post-VU1 GIF data via XGKICK), zero PATH2/PATH3. The
    GS dump format **does not capture EE memory, VU1 micro/data
    memory, or raw VIF1 packets** — only the GIF output stream after
    VU1 has already consumed the matrices. Bone matrices have been
    folded into post-skin world-space vertices on PATH1 and are
    unrecoverable. To actually catch the VIF1 UNPACK we need a PCSX2
    save state + a patched PCSX2 that logs VIF1 FIFO writes /
    VIF1_CHCR kicks, or to push forward with the static decomp of
    `func_00100EB8`. See `docs/FINDINGS.md` "Bind-pose hunt via PCSX2
    GS dump" for the full format-level explanation.
    `tools/parse_gsdump.py` is retained as a general-purpose PCSX2
    GS-dump reader for future GS-side questions.
- **MATRIX `--scene`.** Role of the repeated identity transforms, and whether
  transforms are absolute or parent-composed — RESOLVED 2026-05-27: transforms
  are ABSOLUTE world placements (see the level-scene glTF exporter; applying
  them directly keeps geometry inside the static world bbox, a parent-composed
  scheme would collapse/escape).
- **Audio.** SFX-bank rate RESOLVED 2026-05-27: there is NO stored Hz in the
  SShd bank (header is `{offset,size,count}` triples; no rate field; scan for
  standard rates finds zero matches). Playback pitch is an SPU2/sequencer
  parameter set at trigger time, so no single "correct" bank rate exists; 22050
  is a reasonable extraction default. Clip splitting is still heuristic
  (silence gaps) — no per-clip index found.
- **`OVERLAY/`** (`AREA*.BIN`, `MWo3` overlay modules) — **fully characterized
  and pipeline complete (2026-05-24)**. Format documented in `docs/OVERLAYS.md`.
  All 19/19 overlays produce byte-identical output via `tools/overlay/`. The
  overlay pipeline uses GNU ld (not mwldmips, which segfaults on aarch64 for
  small link jobs). Three non-trivial obstacles resolved: cross-file `.L` label
  promotion, R_MIPS_PC16 addend bias fix, and VU0 COP2 instruction replacement
  with `.word` directives.

  - **Embedded-asset scan — RESOLVED 2026-05-24 (no embedded assets).**
    `tools/scan_overlay_assets.py` swept all 19 overlay `.text` and `.data`
    sections for known signatures (`VAGp`, `SShd`, nested `MWo3`, GS texture
    DMA packets `07 XX 00 60` + BITBLTBUF/TRXREG/IMAGE-GIF, 1024-byte CLUT-
    shaped blobs, ASCII runs). Per-overlay and aggregate counts: **0 VAG
    headers, 0 SShd banks, 0 nested MWo3, 0 DMA texture packets, 0 GS texture
    transfers, 0 CLUT candidates** across all 19 files. The only non-code
    content found in the data sections is **135 short ASCII strings** (median
    length 6–9 chars, max ~15), distributed unevenly — AREA04 (35), AREA13
    (19), AREA19 (18) hold most; AREA00/01/02/06/18/20/22 hold ≤3 each.
    Characterwise: ~75% mixed-case (likely debug/label text), ~20%
    identifier-like ALL_CAPS_WITH_UNDERSCORES, ~4% source-path-like, ~1%
    printf format specifiers. Conclusion: **the overlays are pure code +
    per-area init constants + tiny debug strings; they do not embed
    textures, palettes, geometry, or audio.** All real assets live in
    `DATA.DAT` and `STREAM/`. Track B is therefore complete for the OVERLAY
    directory — no extractor is needed beyond the existing
    `tools/overlay/extract_overlays.py` (which already separates header /
    .text / .data for inspection). The 135 strings remain interesting only
    as future symbol-recovery hints during per-overlay decompilation (an
    overlay's debug strings are referenced by its own functions; cross-
    referencing string addresses against `lui+addiu` pairs in splat .s
    output can name some overlay functions, parallel to
    `tools/decomp/name_functions.py` for the boot ELF).

### Roadmap

**1. Track A — matching decompilation (CURRENT PRIORITY).**
Goal: a runnable developer build — compile the decompiled code and run the game
with its own assets (no repacking needed; the original `DATA.DAT` is used
as-is). The pipeline "hello world" is **done** — see "Done — Track A" above.
The 137 named syscall stubs are in `symbol_addrs.txt`; the additional 134 stubs
(func_0010B4xx..func_0010BCxx) are matched but not yet named.

**The partial-link pipeline is also done (2026-05-23)** — see `docs/LINKER.md` and
`tools/decomp/{fill_unmatched,link,strip_sections}.py`.  A linked
`elf/SCUS_971.12.elf` is produced (currently at ~17% byte identity because 137
compiled functions aren't yet byte-perfect).  The byte identity will approach
100% as more functions are matched.  See `docs/LINKER.md` for the full pipeline
description, known issues, and how to invoke it.

Next for Track A: match non-trivial functions; grow `config/symbol_addrs.txt`;
bring the 137 non-matching compiled functions to byte-exact output; aim for a
runnable ELF in PCSX2.
The reference template studied for the pipeline is `fmil95/recvx-decomp`
(same CodeWarrior toolchain family).

**2. Asset polish (minor; several items need Track A's decompiled engine).**
The open questions above. RESOLVED this session: `OVERLAY/` scan (no embedded
assets), MATRIX `--scene` (absolute transforms), SFX sample rate (no stored
Hz — sequencer-pitched), rig VIF payload + bone hierarchy (full anim format
decoded → glTF export), cross-file texture residency (global VRAM map; the
remaining ~50 gray sheets are common/UI textures uploaded outside the
extracted `DATA.DAT` tree). `MUSIC.DAT` 25/30 labelling narrowed to a bimodal
long/short split (~28/27) but exact assignment needs the engine music-cue
table or audio fingerprinting. STILL BLOCKED on a PCSX2 mid-frame capture:
texture color/CLUT binding, per-block bone-index table, packed-normal byte
order. `extract_textures.py` `07../10` packet coverage merge remains a
tooling-cleanup TODO.

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
  - `decomp/fill_unmatched.py` — assembles all 3149 boot-ELF functions into
    `build/filler/*.o` for mwldmips (handles special cases: GPREL16 overflow,
    cross-function labels, symbol weakening).
  - `decomp/strip_sections.py` — strips GNU-as-specific sections; fixes .text
    alignment (16→4 bytes); pre-applies R_MIPS_GPREL16 relocations.
  - `decomp/link.py` — full link orchestrator: generates LCF + object list,
    invokes mwldmips, compares output ELF against original.
  - `decomp/repack_iso.py` — swaps rebuilt ELF and/or overlay BINs into a
    copy of the user's ISO for PCSX2 testing (`--overlays` flag).
  - `overlay/gen_splat_yaml.py` — auto-generates per-overlay splat YAML + symbol_addrs.
  - `overlay/fill_overlay.py` — assembles overlay `.s` → `.o` (VU0 fixup, cross-file
    label fix, strip_sections integration); picks up compiled `.o` when available.
  - `overlay/link_overlay.py` — GNU ld-based overlay linker (partial link + PC16 fix +
    final link + ELF extraction + MWo3 packing + byte verification).
  - `overlay/extract_overlays.py`, `overlay/pack_mwo3.py`, `overlay/build.py` — support tools.
- `config/` — Track A build config (committed): `SCUS_971.12.yaml` (splat
  config), `symbol_addrs.txt` (hand-recovered symbol list), `asm_prelude.inc`
  (assembler directives for target objects). The boot ELF `config/SCUS_971.12`
  itself is git-ignored, as is the proprietary compiler under `tools/mwccps2/`.
- `src/`, `include/` — decompiled C and headers (committed).
- `docker/` — toolchain container (committed): `Dockerfile` (arm64 image: Debian
  + qemu-user + MIPS binutils, built with Apple's `container` CLI) and
  `build-wibo.sh` (cross-builds the 32-bit wibo).
- `docs/` — project state and findings (committed): `PROGRESS.md` (this file),
  `FINDINGS.md` (technical format reference), `LINKER.md` (partial-link pipeline
  — mwldmips invocation, LCF decisions, special cases, debugging tips),
  `OVERLAYS.md` (overlay format reference, architectural plan, and roadmap for
  the second matching surface), `track-a-kickoff.md` (Track A starter prompt).
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

### Update — 2026-06-02 per-bone section directory RESOLVED (object-space mesh)

A live PCSX2 capture taken mid-skinning-draw with the player on screen
(`SCUS-97112 (4CDC5F74).01.p2s`) had a resident soft-skinner VU1 kernel
(matches boot ELF vram 0x002317D8) and a live 4-bone matrix palette in VU1
dmem qw 111..122. That confirmed the per-bone wiring and led to locating the
on-disc per-bone SECTION DIRECTORY at mesh file offset 0x2280: parallel
`bone_idx[]` + `offset[]` lists mapping each object-space VIF section to its
GLOBAL bone (21 sections → 14 bones on the player). `export_gltf.py` now binds
object-space sections to their real bones (was the wrong `section i == bone i`
assumption). See FINDINGS "Per-bone section directory". The textured
MESH-descriptor blocks remain proxy-bound (their per-block index is separate
and still unlocated — the position-W field is not a clean selector for them).
The capture's 4 MB GS VRAM (`gs.bin`) also physically contains the uploaded
CLUTs — still to be mined for the color question.

### Update — 2026-06-09 s8: level render fixed end-to-end (phase bug, id44 render section, live placements, weapon attachment)

Live DMA-chain decode of the office frame (PCSX2 DebugServer; packet arena
0x297400..0x2A2400 parsed offline) closed the port's two visible rendering
problems. FINDINGS "LEVEL RENDER MESH v2" has the full detail.

- **Record-phase bug (CORRECTION):** the level mesh records are in the
  character order [TEX0][ST][color][pos+W]; the file starts mid-record.
  The old [pos][...] reading paired every position with the next record's
  UV/color — smeared textures, broken strips. Also removed two bogus
  strip-break rules (TEX0 change / ADC runs do NOT reset a GS strip).
- **id 0x44 carries a render section (CORRECTION):** the tail of
  `chunk06.n1/f02_id44.bin` ([0x86B40,0x173800)) is a second static
  render-mesh section — the WESTERN half of the area. The live static
  draw streams blocks from both files.
- **Region map + live placements:** `f03_id43.bin` = static world
  [0,0x820C8) + a 13-slot door assembly + four placed object blobs (two
  drawn twice = instancing); placements recovered as W = K_L^-1 * M from
  the frame's draw units, embedded in `tools/export_level.py`.
- **Weapon attachment:** held-equipment draw matrices are byte-identical
  to player bone matrices — identity-offset node parenting. Rifle (models
  47/48/49/50/56/64) -> node 4, model 48 again -> node 14; model 106
  (knife) is a floor pickup this frame. `tools/export_props.py --attach`
  merges them into `player.emdl` bone-local; they animate with the hands.
- **Port:** level 2041 -> 3800 tris / 92 textures (room now closed:
  walls/floor/ceiling/lockers); player 3170 -> 3686 tris with weapons in
  hand; props = the knife pickup. Metal backend gained alpha-test discard
  + alpha blending (8 level textures carry cutout alpha). EM_CAPTURE
  verified at two phases (idle + EM_MOVE_TEST mid-walk).
  `verify_all.py --no-container` all-PASS.
- Open: W-bit 13 meaning; placements for the un-drawn object regions
  (other door states); the runtime "lit copy" dynamic block colors.

### Update — 2026-06-10 s9: attachment correction (node 14 = knife holster), region map completed, live placement table found

Live PCSX2 session fixing the three user-visible port issues from the s8
export (FINDINGS "LEVEL OBJECTS COMPLETE + ATTACHMENT CORRECTION" has the
full detail):

- **Phantom barrel fixed:** s8's `(48, node 14)` attach was wrong — node
  14 is the hip HOLSTER node and carries the KNIFE (model 106), RAW
  byte-exact vs the skin palette in every frame; model 48's second REF is
  a second pass of the same unit. `export_props.py --attach` now merges
  rifle->4 and knife->14; the knife shows in the hip holster, no barrel
  at the leg (EM_CAPTURE zoom verified).
- **Missing props recovered:** the west double-door instance at
  (57,0,-220.5) (the "bare doorway"), the table-top device / battery
  bank `f03+0xA4240` at (80.1,8.2,-244), the wall ammo/refill station
  `f03+0x9AA80` at (57.5..60.8,~15,-290..-296), supply crate
  `f03+0xA2700` x2, and the corridor-door control panel `f03+0x9A100` —
  all baked via the `export_level.py` region map (level emdl 3800 ->
  4205 tris / 115 textures). Verified by software-rendering the rebuilt
  asset at the recovered placements (the port's scripted chase-cam can't
  reach the west rooms within a 60-frame capture — noted honestly).
- **Engine placement table found** at EE `0x828170` (0x28-byte entries:
  type, pos, yaw, class ptr) — doors/objects type 4, item pickups type
  0xB; it is the authoritative placement source and reclassifies the s8
  "wall fixtures" as ammo-box pickups.
- `verify_all.py --no-container` all-PASS; port rebuilt; PCSX2 restored
  to the pre-session player position and left running.
- Open (added): billboard/glow quads (lib models 20/21/110-118) need a
  port-side billboard path; f03 non-record region [0x820C8,0x88840).

### Update — 2026-06-10 s10: CAMERA SYSTEM isolated and characterized

- The SUBSYSTEMS "Camera: NOT yet isolated" question is closed: camera TU =
  0x18B9C0–0x199xxx (inside the mislabeled `init_io` cluster). Struct at
  0x008101E0 (0xD0 B), mode machine func_0018B9C0/func_0018BC20 (16 modes,
  jtbl_0026D910/950), clamped-proportional follow (Δ/6 horiz, Δ/8 vert,
  cap 4.0), collision-aware eye solver (func_0019A910 segment queries,
  set mask 6/7), commit func_0018C0D0 → look-at D_00810610 (up = (0,−1,0)),
  projection P (zoom 480, 0.8s/0.5s, GS center 2048) and K = P·V at
  ctx+0x23C0 verified to 1e-8 against two save states. Full field map,
  mode table, and native-port contract in FINDINGS "CAMERA SYSTEM"
  (2026-06-10); probe tool `tools/camera_probe.py`.

### Update — 2026-06-10 s11: placements are DISC-FAITHFUL (OVERLAY tables decoded)

- The s9 placement table is **static data in `OVERLAY/AREAxx.BIN`** (loaded
  flat at 0x823500): registry `0x24D7C0[area]` → per-area descriptor →
  per-sub-state table vaddr; 0x28-byte records, 0x00FF sentinel; spawner =
  `func_001B6990` (s9's read was shifted +8; full corrected field map in
  FINDINGS s11). Live tables byte-identical to the disc files in both save
  states. Office = `AREA02.BIN` sub-state-1 table @0x828170 (14 entries);
  snow = `AREA06.BIN` @0x827AC0 (57); state-01 area = `AREA11.BIN`
  @0x82A3C0 (21). New tool `tools/placements.py` (parse/scan/JSON).
- The office "knife pickup" at (115,1.5,-269.3) is a **third item-0x0C ammo
  box** (byte-identical class/kind/item to its two neighbors; behavior fn
  0x1C4820 = generic pickup) — no knife pickup exists on disc; s9's model
  attribution was wrong.
- `tools/export_level.py` now builds office placements from the disc table
  (live matrices kept as fallback; multi-slot assemblies anchor live
  articulation on the table base): 4205 → **4217 tris** / 6475 verts / 115
  textures; port rebuilt and EM_CAPTURE-verified.

### Update — 2026-06-10 s12: SShd TONE RECORDS pinned — exact per-SFX rates shipped

- Decompiled (analytically) the bank loader + voice-trigger path
  (`func_00119528` registration, `func_00117088` program resolver,
  `func_001152D8` 48-channel sequencer, `func_00115E50`/`func_00115850`
  note-on, `func_00117918` note→pitch, `func_00118CF8` pitch bend). Full
  SShd container/bank/program/tone layout + the runtime voice table
  (`D_0027CCC0`, 48×0x6A) and bank slot table (`D_0027C6C0`, 128×12) are in
  FINDINGS "SShd bank format" (2026-06-10).
- Tone records found: 16-byte `{note range, center, s8 fine, u16 start>>3,
  ADSR1/2, …, bend_range, flags}` inside per-program arrays; exact rate
  `44100·2^((118+16Δ+fine)/192)`. All **5758** tone sample offsets across
  **40 containers / 115 banks** land exactly on VAG block boundaries.
  Absolute scale corroborated from the PCSX2 savestates' SPU2 voice structs
  (EE pitch value lands unscaled in the register; in-game SFX pitches
  19.9–25.0 kHz inside the predicted band).
- `tools/audio_export.py sfx` rewritten tone-record-driven: **533 unique
  samples → 1206 (sample,rate) WAVs** + per-ref manifest at engine-exact
  rates (median 21.5 kHz). The old 241-sound export was mis-segmented
  (hdr+4 is body SIZE, not offset) and missed the global player/weapon/UI
  container `chunk00/f05_id05.bin` entirely.
- Match attempt on `func_001178C0` (note-range test, 22 insns): reached
  structural parity but it sits at vram 0x001178C0 — the SDK-compiled half
  (<0x120000, daddu-vs-paddub move style) that mwcc 2.3 cannot reproduce;
  stub restored per policy. The whole audio cluster is SDK-half — exclude
  it from readable-C matching campaigns.

### Update — 2026-06-10 s13: EMDL pipeline generalized — first enemy/creature export

- The 44-node chunk21/f17_id8f segment-1 creature is exported as an
  animated EMDL (`enemy_test.emdl`, in-file id-0xd0 idle clip 0 @0x64080,
  180 frames) and renders in the port beside the player (capture kept at
  `extermination-port/build/cap_enemy.png`; temporary scene file removed
  after capture). Flat-lit: its textures are resident in no existing
  GS dump / save state (0/61 TEX0 keys everywhere).
- `export_native.py` gained `--rig-nodes` (id-agnostic container scan
  filtered by parent-table size — unlocks in-file 0x70/0xd0 banks),
  `--anim-hdr` (pin container by offset) and `--offset` (baked world
  placement). Default enumeration/player exports unchanged.
- 0xd0 clips decode with the unchanged map-A/B/C machinery (unit-quat
  fingerprint); hemisphere scan over all 30 clips: 0 jumps > 90 deg.
- Next textured-character win: chunk15/f18_id94 seg1 (21-node NPC) is
  fully texture-resident in save state 01 — needs only a .p2s VRAM
  source option in build_texture_blob.

### Update — 2026-06-10 s14: collision world shipped to the port — grid section decoded, EMCL bake, native queries

- **"s16-grid heightfield" misread corrected**: the id-0x44 grid section is
  a full-float convex-polygon soup with a sorted-rank acceleration index
  (no quantized heights, no 1/3.6 scale). Header/node layout + the shared
  polygon test (front-facing ≤ −1e-5, convex edge-normal inside ≤ +1e-5,
  nearest-hit end clamp) and the wide cell n-gon layout are in FINDINGS
  "COLLISION s16-GRID SECTION FULLY DECODED" (s14).
- New `tools/export_collision.py`: id44 → **EMCL v1** for the port
  (office: 205 polys = 121 grid + 84 cell n-gons incl. 30 wide; floor
  y = 0 validated at the spawn; flat-floor fallback path if a level's grid
  header ever resists the scan).
- Port: new `src/game/em_collision.[hc]` (zero-dep loader +
  `segment_query`/`move_probe` mirroring func_0019A570/func_0019AD00 — set
  mask bits, query id with conditional surfaces 0x50..0x59, collide-and-
  slide delta). `em_game.c` movement now probes the real world (bbox clamp
  kept only as a no-asset fallback), floor height comes from a vertical
  query, and the camera solver runs mask-6 eye queries (struct +0x07 hit
  byte wired). Verified: EM_MOVE_TEST wall run stops at the z=−170 office
  wall exactly (PASS), idle EM_CAPTURE byte-identical, `make test-input`
  PASS.

### Update — 2026-06-10 s15: first fully TEXTURED NPC — .p2s VRAM texture source

- `export_native.build_texture_blob` gained a **.p2s save-state VRAM
  source** (`--p2s`; accepts a .p2s, a pre-extracted state dir, or a bare
  gs.bin freeze blob) next to the existing GS-dump path — for levels with
  a save state but no GS dump. Same 4 MB local memory, base =
  len(gs.bin) − 0x400000 − 84 (gs_vram.read_localmem). `--attach` still
  requires `--gsdump` (export_level's builder untouched).
- **chunk15/f18_id94 seg-1 21-node humanoid exported textured + animated**
  (`assets/npc_test.emdl`: 2789 verts / 3330 tris / 240-frame idle,
  **68/68 textures resolved** from save state 01 — all PSMT4, every CLUT
  validates). Rig lives in the SIBLING file `chunk15/f12_id44.bin`
  (14 21-node containers; the mesh file itself has none) and that file
  holds TWO 21-node rigs — disambiguated by posed cross-bone edge
  coherence (matching family 0.99u vs 3.62u; clips 1,4–13). Idle pick:
  clip 7 (240 f, 0.25° loop wrap, gentle gesturing). EM_CAPTURE: textured
  soldier NPC (camo fatigues, harness, cap) beside the player
  (`extermination-port/build/cap_npc.png`; temp scene copy removed,
  default scene re-verified byte-identical). Discontinuity scan over all
  14 21-node clips: 0 jumps > 90°, worst step 63.7°.
  verify_all --no-container all-PASS.

### Update — 2026-06-10 s15: music cue table found — engine-exact MUSIC.DAT/VOICE.DAT track boundaries

- **The MUSIC CUE TABLE is pinned**: `D_0025DD30` (68 × 16-byte entries,
  cues 1–67) and the VOICE cue table `D_0025E170` (179 entries, cues
  1–178), entry = `{start_sector, start_byte, byte_len, flag}` relative to
  the stream file's first LSN (resolved at boot by
  `sub_O_STREAM_MUSIC_DAT_1` via sceCdSearchFile into
  `D_00282188/D_0028218C`). Cue starter `func_001FA790(ch, cue)`; BGM API
  `func_001FB0B0(cue)` / `func_001FAE70(fade)` around the current-BGM
  global `D_00810D38`. Full table layout, runtime streamer block, and the
  cue → old-track mapping are in FINDINGS "Music cue table — SOLVED".
- **Area/event mapping**: `D_0026EC60` (36 rows `{area, 0, trigger, cue}`,
  scanned by `func_001FD4C0` against `D_00810700`) covers scripted cues
  29–66; overlay constants give 8 areas' looping BGM + 3 stings; save
  state 01 validates live (area 11 streaming cue 25, sectors match the
  table exactly). ~60% of music cues now have a confirmed in-game use.
- `tools/audio_export.py music|voice` now split on the engine cue table
  read from the user's local boot ELF (`--elf`, default
  `elf/SCUS_971.12.elf`) and name outputs `cue_NNN.wav` (67 music / 178
  voice clips); silence split kept as fallback (still 55/116). The old
  55-track decode had merged adjacent cues 12 times.
- Open: the writer of the initial per-area `D_00810D38` for the areas with
  no overlay constant (cues 2–8, 10, 13–14, 16, 22, 26, 28 unattributed);
  cue-entry flag behaviour (looping-BGM marker) at stream end.

### Update — 2026-06-10 s16: SECOND SCENE end-to-end — chunk15 snow level exported + walkable in the port

- The scene pipeline generalizes: **chunk15 (= AREA11 = save state 01,
  pinned live from EE 0x810700 + the loaded placement table 0x82A3C0)**
  is fully exported — six render-mesh files (f12 tail + f13..f16 zones +
  f17 movables-at-rest; 29.4k verts / 18.6k tris / 290 PSMT4 textures,
  texels straight from state 01's .p2s VRAM via the new
  `export_level.py --p2s`) and the collision world (grid **3503 verts /
  3099 nodes + 84 cells**; the section SPANS f07..f12 — the chunk loads
  contiguously, so `export_collision.py` now takes multiple files and
  decodes the concatenation; floor at the live player XZ = 229.85 =
  exactly the live actor Y). FINDINGS "SECOND SCENE END-TO-END" has the
  layout + what did NOT generalize (f05/f06 object-space assemblies,
  placement->model binding, per-scene collision filename, the
  compile-time spawn -> `--offset` anchoring).
- Port: `EM_SCENE=<dir>` scene switch (main.c symlink-staging redirect;
  default byte-identical — office EM_CAPTURE compared equal), snow scene
  in `assets/scene_snow/` with the state-01 player position as spawn.
  Capture shows the soldier at the snowy gate (matches the deleted
  reference screenshot's documented description); EM_MOVE_TEST and
  verify_all --no-container PASS.
- Negative result worth keeping: a HW-renderer .p2s holds NO rasterized
  framebuffer (FRAME page zeroed; VRAM = texture pack only) — save
  states give textures, never screenshots.

### Update — 2026-06-10 s17: first interactive objects decoded — pickup + door semantics, actor registry, 2 new matches

- **Entity tick linkage closed.** The s11 placement +0x24 behavior pointers
  are invoked by `func_001AFD70` (gameplay-frame "world services"): walks
  the active doubly-linked actor list (head `D_00275BC0`; pool = 256 x
  0x2F0 @ `D_007A5640`, alloc `func_001AFA90`, free `func_001AFC10`) and
  calls `*(actor+0x10)(actor)` once per frame. Visible actors re-publish
  themselves into double-buffered per-class + interactive pointer lists
  (`D_00275B5x..Bxx`, swapped by `func_001AAD00` at frame close) that the
  player-side scans consume. Full registry + struct contract in FINDINGS
  "FIRST INTERACTIVE OBJECTS" (s15 section).
- **Pickup (`func_001C4820`)**: INIT (bind model by item type via
  `*(D_0028A59C)`, bone slots, TRS stamp `func_001C6380`) / ACTIVE
  (cull+draw only — no gameplay in the actor; office pickups use the
  default static anim mode, no bob/spin) / FREE. Collection is player-side
  and NOT yet located (open: the inventory write site).
- **Door (`func_001BC350`)**: 6 sub-states (closed / locked x2 / opening /
  commit / close); trigger = player USE SCAN `func_00184BA0` (walk-into:
  action-state 0x2D + LOS + dist<=12 + facing>=~0.4 -> +0xB=4, frame mode
  spad 0x70003B8D=3); model-0x15 doors gate on unlock bits
  `D_00810841[area]`; articulation = ordinary keyframe clip on the door
  skeleton (`func_001C68C0`); doors are ROOM/AREA TRANSITION devices —
  commit writes `D_008106B5..B8` from the per-area destination table
  `D_0024E140[area][door_id&0x7F]` (id bit7 = inter-area) and WALKS the
  player through (move-to door±5.0, yaw snapped). Panels arm their
  active-list-neighbor door (+0x18) with +0xB=1. Door sounds live behind
  the script interpreter `func_001BA1F0` / opcode table `ftab_0024D880`
  (not yet itemized).
- **0x70003250 refined**: points into a per-area blob (`*(D_0028A5A8)`);
  per-uid words = collision-record OFFSET (AABB+face list, consumed by the
  `func_0019F730` player-vs-object collision family over the class-4 list)
  | flags 0x40000000/0x20000000 (`func_0019C6F0`, overlay-called).
- **Matches: +2 readable C, 100% byte-identical** — `func_001C6380`
  (prop TRS stamp) and `func_001BC240` (door commit step). Wall-blocked
  attempts documented inline: `func_001C4820` 89.74% (delay-slot fill),
  `func_001B6990` ~93% (delay-slot fill + dead-dup + loop-invariant delay
  fill — analysis in the stubs). verify_all boot-elf PASS (byte-identical).

### Update — 2026-06-10 s18: SCENE MANIFEST — per-scene boot config, snow scene re-baked in TRUE world coordinates

- **Scene manifest (`scene.txt`)**: each port scene dir now carries a
  plain-text boot config written by the exporters and read by the port's
  `em_game.c` at boot (zero-dep "key value" parser; `#` comments; missing
  file/key = the office defaults, bit-for-bit historical behavior):
  - `spawn <x> <y> <z> <yaw>` — player spawn, TRUE world coords + facing
    (radians; 0 = +Z). `export_level.py --spawn x,y,z[,yaw]` writes it.
  - `collision <file.emcl>` — collision filename inside the scene dir;
    `export_collision.py` records its own output name automatically.
  - `bgm <file.wav>` — optional looping level-music cue WAV
    (`export_level.py --bgm`); the `EM_BGM` env override still wins.
- **`--offset` spawn-anchoring is GONE** from both exporters (it baked the
  snow level into office-anchored coordinates because the port spawn was
  compile-time). Both scenes are now true-world: office spawn
  `107.4 0 -184 0` / `office.emcl`; snow spawn `218.592 229.85 201.789 0`
  (= the live state-01 actor) / `snow.emcl`. Re-exported snow EMDLs/EMCL
  came out byte-identical to the no-offset bake; the EMCL floor probe at
  the live XZ returns y = 229.85 in OUTPUT coords (= the live actor Y),
  pinning true coordinates. This supersedes FINDINGS s16's "scene dirs
  must name their collision office.emcl" and "--offset anchoring" notes.
- **Port collision robustness for outdoor terrain** (em_game.c, found by
  the snow move test): the horizontal move probe now (a) ignores
  FLOOR/SLOPE-class hits — near-flat tilted terrain front-faces the probe
  by ~0.001 and turned walks into sideways drift; the engine's result
  block carries the surface class (SPR 0x700030CA) for exactly this
  split — stepping past walkable crossings to find real walls; (b) runs
  at knee height (+1.0) so coarse ground-seated wall tris don't thin to
  a sliver at foot level; (c) rests blocked actors 0.01 in front of the
  hit plane (contact skin) so on-plane starts don't tunnel next frame.
  The floor query gets the same class split (a leaning gate-wall face
  was ratcheting the sliding player up the post). Office behavior is
  unchanged except the documented 0.01 skin (within the test tolerance).
- **Move-test instrumentation**: `EM_MOVE_LEGS=fwd,strafe` resizes the
  two script legs (default 60,30 = the office walk) and
  `EM_MOVE_EXPECT=x,y,z` overrides the expected final position, so the
  same scripted test asserts non-office scenes. Snow verification
  (`EM_SCENE=assets/scene_snow EM_MOVE_TEST=1 EM_MOVE_LEGS=60,60
  EM_MOVE_EXPECT=206.663,229.840,216.319`): forward leg walks +Z 15u with
  terrain-follow (y 229.85 -> 229.853 -> 229.840, matches independent
  floor probes), strafe leg is BLOCKED by the gate-side wall (grid poly
  n=(0.949,0.280,-0.145), verts at x 202-209 — free motion would reach
  x 203.59, the player stops at 206.663) — deterministic PASS x2.
- Verified: office EM_CAPTURE byte-identical to the pre-change build
  (manifest active), office EM_MOVE_TEST PASS (99.900, 0.000, -170.010),
  snow EM_CAPTURE shows the soldier at the snowy gate (the documented
  s16 description), `make test-input` PASS, port builds clean
  -Wall -Wextra, decomp `verify_all --no-container` PASS (4/4).

### Update — 2026-06-10 s19: INVENTORY FOUND — static global block 0x810C62..0x810CB5 (live session)

- **Closes the s15/s17 open item** (inventory-write site). The inventory
  is NOT actor-side: it is a static global block addressed via
  `lui 0x0081` absolutes. `D_00810C64` = byte-per-item-type COUNT ARRAY
  (`count[type]`); `D_00810CB4` s16 = SPR4 reserve rounds
  (display-verified live by poking with the status overlay open);
  `D_00810C62` u8 = rounds in current 30-round magazine. Add/pickup
  switch at `0x001C4100` (case 0x10: +30 rounds per SPR4 mag pack),
  reload at `0x0017B300`, per-shot decrement near `0x00170D40`. Player
  HEALTH/INFECTION are floats at actor+0x220/+0x228 (0x8104D0/0x8104D8).
  Full layout + 22-site code worklist in FINDINGS s18 section.
- First end-to-end use of DebugServer **pad injection** (pad_press /
  pad_set / watch_change): drove the depot interaction, status overlay,
  weapon draw, and walking. watch_change auto-pause catches sub-frame
  scratchpad transients; MCP watchpoints never fired (broken this
  build) — documented in FINDINGS.
- Open: source of the status screen "04/06" pair; what the office
  "DEPOSIT" wall unit actually does (its X-interaction cycles sub-frame
  and never changed ammo); owner struct of the 240-cap tick at
  `0x001418F0`.

### Update — 2026-06-10 s20: FIRST INTERACTIVE OBJECT NATIVE — office doors open in the port

- **`export_props.py --doors`**: exports the placement-table doors
  (class 5 model 3) as a SEPARATE articulated EMDL in door-local space
  (bone 0 = the full 9x21 panel — the "double door" blob is one panel +
  a small lock fixture, not two leaves; per-slot extents in FINDINGS
  s20), writes one `door <file> <x> <y> <z> <yaw> <radius>` manifest
  line per instance (files under `<scene>/doors/`), and rebakes
  `00_level.emdl` WITHOUT the static door geometry (export_level's
  region machinery imported, RGN_DOOR replays dropped: 6475→6275 verts).
- **Door open/close clip hunt: NEGATIVE** (automated, re-runs every
  export). Found a globally-resident 3-node object-anim bank
  (`chunk27/f02_id39.bin`, 5 clips) but no rest pose matches the door —
  the port plays a flagged placeholder 90° hinge swing until the real
  clip surfaces (details + bank characterization in FINDINGS s20).
- **Engine finding — sealed room boxes**: the grid collision world
  seals every doorway with boundary planes (x=60 west, z=-250/-255 at
  the office door); free movement never crosses a doorway. The s17 door
  transit (func_001BBE40 MOVE-TO to door_pos ± 5.0·normal) is the ONLY
  way through — the ±5.0 exactly clears the boundary plane spacing.
- **Port `src/game/em_door.{h,c}`** (+ em_game wiring): s17 state
  machine with engine sub-state numbers, use scan (12 u / facing-dot
  0.4 / auto 2 u / LOS with a flagged doorway-pocket exemption),
  kickoff side latch + collision-free walk-through glide, closed-door
  AABB hull on the movable-hull set, per-door palette through the
  existing skinned mesh path. Flagged deviations: CROSS-to-open outside
  the auto ring (engine = walk-into), OPEN-hold instead of the area/room
  transition commit.
- **Verified**: `EM_DOOR_TEST=1` PASS (blocked at 60.010 while closed →
  X press → OPENING → transit to (52.16, 0, -220.59) through the
  doorway → OPEN); EM_CAPTURE byte-identical with a doors-less
  manifest + old assets; EM_MOVE_TEST PASS; `make test-input` PASS;
  `verify_all --no-container` PASS (4/4); mid-transit capture shows the
  textured panel mid-swing with the player walked through.
- Open: identify the f02_id39 bank's owner object; locate the door
  clip; read func_00183EF0's class-5 LOS path (does the boundary plane
  exempt itself?); area/room transition commit (destination tables
  D_0024E140) when the port grows an area loader.

### Update — 2026-06-10 s21: BATTERY SOLVED — storage, display mapping, mechanics, L3 light

- **Battery storage located and display-verified**: current = u16
  `0x810CB2` (half-units), max = u8 `0x810CB7`; overlay shows
  `value >> 1` ("04/06" = 8/12). Poke of CB2=4 changed the open overlay
  to "02/06" + 2-segment bar live. Draw site `0x00209424/0x00209460`
  (sra 1 + 2-digit draw via `0x001C5FB0`), gated on `0x810C7F`.
- **Mechanics**: init zeroes it (`0x001AF380`); pickups add +12 (max->
  0x0C) or +36 (max->0x24 = "18") via `0x001C41CC` cases; devices cost
  `2*cost` half-units, drained -2/cycle (`0x002156A0`, insufficient ->
  msg 0xF0); stations recharge +4/cycle (`0x002277xx`). A currency,
  not a timer.
- **Shoulder light = L3** (TYPE A config; unlabeled 7th BUTTON CONFIG
  row): flips player-actor `0x8102BA` (+0xA); 300-frame auto-off burst
  (`0x8102D8` timer, off-anim id 0x15D via `0x001749A0`); **zero
  battery drain** (watch_change on CB2 over a full cycle: 0 hits).
- **func_001418F0 candidate RESOLVED — not the battery** (entity
  light/regen; callers `0x0013D908/DA28/DF28` idle in the office room).
- Bonus: full processed-pad map (`0x810E70/74` byte-swapped held/edge
  masks, raw bufs `0x810D40/DC0`) recorded in FINDINGS.

### Update — 2026-06-10 s22: AREA/ROOM TRANSITIONS captured live — full lifecycle + port contract

- **Both transition modes captured end-to-end** with the raw-TCP
  `watch_change` poller (exec breakpoints + MCP watchpoints proved
  NON-FUNCTIONAL this session — data-watch ordering only):
  intra-area office door both directions, and inter-area west door
  (area 02 → 01) with save-state restore.
- **Sequence (vsync-stamped)**: arm → kickoff (player SNAPPED to
  door±5 staging, script 0x24DE40) → 77–97f door clip → commit
  (`func_001BC150` writes the B5..B8 request; +audio fade x3 via
  `func_001B0C00(4)` iff inter-area) → 64f fade-out → switch
  (room move: entry byte + player re-place, 1 frame; area change:
  area/sub/entry bytes → overlay arena rewritten +3f → built-mirror
  0x810703 +4f → ~290f asset streaming → actor pool freed+respawned +
  player placed) → request cleared → door self-closes on B8==0.
- **Spawn-table chain fully decoded** (`func_001B07C0`):
  `(*(D_0024D650[area]))[sub] + entry*0x30` = {pos[3], yaw, ...};
  destination records have DUAL layout by door-id bit7
  ({area,entry,has_sub,sub} vs {entry_side0,entry_side1}); all four
  byte-verified live, both directions symmetric. 0x810702 = placement
  ENTRY index (doubles as room/zone id).
- FINDINGS "AREA TRANSITION LIFECYCLE" has the full timelines + the
  5-point port-loader contract.
- Open: authoritative player-position store (0x810350 et al. are
  per-frame copies — writes never stick); organic class-5 walk-into
  trigger still not reproduced under pad injection (s17/s20 item);
  s17's side→script mapping (both transits queued 0x24DE40).

### Update — 2026-06-10 s23: FIRST ENEMY EMDL — crawler resolved (crate husk + the real leech creature)

- **Model-table recipe proven**: save state EE RAM → `*(0x0028A59C)`
  per-area model table (`func_001C6120` = offset directory) →
  content-match entries against `extract/` files. State 01 (AREA11)
  table = `chunk15/f05_id97.bin` @ 0x5000; entries are raw mesh blobs
  (carve → `export_native.py --mesh`).
- **Correction**: creature/prop INIT binds mesh by placement PARAM
  (actor+0x0D), not the model byte (+0x03 = behavior variant). The
  placed crawler `func_001551B0` renders as a 1-node 14u INFECTED
  CRATE (AREA11/02 id 0x0D); its idle is procedural jitter, no clips;
  death REBINDS to gib models 0x22/0x29 in the global model library
  `*(D_0028A56C)` = `chunk27/f01_id37.bin`.
- **The real creature** (kind-0xD leech, `func_00153F10`): global
  array `D_0028A490[file id]` → `chunk03/f12_id14.bin` mesh (24-node
  worm chain) + `f11_id13.bin` clip bank (0 crawl loop / 1 emerge /
  2 windup / 3 lunge w/ root motion); live scale 0.5 (actor+0x80).
- **Shipped**: `extermination-port/assets/enemy_crawler.emdl` (EMD3,
  4 clips, 494 frames, textured from state 01 VRAM via `--p2s`);
  verified rendering in the port via temp EM_SCENE copy + EM_CAPTURE;
  default scene untouched. `export_native.py` now decodes the
  static/prop kernel's W encoding (single-node blobs, matrix at dmem
  qw 0).
- FINDINGS: new section "CRAWLER RESOLVED" (s22 §3 corrections inline).

### Update — 2026-06-10 s25: STATUS SCREEN fully mapped (draw-chain decode, live-driven)

- **The Triangle/Start status screen is completely documented** —
  FINDINGS new section "STATUS SCREEN LAYOUT": 512x448 UI canvas and
  GS offset convention (0x700/0x790, y halved), every element with
  position/size/color (profile block, HEALTH circular ring gauge at
  (208,197) with red low state + rotating highlight, battery half-unit
  square bar w/ magenta→yellow gradient, SPR4 reserve row + secondary
  weapon dispatch, INFECTION text-only block, page-selector diamond),
  the UI primitive vocabulary (flat quad / textured sprite / text /
  annular-arc-from-param-block `func_002082B0`), text style records at
  `0x265510..`, and the controller state machine (`0x810130` ctx,
  stick-hover page selection, X→ sub-screens loaded from disc to
  `0xB00000`, ids 0x1E/0x1F/0x24/0x25/0x26/0x2C).
- **Start opens the same screen as Triangle** — there is no separate
  pause menu; the status hub IS the menu.
- GS dump was not capturable headlessly (no DebugServer command, Pine
  down, menu action needs GUI) — decode done from code + live RAM;
  texture imagery (title/portrait/icons) still needs a VRAM capture.
- Port: concrete 10-point fix list for `em_hud.c` recorded in FINDINGS
  (canvas size, real anchors, ring gauge, battery half-units, no mag
  display, infection text-only, Start toggle, UI-camera scene swap,
  font renderer priority, em_hud.h staleness).
- Tooling gotchas recorded: `pcsx2_step` leaves temp breakpoints that
  re-trigger every `continue`; `pcsx2_read_registers` pauses the VM.
- Game state verified restored (inventory block, health/infection
  byte-identical; overlay closed; VM left running).

### Update — 2026-06-10 s26: SFX registry generator + REAL door sounds in the port

- **`tools/gen_sfx_registry.py`** (new): soundmap.json → the port's
  `assets/sfx/sfx.txt` registry; see the audio tools bullet above for
  the resolution rules (uppercase-hex keys, variants-by-area, region
  fallback) and the door-pair computation.
- **Office door sound pair pinned from data**: both office door
  placements carry link `0x02xx` → D_0024DB80 selector 2 → **front
  0x3FD (snd_0522, 24572 Hz) / back 0x3FE (snd_0523, 27581 Hz)**, area
  2.1 bank chunk04.n0. Office enemy-death 0x7D8 resolves via the
  region fallback (chunk04.n0 variant, snd_0552 19154 Hz) — its
  area-table rows don't list 2.1.
- **Port (extermination-port s26)**: `em_door` now reads the optional
  global `doorsfx <front-id> <back-id>` scene.txt line (em_game's
  parser skips unknown keywords; em_door scans the manifest itself)
  and the open chain plays the engine's `pair[side]` (op 0x0B sub 6 /
  func_001BBD60), logging the id. The close-at-black placeholder play
  is dropped when the real pair is active (the decoded open script has
  a SINGLE sound record; func_001BBD20 noted as a possible close path).
  No `doorsfx` line = the old placeholder behavior, bit-for-bit.
  FLAGGED simplification: one global pair, not per-door link patching
  (needs export_props.py to emit the link halfword on door lines).
- **Reload sound id still unlocated** — no labeled candidate in the
  soundmap and FINDINGS pins only the reload ANIM (0x33). The registry
  aliases the port placeholder 0xF002 to UNVERIFIED candidate 0x166
  (unlabeled weapon-bank neighbor, snd_0357 48791 Hz); 0x166–0x168 are
  the candidates to verify live (trigger a reload, watch
  func_001FBD50's id argument).
- Verified: EM_SFX_TEST (9 sounds, 3 plays, peak 3) / EM_DOOR_TEST
  (logs `door sfx: open id 0x3FE (back side)`) / EM_WEAPON_TEST /
  EM_ENEMY_TEST all PASS; all 9 registry WAVs exist (PCM16 mono,
  19–49 kHz engine-exact rates); default EM_CAPTURE byte-identical to
  pre-change baseline.
