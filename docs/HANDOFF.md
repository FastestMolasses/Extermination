# HANDOFF — Extermination (PS2) decomp + native port

**Current as of 2026-09-25 (Claude, s87).** This is the short cross-repo entry point.
Everything below the "ARCHIVE" line is older notes, kept for reference only; much of it
is superseded.

## Start here
1. Read both `CLAUDE.md` files (this repo and `../extermination-port`). They hold the
   legal hard rules (never commit or print disc-derived material), the fidelity rules
   ("a label is not evidence"; missing workers fault), the test policy (first-level
   scope, ~10 s default, `EM_TEST_FULL=1` exhaustive, headless) and the cleanup policy.
2. **Goal (user, 2026-09-22/23):** port the whole first snow level (AREA11) completely
   and faithfully, up to Roger, then continue into the next level. Nothing may be
   invented: every behavior is a translation of original code and is verified by an
   original-instruction oracle or an original capture. The decomp's C stays
   byte-identical (`tools/verify_all.py`).
3. **Two port profiles (user, 2026-09-23):** Original (the default, and the only thing
   fidelity work measures: the exact GS framebuffer at 4:3, no smoothing, no CRT) and
   Enhanced (switches over the same logic). See port `docs/PORT_PROFILES.md`.
4. The port roadmap lives in its docs:
   - `docs/FIRST_LEVEL_AUDIT.md`: H-items and WP-0..WP-18 with status.
   - `docs/SCENE_COORDINATOR_DESIGN.md`: S1..S13, lead decisions D2..D5.
   - `docs/ORIGINAL_FRAME_ORDER.md`.
   - `docs/FIRST_LEVEL_ROUTE.md`: the original route to Roger, captured in
     `build/s87/route/` (keep).
   - `docs/EE_FLOAT_MODEL.md`: measured EE/VU0 float rules.

## Verification toolkit
- Original-instruction oracles: port `tools/test_*_reference.py` run the ORIGINAL
  ELF/overlay code in a Python MIPS/VU oracle over captured RAM in
  `build/startup-reference/`. Floats follow port `tools/ee_float_model.py`; the
  recordings are in `build/startup-reference/ee_float/` and the recorders in
  `tools/ee_float/`.
- Original runtime: `tools/pcsx2_session.py` runs PCSX2 hidden, with exact one-frame
  steps, pad input, memory reads and snapshots. Save states are in
  `build/startup-reference/portable-data/sstates/` (01-15 are the user's; never
  overwrite them).
- Port live checks: `EM_STARTUP_TEST=newgame-control` (displacement 9.599989) and the
  level smoke `EM_STARTUP_TEST=newgame-level` + `tools/test_level_smoke.py`. Its phases
  are checked against the route captures.
- Decomp gate: run `tools/decomp/build.py build` fresh, then `tools/verify_all.py`.
  Latest (bdd40fb): 2150/2211 units; boot ELF byte-identical; 19/19 overlays.
- Lanes: each owns disjoint files, builds privately and gets an adversarial review. The
  lead commits after an isolated index build (`git checkout-index` into scratch, then
  `make all`) and a leak scan.

## State (2026-09-25)
- **Live in AREA11 on original code** (port chains C3..C5; the level smoke compares each
  beat row for row with its PCSX2 route capture):
  - beats 00..08, 12 and 14: panel without power, the battery pickup and status pop-up,
    the no-power refusal with black bars, powering the panel, the elevator ride,
    **climbing the boxes (05)**, **the hill slide (06)**, the truck preview with black
    bars and camera script (07), the truck crossing (08), the crevice jump (12) and
    Roger's encounter (14);
  - the non-director phases of beats 10, 11 and 13 (cage-roof ladders, crevice, tower).
  - Also live: the player stage, the whole FLOOR closure, the Use chain and dispatcher,
    the collision world (native walkers, grid pass, hull locks), the crates and drums as
    original owners, the camera, locomotion and footsteps, and the fog background layer.
- **Not live yet (chain C6 in progress):**
  - the director phases of beats 10/11/13 and Roger's first voiced line: WP-8b
    voice lanes;
  - the render context, and with it the effects and the player equipment (the
    rifle on his back);
  - the fence door (side beat 09);
  - the GS-exact actor draw (VU1 object kernel);
  - the drop shadow;
  - the BATTERY page's original draw and the mode-3/4 presenters;
  - several SFX ids missing from the export (phase B14).
- **Census:** port docs/FIRST_LEVEL_CENSUS.md, recounted after C5 (section 1.14):
  about 246+ live of 716 game functions and only a handful missing. Run the
  recount again after C6.
- **Guards:** tools/check_no_disassembly.py in both repos. The asm bodies stay by the
  user's decision. Never stage while a workflow chain's committer is running (the index
  is shared).
