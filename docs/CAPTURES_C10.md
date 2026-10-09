# C10 original captures

Original-game captures of the C10 lanes, recorded in the hidden, MCP-enabled
PCSX2 from the user's own disc and save states through
`tools/pcsx2_session.py`. Each lane has its own section; lanes run one at a
time under the PCSX2 lock (`build/.pcsx2.lock`). This file cites addresses,
frame numbers and what was recorded. It holds no original code, no disassembly
and no disc bytes, and it describes in-game text instead of quoting it. All
outputs are local and ignored (`build/`).

## AIM: aiming and firing in AREA11

Lane AIM, 2026-10-01. Follows the capture beat plan of the port's aim/fire
lane (port `docs/AIM_FIRE.md`, "Capture beat plan",
read only). Twelve beats, an opt-in group `aim` in `tools/route_capture.py`,
outputs in `build/aimfire/capture/<beat>/` (listed in
`build/aimfire/capture/README.md`).

### Start state

Every beat but one starts from the route snapshot
`build/s87/route/08_truck_crossing/state.p2s` (AREA11, north of the fence,
(371.5, 184.8, 361.3), in control). The player is armed there, as in every
route snapshot: the seven 0018A6B0 equipment nodes are allocated (pool records
0x20..0x26), the gun (flavour 0) at player +0x20 = 0x7AB730 and the knife
(flavour 4) at player +0x18 = 0x7AB440. The weapon globals are: magazine
D_00810C62 = 30, reserve D_00810CB4 = 60, fire mode D_00810C61 = 0 (single),
D_00810C73 = 0, equipment bytes D_00810CA4..CA7 = FF 05 00 07 (so D_00810CA4
is 0xFF and the attachment byte D_00810CA6 is 0), gun light D_00810D3C = 0.
The action masks at 0x70003B74..7E are 0x80, 0x40, 0x20, 0x10, 0x08, 0x02
(Square, Cross, Circle, Triangle, R1, R2 in the processed pad word). The
processed pad words D_00810E70 (held) and D_00810E74 (pressed) were confirmed
live: low byte Square 0x80, Cross 0x40, Circle 0x20, Triangle 0x10, R1 0x08,
R2 0x02; high byte Left 0x8000, Down 0x4000, Right 0x2000, Up 0x1000, L3
0x200. A pad state set before frame f first shows in the player at f + 3.

### What each beat records

Per frame (`trace.json` rows): the whole player record +0..+0x31F (`pl`) and
its weapon fields decoded (`w`: +4..+7, +1F0/+1F1, +200, +236, +26C/+270,
+274..+278, +27C, +28, +2A, +2E, +2D0 aim point, +2A0 hand matrix, +2F0,
+2F2, +2F4, +302, +317, +318); the globals (`g`: fire mode, magazine, C63,
reserve, C73, CA4..CB3, the light D3C, D_008106C6/C7/CC, the targets
D_008106E0/E4/E8, the pad words, the masks, the per-class list counts at
D_00275B5C..BAC); the status page record D_00810130..8F; the scratchpad words
0x70003A20, 0x70003190..31DF (the ray query block), 0x70003600, 0x700036A0..EF,
0x700038A0..CF and the bytes 3B8C..3B93; the camera blocks; the gun node
(+0..+3F, +A0 muzzle, +B0, +C0 barrel direction, +1F0..+21F), the knife node,
the security gun 0x7A6AD0 (+0..+3F, +1F0..+22F) and its cable 0x7A6DC0
(+0..+3F, +98..+9F); area 11's taken-bit row D_008109C0; and every pool record
(D_007A5640, 0x100 x 0x2F0) whose header +0..+17 differs from row 0, with its
+18..+3F and +A0..+DF (spawned effects, shot nodes, impact markers). Row 0
also lists every allocated record (`pool0`). Each beat ends with a snapshot.

| Beat | Frames | Inputs and result |
|---|---|---|
| aim_00_r1_hold | 146 | R1 at f10. f13: +5 0x1D, +1F0 0x31 (001607D0), clip 0x110 (draw), camera byte D_008101E6 = 1; f29: +6 2, clip 0x112, +2F2 1. R1 released at f89: +6 0x63 (f92), 0x64, 0x65 (f97, clip 0x111, the holster), 0x66, 0x6E (f115), idle at f116. |
| aim_01_r2_hold | 154 | The same with R2: +5 0x1E, +1F0 0x32, camera byte 2. The ramp-out is 8 frames (0x63 at f92, 0x65 at f101), idle at f124. |
| aim_02_r1_r2_both | 368 | R2 added to R1 switches the stance to 0x1E / 0x32 at once (+6 stays 2, +318 pulses 1); releasing R2 goes back to 0x1D; releasing R1 while R2 is held keeps 0x1E; R1 and R2 pressed on one frame from idle enter 0x1E (R2 is tested first). |
| aim_03_single_fire | 460 | Fire mode single. R1: three Circle presses fire at f43, f59, f75 (00170A60 states 0x0A then 0x0B; magazine and reserve drop together; +274 latch, gun +2E event); a 90-frame hold fires once (f91). R2: two presses and a 60-frame hold fire three rounds (f277, f293, f309). Seven rounds, 23 / 53 left. |
| aim_04_world_hit | 1404 | Walk to (381, 312), R1, manual aim with the left stick (0017ABA0 steers +278 / +27C; aim_at in the tool) and one round per target. Impact markers (class 1, behaviour 0018ABA0, +2E impact code, +B0 point, +C0 normal): ground (381.01, 184.81, 299.44) code 0, normal up; concrete pillar (386.96, 204.97, 290.27) code 0x201; the fence aim did not converge (yaw limit) and the round landed beyond the fence at (346.21, 192.18, 241.9), code 0x201; the round aimed high above the fence spawned no marker (a miss). |
| aim_05_burst_fire | 471 | START opens the status screen; stick right with Cross enters SPR4, stick right with Cross enters the selector (00217FA0). The list shows two entries, single fire and 3-round burst; the third (full auto) is only appended when D_00810C73 is set, which it is not. Every menu press is repeated until its effect shows in the page record (a press can land while the menu is busy). Down, Cross (a yes/no prompt about changing the firing mode), Left (yes), Cross: D_00810C61 = 1; Triangle closes. R1: a 2-frame press fires one round (f234); each 60-frame hold fires three rounds 6 frames apart (f259/265/271, f339/345/351, states 0x14..0x16) and then waits in 0x17 for the release. |
| aim_06_reload_partial | 417 | Five rounds (25 / 55). L3: +6 3, +1F0 0x33, clip 0x11B; the magazine is 30 at once and the reserve keeps 55 (0017B300 mode 2). L3 again with a full magazine does nothing. Two rounds (28 / 53), L3, R1 released 12 frames into the reload: the reload runs to its end, then the stance goes straight to 0x65 (holster). |
| aim_07_reload_empty | 1231 | 60 single rounds, 16 frames apart. The 30th empties the magazine (f507); f519 the automatic reload (+6 3, clip 0x11B, magazine 30 from reserve 30). The 60th empties both (f1066). Two dry presses: no round, the gun's +2E event still cycles 0, 1. L3 with nothing left does nothing. |
| aim_08_light_holster | 316 | R1, Square: D_00810D3C = 1 and the lamp D_008106C7 = 1 (0017A970 with CA6 = 0). Cross (0017AAD0) does nothing: no attachment. Holster: the lamp goes to 0, D3C stays 1. R2 draw: the lamp is 1 again when the stance loop starts (0016F530). Square: both 0. Holster. |
| aim_09_melee | 267 | Circle from idle: +5 0x21, +1F0 0x36, clip 0x10B; the knife node's +0 goes 2, 1, 2 around the hit window; back to idle at f51. A Circle chain gives the three-hit combo (clips 0x10B, 0x10C, 0x10D; +2E latches the next hit). Square from idle: +5 0x22, +1F0 0x37, clip 0x10E. Melee is available in AREA11. |
| aim_10_cable_shots | 1441 | Walk to (387, 318) facing the pillar under the security gun. R1 and then R2, each aimed at the cable top, its bone-1 joint and the strand (aim_at; R1 joint and R2 top did not converge and fired anyway), one round each. All six rounds end on the pillar's north face (z 290.27, code 0x201, at the cable's x 387.06..387.12); the cable keeps +36 = 0, lifecycle 1; the gun stays in 0x64; the targets D_008106E0..E8 stay 0 in both stances. |
| aim_11_cable_melee | 585 | From aim_10's end. Walk to the strand's foot (386.8, 184.8, 294.95) and press Circle. f249: cable +36 = 1; f250: cable lifecycle 2, gun lifecycle 2 with +21C = 90, the player's melee goes to +6 0x50 / 0x51 (hit confirm), the effect nodes 0021AAC0 and 0021A500 spawn; f251: taken bit 0x50 set in area 11's row; f309: cable lifecycle 3, freed at f310 (its record is reused later). 300 idle frames follow. |

### Findings

- **Fire modes offered in AREA11:** the sub-weapon byte +275 stays 0 in every
  beat. With D_00810CA4 = 0xFF and D_00810CA6 = 0, Square only toggles the gun
  light and Cross (0017AAD0) changes nothing, so the other five fire machines
  (+275 = 1..5) are not reachable through the controls here. The selector
  offers single and 3-round burst; full auto is not offered (D_00810C73 = 0).
  Single: one round per press, a held trigger fires once. Burst: a short press
  fires one round, a hold fires three and waits for the release.
- **Targets:** D_008106E0..E8 stayed 0 in every frame of every beat, in both
  stances, including in front of the cable.
- **The cable:** rounds aimed at it land on the pillar face behind it and do
  not hit it. A light melee at its foot does: the gun goes to lifecycle 2
  and taken bit 0x50 is set (which the gun's lifecycle 0 reads on a later
  visit, per SECURITY_GUN.md; no later visit was captured). This answers the open question of
  decomp `docs/CURIOSITIES.md` entry 26 (updated there).
- **R1 / R2:** R1 enters stance 0x1D (camera byte 1), R2 stance 0x1E (camera
  byte 2); R2 wins when both are pressed; the switch between the two keeps
  the stance loop running.
- **Page-module waits (port chain step AIMCAP, 2026-10-02):** aim_05's rows
  where the page record waits on a module are SPR4's 0x2C f50..f77 (28
  rows), the SELECTOR's 0x31 f82..f99 (18) and the SPR4 reload f154..f176
  (23). aim_09's camera is identical on all 267 rows, so its end
  snapshot's clip matrix holds for every row (the knife trail's check, port
  `docs/AIM_FIRE.md` section 11.3).

### Replay

Decomp `.venv` python, repo root, PCSX2 lock held:

    .venv/bin/python tools/route_capture.py run --beats aim       # all 12 beats
    .venv/bin/python tools/route_capture.py run --beats aim_05_burst_fire
    .venv/bin/python tools/route_capture.py events --beats aim    # change logs
    .venv/bin/python tools/route_capture.py verify --beats aim    # snapshots resume

