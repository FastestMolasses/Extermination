# Full run plan: one continuous guided play, New Game to the ending and into the second playthrough

Lane PLAN, 2026-11-02. Target: SCUS-97112, boot ELF SHA-256
`ee052236783e7d3e865754d3ff9fee71290addeb7d146c86caa7ff2724d1e17a`.

**Status: a plan and a tool design. Nothing here is recorded yet.** A later lane records it
(the emulator was busy with a capture lane while this was written). This document is the route
from New Game to the ending credits as ONE continuous play on the original, what each stretch
needs, the code facts behind each gate, where our captures already walked it, the 15 dog tags
and the knife, the completion save and the second playthrough, and how the scripted run is
recorded so that it plays in the port exactly like the user's own takes.

In-game text is described, never quoted (the area names below are paraphrases of the title
cards). Positions are world units, yaw is the player's +0xC4 (X = sin, Z = cos). Actor indices
`[n]` are the registry indices of WORLD_GRAPH.md section 6; `g[n]` a deferred group's record.
Items: `C64 0xNN` is the inventory byte D_00810C64 + 0xNN (take family 0), `CC3 0xNN` the
key-item / document array D_00810CC3, `CB8` the maps (FINDINGS "ITEM PICKUP SYSTEM FULLY
DECODED" section 5).

## 0. Rules of the run (user decisions, 2026-11-02)

1. **One continuous play from New Game.** No restart from a save, no constructed start, no
   memory write. The earlier beat-by-beat route (each beat from its own start card) looked
   disjointed to the user. The run may *save* (the game's own save screens) but never loads.
2. **Two recordings.** The user records a full game with their own pathing
   (`tools/record_original_play.py`, `build/human_play/`). We record our own scripted, guided
   run (this plan) in a separate folder (section 8).
3. **The guided run is 100% Claude.** Nothing is taken from the user's recordings: no pad
   input, paths, timings or positions copied or derived from `build/human_play/**` (run1,
   test1 or any later take), nor from the port's playbacks of them (`build/takes/**`). Allowed
   sources: our own scripted route recordings (`build/fork_refs/**`), the route docs' measured
   paths, the beat functions and path constants of `tools/route_capture.py` /
   `record_level_routes.py`, our code reads, and public guides (as leads). Section 8.6 says how
   the driver enforces it.
4. **Visible play.** The user watches the run as it is played: a visible fork window at normal
   speed (59.94 Hz) with sound, the window's own keyboard and controller inputs off. This is an
   exception to the hidden-only launch rule granted by the user for this run, as
   `record_original_play.py` documents for the user's own takes. Only the exactness check (the
   hidden replay of each take) stays hidden and fast.
5. **Every dog tag and Roger's knife**, then the ending, the completion save on the run's own
   card, and on into the second playthrough far enough to show the bonus items (section 5).
6. **Deaths are not part of the kept run.** A death, a missed jump or a soft lock is a failed
   try of that segment; the segment is retried from the previous take's end state (section
   8.4). Every try is kept as a recording.

Evidence marks per stretch: **played** (a route beat of the port's `*_LEVEL_ROUTE.md` walked
it; its paths are reusable as waypoints), **played: constructed** (walked only from a
constructed start; waypoints reusable, the state is not), **unplayed: code** (our C, the world
graph or a grid read covers it), **unplayed: guide** (only a public guide covers it; a lead,
not evidence). The guide used is the GameRevolution walkthrough,
<https://www.gamerevolution.com/?p=30320> (paraphrased briefly, never copied).

## 1. Area map: AREA number -> in-game place -> the guide's section

The title card draws D_002671C0[D_00289B40[area] + sub] (SCENE_COORDINATOR_DESIGN.md #30;
STATUS_UI_LEFTOVERS.md 2.6). Read from a fork snapshot's RAM (a07b's eeMemory.bin; the table is
the same in every area): D_00289B40[area] holds the base index in its low half. The strings
are paraphrased here.

| Area / sub | Title card (paraphrase, table index) | Guide section(s) | Our route |
|---|---|---|---|
| AREA11 | the base's rear entrance (26) | Rear of Compound; its second visit | FIRST |
| AREA01 s0 / s1 | the tunnel, area B (3, 4) | Section B; the drawbridge on the Underground Tunnel trip | SECOND, FOURTH, SEVENTH, EIGHTH, EIGHTEENTH |
| AREA00 s0 / s1 / s2 | the tunnel, area A (0..2) | Section A; s2 = Section A's boss and Security Room | THIRD; s2 constructed only |
| AREA02 s0 / s1 / s2 | the tunnel, area C (5); a supply room (6); area C (7) | Section C (s0 / s1); the Underground Tunnel trip (s2) | FOURTH, EIGHTEENTH |
| AREA04 s0 / s1 | storage and the level-1 lab's security (11, 12) | Underground Storage / LV1 Lab, its second and third visits | FIFTH, EIGHTH, SIXTEENTH .. EIGHTEENTH |
| AREA22 | the command centre's corridor (40) | (the way to the Control Tower) | SIXTH, EIGHTH |
| AREA06 s0 / s1 | the command centre (14, 15) | Control Tower; the drop to Maximum Security | SEVENTH, EIGHTH |
| AREA13 | the base's main compound (28) | Front of Compound | NINTH .. TWELFTH |
| AREA19 s0 | the water filtration plant (36) | Underground Water Filtration Plant | TENTH, TWELFTH .. FIFTEENTH |
| AREA19 s1 | the ground facility of building B (37) | the guide's "Residential Area" (B Wing's ground floor, the truck) | THIRTEENTH .. FIFTEENTH |
| AREA15 s0 / s1 | building B: infirmary and bar (30); roof (31) | B Wing, both visits | FOURTEENTH, FIFTEENTH |
| AREA03 s0 / s1 / s2 | the tunnel, area D (8); the level-2 lab's access way (9); its security (10) | Section D | FIFTEENTH .. SEVENTEENTH |
| AREA07 s0 / s2 / s3 | the level-1 lab: research labs, security corridor, restricted area (16, 18, 19) | part of the guide's Storage third visit (the first detonator) | SIXTEENTH, SEVENTEENTH |
| AREA08 s0 / s2 / s3 | the level-2 lab: the same three (20, 22, 23) | LV2 Lab (the second detonator) | SIXTEENTH |
| AREA14 | the power reactor area (29) | Power Plant | constructed only (k01_02 arrival) |
| AREA18 | the military transport plane (35) | Power Plant (the plane, Gary's bomb) | none |
| AREA17 | the power reactor building's roof (34) | Power Plant (Roger's fight, the cable) | none |
| AREA16 s0 / s1 | maximum security: corridor (32), lab (33) | Maximum Security Area | none |
| AREA20 | the secret dock (38) | Secret Naval Port | none |
| AREA21 | the hovercraft (39) | Hovercraft, Origin | none |

