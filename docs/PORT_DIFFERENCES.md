# PORT_DIFFERENCES.md — port-vs-engine divergence inventory (master review list)

**Audit date:** 2026-06-11.
**Scope:** every game module of `extermination-port` (`src/game/*.c/h`, `src/em_input.*`,
`src/em_model.*`, `src/em_math.h`, `src/em_gfx.h`, `src/main.c`, `src/game/em_frame.*`,
`src/game/em_task.*`, and the gameplay-relevant behavior of the gfx/audio backends)
audited against the decoded engine truth (this repo's `docs/FINDINGS.md`; the `.s`
disassembly is the arbiter where FINDINGS is silent).
**Method:** (1) full sweep of the port's own flag markers (TODO / flagged / stand-in /
PLACEHOLDER / port-invented / ASSUMED / deviation / unverified — 308 marker lines
inventoried); (2) top-to-bottom re-read of every module against its FINDINGS contract,
hunting unflagged divergences; (3) census of engine systems with no port counterpart at
all (FINDINGS + SUBSYSTEMS.md cluster inventory).

This is a REVIEW ARTIFACT, not a bug list: many entries are deliberate, documented
port decisions. The point is that every known difference is on one list.

## Legend

**Severity**
- **B (behavioral)** — gameplay outcomes can differ from the original.
- **V (visual)** — looks/sounds different; gameplay timing/outcomes identical.
- **S (structural)** — different mechanism/data path; intended result matches, but the
  translation is not the engine's machinery (matters for long-term fidelity/matching).

**Status**
- **FS — flagged stand-in**: explicitly flagged in the port source as a stand-in/placeholder.
- **PC — port-invented constant/value**: a number the engine does not supply (tuned or guessed).
- **UT — untranslated path**: the engine mechanism is known (or located) but not implemented.
- **SI — simplification**: engine mechanism implemented in reduced form.
- **SU — suspected / unverified**: a divergence (or equivalence) not yet proven against the
  engine; needs a live capture or a deeper .s read.
- **MISSING** — entire engine system with no port counterpart (section Q).

---

## Executive summary

**Counts** (sections A–M divergence rows; "(match)" rows and the P cross-references
excluded; rows with combined classes counted once by their primary class. Section Q
lists whole missing systems):

| Severity | count |   | Status | count |
|----------|------:|---|--------|------:|
| B behavioral | 50 |  | SI simplification | 51 |
| V visual     | 41 |  | UT untranslated path | 30 |
| S structural | 40 |  | FS flagged stand-in | 28 |
|              |    |  | PC port-invented constant | 13 |
|              |    |  | SU suspected/unverified | 9 |
| **total entries** | **131** | | whole missing systems (Q) | 24 |

**Top 10 worst divergences** (impact-ranked; section.entry cites):

1. ~~**No player damage/death pipeline**~~ — CLOSED 2026-06-11 s58: decoded processor
   func_0021C440 translated (flinch/death/infection/i-frames/kill plane + corpse hold +
   fade); the game-over SCREEN is a flagged stand-in (engine module undecoded). The s33
   "event 3" pad write turned out to be INFECTION (+0x22C), not health — the old consume
   was wrong twice over (C12, C14, P3, Q5 all closed).
2. **No pickups / no inventory** — the engine's most common interactive object
   (func_001C4820 props, class-4 kind-0xB items, ammo boxes) does not exist in the port;
   scenes ship without them (Q1).
3. **Character lighting is a fixed directional stand-in** — the engine's per-room light
   rigs `D_00251C50` + always-on camera light + ≤32 dynamic point lights
   (func_001D89D0/func_001D7FA0, FINDINGS s51) are untranslated; every character in every
   room is lit by one hardcoded shader light (F1).
4. ~~**Projection mismatch**~~ — CLOSED s59: the engine s = 480 zoom model is adopted
   exactly (hfov 67.38° / vfov 50.03° at 4:3, near 0.1 / far 16711680 decoded bit-exact,
   4:3 letterbox presentation); verified to <0.001 px against the state01 engine K (D10).
