# Extermination — Curiosities, hidden systems and engine quirks

Cut, hidden, unused or unreached content and notable engine quirks found
during the decompilation and the port work. This file is the source for the
port's Enhanced cut-content switches (port `docs/PORT_PROFILES.md` rule 5 and
`docs/LAUNCHER_OPTIONS.md`).

**Statuses**

- **decoded** — the behaviour is proven from the original code (byte-matched C,
  an original-instruction oracle, or a static decode that names every step) or
  from an original capture.
- **partial** — some of it is proven, the rest is open; the entry says which.
- **unverified** — an inference, a label or a single observation; nothing
  proves it yet.

**Rules**

- Only **decoded** entries may become Enhanced cut-content switches, and only
  when their kind is restorable content (below). Engine quirks are Original
  behaviour that the port reproduces; a fix for one belongs in
  `LAUNCHER_OPTIONS.md` as a bug-fix option, not here.
- **Kinds.** Restorable content: *hidden system*, *hidden or unreached UI*,
  *unreached content*, and *cut content* that left something on the disc.
  Trivia, never a cut-content switch: *shipped feature* and *optional
  content* (both already in normal play), *design detail*, *assets*, *player
  clips once thought unused*, *dead code*, *engine quirk* (Original
  behaviour), and cut content with nothing left to restore.
- Every entry points at its evidence (FINDINGS section titles, port docs,
  `CAPTURES_C7.md`). A label is not evidence: an entry built on a name or a
  description, not on the code or a capture, is unverified.
- Entry numbers are stable IDs (FINDINGS and PROGRESS cite them). Withdrawn
  entries keep their number in the last section. New entries take the next
  number.
- No disassembly, no game text (describe, don't quote) and no disc data.

Last reviewed: 2026-09-27 (every entry re-checked; entries 17-25 added;
review corrections applied to 1, 3, 4, 6, 8-12, 14, 15, 17, 18, 21 and
23-25).

---

## 1. Light and enemy perception — partial

*Kind: hidden system.* The old "light-based stealth system" flagship was
largely a misreading; what is left is smaller but real.

- **Corrected (the old reading is withdrawn).** `func_001418F0` is byte-matched
  C. It takes an actor and that actor's AI block. It reads bit 0 of byte +0xA
  of the actor it is given, and its byte-matched caller 0013DD40 passes the
  actor itself (the other callers, 0013D850 and 0013D980, are assembly and
  NEARMISS C). So +0xA bit 0 is a per-actor flag of the actor passed, not the
  player's light. Which code writes it for the actors 0013DD40 serves is not
  identified. (The crate brain 001551B0 writes +0xA only on boxes, models 6,
  0x1C, 0x1E, 0x1F and 0x50 with +0x52 set; port `CRATES_DRUMS_ORIGINAL.md`.)
  The timer (+0x70) and counter (+0x78) live in the AI block:
  - With the timer at zero and the bit set, the timer is armed (240) at once.
    Without the bit it computes a bearing from floats +0xB0 / +0xB8 of the
    record at `*(D_00275B40+0x3C)` and passes it, with 18.0, to 001B3F10.
    Each frame 001B3F10 returns non-zero the counter is bumped, otherwise it
    is reset, and at a difficulty-indexed count (D_002753C8) the timer is
    armed. The record is unidentified: elsewhere D_00275B40 is the current
    actor's bone-pointer array (FINDINGS "Per-bone animation evaluator";
    port `LOCOMOTION_DISPLAY.md`: the player's +0x110 during the player
    callback), and which actor's array it holds when 001418F0 runs is not
    established. The "camera/view struct" in the `func_001418F0.c` header is
    a label. What 001B3F10 tests is also read from C headers only
    (unverified): 001418F0's header calls it a turn-done test, 001B3F10's own
    NEARMISS header a bearing, range and line-of-sight test against
    D_00810360. The "12.5 degree view cone" was a misread argument: the call
    passes 18.0.
  - An armed timer clears after the AI block's +0x5C float has stayed above
    70.0 for 180 consecutive frames; at or below 70.0 the counter resets.