Every hypothesis of the brief holds except two refinements: the Control Tower is AREA06 (the
command centre; AREA22 is its corridor), and AREA00 sub 2 and AREA14 are confirmed, while the
guide's "Residential Area" is AREA19 sub 1 (building B's ground facility), not AREA15.
"Underground Tunnel" in the guide is the umbrella for AREA00..03 (the cards call them tunnel
areas A..D).

## 2. The route, stretch by stretch

Each stretch ends at a **checkpoint** (CP): the tick (main-loop counter 0x70003B64) at the end
of the take that closes it, with area bytes D_00810700..02, health, infection, charge and the
item bytes, recorded in the chain's `chain.json` (section 8.3). A checkpoint is a place in the
continuous run, never a load. Stretches S01..S19 reuse played paths; from S20 on the run is
unplayed.

### Part A: the first half, as recorded (S01..S19)

**S01 Title -> New Game -> first control (AREA11).** From the fork's title state
(`build/startup-reference/fork-states/slot01_title/state.p2s`, the start every take uses), New
Game, the opening and its movie, control. Played: the C10 pass `dmg_04_new_game` and the port's
`newgame-control` check cover this flow; the beats of S02 start from save state 04 (first
control), so S01 itself must be driven fresh from the title. CP01 first control.

**S02 AREA11, the rear entrance.** FIRST_LEVEL_ROUTE.md beats 00..15: the battery (item 0x1B,
the 6-gauge pack), the panel and the elevator, crates, the slide, the truck, the fence door,
the cage roof (Roger), the crevice, the east tower, Roger's encounter, the exit to AREA01 entry 4
(0x823C40). Take the pickups the BR pass (CAPTURES_C10.md) recorded: the ledge ammunition, the
map, the magazine, the cage key, the yard and tower ammunition (C64 0x10 g[5]; healing C64 0x1E
g[1], g[3], 0x1F g[2]). Played. CP02 AREA01 arrival.

**S03 AREA01 sub 0, tunnel B.** SECOND_LEVEL_ROUTE.md a01_00..a01_07 plus the side items
(a01_s0..s7: the NPC's talks, the control room's items, the duct). Played. Exit to AREA00 entry 0.

**S04 AREA00 subs 0 / 1, tunnel A.** THIRD_LEVEL_ROUTE.md a00_00..a00_10 (the padlock, the cage
terminal that moves the train, the ferry deck, the duct, the switch) back to AREA01. Played.

**S05 AREA01 -> AREA02, tunnel C and the supply room.** FOURTH_LEVEL_ROUTE.md a01r_00..03 (the
event gives C64 0x20; the bridge crossing sets flag 0x08), the bed a02_s0 (C64 0x20 used:
infection to 0), a02_00..05 (duct, switch, ladder escape, the wreck, the panel [24] with the
charge: D_00810843 bit 3), door [25] to AREA04 entry 0. **Dog tag 5** on the y-15 platform (section
4). Played except the tag. CP05 AREA04 arrival.

**S06 AREA04 sub 0, storage (first visit).** FIFTH_LEVEL_ROUTE.md a04_00..05 (door [40], the
console [69] that rolls the reel, back to the hall, the conveyor, door [37] to AREA22). Played.

**S07 AREA22 -> AREA01 upper floor -> AREA06.** SIXTH_LEVEL_ROUTE.md a22_00..02;
SEVENTH_LEVEL_ROUTE.md a01u_00..02 (the gap jump, the catwalk event, door [19]). Played.

**S08 AREA06, the command centre.** SEVENTH_LEVEL_ROUTE.md a06_00..03 (the beam, door [2], the
keypad: D_00810845 bit 5), then **not** the beam's collapse (a06_04: its pit's door [3] needs
D_00810847 bit 2, which comes only in Part C); back over the beam by the running jump
(EIGHTH_LEVEL_ROUTE.md a06b_00, a06b_01). **Dog tag 4.** Played except the tag.

**S09 AREA01 -> AREA22 -> AREA04 sub 0 (storage, second visit) -> AREA13.**
EIGHTH_LEVEL_ROUTE.md a01v_00..02, a22b_00..01, a04b_00..04: door [45] (bit 5), the room behind
it (the guide puts the Shotgun Unit, the Zoom Scope and shells there: fit them before the
fight), the NPC [2] (0x824490: C64 0x23, the Alpha card), the reader [50] (D_00810845 bit 7,
flag 0x12 = 1), the event [3] and its creature, the lift [51]. **Change from the recording:
kill the creature** (a04b_04 rode past it alive): **dog tag 1** appears only after flag 0x12 =
0xFF (section 4). The fight is recorded on the fork (`build/fork_refs/boss_fight_20261030/`,
a04b_g0..g5 with the shotgun, a04b_s5..s7 with the pistol; HANDOFF 2026-10-30; CURIOSITIES
entry 37: one shotgun shell lands one pellet's damage per tick, six shells kill it; the x5 spot
on bone 4; it cannot be hit until the player is at y >= 47 inside D_00245020). Played (the
boss fight from its own start card; reuse its tactics, re-steer live). CP09 AREA13 arrival.