5. ~~**Hinged doors require the CROSS button**~~ **RESOLVED s58 — the premise was a
   misread**: the engine triggers ALL doors on a CROSS press edge (the use scan only runs
   on `D_00810E74 & spad-3B76` = config "use" 0x0040; FINDINGS "DOOR TRIGGER IS THE CROSS
   PRESS EDGE"). The port's hinged CROSS gate was engine-true all along; the s56 slider
   walk-into arming was the actual deviation and is now CROSS too (K2).
6. **Crawler aggro is port-invented** — the 32-u distance wake was added so a lone crawler
   engages at all (engine IDLE wakes only via group alarm or damage), and the damage
   window is widened to IDLE+ATTACK (J1, J2).
7. **Bullet auto-aim is a different shape** — distance + 10° facing cone stands in for the
   engine's screen-space cone func_00199220 (|x| ≤ 66+50s, |y| ≤ 45+45s on the GS canvas);
   target selection can differ per shot (H3).
8. **Camera wall response is a port-invented model** — the observed "rise over the wall"
   behavior was re-created with port constants; the engine's 6984-byte solver
   func_0018DD20 is unread (D3).
9. **Audio model is flat** — no per-sound pitch (tone center notes undecoded), no volume
   parameter (engine vol 150/300 ignored), no positional attenuation, no voice stealing,
   first-event-only triggers (M1–M4).
10. **Locked doors and per-door sounds are missing** — no unlock bitmask `D_00810841`, no
    locked sequences (door subs 1/2, slider locked script), one global doorsfx pair
    instead of the per-door link-indexed `D_0024DB80` pair (K6, K8, K10).

---

## A. Frame loop, tasks, entry point (`main.c`, `em_frame.*`, `em_task.*`)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| A1 | Output frame | 512x448 NTSC GS frame, 4:3 CRT | 960x720 window, title "Extermination (native port)" (`main.c`) | S | PC |
| A2 | Frame clear color | GS frame cleared per render-env setup (scene-owned; effectively black) | hardcoded clear (0.08, 0.09, 0.12, 1.0) every frame (`em_frame.c` step B) — visible wherever the scene leaves background pixels | V | PC (unflagged) |
| A3 | Frame pacing | vblank ISR func_001AB140 sets flag 0x810E98; main polls (step P) | `nanosleep` absolute-deadline pacing to 60/1.001 Hz after the present; `EM_UNCAPPED=1` disables (`em_frame.c`) | S | SI |
| A4 | Steps A/G/H/J/K/L/M/N/O/Q..V | PS2 hardware services (DMA watchdog, GS env, dispenv flip, IOP pumps…) | documented no-ops / folded into B, C, F, P (mapping table in `em_frame.c`) | S | SI (by design) |
| A5 | Fade machine tick count | step D func_001AEBE0 AND func_001AEE70 at G and O (second fade tick) | one tick per frame (step D); G/O are no-op hooks. The captured 64-frame door ramp matches, but whether func_001AEE70 ever advances the level a second time is unproven | B | SU |
| A6 | Fade modes | mode 0 subtractive (translated); mode 1 additive fade-to-WHITE (GS ALPHA 0x68) | mode 1 not implemented — no port caller yet (`em_gfx.h`, `em_frame.h`) | V | UT |
| A7 | Pad input block | second button triple at +0x16..+0x1A (pad 2) | single-pad triple only (`em_frame.h`) | S | UT |
| A8 | Task table | 3 x 0x20-byte records at 0x28A750; engine uses multiple slots over its lifetime (boot/flow/game) | faithful 3-slot table (`em_task.*`), but natively only slot 0 is ever populated; whatever the engine runs in slots 1/2 is unmapped | S | SU |
| A9 | Fade representation | level byte 0..255, +0xC0 sub-state byte (3 fade-out / 2 hold-black / 1 fade-in) other systems gate on | float 0..1 + dir int; the sub-state is implied (em_door re-derives the gates) | S | SI |

## B. Input (`em_input.*`)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| B1 | Input device | DualShock 2 analog sticks (continuous 0x00..0xFF), pressure-sensitive buttons | keyboard map (the user's PCSX2 binding); digital keys synthesize stick bytes | S | SI (hardware reality) |
| B2 | Gait selection | continuous stick magnitude through rings 48/88/122 (func_001B5CC0) | exactly three deflections — 1.0 (run) / 0.8 Cmd (jog) / 0.5 Option (walk); the modifier keys are port inventions to reach the lower rings | B | PC (documented) |
| B3 | Diagonal deflection | analog stick is naturally magnitude-bounded | full-push diagonals overshoot the run ring by sqrt(2) (deliberately mirrors PCSX2 keyboard behavior); under a modifier hold the vector IS normalized | B | SI (documented) |
| B4 | Button polarity | hardware reports active-low | active-high `EmPadState.buttons`; a future decomp shim inverts (`em_input.h`) | S | SI |
| B5 | Button pressure values | DS2 8-bit analog per button | not modeled (`em_input.h` FUTURE WORK) | B | UT |
| B6 | Button config | engine reads actions through the config-mask block spad 0x70003B70..7E (re-mappable; an options screen exists) | port hardcodes the decoded DEFAULT config; no remapping | B | UT |
| B7 | Gamepad backends / rumble | libpad actuators available (engine use unverified) | none (keyboard only) | B | UT |

## C. Player movement, locomotion, footsteps, status (`em_game.c`)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| C1 | Gait/tier ramp | func_001B5CC0 rings, func_00174AC0 target speeds D_00248870 {0,0.1,0.3,0.8}, ramp func_0017BC40, accel/decel D_00248880/90 | translated with the ELF-dumped table values; sustained tiers/speeds match | — | (match; listed for review completeness) |
| C2 | Run-down decay | phase-2 decay 0.03125 u/tick; **x2 with carried gear**; mode-6 stop-skid anims (ids 4/5) | decay translated; carried-gear x2 and the stop-skid anims untranslated (`em_game.c` ANALOG GAIT block) | B/V | UT (flagged) |
| C3 | Facing turn rate | engine turn-toward-heading rate undecoded (lives in the locomotion tops) | `TURN_SPEED` 12 rad/s shortest-arc seek | B | PC (unflagged) |
| C4 | Move heading | engine locomotion tops drive heading/velocity through their own state machine (func_001612D0 family) | camera-relative stick direction + smooth turn — a re-model, not a translation | S | SI |
| C5 | Wall collision | five radial probes at yaw + D_00248950, radius 4.5, ankle 0.05 / chest 4.01, push-back delta (func_001764E0) | translated faithfully, including per-probe re-read; **but** the walkable-crossing step-past loop (8 iters, 1e-3 nudge) replaces the engine's surface-angle band filter | S | SI |
| C6 | Floor resolve | engine vertical hub queries (e.g. y+200..y-200 window) | ±8-unit probe window (`FLOOR_PROBE_UP/DOWN`) | B | PC |
| C7 | No-collision fallback | n/a (world always present) | room-bbox clamp + flat floor when no EMCL — port-only runnable-standalone mode | S | FS |
| C8 | Door-transit walk speed | engine SNAPs the player to the staging point in one frame (func_00182F90) | scripted MOVE-TO walk at `WALK_SPEED` 15 u/s | B | PC (flagged) |
| C9 | Idle cycle | id 0 breathing + 300-frame timer + fidget 0x15D (decoded s46) | translated; cross-fades via linear palette lerp (engine: bone-channel blend args 12.0/8.0) | V | SI (flagged) |
| C10 | Locomotion clip rates | engine: per-tier property rates (tier 2 hard-codes +0x204 = 0.75); mid-ramp blend TOWARD next tier's clip (anim_matrix_player sub 1) | continuous rate = move_speed / clip natural speed (stride lock); tier-swap crossfade approximates the mid-ramp blend | V | FS |
| C11 | Footstep mapping | func_00182430 compiled-in material blocks + tier sub-base + rand5 (decoded s37) | translated; **rand5 uses a private LCG** (engine: EE libc rand()); decal/FX layer func_00187EE0, movable-object attr override (crate = attr 2/4), first-contact one-shots 0x5A/5B/5C, deep-water block 0xCB (+0x23C state) all untranslated | B/V | SI + UT (flagged) |
| C12 | Player damage | per-frame processor func_0021C440: pending floats +0x224 (health) / +0x22C (infection), type byte D_008102BF, state-2 flinch/death subs, producer-side i-frames (event byte + 0x20E window), corpse hold + fade (FINDINGS "PLAYER DAMAGE & DEATH PIPELINE") | **CLOSED 2026-06-11 s58**: generic tail translated — flinch (decoded clips 0x1E-0x21/0x56-0x57/0x1C7 + voices 0x152/0x153), death sequence (clips 0x2A/0x5C/0x1C4, the 5-sound cue chain, 120-frame hold, 4-speed fade), infection-at-100 latch + 60-cap + drain, kill plane, i-frames. Remaining: typed paths 1..0xB (no native producers), latch ticks, blood-pool/gore effects (no effect system), heartbeat rumble (no FF backend), hazard-room drain (no room attrs) — all flagged | B | SI (typed paths UT) |
| C13 | Player status values | live save: health 75/100, infection 60, mag 4/30, reserve 120, battery 4/6 (FINDINGS "INVENTORY LOCATED") | static demo values seeded at install; only mag/reserve are live (mirrored from em_weapon) | B | FS |
| C14 | Health display max swap | engine flag 0x8104E4 (= player +0x234, the INFECTED latch) swaps display max to 60 | **CLOSED 2026-06-11 s58**: infection-at-100 sets the latch and `health_max` 60 (em_game player_apply_infection) | V | (match) |
| C15 | Dead constant | — | `STICK_DEADZONE` 0.25 defined, never used (superseded by gait rings) | — | (cleanup note) |
| C16 | Scene boot | engine area flow: spawn tables in boot ELF .data, difficulty map, HUD/weapon context init (func_001AE040 state 0) | scene manifest `scene.txt` (spawn/collision/bgm/doors/enemies/camregions) — a port-side data path replacing the engine loader | S | SI (by design) |

## D. Camera (`em_game.c` camera section)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| D1 | Chase primitives | func_0018C6A0 (/6, cap, 0.25 snap), func_0018C4B0 (/8) | translated exactly | — | (match) |
| D2 | Camera modes | modes 0..15 (cut/smooth tables), top-mode 3 scope func_0022EEF0, init mode 8 settle | only mode 0 (follow) + mode 1 (aim) exist; init snaps actual=desired instead of mode-8 settle; modes 2..15 + scope TODO | B | UT (flagged TODO) |
| D3 | Blocked-eye response | unread solver func_0018DD20 (FINDINGS confidence "medium"); observed behavior: camera RISES | port re-creation: rise search `CAM_RISE_STEP` 2.0 / `CAM_RISE_MAX` 40 + pull-in fallback with `CAM_WALL_MARGIN` 0.5 — all port constants | B | PC + SU (flagged) |
| D4 | R1/L1 orient | engine: camera-mode swaps; orient-to-heading family rate 2°/frame (func_001921D0) — which player state L1 routes through is unpinned | L1 one-shot seek at the family rate; R1 owned by the aim camera | B | SI (flagged) |
| D5 | Idle auto-orient | decoded: 481-frame timer, 3° deadband, 0.2°/frame orbit, wall bits 0xD/0xB cancel (func_001921D0/func_00193D90) | translated; the solver wall bits are mapped onto a port rotation-path segment test | S | SI (flagged) |
| D6 | Aim camera mode 1 | decoded (func_00197D20/740/870); flagged areas use eye floor +11 | translated; port always uses the +2 floor (flag-area param not carried) | B | SI (flagged) |
| D7 | Aim release | engine swaps to transition mode 2 (func_00198650, untranslated) | chase yaw re-seeded from eye→player heading, mode-0 blends back | B | FS |
| D8 | Fixed-camera regions | director regions: snow case approaches spec at 0.7 u/frame via chase primitives; spawn-record cameras hard-place (func_001B0460) | port hard-places for BOTH kinds (correct for spawn-record, divergent for director regions) | V | SI (documented) |
| D9 | Door camera cues | op 0x0D sub 5 cut + cam+0xA0 target re-blend + locked-look func_001BBBF0 (decoded + live-verified) | translated; locked-look reachable only via `EM_DOORCAM_LOCKED` (its real trigger, the locked sequence, is absent — K6) | B | UT |
| D10 | Projection | s = 480 zoom model: x = 0.8s·x/z + 2048, y = 0.5s·y/z + 2048, GS-Z row (FINDINGS "CAMERA SYSTEM" §3 + s59 exact derivation) | ~~50° fovy at WINDOW aspect, near 0.5, far 800/500~~ — CLOSED s59: `em_mat4_perspective_gs` (tan h/v = 320/s, 224/s; near 0.1 / far 16711680 decoded bit-exact), 4:3 letterbox, zoom wired (scope-ready); EM_PROJ_TEST reproduces the engine K of state01 to <0.001 px | — | (match) |
| D11 | Camera latency | PS2 kicked chain consumes the PREVIOUS frame's matrices | native chain flushed same-frame → one frame LESS camera latency | V | SI (documented) |
| D12 | Top modes 1/2 (frozen) | commit-only frames | modeled via the status-screen pause gate reaching commit only | S | SI |

## E. Scripted anim mailbox + status-menu UI scene (`em_game.c`)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| E1 | Anim request/commit | +0x1F2 request / +0x20C commit, one-frame latency, clip-end flag +0x200 & 0x1000 | translated (sa_* fields); hold/hold_restart model the stance re-selection and the fire-counter re-seed | — | (match) |
| E2 | Menu turntable | engine renders the menu player under the RAW IDENTITY camera; room-rig lighting (mode-2 light path, office key 0x200 rec) | port orbits the rig to the shader stand-in light's azimuth so the camera-facing side is lit, and substitutes a camera-anchored spot FILL (rgb 0.62) for the room rig | V | FS |
| E3 | Menu tint | engine ADDITIVE per-actor color delta over the GS 128 base | approximated MULTIPLICATIVELY: rgb_mul = (128+delta)/128 | V | FS |
| E4 | UI projection | engine UI camera projection not directly read | tan(fovy/2) = 0.74 PINNED by the x-anchor consistency argument (7.4 u at z 40 → canvas column 208) — derived, not dumped | V | SU |
| E5 | Menu clip select | displayed-health copy (lags during count-up) picks 0x1C2 vs 0xA | port reads the status TARGET value instead of the lagging display copy | B | SI (documented) |

## F. Rendering contract + Metal backend (`em_gfx.h`, `em_gfx_metal.m`, `em_model.*`, `em_math.h`)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| F1 | Character lighting | per-room rigs D_00251C50 (ambient + 2 directionals per room) + always-on camera light (flag +0x2 bit 0x20) + ≤32 dynamic point lights (func_001D89D0 / func_001D7FA0 — FINDINGS s51) | ONE fixed directional L = (0.4, 0.8, 0.45) with 0.30 ambient floor, identical in every room | V | UT (the biggest visual gap) |
| F2 | Untextured fallback | n/a | flat grey (0.55, 0.62, 0.70) | V | PC |
| F3 | Alpha test / blending | GS per-draw ATE/AREF + blend configs per context | shader alpha-test at 0.5 ("CLUT alpha is mostly binary") + standard alpha blend, NO translucent draw sorting | V | SU |
| F4 | Glow/billboard pass | GS ALPHA Cv = Cs·FIX(0x80) + Cd, ZMSK=1 (decoded s17) | translated (additive, depth-write off, camera-plane corners) | — | (match) |
| F5 | Screen fade blend | subtractive GS sprite ALPHA_2 0xA1/FIX 0x80 (decoded s54) | translated (reverse-subtract ONE/ONE) — the old alpha approximation retired | — | (match) |
| F6 | Overlay flush order/budgets | engine GS packet order per screen | fixed order backdrop→untextured→sprites→glyphs→subtract; per-frame quad budgets (1024/64/16) are port caps; overflow DROPPED | V | PC |
| F7 | Texture pipeline | PSMT4/PSMT8 + CLUT in GS VRAM, TEX0 tokens | exporter pre-converts to RGBA8; REPEAT wrap via tiled pow-2 array slices | S | SI (by design) |
| F8 | Asset containers | DATA.DAT/INDEX.IDX, MWo3 blobs, id 0x44 world | EMDL/EMCL/EMTX/EMUI/EMFN/EMSG port containers, user-generated locally | S | SI (by design) |
| F9 | Looping clip interpolation | engine sampler behavior at the loop seam unverified | `em_model_palette_at` blends the last frame into frame 0 (`(f0+1) % frame_count`) | V | SU |
| F10 | Laser/beam pass | GS LINE prims (1 screen pixel) via func_001E2BA0 | axial-billboard quads, width 0.12 world units ≈ 1 px at typical aim depth; pass camera = this frame's LAST skinned draw | V | PC (flagged) |
| F11 | View-matrix handedness | GS y-down, +z-into-screen view (func_00102CD0, up = (0,-1,0)) | `em_mat4_lookat_gs` negates all three view rows (X negation = chirality choice validated against captures) | S | SI (documented) |
| F12 | D3D12 / Vulkan backends | n/a | stubs returning NULL (`em_gfx_d3d12.c`, `em_gfx_vk.c` — TODO); Windows/Linux platform + audio backends likewise | S | UT |
| F13 | Bone publish for equipment | gun tick reads player hand-bone matrix (+0x90) same-frame | `em_gfx_last_skinned_bone` — one frame of latency by construction (same staleness class as the engine's fire mailbox); menu-open frames record the MENU pose (documented corner case) | S | SI |

## G. Collision (`em_collision.*`)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| G1 | Polygon walkers | func_0019ED80 / func_001A4030 epsilons (±1e-5), facing rule, per-axis interval, convex ring, segment clamp; conditional attrs 0x50..0x59; surface classify 0.49029/3.0 | translated instruction-faithfully | — | (match) |
| G2 | Acceleration index | rank-table binary search func_0019F1A0 | linear scan of the poly list (results identical; pure perf) | S | SI |
| G3 | Movable-hull set (mask bit 0) | per-uid AABB records in the area state blob (func_001A6440) | not in em_collision; doors covered by `em_door_probe` segment-vs-AABB; all other movable objects (crates the player stands on, props) have NO collision | B | FS + UT |
| G4 | Query id | per-actor world slot (actor +0x02 & 0x1F) | move probes hardcode id 0 (player); enemies use their own queries without ids | S | SI |

## H. Weapon — SPR4 rifle (`em_weapon.*`)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| H1 | Ammo/reload | func_0017B300 matched 100% (TOTAL-pool rule, top-up fill quirk replicated verbatim) | translated | — | (match) |
| H2 | Fire sub-machine | func_00170A60 sub-states; semi interval = ladder clip length (+0x2F4), burst/auto 12.0; +0x2A queue window; dry-mag reload at expiry | translated (states collapsed to 5 port enums, cadence/timing faithful); `WPN_BURST_PAUSE` 8 is the FINDINGS contract value | S | SI |
| H3 | Target acquisition | screen-space cone func_00199220: \|x\| ≤ 66+50s, \|y\| ≤ 45+45s on the GS canvas, 3-target cycle | distance + cos 10° facing cone (`WPN_AIM_CONE`), nearest only | B | FS |
| H4 | Bullet victim test | the segment query itself reports the hit actor (*0x700031D4; movable hulls in set 0) | `em_enemy_ray_test` sphere-vs-segment beside the world query, nearest wins — documented design split | B | SI |
| H5 | Enemy hit-sphere radii | per-model collision hulls (id 0x73 rig hulls exist on disc) | spheres: crawler 3.0, crate 3.5 — port constants | B | PC (flagged in em_enemy) |
| H6 | Fire sounds | per-stance code: 0x164 (0x31/0x34) vs 0x165 (0x32/0x35) | only the 0x1D-family stance exists → always 0x164 | V | UT |
| H7 | Impact sounds | surface-keyed impact family suspected (0x188/0x18A/0x18B neighbors); unpinned | 0x189 for every wall | V | FS |
| H8 | Tracer / gore / impact markers / rumble | func_001860A0 tracer + FX dispatch | untranslated | V | UT |
| H9 | Muzzle anchoring | hand matrix x D_0024A220/D_0024A2A0 table points (decoded) | translated via node-4 palette read; chest-height/yaw fallback for clip-less EMDLs | — | (match; fallback FS) |
| H10 | Muzzle flash | chunk27 models 0xD/8/7 as real meshes, per-shot pool FX actors; rotation lerp tick ≥ 4 | model-per-tick schedule drawn as textured billboards; ONE flash slot (new shot restarts); rotation lerp translated as a roll around the gun axis (one of three Euler components) | V | SI (flagged) |
| H11 | Laser sight | 32 GS LINE segments + dot billboard; hide window +0x2F2 (all decoded) | translated; lock-on variant (warm color, 5.0 dot — D_008106E0 aim option 1) pending (no lock-on system) | B | UT |
| H12 | Flashlight render | THE ENGINE DRAWS NOTHING for the toggle (exhaustive sweep, FINDINGS s51) | deliberate deviation: forward spot term + visible cone mesh; spot values port-tuned (cone angle asset-derived 7.13°, gain x3 stands in for the 0x10/0x11/0x16 shell stack); level-only lighting with the camera-fill exception | V | FS (deliberate, documented at the API) |
| H13 | Shoulder-light burst (s28b L3 stealth light) | 300-frame auto-off burst, anim/sound 0x15D, zero battery | code kept but UNHOOKED (input path undecoded; L3 is the port's reload) | B | UT |
| H14 | Fire-mode select | D_00810C61 set by game UI (page-2 weapon customization, presumably) | only the test API `em_weapon_set_fire_mode` — no in-game input changes modes | B | UT |
| H15 | Sub-weapon attachments | D_00810CA6 attachment selects the SQUARE action (0 = flashlight; others exist) | attachment 0 hardcoded — SQUARE while armed is always the flashlight | B | UT |
| H16 | Aim options | D_00810CA4 remaps the muzzle table row (0→7, 2→6) and gates auto-aim/lock-on | fixed to manual-aim row 7 | B | SI |
| H17 | Casing/impact sound scheduling | s29 live latencies (+2 frames, +42 ticks) | translated; 8 casing slots is a port mechanism (saturation drops oldest overlap) | S | SI |
| H18 | Reload/draw anim holds | engine stance tops re-select the pose every frame (no idle interlude) | HOLD-type requests clamp the clip's last frame (the stagger fix) — same result, different mechanism | S | SI |
| H19 | RNG | engine func_00122BB8 = libc rand() | private LCG (deterministic per run) | S | SI (flagged) |

## I. Knife / melee (`em_weapon.*`)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| I1 | Dispatch / tables | two buttons (CIRCLE light combo / SQUARE heavy), anim ids, gates, damages, sounds — decoded s36 + corrected lengths | translated | — | (match) |
| I2 | Range | NO player-side range: damage goes to the melee-target link's mailbox; the TARGET polls its own proximity (func_00219870 → func_0019AA80) | victim resolved player-side: nearest enemy within reach 12.0 (documented hands-reach) inside a 60° cone (PORT constant) at the impact tick | B | FS + PC |
| I3 | Heavy yaw steer | func_00173DD0 steers yaw at D_002486F0 rates during the swing | player stays planted | B | UT (flagged) |
| I4 | Alternate melee row | +0x236 selects anim ids 0x1BD..0x1C1 (alternate context) | untranslated | V | UT |
| I5 | Hit markers / swing FX | +0x25E codes 0x81/0x82/0x83 feed func_00187350 → func_00182430 effects | untranslated | V | UT |
| I6 | Impact timing | down-count reading impact = len − T (self-consistent for all four attacks) | `max(3, len − T)`; a live capture remains the final word | B | SU |
| I7 | Recover blend | engine passes blend 4.0 to the arbiter | played at property rate 1.0 with the port's standard crossfade | V | SI |

## J. Enemies (`em_enemy.*`)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| J1 | Crawler wake | IDLE wakes ONLY via group alarm (+0x0A) or damage; vision is distance-only in this engine (leech ≤ 32 u) | additionally wakes when the player is within 32 u (else a lone placement never engages) | B | FS (deliberate) |
| J2 | Damage window | FINDINGS open item: state 1 (attack) never visibly polls +0x36 ("undamageable mid-lunge — verify live") | mailbox polled in IDLE and ATTACK | B | SU (flagged) |
| J3 | Lunge damage | contact-damage code family 0x4000\|amount (func_001A9480); crawler amount unpinned | writes 0x400A (amount 10) to a player-side mailbox | B | FS |
| J4 | Movement constants | hop speed/airtime, probe length/lift not exported | `ENEMY_HOP_SPEED` 0.32, `ENEMY_HOP_VY` 0.42, probes 6.0/1.0 — port values; turn rate 0.0524 and gravity 0.052 ARE engine values | B | PC (flagged) |
| J5 | Steering probes | func_0019AB20 directional probes over the actor's 4 precomputed diagonals | knee-height segment queries through em_collision | S | FS |
| J6 | Death | state 2: nest children, gore FX, MODEL REBIND to gib entries 0x22/0x29, RNG 90° knockback corpse-slide | gibs launched with engine rotation steps + PORT launch constants (speed/vy/spin/jitter flagged); nest children + gore untranslated; no-gib cases get a 30-frame alpha corpse fade (placeholder) | B/V | FS + PC + UT |
| J7 | Crate disguise | per-area model-table binding by param; jitter via D_002468B0 tables; alarmed crawler hops AS the crate | own spawn kind; jitter MECHANISM kept with PORT amplitudes/periods; ~10-u proximity burst trigger is a PORT stand-in (engine state-4 wake unpinned); group alarm ignored; alarmed-crate hop untranslated | B | FS + PC |
| J8 | Generator | fully decoded (tables, charge counts, 4-worm cap, delays, trap damage event 3 magnitude 5) | translated with the decoded values; mode draw + delay pick use the module LCG (engine: frame RNG at spad 0x70003B68); second box pass (waking D_00275BB0-list actors) untranslated; trap hit = one-shot 5-damage mailbox write (engine: event-3 knockdown) | B | SI + UT (flagged) |
| J9 | Generator visual | procedural VU-morph pad (func_001E9580/001E9E60, private 0xA060 buffer) | original placeholder mound scaled to the decoded footprint, phase as a Y swell | V | FS |
| J10 | Tendril field | fully decoded (s33: rings, ramps, girth table, bob integrator, thrash) | translated; room-tint table D_00246800 UNDECODED → neutral (128,128,128) rest blend (TODO); alpha = ramp/300 vs engine roomC.w/128 target; sounds 0x42D/0x42F unmapped in the registry; pool cap 8 is a port cap | V | FS + UT |
| J11 | RNG | engine frame RNG (spad 0x70003B68) / rand() | module LCG everywhere | S | SI (flagged) |
| J12 | Slot pool | global engine actor pool | 16 crawler/gib slots + 12 generators + 8 tendril fields — port pools; spawn failures emulate pool exhaustion | S | SI |
| J13 | Placeholder meshes | disc models | procedural box crawler / box crate / mound when assets absent | V | FS |

## K. Doors & area transitions (`em_door.*` + `em_game` scene switch)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| K1 | Use scan | func_00183EF0 class-5: dist ≤ 10 / \|dy\| ≤ 8 from the DOORWAY CENTER, side + π/4 facing (decoded s45) | translated | — | (match) |
| K2 | Trigger input | ALL doors trigger on the CROSS press edge — use scan func_00184BA0 runs only on `D_00810E74 & spad-3B76` (0x0040); NO walk-into (s58 — the old "state 0x2D walk-into" was the class-7 prefix) | CROSS press edge (in->pressed), hinged + sliders alike (s58) | — | (match) |
| K3 | Staging | engine snaps (func_00182F90 instant translate) | scripted MOVE-TO walk to the same point (see C8) | B | FS |
| K4 | Model byte | placement record carries the model byte | parsed from the `door_mXX.emdl` FILENAME (hinged/slider) | S | FS (flagged) |
| K5 | Transit modes | B8==2 room move vs B8==1 area change (audio fade + overlay/asset/actor-pool reload) | every transit is the room-move shape; goto doors approximate B8==1 with the scene switch, NO audio fade | B | SI (flagged) |
| K6 | Locked doors | subs 1/2 locked sequence, unlock bitmask D_00810841, locked VO + camera (m03 + slider locked scripts) | absent (no lock bitmask) | B | UT (flagged) |
| K7 | Door articulation | slot-0x39 bank clips (decoded s30/s32), pumped 1.0/frame | real clips shipped; clip-less EMDL falls back to a 90-frame placeholder hinge swing (90° — real clip length unknown for that case) | V | FS |
| K8 | Door sounds | per-door pair D_0024DB80[link>>8][side] patched into the open script; close is SILENT (s29 verdict) | ONE global `doorsfx` line per scene (manifest lacks the link halfword); without the line, legacy placeholder ids (close-at-black play preserved) | V | SI (flagged) |
| K9 | Sliders | brain func_001BB860 decoded: CROSS-edge use-arm (s58 — not walk-into), 6.0 staging, native slide 0.2 u/frame to 9.0, walk-through, no fade, no player anim; engine re-close via room re-entry state | translated (trigger = CROSS since s58); re-close by running the clip BACKWARDS when the player leaves the scan radius + 2-u hysteresis (motion-identical stand-in); goto-slider commit/fade interleave unread | B | SI + SU (flagged) |
| K10 | Slider locked script | D_0024DA40 (camera + VO, no motion) | absent | B | UT |
| K11 | Office VENT | overlay-scripted in-room mechanism (class-0x0B trigger record at (43, 3.5, -147)); not a placement object | flagged SYNTHETIC vent door line in the manifest whose goto lands on office0 spawn entry 5 | B | FS |
| K12 | Arrival walk-out | player state 5/1 decoded (50/30/30 phases, 0.3 u/tick, decay 0x3C3A2E8C); armed families request scripted walks 0x4D/0x4E | translated for family 0; armed-family walk clips untranslated. NOTE: `em_door.h` step-4 comment still labels id 2 "RUN" (pre-tier-ramp label; id 2 = JOG) — doc staleness only | B | SI |
| K13 | Two transit locks | movement lock (scripted mode + state 5) / menu lock (func_001AE7E0 fade gate) — decoded split | translated; func_001AE7E0's OTHER duties (end-of-level poll) not modeled | S | SI |
| K14 | Goto scene switch | engine area loader: overlay reload, actor-pool rebuild, asset streaming | `em_game_scene_switch` frees/reloads the manifest scene; player model, BGM, sfx registry persist | S | SI (by design) |
| K15 | Movable-hull collision | per-uid AABB records, mask bit 0 | `em_door_probe` segment-vs-AABB per door, joined only into the chest probe pass | S | FS |

## L. Status screen / HUD (`em_hud.*`)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| L1 | Background | identity UI camera 3D scene + animated tile layers (func_0020A7A0, decoded s44) | real backdrop + UI-scene player when assets present; full-screen DIM RECT fallback without them (flagged) | V | FS |
| L2 | Ring highlight | additive blend-mode-1 arcs (transparent↔(80,80,80)) | white at alpha 80/255 through the alpha-only overlay pass | V | FS |
| L3 | Ring gradient corners | 0x60 arc blocks carry 4 gradient slots; corner assignment unverified | radial inner→outer assumption | V | SU |
| L4 | Fonts | 1bpp glyphs streamed from EE RAM into a PSMT4 strip at GS 0x1B00, batched sprite | pre-baked RGBA8 sheet (`font.emfn`), per-glyph quads; placeholder rects at real glyph metrics without the asset | S/V | SI + FS |
| L5 | Decor | boot-resident sprites (decoded s26); engine blend mode 3 on icons | exported `ui.emui`; standard alpha blend approximates mode 3; nothing queues without the asset | V | SI |
| L6 | Page navigation | func_0020D930 hover + func_0020CDC0 remap (decoded s39) | skeleton translated (hover/enter/back/close, edge mask 0x830) | — | (match, skeleton) |
| L7 | Page interiors | item lists, map cursor, weapon customization, database records | NOT modeled: amber "CONTENT TBD" flag strips; ITEM page partial (battery + a PORT-LABEL "SPR4 MAGAZINE" derived count — engine per-type inventory D_00810C64 untranslated); missing page asset → flagged placeholder panel | B/V | UT (flagged) |
| L8 | Keypad pages 4/5 | chunks 0x25/0x26, entered via external request byte D_008106C5 | unreachable in the port | B | UT |
| L9 | Page texture anchors | title art asm-anchored (8,0); tile/legend anchors not statically known | flagged ASSUMED in the exporter | V | SU |
| L10 | Not composed | page-tab strips, spinning cyan double ring + sparkle emitter | absent | V | UT |
| L11 | Help line / message bank | selection rule decoded (s42); bank exported | translated when assets present; row layout of ITEM categories ASSUMED | V | SI |
| L12 | Battery block gate | engine gates on 0x810C7F != 0 | gated on battery_max != 0 | S | SI |
| L13 | Pulse re-seed RNG | engine rand() | fixed-seed LCG | S | SI (flagged) |
| L14 | Pause semantics | engine flag 0x8106C4 halts gameplay while the menu is up | `em_hud_is_open` gate in gameplay_frame (EM_HUD_FORCE deliberately excluded) | — | (match) |

## M. Audio (`em_sfx.*`, `em_bgm.*`, backends)

| # | Behavior | Engine truth | Port | Sev | Status |
|---|----------|--------------|------|-----|--------|
| M1 | Pitch | engine repitches per trigger note: 44100·2^(dnote/12); per-sound center notes in the tone records | each WAV plays at its stored rate — NO repitch (registry will grow a pitch column) | V | UT (flagged) |
| M2 | Volume / position | play_sound carries volume (150/300 observed) + positional attenuation | `em_sfx_play(id)` has NO volume or position parameter; everything full-gain (x0.6 PORT headroom) | V | UT |
| M3 | Voice management | 48-channel sequencer with voice STEALING | 8 voice slots; saturation DROPS the play (counted) | B | SI (flagged) |
| M4 | Trigger scripts | multi-event note-on scripts (e.g. casing 0x16A is 2-event) | first event only (single WAV per id) | V | SI (flagged) |
| M5 | Bank model | SShd banks, id→bank/script/tone chain (decoded s28/s29) | text registry `sfx.txt` id→WAV; unmapped id = silent no-op | S | SI (by design) |
| M6 | BGM cues | cue table D_0025DD30 (67 cues, loop flags), func_001FB0B0/func_001FAE70 fade-restart | path-named WAV (`EM_BGM` env or manifest `bgm` line) stands in for the cue id; ~1 s fade approximates the engine fade shape | S | FS |
| M7 | Inter-area audio fade | B8==1 transitions fade audio | not performed (tied to K5) | B | UT |
| M8 | Device model | IOP/SPU2: streamed BGM + sequenced SFX | one OS audio device owned by em_bgm; SFX summed in its callback | S | SI (by design) |
| M9 | Win/Linux audio | n/a | TODO stubs (`em_audio_win.c`, `em_audio_linux.c`) | S | UT |

## P. Cross-cutting unflagged/incidental findings

| # | Finding | Where |
|---|---------|-------|
| P1 | Frame clear color (0.08, 0.09, 0.12) is an unflagged port value (see A2) | `em_frame.c:277` |
| P2 | `TURN_SPEED` 12 rad/s facing seek has no engine citation and no flag (see C3) | `em_game.c:223` |
| P3 | ~~No death/game-over handling at 0 HP~~ — CLOSED s58 (see C12; the game-over SCREEN remains a flagged stand-in — the engine module + trigger are undecoded, FINDINGS s58 §6) | `em_game.c` PLAYER DAMAGE & DEATH |
| P4 | `STICK_DEADZONE` dead constant (see C15) | `em_game.c:404` |
| P5 | `em_door.h` step-4 walk-out comment labels anim id 2 "RUN" — stale vs the 2026-06-11 tier relabel (id 2 = JOG); behavior correct | `em_door.h:112` |
| P6 | ~~Camera far/near clip planes (0.5 / 500–800) are port values inside the flagged projection TODO~~ — CLOSED s59 (engine planes 0.1 / 16711680 derived bit-exact from the GS Z-row literals; see D10) | `em_game.c` camera_commit |
| P7 | Menu-open frames make the UI player the "last skinned palette" — em_weapon's muzzle anchor would read a menu pose next frame (documented as untestable corner) | `em_game.c` ui_scene_render |
| P8 | em_model header sanity bounds (bone ≤ 1024, vert ≤ 4M …) are port safety caps, not engine limits | `em_model.c` |

## Q. Engine systems with NO port counterpart

From FINDINGS + SUBSYSTEMS.md cluster inventory. Each is a whole subsystem absent from
the port (beyond the per-module gaps above).

1. **Pickups / item collection** — func_001C4820 placed-prop behavior, class-4 kind-0xB
   item records (the office "knife pickup" ammo box, s11), category-list publish + the
   player-side take path, inventory writes. Port scenes contain no pickups at all.
2. **Inventory system** — the byte-per-type count array D_00810C64 (s18), item usage,
   healing items, equipment/event items.
3. **Battery mechanics** — the spend/recharge currency (s21); the port only displays the
   static value.
4. **Infection mechanic** — PARTLY CLOSED s58: the at-100 consequence (60-HP cap, drain,
   infected flinch/death variants, display-max swap) is in; the infected SKIN tint on the
   in-world player (s49 tint vec) and infection pickups/cures remain display-only.
5. ~~**Player damage/knockdown/death/game-over**~~ — CLOSED 2026-06-11 s58 (see C12);
   remaining inside it: typed/latch reaction paths 1..0xB, the engine game-over screen
   module (port shows a flagged stand-in).
6. **Other weapons** — the weapon_equip cluster's NIGHT VISION SYSTEM, SPECIAL PURPOSE
   MISSILE LAUNCHER, DELTA AUTO SIGHT SYS, TACTICAL ADVANCED; the port has SPR4 + knife.
7. **Lock-on / aim options** — D_008106E0 target lock (laser color/dot swap), aim option
   modes D_00810CA4.
8. **Enemy families beyond crawler/crate/generator/tendril** — the kind-0xD leech brain
   proper (func_00153F10), the fixture (func_00156620), nest children, the rest of the
   s22 behavior-pointer census (95 placements across 13 areas use more behaviors than the
   port models).
9. **Room light rigs + dynamic lights** — D_00251C50 per-room rigs, the ≤32-slot point
   light pool (func_001D7FA0), per-actor VU1 light matrices (s51) (see F1).
10. **Area overlays** — AREAxx.BIN runtime code (per-area gameplay/event logic,
    area_logic/area_state clusters), the overlay dispatcher func_001E7780.
11. **Per-area render env** — func_001C1D00/func_001C1DC0 (GS regs, channel enables,
    fog, weather); skeleton no-op in the port.
12. **Scripted events / cutscenes** — the cutscene frame variant func_001AE6B0 (skeleton
    only; the frame selector never routes there), in-game scripted sequences.
13. **Difficulty map** — built at scene init by the engine (func_001AE040 state 0).
14. **End-of-level poll** — func_001AE7E0 beyond its menu-gate slice.
15. **Camera modes 2..15 + scope** — one-shot repositions, timed holds, settle mode 8,
    the sniper/scope camera func_0022EEF0 (zoom 224/x), scripted zoom lerps func_001D2590.
16. **Title screen / front-end menus** — ui_screens cluster (area-name title screens,
    menu flows; game_task states beyond the in-game arm are pending).
17. **Options screen / button remapping** — the config-mask block is hardcoded to the
    decoded defaults (B6).
18. **Save system / memory card** — save_memcard cluster (BASCUS-97112 EX_DATA savegame
    build/format, libmc service), save/load UI.
19. **Movie playback** — the libmpeg movie cluster (FMVs).
20. **Credits roll** — ui_credits cluster.
21. **Status sub-page interiors** — map page, database, weapon customization, passcode
    keypads 4/5 (L7/L8).
22. **IOP audio architecture** — SPU2 voice envelopes/ADSR, the sequencer, reverb,
    positional audio, the streaming IOP RPC pumps (M1–M4 are the per-call symptoms).
23. **Second player port / multitap input** — the engine's second button triple (A7).
24. **GS readback path** — func_001D7410 (gated off in every live sample; documented
    no-op — listed for completeness).

---

## Review notes

- The port's inline documentation is excellent: ~90% of the entries above were already
  flagged at the divergence site. The genuinely UNFLAGGED items found by this audit are
  P1–P6 (clear color, TURN_SPEED, no-death gap, dead constant, stale walk-out label,
  clip planes) and the SU rows (A5 second fade tick, F3 alpha test, F9 loop seam, E4 UI
  projection derivation, I6 melee impact direction, K9 slider commit interleave, J2
  damage window, D3 wall-rise model, L3 ring corners, L9 page anchors).
- Highest-leverage fidelity work, in order: pickups+inventory
  (Q1/Q2), room lighting (F1),
  screen-space acquisition (H3), audio pitch/volume (M1/M2). (K2 door triggers: closed
  s58 — the engine trigger is the CROSS edge and the port now matches for all families.
  D10 engine projection: closed s59 — adopted exactly, verified against the state01
  engine K; H3's screen-space cone is now directly implementable on the real canvas
  mapping.)
- When any FINDINGS section is revised, re-audit the matching module section here — this
  file is the port-side mirror of FINDINGS and goes stale the same way.