Each beat is closed loop (predicates on the recorded fields, not frame
counts), so it reaches the same goals from its source snapshot. Each end
snapshot was checked to resume (route_capture's `resumes`).

### Census

Pass `AIM` (`.venv/bin/python tools/route_census.py run --segments aim --pass
AIM`, then `aim-delta --passes AIM`): boot functions plus the AREA11 overlay
armed, one-shot, re-armed per beat; no beat reaches AREA01. Runs:
`build/s87/census/runs/AIM/<beat>.json`; log
`build/aimfire/capture/census_run_AIM.log`; delta
**`build/aimfire/capture/census_delta.json`** (against
`build/s87/census/classified.json`, the first-level census of 1,184 functions).

- All 12 beats replayed closed loop to completion. Every replay's outcome
  (rounds fired, end magazine / reserve / fire mode / light, the cable, the
  gun, the taken row) equals the recording; the start phase differs by up to
  7 frames (pcsx2_session lets the loaded state run until Pine answers), so
  6 replays (aim_01, 02, 05, 07, 08, 10) are row-identical to the recording
  and the other 6 are shifted by their start offset.
  The first census of aim_05 failed three times (START ignored at that start
  phase); the menu helper now repeats each press until it takes effect, the
  beat was recaptured and its census replay is row-identical (472 / 472).
- **738 functions ran; 624 are already in the first-level census; 114 are new**
  (20,583 instructions, all boot ELF; no new AREA11 overlay function). Decomp
  status of the new ones: 61 byte-matched C, 40 NEARMISS, 8 inline asm, 3
  word asm, 1 C linked from asm, 1 undecompiled. The 624 known ones are 169
  live, 258 verified-unbound, 5 unverified, 4 legacy stand-ins, 7 missing,
  181 boundary.
- New functions by the beat that first ran them:
  - aim_00 (24): the R1 stance and its workers: 0016F530, 0016F5D0,
    0016FCF0, 00170A60, 0017A130, 0017A0B0, 00179BC0, 00179CA0, 0017ABA0,
    0017B300, the laser lock 00185A10, the aim drawers 001854E0 / 00185760,
    001E2800, 001E2BA0 and helpers (00102870, 0011E860, 0018F870, 00197490,
    00197740, 00197870, 00197D20, 001A6AD0, 001B5DC0).
  - aim_01 (10): 001703E0 (R2 stance), 0018C850, 0018C920, 001912B0,
    00198050, 00198240, 00198440, 00198650, 001999C0, 001DB800.
  - aim_03 (21): the shot: 0017A8B0, 001861C0, 00187CC0, 001860A0, the impact
    marker 0018ABA0, the shot node 001F5040, and 001B1C60, 001C63D0, 001CA3B0,
    001CA4D0, 001CD940, 001D80E0, 001EBA20, 001F02C0, 001F2F90, 001F3340,
    001F3620, 001F3E30, 001F4010, 001F4F40, 001F4F90.
  - aim_04 (6): 00183AC0, 001A9C40, 001EACF0, 001EBBB0, 001F00A0, 001F0460.
  - aim_05 (30): the status screen's SPR4 page and selector: 00211970,
    002121A0, 002125B0, 00212B60, 00212F30, 00217FA0, 0020BF20, 0020D930,
    0020AC70, 00209DF0, 00209860 and 00100130, 001AF7C0, 001AFF10, 001B0000,
    001B62C0, 001C69A0, 001CB4F0, 001CB580, 001D66A0, 002082B0, 00208750,
    00208AB0, 00208AD0, 0020E1E0, 0020E250, 0020E3A0, 0020E460, 0020E6F0,
    0020EC80.
  - aim_06 (1): 0016F600.
  - aim_08 (4): 0017A970, 0017AAD0, 00187690, 00187780.
  - aim_09 (11): melee 001735C0, 00173E60, 00173DD0, the knife probe 0019B2C0,
    001AA7A0, 001AA840, 001EFF10, 001F1550, 001F15F0, 001F18C0, 00102990.
  - aim_11 (7): the cable reaction: 001EFE00, 001EFEB0, 0021AAC0, 0021A500,
    the strip packet builder 001CE860, 0018A180, 001EAB50.
  - aim_02, aim_07 and aim_10 ran no function the earlier beats had not.

### Rules kept

PCSX2 ran hidden, one session at a time, under the lock for the whole lane, and
no PCSX2 process was left running. No save-state slot 01..15 was written
(snapshots go through a free slot and are moved out). The renderer setting was
not changed (no framebuffer pixels were needed). No memory was written; no beat
reaches a save point. As C7 recorded, PCSX2 updates the memory cards' mtimes
on every exit; their SHA-1s are in `build/aimfire/capture/memcard_hashes.txt`.

## EXIT: the level exit and the transition into AREA01 (beat 15)

Lane EXIT, 2026-10-01. Covers route beat 15 end to end: the fan crossing that
triggers Roger's departure (0x828A10), the departure walk, the departure movie,
the area-change request, the AREA01 load and the AREA01 arrival up to the first
frame of player control (plus 60 idle frames). Opt-in group `exit` in
`tools/route_capture.py`, outputs in `build/c10/exit/<beat>/`.

### Why it was re-recorded

The earlier recording (`build/s87/route/15_level_exit/`, the port's
`docs/FIRST_LEVEL_EXIT.md`) is closed loop and per frame over the whole exit,
but its rows keep only the decoded route fields plus the exit fields (no whole
player record, no Roger node beyond the route's owner fields, no globals block,
no pool), its census armed only the AREA11 overlay (so the AREA01 overlay
functions of the arrival were not counted), and its replay matched 0 of 802
rows because the replay started one frame off the recording and gave its
inputs one frame later. Beat 15 and its snapshot are left untouched (the a01
beats start from it); the group below re-records the same route.

### Beats

| Beat | Source | Frames | Counters | What happens |
|---|---|---|---|---|
| exit_00_departure | 14_roger_encounter | 434 | 15761..16195 | Neutral pad up to counter 15800 (the pin, below). Walk toward (331, 177) and (329.5, 172) at half stick, stop outside fan r2's hit band, wait for its slow window (phase 1, timer 55 or more: counter 16065), walk toward (329.5, 150). Counter 16105: Z < 156, D_008107D8 = 0x81 and Roger r8 starts script 0x828A10 in the same frame (stick released). 16106: 3B8D = 2. 16107: player action 0x41. The script walks the player to (330.0, 289.0, 127.0), reached at 16194. Ends at 16195, the first frame with Roger's script record at 0x828AD0 (eight frames before the movie frame). 50 pad changes, all stick, no buttons. |
| exit_01_movie_arrival | exit_00_departure | 366 | 16197..16563 | No input. 16200: record 0x828B50 (op 0F). 16203: the movie frame (D_00275C78 = 1; the game's vsync counter 0x810E90 advances 4752 in that frame; 79 s of host time). 16206: the script ends, D_00810758[0] = 0xFF, 3B8D = 3, request bytes B5..B8 = 01 00 04 01, Roger r8 lifecycle 3. 16207: area bytes D_00810700..702 = 01 00 04. 16208: D_00275BD8 = 1 (load pending) until 16420. 16214: the resident overlay header changes from id 9 to id 2. **16503: first frame of control in AREA01** at (41.0, 0.0, -565.6), yaw -1.43411 (AREA01 sub 0, spawn entry 4). Then 60 idle frames. |

The end snapshot of exit_01 shows the arrival view with the AREA01 area title
card (the name of the underground tunnel area) over it.

Files per beat: `trace.json` (meta, the input script `inputs` with frame
indices, `marks` = the first frame of each event above, per-frame `rows`),
`state.p2s` (end snapshot), `eeMemory.bin`, `gs.bin`, `scratchpad.bin`,
`original.png`, `snapshot.json`. Both end snapshots were checked to resume
(route_capture's `resumes`, and again with `verify`). Change logs:
`build/c10/exit/events.txt`.

### What each row records

The route row (decode: player position, hip, yaw, action, clip, clock, ground,
3B8C..3B93, both camera blocks, the request block D_008106B0, fade 0x28A9A0,
screen 0x28A8D0, message machine, status UI, power, story bytes, D_008107D8..,
item counts, charge, area, and the AREA11 owner nodes' +0..+F, +B0, +1F0, +2DC)
plus beat 15's exit fields (fan records [1] and [2]: phase, timer, flags, spin,
rot z; D_00810758; D_00810700..703; task slots 0..2 at 0x28A750; D_00275BD8;
D_00282157; the overlay header; D_00275C78; D_00821058) plus: the whole player
record +0..+0x31F (`pl`); Roger r8 (0x7A8830) and attachment r9 (0x7A8B20)
+0..+3F, +A0..+DF, +1F0..+22F, +2C0..+2EF and Roger's script record pointer;
the globals 0x810600..0x81085F; the scratchpad 0x70003B40..9F; the processed
pad words D_00810E70 / E74; the vsync counter D_00810E90; the overlay header
0x823500..0F; and, as the AIM rows do, the D_007A5640 pool record headers that
differ from row 0 (row 0 lists every live one) with +18..+3F and +A0..+DF of
the live ones (Roger and r9 being freed, the AREA01 records being placed).

### Start pin and replay results

pcsx2_session lets a loaded state run freely until Pine answers, so a replay
of exit_00 from the beat-14 snapshot started anywhere from counter 15760 to
15770 in this lane. exit_00 therefore idles with a neutral pad until counter
15800 (`EXIT_PIN`) before its first input, so every recording and replay
gives the same input at the same main-loop counter. route_census now also
compares rows by counter (`trace_vs_recorded_by_counter`, pool headers rebuilt
to absolute values) and inputs by counter, and keeps each EXIT replay's own
rows in `build/s87/census/runs/<pass>/traces/`.

Two census replays (passes EXIT and EXITB) against the recording:

- exit_00: inputs identical by counter in both; all 435 rows paired by counter
  agree in every field except the vsync counter (a constant offset of -4 after
  the first rows) and the stream gate byte D_00282157 (52 rows, the streamed
  audio's read timing); end snapshot: player and owner spans identical, one
  globals word (0x810D98) differs. Same event counters (window 16065,
  crossing 16105, walk end 16194, record 0x828AD0 16195).
- exit_01: same counters for the movie (16203), the request (16206), the area
  bytes (16207), the load start (16208) and the overlay swap (16214); the
  movie frame's vsync advance differs (4752 recorded, 4754 in EXITB) and so
  does the load length: load done 16420 recorded, 16419 in EXITB; first
  control **16503 recorded, 16502 in EXIT and EXITB**. The first lane run
  (before the pin, discarded) gave 16503 recorded and 16498 in its census, and
  beat 15 had 16502. So the disc load after the movie varies by up to 5 frames
  between runs (host timing inside the emulator's disc and movie path, not
  game logic); everything from the arrival on is the same sequence shifted by
  that offset, and the end snapshots agree in the player and owner spans (one
  globals word, 0x810D98, differs). A comparison of the original with the port
  has to align on the arrival frame (the first control frame in AREA01), not
  on a frame count across the load.

### Census

`route_census.py run --segments exit --pass EXIT` (and `--pass EXITB`): the
exit segments use `OverlayCensusSession`, which arms the boot functions plus
the resident overlay's candidates and, at the first frame boundary where the
overlay id changes (counter 16214), removes the 23 AREA11 overlay breakpoints
still armed and arms the 62 AREA01 splat pieces; overlay hits carry the
resident id, AREA01 pieces are regrouped into real functions. Both passes
completed with no unexpected pause and no unattributed hit. Runs:
`build/s87/census/runs/{EXIT,EXITB}/exit_*.json`; logs
`build/c10/exit/census_run_EXIT.log`, `census_run_EXITB.log`; delta
**`build/c10/exit/census_delta.json`** (`route_census.py c10-exit-delta
--passes EXIT,EXITB`, against `build/s87/census/classified.json`, the
first-level census of 1,184 functions, keyed by overlay and address).

- **1,028 functions ran; 953 are already in the first-level census; 75 are
  new** (12,134 instructions): 61 boot, 2 AREA11 overlay, 12 AREA01 overlay.
  Known ones by port status: 180 live, 309 verified-unbound, 4 unverified,
  1 legacy stand-in, 7 missing, 452 boundary.
- By phase: 6 before the area-change request (departure), 4 between the
  request and the arrival placement 001B07C0 (change and load), 65 from the
  arrival on (53 boot, 12 AREA01 overlay).
- Decomp status of the 75: 28 byte-matched C, 25 NEARMISS, 13 overlay C,
  3 inline asm, 2 word asm, 2 C linked from asm, 1 undecompiled overlay piece
  (0x823C80), 1 without a source file (0x001C0004).
- Against beat 15's `exit_delta.json`: all 63 functions it listed are new here
  too, and no other boot or AREA11 function is; the 12 AREA01 overlay
  functions are what the earlier census could not see.
- Departure (6): 00194D10, 0022FCA0, 00230000 (first at counter 16086, during
  the walk under the fan), the AREA11 overlay pieces 0x823C40 and 0x823C80
  (counter 16105, the crossing frame; 0x823C40 runs again in exit_01, the
  piece at 0x823C80 does not, although 001B0C60's return address 0x823CA8 lies
  in its span), and 001B7A30 (op 0F, counter 16201).
- Change and load (4): 001B0C00, 001B0C60, 001FAD70 (counter 16206) and
  001195A8 (during the load).
- AREA01 overlay (12), runtime addresses: 0x823A50 (counter 16502, the
  arrival frame), 0x823580, 0x825350, 0x825740, 0x8261A0, 0x826200, 0x826440,
  0x8267C0, 0x826CF0, 0x826D40, 0x828850 (the frame after), 0x8254B0 (one
  frame later); all overlay C in `src/overlays/AREA01`.
- The 53 boot functions of the arrival are the list of FIRST_LEVEL_EXIT.md
  section 5 (same addresses).

### Replay

Decomp `.venv` python, repo root, PCSX2 lock held:

    .venv/bin/python tools/route_capture.py run --beats exit        # both beats (exit_01 ~2-5 min)
    .venv/bin/python tools/route_capture.py run --beats exit_01     # from the exit_00 snapshot
    .venv/bin/python tools/route_capture.py events --beats exit
    .venv/bin/python tools/route_capture.py verify --beats exit     # snapshots resume
    .venv/bin/python tools/route_census.py run --segments exit --pass EXIT
    .venv/bin/python tools/route_census.py c10-exit-delta --passes EXIT,EXITB

The lane's runner scripts are `build/c10/exit/lane.sh` and `census_b.sh`.

### Tool fix

`route_capture.wait_for_free_emulator` and `pcsx2_session._emulator_pids`
matched any process whose command line contained the emulator path, including
another lane's shell waiting for the lock; the capture then waited for a
"running emulator" that was a shell. Both patterns are now anchored to the
start of the command line, so only the emulator binary itself matches.

### Rules kept

PCSX2 ran hidden, one session at a time, under the lock for each run of the
lane (three acquisitions; the first run was stopped by the tool fix above
after its captures and redone), and no PCSX2 process was left running. No
save-state slot 01..15 was written (the sstates listing is unchanged). The
renderer setting was not changed. No memory was written; no beat reaches a
save point. The memory cards' SHA-1s are unchanged across the lane
(`build/c10/exit/memcard_hashes.txt`).

## DAMAGE: damage, death, the game-over screen and what follows in AREA11

Lane DAMAGE, 2026-10-01. Everything that can hurt the player in the first
level, found from the code (decomp C of 0021C440, 001A8660, the flame
0x8235F0 and its callback 0x823580; the port's FAN_ORIGINAL.md,
PLAYER_FALL.md, SECURITY_GUN.md, CRATES_DRUMS_ORIGINAL.md), the world graph
and the AREA11 grid, then recorded. Nine beats, opt-in group `dmg` in
`tools/route_capture.py`, outputs in `build/c10/damage/<beat>/`; change logs in `build/c10/damage/events.txt`,
capture logs `capture_run.log` and `capture_run_0405.log` there.

### What can hurt the player in AREA11

| Source | Code | In AREA11 | Captured |
|---|---|---|---|
| The flame (r7, node 0x7A8540 at (452.3, 278.6, 277.6), on the pipe end next to beat 11's release point) | class-1 sphere pass 001A8660 over the hazard list D_00275BA0: callback 0x823580 (effect 0x80000027 on the player, +0x0F = 0xC, the flame's cooldown +0x210 = 60), +0x224 = D_0024A740[kind +0x0D = 0] = 5.0, +0 = 3, +0x70 = direction | yes, 5 per contact; contact at about 9.6 from the flame's x/z | dmg_00..02 |
| Fan r2 (0x827630, record [2] at (329.3, 309.3, 160.8)) | fast arm only (spin +0x38 >= 0.0349), player X 318..340, Y 280..320, Z 156..166.5 (Z < 156 is the exit): +0x224 = 5.0, +0x0F = 6, +0 = 3, +0x70 = (0, 0, 1, 1) | yes, 5 per hit | dmg_08 |
| Landing hit (0017C580) | drop d = +B4 - +2F4 <= -50: rumble, +0x224 = 5.0, 0021C350, landing +6 = 3 (00163E90) | yes; walking toward a high edge, the player stops short of it (observed), but a walking jump off the plateau falls short into the crevice | dmg_06 |
| Heavy landing (0017C580) | d < -104: +0 = 2, health 0, clip 0x2B, then 0021D2E0 | only from the two tower tops (289.75) onto the ground (184.8): d = -104.95. Not captured: the walk stopped the player short of every high edge tried (cage roof north edge at z 233.3, north block south edge at z 199.3, east tower south edge at z 199.25, west tower south edge at z 199.5) and Cross there started no running jump (+5 never became 6) | no |
| The attribute-0x5D plane (70 grid nodes, y 100.8, x 80..571, z 360..658: the truck pit and the south) | the fall state's ground test: 0021D250 (+0 = 2, health 0, +5 = 0x16), 0021D2E0 | yes: ride the truck down (roof y 154.6), step off its roof | dmg_07 |
| Kill plane y < -200 (0015D460) | state 1 only | not reached: below the play area is only the 0x5D plane, which kills first; the dead body then falls past -200 in state 2 (y -636 when the game-over screen comes up) | no (unreachable alive) |
| Security gun 0x825940 | 5.0 per shot | dormant all first visit (story flag 0x30 clear; CURIOSITIES 26) | no (not active) |
| Crates 001551B0, drums 00156620 | never read or write the player | no | - |
| Infection | +0x22C only from sphere kinds 3/4 (the flame is kind 0) and class-3 pads (none in AREA11); the area drain needs D_008106C8 & 0x60 (AREA11: 0x20081910) | nothing raises it: +0x228 = 0 and +0x234 = 0 in every frame of every beat | - |

### Beats

Every beat idles with a neutral pad up to its pin (the source snapshot's
main-loop counter + 30; + 45 from dmg_03's title-menu snapshot, where a
replay once started 31 frames late), then drives closed loop (predicates on
the recorded fields). Frame numbers are trace frames; counters in brackets.

| Beat | Source | Frames | Counters | What happens |
|---|---|---|---|---|
| dmg_00_flame_hit | 11_crevice_prompt | 197 | 12786..12983 | Walk at the flame. f67 (12853): +0x224 = 5.0, +0 = 3, +0x0F = 0xC; f68: health 95, +0x0F cleared, +1F1 = 4, flinch +4 2 +5 0, +1F0 0x3E, rumble (small motor 0xC0); clips 0x1E/0x20. Walk back to the release point during the 60-frame protection (+0x20E counts after the flinch; the knock-back alone does not always clear the flame's reach). |
| dmg_01_flame_low_health | dmg_00 | 1621 | 12984..14605 | Twelve more contacts, every 97 or 112 frames while standing in reach: 95 to 35. f1222 (14206): health 35, +0x235 bit 0 set. Retreat, 300 idle frames: the low-health heartbeat (0015D000) is the pad's small motor at 0xD0 for 3 frames every 121 frames; no other effect. |
| dmg_02_flame_death | dmg_01 | 1305 | 14607..15912 | Contacts to 10 (heartbeat now 0xE0 every 61 frames), 150 idle frames out of reach, two more contacts. f889 (15496): health 0, +4 2 +5 1 (death reaction), +1F0 0x40, clip 0x2A, D_008106B9 = 1 in the same frame; big-motor rumble (0xEE) later; the death terminal 0021D2E0 spawns a pool record with behaviour 0x1F77B0. f1143 (15750): fade-out; f1207 (15814): the game-over wait 001AD4E0 (task slot 0 state 3 sub 2), screen module load (D_00275BD8 = 1 for 22 frames), f1232 the game-over screen fades in, f1295 (15902) shown, hold timer at 177. |
| dmg_03_gameover_timeout | dmg_02 | 354 | 15924..16278 | No input. The 240-frame hold counts from the fade-in start and runs out at 16079: fade-out; 16144 the continue machine 001AC070 replaces the gameplay task (D_00275BDC = 1, from death); 16186 001AC480 starts the title menu (screen module 1) with the cursor on its second entry; 16248 the menu takes input (idle timer 1200). The menu has three entries (start a new game, load a saved game, options); none continues. |
| dmg_04_new_game | dmg_03 | 1847 | 16289..18136 | Up (cursor 0, the new-game entry), Cross: fade-out, state 4 reinstalls the gameplay task 001ACEC0 (16409, D_00275BE0 = 0), the new-game path (state 1) resets the progress (battery item, panel power, truck byte back to 0; health 100), AREA11 loads (state 3 sub 5), the opening plays and control returns at 18076 at (250.8, 229.9, 209.0), yaw 0.61087: the same spot and the same opening length as the startup census (S2). 60 idle frames. |
| dmg_05_load_screen | dmg_03 | 382 | 16295..16677 | Cross on the load entry: fade-out, state 5 (16395, 00225A00 / 00225AC0, D_00275BE0 = 1); the load screen asks which memory card slot to use. The beat stops there: Triangle (the screen's exit) returns to the title menu (state 2 again). No slot was chosen. |
| dmg_06_crevice_fall | 11_crevice_prompt | 653 | 12787..13440 | Beat 12's walk to the plateau edge, facing north, stick at 0.45, Cross at z <= 252 (where the walk stops short of the edge): a walking jump (+5 6, +1F0 0x0C, take-off +2F4 = 269.65) falls short of the north block into the crevice. f482 (13269): landing at 184.8, d = -74.8 (hip): health 95, landing +6 = 3, small motor 0xD0; control back, protection out, 30 idle. |
| dmg_07_pit_fall | 07_truck_preview | 639 | 7701..8340 | Walk onto the truck and stay: its fall carries the player down to the roof at y 154.6 (D_00810792 = 0xFF). Walk south off the roof: fall state +5 5; f355 (8056): ground attribute 0x5D, health 0, +4 2 +5 0x16, +1F0 0xE, D_008106B9 = 1, big-motor rumble; the body falls through the plane (y -636 at the end). f477 fade-out, f541 game-over wait, f629 (8330) the game-over screen. |
| dmg_08_fan_hit | 14_roger_encounter | 285 | 15760..16045 | Pinned past 15800 (the player is held until then), walk to the fan pair, step into fan r2's band: f142 (15902): +0x224 = 5.0, +0x0F = 6, +0 = 3 at Z 166.4; f143: health 95, +5 0x11 (0021E9C0), +0x0F 0x86; knocked back to Z 173.4. Protection out, 30 idle; never below Z 156. |

The end snapshots of dmg_02 and dmg_07 show the game-over screen; dmg_03 and
dmg_05 the title menu; the load screen of dmg_05 shows the slot choice.

Files per beat: `trace.json` (meta, `inputs`, `marks`, `hit_frames`,
`health_path`, `end`, per-frame `rows`), `state.p2s` (end snapshot),
`eeMemory.bin`, `gs.bin`, `scratchpad.bin`, `original.png`,
`snapshot.json`. Every end snapshot was checked to resume (route_capture's
`resumes`).

### What each row records

The route row (decode: position, hip, yaw, action, clip, clock, ground,
3B8C..3B93, both camera blocks, the request block, fade 0x28A9A0, screen,
message machine, status UI, power, story bytes, D_008107D8.., item counts,
charge, area, the AREA11 owner nodes) plus beat 15's exit fields (fans [1]
and [2], D_00810758, D_00810700..703, task slots 0..2 at 0x28A750,
D_00275BD8, D_00282157, the overlay header, the movie bytes) plus: the whole
player record +0..+0x31F (`pl`) with its damage fields decoded (`vit`:
health +0x220, pending +0x224, infection +0x228, pending +0x22C, +0, +0x0F,
+0x0D, +4..+7, +1F0/+1F1, +0x20E, +0x234, +0x235, +0x236, +0x23A, +0x23B,
+0x25C, +0x25F, +0x28, +0x302, +0xB4, +0x2F4, +0x70); the globals
0x810600..0x810D3F (`glob`; decoded `gv`: D_008106B9, D_008106C8,
D_008106CE/CF, D_008106F1, D_00810707, D_0081070A, D_0081083C, the display
vitals 0x810858/5C); the pad block 0x810E40..0x810E9F (`rumble`: +0x16,
+0x18, +0x19, +0x28; the processed pad words; the vsync counter);
D_00275BD0..EF (`tasks`); the per-class lists 0x275B54..BB; the scratchpad
0x70003B40..9F, 0x70003A20, 0x700038A0..BF; the flame node (+0..+3F,
+A0..+BF, +1F0..+21F, cooldown +0x210); and the D_007A5640 pool headers that
differ from row 0 with +18..+3F / +A0..+DF of the live ones.

### Replay

Decomp `.venv` python, repo root, PCSX2 lock held:

    .venv/bin/python tools/route_capture.py run --beats dmg            # all 9 beats, in order
    .venv/bin/python tools/route_capture.py run --beats dmg_07_pit_fall
    .venv/bin/python tools/route_capture.py events --beats dmg         # change logs
    .venv/bin/python tools/route_capture.py verify --beats dmg         # snapshots resume
    .venv/bin/python tools/route_census.py run --segments dmg --pass DMG
    .venv/bin/python tools/route_census.py dmg-delta --passes DMG,DMGB

The census replays against the recordings, rows paired by main-loop
counter. route_census keeps a replay's own rows in
`build/s87/census/runs/<pass>/traces/`: DMGB has them for all nine beats;
DMG only for dmg_04 and dmg_05 (trace saving was added before their 03:30
re-run), and its other seven replays keep only the by-counter summary
(`trace_vs_recorded_by_counter`) in their run JSONs.


- Two passes (DMG and DMGB), every beat completed with no unexpected pause.
  Inputs by counter equal the recording in every replay except dmg_01's
  (one input, below). In all 18 replays the hits land on the recorded
  counters, and the health path, the low-health latch, the death and the
  end state (health, infection, +0, +4/+5, +0x235, D_008106B9, task slot 0,
  fade, area, and the player span of the end snapshot) equal the
  recording. The events after a disc load come one counter later than
  recorded, in both passes: the game-over screen in dmg_02 and first
  control after the new game in dmg_04 (below); the title-menu events of
  dmg_03 are on the recorded counters.
- Row-identical by counter in every field: dmg_00 (198/198 in DMG, 196/196
  in DMGB) and dmg_05 (383/383 in both). dmg_07 and dmg_08 differ only in
  the vsync counter and the stream gate byte D_00282157.
- dmg_06: besides vsync and D_00282157, the processed pad held word differs
  in one row in both passes (counter 13171: 0x40 recorded, 0 in the
  replays), a pad-pickup timing difference. Inputs by counter are equal,
  and the jump and the landing hit (13269) land on the recorded counters.
- dmg_02: the game-over screen module load completes one frame later than
  recorded in both passes (the task slot and D_00275BD8 bytes first differ
  at 15838), so everything after it is one counter late: the fade differs
  in 64 rows (15839..15902), the task slot bytes in 83 rows
  (15823..15912), and the game-over screen appears at 15903 instead of
  15902. The death (15496), the fade-out (15750) and the game-over wait
  (15814) are on the recorded counters. This is the same kind of disc-load
  shift as dmg_04's.
- dmg_03 and dmg_04 reach the same events at the same counters except
  across disc loads (dmg_04's first control comes one counter later than
  recorded in both passes, as the EXIT lane saw for the AREA01 load).
- dmg_01 differs twice from the recording, in the same way in both passes,
  so the two replays agree with each other and the recording is the
  outlier. First, the game's processed pad word takes the recorded stick
  change two frames late: held is 0x8000 from counter 13103 in the
  recording but from 13105 in the replays (pressed at 13103 recorded,
  13105 replayed). Second, the closed loop sends one stick input, (lx 4,
  ly 99), at counter 14079 instead of the recorded 14080. As a result the
  position and hip differ from the recording by small amounts (at most
  about 0.005 units) in 1100 rows (13104..14203), the player record in
  1150 rows and its decoded damage fields in 990 rows (13103..14204), and
  the yaw in 413 rows. Every hit (13064, 13161, ... 14206), the health
  path 95 to 35 and the +0x235 latch (14206) land on the recorded
  counters.
- DMGB's dmg_02 ran 36 more low-memory functions (0x10B800..0x124F58) in the
  frame its fade-out started; all of them are in the first-level census
  already, so the delta is the same with or without DMGB.
- The pins: dmg_05's first DMG replay started 31 frames after its source
  snapshot and refused to run past the then 30-frame pin; the beats from the
  title menu (dmg_04, dmg_05) now pin at + 45 and were re-recorded and
  censused again (DMG) before DMGB. Earlier, unpinned recordings (replays
  shifted by up to 22 frames) were superseded and deleted.

### Census

Passes DMG and DMGB (`route_census.py run --segments dmg --pass DMG`, the
same with DMGB, then `dmg-delta --passes DMG,DMGB`): boot functions plus the
AREA11 overlay armed, one-shot, re-armed per beat (the new game reloads the
same overlay); an arena hit counts only with overlay id 9 resident (no hit
was unattributed); no beat reaches AREA01. Runs:
`build/s87/census/runs/{DMG,DMGB}/dmg_*.json` (replay rows in `traces/`);
logs `build/c10/damage/census_run_DMG.log`, `census_run_DMG_0405.log`,
`census_run_DMGB.log`; delta **`build/c10/damage/census_delta.json`**
(against `build/s87/census/classified.json`, the first-level census of 1,184
functions, keyed by overlay and address).

- **1,129 functions ran; 1,079 are already in the first-level census; 50
  are new** (5,985 instructions): 49 boot, 1 AREA11 overlay (the flame's
  contact callback 0x823580). Decomp status of the new ones: 31 byte-matched
  C, 10 NEARMISS, 4 inline asm, 4 word asm, 1 overlay C. The 1,079 known
  ones by port status: 218 live, 388 verified-unbound, 5 unverified, 1
  legacy stand-in, 7 missing, 460 boundary.
- New functions by the beat that first ran them (frame of the DMG replay):
  - dmg_00 (17), the contact and the flinch: 0x823580 (AREA11), 001D8100,
    001EFE00 (the attached effect), 0021BC40, 0021C350 (health apply),
    0017C370, 001CD070, 001F0190, 001F0290, 0022B700, 0022B7A0, 0022BB70,
    0022BBC0 (the effect 0x80000027's owner), 0015B770 (player state 2),
    0021D1A0, 0021D600, 0021D800 (flinch, sub 0).
  - dmg_02 (7), death and game over: 0021E240 (death, sub 1), 0021D490,
    0021D2E0 (the terminal), 001F77B0 (the record it spawns), 001AD140,
    001AD4E0 (the game-over wait), 001D2880.
  - dmg_03 (2): 001ADF00 (installs the continue machine), 001195A8.
  - dmg_05 (14), the load screen: 00225A00, 00225AC0, 00225720, 00225A20,
    00225CF0, 00225D20, 00226070, 00114848, 00114930, 00114988, 001FCBD0,
    001FE9A0, 001FECB0, 001FE8D0.
  - dmg_06 (2), the landing hit: 001639E0, 00163E90 (landing +6 = 3).
  - dmg_07 (4): 001A58B0, 001755B0, 0021D250 (the 0x5D floor), 00225570
    (+5 0x16).
  - dmg_08 (4): 00194D10, 0022FCA0, 00230000 (under the fans; also new in
    the EXIT lane's departure), 0021E9C0 (+5 0x11, the fan's reaction).
  - dmg_01 and dmg_04 ran no function the earlier beats had not.
- Not in the census: the own code of the DATA.DAT screen modules (1, the
  title menu; 0x27, the game-over screen) is not among the candidates (boot ELF
  functions and the AREA11 overlay only); the overlay arena held AREA11
  (id 9) in every frame of every beat.

### Rules kept

PCSX2 ran hidden, one session at a time, under the lock for the whole lane
(one acquisition), and no PCSX2 process was left running. One probe run left
a snapshot in the free slot 16 when its Pine save failed; the file was
removed (the sstates listing equals the one taken at the lane's start). No
save-state slot 01..15 was written. The renderer setting was not changed (no
framebuffer pixels were needed; the save states' embedded screenshots were
enough). No memory was written. The load screen was left without choosing a
memory card slot; no save path was entered. The memory cards' SHA-1s are
unchanged across the lane (`build/c10/damage/memcard_hashes_before.txt` and `memcard_hashes_after.txt`,
checked after every run).

### Port

Port chain step DAMAGE (2026-10-02): the port's level smoke replays dmg_00..04,
06 and 07 as the side runs dmg_flame / dmg_crevice_fall / dmg_pit_fall (port
docs/DAMAGE.md section 8, tools/level_smoke_damage.py), all PASS. Every window
is aligned on its event; the game over is aligned on the end of the screen
module's load (host speed: 10 ticks against the recording's 23). The dmg_02 end
snapshot's live 001F77B0 node (record 0x007B0970, subtype 2, sizes 186/210) is
the port's lockstep oracle case. dmg_05 and dmg_08 are not replayed yet (the
OPTIONS step binds the load screen; dmg_08's fan hit needs Roger's departure,
which the port's EXIT step made live, so a dmg_fan side run can follow).

## BRANCH: the AREA11 branches the main route skips

Lane BRANCH, 2026-10-01. What AREA11 offers that no route, C7, AIM, EXIT or
DAMAGE beat covers, found from the placement table 0x82A3C0 (21 records),
the deferred group D_0024D820[11] (0x828180, nine records), the owners' code
(decomp C of 00159210 and the AREA11 overlay's Roger, manager, opening
controller and terminal owners) and the AREA11 grid (floor, wall and
attribute-0x32 ladder nodes, read from a route snapshot's RAM through the
scratchpad pointers 0x700031FC..0x7000320C), then recorded. Fifteen beats,
opt-in group `br` in `tools/route_capture.py`, outputs in
`build/c10/branch/<beat>/`; change logs in `build/c10/branch/events.txt`.

### What AREA11 offers, and which beat covers it

| Interaction | Owner / record | Covered by |
|---|---|---|
| Battery pickup g0.0 (item 0x1B) | 00219550, deferred g0.0, puid 1 | route 01 |
| Pickup g0.1 (item 0x1E) on the yard floor (467.3, 184.8, 227.4) | 00219550, g0.1, puid 4 | br_10 |
| Pickup g0.2 (item 0x1F) on the 355 top (431.9, 354.8, 411.8) | 00219550, g0.2, puid 5 | br_12 |
| Pickup g0.3 (item 0x1E) on the 220 ledge (213.6, 219.8, 311.9) | 00219550, g0.3, puid 6 | br_00 |
| Pickup g0.4 (item 0x32, model 2, a wall mount) on the cage floor (381.3, 225.3, 266.8) | 00219550, g0.4, puid 7 | br_09 |
| Pickup g0.5 (item 0x10) inside box r6 on the 250 ledge (311.6, 249.8, 328.7) | 00219550, g0.5, puid 8 | br_06 + br_07 |
| Map item g0.6 (item 0x08), 16 above the ground south of the slide's foot (231.1, 200.9, 428.0) | 0015AFA0, class 0x87, g0.6, puid 9 | br_01 |
| Power panel r18: no battery; battery, Yes | 00159210 | route 00; route 03 |
| Power panel r18: battery, No on the two-unit prompt (cancel script 0x247DA0) | 00159210 | br_03 |
| Power panel after the power is on | 00159210 idles in sub-state 3 (+0 = 2, no use) | nothing to capture |
| Elevator terminal r19: refusal; the ride down | overlay 0x827B10 | route 02; route 04 |
| Elevator terminal r19 on the lower floor: the ride back up (D_0081083A 1 -> 0) | overlay 0x827B10 | br_02 |
| Fence door r0, both sides | 001BC350 | route 09; C7 DOOR1 |
| Cage ladder (column x 359.8..360), up | attribute 0x32 | route 10, br_09 |
| West-yard ladder (column x 316.9..325.6), up and down | attribute 0x32 | br_05, br_08 |
| Plateau ladder (column x 467.6..476.7), up and down | attribute 0x32 | br_11, br_13 |
| Boxes broken by the light melee | 001551B0 r3..r6 | br_04 (r5, which wakes the raised r3), br_06 (r6) |
| Drums r14 / r15 | 00156620 | not broken in this lane (below) |
| Roger r8 after the encounter (D_008107D8 = 1): talk, script 0x828810 | overlay 0x8237E0, third branch 0x823B70 | br_14 |
| Roger r8: conversation, encounter, departure | overlay 0x8237E0 | route 10, 14; EXIT |
| Fan r2's box: slow-arm exit; fast-arm hit | overlay 0x827630 | EXIT exit_00; DAMAGE dmg_08 |
| Security gun g0.7 and its cable g0.8 | overlay 0x825940 / 0x827490 | AIM aim_10, aim_11 (the gun is dormant on this visit) |
| Manager r11: script 0x828C70 when D_00810788 != 0 | overlay 0x823CE0 | not reachable here: D_00810788 (event flag 0x30) is 0 in every row of every BRANCH beat |
| Record 13, state 1 (event 0x30 set) | overlay 0x8257A0 | not reachable here (frees itself after the load) |
| Opening controller r10 | overlay 0x823E80 | publishes no use (class 4); runs in the opening |

Not reachable as captured: the fan's fast-arm exit (Z < 156 with the fast
arm needs the player to cross the hit band Z 156..166.5 first, and the hit
takes the box away from it until +0 is 1 again) and the fan's direct area
change 001B0C60(1, 1, 4) (D_00810758 = 0xFF, after Roger's departure, on a
later visit).

**The drums.** Neither drum broke. The light melee could not reach drum r14:
the player is held 8.4 from its centre (its hull), and six Circle presses
from there left its +0x36 at 0. An exploratory run (not kept) fired eight
rounds at it in the R1 stance, level and with the aim lowered: no round
changed its +0x36 or its state. What damages 00156620 is not established;
no beat records it.

**How the areas connect** (from the grid's floor and wall nodes, confirmed by
the captures): the yard north of the fence (z < 289.5) is reached only
through the fence door (route 09's entry 2); the corridor north of the cage
(z 205..224) is closed at x 344.4..354.4 by a box 10 high, which the player
climbs from the east (the ledge climb) and runs off on the west (a walk at
half stick stops at its edge); that pocket holds the west-yard ladder's
foot. The plateau (270) is crossed by a raised pipe (279.1, x 445..495) that
the player climbs and runs off the same way; the plateau ladder is fixed to
the east face of a column (x 462..473, z 394..418); the 355 top is a ring
round a block (x 434..457, z 397..420), with g0.2 on its west side.

### Beats

Every beat idles with a neutral pad up to its pin (the source snapshot's
main-loop counter + 30; from 14_roger_encounter at least 15800, as EXIT and
DAMAGE do), then drives closed loop (predicates on the recorded fields; a
Use press that starts nothing is repeated). Frame numbers are trace frames.
Pickup takes (route 01's pattern): the use scan (3B8D = 3), the take clip,
request B0/B1, the status opens on a page with the item's text (message
mode 4, token = the item type), Triangle once the page record stops
changing, then the owner and its light child free themselves and the
taken bit (D_00810860 + 32 * 11, bit = puid) is set.

| Beat | Source | Frames | Counters | What happens |
|---|---|---|---|---|
| br_00_ledge_ammo | 05_boxes | 626 | 6930..7556 | On the 220 ledge, Cross toward g0.3 (f110 starts nothing, f172 does): f175 scan, clip 0x42, f239 request 01/1E, f242 status page (ui 03 02 00 07), text countdown to f509; Triangle f589; f596 taken bit 6. Item 0x1E count 0 -> 1. |
| br_01_map_item | 06_hill_slide | 425 | 7144..7569 | South-west to g0.6; Cross f219, f222 scan, clip 0x40 (0015AE20 picks the grab clip by the item's height), f275 request 02/08, f278 status page (ui 03 02 02 00, token 0x08); Triangle f388; f396 taken bit 9. |
| br_02_elevator_up | 04_elevator_ride | 490 | 6255..6745 | At the terminal on the lower floor (heights 190, D_0081083A = 1): Cross f42 starts nothing, f94 does: f97 script 0x82A750 (letterbox, clip 0x47), the carry up; f460 D_0081083A = 0; control at (222, 230, 250). |
| br_03_panel_decline | 02_elevator_refusal | 591 | 4991..5582 | Cross at the panel with the battery (f261): script 0x2477A0, message 0x80000018, f419 request 01/82, the BATTERY page, f449 its two-unit prompt (default No); Cross on No (f479) goes back to the battery list (f482, token 0x1B); Triangle f541: the status closes, 00159210 starts the cancel script 0x247DA0 (f548); control at f561. Power bit clear, charge 12. |
| br_04_crate_stack_break | 04_elevator_ride | 552 | 6256..6808 | To box r5 under r3; Circle f211 (light melee: +5 0x21, +1F0 0x36, clip 0x10B): f227 r5 +0x36 = 0x1003, f228 r5 breaks (state 2; the husk stays). f229 r3 (raised, woken by the break) enters state 1 with no supported corner and drops straight down, 203.8 to 189.4, then breaks (state 2, 3, f253..f254) and is freed (f255, no husk). |
| br_05_west_ladder_up | 09_fence_door | 1223 | 8479..9702 | Behind the fence, west to the corridor box; Cross f312 starts nothing, f374 climbs it (+1F0 8); run off its west side (+1F0 0x0B, 0x0F; +5 5, 8); to the ladder foot; Cross f707: f710 +1F0 0x15, +5 0xB (clip 0xE3); stick up: 0x17, +5 0xC (clips 0xE6, 0xE8 / 0xEA) from f769; 0x18 (clip 0xF0) at y 233 (f1093); control on the 250 ledge at f1187. |
| br_06_ledge_crate_break | br_05 | 486 | 9706..10192 | South along the 250 ledge to box r6; Circle f253 misses, f325 hits: f341 +0x36 = 0x1003, f342 r6 breaks (the husk stays). |
| br_07_ledge_magazine | br_06 | 621 | 10199..10820 | Take g0.5 (Cross f102 starts nothing, f164 does): f167 scan, clip 0x42, f231 request 01/10, status page (ui 03 02 00 09, then 00 01), token 0x10; Triangle f584; f591 taken bit 8. Reserve D_00810CB4 60 -> 90; item 0x10's count 2 -> 3. |
| br_08_west_ladder_down | br_07 | 779 | 10821..11600 | Back to the ladder's top node, Cross facing the drop (f277): f280 +1F0 0x16, +5 0xB (the grab from above, clip 0xE4); stick down: 0x17 (clips 0xE6, 0xE9 / 0xEB) from f379; 0x18 (clip 0xF1) at y 188 (f701); control on the yard floor at f749. |
| br_09_cage_key | 08_truck_crossing | 1029 | 7945..8974 | Route 10's ladder A (Cross f299, grab f302, top 225.4 at f611); east across the cage floor; Cross facing +x (f752 starts nothing, f814 does): f817 scan, clip 0x42, f881 request 03/32, status page (ui 03 02 04 00, token 0x32); Triangle f992; f999 taken bit 7. |
| br_10_yard_ammo | 09_fence_door | 695 | 8478..9173 | Behind the fence, across the yard to g0.1; Cross f251, f254 scan, f308 request 01/1E, status page (ui 03 02 00 07); Triangle f658; f665 taken bit 4. |
| br_11_plateau_ladder_up | 11_crevice_prompt | 1191 | 12787..13978 | Off the pipe end onto the plateau, south to the raised pipe; Cross f255 climbs it; run off its south side; to the ladder foot; Cross f527, grab f530 (0x15), climb from f589, 0x18 at y 339 (f1061); control on the 355 top at f1155. |
| br_12_tower_ammo | br_11 | 693 | 13980..14673 | Round the block by its south side to g0.2; Cross f189 starts nothing, f251 does: f254 scan, f306 request 01/1F, status page (ui 03 02 00 07); Triangle f656; f663 taken bit 5. |
| br_13_plateau_ladder_down | br_12 | 928 | 14676..15604 | Back round the block to the ladder's top node; Cross f218 starts nothing, f300 does: f303 grab from above (0x16), climb down from f402, 0x18 at y 273 (f850); control on the plateau at f898. |
| br_14_roger_talk | 14_roger_encounter | 929 | 15760..16689 | On the west tower top, facing Roger, Cross (f99 starts nothing, f206 does): f209 the use scan marks Roger (+0x0B = 4) and his third branch starts script 0x828810 (3B8D 3, then 2; letterbox; camera byte 1); his line (message mode 2, token 0x13) from f235 to f791, player clips 358 / 357; f867..f868 the script ends (Roger's +0x0B back to 0); control at f869. |

The end snapshots show the 220 ledge (br_00), the slide's foot (br_01), the
upper floor by the terminal (br_02), the panel (br_03), the lower floor with
r5's husk (br_04), the 250 ledge (br_05..br_07), the yard by the west-yard
ladder (br_08), the cage floor (br_09), the yard (br_10), the 355 top (br_11,
br_12), the plateau (br_13) and the west tower top (br_14).

Files per beat: `trace.json` (meta, `inputs`, `marks`, `end`, per-frame
`rows`), `state.p2s` (end snapshot), `eeMemory.bin`, `gs.bin`,
`scratchpad.bin`, `original.png`, `snapshot.json`. Every end snapshot was
checked to resume (route_capture's `resumes`).

### What each row records

DAMAGE's row (the route row, beat 15's exit fields, the whole player record
+0..+0x31F `pl` with its decoded fields `vit`, the globals 0x810600..0x810D3F,
the pad block, D_00275BD0..EF, the per-class lists, the scratchpad
0x70003B40..9F, 0x70003A20 and 0x700038A0..BF, the flame node and the pool
headers that differ from row 0) plus: the status page record
0x810130..8F (`ui_rec`); the message service record 0x282210..23;
the ray query block 0x70003190..DF and the action masks 0x70003B70..7F;
the taken-bit row of area 11 (`taken11`), the item counts of types 0x08,
0x10, 0x1B, 0x1E, 0x1F, 0x32 and the weapon bytes C61 / C62 / CB4 (`inv_br`,
`weap`); the terminal r19 (+0..+3F, +1F0..+21F, D_0081083A and its four
heights 0x82A7C4 / 0x82A844 / 0x82A944 / 0x82AB14); the panel r18 and Roger
r8 (+0..+3F, +1F0..; Roger's script pointer); the six item owners g0.1..g0.6
(+0..+3F, +B0, +1F0..+21F, +2E0..+2EF); and the boxes r3..r6 and drums
r14 / r15 (+0..+DF with the +0x36 damage word, +1F0..+21F).

### Replay

Decomp `.venv` python, repo root, PCSX2 lock held:

    .venv/bin/python tools/route_capture.py run --beats br            # all 15 beats, in order
    .venv/bin/python tools/route_capture.py run --beats br_09_cage_key
    .venv/bin/python tools/route_capture.py events --beats br         # change logs
    .venv/bin/python tools/route_capture.py verify --beats br         # snapshots resume
    .venv/bin/python tools/route_census.py run --segments br --pass BR
    .venv/bin/python tools/route_census.py br-delta --passes BR,BRB

Two census replays per beat (passes BR and BRB) against the recordings,
rows paired by main-loop counter (route_census keeps each replay's rows in
`build/s87/census/runs/<pass>/traces/`). Every replay completed, reached
the recorded outcome (the taken bits, item counts, reserve, box states,
D_0081083A, power, Roger's script, the end position) and gave the same inputs
by counter in BR and BRB.

- Identical to the recording in every row except the vsync counter and
  the stream gate byte D_00282157, in both passes: br_00, br_01, br_02,
  br_06, br_07, br_08, br_12, br_14. br_03 and br_13 also differ in the
  processed pad word on one or two rows (a pad pickup timing difference;
  inputs by counter equal).
- br_04: inputs by counter equal; the player byte +0x302 differs from the
  recording on 326 rows from counter 6483 (the melee's hit frame); the two
  replays agree with each other.
- br_05, br_09, br_10, br_11: the two replays agree with each other (inputs
  by counter equal; rows equal but for vsync, D_00282157 and one pad row in
  br_05), and the recording is the outlier, as DAMAGE's dmg_01 was: br_10's
  recording took the Cross press one frame earlier (processed held word 0x40
  at counter 8733, 0 in the replays), so its Triangle came one counter
  earlier; br_05's recording differs first at counter 8780 (the player's +6,
  +0x28 and clip phase); br_09's and br_11's at counters 8632 and 13153 (the
  position, by small amounts), after which the closed loop's stick values
  differ by one or two steps. All four reach the recorded outcome; the end
  positions are equal except br_09's (z 266.50 recorded, 267.05 replayed).
- The first BRB run stopped responding at br_03's start (the DebugServer
  never answered after the state load, 17 minutes); it was stopped and
  BRB was run again for br_03..br_14 (logs `census_run_BRB_part1.log`,
  `census_run_BRB_part2.log`).

### Census

Passes BR and BRB (`route_census.py run --segments br --pass BR`, the same
with BRB, then `br-delta --passes BR,BRB`): boot functions plus the AREA11
overlay armed, one-shot, re-armed per beat; an arena hit counts only with
overlay id 9 resident (none was unattributed); no beat reaches AREA01. Runs:
`build/s87/census/runs/{BR,BRB}/br_*.json`; logs
`build/c10/branch/census_run_BR.log`, `census_run_BRB_part1.log`,
`census_run_BRB_part2.log`; delta **`build/c10/branch/census_delta.json`**
(against `build/s87/census/classified.json`, the first-level census of 1,184
functions, keyed by overlay and address; each new row also says whether the
AIM, EXIT or DAMAGE lane's delta already has it). BRB found no function BR
had not.

- **813 functions ran; 744 are already in the first-level census; 69 are
  new** (12,521 instructions): 68 boot, 1 AREA11 overlay piece. Decomp
  status of the new ones: 34 byte-matched C, 23 NEARMISS, 7 inline asm,
  4 word asm, 1 overlay piece (inside a byte-identical C function; see br_14 below). The 744 known ones by port
  status: 206 live, 333 verified-unbound, 6 unverified, 4 legacy stand-ins,
  7 missing, 188 boundary.
- **36 of the 69 are new to every C10 lane**; of the other 33, 31 are in
  the AIM delta (the light melee, its knife probe and effects, the status
  screen's SPR4 page), 0022FCA0 in the EXIT and DAMAGE deltas and 001A58B0
  in DAMAGE's.
- New functions by the beat that first ran them (* = also new in another
  C10 lane):
  - br_00 (3): 002160B0, 002082B0*, 00208AD0*.
  - br_01 (14), the map item and its page: 001C4720, 0020F950, 001CB480,
    00207D90, 00208040, 00210030, 002101C0, 00210A00, 00210C00, 00210F30,
    00211400, 001AFF10*, 001AF7C0*, 001B0000*.
  - br_02 (3), the ride up: 0015FDF0, 001AA4E0, 001A58B0*.
  - br_04 (21), the melee and the box breaks: 00189EC0, 00189FE0,
    001C6200, 001EBD20, 001F2BA0, 001F2E90, 001FC580 and 001735C0*,
    00102990*, 0019B2C0*, 001AA840*, 001EFF10*, 001F1550*, 001F15F0*,
    001F18C0*, 001CA3B0*, 001F2F90*, 001CA4D0*, 001F3340*, 001F3620*,
    001F3E30*.
  - br_05 (2): 001EAD70, 0022FCA0*.
  - br_07 (12), the magazine's status page (the functions AIM's aim_05
    first ran for the status screen's SPR4 page): 00211970*, 001D66A0*, 00208AB0*, 0020AC70*,
    0020BF20*, 002121A0*, 002125B0*, 00212B60*, 00212F30*, 00100130*,
    001B62C0*, 0020D930*.
  - br_08 (6), the descent: 0017FD40, 00180530, 00199FA0, 001A44B0,
    001A4830, 001028E8.
  - br_09 (7), the key and its page: 002131B0, 002134C0, 00213F30,
    00214020, 001FCF60, 001FCF90, 001FE660.
  - br_14 (1): the AREA11 overlay piece at runtime 0x823BB0 (splat
    `func_overlay_AREA11_00823B70`, 36 instructions), first on the frame the
    use scan marked Roger (counter 15968). It is not a separate function: it
    lies 0x40 into the function at runtime 0x823B70 (decomp
    `func_overlay_AREA11_00823B30`, byte-identical C, which absorbs that
    piece; docs/AREA11_OVERLAY.md "Split handling").
  - br_03, br_06, br_10..br_13 ran no function the earlier beats had not
    (the cancel script, the second box break, the other pickups, the
    plateau ladder both ways).

### Port

Port chain step BRANCHES (2026-10-03, port branch c11-t1) replays all fifteen
recordings as ten level-smoke side runs: br_ledge_ammo, br_map_item,
br_elevator_up, br_panel_decline, br_crate_stack, br_west_ledge (br_05..br_08),
br_yard_ammo, br_cage_key, br_plateau (br_11..br_13) and br_roger_talk. Each
runs this lane's br_beat_* policies on the port and is compared window by
window with `build/c10/branch/<beat>/trace.json`; all ten PASS (port
`make test-level-smoke-branch`; port docs/LEVEL_SMOKE.md "The BRANCH side
runs"). Two rules came out of the replay:

- The recordings' pad takes effect three rows after the row it was set on,
  where the port overlay's takes one; the port's program delays its pads by
  two ticks.
- br_11's pad lost the held stick for one row at f1031; the comparison
  realigns on the dismount.

Of the 36 functions new to every C10 lane, the port census
(FIRST_LEVEL_CENSUS.md 1.60) has 33 live, 001CB480 unverified, and
00207D90 / 00208040 at the 2D GS boundary.

### Tool changes

`tools/route_capture.py`: the opt-in group `br` (BR_BEATS, BrSampler /
decode_br, br_take, br_ladder_up / br_ladder_down, br_climb, br_melee,
br_pin, br_event_keys; outputs `build/c10/branch/`). `tools/route_census.py`:
`run --segments br`, `br-delta`, and the BRANCH replays keep their own rows
(as DAMAGE's do). Both are additive; no other group's behaviour changed.

### Rules kept

PCSX2 ran hidden, one session at a time, under the lock for the whole lane
(one acquisition, released at the end), and no PCSX2 process was left
running (the hung BRB session's emulator was stopped). No save-state slot
01..15 was written (the sstates listing is byte-for-byte the one taken at
the lane's start, `build/c10/branch/sstates_before.txt`). The renderer
setting was not changed (it is 17; no framebuffer pixels were needed). No
memory was written. No save path was entered. The memory cards' SHA-1s are
unchanged across the lane (`build/c10/branch/memcard_hashes_before.txt`,
`memcard_hashes_after.txt`).

## OPTIONS: the options screen and the save paths of AREA11

Lane OPTIONS, 2026-10-01. The settings screen the player can open in AREA11
and every memory-card path reachable there, found from the code (decomp C of
001AE7E0, the frame machine 001AE040 (`anim_frame_top_b.c`), 0022A650 and its
row screens 00201720, 00201C50, 00201F70, 00202BA0, 00202D10, 0022B420,
0022A590, the memory-card screen 00225AC0, 001AD010, 001AD250, 001AD740,
00157F60, 0020CDC0) and a static scan of the boot ELF and every overlay,
then recorded. Nine beats, opt-in group `opt` in `tools/route_capture.py`,
outputs in `build/c10/options/<beat>/`; change logs in
`build/c10/options/events.txt`, capture logs `capture_*.log` there.

### Where the options are

The status screen (START or Triangle in gameplay) has no options page: its
hub enters DATABASE, SPR4, MAP and ITEM only (hovers 1..4 to pages 3, 2, 1,
0; port `docs/STATUS_PAGES.md` section 1). The options are a screen of their
own, opened by SELECT:

- The classifier 001AE7E0 returns 1 on a SELECT edge (pressed word bit
  0x100), or whenever the pad-state byte D_00810E50 is not 4. It returns 0
  first while a room move (D_008106B8) or the death latch (D_008106B9) is
  set, 3 or 2 for a pending end-screen or status request, and 0 while the
  fade machine runs or the scene selector 3B8D is set; the SELECT test comes
  before the menu-inhibit byte D_008106B3, which only blocks the status
  screen's START / Triangle (return 2).
- The frame machine (the gameplay task 001ACEC0 in slot 0x28A750; +0xB is
  its frame state) then sets D_008106C4 = 2, stops the sounds, plays sound
  0xC and goes to state 2, which calls 0022A650 every frame with the world
  frozen. 0022A650's state is task +0xC, its sub-state +0xD, its cursor
  +0x1C (u16), its saved byte +0x13. When it returns 1 the frame machine
  goes back to state 1 (D_008106C4 = 0, sound 0xD, music resumed); 2 is a
  successful load (the gameplay task restarts in its load state); 3 calls
  001AD140 (the game-over wait, which leads to the title menu).
- The screen lists nine rows (cursor 0..8, the action table D_002672E0 =
  7, 0, 1, 2, 3, 4, 9, 5, 8): a row to leave the screen, vibration (shown
  on or off), sound (stereo or mono), screen position, brightness, button
  config (type A, B or C), load, default and quit game, under a title and
  a legend of the three buttons (Cross confirms, Circle goes back, Triangle
  exits). Down / Up move the cursor through the pad's repeat word
  D_00810E78 and wrap at both ends. The vibration row is skipped, and its
  action ignored, when the pad byte D_00810E6A is not 7 (it is 7 here).
- The settings live in D_00810118: +0 the button type, +1 vibration,
  +3 the default prompt's choice, +4 sound, +8 / +0xA the screen offset kept
  at the last confirm. The screen offset itself is 0x70003B94 (x) /
  0x70003B96 (y); the button type's action masks are 0x70003B74..0x70003B82.

The options screen draws over a moving background (0020A7A0, the hub's
tile) and its row screens are the DATA.DAT screen module 0x2B (loaded by
0022A590 in state 10 through 001FF080, D_00275BD8 busy for about 23
frames). Start state of every beat (route snapshot 08_truck_crossing,
AREA11, in control): D_00810118 = 00 01 00 00 00 ... (type A, vibration on,
stereo, offset 0, 0), masks 0x80, 0x40, 0x20, 0x10, 0x08, 0x02, 0x04,
0x01, D_0028215B = 0, D_00810E6A = 7, D_00810E50 = 4.

### Save paths: the first level offers none

Proof from the code (`route_census.py opt-save-scan`, written to
**`build/c10/options/save_scan.json`**; the scanner is the world graph's
linear scan over the boot ELF and the text of all 19 overlays, with its
address window widened to 0x810000..0x810E00):

- The memory-card screen 00225AC0 takes its mode from its argument: 0
  stores +0x14 = 2 in D_00810040 (load), 1 stores +0x14 = 1 (save). Its
  callers: with 0, the title menu's continue machine 001AC070 (DAMAGE's
  dmg_05) and the options screen 0022A650 (its load row); with 1, only
  0020CDC0 (the status screen's request-6 phase) and 001AD740. No overlay
  calls it.
- 001AD740 runs only as the gameplay task's +9 = 3, which 001AD010 sets at a
  room move when the scratchpad byte 0x70003B93 is nonzero. The only store of
  a nonzero value to 0x70003B93 in the boot ELF and every overlay is in the
  AREA21 overlay (function at runtime 0x827040); the boot ELF only clears it
  (001ADF00, 001AFCF0). 001AD740 rebuilds the clear data (the area bytes go
  back to AREA11, sub 0), shows screen module 0x37 and then opens 00225AC0
  in save mode: an end-of-game path that AREA11 cannot reach.
- Request 6 (D_008106B0 = 6) is stored only by 00157F60, and only for an
  owner whose byte +3 (the placement record's model byte) is 0x38. The save
  terminals are the 00159B90 records (class 0x84) with model 0x38: one or
  two in AREA00, AREA01, AREA02, AREA06, AREA07, AREA08, AREA13, AREA15,
  AREA16, AREA18 and AREA21, none in AREA11 (`terminal_records` in the scan
  lists every 00159B90, 00159210 and 00159970 record and every record with
  model 0x37 or 0x38 of every area).
  AREA11's 30 records (21 placements, the nine deferred g0.*) hold no
  00159B90 record and no record with model 0x37 or 0x38; its one 00159210
  terminal is the power panel r18, model 0x24, which posts request 1 (the
  BATTERY page). The four data words pointing at 00157F60 are in the boot
  ELF's script records (0x2473E4, 0x247664, 0x247764, 0x247864).
- So the first level has no save path. The nearest save terminal is AREA01's
  record [20] (sub 0) at (167, 7, -626.7), in the east room off the arrival
  corridor; the port's `docs/SECOND_LEVEL_ROUTE.md` (beat a01_s4_east_room)
  records it up to its battery-cost prompt, declined. Its save screens (the
  battery payment, then 00225AC0 in save mode) are beyond the first level
  and were not entered.
- The one memory-card screen AREA11 reaches is the options screen's load
  row (00225AC0 in load mode). opt_07 records it up to its memory-card slot
  choice and leaves it; no slot was chosen.

### Beats

Every beat starts from `build/s87/route/08_truck_crossing/state.p2s`, idles
with a neutral pad up to its pin (the snapshot's main-loop counter + 30, as
DAMAGE and BRANCH do), then drives closed loop: every press is a two-frame
tap repeated until its effect shows in the recorded fields (the cursor, the
task states, D_00810118, the screen offset, the masks). Frame numbers are
trace frames (one main-loop frame each); every beat ends in control with
D_00810118, the offset and the masks back at their start values.

| Beat | Frames | Counters | What happens |
|---|---|---|---|
| opt_00_browse_close | 418 | 7945..8363 | SELECT at f39: f42 frame state 2, D_008106C4 = 2; f43 0022A650 state 1, message mode 4. Down nine times (cursor 1..8, then 0: the wrap), Up nine times (8 first: the wrap, back to 0), each taking effect 3 frames after the tap. Cross on the exit row (f251): 3 frames later state 12, frame state 1 and D_008106C4 = 0 on the same frame. Opened again and closed with Circle (f298), Triangle (f346) and SELECT (f394): each sets state 12 3 frames after the tap and closes on the next frame. |
| opt_01_vibration | 216 | 7945..8161 | Cursor 1, Cross: state 5 (00201720), +0x13 = 1 (the value on entry). Right: D_00810118+1 = 0 (off), the row blinks (+0xD 2, ten frames); Cross keeps it (state 1). Cross, Right: +1 = 1 (on) and the pad block's rumble byte D_00810E56 is 1 for four frames (the confirmation rumble of switching vibration on); Cross keeps it. Closed on the exit row. Left does nothing in state 5 (only Right flips the value; seen in the probe that preceded the beat). |
| opt_02_sound | 237 | 7946..8183 | Cursor 2, Cross: state 5; Right: +4 = 1 and D_0028215B = 1 on the same frame (the mono flag the sound mixer reads); Cross keeps it. Cross, Right: +4 = 0, D_0028215B = 0; Cross. Closed. |
| opt_03_screen_position | 459 | 7945..8404 | Cursor 3, Cross: state 10 (+0x13 = 7), screen module 0x2B loads (D_00275BD8 1 for 23 frames), state 7 (00201F70), which copies the offset to +8 / +0xA. Up 3 (0x70003B96 1, 2, 3), Left 2 (0x70003B94 1, 2; the screen shows the two values), Cross: kept (+8 / +0xA = 2, 3; state 2, 00200970(1), state 1). Again: Down 3, Right 2 (back to 0, 0), Cross: kept. Again: Up 2 (0, 2), Circle: 00201F70 restores the offset it was entered with (0, 0) and returns to the list. Closed. |
| opt_04_brightness | 396 | 7946..8342 | Cursor 4, Cross: state 10 (+0x13 = 8), module 0x2B, state 8 (00202BA0): a still screen of grey bars with a red line and a line of advice to set the TV so the bars right of the line are black; there is no setting. Cross (f175) and Circle (f273) return to the list (state 2, then 1). The third time Triangle (f371): 00202BA0 returns 2, state 11, state 12, the options screen closes. |
| opt_05_button_config | 425 | 7945..8370 | Cursor 5 (Up from the exit row), Cross: state 10 (+0x13 = 9), module 0x2B, state 9 (00202D10; +0x13 = the type on entry): a screen with the three types, the action list with each action's button and a pad picture. Right: +0 = 1 (B); Cross commits through 001AF470: masks 3B74..3B82 = 0x20, 0x40, 0x80, 0x10, 0x08, 0x02, 0x04, 0x01 (Square and Circle swapped from A). Again Right: +0 = 2 (C); Cross: 0x80, 0x20, 0x40, 0x10, 0x02, 0x08, 0x04, 0x01 (Cross and Circle, R1 and R2 swapped from A). Again Left, Left: +0 = 0 (A); Cross: the start masks. Closed. |
| opt_06_default | 306 | 7945..8251 | Vibration off first (cursor 1, Cross, Right, Cross). Cursor 7 (default), Cross: state 6 (00201C50), its choice +3 = 0 (No, shown on the row with an arrow); Cross: nothing changes, state 1. Cross, Right: +3 = 1 (Yes); Cross (f238): the defaults are written in one frame (vibration 1 because the pad byte D_00810E6A is 7, sound 0, type 0 through 001AF470, offset 0 in +8 / +0xA and 0x70003B94 / 96, +3 back to 0): D_00810118 is the start value again. Closed. |
| opt_07_load_cancel | 329 | 7945..8274 | Cursor 6 (Up from the exit row), Cross (f86): 001AF6F0 clears the 212-byte record D_00810040, state 3, 00225AC0(0): +0x14 = 2 (load), the sounds and the stream stop, screen module 0x2A loads (D_00275BD8 1 from f91 to f114), f115 the load screen runs (00225AC0 state 1, 00225D20; its own state +0x15 = 1 from f116: 00226070, the slot choice). The screen shows a title, the button legend, an empty data box and a prompt asking which memory-card slot to use, with the two slots listed and the first highlighted. Triangle at f175 (60 frames later): f178 +0x15 = 3 and the result 1, f179 the fade-out, f211 the screen's state 3, f212 00225AC0 returns 1: state 2 (00200970(1)) with the fade-in, f213 the list, fade idle at f243. Closed. No slot was chosen. |
| opt_08_quit_cancel | 299 | 7945..8244 | Cursor 8 (Up from the exit row), Cross: state 4 (0022B420): the row shows No with an arrow (+0x13 = 0). Right (f97): +0x13 = 1 (Yes); Right (f130): 0 (No); Cross on No (f163): 0022B420 returns 1, the list. Cross, Circle (f219): returns 1, the list. Cross, Triangle (f275): returns 2, state 12, the options screen closes. Yes was never confirmed. |

The end snapshots all show the gameplay view at the start position
(371.5, 184.8, 361.3); the probe snapshots used to describe the row screens
were deleted after viewing.

Files per beat: `trace.json` (meta, `inputs`, `marks` = the frame of every
press that took effect, `end`, per-frame `rows`), `state.p2s` (end
snapshot), `eeMemory.bin`, `gs.bin`, `scratchpad.bin`, `original.png`,
`snapshot.json`. Every end snapshot was checked to resume (route_capture's
`resumes`, and again with `verify`).

### What each row records

DAMAGE's row (the route row, beat 15's exit fields, the whole player record
+0..+0x31F, the globals 0x810600..0x810D3F, the pad block
0x810E40..0x810E9F, D_00275BD0..EF, the per-class lists, the scratchpad
0x70003B40..9F, 0x70003A20 and 0x700038A0..BF, the flame node, the pool
headers that differ from row 0) plus: D_00810118..27 (`opt118`), the
memory-card record D_00810040..0x810113 (`mc040`), D_002821B0..0x28225F
(`msgblk`), 0x282150..5F (`snd150`, D_00282157 and D_0028215B) and the
gameplay task slot 0x28A750..7F (`task0x`), decoded in `ov`: the settings
(type, vibration, default choice, sound, saved offset), the task's +4
function, +8, +9, the frame state +0xB, 0022A650's state +0xC and sub-state
+0xD, +0x12, +0x13, the cursor +0x1C and timer +0x1E, D_008106C4, the
offset 0x70003B94 / 96, 0x70003B90, 0x70003B93, the masks
0x70003B74..0x70003B83, D_00810E6A, D_00810E50, the repeat word
D_00810E78, the memory-card screen's state, sub-state, mode (+0x14) and
result (+0x16), D_0028215B and D_00275BD8.

### Findings

- **Close paths.** In the list, Cross on the exit row closes on the frame it
  is read (state 12 and frame state 1 together); Circle, Triangle and
  SELECT set state 12 and close one frame later. A row screen left with
  Cross or Circle returns to the list; Triangle there closes the whole
  options screen (00202BA0 returns 2: state 11, then 12).
- **The quit prompt's Circle and Triangle.** The original 0022B420 returns
  1 (back to the list) for Circle and 2 (close the options screen) for
  Triangle; opt_08 shows both. The NEARMISS C in `src/func_0022B420.c` has
  the two swapped (its cancel result is 1 when bit 0x10 is set); the boot
  ELF is unaffected (the linker fills the function from the splat .s), but
  the C is wrong as ground truth. Flagged as a follow-up task, not changed
  here.
- **Vibration** is a toggle on Right only; switching it on gives a short
  confirmation rumble (the pad block's byte D_00810E56 is 1 for four
  frames). Circle in the toggle restores the value it was entered with
  (00201720 keeps it in +0x13; seen in the probe that preceded the
  beats).
- **Sound** writes D_00810118+4 and the mixer's mono flag D_0028215B
  (FINDINGS: mono gives both channels the same gain) on the same frame.
- **Screen position** changes 0x70003B94 / 96 live (Left raises x, Up
  raises y, one step per tap through the repeat word; the code clamps each
  to 20 either way); Cross keeps the offset in +8 / +0xA, Circle restores
  the entry values.
- **Brightness** is a still calibration picture with nothing to set.
- **Button config** commits only on Cross: the three types' masks are
  listed under opt_05.
- **Default** with Yes writes every default in one frame (vibration on only
  when the pad byte D_00810E6A is 7).
- **Load** is the only memory-card screen in AREA11 (00225AC0 in load
  mode); Triangle at its slot choice returns to the list. The memory cards'
  hashes are unchanged.

### Replay

Decomp `.venv` python, repo root, PCSX2 lock held:

    .venv/bin/python tools/route_capture.py run --beats opt           # all 9 beats, in order
    .venv/bin/python tools/route_capture.py run --beats opt_05_button_config
    .venv/bin/python tools/route_capture.py events --beats opt        # change logs
    .venv/bin/python tools/route_capture.py verify --beats opt        # snapshots resume
    .venv/bin/python tools/route_census.py run --segments opt --pass OPT
    .venv/bin/python tools/route_census.py opt-delta --passes OPT,OPTB
    .venv/bin/python tools/route_census.py opt-save-scan              # no emulator needed

Two census replays per beat (passes OPT and OPTB) against the recordings,
rows paired by main-loop counter (each replay's rows are kept in
`build/s87/census/runs/<pass>/traces/`). All 18 replays completed, gave the
recorded inputs by counter, ended on the recorded counter with the recorded
outcome (D_00810118, the offset, the masks, D_0028215B, the task states,
D_008106C4, the position) and the recorded player, owner and global spans,
except one global word (0x810D98) at the end of opt_01, 02, 04, 05 and 06
in both passes. The fields that differ from the recording are only the pad
block's timing words (0x810E88, the vsync counter 0x810E90, 0x810E98) and
the loader bytes 0x282154, D_00282157 and 0x282158: opt_07 in OPT and
opt_08 in OPTB are identical to the recording in every row (330 / 330 and
300 / 300); opt_03 in OPTB in 439 of 460 rows. A replay can start up to ten
frames after its recording (the loaded state runs until Pine answers); the
pin makes the inputs land on the same counters.

### Census

Passes OPT and OPTB (`route_census.py run --segments opt --pass OPT`, the
same with OPTB, then `opt-delta --passes OPT,OPTB`): boot functions plus the
AREA11 overlay armed, one-shot, re-armed per beat; an arena hit counts only
with overlay id 9 resident (none was unattributed); no beat leaves AREA11,
so the AREA01 overlay was not armed. Runs:
`build/s87/census/runs/{OPT,OPTB}/opt_*.json` (replay rows in `traces/`);
logs `build/c10/options/census_run_OPT.log`, `census_run_OPTB.log`; delta
**`build/c10/options/census_delta.json`** (against
`build/s87/census/classified.json`, the first-level census of 1,184
functions, keyed by overlay and address; each new row says whether the AIM,
EXIT, DAMAGE or BRANCH delta already has it). OPTB found no function OPT
had not.

- **556 functions ran; 529 are already in the first-level census; 27 are
  new** (4,359 instructions), all boot ELF. Decomp status of the new ones:
  17 byte-matched C, 8 NEARMISS, 1 inline asm, 1 word asm. The 529 known
  ones by port status: 133 live, 205 verified-unbound, 3 unverified, 1
  legacy stand-in, 7 missing, 180 boundary.
- **14 of the 27 are new to every C10 lane**; the other 13 are in the
  DAMAGE delta (its dmg_05 ran the same memory-card screen from the title
  menu, and 001FCBD0).
- New functions by the beat that first ran them (* = also new in DAMAGE):
  - opt_00 (6): the options screen 0022A650 and its row drawer 0022AEA0,
    001FCE30, 001FCBD0*, 00123280 (on the open), 001AF1C0 (on the close).
  - opt_01 (1): 00201720 (the vibration / sound toggle).
  - opt_02 (1): 00119870 (on the sound change).
  - opt_03 (2): 0022A590 (the module load of state 10), 00201F70.
  - opt_04 (1): 00202BA0.
  - opt_05 (1): 00202D10.
  - opt_06 (1): 00201C50.
  - opt_07 (13): 001AF6F0 and the load screen: 00225AC0*, 00225720*,
    00225A20*, 00225CF0*, 00225D20*, 00226070*, 00114848*, 00114930*,
    00114988*, 001FE9A0*, 001FECB0*, 001FE8D0*.
  - opt_08 (1): 0022B420.
- Not in the census: the own code of the DATA.DAT screen modules (0x2B, the
  row screens' data; 0x2A, the load screen) is not among the candidates.

### Tool changes

`tools/route_capture.py`: the opt-in group `opt` (OPT_BEATS, OptSampler /
decode_opt, opt_pin, opt_open, opt_cursor_to, opt_close, opt_toggle,
opt_enter_module, opt_screen_moves, opt_button_type, opt_press,
opt_event_keys; outputs `build/c10/options/`). `tools/route_census.py`:
`run --segments opt`, `opt-delta` (c10_group_delta, br-delta's report for
any AREA11 C10 group), `opt-save-scan` (the static save-path proof), and
the OPTIONS replays keep their own rows (as DAMAGE's and BRANCH's do). Both
are additive; no other group's behaviour changed.

### Rules kept

PCSX2 ran hidden, one session at a time, under the lock for the whole lane
(one acquisition, released at the end), and no PCSX2 process was left
running. No save-state slot 01..15 was written (the sstates listing equals
the one taken at the lane's start, `build/c10/options/sstates_before.txt`;
the probe sessions' snapshots, taken only to look at the row screens,
were deleted). The renderer setting was not changed (it is 17; the save
states' embedded screenshots were enough). No memory was written. No save
path was entered and no memory-card slot was chosen; the memory cards'
SHA-1s are unchanged across the lane
(`build/c10/options/memcard_hashes_before.txt`, `memcard_hashes_after.txt`).