**S10 AREA13, the main compound.** NINTH_LEVEL_ROUTE.md a13_00..a13_03 (door [8] and [3]'s
scene, the button [15], door [17] from the east: [4]'s scene gives C64 0x1A, the cold
protection; the pickup g[5]: C64 0x27, the crowbar). **Skip the hatch [62]** this time
(a13_04 / a13_05 and the tenth level's AREA19 entry-9 rooms and climb back, a13b_*, were a
detour: that side is the guide's B Wing second visit, S23). Then ELEVENTH_LEVEL_ROUTE.md
a13c_00..06 (the recharger [60], the battery machine [44] and its blast, the cure g[3] (C64
0x22), the boom and [7]'s area) and TWELFTH_LEVEL_ROUTE.md a13d_00..07 (the south roof: stair,
ladders, the running jump; door [20]; the hatch [63] with the crowbar). **Dog tag 12** on the
roof by the south-east roof ladder (a13b_s0 ended 6 units from it). Played; but a13c was played
from a13b_01, after the detour: the freed [3] / [4] shift later pool nodes (TENTH section
"Back to AREA13"), so steer by positions and re-find owners in the live table. The guide puts
a two-Predator fight (flamethrower) in this area; our captures met no such fight on this line
(guide-only, see D11). CP10 AREA19 entry 10.

**S11 AREA19 sub 0, the filtration plant.** TWELFTH a19b_00..02 (the arrival scripts, the east
ledge, the ladder at z 974: [6]'s first stage); THIRTEENTH a19c_00..07 (the bar and its swing,
the second bar into [7]'s room, [7]'s Use: D_00810854 bit 2, door [27], the ladder at z 859.5 to
sub 1 entry 7, door [52]: [34]'s scene). **Dog tag 11** by the y-190 platform near a19b_01.
Played except the tag.

**S12 AREA19 sub 1 -> sub 0 -> the stair tower -> AREA15.** FOURTEENTH a19d_01..20 (the valve
[37] raises the lift [36], the cage ladder down, sub 0's west part, the bars 858 / 1195, the
ladders 1016 and 694, the seal [48] broken from inside, door [50]). Played.

**S13 AREA15, building B.** FOURTEENTH a15_00..01, FIFTEENTH a15b_00..04: sub 0's [0] scene
(flag 0x22), door [51] to sub 1 (the roof), the mezzanine event (0x823C80: C64 0x06, the dog-tag
radar unit the guide names; D_00810730 rewritten for AREA00, 01, 02, 04, 06), the forced return,
sub 0's [1] / [2] (C64 0x25, the truck key), the bed [16] with C64 0x20 (health 100,
infection 0), door [14]. **Dog tag 9** on sub 1 (the roof, near (863, 290, 973)). Played except
the tag.

**S14 AREA19 sub 1 -> sub 0 -> AREA03.** FIFTEENTH a19e_00..07: the hall's two creatures, the
truck [46] opened with C64 0x25 from the EVENT page, its g[0] (C64 0x1C, the 18-gauge pack), the
deck's panel [24] (32 half-units: D_00810854 bit 1), door [25] to AREA03 entry 1. Played.

**S15 AREA03, tunnel D and the level-2 lab's access.** FIFTEENTH a03_00 (the lift), SIXTEENTH
a03b_00..06 (door [13]; the ducts; [22]'s room: flag 0x29, C64 0x2A, the metal plate; back;
the slider [29]; sub 2's lift [7] with C64 0x2A: D_00810844 bit 7). **Dog tags 6** (sub 0), **7,
3, 13** (sub 1) and **10** (sub 2). Played except the tags. (Order note D2.)

**S16 AREA08, the level-2 lab.** SIXTEENTH a08_00..06 (door [17], the magazine g[3], the switch
[21], the slider, the ceiling crossing of sub 2's live floor, sub 3's [13] / [2]: flag 0x18,
the lift back to AREA03 sub 2 entry 1). Played. Sub 0's door [6] (0x823870) and the flag 0x17
scenes behind it were not played (see D3).

**S17 AREA03 -> AREA04 sub 1.** SIXTEENTH a03c_00..01, then SEVENTEENTH's branch a03d_00..01
(the seal [23] broken while leaving the creature at once: infection stays 85; door [24] to
AREA04 entry 2 on the west balcony's south ledge; D_00810845 bit 2). Played.

**S18 AREA04 sub 1, storage (third visit).** SEVENTEENTH a04d_00..02 (down into the hall,
0x825040's polygon: flag 0x28, C64 0x29, the guide's meeting with Gary; the slider [73] to the
north room; the socket [61] and the lift [62] to AREA07). The guide names two Gunners here (snipe
them from the upper level with the Zoom Scope). Played.

**S19 AREA07, the level-1 lab.** SEVENTEENTH a07b_00..07 (switch [28], the side crawl in sub 2
with g[0], sub 3 door [11], [17]'s script 0x823C20: flags 0x15 and 0x25 = 0xFF, the lift [0]
back to AREA04 sub 1 entry 10). Played. CP19 AREA04 sub 1 north room, flag 0x25 set.

### Part B: from the storage hall to the power plant (S20..S27)

**S20 AREA04 sub 1: the hall to the upper level and door [44].** a04e_00 (the north room to the
hall, slider [51]) is played. Then the way onto the west balcony's south ledge (door [44] to
AREA03 entry 0, sub 1; WORLD_GRAPH section 6: open). **Unplayed: guide + code; the run's first
real blocker (B1).** The guide (storage, second and third visits, and the rear-of-compound second
visit) has the upper level reached by two switches: one raises a platform, the forklift's side
switch raises its crate, and the player climbs the crates and jumps. Code: the console [69]
(0x825510, C) raises the platform [68] (counter 0x65; played as side beat a04e_s0, not ridden);
the console [37] (0x825880, C: script 0x82C1F0) moves the lift [36] (0x825B00, C: counter 0x5C)
up 10 to y 24.9 beside the pillar (top 53.9, at the balcony's unfenced edge x 390..404); the box
stack (tops 28.9 / 42.9) meets the south ledge (y 55) under a fence of attribute 0x50 (EIGHTEENTH
sections 2 and 7b). Leads, in order: [37] then [36] as a step onto the pillar and the north strip;
the platform [68] up then a jump; the box stack and the fence (attribute 0x50: test whether it
is a ledge-climb face). Scout it from the chain's own CP19 state (section 9). CP20 AREA03 sub 1.

**S21 AREA03 -> AREA19 -> the tower: item 0x24.** AREA03 sub 1 door [25] to sub 0 (entry 0),
sub 0's slider [15] to AREA19 entry 0 (the y-265 deck), the ladder 694 to sub 1 entry 8 (the
tower; played as a19d_16 / a19e_s0), up to the y-450 landing: [53] (0x827550, C) with flag 0x25 =
0xFF and flag 0x26 clear, the player at y 445..460 inside the polygon 0x82F820, plays script
0x82D590 and gives C64 0x24 (the Beta card; the guide: from Sonia's body). Unplayed: code (the
ladder and the tower are played). **Dog tag 11** if not taken in S11.

**S22 AREA15 sub 0: Cindy in the infirmary.** The guide's B Wing second visit. Code lead: AREA15
sub 0's 0x824E50 (owners [8] / [9]) sets counter 0x2C = 1 and its script 0x829330 sets flag 0x2C
= 1 and gives CC3 0x15; flag 0x2C is the flag that lays AREA01's drawbridge flat (0x826200, C;
EIGHTEENTH 7b item 5). Whether 1 is enough for the bridge, and what gates 0x824E50, is not read
(D6). The bed [16] again if infection is up. Unplayed: code.

**S23 To AREA13 by the storage's upper level and lift; the tool-shed hatch [62] -> AREA19
entry 9 -> the 24-gauge pack.** The guide: back up to the storage's upper level, the lift to
the compound, the crowbar on the tool shed's hatch, a slope and a laser trap, the plant's
platform with the 24-slot battery parts. Code: AREA19 door [25] (bit 1 already set) to AREA03
entry 1 -> sub 0 lift -> door [13] -> sub 1 door [24] to AREA04 entry 2 (the south ledge) -> the
east balcony -> lift [57] (flag 0x14 already 0xFF) -> AREA13 entry 0 -> the hatch [62] (a13_04,
a13_05, played) -> AREA19 entry 9 -> the duct (a19_00, played) -> the room behind door [32]
(g[2], a19_01) -> the reader [21] with C64 0x24 (D_00810854 bit 0) -> door [22] (room move) ->
sub 0's g[4], C64 0x1D, the 24-gauge pack, at (877.2, 210, 976). Unplayed: code beyond a19_01;
the south ledge -> east balcony link is the same open question as S20 (the beam at z 376 is a
one-way way down into the hall).

**S24 Back to the storage's ground floor.** AREA19 entry 9's ladder -> AREA13 entry 6 (a13b_00)
-> door [17], button [15], door [8], the lift [10] (a13b_01..05) -> AREA04 entry 7 on the east
balcony -> down to the hall floor -> the slider [41] (a04e_01, played) -> AREA02 sub 2 entry 4.
Mostly played.

**S25 AREA02 sub 2 -> AREA01 sub 1 -> the drawbridge -> AREA00 sub 2.** a02c_00 (the seal [21]:
D_00810843 bit 0 and D_00810842 bit 1) and a02c_01 (door [22] to AREA01 entry 3), played. **Dog
tag 5** here if not taken in S05 (the same pickup id in subs 0 and 2). The guide adds an ICU and a
save. Then the bridge south to door [9] (AREA00 entry 0, sub 2). **Blocker B2:** played once
(a01w_s0_bridge) with flag 0x08 set and 0x2C clear: the south half stood raised and the walk fell
to its death. Unplayed: guide (crossable) vs code (D6).

**S26 AREA00 sub 2: Section A's boss, the Security Room, the train.** The guide: a tentacled
boss with a core (keep moving, shoot the exposed core, keep to the wall beside it, avoid the
wall's gas; a grenade launcher is picked up there), then the Beta card at the Security Room's
door, the Train Key, the train's red panel from the item menu. Code: the arrival corridor is
closed to the reader by player-only walls 1584..1586 and a hive mound (EIGHTEENTH 7c; **blocker
B3**: lead: sub 2's [30] 0x825E80 writes counter 0x2B = 3 and flag 0x2B is the only flag AREA00
tests, so the boss's end is the likely opener); the reader [19] at (-185, -45, -1499.1) facing -x
with C64 0x24 (001576E0, BM: D_00810841 bit 4), the slider [20], g[3] (C64 0x26) at (-214.3,
-48.9, -1477.9), [28] (0x826790) with C64 0x26 used from the EVENT page (00184D20: class 4 model
0x49 -> +0x0B = 5) at its Use point (33.9, -47, -1630.4), yaw about 1.92, script 0x82D070, AREA14
entry 0. Played: constructed (k01_01, k01_02: the slab, the ledge climb onto [23], the EVENT page
on its second entry; reusable waypoints only). CP26 AREA14 entry 0.

**S27 AREA14, the power reactor area.** Code (EIGHTEENTH 7c and section 8): entry 0's valley (y
390); C64 0x19 (the climbing spikes) is g[1] at (394.7, 448.5, 1375.5) on the bench (y 430..450),
**blocker B4**: the way up from the valley was not found (walls at x 484 / 522, pockets at y 410 /
425: ledge-climb leads; the guide: a box beside a sentry gun); the 0x33 climbing face (nodes
1465..1479, 1618) needs C64 0x19 held (0015D4C0, NM); its base is in [0]'s polygon (0x8238E0:
script 0x826C10, CC3 0x16); **dog tag 2** at (349.5, 431.9, 1283.6), 30 units from the climb's
base; at the top, [2] (0x823B30) gives C64 0x0F (the guide: full-auto parts from the Major).
Unplayed: code.

### Part C: the power plant to the ending (S28..S35)

**S28 AREA14 -> AREA18, the plane -> AREA14 entry 1.** 00190F20 sends the player to AREA18
inside the quad D_0024A4B0 ((754.1, 829.6), (767.6, 795.0), (809.8, 814.3), (791.2, 840.5));
AREA18's g[1] is C64 0x28 (the guide: Gary's bomb) at (521.3, 153.1, 184.4); its recharger [21];
back at x <= 285 to AREA14 entry 1, where [4] (0x823E30: flag 0x3F, CC3 0x18) plays. The guide:
Gunners in the cargo hold, crates to break, a jump to the opposite ledge. Unplayed: code.