- **Level 2 (AREA01) on the side** (user, 2026-09-25; rules in memory
  `level2-side-track`: new files, decomp overlay sources, ignored build/ only, never the
  port's live files while a first-level chain runs):
  - Phase 1 done: original route recorded (`tools/route_capture.py` group `a01`,
    `build/s87/route_a01/`), census delta (`route_census.py a01-delta`: 154 new
    functions, 89 main line), overview (`tools/area_overview.py`), overlay matching
    (bdd40fb: 33 of 41 byte-identical C). Docs: port `docs/SECOND_LEVEL_ROUTE.md`,
    `docs/AREA01_OVERVIEW.md`; FINDINGS "AREA01 route capture" section.
  - Phase 2 next: local AREA01 asset exports checked against the a01 captures, and
    standalone translations (new files + oracles) of the new functions, main line first.
  - Phase 3 (after the first level is done): an AREA01 binding chain.
- **Next:** the Original profile (exact 512x448 GS framebuffer, 4:3) and the
  framebuffer-compare harness, EE-float harmonization of the older oracles.

---
ARCHIVE (pre-s87, partly stale — do not trust without re-checking)
---

### Continuation through native f137482 (2026-09-22)

Native checkpoints now include `5a29c4c` original status decoration and
bounded4096-record queue; `d81656d` verified AREA11 panel sound bank;
`3da97b0` original door transit/program and player43/45 channels;
`c7d07b9` required AREA11 sound-scope lifecycle; `f137482` original Roger
capture and player face allocation/reset/free proofs.

Door both-side real-resource ASan/UBSan fixture reaches original room entry2
after98 callbacks or entry1 after78. It stops at the room request; loading
and ownership release are not claimed. Full original controller/program732
callbacks, kickoff1349+destination112 and shared-frame11520 cases pass.
Sound3EE is original silence;3EF pitch862 and Q14 gains2217 are verified
through288 EE dispatch/end cases,216 pitch cases and336 original IOP/libsd
register writes across48 voices. Corrected native bank uses10101.5625Hz;
linear interpolation, reverb, envelope microtiming and scheduling remain
boundaries. Host missing-bank cleanup/reload and prior panel flows pass.

Fresh original Roger state15 was captured after a controlled position-only
trigger from immutable state03. Both actors at source25.5 match1050 channel
float words and126 key cursors exactly. Camera source25.0 eye/target bytes
match; view-matrix error is at most0.000123. This is not movement or pixel
fidelity evidence. Both actors have face objects and suppressed body head7.
Full original allocator/reset/free execution proves captured first-control
and status player face allocations start zeroed, AF890 clears208 bytes on
free, and256 repeated attachments preserve existing weights/wait fields.
Later whole-game RNG order remains open. Player dialogue calls D06E0
straight from FD950; it does not consume Roger's activity-byte convention.

Goal remains active and unfinished. The assembled host is still NOT wired
into the complete live11-owner scene loop. Current parallel work: Dennis
cinematic face host; dynamic status health/battery/ammo draw workers;
BC350 readable-C actor forwarding and match verification. Root owns Roger
camera/media integration and the eventual scene coordinator. PS2 container
is reserved to area11_original; GPU is free. Never move/mount/restore the
emulator or backups: user cancelled that. Preserve native user README.md
and tests/run_suite.sh changes.

# HANDOFF — Extermination (PS2) matching decompilation

Paste this whole file as your first message. Working directory:
`/Users/abe/Documents/Extermination.nosync/Extermination`
**Never** the iCloud copy at `~/Documents/Extermination` — that is a different, stale tree.

Read `./CLAUDE.md` first. Its hard rules are legal constraints, not style preferences.
The short version: **never commit, print in full, or redistribute the ISO, the boot ELF,
disassembly of the original binary, or extracted assets.** Everything disc-derived is
generated locally from the user's own disc and is gitignored. Check `git status` before
every `git add`. Also keep this project entirely isolated from the user's other repos.

---

## Active continuation (2026-09-22 UTC)

### Continuation through native bc0a888 (2026-09-22)

New native checkpoints: `5142f99` Roger actor/face adapter; `425dc6b`
bank96 camera/player export and raw proofs; `2c439ee` original cinematic
entry sub9–12; `3753a89` normal status hub/arc geometry/SDK ±4π;
`e8a127e` initialized AREA11 door controller/resources; `bc0a888` deferred
cinematic player-bank request, real half-rate player clock and release.
Decomp remains `a73c2a7`, 98.16%; no additional byte-match claim.

Roger camera passes1385 original samples and player5010 decoded keys.
Cinematic entry passes6184 original state/service-order cases plus8 failed
worker boundaries. Player bank request waits until the next player stage,
then83090 initializes and advances SAME callback: first sample0.5,
remaining690.5.1388 original clock callbacks+release match; actual-resource
ASan/UBSan shared ownership/status freeze/world-palette fixture passes.
Mode2 uses identity owner matrix (C6960), release restores default idle0 at80
before next ordinary79. Body bank and face attachment are separate operations.
CORRECTION: B81D0/CA700 attaches face at+90 and suppresses bone7; ready2 means
face attached, not a replaced body skeleton. Shared runtime now acceptsready2
only with an explicitly installed face/body worker; missingworkerfaults.
The real Dennis face, Roger camera/media and live11-ownerloop are still open.

Status hub17520 original cases; arc500cases/5562exactvertices; SDK6418trig
results including±4π; page1460cases and normal-open→ITEM→Back→hub→exit
sanitizerPASS. Actualstate12→fresh14hub captured with source12unchanged.
Referenceagent owns original2D export/render; fixture exposed2376decorrecords
versus1024capacity, narrow4096decorqueue change in progress. GPU currently
reserved for its bounded fixture. No emulator relocation/recovery authorized.

Door5662 original controller cases,123decodedkeys,50capturedfloats,
16ownermatrix+32palettewords exact;240passive actualassetASan/UBSanPASS.
Passive door does not advance animation. Areaagent owns BBE40/BC150 transit,
frame/sub0, player43/45 source clips/timing/allowlists. Side chooses sound401/402
(not open/close). Current transitworkers are still not installed in scene.

Soundagent proved AREA11 cue3EE intentionally mapsFF/no sound;3EF is real.
Raw FB9F0→119EA0→117088→115850→117918 path and residentSPURAM sample agree:
pitch862/4096*48000=10101.5625Hz, direct Q14 gains2217/16384, ADSR80FF/5FD0,
1312ADPCMbytes,no loop.288dispatch/end cases+216pitch casesPASS. Scoped
nativebank/mixer under implementation; host must select(11,0), clear(-1,-1)
when em_sfx_set_area APIready. Old manifestpitch15480wasincorrect.

Goal remains active and unfinished. Host37ca9fe is built but NOT installed
in the live scene loop. No claim of a finished faithful first-level checkpoint.
UserREADME.md modification and tests/run_suite.sh remain untouched.



Latest native checkpoints: `37ca9fe` assembled native panel/player/status
host, `a7c175d` SDK reference-runner scalar-width/tailcall corrections,
`a48a982` original door predicate/checked Use arbitration, `9e779aa` Roger
controller/resources, `75d91ba` geometry/camera commit, `d88527f` battery
pickup/device lookup, and `302124f` hip mirrors. Decomp `a73c2a7` corrects
D930 hub/alternate branches;8064 compiled-readable-C state/sound cases pass,
similarity67.97805->72.58049%, stillassembly-backed. Full6gatePASS in
build/item_root/verify_d930_hub.log; PS2container is FREE. Earlier `4576740`
anglewrapper ordinaryC100% remains selected; overall98.16%unchanged.

Alignment passes1639 original B6F00/SDK cases. Projection passes1606 original
DD980/DD950 cases, all24contextbytes. Camera commit gate passes1623cases;
status's nonzero argument pushes4 even in top-mode3, except mode0A uses-1.
Recovery is decremented at the real18B9C0 stage after player/owners, including
top-mode1/2; status completion writes70 and its final consumed frame preserves
70. Use seeing1 stays blocked until the next frame. The recovery oracle covers
5120states,16frameorders,256Usegates,5statuspairs. Scope0 is bits43F02F4F
(480.3695983886719), not480 or platformtan.

Player face/align mirrors are fixed: raw3053cases pass including1624new order
comparisons. Face must preserve cached hip and savedEuler; align translates
that prior hip and savescurrentEuler. The next player tail republishes the
new pose. CORRECTION: refusal align/face/D5 records yield betweencommands;
there is no proven same-callback refusalD5 effect. Panel face->align does
occur in a single ownerhook. Do not repeat the earlier timing assumption.

Battery pickupstatus kind1/index1B..1D now runs real240-callbacknotice and
original group4text; actualassetASan/UBSan passes, rawCDC0 1352cases and149F0
11904callbacks+84init/inputcases pass. Device lookup uses first eligible
publishedowner, not nearest or a rememberedpanelpointer (2764originalcases).
Rogerbank4A has9rawclips;6258keys and2saved525float/63cursorstates match exactly.
His controller6912, predicate900 and trigger324cases pass. Bank96 encounter
camera/player/Roger clips0/1/2 remain under runtimeintegration. Distantdoor
predicate3725casesincl75actualplacementcases passes; transit still separate.

Native host37ca9fe is now built inCOMMON, but NOT installed in live scene
loop. ActualassetASan/UBSan fixture passes panelno-battery156ordinaryticks,
firstbatteryactualturn/settle118, defaultNo/reselect/cancel170, and61status-
dischargecallbacks thenpower/release285. Rawpose/palettes/ownerscripts freeze
throughstatus; final consumedframe70; acquiredowner teardown/reloadpasses.
CLEAR_DRAW after END_PROJECTION initiallyfaulted; fixed separateUIpoollife.
Pickupframecommands require camera_fields synchronization before laterowner
begin_owner; pickupcamera sub8changes actualtargetonly, desiredunchanged.
Frozen18B9C0 top1/2 onlycommitarg1 (no nativecamera_update timer/substate).
The currentSFXmanifest lacks panelcue3EE/3EF; audioassetbindingsremainopen.
Complete11-ownerwalker, normalstatushub, andRoger/doorlivehostsremainopen.

Rootnewuncommitted tools/export_roger_cinematic.py +reference test export
bank96camera0/player1:691frames,1385fulloriginalC7C00samples exact,5010raw
playerkeys exact(+63sentinels). Ignoredassets roger/encounter_camera.emcc and
encounter_player.empc. NPC96clip2 handledseparatelybyold_portagent. Roger
runtime/newfacehelperpasses actualassetsanitizer butnotyetcommitted; its
sharedplayer/camera/message/audio/scriptworkers are next. Areaagent is now
recoveringoriginaldistantdoorBC350/BC300controller+realpublication; hubagent
workingoriginalnormalstatushub+models/artwork. UserREADME.md modification
andtests/run_suite.sh remain untouched.

The goal remains active and unfinished. Keep working autonomously with
parallel agents. GPU is free. Never move, mount, restore or recover the
emulator/backup: the user cancelled that work with "Ok dont move it".



The user now prioritizes **New Game through the original first-level opening,
correct cutscenes, and player interaction**, while continuing readable C and
PS2 byte matching. Keep automating with subagents; the active goal is unfinished.
Use original executable/runtime evidence, not existing port claims.

The user explicitly authorized examining the old port at
`/Users/abe/Documents/extermination-port`. Keep that copy read-only. The current
port is the sibling `../extermination-port`. This permission does not extend to
the stale old decomp at `~/Documents/Extermination`.

Native startup runs the warning, Sony, Deep Space, complete E900 intro and
interactive title. New Game requests E900 again and now runs the original
AREA11 opening script, camera, actor clips, dialogue/audio, fades and completion
handoff. Full GPU run completed at native gameplay frame1304, then captured the
first-level view at1400; see port build/opening_visual/frame1400.log/png.
This is NOT a completed fidelity checkpoint: exact locomotion/stop poses, camera
solver behavior, face initial state/RNG ordering, snowfall rasterization,
material/geometry/lighting differences and other port approximations remain. Earlier native3de0882 adds original ITEM/status-page cores and ordered artwork,
after13eba00 panel runtime, ceda5ac raw player channels and9a03782 timed
elevator refusal. Earlier589895c adds flame particles, UI blending and camera
rotation;8eabeb2 shared interaction/status/elevator adapters andf79be72
and970a98a (lever animation),6714643 (vertex lighting),662f6ec (panel components),
8076198 (elevator program),62d02b8 (re-entry probe order) and earlier checkpoints;
user README.md edits and tests/run_suite.sh remain untouched/uncommitted.

Opening assets are exported from verified original resource locations. Bank98
is chunk15/f12_id44+0xD0800; the unrelated file f06_id98 is not this bank.
Camera sample134.5 matches all six original eye/target float bytes in the saved
EE state. Opening camera mode3 has NO four-unit gameplay camera push; corrected
native view differs by at most0.000031 from the original matrix after Y/Z row
conversion. All roll samples in this opening track are zero. Actor pose baking
follows original unnormalized quaternion blending and stateful half-tick
cursors/cut flags. Body transforms remain within0.000092 of original EE/VU
matrices. Dennis also needs the seven independently drawn rifle/knife children,
attached to bones4/14; these are now exported after a GUI comparison caught the
missing parts. The disabled class8 controller does not imply its children hide.

Headless opening integration uses actual exported assets and real script,
loaders, audio mixer, dialogue/fades and actor/camera modules under ASan/UBSan.
Normal and skip paths restore the exact final script placement and grant event
B9/key0 once; missing required assets fail without granting completion. Native
prefill completes faster than original disc/IOP readiness, so absolute scene
frame numbers differ. Compare camera/actor cursors, not wall time alone.

The old snow/drawbridge asset directories supplied65 missing files. Hashes are
in port build/old_port_copied_assets.json. Snow's six meshes and collision
regenerate identically from current extraction inputs. Both old and current
collision filters were wrong: native camera/movement/general predicates now
pass an oracle executing6144 original ELF branch cases. Baseline and modified
broad gameplay suites both had15PASS/5FAIL with incomplete office fixtures;
that suite is not a fidelity certificate.

Face mesh/morph behavior is integrated: 11,552 full-state comparisons and 26,880
VU vertex blends pass. Original pooled initial weights, global random-call
ordering and separate head lighting remain unverified. The frame280_faces GPU
capture confirms the subtitle now draws over the letterbox bars. Original
canopy placement and six pickup-light children have also been recovered.

Current lanes: live raw player-pose tracking; complete original use arbitration/
publication including pickups, Roger and the distant door; ITEM trail and real
BATTERY/status host integration. Root panel runtime passes actual asset tests
with original message, raw source channels, discharge, pause and fault handling.
The original flame replacement is integrated; sound/hazard contact bindings
remain separately incomplete. Native scene interaction orchestration remains open.
Panel collision/upper probes and auxiliary point light are integrated in f79be72.
The first30input
frames now travel9.599989 versus original9.599849. Final script camera ownership
fixaf598bc reduces horizontal endpoint error to0.000168. Run release plays
original clip5 then idle, and interrupted stop restarts clip2 at source27 with
four blend ticks. Commitbd0aeb9 records the latter, but follow-up audit found
argument1 of00178B90 probes immediately between its first and second translations;
that correction is committed62d02b8, with9 original call-order cases passing.
The later railing displacement was caused by omitted actor collision, not scalar
speed. Do not alter speeds to hide collision mismatches.
The original hit is now traced to actor7AA590 (panel), collision cell18:
five type2000 compact faces, X238..242/Y242.2..248.1/Z230.8..232.8, previously omitted
from EMCL. f79be72 adds a separate original compact-cell resource plus interleaved
upper18 radial probes. 4,800 compact-face cases,721 exact hits and captured4140
push pass. Full opening/re-entry GPU run now matches original4141 X within0.000046
and Z exactly, with lane1 blocked02. Low-clearance/ledge writer, ankle response
and other compact cells remain open; see port docs/FIRST_CONTROL.md.

Snow now uses the original emitter, VU generator, projection and GS corner
quantization in native Metalc3f1e8f. Original projection matches5,375 cases,
including all4,320 particles from216 captured DMA tiles;151,664 output bytes
and192 reconstructed matrix bytes match. The old visible-center clipping was
wrong: the original uses a wider guard band. It also pairs ST(0,0) with plus
extent and(1,1) with minus. Headless240ticks pass ASan/UBSan with27,476 submissions,
15,993 outside-viewport centers. Full Metal frame280 smoke completed exit0:
port build/opening_snow_projection/frame280.png/log (frozen binary alongside).
Global RNG ordering, upstream camera arithmetic and native/GS raster differences
remain open; no pixel-identical snow claim. The CPU emitter's216 captured tile
qwords and6postphases still match; generator1280cases/8480records pass.

Elevator core988b31f and adapter8076198 recover original00827B10 plus15script
records. The828050 carry helper matches240 raw-instruction cases; active
owner2520cases pass. Original owner+2A=300 suppresses a delayed sound, it is not
a refusal cooldown; both exporter entries now use0. New unbound program adapter
and real-asset sanitizer test preserve immediate alignment/yaw, clip47 wait,
150carry ticks, two-call camera copies, then final owner toggle. Frame/camera/
player/message host bindings remain explicit requirements. Clip47 exporter970a98a
replaces only200 lever frames using original stateful unnormalized sampling.
All21 captured source39 world bone matrices agree within0.0000610352 versus
old0.00436401. Other56clips, tables, mesh and textures are preserved; repeat is
idempotent. Original fixture writes only owner arm4 from saved post-panel06,
captures new slot11, and preserves the starting archive.

Source commit e022c4d corrects E67C0's wrong CFAE0 float arguments and false
explosion/audio descriptions. Corrected C is70.915985%/1784B versus original
1952B, remains NEARMISS and assembly-linked.24compiled cases/2376 submissions
and all6 full gates pass; log build/weather_reference/match/verify_all.log.

Source commit941daa1 fixes a broader resident PSMT8 palette export error:
physical PSMCT32 memory decoding must precede CSM1 index permutation. The
omission turned soft snow into a ring. Three synthetic palette tests pass;
full six-stage decomp gate passed with the correction. Of290 first-level
static texture bindings289are PSMT4; the one PSMT8 movable was regenerated
with geometry/animation prefix unchanged. Opening/face/prop/snow exports also
regenerated. See docs/CLUT_LAYOUT.md. Older assets elsewhere need regeneration.

Battery inventory preserves original half-unit charge/capacity semantics.
Panel owner448cases and BATTERY002149F0 page936 original instruction cases pass.
The new em_battery_ui page matches all29 captured decor/cursor quads exactly;
its ASan/UBSan flow and exported original panel script tests pass. The129-tick
post-menu script's sound/power callbacks match original captured timing.
The live scene binding still waits on original frame/status and camera solver
bindings; never silently grant panel power or auto-confirm. Clip15C export
preserves all56 prior player clips, mesh and textures while appending121frames;
all21 captured frame30 bone matrices match within0.000061. A0 is the ground
origin, B0 is the animated hip: this clip does not move A0. Original confirmation
capture is build/startup-reference/panel/confirm.png; fixture seeds a battery
and arms the actual owner, not an unassisted walk. Controls:8000Left,2000Right,
40Cross; default selection No. Native old Found-message/status shortcuts remain
incomplete. Model72 pickup boxes retain all3 authored rest-pose nodes.

Lighting6714643 now lights authored vertices and quantizes colors before
affine GS-style color interpolation.6,720 original face/body record cases,
1,000 synthetic cases,224 captured matrix bytes and24 rig direction bytes match.
Face and body use separate rigs. Frozen Metal before/after frame280 runs exit0
at the same camera/actor cursors; this is not a full pixel-match claim.
Decomp exporter80d35cb preserves9digits/original rotation polynomial and corrects
the false elevator cooldown. Auxiliary room point light is a separate controller,
not the steam actor. f79be72 integrates original registration/update/fold with
256 full pool steps,780 RNG outputs,256 folds and40 registrations checked. All64
captured flicker-matrix bytes and12 body DMA color bytes match. Frozen Metal
frame280 and normal/skip opening smoke pass; whole-game RNG order remains unverified.

Native f79be72 also adds the original interaction animation clock:662 raw original
callback comparisons cover clip47/15C with blend0/1, preserving commit, first-pose
and end-flag timing. Clip47 blend1 ends201 callbacks after commit;15C blend0 ends121
later. Camera retarget216 prepass/432 bounds cases, captured camera fields and
2,592 frame sub2/sub4 cases pass. Idle0 exporter preserves other56 clips and checks
all21 captured matrices. These components are not yet the complete live interaction.
Shared runtime/elevator adapter8eabeb2 tests actual200-frame animation and
both ride directions, ownership contention, status freezing and failure retention.
589895c adds optional raw-channel pose worker, called even on blend1 preserving
callbacks. Original source channels are being integrated; do not derive them
from displayed matrices. New exact749A0 confirms same-idle acquisition preserves
its cursor (no unconditional8-tick restart). Scripted47/15C release metadata0
forces idle0/blend0, then the default16 request is a same-clip no-op.

Native589895c replaces the invented steam with original80-particle flame owner
008235F0.280 owner/768 contact/1135 projection cases pass; physical VU scratch
1152bytes match.400 runtime ticks/30360submissions pass ASan/UBSan; frozenframe280
and near-effect GPU fixtures pass. Sound413's real radius100/key-off/ADSR/loop
and contact effect80000027 remain unbound, documented; no guessed hiss remains.
Whole-game RNG ordering is unfinished. Original world fog is missing separately.
UI overlay alpha/add/subtract/opaque and ordering/clamping pass a GPU pixel fixture.
Camera SDK rotation972cases/77760bytesexact; D5refusal capture fresh13 from06
matches six eye/target coordinates, bounds andflags. One overheadcollisionfloat
is1ULP different(0.0000305176); don't call completecollisionequivalence.
OriginalPID57406exited0,source06hashunchanged;GPU/containerfreeatcheckpoint.
Currentagents:area11_original rawplayerposeAPI+ordinarysourcechannels;
reference_runtime originalradialITEMroot/BATTERYlifecycle;old_port_audit single
originaluse-scan/winnerlist/candidatepredicate. Root owns next scenehost binding.

One PS2 container at a time, coordinated with agents. Original and native GPU
runs are serialized. Do not use execution breakpoints in this PCSX2 fork;
they stalled it. Immutable states01..04 remain preserved.

CRITICAL overlay analysis issue: canonical overlay splat maps file0x40 to
VRAM823500, while the runtime loader maps the whole MWo3 module to823500.
Generated labels are therefore0x40 lower than runtime addresses, and JAL
splits can be wrong. Use isolated build/area11_original analysis; do not
silently change canonical mappings before verifying all overlays.

PS2 HEADfd8ca8e adds exact212-byte anim_clip_init and72-byte001749A0
with explicit blend/source-frame arguments; both now link ordinary compiled C
instead of size-list assembly fallbacks. All6 fresh checks pass, boot identical,
19/19 overlays, objdiff2051/2148 and98.16%. Evidence:build/interaction_init/.
Precedingedda070 makes script animation worker00183090 readable exact C (208 bytes),
verified as an actual ordinary compiled-C object, and fixes D7C30's scratch buffer
from4 to16 floats (still assembly-backed NEARMISS). Fresh six-stage gate passes:
build/interaction_animation/verify_all.log and link_provenance.json. It follows
80d35cb and87a097b (CBD0 falloff corrected,91.78->94.04%),
c356608 (C440 arguments, unchanged96.65% score),941daa1 and earlier fixes.
Both corrected functions remain assembly-backed NEARMISS; full six-stage
gates pass. Quaternion bounded respelling search remains99.05797%, no promotion.
Original startup compositor001ABF90 is
readable exact C, screen loader001FF080 actually links compiled C, and title
renderer001AC7F0 remains NEARMISS97.84173%. Commit781be11 corrects collision
0019D330 (88.75%) and selector gate001C5930 (69.04%); all6 gates passed in
build/area11_original/verify_semantics.log, boot0x175b00 identical, overlays19/19.
Commitcdbe555 corrects BA1F0 skip-loop C (73.65%) and replaces quat_nlerp
assembly wrapper with readable unnormalized blend C (99.05797%). Both remain
NEARMISS with original assembly linked. Full gate
build/area11_original/script_verify.log passed all6 checks; integrator registry
upsert now prevents stale duplicate rows, with16 host tests passing.
The integrator no longer asserts semantic faithfulness from object similarity.
Commit9034edf corrects D3E40's DMA channel argument and D0720's unknown-state
talk fallback. Both remain NEARMISS (81.26471%,97.27811%) and assembly-backed.
Compiled candidate oracles pass384 DMA cases and7,680 face state transitions;
full six-stage gate passes. See docs/OPENING_FACE_DECOMP.md.

**Audited metrics (latest animation-init checkpoint):** source lexical classification is1510
ordinary C,638 inline-assembly wrappers,790 NEARMISS,15 INCLUDE_ASM (2953 total).
The3010 canonical boot slots select1224 ordinary compiled-C objects,537 inline
assembly objects and1249 original-assembly fallbacks. Ordinary compiled C covers
314,448 slot bytes. All1761 copied objects' prepared text and relocations agree
with linked filler objects; the full boot/overlay gate passes. These are code
slots, not a proof that every canonical function boundary is correct.
Fresh objdiff reports2051/2148; the old2051/2149 counted stale quat_nlerp output.
Build/report tools now enumerate current sources, with3 regression tests. See
build/area11_original/{provenance_verify.log,link_provenance.json}.

**PCSX2 relocation stopped at user request: "Ok dont move it".** Do not make
further moves, mount attempts, or Time Machine recovery attempts unless asked.
The user initially authorized moving Desktop's
fork under the parent. parent/pcsx2 now exists but its Git objects and2288 of
2968 tracked files are missing; preserve it, never reset/overwrite. All680
surviving files match index after CRLF normalization. A pre-move Time Machine
snapshot19:47:51 exists on Data /dev/disk3s5. Administrator-authenticated
read-only mount still failed Operation not permitted due Full Disk Access.
Computer Use also did not approve Time Machine access. Recovery is now stopped
per the user's instruction. Recovery helper/evidence are in
build/startup-reference. Do not claim history was preserved until recovered.

parent/PCSX2-MCP is separately verified clean atfe9056c: source/.git copied,
lockfile dependencies restored with npm ci, npm build and35-tool stdio smoke
PASS. Old Desktop/PCSX2-MCP remains a backup. Installed /Applications/PCSX2.app
is already the MCP-enabled x86_64 Rosetta v2.6.3-3-g00d19ccce; its isolated copy
still runs the original game with DebugServer port21512. PINE uses a macOS Unix
socket, while the companion defaults to TCP28011. No emulator update is needed
for current reference capture; source recovery comes before any rebuild.

## 1. What this project is, in one paragraph

A matching decompilation of **Extermination** (PS2, 2001), target **SCUS-97112**. "Matching"
means the C we write must compile to **byte-identical** machine code. The original was built
with **Metrowerks CodeWarrior for PS2** (`MW MIPS C Compiler 2.3.1.01` per `.comment`), with
some Sony SDK objects built by **ee-gcc**. The boot ELF is stripped — no DWARF, no symbol
names — so this is blind matching. 2,953 functions total.

**Current standing (code committed, HEAD = `80d35cb`, all six checks green; 2026-09-22 UTC):**

| | count | of 2953 |
|---|---|---|
| ordinary C source files (not a semantic correctness claim) | **1508** | 51.1% |
| `// NEARMISS` (compiled-C link not yet verified) | 790 | 26.8% |
| `// INCLUDE_ASM` stubs (no readable C at all) | 15 | 0.5% |
| Source files containing inline assembly | **640** | 21.7% |

Two separate goals, do not conflate them: readable semantic C and compiled-C byte
matching. Both require more work; object matching is not proof of readable C.

The 2026-09-22 continuation added **four actual compiled-C/ELF-verified matches**:
`func_00180850`, `func_001EA240`, `func_0012CAA0`, and `sub_PsIIlibpad_2000`.
The object tracker is2050/2148, including inline assembly and fallback units.
Use the audited1221 ordinary compiled-C slots above for current link provenance.
`func_00135D00` now scores 100% but remains NEARMISS because its 36-byte local
jump table is not pinned at jtbl_0026D190. The linker still supplies its assembly.

---

## 2. THE COMPILER — read this section twice

The period-correct compiler is a **32-bit Windows PE** that cannot run natively on the user's
Apple Silicon Mac. The chain is:

```
Apple `container` (Linux VM, arm64)
  └─ qemu-i386            (x86 emulation)
       └─ wibo32          (minimal Win32 PE loader)
            └─ mwccps2.exe (CodeWarrior MIPS compiler)
```

The MIPS **assembler** and **linker** are arm64-native inside the same container; only the
compiler needs qemu+wibo. `objdiff-cli` runs natively on the host.

### 2.1 Start the container service first

```bash
container system start
```

**Known wart, do not be alarmed:** `container images list` fails with
`Error: Plugin 'container-images' not found` — the CLI (0.12.3) and the installed plugins
(`/usr/local/libexec/container/plugins/`, which provides `container-core-images`) disagree on
a plugin name. **`container run` works fine**, and that is all the build uses. Verify with:

```bash
container run --rm exterm-permuter sh -c 'echo CONTAINER_OK'
```

If the whole daemon is down, `verify_all.py` reports `boot-elf FAIL — link produced no verify
line`. That is a stopped daemon, **not** a broken build. Start the service and re-run.

### 2.2 The image

`IMAGE = "exterm-permuter"` (set in `tools/decomp/build.py`). It is a superset of the older
`exterm-toolchain` image: adds i386 libs for ee-gcc plus decomp-permuter dependencies.
Recipe at `docker/Dockerfile`. Every container call is:

```bash
container run --rm -v <REPO_ROOT>:/work -w /work exterm-permuter sh -c '<script>'
```

### 2.3 The actual compile commands

CodeWarrior (the game code, default):
```bash
qemu-i386 tools/bin/wibo32 tools/mwccps2-233/mwccps2.exe -c <FLAGS> -o build/obj/<name>.o src/<name>.c
```
ee-gcc (Sony SDK functions):
```bash
tools/eegcc/ee-compile.sh src/<name>.c build/obj/<name>.o <FLAGS>
```

### 2.4 Which CodeWarrior builds actually exist

`tools/mwccps2*/` are **user-supplied and gitignored**. Only **three** are installed:

| directive | path | notes |
|---|---|---|
| `mwcc` (default) | `tools/mwccps2/mwccmips.exe` | 2.3, build 991202 |
| `mwcc233` | `tools/mwccps2-233/mwccps2.exe` | 2.3.3 (000906) — **the workhorse**; cracks the idiom-13 delay-slot family |
| `mwcc24` | `tools/mwccps2-24/mwccps2.exe` | 2.4 |

`build.py` also knows `mwcc30` / `mwcc301`, but **those are NOT installed.** Any sweep that
reports them is reporting `n/a`. Do not plan around them.

Matching a function with a *later* CodeWarrior build than 2.3.1 is legitimate — byte-identity
of the loadable region is the only criterion, and `.comment` is not in that region.

### 2.5 Per-file compiler routing

A file selects its own compiler and flags via directives in the **leading comment block**:

```c
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
```

Defaults are `mwcc` and `-O4,p`. `-sdatathreshold` (0 / 4 / 8) matters a lot and is
per-file — getting it wrong silently costs ~0.5%; one function this session measured
99.44 / 99.96 / 99.57 across the three values.

**Historical bug worth knowing:** `file_cflags()` used to stop scanning at the first
non-directive comment line, which silently discarded the declared flags for all 628 near-miss
files (whose first line is the `// NEARMISS` banner). Fixed. If you touch that parser, re-measure.

---

## 3. The build loop

```bash
cd /Users/abe/Documents/Extermination.nosync/Extermination
source .venv/bin/activate          # splat, objdiff bindings, pygltflib live here

python3 tools/decomp/build.py setup   # splat split + asm_fixup + regenerate objdiff.json
python3 tools/decomp/build.py build   # assemble expected/*.o + compile obj/*.o  [container]
python3 tools/verify_all.py           # THE GATE
```

`setup` is only needed when the set of translation units changes (a file flips between
stub/NEARMISS/compiled). `build` alone suffices after editing an existing compiled unit.
A full `setup && build && verify_all` is roughly 4–6 minutes, dominated by the ~130 s link.

`verify_all.py` must print **all six PASS**:

```
boot-elf   PASS  boot ELF byte-identical (0x175b00 loadable bytes)
overlays   PASS  19/19 overlays passed
match      PASS  matched_code 98.14% (floor 95.0), functions 2052/2150
gltf / selftest / gs-offset  PASS
```

**Do not commit unless `boot-elf` says byte-identical.** That is the project's whole point.

### Beware the `matched_code` percentage

`matched_code 98.14%` counts matched over **compiled** units, so near-misses are excluded from
the denominator by construction. Promoting correct functions *lowers* it. It is not the
progress metric. The whole-game object-matching number is **2052 / 2953 = 69.5%**;
`docs/PROGRESS.md` also records the assembly-fallback qualification above.

---

## 4. The `// NEARMISS` mechanism

A `src/<f>.c` whose **first line** starts with `// NEARMISS` is excluded from the
ordinary matched-unit selection. Matching tools can compile and measure it;
the marker alone is not a claim that it never compiles. The linked filler uses
original assembly for these slots. Verify actual link provenance whenever
promoting a candidate; a passing boot ELF alone can hide assembly substitution.

This lets us commit *body-correct readable C* that is not byte-exact — valuable as ground
truth for the native port — without weakening the byte-identity guarantee. Every near-miss
header must record the real re-measured objdiff %, the compiler, and the precise divergence.
Registry: `docs/NEARMISS.md`. Promote one by making it byte-exact and deleting the
marker, so it becomes a normal compiled unit.

---

## 5. THE MOST IMPORTANT LESSON: objdiff 100% is NOT a sound promotion gate

Late in the last session, all 792 near-misses were re-measured through the **canonical**
pipeline (trial-promoted so `build.py`'s `expected` path, including `inject_relocs.py`,
applied — not the scratch harness the sweeps use). Five scored 100.0%. **Only one was real:**

```
KEPT     func_001152D8   boot ELF byte-identical
REVERTED func_0012CAA0   first diff 0x0012cccc  0xc9 vs 0x8e
REVERTED func_00169730   first diff 0x0016987a  0x24 vs 0x25
REVERTED func_0016AE40   first diff 0x0016af4e  0x24 vs 0x25
REVERTED func_001EA240   first diff 0x001ea954  0x80 vs 0xa0
```

**Four false positives out of five**, every one caught only by linking and comparing the ELF.
The failing bytes matched the parked wall descriptions exactly (`0x24` vs `0x25` is the
a0-vs-a1 register colouring; `0x80` vs `0xa0` is the `0x3f80`/`0x41a0` FP-constant ordering).

**Gate every promotion on the boot ELF, bisecting when a batch fails.** Commit `9ac388a` has
the original write-up. Also verify `fill_unmatched.py` actually selects the candidate's
compiled object: GPREL_FORCE_ASM, SIZE_DRIFT_FORCE_ASM, local data, and size drift can
silently replace it with assembly. A passing ELF then says nothing about that C.
This continuation removed a stale force-assembly entry for func_00180850 and kept
func_00135D00 parked on its local jump-table placement blocker. The new source fixes
for func_0012CAA0 and func_001EA240 now pass the actual compiled-C ELF gate.

Corollary: **an agent's reported percentage is not evidence.** Four agent 100%-claims failed
integration across three batches last session. Always re-measure with
`tools/match/integrate_nearmiss.py`, which does its own canonical measurement.

---

## 6. Tooling map

```
tools/decomp/build.py              setup / build / --single-file (objdiff's custom-make hook)
tools/decomp/asm_fixup.py          re-applies VU/endlabel fixups after splat regenerates .s
tools/decomp/inject_relocs.py      relocation injection on the expected-object path
tools/decomp/fill_unmatched.py     linker fill from splat .s; LOCALDATA_FORCED + drift guards
tools/verify_all.py                the six-check gate
tools/match/integrate_nearmiss.py  THE integrator: <wave.json> [MIN%]; re-measures, writes
                                   src/, updates docs/NEARMISS.md. Pass 0 to keep low-% C.
tools/match/clean_registry.py      drops stale/duplicate NEARMISS.md rows after promotions
tools/match/baseline.py            measure named functions through the canonical harness
tools/match/permute.py             drive decomp-permuter (tools/permuter/)
tools/match/*_wave.js              Workflow fan-out scripts (Claude-specific; see §8)
docs/fanout/MATCHING_GUIDE.md      THE compiler-idiom bible — read before any matching work
docs/NEARMISS.md                   near-miss registry (committed)
docs/PROGRESS.md                   standing + roadmap
```

`docs/*.md` other than `NEARMISS.md` are intentionally left **uncommitted** in this project.

---

## 7. Compiler idioms — read `docs/fanout/MATCHING_GUIDE.md`

31 documented idioms. The latest is **idiom-31**, nonzero integer-to-float staging:
20 staged through an int fixes func_001EA240; four degrees converted/scaled to the
identical float bits fixes func_0012CAA0. Correct callee prototypes also resolved
func_00180850 and the remaining object residual in func_00135D00.

Two useful earlier wins:

**idiom-29 (strength-reduced multiply: fresh vs in-place shift destination).** The original
allocates a **fresh** register for the final shift (`FRESH = src << k; FRESH >>= 15;
src = FRESH + B`); all three installed mwcc builds reuse the dying source register in place.
The fix is a **source spelling**, not a flag: make the multiply and the shift compound
assignments on the *same* variable.

```c
int x = func_00122BB8() >> 16;
x *= K;
x >>= 15;
... = x + B;              /* the + B must be a SEPARATE expression */
```

Folding the multiply into the initialiser (`int x = r * K; x >>= 15;`) does **not** work, nor
does splitting stages across variables. ~50 respellings were tried; do not re-derive them.
This took the whole 7-function family at once (`func_001549C0` went 64.14 → **100**).

**idiom-30.** Hoisting a call out of a float argument reproduces the target's delay-slot `nop`
in the `mtc1`/`mul.s` sequence.

**The generalizable rule from idiom-29:** when a wall has **no matched exemplar anywhere in
the corpus**, it is a *respelling* problem, not a *search* problem. The permuter did not crack
idiom-29 despite it being pure register colouring; a source-spelling sweep did, with corpus
mining supplying the model. Grep the matched corpus before recording any wall.

---

## 8. If you are Codex (or any non-Claude agent): the parallelism story

`tools/match/*_wave.js` are **Claude Code `Workflow` scripts** and will not run for you. They
are still worth reading as specifications — each encodes the prompt, the schema, and the
per-function attack plan that produced last session's results.

The underlying loop is plain and tool-agnostic:

1. Pick candidates (§9).
2. For each: read the splat `.s`, read the current near-miss C, diff against the expected
   object, form a hypothesis about the divergence, respell, re-measure.
3. Sweep the three installed mwcc builds and the `-sdatathreshold` values 0/4/8.
4. Collect results as JSON: `[{"func": ..., "c_source": ..., "wall": ...}, ...]`.
5. `python3 tools/match/integrate_nearmiss.py <that.json>` — it re-measures canonically and
   decides KEEP / NEARMISS / REVERT.
6. `build.py setup && build.py build && verify_all.py`, then commit.

**Run ONE container invocation at a time.** The Apple `container` daemon has wedged under
concurrent `container run` calls in past sessions. Parallelise your *reasoning*, serialise
your *builds*.

---

## 9. Concrete next tasks, highest value first

### (a) Sweep the ≥99% near-miss band — the reliable grind
Current source headers contain 4 functions in 99.9–100%, 14 in 99.5–99.9%, and
27 in 99.0–99.5%, plus func_00135D00 at 100% with the linker-table blocker.
Regenerate rankings from each near-miss header; percentages are object scores.
Historical yield varies. This continuation recovered four verified matches.

Remaining top targets:
```
func_0016AE40  99.98  mwcc233 -O4,p -sdatathreshold 8
func_001F3620  99.92  mwcc233 -O4,p -sdatathreshold 8
func_001796C0  99.91  mwcc233 -O4,p -sdatathreshold 0
func_0010E8A8  99.89  eegcc   -O2
func_001662D0  99.83  mwcc233 -O4,p -sdatathreshold 8
```

Do not repeat the just-tested modulo compound-assignment variants on func_001F3620
or float-local/union variants on func_001796C0: no new match. For func_0016AE40,
the missing actor argument after its pad-mask test is a real source concern, but
explicitly forwarding it adds a reload; same-TU leaf experiments did not solve it.
See docs/FINDINGS.md. Auditing the 212 stale/intentional explicit fallback entries
among objdiff-100% units is another useful lane; trial each through the ELF gate.

**Skip `func_00169730` and `func_002134C0`** — both are genuine walls confirmed by ~31.5k and
~25k permuter iterations, and both were re-confirmed by the ELF test in §5. Do not re-grind them.

### (b) Family hunting — the step-change lever
idiom-29 produced seven improvements from one insight. Look for other idiom classes where
**every** user is a near-miss and **no** matched exemplar exists. That signature is the tell.
Mine `docs/NEARMISS.md` for repeated wall descriptions, then check the matched corpus.

### (c) The `lui`+`sw` scratchpad symbolization gap — diagnosed, NOT yet fixed
splat symbolizes `lui`+`addiu` pairs for `0x7000xxxx` scratchpad addresses but leaves
`lui`+`sw` pairs as raw constants, so the expected object cannot carry the relocation and
`func_001BF6B0` shows a permanent 4-row residual that is **not** a compiler wall — its linked
bytes are already correct.

**Caution, this has bitten twice.** A direct attempt to symbolize those pairs measured
99.9577 → **99.7531**, i.e. worse. The negative result is recorded in `build.py` beside
`_SPAD_SYMS`. Read that note before trying again, and measure the control through the same
harness.

### (d) The 15 remaining `INCLUDE_ASM` stubs — mostly not real
- **8** (`func_0025F0D4`…`func_0025F144`) are **data, not code** — a 36-byte-stride table whose
  words happen to decode as `bltzal`/`bgezal`, with zero references anywhere. splat false
  positives. Suppressing them means editing the segment config; not worth risking byte-identity
  for a counter.
- **`sub_D2_TADR_08x`** is an alias of the already-decompiled `0x00100A60`.
- **`func_001000B0`** is a genuine 2-instruction syscall thunk (it loads syscall number 0x23 and traps).
- **5** (`func_001CE860`, `anim_eval_skeleton`, `func_001F0A60`, `func_001F6FB0`,
  `sub__0000000000000000Inf`) were decoded to readable C last session, but that C **failed to
  compile** and the wave output has since been lost to temp cleanup. **These need re-deriving.**
  They are large VU0/COP2 functions; the C will never byte-match (see §10) — the goal is
  compiling, body-correct, documented C.

So exactly **one** genuinely undecoded function remains, and it is a kernel-call stub.

---

## 10. What can never byte-match, and why

VU0 macro mode (COP2), MMI 128-bit SIMD, and COP0 have **no C spelling under mwcc 2.3**. Those
functions are permanently near-misses; their C is a faithful scalar equivalent with the
divergence documented per file. Do not burn cycles trying to match them.

Related: PS2 floats are **not IEEE-754** (flush-to-zero, saturation, round-toward-zero). Note
the divergence; don't emulate it in the decomp.

One classification trap that cost a lot of time: **the EE byte-add of a register with zero is
mwcc's REGISTER MOVE idiom, not SIMD.** A classifier that reads it as SIMD will invent a "structural ceiling"
that does not exist. Stripping move idioms first collapsed "genuine SIMD" from 30 functions to 1.

---

## 11. Process rules that were learned the expensive way

1. **A label is not evidence — read the definition.** splat marks compiler output
   "handwritten"; thunk headers describe sound cues as "allocations"; twelve callers declare a
   no-argument function with arguments. Every significant win last session came from checking
   the definition against its description.
2. **Measure the control through the same harness before attributing anything.** One
   conversion read as "only 4 of 47 reached 100%" — a failure — until the baseline run showed 3
   of those 4 already matched *unconverted* and 12 others had improved. Same numbers, opposite
   conclusion.
3. **Validate a detector against a known positive before generalising.** A detector that finds
   nothing looks exactly like a clean codebase. One returned zero twice on the very function it
   was written for.
4. **Never size a class by raw pattern count** (20,019 bare `lui`s; 11 real sites).
5. **Run audits at least twice** — round 2 has caught regressions round 1 introduced.
6. **A wrong "fix" is worse than an honest unknown.** Mark port-side choices explicitly rather
   than dressing them as decoded.
7. **Watch for silent no-ops in your own tooling.** `integrate_nearmiss.py`'s REVERT path used
   to fail silently, leaving non-compiling C on disk — invisible downstream, because a
   near-miss is never compiled, so every check including the boot ELF still passed. It now
   verifies its own post-condition. Assume your other scripts have the same class of bug.

---

## 12. There is also a port repo

`/Users/abe/Documents/Extermination.nosync/extermination-port` — a native macOS/Windows/Linux
port (Cocoa+Metal / Win32+D3D12 / X11+Vulkan), clean-room, **zero third-party dependencies**,
no code from any emulator. It consumes this repo's readable C as ground truth.

**Current user priority:** continue PS2 byte matching and readable native C together,
prioritizing a faithful first-level opening and player interaction. The later explicit
checkpoint expands the earlier PS2-only selection. Do not trust existing port behavior
or decomp comments without checking original instructions/runtime evidence.

If the port: `CLAUDE.md` there has a "Verified backlog" section with per-item blockers, and
`tools/xref_decomp.py` joins port annotations to decomp status.


## 13. Latest integration safeguards (2026-09-22 UTC)

Commit d80e74d hardens integrate_nearmiss.py: remove old outputs before compiling,
require command success and fresh objects, reject duplicate function entries,
restore exact original source bytes (including NEARMISS) on build/validation
failure or interruption, and accept the documented direct-list JSON input.
Run `.venv/bin/python3 tools/match/test_integrate_nearmiss.py` (14 synthetic tests).
GNU as pads reference .text sections: known nonzero compiled sizes may be smaller
than the reference section. Exact section-size equality wrongly rejected four real
matches; oversize remains rejected. The linked-ELF/provenance gate is still required.