- **The L3 byte.** L3 (the unlabelled 7th button-config row, entry 12) flips
  player byte +0xA, and in a live session it returned to 0 by itself after
  about 5-6 s (FINDINGS "BATTERY LOCATED", live). No reader of the player's
  +0xA is known (the s51 sweep found none through the player base), and the
  writer that clears it is not identified. The "300-frame burst with an
  off-gesture" once attributed to it is the idle look-around timer of
  00161020 (FINDINGS "PLAYER IDLE CYCLE"; port `em_locomotion_display`,
  translated from the original instructions, touches no light byte).
  What the player sees when pressing L3 is not recorded.
- **The gun light draws** (Square while aiming: D_00810D3C, draw enable
  D_008106C7). While the player is in the aim states, 00187780 (NEARMISS C)
  issues the 00187690 light draw request, and it adds the light-cone shells
  through 001D9530 only when bit 0x20000000 of the area flags word
  (001B0070, which returns D_008106C8) is clear (`src/func_00187780.c`, the
  001D9530 call). AREA11's D_008106C8 is 0x20081910 in every capture (port
  `ORIGINAL_FRAME_ORDER.md` P31), so in the first level the cone shells are
  skipped and only the draw request runs; AREA01 (0x8D00 / 0x8D01) draws
  them. FINDINGS "FLASHLIGHT RENDER DECODE" (s87 correction) and port
  `FIRST_LEVEL_AUDIT.md` R03/R04 name the gate but not that AREA11 sets
  the bit. The gun light costs no battery (FINDINGS "WEAPON-VISUAL
  FIDELITY", user-attested).
- **One behaviour reacts to the gun light (decoded).** `func_00138900`
  (byte-matched C), state 2: the counter is bumped while the gun light is on
  and the player (D_00810360) is within 150 units, and reset otherwise. The
  advance test is counter > 0, so the actor advances on the first such
  frame (or when its own +0xA bit 0 is set), plays clip 3 and sound cue 0x816.
  Which creature runs it is not identified; it is in AREA01's sub-1 owner
  lane (port `AREA01_OVERVIEW.md`, lane S1) and did not run on either
  recorded route.
- **Unverified.** `func_00185A10` (NEARMISS C, a target-acquisition scan)
  switches its scan cone between two angles depending on D_008106C7 under a
  difficulty condition, per its C header; whose scan it is has not been
  established.
- **Decoded, unchanged:** lighting comes from the per-room light-rig table
  D_00251C50 (45 entries) plus an always-on camera fill light that player init
  sets on the player (FINDINGS "FLASHLIGHT RENDER DECODE" section 2, "PER-ROOM
  LIGHT RIGS FULLY DECODED").
- The dark-area animation rows once folded into this entry (001B0070() & 4)
  are entry 17.
- Open: the creature behind 00138900; who writes +0xA for the actors
  0013DD40 serves; who clears the player's +0xA; whether anything reads it.

## 3. Four player clips with the variant container header — partial

*Kind: player clips once thought unused.* Player clips 54, 94, 115 and 375
use a container header whose two metadata halfwords are populated (a
follow-on link and a category), not the common sentinel. They were once
"unknown payloads"; they are ordinary chained clips and all bake (FINDINGS
"NON-SENTINEL ANIM CONTAINER"; port `PLAYER_CLIPS.md` section 2: 0x36 → 0x35,
0x5E → 0x5F, 0x73 → 0x72, 0x177 → 0x178). Identified by the user watching
them rendered, then traced (FINDINGS "WOODEN CRATE behaviour corrections +
the unused player clips"). Two of them are on the first-level route:

- **94 (0x5E) — on the route.** The user identified it as a crouch; in play
  it is the slide entry, requested in route beat 06, and chains to 0x5F
  (port `PLAYER_CLIPS.md`; FINDINGS names 0016C6A0 as a static requester).
- **115 (0x73) — on the route.** The user identified it as a landing; it is
  requested on the falls of route beats 10, 11 and 12 and chains to 0x72
  (port `PLAYER_CLIPS.md`; FINDINGS names 0016C6A0 and 00162DB0 as static
  requesters).
- **54 (0x36) — a shake-off struggle, not seen on the route.** Static
  decode: the reaction handler 002208C0 plays clips 0x35 → 0x36 → 0x35 with
  a rumble and sound 0x154. The live bug latch uses different clips (0x2C /
  0x2E / 0x24, FINDINGS "BUG LATCH / SHAKE-OFF — LIVE-VERIFIED"), so the
  attacker that triggers the 0x35/0x36 path is not identified.
- **375 (0x177) — unverified, not seen on the route.** Chains to 0x178. No
  static requester; probably a scripted or cutscene request (needs a write
  watch on the player's clip id).

## 4. Passcode keypads with codes in memory — partial

*Kind: hidden or unreached UI.* Status-menu pages 4 and 5 are 3x4 numeric
keypads that the page diamonds cannot reach. They open only by an external
request (D_008106C5), compare the entry with a string from D_00275858[page-4]
and, on page 4, set unlock bit 0x20 of D_00810845 (FINDINGS "STATUS
SUB-PAGES", static decode). The code strings at D_00275858 are not dumped.
Open: who sets D_008106C5, the doors behind the unlock bit, and the keypad
textures (data-table driven, not exported).

## 5. Dead code shipped on the disc — decoded

*Kind: dead code.* A brute-force scan of the boot ELF and all 19 overlays (call
words and address-building pairs) finds no reference to the VIF1 helper
library at 0x0011BA00..0x0011BCF8, nor to the 0011A9F0..0011AB70 microprogram
dispatcher cluster. That cluster holds the boot ELF's only EE-side VU1
microprogram start, so VU1 programs are started only by DMA-chain tags
(FINDINGS "Bind-pose matrix EE-side trace", 2026-05-25). None of them ran on
either recorded route (route census). Linked-in library code, not cut
gameplay. The AREA13 "broken helper" once listed here is withdrawn (see
Withdrawn).

## 6. The battery is a currency, not a timer — decoded

*Kind: engine quirk.* The battery never drains on its own. Powering a device
spends twice the device's cost in half-units and stations recharge in steps
(live-verified, FINDINGS "BATTERY LOCATED"). Three battery-pack pickups,
001C40B0 cases 0x1B, 0x1C and 0x1D, add 12, 36 and 48 half-units per pack,
raise the capacity to at least that pack's value and cap the charge at the
capacity, so a smaller pack refills charge without lowering the capacity
(port `AREA11_PANEL.md`, "Inventory and validation", from the original
instructions). They match the catalog's 6, 18 and 24 gauge battery packs
(FINDINGS "MESSAGE BANK EXPORTED", group 3). The older FINDINGS "BATTERY
LOCATED" lists only the first two classes. The display shows half the
stored value; the AREA11 battery page's half-unit charge is re-proven by the
port's oracle (port `AREA11_PANEL.md`).

## 7. The magazine-equivalent 30:1 fold — decoded

*Kind: engine quirk.* Ammo bookkeeping keeps a hidden magazine-equivalent
counter (D_00810C63, cap 98) whose overflow folds into the round reserve at
30:1. The reserve is the total pool including the loaded magazine: reloading
sets the magazine to min(30, reserve) and subtracts nothing, and each shot
decrements both (FINDINGS "INVENTORY LOCATED", "WEAPON SYSTEM").

## 8. Per-area crate models — partial

*Kind: assets.* The breakable box (owner 001551B0, which never reads the
player: port `CRATES_DRUMS_ORIGINAL.md`) takes its model from the per-area
model table (placement param 0x0D → table entry 0x0D), so the same object
has a different model per area: a 14-unit bevelled crate in AREA11, a small
cardboard box in the office table (FINDINGS "OFFICE CRATE BLOB"; the old
"crawler disguise" wording there is a label). The debris husk also differs
by placement byte: the wooden husk for byte 6 (every crate in
AREA01/02/11/13/18/20/22) and a grey-cyan family in AREA03/06/07/08
(FINDINGS "GIB SET", closing note). FINDINGS "WOODEN CRATE behaviour
corrections" item 1 calls its 14-unit hit box the "office/AREA02" crate, but
its live reads were of AREA01 crate actors (0x7AB440…0x7AC2F0), so that is
most likely a mislabel rather than a conflict with the office's cardboard
model. Open: no AREA02 crate has been measured.

## 9. The 15 named RECON dogtags — partial

*Kind: shipped feature (collectibles).* The item catalog (message bank
group 3, lines 45-59) names 15 dogtags after members of the RECON squad
(FINDINGS "MESSAGE BANK EXPORTED"). At least one is placed in the shipped
game: the office sub-0 item group holds dogtag 13 on the upper floor
(FINDINGS "ITEM PICKUP SYSTEM FULLY DECODED"). Open: the other placements
and any reward for collecting them. Until a dogtag is shown to be unplaced,
none of them is cut content.

## 10. The infection diary — decoded

*Kind: shipped feature (text shown in normal play).* When no page is
hovered, the status hub's help panel shows one of five first-person diary
lines graded by the displayed infection (1-19, 20-49, 50-69, 70-89, 90-99),
and a final line announcing the protagonist is infected at 100 (FINDINGS
"MESSAGE BANK EXPORTED", hub
help-line selection rule). Infection 100 also sets the infected latch that
caps health at 60, starts the infected drain and swaps the music (entry 14).

## 11. The alternate melee row: melee while ducked under a low ceiling — decoded

*Kind: shipped feature (situational moves).* Every melee table has a second
row (knife clips 0x1BD..0x1C1, their own impact and release timings)
selected by player +0x236 (FINDINGS "KNIFE/MELEE DECODED" section 5, which
still describes the gate as open). +0x236 is the low-clearance latch:

- 001764E0's overhead column probe raises it when an overhang is at least
  13.8 above the feet and there is crawl space ahead, so the player ducks
  under it (port `PLAYER_FLOOR.md` P16, `FIRST_CONTROL.md`; the port's
  translation is oracle-tested against the original).
- 001756E0 keeps it after the floor snap only while cover within 13.99
  remains overhead, and releases it otherwise; area 0x12 forces it on link
  type 6 (port `PLAYER_FLOOR.md`).
- The recovery path of 00162A40 and the hang's climb-up end set it through
  001760C0 (port `PLAYER_RECOVERY.md`, `PLAYER_HANG.md`); entering the fall
  state (00179680) clears it.

So row 1 is melee while ducked under a low ceiling. (The caller's inherited
`$s1 & 4`, entry 21, gates a different clause of 001764E0, not this row.)
The port's `em_player_weapon_states_b.c`
indexes both rows. The FINDINGS player field table's "armed-stance flag"
name for +0x236 is a label.

## 12. The unlabelled 7th button-config row — partial

*Kind: shipped feature (an unlabelled control).* The BUTTON CONFIG screen
has a 7th row with no label. In TYPE A it binds L3, which flips player
byte +0xA (live, FINDINGS "BATTERY LOCATED"); L3 also doubles as reload
while the weapon is drawn. What +0xA does is open (entry 1).

## 13. Multi-actor animation track sets — partial

*Kind: assets.* `chunk21/f17_id8f` opens with a 21-node, a 3-node and three
1-node track containers that all share a 401-frame length, beside two mesh
segments and a 41-clip bank (FINDINGS "chunk21/f17_id8f is an ENCOUNTER
PACKAGE"). The structure is decoded from the data. That the synchronized
tracks are a cutscene is an inference; no code that plays them is
identified.

## 14. Music cue table: the credits cue and the infected-state music — partial

*Kind: design detail.* The cue table is decoded from code (FINDINGS "Music
cue table — SOLVED"). Cue 24 is a hardcoded override: 001FAE70 plays it
instead of the area music while D_008104E4 == 1 (except in area 0x15 and for
cues 0xB, 0xC and 0x17). D_008104E4 is player +0x234, the infected latch: it
becomes 1 when infection reaches 100, and the same byte caps health at 60
and arms the infected drain (FINDINGS "PLAYER DAMAGE & DEATH PIPELINE", the
field table and the infection apply). So cue 24 is the music that plays once
the player is infected (entry 10). The "alert-mode override" name in
FINDINGS is a label the code contradicts. Cue 1 as the end credits rests on
an audio match with the official soundtrack, not on a traced caller
(unverified).

## 15. Crates hatch bugs, and some hide items — decoded

*Kind: design detail.* Nest-linked crates spawn the records of their area's
nest group (the deferred-spawn registry D_0024D820), verified against RAM
(FINDINGS "CREATURE IDENTITY CORRECTION"):

- The children are a 15-node bug (global creature slots 0x0F/0x10, a 36-clip
  bank, HP 15 or 30, 30 or 50 on the harder setting), never the chain worm.
- AREA01's nest-linked crate hatches four bugs. AREA03's hides a vaccine
  pickup guarded by two bugs. AREA06's holds only an item. AREA22's hatches
  three bugs (00128C10). The five office crates hatch 2-3 each. Most crates
  link to no nest and only break (FINDINGS "CREATURE IDENTITY CORRECTION"
  section 2, the 19-area survey table).
- AREA11 places no generators and its crates carry no nest links.
- Event flag 0x30 switches every later hatchling from the grey-blue skin
  (slot 0x0F) to a red infected skin (slot 0x10). The story moment that
  posts it is open.
- Correction: no player-light gate was found in the two bug brains
  (structural decode, FINDINGS "BUG BRAIN STATE MACHINES DECODED" section
  6). Both appear in the s51 list of +0xA readers, and FINDINGS leaves light
  detection "not resolved here".

## 16. The chain creature never touches dry land — decoded

*Kind: design detail.* The 24-node chain creature is installed only by
mode-2 generator pads (organic growths); no nest record anywhere installs it.
Its loop clip is an anchored, reared sway (the base never moves). Its emerge
clip starts about 40 units below the floor, a missed lunge despawns it, and
no weapon can hurt it (FINDINGS "CREATURE IDENTITY CORRECTION" section 6;
"LIVE VERIFICATION SWEEP" section 1). The water-borne reading matches the
story's virus (the user's knowledge of the game).

## 17. Area flag bit 2: alternate locomotion rows and a slow health drain — partial

*Kind: hidden system.* Bit 2 of the per-area flags word D_008106C8 (from
the area table) switches the player's clip selection 0017B490 to an
alternate row set (port `LOCOMOTION_DISPLAY.md`). Together with either bit
of 0x60 (and D_00810C7E == 0) it arms a passive drain in 0015D100 that takes 1.0
health every 360 ticks (port `PLAYER_STAGE_WORKERS.md`; both translated from
the original instructions). Neither bit is set in AREA11 (D_008106C8 =
0x20081910) or AREA01 (0x8D00/0x8D01). Open: which areas set them. The old
"dark area" and "hazard room" names are labels.

## 18. Breaking a box wakes only the boxes not resting on world ground — decoded

*Kind: engine quirk.* The breakable box 001551B0 (a box, not a crawler; it
never reads the player) sets +0x52 at INIT from a floor probe: 0 when the
probe result is 4 (world ground), 1 for any other result. When a box is
damaged, its state-4 broadcast walks the whole live list and sets +0x0A = 1
on every box-model node (6, 0x1C, 0x1E, 0x1F, 0x50) whose +0x52 is non-zero;
there is no radius check. So breaking any box wakes exactly the boxes not
resting on world ground (in AREA11, the one box stacked on another), and
boxes on world ground are never woken. In AREA11, box 0x7A7980 sits 14
units above 0x7A7F60 and is the only one with +0x52 = 1; damage on any
AREA11 box wakes it (port `CRATES_DRUMS_ORIGINAL.md`, three AREA11 captures
and an oracle run over the captured list). The s76 result "breaking one
wakes none" (FINDINGS "WOODEN CRATE behaviour corrections", item 2, which
claims +0x52 is 0 on every placed crate) held only for AREA01's unstacked
crates.

## 19. Sound requests that play nothing — decoded

*Kind: engine quirk.*

- AREA11's power-panel callback 001580C0 requests sound 0x3EE, but the area's
  remap byte for it is FF, so the request returns -1 and nothing plays (port
  `AREA11_PANEL_SFX.md`, `SFX_REGISTRY_FIRST_LEVEL.md`).
- In AREA02 sub 1, 0x7D8 and 0x3F2 are remapped to FF as well (port
  `SFX_PITCH.md`).
- The flame sound 0x413 was requested 812 times on the first-level route;
  808 requests were refused because the gains were out of range, so no track
  started (port `SFX_REGISTRY_FIRST_LEVEL.md`, capture probe).

## 20. The intro movie's data left in the effect lane-3 parameters — decoded

*Kind: engine quirk.* The effect lane-3 ring's +0x40 quadwords double as the
movie player's data buffer (block_copy under 002036E0). Their contents at the
AREA11 opening depend on whether and when the player skipped the New Game
movie. Played to the end, 4 slots are non-zero (the same as at the title).
Skipped about 1.9 s in (as in the route captures' save states), 20 slots are.
The effect barrel 001F0720 copies them into its lane-3 packets every frame,
but no lane-3 slot is active and nothing from lane 3 is drawn in the first
level, so the leftovers are invisible (`CAPTURES_C7.md`, conclusions and
sections 2 and 2b).

## 21. Code that uses registers its caller left — decoded (one case reached)

*Kind: engine quirk (latent).* Several original functions read a saved
register they never set on some path, so the result depends on the caller.
The first case below is reached in normal play; for the others, whether the
game reaches the path is not established.
C cannot express this, so the decomp's C and the port document these paths or
refuse them.

- **001764E0** (the player's radial wall probes; the function runs on every
  idle and walk tick): its ankle pass, which runs only on walk-callback ticks
  (+4 == 1 and +5 == 1), applies a class-0x2000 slope hit only when the
  caller's `$s1 & 4` is set, and it never sets `$s1`. On ordinary ticks the
  main register file holds 1 there (the save-state register file), so the
  slope clause is off; only 001612D0's reversal-resume path leaves the
  0017B490 clip id in `$s1` and can turn it on (port `PLAYER_FLOOR.md` P16,
  `LOCOMOTION_DISPLAY.md`). This case is reached and decoded.
- **0019D770** (camera grid walker): when no span is shorter than the limit
  word, it walks with the three saved registers left by its only caller,
  0019A910. Whether the game ever reaches this path is not established.
  0019CF50 has the same shape (FINDINGS "NEARMISS body corrections from the
  AREA01 lanes").
- **0022BBC0** (the player's burn-particle driver on the AREA01 route; its old
  "staff-roll director" label is unproven): the trail period and the burst
  kind live in saved registers. An actor whose burst kind is 6 or more reuses
  the previous actor's burst. For the first such actor, and for the period
  divisor when seq[0xD] >= 10, the values are whatever the caller left
  (FINDINGS "NEARMISS body corrections from the AREA01 wave-2 lanes"; port
  `AREA01_UI.md`).
- **001551B0** (breakable box): INIT with +0x0E bit 0 set (the box still
  owes its nest a child) and no nest link reads a saved register that
  001AFD70's walk left holding the next node; the port module keeps that
  (port `CRATES_DRUMS_ORIGINAL.md`).
- Port `AREA01_SYS.md` lists more reads of this kind in stubbed callees
  (0015A750, 0017B490, 0018CBD0, 0019C830, 0019CB60, 0019D330, 0019E280,
  001CFBE0), each on a path the port must keep unreachable.

## 22. AREA01's console character and the optional first conversation — decoded

*Kind: optional content.* Placement [36] in AREA01 is a character at the
control-room console, not a crank (the old FINDINGS s69/s74 "crank" and
"bridge-lowering cutscene" labels are withdrawn). The AREA01 route capture
shows the following (FINDINGS "AREA01 route capture", port
`SECOND_LEVEL_ROUTE.md` section 2):

- With the story byte D_008107D9 = 0 the character's conversation is
  optional and changes no sampled progress byte.
- Trying the locked shaft door sets the byte to 0x80. The second
  conversation then sets it to 0x81, which opens the shaft door. This is the
  main path.
- A third branch (byte 0x81, script 0x82A660) exists and was not played:
  unverified.

## 23. Content on the disc not reached on the recorded routes — partial

*Kind: unreached content.* "Not reached" means absent from the recorded
first-level and AREA01 route censuses (decomp `tools/route_census.py`); it is
not a proof of unreachability.

- **The fan's second exit (code-proven, not captured).** AREA11 fan record
  [2] exits directly to AREA01 sub 1, entry 4 only after Roger's departure
  has run once (D_00810758[0] == 0xFF). The first pass leaves through the
  departure to AREA01 sub 0 instead. So one known way into AREA01 sub 1 is
  returning to AREA11 and taking the fan's second exit; other ways in (doors
  from other areas) have not been excluded. The 108 functions of lane S1 are
  statically reached only from sub-1 owners, but some of them ran on the
  route through other callers: 17C370, 187EC0 and 21BC40 are tagged route in
  port `AREA01_OVERVIEW.md`, whose lane-S1 note "only reachable through the
  fan's direct exit" overstates this (port `FIRST_LEVEL_EXIT.md` section 4).
- **AREA01, unvisited:** the north room beyond the two 0x8261A0 owners
  (called bridges in FINDINGS; not seen moving), the upper floor (doors to
  area 0x16 and AREA06), a second character record [38] (overlay 0x825740,
  not spawned in the recorded load), and several deferred pickups (port
  `SECOND_LEVEL_ROUTE.md` section 7).
- **Functions that never ran** on either route: the gun-light-reactive
  00138900 and the alert-timer family 001418F0 / 0013D850 / 0013D980 /
  0013DD40 (entry 1), the gun-light lamp 00187780 (D_008106C7 stays 0 on the
  route; port `FIRST_LEVEL_CENSUS.md`), and the beam 001E3630. The beam has no
  static caller and may be installed through a runtime pointer (FINDINGS "BUG
  LATCH / SHAKE-OFF", unverified).

## 24. Four missing level numbers — partial

*Kind: cut content, trivia (nothing on the disc to restore).* The disc has
no AREA05, AREA09, AREA10 or AREA12 overlay. The shipped overlays carry
dense ids 1-19 in filename order (AREA04 is id 5 because AREA00 is id 1;
AREA11 is id 9), and the boot ELF's overlay filename table lists exactly the
19 shipped names (decomp `docs/OVERLAYS.md` section 1 and "Filename table";
FINDINGS "`OVERLAY/AREA*.BIN` — MWo3 runtime code overlays"). The
byte-matched area-transition dispatcher `func_001E7780` (linked from the
compiled C) has arms for areas 0-4, 6-8, 11 and 13-22 and none for 5, 9, 10
or 12, so the code has no slot for them. The inference (not proven) is that
the numbers were dropped before the overlay ids and the filename table were
built. OVERLAYS.md section 8 item 5 gives the wrong cause for AREA04's id:
it says AREA04 is id 5 because AREA05 is absent and the id counter
continues, but the id is 5 because AREA00 takes id 1. Open: whether any
data table (area flags, names, maps) still reserves those numbers, and what
the levels were.

## 25. The random generator is not seeded for New Game — partial

*Kind: engine quirk.* rand (0x00122BB8, byte-matched ee-gcc C) is a linear
congruential generator whose state starts at 1. The C7 trace of New Game
found the state still 1 at the NEW GAME commit and no srand call in any
captured stretch (`CAPTURES_C7.md` section 3, "What was recorded" and the
findings). So a New Game from a fresh boot, with no attract demo shown,
starts from state 1 (decoded for that case, from the capture). This does
not make every New Game identical:

- The only direct call to srand (0x00122BA8) in the boot ELF is at
  0x001ACCE0 in `anim_frame_top_a` (NEARMISS C, 0x001ACA20), state 4 sub 0,
  which seeds 0x45 and then enters play; no overlay calls it directly (a
  word scan of the local overlays, 2026-09-27). That function is the
  attract-demo task (decomp `docs/STARTUP.md`, title timeout). New Game does
  not reseed, so a New Game after a title timeout has shown the demo, or
  after earlier play, starts from wherever those left the state (static
  only; a call through a pointer is not excluded).
- Even from state 1 the callers' draw order is not fixed: two New Game runs
  agree call for call only up to AE+31, then the order of callers diverges
  with the run's timing (`CAPTURES_C7.md` section 3, "Run to run"), so which
  draws each consumer gets varies from run to run.

Port `FIDELITY_FEATURES.md` (random numbers) agrees: it records that the
start state was measured with no attract demo run and that the opening's
call order differs between runs.

---

## Notes for the port's candidate lists

The candidate list in port `LAUNCHER_OPTIONS.md` and the examples in port
`PORT_PROFILES.md` (cut and hidden content) predate this review and need
correcting there:

- "The light-based stealth system" is withdrawn (entry 1); what remains is
  partial.
- "Hidden animation directory entries" are ordinary clips, two of them on
  the first-level route (entry 3).
- The 15 dogtags (9), the infection diary (10) and the unlabelled 7th config
  row (12) are shipped features, not cut content, so none of them can be a
  cut-content switch.

---

## Withdrawn entries and claims

Kept so the old claims are not reintroduced.

- **2. The unreachable sprint.** Anim id 3 is the ordinary full-stick run: the
  tier ramp 0017BC40 promotes the locomotion tier until it matches the target
  speed (walk id 1, jog id 2, run id 3). FINDINGS "LOCOMOTION TIER RAMP" (s56).
- **5 (part). The AREA13 "drifted, broken helper".** The director's call to
  0x823FE0 was judged to enter mid-function. That reading used link addresses:
  overlays are linked at 0x00823500 but run 0x40 higher (PROGRESS 2026-09-25).
  At runtime 0x823FE0 the shipped AREA13.BIN begins a function with its stack
  setup (checked against the local AREA13.BIN on 2026-09-27), so the call is
  an ordinary area-13 camera hook. FINDINGS "MODE-0 CAMERA DIRECTOR" section
  3 still carries the old reading.
- **1 (part). The light-based stealth loop:** "light on means instant maximum
  awareness", the 12.5 degree cone, the 300-frame shoulder-light burst and the
  clip-0x15D off-gesture. Corrected in entry 1.
- **3 (part). "Hidden, undecodable animation payloads."** The clips are
  ordinary (entry 3).
- **15 (part). "The bug brains read the player's light flag."** No light gate
  was found (entry 15).
- **18 (part). "Breaking one crate wakes none."** True only for AREA01's
  unstacked crates; a box not resting on world ground is woken (entry 18).
- **11 (part). "The port plays row 0 only"** and the ledge / ladder / hang
  guesses for the alternate melee row. Row 1 is the low-clearance row
  (entry 11).
- **14 (part). "Alert-mode music override."** Cue 24 follows the infected
  latch (entry 14).
- **AREA01 "drawbridge crank" and "bridge-lowering cutscene"** (FINDINGS
  s69/s74 labels, never entries here). Placement [36] is a character
  (entry 22).