**S29 AREA14 [6] -> AREA17.** [6] (0x825FD0 at (229.5, 680, 1202.8)) is usable only while C64
0x28 is held; Use point (222.5, 680.1, 1185.1), r 7, h 20, yaw about -1.85; script 0x828120, its
callback 0x825C40 requests AREA17 entry 0. Unplayed: code.

**S30 AREA17, the reactor roof: Roger.** [30] (0x823BA0, C) runs the fight's scripts and spawns
the boss group 0x8263C0 (00133E30 at (340.45, 220, 269)); the guide: grenade launcher, keep
moving, shoot the cores. At its death 0x824280 (C: the effects 0x8000005C and the purple pool
0x80000004, then group 0x826500) drops **Roger's knife, C64 0x2C**, at (340.0, 221.5, 269.0)
(identification: the guide says the knife and dog tags drop from Roger; the code drops item 0x2C
where the boss stood). **Dog tags 14 and 15** lie on the roof from the arrival (section 4). Then
[33] / [34] (the Major's lift; 0x824700 takes C64 0x28, gives C64 0x2B, the guide's program disk,
and CC3 0x19, and drops C64 0x18, the pulley harness, at (511.5, 220.3, 316.5)); the cable ride
(0016BC40, BM: state 22, entered with C64 0x18 on a floor of attribute 0x20 under a cable of
attribute 0x3C; it is the only writer of counter 0x5D); [38] (0x825470) -> AREA11 entry 3 (465.2,
270.5, 426.4). Unplayed: code.

**S31 AREA11 again -> AREA01 -> AREA02 -> AREA04 sub 1 -> AREA03 -> AREA19 -> AREA15: Cindy
takes the disk.** At AREA11 entry 3, [11] (0x823CE0: script 0x828C70) sets flag 0x30 = 0xFF.
AREA11's area exits are 0x823C40 / 0x827630 to AREA01 entry 4 (sub 1 now). The guide: the gaps,
the fan, a save room and the ICU, three Gunners in storage, the forklift crates and a jump to a
door, the stairs down to the filtration plant (optional harness zip lines there: the six-shot
grenade launcher and missiles), the ladder up to building B. Code: AREA01 sub 1 door [11]
(bit 1, from S25) to AREA02 e0 -> sub 2 [25] (bit 3) -> AREA04 entry 0, sub 1 -> the upper level
(as S20) -> door [44] -> AREA03 -> [15] -> AREA19 -> the tower -> AREA15: 0x824990 exists only
with flag 0x30 = 0xFF and until flag 0x2A; at entry 1 its 0x824B40 script takes C64 0x2B and
sets D_00810847 bit 2 (and CC3 0x1C). Unplayed: guide + code.

**S32 To AREA06 and down into maximum security.** AREA15 -> AREA19 sub 1 -> ... -> AREA04 sub 1
(the east balcony) -> door [43] -> AREA22 -> [8] -> AREA01 entry 6 (upper floor) -> [15] ->
AREA06 -> now trigger the beam's collapse (a06_04, played) and fall into the pit -> door [3]
(sub 0) / [1] (sub 1) with bit 2 -> AREA16 entry 0. Partly played (a06_04, a06_05 locked).

**S33 AREA16, maximum security.** The guide: the water barrier (slash the pipe, freeze it with
the wall's cooling unit), the 24-slot battery door, the third launcher part, the tanks opened.
Code: sub 0's panel [2] (00158EC0, model 0x23: D_00810851 bit 1; its charge cost is +0x34 of that
model, not read; the guide says it needs the 24-gauge pack) opens the room move [3]; [1] reloads
sub 1 (the lab); sub 1's [11] (0x824690) runs flag 0x31's event; [7] -> AREA06 entry 3, where
0x825E20 (flag 0x31 set) plays script 0x8276C0: flag 0x2A, D_00810845 bit 4, CC3 0x1F. Unplayed:
code.

**S34 AREA06 -> AREA01 -> AREA22 -> AREA04 sub 1 door [48] -> AREA20, the secret dock.** Door
[48] needs bit 4 (the guide: the room right of the level-1 lab's entrance; recharge first).
AREA20: **dog tag 8** at (27.7, 90.8, 231.2); [32] (0x823B00 / 0x823BE0: counter 0x34, flag
0x34's script, then a quad) requests AREA21 entry 3 (the guide: approach the hovercraft).
Unplayed: code.

**S35 AREA21, the hovercraft: Origin.** The guide: tell Cindy to lock the doors; watch where the
creature is (rear: the truck's cannons; sides: the zip line to the walkway's Gatling gun, do not
overheat it, shoot the blobs); after several hits it boards (shoot its yellow core with the
Gatling or missiles); then a double (climb a crate, missiles or grenades from cover). Code:
0x826840 / 0x8268F0 / 0x8269A0 (counter 0x36 = 0x11, 0xFF, 0x10; flag 0x36's script 0x82C5C0);
the doors [27] / [33] (0x826E60) and [30] (0x826BB0, which writes the lock bytes indexed);
0x826310 (counter 0x35, D_00810856); [53] (0x827040) after flag 0x36's event plays script
0x82CBD0 (flag 0x37) and stores D_008106B8 = 1. Unplayed: code. CP35 the ending starts.

### Part D: the ending, the completion save, the second playthrough (S36)

**S36** follows section 5: the end task, the ending pictures (skippable), the save screen on the
run's own card, then on into the second playthrough to first control and the status screen's
ITEM page. Unplayed: code (section 5 says which parts were read).

## 3. Health, ammunition, battery and infection

- **Healing items** (C64): 0x1E (+30 health, measured 45 -> 75 a02c_00), 0x1F (health to 100,
  a03b_03), 0x20 (used at an MTS bed: infection to 0; FOURTH a02_s0, FIFTEENTH the bed [16]),
  0x21, 0x22 (health 100 and infection 0, ELEVENTH a13c_04). Take every 0x1E..0x22 pickup on the
  line (WORLD_GRAPH section 6 "Pickups" per area; e.g. AREA00 s0 has five 0x1E / 0x21, AREA04
  0x1E x2 per sub, AREA06 three 0x20, AREA14 two 0x1E and two 0x21, AREA17 0x1E and 0x1F,
  AREA20 three 0x1F, AREA21 0x1E, 0x1F, 0x20). Keep two in the bag before every boss (S09, S26,
  S30, S35) and use the HEALING page when health falls under 50 outside a fight.
- **Infection**: bites raise it (+10, +15); at 100 health is capped at 60 and drains 2 per
  about 240 frames in play (SIXTEENTH section 4; CURIOSITIES entry 10). Reset it at the beds
  (AREA02 s0 bed [0] in S05; AREA15 bed [16] in S13 and S22) and with C64 0x22. Arrive at AREA13
  (S10), AREA14 (S27) and AREA21 (S35) at infection 0 if possible.
- **The cold**: AREA13 outdoors drains health without C64 0x1A (0015D100 returns early while
  D_00810C7E != 0; TENTH). Take [4]'s scene (door [17] from the east) before any long outdoor
  stretch.
- **Battery charge** (D_00810CB2, half-units): rechargers are 00159210 model 0x2C (AREA02 s1 [4],
  AREA07 s0 [20], AREA08 s3 [8], AREA13 [60], AREA15 s0 [26], AREA16 s1 [6], AREA18 [21], AREA21
  [38]). Pack capacities: 0x1B 12, 0x1C 0x24, 0x1D larger (the guide: 24 slots). Every save costs
  4 (SAVE_GAME.md: save mode's Yes takes 4 off D_00810CB2). The panel [24] (AREA19) needs 32;
  AREA16's panel [2] needs the 24-gauge pack (guide). Recharge before S14, S23, S33 and S34.
- **Ammunition**: SPR4 magazines (C64 0x10; the reserve is capacity x 30); the shotgun from
  storage's door-[45] room for S09; the guide's grenade launcher (AREA00 sub 2, S26) for the
  Section A boss and Roger; missiles for Origin. Take every 0x10..0x16 pickup on the line.
- **Saves**: optional, at the save points the line passes (they are play, not loads), never
  loaded. Two are worth their charge: one before S20's scouting is replaced by real play, and
  one before S35. The completion save of S36 is required.

## 4. The 15 dog tags and Roger's knife

**Count and identity (code).** The item catalog's lines 45..59 are the 15 RECON dog tags
(FINDINGS "MESSAGE BANK EXPORTED"; CURIOSITIES entry 9), i.e. items C64 0x2D..0x3B (the
catalog line is the item id). Scanning every area's deferred groups for take family 0 (C64) with
item 0x2D..0x3B (build/s87/areaNN/overview.json of `tools/area_overview.py`; family-2 records of
the same numbers are documents, CC3) finds exactly one pickup site per id, 15 in all: the
public count of 15 holds. Every one is 0015AFA0 with library model 0x57.

| Tag (item) | Area / sub, record | Position | Condition | When in the run | Detour |
|---|---|---|---|---|---|
| 1 (0x2D) | AREA04 s0 g[11] (s1 g[10]); group 0x827230 at the death | (544.9, 55.0, 255.7) | cond 3: flag 0x12 = 0xFF and not taken; cond 4: counter 0x12 != 0 (the group 0x824A40 spawns) | S09, after killing [3]'s creature | the fight itself (several minutes) |
| 2 (0x2E) | AREA14 g[10] | (349.5, 431.9, 1283.6) | not taken | S27, at the climb's base | small |
| 3 (0x2F) | AREA03 s1 g[7] | (641.5, 25.2, 676.7) | not taken | S15, by [22]'s Use point (649.8, 684) | very small |
| 4 (0x30) | AREA06 s0 g[4] (s1 g[4]) | (-304.1, 60.2, -684.5) | not taken | S08 | unknown: 174 units west of door [19]'s arrival (-130.1, 60, -670.9), same height |
| 5 (0x31) | AREA02 s0 g[17] (s2 g[4]) | (316.0, 15.9, 96.4) | not taken | S05 (or S25) | unknown in sub 0; in sub 2 on the y-15 platform, 123 units west of entry 4 (439, 15, 99) |
| 6 (0x32) | AREA03 s0 g[1] | (357.8, 0.2, 502.3) | not taken | S15, after the lift ride down | unknown (not walked) |
| 7 (0x33) | AREA03 s1 g[2] | (452.2, 25.2, 718.2) | not taken | S15, after door [13] | about 95 units from door [13]'s arrival (432, 25, 625.6) |
| 8 (0x34) | AREA20 g[6] | (27.7, 90.8, 231.2) | not taken | S34 | unknown |
| 9 (0x35) | AREA15 s1 g[6] | (863.4, 290.1, 973.3) | not taken | S13 (the roof) | unknown (not walked) |
| 10 (0x36) | AREA03 s2 g[0] | (763.3, 25.1, 629.9) | not taken | S15, at sub 2's entry 0 (770, 25, 672) | very small |
| 11 (0x37) | AREA19 s0 g[10] | (1071.1, 190.2, 990.8) | not taken | S11 (or S21) | small (lead): the east y-190 platform of a19b_01 |
| 12 (0x38) | AREA13 g[10] | (772.5, 215.3, 1232.6) | not taken | S10 (the roof) | small: 6 units from a13b_s0's end |
| 13 (0x39) | AREA03 s1 g[8] | (521.4, 55.1, 840.7) | not taken | S15 (the y-55 level) | unknown |
| 14 (0x3A) | AREA17 g[1] | (407.0, 220.1, 284.0) | not taken | S30 | unknown (not walked) |
| 15 (0x3B) | AREA17 g[2] | (495.0, 228.2, 215.6) | not taken | S30 | unknown (not walked) |

Tags with two records (1, 4, 5) share one taken bit (the same puid in both subs), so each is taken
once. Taken tags stay in C64; nothing on the line uses them up (no 001C47E0 call names 0x2D..0x3B).
The radar unit (C64 0x06, S13) helps find them (guide).

**Roger's knife: C64 0x2C**, dropped by 0x824280 at Roger's place (S30). It is the flag the
ending reads for the healing bonus (section 5).

## 5. The ending, the completion save and the second playthrough (code)

Read from the byte-matched C of `src/func_001AD740.c` (task slot state +9 = 3 of the game task
001AD250, BM). Its header still calls it the game-over / continue task; that is a label (it
predates this read): what it writes is the ending and the second playthrough's start. What
enters it: 001AD010 (BM), at the fade's hold-black with D_008106B8 set, moves the task to +9 = 3
when the request byte 0x70003B93 is non-zero (2 also stops the stream). AREA21's [53] stores
D_008106B8 = 1; **who sets 0x70003B93 for the ending was not read.**

- **State 0 / 1** (only when 0x70003B93 == 1): a 0x30-frame wait, then a fade and a wait for
  Start (pad bit 0x800) or 0022D380, then 001B0C00(4).
- **State 2, the completion.** Before anything else it reads the bag: `knife` = C64 0x2C
  (D_00810C90) non-zero; `tags` = how many of C64 0x2D..0x3B are non-zero. Then: the clear
  counter D_0081070A (save-block byte 0x0A) + 1, saturating at 0xFF; the new game 001AF2C0 (the
  0x640-byte block reset) with D_00810750 (block +0x50, the play time) kept; AREA11 sub 0 entry 0;
  8 SPR4 magazines (001C40B0(0x10, 8)); the pools D_00810CA8 = 50, D_00810CAA = 30, D_00810CAC =
  10, D_00810CB0 = 25. **With the knife:** C64 0x1E x20, 0x1F x5, 0x20 x5, 0x21 x5, 0x22 x3 (the
  healing items). **With all 15 tags:** C64 0x03 + 1 (a weapon attachment: catalog lines 0..15;
  which one is not named here), D_00810CAA = 99, D_00810CA6 = 3 (C64 0x42). The guide does not
  state these rewards; they are code only.
- **State 3**: the ending pictures (two image packets shown in turn, countdowns of 0x1C60 and
  0x1AC frames, Start skips), with 001FF080(0, 0x37) at its start.
- **State 4 / 5**: the save screen 00225A00, then 00225AC0(1) until done. This is "the game asks
  to save": the block saved is already the second playthrough's start with the bonuses. The slot
  list then shows the glyph 0x9E for a block whose byte 0x0A is set (SAVE_GAME.md, the slot list).
  Whether this save also costs 4 charge (the ordinary save mode does) was not read; after the
  reset the charge is the new game's.
- **State 6**: the task ends (+9 = 4, 001ADF00). What runs next (presumably the new game's
  AREA11 start) was not read: the recording shows it.

**What the run does (S36):** let the ending play (do not skip the pictures in the kept take, so
the user sees them; a skip is one Start press if time matters), save to the run's own card (its first
free slot) when asked, then keep playing: through the second playthrough's opening to first control,
open the status screen's ITEM page and hold it long enough to show the magazines, the healing
items and C64 0x03 / 0x42, close it, and end the chain there. If state 6 returns to the title
instead of the new game, the run picks LOAD GAME, the completion save, and plays to the same ITEM
page: that is the game's own path, not a restart (the run did not die), and it is recorded in
the same chain. A side take from the title (New Game on a fresh card) is not needed.

## 6. Guide and code: disagreements and open blockers

| # | Topic | Guide (lead) | Code / captures | Status |
|---|---|---|---|---|
| B1 / D1 | Storage's upper level from the hall (S20, S23, S31) | two switches: a platform, then the forklift's side switch raises its crate; climb and jump to the door | only the console [69] / platform [68] was tried (it rises 82 units from the console; joins the balcony's north part only); the lift [36] / console [37] at the pillar is the untried lead; the beam at z 376 goes one way down | **open; the first thing to scout** |
| D2 | Order of the labs | Section D first (metal plate, the level-2 lab's door unlocked, not entered), storage (Gary, AREA07's detonator), back up to door [44], then AREA08 | we did AREA08 first, then AREA04 sub 1 and AREA07; flag 0x25 is set by whichever lab is second (both C) | both orders work by the code; ours is played and shorter after B1 (door [44] -> AREA03 -> AREA19 directly) |
| D3 | Leaving the level-2 lab | an air vent and a pipe to the residential area | AREA08's only area exit is the lift [8] to AREA03 sub 2 (WORLD_GRAPH section 6); sub 0's door [6] (0x823870, flag 0x17 scenes) is unplayed | the plan uses the lift and AREA03's [15]; door [6] to be looked at |
| D4 | The truck's battery pack | 16 slots | C64 0x1C capacity 0x24 half-units (001C40B0 case 0x1C, NM; measured) | the guide's number is wrong or counts differently |
| D5 | First way into the filtration plant | the detonator's bridge and a hatch; the tool shed's hatch "for later" | hatch [63] behind door [20] (played) first; hatch [62] leads to entry 9 rooms whose door [22] needs C64 0x24 | agree; our ninth / tenth levels took [62] first as a detour |
| B2 / D6 | The drawbridge on the way to AREA00 sub 2 | crossed | 0x826200 (C): tilted 60 degrees while flag 0x08 is set, flat when flag 0x2C is set; a01w_s0 fell; flag 0x2C = 1 by AREA15's 0x824E50 script 0x829330 (S22) is the lead | **open** |
| B3 / D7 | AREA00 sub 2's arrival corridor | a boss arena | player-only walls 1584..1586 and a hive mound close it; [30] 0x825E80 (counter 0x2B) is the lead | **open** |
| B4 | AREA14 valley -> bench (C64 0x19) | a box by a sentry gun | not found (walls, pockets at y 410 / 425) | **open** |
| D8 | Dog tag 1's place | a cheat list: "on the treadmill" | fixed (544.9, 55, 255.7), past the conveyor's top (CURIOSITIES 40) | code decides |
| D9 | Roger's dog tags | Roger leaves his tags and knife | Roger's death drops only C64 0x2C; tags 14 / 15 are on the roof from the arrival | code decides; take all three |
| D10 | The storage creature's tag | where it falls | cond 3 / 4 records at a fixed place, after flag 0x12 = 0xFF | agree in substance |
| D11 | Two Predators at the compound | a flamethrower fight | no such fight met on our AREA13 line (two hits in the field in a13c_03, source not read) | unread |
| D12 | The residential area | separate from B Wing | AREA19 sub 1 (building B's ground facility) | naming only |
| D13 | Shotgun unit and zoom scope | listed in storage's first and second visits | in the room behind door [45], which opens with the keypad's bit 5 (second visit) | second visit |
| D14 | The ending's trigger | n/a | who sets 0x70003B93 at the ending, and what follows the end task's state 6 | read before S35 if cheap; the run shows it |

## 7. Checkpoints (named in the chain, never loaded)

CP01 first control; CP02 AREA01 arrival; CP05 AREA04 arrival; CP08 keypad done; CP09 AREA13
arrival (tag 1 held); CP10 AREA19 entry 10; CP12 AREA15 arrival; CP13 C64 0x25 held, infection
0; CP14 AREA03 arrival; CP15 C64 0x2A held, tags 3, 6, 7, 10, 13; CP16 flag 0x18; CP19 flag 0x25;
CP20 door [44] passed; CP21 C64 0x24; CP23 C64 0x1D; CP25 AREA00 sub 2; CP26 AREA14; CP28 C64
0x28; CP30 C64 0x2C, 0x2B, 0x18, tags 14 / 15; CP31 D_00810847 bit 2; CP33 D_00810845 bit 4;
CP34 AREA21; CP35 D_008106B8 = 1; CP36 the completion save written; the end: the ITEM page of
the second playthrough. Each CP is the end tick of the take that reaches it, with its machine
hashes (`end.json`).

## 8. Recording design

### 8.1 What a guided take is

Exactly a user take (README "Recording your own gameplay"; `tools/record_original_play.py`):
a folder with `input.log` (the fork's 32-byte input records, the pad it applied per vsync),
`rows.tbl` (pad block, position, counter, area, rand, clock per game tick), `movie.tbl`,
`start.json` / `end.json` (vsync, tick, EE cycle, PC, region hashes), `end.p2s`,
`card_start.ps2` / `card_end.ps2`, `verify.json` (the hidden exact replay) and `take.rec` (the
EMREC the port plays). A take starts from the previous take's `end.p2s` and card, so the chain
is one machine history from the title. The port's `tools/watch_takes.py` stitches a chain by
the `after` links of `start.json` and plays it tick for tick (rand and clock columns follow).

### 8.2 The driver: `tools/record_guided_run.py` (new; not written by this lane)

- It imports `record_original_play` and reuses `arm`, `disarm`, `machine`, `records`,
  `timeline`, `use_card`, `replay`, `write_rec`, `first_difference` (all take a take folder as
  an argument; it does not call `play` / `verify` / `listing`, which use the module's
  `build/human_play` root).
- **Launch** (the visible exception, section 0 rule 4): `LaunchConfig(iso, statefile=<previous
  end.p2s or the title state>, rtc=RTC, renderer=17, present=True, unlimited=False,
  audio_backend='Cubeb', lease_s=86400, ini_overrides={'InputSources': {'Keyboard': 'false',
  'SDL': 'false'}, 'SPU2/Output': {'OutputMuted': 'false'}})`, with the default launcher ini (not
  `play_ini`, which binds a controller), `launcher._hide` disabled for this instance, and
  `show(pid)` called **once** after the VM is up (no later focus stealing). The only pad the
  game can read is the DebugServer's `pad_set`. Cards: `use_card` with the take's own card.
- **Playing**: segments are Python functions in `tools/guided_run/segments.py`, one per take,
  built from route_capture's helpers (`walk_path`, `face`, `goto`, `use_press`, `stick_toward`,
  the l18 helpers) over a thin client that steps the live instance one vsync at a time
  (`run until vsyncs 1`), reads the row spans, and sets the pad (closed loop: the pad reaches the
  game's pad block two frames later, pcsx2_session.py). The loop is paced to 59.94 Hz of wall
  time so the user sees normal speed; if the round trip cannot keep up, the run is slower than
  real time but still exact (section 9 step 2 measures it). Waypoints come from the route docs'
  measured positions and the beats' traces in `build/fork_refs/**` (allowed sources), re-steered
  live: the run's state differs from the beat chain's (rand, health, freed nodes), so beat pads
  are never replayed blind.
- **Status line** per segment in the terminal: take name, stretch (e.g. `S10 the main
  compound`), try n of N, tick, health / infection / charge, next target, and the tags held.
- **End of a take**: the segment's success predicate (area bytes, a flag, an item, a position,
  control back) -> halt -> `machine`, `disarm`, exact `state_save end.p2s`, card copied, then
  `write_rec`, then the hidden exactness replay (`replay`, TRIES 3, `replay_config`: renderer
  13, unlimited). A take whose replay is not exact is not kept (it is re-recorded; section 8.4).

### 8.3 Folders and names

- `build/guided_play/` in the decomp (ignored; disc-derived; add it to CLAUDE.md's never-delete
  recording list beside `build/human_play/**`; the user owns that file, so the orchestrator asks).
- Kept takes: `build/guided_play/g01_title_newgame`, `g02_area11`, ... (`g` + two digits +
  stretch slug); `start.json` `after` names the previous kept take; the first take's `after` is
  null and its state is the title state.
- Failed tries: `build/guided_play/_tries/<take>/try01`, `try02`, ... each a complete take
  folder (with `take.rec`; `after` set to the kept take it started from), never deleted.
- `build/guided_play/chain.json`: the kept order, each take's checkpoint (section 7), tries, the
  plan revision and the sources each segment used.
- **The port**: `tools/watch_takes.py` reads only `../Extermination/build/human_play`; a port
  lane adds `--root DIR` (default unchanged) so `python3 tools/watch_takes.py --root
  ../Extermination/build/guided_play g01_title_newgame` stitches the guided chain. `chain()`
  already follows `after` links inside one root, so the two chains never mix.

### 8.4 Retries without breaking continuity

A segment's try starts from the last kept take's `end.p2s` and `card_end.ps2`. If it fails
(death: D_008106B9 / health 0; a fall out of bounds; a timeout; the success predicate not met)
the try is closed and saved to `_tries/`, and the next try starts from the same end state with a
changed input (another waypoint set, a different wait before a jump, another weapon or tactic);
the same input from the same state reproduces the same failure, so the driver records what it
varied. Only the try that succeeds and replays exactly becomes `gNN` and joins the chain. A
blocked stretch (B1..B4) is scouted hidden from the chain's last end state (section 9 step 3),
which is exploration, not part of the chain. Retries are played visibly like the kept tries.

### 8.5 Time cost

The game is about 5 to 6 hours of play for a practised player; a scripted run with the detours
of section 4 is likely 6 to 8 hours of game time, recorded in real time, plus retries (estimate
+30 to 50%), plus the hidden checks at about 3.5x (+30%): on the order of 10 to 15 hours of
exclusive emulator time over several sessions, before the scouting of the unplayed stretches.
Takes of 10 to 30 minutes keep a failure cheap.

### 8.6 Nothing from the user's recordings (how it is guaranteed)

- The driver's only root is `build/guided_play`; one path helper resolves every file it reads
  or writes and refuses anything under `build/human_play` or the port's `build/takes`.
- `--after` resolves inside `build/guided_play` only; the driver has no `--beat` from human
  takes. The first take starts from the fork's title state, the same start every take uses (it
  is not from a user take).
- Segment waypoints carry a `src` field: a route beat name, a route doc section, a code read,
  or `new` (scouted by this run). The driver refuses a segment without one.
- A check in the driver's self-test (and a grep before each commit of the driver or plan files)
  fails if `human_play` appears anywhere except in the refusal helper.
- The guided chain uses its own cards (`ps2_card.write` for the first), never a human take's.

## 9. What the recording lane does first

1. **Probe (hidden, 2 minutes of play):** from the title state, inputs OFF in the config, drive
   `pad_set` for a few hundred frames with `arm`'s `input_log` and tables on; confirm that
   `input.log` records the scripted pad, that `timeline(records)` replayed by `replay` is exact,
   and that `write_rec` gives a `take.rec` the port's `watch_takes --check` accepts. If the
   fork's input log does not record `pad_set`, fix that before anything else.
2. **Pacing (visible, 1 minute):** the closed-loop step rate with sound on; decide 59.94 Hz
   pacing or the slower-than-real-time fallback, and tell the user.
3. **Scout the blockers hidden**, in route order, from our own states (allowed: `fork_refs`
   beats; constructed cards only for information, never in the chain): B1 from a07b_07 / a04e_00
   (the lift [36] / console [37], then [68] / [69], then the box stack); B2 (does flag 0x2C = 1
   from AREA15's 0x824E50 lay the bridge flat; what gates 0x824E50); B3 (AREA00 sub 2's [30]);
   B4 (AREA14's bench). Record each finding in the port's route docs as usual.
4. Then record the chain from `g01`, Part A first (all played), each tag on the way, and stop
   at the first stretch whose scouting is not done.
5. After the chain reaches CP35, record S36 as section 5 says, and update this document's
   status lines with the kept takes and their checkpoint ticks.
