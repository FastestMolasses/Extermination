# World graph: every area change, lock and story gate (code and data)

Lane NEXT (graph), 2026-09-28 (session s88). Target: SCUS-97112, boot ELF
SHA-256 `ee052236783e7d3e865754d3ff9fee71290addeb7d146c86caa7ff2724d1e17a`.

This document lists, for all 19 area overlays, every way the game changes
area (doors, lifts, triggers, falls), the lock or story condition on each,
every writer of those locks and story bytes, and where the items come from;
then it derives the story's progression order. It is built from the boot ELF,
the overlays and the level registries only (no emulator); section 8 lists
what the eighth-level captures (port `docs/EIGHTH_LEVEL_ROUTE.md`) later
measured, section 8b the ninth-level captures (port
`docs/NINTH_LEVEL_ROUTE.md`), section 8c the tenth-level captures (port
`docs/TENTH_LEVEL_ROUTE.md`), section 8d the eleventh-level captures
(port `docs/ELEVENTH_LEVEL_ROUTE.md`) and section 8e the twelfth-level
captures (port `docs/TWELFTH_LEVEL_ROUTE.md`); section 7's table marks each edge
played or not. Nothing here quotes disc text; the tables are addresses and
numbers.

**Evidence marks.** Each rule names its source: **BM** (byte-matched C),
**NM** (NEARMISS C: body-correct, not byte-identical), **C** (overlay C that
`overlay_match.py` reports byte-identical), **ins** (read from the original
instructions by this lane; described, not reproduced), **scan** (the
mechanical scan of section 1, a lead until read), **measured** (a route
capture). A claim marked **inferred** is a conclusion drawn from several of
these that no single function states; the progression order (section 7) is
inferred edge by edge.

## 1. Tool

`tools/route_census.py graph` (additive to the census tool; no emulator):

```sh
# decomp repo root, macOS arm64
.venv/bin/python tools/route_census.py graph
# -> build/s87/census/world_graph.json and world_graph_tables.md (ignored)
```

It reads `config/SCUS_971.12`, every `extract/OVERLAY/AREAnn.BIN` (loaded
flat at 0x823500; runtime = splat label + 0x40) and the local splat trees
(function starts only), and:

- decodes each area's registries with `tools/area_overview.py`'s readers:
  placements D_0024D7C0[area][sub] (0x28-byte records), deferred groups
  D_0024D820, spawn tables D_0024D650, door destinations D_0024E140[area]
  (4 bytes per door id);
- scans every function of the boot text and of each overlay linearly: a
  load or store whose address comes from a `lui` / `addiu` (or `ori`) pair
  in the story RAM 0x810600..0x810E00 is recorded (an `addu` of such an
  address marks it "indexed"), and so is every `jal` with the argument
  registers a0..a3 whose values are known from straight-line code (delay
  slot included). A value set on another path is reported as `?`;
- walks the 0x40-byte script records (word 0 bit 31 stop, bit 30 jump to
  word 1, op = word 0 & 0xFFF) from every 001BA1A0 second argument and from
  every data word pointing into the same data section;
- names an overlay function's owner: the placement or deferred record whose
  behaviour it is, reached through direct calls, or through a script that
  such a function starts and whose op09 record calls it back.

Overlay function starts are the jal targets, the behaviours and op09
callbacks, and the splat pieces that follow a function return (a splat piece
otherwise can split one function in two). The scan is a lead generator:
every rule below was checked by reading the function (the mark says how),
and every `?` in the tables was either read by hand in the text or left
open.

## 2. State bytes and how an area is chosen

| Bytes | Meaning | Evidence |
|---|---|---|
| D_00810700 / 701 / 702 / 703 | area, sub-state, spawn entry, previous area | 001AD010 (BM) |
| D_008106B5..B8 | the area-change request: B5 area, B6 sub (0xFF = look up), B7 entry, B8 = 1 area change / 2 room move | 001BC150 (BM), 001B0C60 (BM), 001AD010 (BM) |
| D_00810730[area] | each area's current sub; bit 7 = kept. 001AD010 takes `D_00810730[B5] & 0x7F` when B6 = 0xFF | 001AD010 (BM) |
| D_00810841[area] | the area's lock byte; a door's bit is its id (`1 << (id & 31)` against a byte, so only ids whose low 5 bits are 0..7 can open: ids 0..7 and 0x80..0x87, e.g. AREA01 [14], id 0x81, tests bit 1) | 001BC350 (BM), 001BB860 (ins) |
| D_00810758[n] | story flags: op06 sub 0 / 1 store 1 / 0xFF; op07 sub 5 stores 0xFF; 001BA1C0 tests `== 0xFF` | 001BA080 (BM), 001B82D0 (BM), 001BA1C0 (BM) |
| D_008107D8[n] | story counters (the same index as the flag of the same event): op06 subs 3 / 5 / 6, op07 sub 6 | 001BA080, 001B82D0 (BM) |
| D_00810C64[n] ("C64") | item counts; 001C47A0 -> 001C40B0 (NM) adds; 001C47E0 (BM) takes (returns -1 without change when fewer are held) | BM / NM |
| D_00810CB8[n] ("CB8") | a second item table (001C4720, BM) | BM |
| D_00810CC3[n] ("CC3") | the third table (001C4760, BM): keys, notes and documents (an index >= 0x20 also posts a status request) | BM |

Sub changes. (1) A door or trigger request with an explicit sub (the door
record's byte 2 non-zero, or a 001B0C60 call). (2) Script op11 (001B7700,
NM): sub 1 requests `slot`, subs 0 / 2 request `slot | 0x80`; the frame
machine then calls 001FEFE0 / 001FF030 (BM), which keep a bit-7 value in
D_00810730[area]. Only one op11 record exists (AREA00 script 0x82A540:
AREA00 becomes sub 1, kept; measured in the third level). (3) Direct
stores: only AREA15's 0x823C80 (ins; section 7 step 6) writes
D_00810730[0, 1, 2, 4, 6] = 0x82, 0x81, 0x82, 0x81, 0x81. The boot writers
001AD360 / 001AD740 / 001AF220 (New Game, continue) store AREA11's sub 0.

## 3. Doors and lifts (boot behaviours)

- **001BC350** (hinged door, BM): sub-state 0 tests the lock only for model
  0x15: `D_00810841[area] & (1U << id)`.
- **001BB860** (slider, NM 57.68%; the gate read from its instructions): it
  tests the same bit for models 0x16, 0x17 and 0x3E; every other model
  opens.
- **001BC150** (the commit, BM): the record is
  `D_0024E140[area] + 4 * (id & 0x7F)`. Id bit 7 set: an area change to
  area rec[0], entry rec[1], sub rec[3] when rec[2] != 0, else 0xFF
  (D_00810730). Bit 7 clear: a room move to entry rec[side] (the side
  latch +0x2E).
- **001BD560** (lift, NM 99.08%): the commit through 001BC150 as above. Its
  state 0 waits for `+0x0B == 2`, which its call buttons write (below).
  Model 0x0B ("keyed"): at the first ride D_0081076C (flag 0x14) goes 0 -> 1
  and the lift parks in its state 1 sub-state 7 (+5) until the flag reads
  0xFF (measured, a04b_04) (AREA04's [4]
  0x824DC0, C: while the flag is 1, script 0x828BE0, then 0xFF). Other
  models toggle D_00810836[0 / 1] (areas 4 / 7) and are not gated.
- **Call buttons 001BC960 / 001BDFC0** (models 0 and 1; both BM).
  001BC960 is byte-matched and linked from its compiled C
  (tools/decomp/audit_link_provenance.py: route
  `compiled_object_ordinary_c`, with its local .rodata pinned); the rules
  below were also read from its original instructions (ins), and the two
  agree. A button's p[3] tells the outer one (0: the call, the
  linked lift's +0x0B 0 -> 2) from the inner one (1: the ride, 3 -> 4);
  the lift is reached through the node's +0x18 chain. 001BDFC0 tests the
  lock bit. 001BC960:
  - state 0 (001B0FD0 advances it to state 1 unless its 001B0EA0 test is
    non-zero; the rest can override that). Id 0xFF: the inner button at
    AREA13 entry 0 or AREA04 entry 7 (the spawn entry D_00810702; the two
    lift arrivals: lift [51] leads to AREA13 e0, AREA13's lift [10] to
    AREA04 e7) starts script 0x24E1A0 and runs state 4, then state 1. Id
    not 0xFF in AREA04: at entry 7 the same for the inner button; at other
    entries the outer button goes to state 2 while flag 0x12 (D_0081076A)
    is 0 and the inner one to state 2 sub-state 1 while flag 0x12 is not
    0xFF. Elsewhere state 1.
  - state 1 sub-state 0: Use is offered (001BC740) while the lift is ready
    (outer: its +0x0B == 0; inner: == 3), with no lock test for id 0xFF
    and the area's lock bit `1 << id` otherwise.
  - state 2 sub-state 0 (outer): with the lock bit, when the halfword at
    0x70003B84 is 0x208 it sets the lift's +0x0B = 2 and goes to state 1
    sub-state 5.
  - state 2 sub-state 1 (inner), only with the lock bit: flag 0x12 == 0xFF
    -> sub-state 2 (Use offered every frame). Otherwise, counter 0x12
    (D_008107EA) == 0x10: with D_008106C0 null, Use is offered every frame;
    with it not null, sub-state 2 when that record's +4 >= 2, else Use is
    offered while its float +0xB0 < 554.0. Any other counter value: nothing.
  - Use: 001BC740 (BM) acts when the node's +0x0B bit 2 is set; it turns
    the player to the button's yaw + pi, places the player in front of it
    (measured: 5 units out, at (566.2, 54.9, 241.7) for [54]) and starts
    script 0x24E3A0 (outer) or 0x24E560 (inner). State 1 sub-state 1 runs the script; at its end the outer button goes to
    sub-state 2 (then 4 at once or after a 300-frame wait, by D_0081083E
    and the id; sub-state 4 sets the lift's +0x0B 0 -> 2), the inner one to
    sub-state 6 (the lift's +0x0B 3 -> 4).
  Measured (a04b_04): with bit 7 set, flag 0x12 = 1, counter 0x12 = 0x10
  and D_008106C0 = 0, AREA04's inner button [54] (yaw +pi/2) took Use
  with the player facing -x and the lift left for AREA13.
- **001BD9F0** (NM): the call buttons of AREA13's door [14] (001BDE60):
  [15] (model 3, south side) and [16] (model 4, north side). Its state 0
  sets the side latch +0x2E to 1 for model 3 and 0 otherwise; sub-state 0
  offers Use through 001BC860, except that model 3 is refused (+0 = 2)
  while flag 0x1C (D_00810774) == 1; after the Use script, sub-state 2
  commits the room move through 001BC150 with the latch (door id 2, record
  08 03: model 4 -> entry 8, model 3 -> entry 3). AREA13's door [17]
  (0x823580, ins) has the same flag 0x1C == 1 refusal. Measured (a13_01):
  [16] took Use facing -z and moved the player to entry 8.

## 4. Lock-bit writers

Boot behaviours (each ORs `1 << +0x2E`, the record's id byte, into the
current area's lock byte):

| Behaviour | Condition | Evidence |
|---|---|---|
| 001581A0 (model 0x0E) | the object is hit (+0x36 != 0) | NM; measured (third level, a00_03) |
| 001582E0 (model 0x0E) | a hit; also D_00810842 \|= 2 (AREA01 bit 1) | NM |
| 00158430 (model 0x0E) | a hit; also D_00810845 \|= 4 (AREA04 bit 2) | BM |
| 00158810 reader | Use with item 0x23 (models 0x12, 0x2F) or item 0x24 (other models): 001576E0 starts script 0x246C20, whose op09 001580C0 sets the bit (not for model 0x2C); 00158810 also stores D_0081076A = 1 | BM (001576E0, 001580C0), NM (00158810); measured refused in the sixth level |
| 00158EC0 panel | 00157860 (arg 1): item 0x1B (D_00810C7F) held and +0x0B bit 0 -> script 0x2478A0 -> 001580C0 | BM |
| 00159210 panel | 00157860 (arg 0), the same through script 0x247BA0; model 0x2C sets no bit (001580C0 skips it) | BM |
| 00159E70 socket | item 0x29 (in area 4) or 0x2A (elsewhere) non-zero | BM |
| 0015A070 switch | Use: 00159FC0 -> script 0x246A20 -> 00157360 | NM |

Overlay code with a direct store (scan, then read): AREA04 0x823B40 (C,
bit 3; the fifth level's event), AREA06 keypad page 00207350 (BM, slot 0:
D_00810845 bit 5), AREA06 0x825E20 (C, D_00810845 bit 4), AREA15 0x824B40
(ins, D_00810847 bit 2), AREA19 0x825930 (ins, D_00810854 bit 2), AREA21
0x826310 / 0x827250 (scan: D_00810856 = 0xFF / 0) and 0x826BB0 (scan:
indexed). No script op writes a lock byte: the highest op06 / op07 counter
slot in any script is 0x67 (D_0081083F), below D_00810841.

## 5. Other area changes

| Site | Request | Condition | Evidence |
|---|---|---|---|
| 00190F20 (player frame, always first) | AREA14 entry 1 sub 0 / AREA18 entry 0 sub 0 | in area 0x12 with the player's +0xA0 <= 285; in area 0xE inside quad D_0024A4B0 | BM |
| 00193EB0 (action 0 handler) | AREA19 sub 0 entries 0xD / 0xB / 0xC | in area 0x13 sub 0, falling below y 356 / 365 in three x / z bands | NM |
| 00193EB0 | AREA19 sub 0 entry 9 / 0xA | in area 0xD (AREA13) with entry 4 / 6 resp. 5 / 7 and the player's y <= 159 | NM |
| 00196970 (called by 00196CE0) | AREA13 entry 6 / 7, AREA19 sub 1 entries 8 / 7 / 6 | five circles (radius 8); the player's height above a threshold | BM |
| 00179910 (player frame) | AREA02 sub 1 entry 6; sub 2 or 0 entry 5 | in AREA02 subs 0 / 2 within 8 of spawn entry 6; in sub 1 within 8 of entry 5 (sub 2 when D_00810732 bit 7) | NM |
| AREA00 0x823580 ([52], shaft door) | AREA01 entry 0 sub 0xFF | its progression branch (third level) | C |
| AREA00 0x826790 ([28], sub 2) | AREA14 entry 0 sub 0 | Use with +0x0B bit 0: takes item 0x26, script 0x82D070, then the request | C |
| AREA11 0x823C40 ([8]) / 0x827630 ([1], [2]) | AREA01 entry 4 (subs 0 / 1) | the first level's exits | scan; first level |
| AREA14 0x825C40 (op09 of script 0x828120, owner [6]) | AREA17 entry 0 sub 0 | the script's callback | scan, ins (args) |
| AREA15 0x823C80 (sub 1 [4] / [6]) | AREA15 entry 0 sub 0, by direct stores | counter 0x23's last step; see section 7 step 6 | ins |
| AREA17 0x825470 ([38]) | AREA11 entry 3 sub 0 | reads counter 0x5D; script 0x8284F0 (flag 0x30 = 1) | scan |
| AREA20 0x823BE0 ([32]) | AREA21 entry 3 sub 0 | counter 0x34 = 1 and the player in its quad: script 0x8270B0, then the request | scan |
| AREA21 0x827040 ([53]) | B8 = 1 with 0x70003B93 = 1 or 2: 001AD010 takes its non-load branch (the ending) | counter 0x37 set: script 0x82CBD0 | ins |

## 6. Per-area tables

The per-area tables follow (generated by `route_census.py graph`, then
annotated). "Owner" is the placement `sN[i]` or deferred record `sN g[i]`
that reaches the code. In the Destination column `eN` is the spawn entry
and `s=` the sub-state the load uses (a number from the door record, or
`D_00810730[area]` when the record leaves it to the area's current
sub-state). Section 7 carries this lane's reading of the rows the story
depends on.

<!-- world_graph tables begin -->
#### Boot ELF

Area-change requests 001B0C60: 0x190f20 (area 0xe, s=0x0, e=0x1); 0x190f20 (area 0x12, s=0x0, e=0x0); 0x193eb0 (area 0x13, s=0x0, e=0xd); 0x193eb0 (area ?, s=0x0, e=0xb); 0x193eb0 (area ?, s=0x0, e=0xc); 0x193eb0 (area 0x13, s=0x0, e=0x9); 0x193eb0 (area 0x13, s=0x0, e=0xa); 0x196970 (area 0xd, s=0x0, e=0x6); 0x196970 (area 0x13, s=0x1, e=0x8); 0x196970 (area 0x13, s=0x1, e=0x7); 0x196970 (area 0xd, s=0x0, e=0x7); 0x196970 (area 0x13, s=0x1, e=0x6)

Direct stores to D_008106B5..B8: 0x179910, 0x1b0c60, 0x1bc150

Stores to D_00810730[] (sub-states): 0x1ad360, 0x1ad740, 0x1af220, 0x1fefe0, 0x1ff030

Stores to the lock bytes D_00810840..57: 0x157360, 0x1580c0, 0x1581a0, 0x1582e0, 0x158430, 0x158810, 0x159e70, 0x207350

Direct flag stores: flag 0x12 = 0x1 (0x158810); flag 0x1c = 0xff (0x1aca20); flag 0x42 = 0xff (0x1aca20); flag 0x14 = ? (0x1bd560); flag 0xe = 0xff (0x1c02e0)

Boot script op09 callbacks: 0x246ba0 -> 0x157360 (chain 0x246a20); 0x246d20 -> 0x158130 (chain 0x246c20); 0x246da0 -> 0x158050 (chain 0x246c20); 0x246e20 -> 0x1580c0 (chain 0x246c20); 0x2473e0 -> 0x157f60 (chain 0x2471e0); 0x247660 -> 0x157f60 (chain 0x247420); 0x247760 -> 0x157f60 (chain 0x2476a0); 0x247860 -> 0x157f60 (chain 0x2477a0); 0x247a20 -> 0x158050 (chain 0x2478a0); 0x247aa0 -> 0x1580c0 (chain 0x2478a0); 0x247a20 -> 0x158050 (chain 0x2478e0); 0x247aa0 -> 0x1580c0 (chain 0x2478e0); 0x247ca0 -> 0x1575b0 (chain 0x247ba0); 0x247d20 -> 0x1580c0 (chain 0x247ba0); 0x247ca0 -> 0x1575b0 (chain 0x247be0); 0x247d20 -> 0x1580c0 (chain 0x247be0); 0x247f60 -> 0x157f30 (chain 0x247ee0); 0x247fe0 -> 0x1575e0 (chain 0x247fa0); 0x247fe0 -> 0x1575e0 (chain 0x247fe0); 0x248400 -> 0x1b6ea0 (chain 0x2482c0); 0x2484c0 -> 0x1b6ea0 (chain 0x248480); 0x24d9c0 -> 0x1bb400 (chain 0x24d900); 0x24da80 -> 0x1bb310 (chain 0x24da40); 0x24db00 -> 0x1bbae0 (chain 0x24da40); 0x24ddc0 -> 0x1bbae0 (chain 0x24dcc0); 0x24df00 -> 0x1bbbf0 (chain 0x24dec0); 0x24ddc0 -> 0x1bbae0 (chain 0x24dec0); 0x24e320 -> 0x1bc6d0 (chain 0x24e1a0); 0x24e5a0 -> 0x1bc560 (chain 0x24e560); 0x266760 -> 0x1b6ea0 (chain 0x266620); 0x266820 -> 0x1b6ea0 (chain 0x2667e0)

#### AREA00 (overlay id 1, subs 0, 1, 2, lock byte D_00810841, door table 0x24df80)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0+1 | [51] | 0x825170 (overlay) | 0x03 | 0x03 | room move e7 / e6 | see text |
| 0+1 | [52] | 0x823580 (overlay) | 0x03 | 0x80 | AREA01 e0 s=D_00810730[1] | see text |
| 0+1 | [55] | 001BC350 (hinged) | 0x15 | 0x02 | room move e3 / e4 | D_00810841 bit 2 |
| 0+1 | [56] | 001BC350 (hinged) | 0x03 | 0x01 | room move e1 / e2 | none |
| 0+1 | [58] | 001BB860 (slider) | 0x16 | 0x04 | room move e12 / e11 | D_00810841 bit 4 |
| 2 | [15] | 001BC350 (hinged) | 0x03 | 0x80 | AREA01 e0 s=D_00810730[1] | none |
| 2 | [20] | 001BB860 (slider) | 0x16 | 0x04 | room move e12 / e11 | D_00810841 bit 4 |

Other area-change sites: 0x823580 (owner s0[52], s1[52]) -> 001B0C60(area 0x1, s=0xff, e=0x0); 0x826790 (owner s2[28]) -> 001B0C60(area 0xe, s=0x0, e=0x0)

Lock-bit writers: s0[54] seal 001581A0 model 0x0e bit 2; s0[57] reader 00158810 (001576E0) model 0x13 bit 4; s1[54] seal 001581A0 model 0x0e bit 2; s1[57] reader 00158810 (001576E0) model 0x13 bit 4; s2[19] reader 00158810 (001576E0) model 0x13 bit 4

Flags / counters written by code: flag 0x3 = 0xff (0x825170, owner s0[51], s1[51]); counter 0x3 = 0xff (0x825170, owner s0[51], s1[51]); flag 0x3 = 0x1 (0x825170, owner s0[51], s1[51]); counter 0x4 = ? (0x825480, owner s0[40], s1[40]); counter 0x2b = 0x3 (0x825e80, owner s2[30]); counter 0x2c = ? (0x826790, owner s2[28]); flag 0x2c = 0xff (0x826790, owner s2[28]); flag 0x2c = ? (0x826790, owner s2[28])

Flags / counters written by scripts: counter 0x2 = 0x2 (script 0x8284e0 op06/3, started by 0x823580); flag 0x6 = 0xff (script 0x8286e0 op06/1, started by 0x823580); flag 0x2 = 0x1 (script 0x828d60 op06/0, started by 0x824ea0); counter 0x2 = 0x1 (script 0x828d60 op06/3, started by 0x824ea0); flag 0x2 = 0xff (script 0x828d60 op07/5, started by 0x824ea0); flag 0x4 = 0xff (script 0x8299e0 op06/1, started by 0x825480); counter 0x4 = 0x2 (script 0x829be0 op06/3, started by 0x825600); counter 0x4 = 0x0 (script 0x82a0e0 op06/3, started by 0x825600); flag 0x5 = 0xff (script 0x82a540 op07/5, started by 0x825920); flag 0xa = 0xff (script 0x82a720 op07/5, started by 0x825c80); counter 0x2b = 0x1 (script 0x82abc0 op06/3, started by 0x825e80); counter 0x2b = 0x2 (script 0x82abc0 op06/3, started by 0x825e80); counter 0x2b = 0x81 (script 0x82b080 op06/3, started by 0x825fc0); counter 0x2b = 0xff (script 0x82b080 op06/3, started by 0x825fc0); flag 0x2b = 0xff (script 0x82b080 op07/5, started by 0x825fc0); flag 0x43 = 0xff (script 0x82b790 op07/5, started by 0x8261e0); flag 0x0 = 0xff (script 0x82b990 op07/5, started by 0x8262d0); flag 0x0 = 0xff (script 0x82d070 op07/5, started by 0x826790)

Flags tested (001BA1C0): 0x2b

Pickups: C64 0x10: s0 g[26], s0 g[27], s1 g[26], s1 g[27]; C64 0x15: s2 g[0]; C64 0x1e: s0 g[29], s0 g[32], s0 g[37], s1 g[29], s1 g[32], s1 g[37], s2 g[5]; C64 0x2: s2 g[9]; C64 0x20: s0 g[33], s1 g[33]; C64 0x21: s0 g[36], s0 g[38], s1 g[36], s1 g[38], s2 g[8]; C64 0x26: s2 g[3]; CB8 0xa: s2 g[10]; CC3 0x21: s0 g[31], s1 g[31]; CC3 0x22: s2 g[2]; CC3 0x35: s0 g[35], s1 g[35]; CC3 0x36: s0 g[34], s1 g[34], s2 g[7]; CC3 0x37: s0 g[30], s1 g[30], s2 g[6]; CC3 0x38: s0 g[28], s1 g[28], s2 g[1]; CC3 0x5c: s2 g[4]

Items by code: CC3 ? += 0x1 (0x824ea0, owner s0 g[25], s1 g[25]); CC3 0x4 += 0x1 (0x825170, owner s0[51], s1[51]); C64 0x26 -= ? (001C47E0, 0x826790, owner s2[28])

Sub-state writes: op11 in script 0x82a540 (started by 0x825920): sub request 0x81

#### AREA01 (overlay id 2, subs 0, 1, lock byte D_00810842, door table 0x24dfa0)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [12] | 0x823580 (overlay) | 0x03 | 0x80 | AREA00 e0 s=D_00810730[0] | see text |
| 0 | [14] | 001BC350 (hinged) | 0x15 | 0x81 | AREA02 e0 s=D_00810730[2] | D_00810842 bit 1 |
| 0 | [15] | 001BC350 (hinged) | 0x03 | 0x02 | room move e1 / e2 | none |
| 0 | [16] | 001BC350 (hinged) | 0x03 | 0x83 | AREA02 e1 s=1 | none |
| 0 | [17] | 001BB860 (slider) | 0x09 | 0x04 | room move e9 / e8 | none |
| 0 | [18] | 001BB860 (slider) | 0x09 | 0x85 | AREA22 e5 s=D_00810730[22] | none |
| 0 | [19] | 001BC350 (hinged) | 0x03 | 0x86 | AREA06 e0 s=D_00810730[6] | none |
| 1 | [9] | 001BC350 (hinged) | 0x03 | 0x80 | AREA00 e0 s=D_00810730[0] | none |
| 1 | [11] | 001BC350 (hinged) | 0x15 | 0x81 | AREA02 e0 s=D_00810730[2] | D_00810842 bit 1 |
| 1 | [12] | 001BC350 (hinged) | 0x03 | 0x83 | AREA02 e1 s=1 | none |
| 1 | [13] | 001BB860 (slider) | 0x09 | 0x04 | room move e9 / e8 | none |
| 1 | [14] | 001BB860 (slider) | 0x09 | 0x85 | AREA22 e5 s=D_00810730[22] | none |
| 1 | [15] | 001BC350 (hinged) | 0x03 | 0x86 | AREA06 e0 s=D_00810730[6] | none |

Flags / counters written by code: counter 0x1 = 0x80 (0x823580, owner s0[12]); counter 0x7 = 0x10 (0x824770, owner s0 g[0], s0 g[1], s0 g[2], s0 g[3], s0 g[4]); flag 0x1 = 0xff (0x825590, owner s0[36]); counter 0x1 = 0x81 (0x825590, owner s0[36]); counter 0x7 = 0x1 (0x825be0, owner s0 g[5], s0 g[7]); counter 0x7 = 0x2 (0x825be0, owner s0 g[5], s0 g[7]); counter 0x7 = 0x40 (0x825d30, owner s0 g[5], s0 g[7]); counter 0x7 = 0x80 (0x825ea0, owner s0 g[5], s0 g[7]); counter 0x7 = 0xff (0x825f00, owner s0 g[5], s0 g[7]); counter 0x8 = 0x1 (0x826440, owner s0[41], s0[42], s1[17], s1[18]); counter 0x8 = 0xff (0x826440, owner s0[41], s0[42], s1[17], s1[18]); counter 0x3c = 0xff (0x826ba0, owner s1[19])

Flags / counters written by scripts: flag 0x7 = 0xff (script 0x82ad90 op07/5, started by 0x825f00); counter 0x8 = 0xe0 (script 0x82b0d0 op06/3, started by 0x826440); counter 0x8 = 0x2 (script 0x82b0d0 op06/3, started by 0x826440); flag 0x8 = 0xff (script 0x82b0d0 op07/5, started by 0x826440); flag 0xf = 0xff (script 0x82b590 op07/5, started by 0x8267c0); flag 0x3c = 0xff (script 0x82bad0 op07/5, started by 0x826ba0)

Flags tested (001BA1C0): 0x6, 0x7, 0x8, 0xf

Pickups: C64 0x10: s0 g[16], s1 g[5]; C64 0x1e: s0 g[10], s0 g[13], s0 g[14], s1 g[2], s1 g[4]; CB8 0x0: s0 g[17]; CC3 0x20: s0 g[15]; CC3 0x33: s0 g[11], s1 g[3]; CC3 0x34: s0 g[12]; CC3 0x48: s0 g[9], s1 g[1]; CC3 0x49: s0 g[8], s1 g[0]

Items by code: CC3 0x2 += 0x1 (0x825590, owner s0[36]); C64 0x20 += 0x1 (0x825f00, owner s0 g[5], s0 g[7]); CC3 0x5 += 0x1 (0x825f00, owner s0 g[5], s0 g[7]); CC3 0x1b += 0x1 (0x826ba0, owner s1[19])

#### AREA02 (overlay id 3, subs 0, 1, 2, lock byte D_00810843, door table 0x24dfc0)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [22] | 001BC350 (hinged) | 0x15 | 0x80 | AREA01 e3 s=D_00810730[1] | D_00810843 bit 0 |
| 0 | [25] | 001BB860 (slider) | 0x17 | 0x83 | AREA04 e0 s=D_00810730[4] | D_00810843 bit 3 |
| 1 | [5] | 001BC350 (hinged) | 0x03 | 0x81 | AREA01 e5 s=D_00810730[1] | none |
| 1 | [6] | 001BC350 (hinged) | 0x03 | 0x02 | room move e3 / e2 | none |
| 2 | [22] | 001BC350 (hinged) | 0x15 | 0x80 | AREA01 e3 s=D_00810730[1] | D_00810843 bit 0 |
| 2 | [25] | 001BB860 (slider) | 0x17 | 0x83 | AREA04 e0 s=D_00810730[4] | D_00810843 bit 3 |

Lock-bit writers: s0[21] seal 001582E0 model 0x0e bit 0; s0[24] panel 00158EC0 (00157860 arg 1) model 0x14 bit 3; s1[4] panel 00159210 (00157860 arg 0) model 0x2c bit 0; s2[21] seal 001582E0 model 0x0e bit 0; s2[24] panel 00158EC0 (00157860 arg 1) model 0x14 bit 3

Flags / counters written by code: flag 0x9 = 0xff (0x823980, owner s0[31], s0[32], s0[33], s0[34], s0[35]); counter 0x9 = 0xff (0x823980, owner s0[31], s0[32], s0[33], s0[34], s0[35]); flag 0x9 = 0x1 (0x823980, owner s0[31], s0[32], s0[33], s0[34], s0[35]); counter 0x9 = 0x1 (0x823980, owner s0[31], s0[32], s0[33], s0[34], s0[35]); counter 0x9 = ? (0x823980, owner s0[31], s0[32], s0[33], s0[34], s0[35]); flag 0x9 = 0xff (0x824020, owner s0[31], s0[32], s0[33], s0[34], s0[35]); counter 0x9 = 0xff (0x824020, owner s0[31], s0[32], s0[33], s0[34], s0[35]); counter 0x9 = ? (0x8242f0, owner s0[31], s0[32], s0[33], s0[34], s0[35]); counter 0x9 = ? (0x8242f0, owner s0[31], s0[32], s0[33], s0[34], s0[35]); counter 0x9 = ? (0x8242f0, owner s0[31], s0[32], s0[33], s0[34], s0[35]); counter 0x9 = ? (0x8242f0, owner s0[31], s0[32], s0[33], s0[34], s0[35]); counter 0x67 = 0x0 (0x825100, owner s0[27], s0[28], s2[27], s2[28]); counter 0x67 = 0x0 (0x825100, owner s0[27], s0[28], s2[27], s2[28]); counter 0x67 = 0x1 (0x825100, owner s0[27], s0[28], s2[27], s2[28]); counter 0x67 = 0x0 (0x825100, owner s0[27], s0[28], s2[27], s2[28])

Flags / counters written by scripts: flag 0x9 = 0xff (script 0x826b40 op06/1, started by 0x823980); flag 0x9 = 0xff (script 0x826b40 op07/5, started by 0x823980); flag 0x9 = 0xff (script 0x826f40 op06/1, started by 0x823980); flag 0x9 = 0xff (script 0x826f40 op07/5, started by 0x823980); flag 0xb = 0xff (script 0x827670 op07/5, started by 0x824fa0); counter 0x67 = 0x2 (script 0x828e10 op06/3, started by 0x825100); counter 0x67 = 0x1 (script 0x828e10 op06/3, started by 0x825100)

Flags tested (001BA1C0): 0x9

Pickups: C64 0x1e: s0 g[13], s0 g[18], s2 g[0], s2 g[5]; C64 0x20: s0 g[15], s1 g[1], s2 g[2]; C64 0x31: s0 g[17], s2 g[4]; CB8 0x4: s0 g[19], s2 g[6]; CC3 0x23: s1 g[2]; CC3 0x24: s1 g[0]; CC3 0x25: s0 g[16], s2 g[3]; CC3 0x39: s0 g[14], s2 g[1]

#### AREA03 (overlay id 4, subs 0, 1, 2, lock byte D_00810844, door table 0x24dfd0)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [13] | 001BC350 (hinged) | 0x03 | 0x85 | AREA03 e1 s=1 | none |
| 0 | [15] | 001BB860 (slider) | 0x08 | 0x86 | AREA19 e0 s=D_00810730[19] | none |
| 1 | [24] | 001BC350 (hinged) | 0x15 | 0x80 | AREA04 e2 s=D_00810730[4] | D_00810844 bit 0 |
| 1 | [25] | 001BC350 (hinged) | 0x03 | 0x81 | AREA03 e0 s=0 | none |
| 1 | [26] | 001BC350 (hinged) | 0x03 | 0x02 | room move e3 / e2 | none |
| 1 | [28] | 001BC350 (hinged) | 0x15 | 0x03 | room move e5 / e4 | D_00810844 bit 3 |
| 1 | [29] | 001BB860 (slider) | 0x08 | 0x84 | AREA03 e0 s=2 | none |
| 2 | [5] | 001BB860 (slider) | 0x08 | 0x88 | AREA03 e6 s=1 | none |
| 2 | [7] | 001BD560 (lift) | 0x0d | 0x87 | AREA08 e0 s=D_00810730[8] | see text |

Lock-bit writers: s1[23] seal 00158430 model 0x0e bit 0; s1[27] seal 001581A0 model 0x0e bit 3; s2[6] socket 00159E70 model 0x3c bit 7

Flags / counters written by code: counter 0x29 = 0xff (0x8235a0, owner s1[22]); flag 0x29 = ? (0x823810, owner s1 g[3])

Flags / counters written by scripts: flag 0x29 = 0x1 (script 0x827170 op06/0, started by 0x8235a0); counter 0x29 = 0x1 (script 0x827170 op06/3, started by 0x8235a0); flag 0x29 = 0xff (script 0x827170 op07/5, started by 0x8235a0); flag 0x0 = 0xff (script 0x827630 op07/5, started by 0x8235a0)

Pickups: C64 0x15: s1 g[11]; C64 0x16: s1 g[0]; C64 0x1e: s0 g[0]; C64 0x1f: s1 g[6]; C64 0x2f: s1 g[7]; C64 0x32: s0 g[1]; C64 0x33: s1 g[2]; C64 0x36: s2 g[0]; C64 0x39: s1 g[8]; C64 0xb: s1 g[1]; CC3 0x2b: s1 g[5]; CC3 0x40: s1 g[10]; CC3 0x41: s0 g[2]; CC3 0x42: s1 g[4]; CC3 0x58: s1 g[9]

Items by code: C64 0x2a += 0x1 (0x8235a0, owner s1[22]); CC3 0x12 += 0x1 (0x8235a0, owner s1[22])

#### AREA04 (overlay id 5, subs 0, 1, lock byte D_00810845, door table 0x24e000)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [35] | 001BB860 (slider) | 0x09 | 0x88 | AREA02 e4 s=D_00810730[2] | none |
| 0 | [37] | 001BC350 (hinged) | 0x03 | 0x81 | AREA22 e0 s=D_00810730[22] | none |
| 0 | [38] | 001BC350 (hinged) | 0x15 | 0x82 | AREA03 e0 s=1 | D_00810845 bit 2 |
| 0 | [40] | 001BC350 (hinged) | 0x15 | 0x03 | room move e4 / e3 | D_00810845 bit 3 |
| 0 | [42] | 001BC350 (hinged) | 0x15 | 0x84 | AREA20 e0 s=D_00810730[20] | D_00810845 bit 4 |
| 0 | [45] | 0x823700 (overlay) | 0x16 | 0x05 | room move e8 / e9 | see text |
| 0 | [49] | 001BC350 (hinged) | 0x15 | 0x06 | room move e11 / e12 | D_00810845 bit 6 |
| 0 | [51] | 001BD560 (lift) | 0x0b | 0x87 | AREA13 e0 s=D_00810730[13] | flag 0x14 key (first Use sets it to 1, runs on at 0xFF) |
| 0 | [56] | 001BD560 (lift) | 0x0d | 0x80 | AREA07 e0 s=D_00810730[7] | see text |
| 1 | [41] | 001BB860 (slider) | 0x09 | 0x88 | AREA02 e4 s=D_00810730[2] | none |
| 1 | [43] | 001BC350 (hinged) | 0x03 | 0x81 | AREA22 e0 s=D_00810730[22] | none |
| 1 | [44] | 001BC350 (hinged) | 0x03 | 0x82 | AREA03 e0 s=1 | none |
| 1 | [46] | 001BC350 (hinged) | 0x15 | 0x03 | room move e4 / e3 | D_00810845 bit 3 |
| 1 | [48] | 001BC350 (hinged) | 0x15 | 0x84 | AREA20 e0 s=D_00810730[20] | D_00810845 bit 4 |
| 1 | [51] | 001BB860 (slider) | 0x16 | 0x05 | room move e8 / e9 | D_00810845 bit 5 |
| 1 | [55] | 001BC350 (hinged) | 0x15 | 0x06 | room move e11 / e12 | D_00810845 bit 6 |
| 1 | [57] | 001BD560 (lift) | 0x0b | 0x87 | AREA13 e0 s=D_00810730[13] | flag 0x14 key (first Use sets it to 1, runs on at 0xFF) |
| 1 | [62] | 001BD560 (lift) | 0x0d | 0x80 | AREA07 e0 s=D_00810730[7] | see text |

Lock-bit writers: s0[48] seal 001581A0 model 0x0e bit 6; s0[50] reader 00158810 (001576E0) model 0x2f bit 7; s0[55] socket 00159E70 model 0x3c bit 0; s1[54] seal 001581A0 model 0x0e bit 6; s1[56] reader 00158810 (001576E0) model 0x2f bit 7; s1[61] socket 00159E70 model 0x3c bit 0; 0x823b40 (owner s0[1]) stores D_00810845

Flags / counters written by code: flag 0xc = 0xff (0x823b90, owner s0[1]); flag 0xc = 0xff (0x823b90, owner s0[1]); counter 0xc = 0x2 (0x823b90, owner s0[1]); counter 0xd = 0xff (0x8241f0, owner s0[61], s1[67]); counter 0x11 = 0xff (0x824320, owner s0[2]); counter 0x11 = 0x1 (0x824490, owner s0[2]); counter 0x12 = 0x10 (0x824830, owner s0[3]); flag 0x12 = 0xff (0x824930, owner s0[3]); counter 0x12 = 0xff (0x824930, owner s0[3]); counter 0x12 = 0xff (0x824930, owner s0[3]); flag 0x14 = 0xff (0x824dc0, owner s0[4]); flag 0x28 = 0x1 (0x825040, owner s1[1]); counter 0x28 = 0x1 (0x825040, owner s1[1]); counter 0x49 = ? (0x825310, owner s1[3]); counter 0x49 = ? (0x825310, owner s1[3]); counter 0x49 = ? (0x825310, owner s1[3]); flag 0x65 = 0x1 (0x825510, owner s0[62], s0[63], s1[68], s1[69]); flag 0x65 = 0x0 (0x825510, owner s0[62], s0[63], s1[68], s1[69]); counter 0x5c = 0x1 (0x825b00, owner s0[65], s1[36]); counter 0x5c = 0x0 (0x825b00, owner s0[65], s1[36]); counter 0x63 = 0xff (0x825df0, owner s0[69]); counter 0x63 = 0xff (0x825df0, owner s0[69])

Flags / counters written by scripts: counter 0xc = 0x1 (script 0x8275a0 op07/6, started by 0x823b90); flag 0xc = 0xff (script 0x8278d0 op07/5, started by 0x823b90); flag 0xd = 0xff (script 0x827d10 op06/1, started by 0x8241f0); flag 0x11 = 0xff (script 0x827d90 op07/5, started by 0x824490); counter 0x12 = 0x2 (script 0x8286d0 op06/3, started by 0x824930); flag 0x12 = 0xff (script 0x8286d0 op07/5, started by 0x824930); flag 0x14 = 0xff (script 0x828be0 op07/5, started by 0x824dc0); flag 0x28 = 0xff (script 0x829020 op07/5, started by 0x825040); flag 0x49 = 0xff (script 0x829570 op07/5, started by 0x825310); flag 0x49 = 0xff (script 0x829930 op07/5, started by 0x825310); flag 0x49 = 0xff (script 0x829cf0 op07/5, started by 0x825310); counter 0x65 = 0x1 (script 0x82bd60 op06/3, started by 0x825510); flag 0xd = 0xff (script 0x82bd60 op07/5, started by 0x825510); counter 0x65 = 0x2 (script 0x82c060 op06/3, started by 0x825510); counter 0x65 = 0x2 (script 0x82c120 op06/3, started by 0x825510)

Flags tested (001BA1C0): 0xc, 0xd, 0x12, 0x14, 0x1a, 0x28, 0x31, 0x65

Pickups: C64 0x1: s0 g[4], s1 g[4]; C64 0x10: s0 g[9], s1 g[9]; C64 0x11: s0 g[7], s1 g[7]; C64 0x1e: s0 g[3], s0 g[5], s1 g[3], s1 g[5]; C64 0x1f: s0 g[10]; C64 0x20: s0 g[6], s1 g[6]; C64 0x2d: s0 g[11], s1 g[10]; C64 0x8: s0 g[2], s1 g[2]; CB8 0x1: s0 g[12], s1 g[11]; CC3 0x3a: s0 g[8], s1 g[8]; CC3 0x4a: s0 g[1], s1 g[1]; CC3 0x4b: s0 g[0], s1 g[0]

Items by code: CC3 0x6 += 0x1 (0x823b90, owner s0[1]); C64 0x23 += 0x1 (0x824490, owner s0[2]); CC3 0x8 += 0x1 (0x824490, owner s0[2]); C64 0x29 += 0x1 (0x825040, owner s1[1]); CC3 0x10 += 0x1 (0x825040, owner s1[1])

#### AREA06 (overlay id 6, subs 0, 1, lock byte D_00810847, door table 0x24e028)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [1] | 001BC350 (hinged) | 0x03 | 0x80 | AREA01 e7 s=D_00810730[1] | none |
| 0 | [2] | 001BC350 (hinged) | 0x03 | 0x01 | room move e2 / e1 | none |
| 0 | [3] | 001BB860 (slider) | 0x3e | 0x82 | AREA16 e0 s=D_00810730[16] | D_00810847 bit 2 |
| 1 | [1] | 001BB860 (slider) | 0x3e | 0x82 | AREA16 e0 s=D_00810730[16] | D_00810847 bit 2 |
| 1 | [4] | 001BC350 (hinged) | 0x03 | 0x80 | AREA01 e7 s=D_00810730[1] | none |
| 1 | [5] | 001BC350 (hinged) | 0x03 | 0x01 | room move e2 / e1 | none |

Lock-bit writers: 0x825e20 (owner s1[44]) stores D_00810845

Flags / counters written by code: flag 0x10 = 0x1 (0x824560, owner s0[11]); flag 0x10 = 0xff (0x824560, owner s0[11]); counter 0x2a = 0xff (0x825e20, owner s1[44])

Flags / counters written by scripts: flag 0x10 = 0xff (script 0x827180 op07/5, started by 0x824560); flag 0x2a = 0xff (script 0x8276c0 op07/5, started by 0x825e20)

Flags tested (001BA1C0): 0x10, 0x2a, 0x31

Pickups: C64 0x10: s0 g[7], s1 g[7]; C64 0x1e: s0 g[10], s1 g[10]; C64 0x20: s0 g[1], s0 g[8], s0 g[9], s1 g[1], s1 g[8], s1 g[9]; C64 0x21: s0 g[2], s1 g[2]; C64 0x30: s0 g[4], s1 g[4]; CC3 0x3b: s0 g[6], s1 g[6]; CC3 0x3c: s0 g[5], s1 g[5]; CC3 0x4d: s0 g[3], s1 g[3]; CC3 0x4e: s0 g[0], s1 g[0]

Items by code: CC3 0x7 += 0x1 (0x824340, owner s0[6]); CC3 0x1f += 0x1 (0x825e20, owner s1[44])

#### AREA07 (overlay id 7, subs 0, 1, 2, 3, lock byte D_00810848, door table 0x24e040)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0+1 | [0] | 001BD560 (lift) | 0x0d | 0x80 | AREA04 e10 s=D_00810730[4] | see text |
| 0+1 | [5] | 001BB860 (slider) | 0x16 | 0x81 | AREA07 e0 s=2 | D_00810848 bit 1 |
| 0+1 | [8] | 001BB860 (slider) | 0x08 | 0x02 | room move e7 / e8 | none |
| 0+1 | [10] | 001BB860 (slider) | 0x08 | 0x03 | room move e9 / e10 | none |
| 0+1 | [12] | 001BB860 (slider) | 0x08 | 0x04 | room move e12 / e11 | none |
| 0+1 | [14] | 001BC350 (hinged) | 0x03 | 0x05 | room move e4 / e3 | none |
| 0+1 | [15] | 001BC350 (hinged) | 0x03 | 0x06 | room move e2 / e1 | none |
| 2 | [11] | 001BB860 (slider) | 0x08 | 0x87 | AREA07 e5 s=0 | none |
| 2 | [13] | 001BB860 (slider) | 0x08 | 0x88 | AREA07 e0 s=3 | none |
| 3 | [0] | 001BB860 (slider) | 0x08 | 0x89 | AREA07 e1 s=2 | none |
| 3 | [1] | 001BC350 (hinged) | 0x03 | 0x0a | room move e2 / e1 | none |

Lock-bit writers: s0[7] switch 0015A070 model 0x05 bit 1; s0[20] panel 00159210 (00157860 arg 0) model 0x2c bit 0; s1[7] switch 0015A070 model 0x05 bit 1; s1[20] panel 00159210 (00157860 arg 0) model 0x2c bit 0

Flags / counters written by code: counter 0x15 = 0xff (0x823b40, owner s3[7]); flag 0x15 = 0xff (0x823c20, owner s3[7]); counter 0x15 = 0xff (0x823c20, owner s3[7])

Flags / counters written by scripts: flag 0x15 = 0xff (script 0x8264b0 op07/5, started by 0x823b40); flag 0x25 = 0x1 (script 0x826930 op06/0, started by 0x823c20); flag 0x25 = 0xff (script 0x826930 op07/5, started by 0x823c20); flag 0x0 = 0xff (script 0x8271a0 op07/5, started by 0x823da0); flag 0x0 = 0xff (script 0x827360 op07/5, started by 0x823da0)

Flags tested (001BA1C0): 0x15

Pickups: C64 0x1e: s2 g[0], s3 g[2]; C64 0x20: s3 g[1]; C64 0xa: s0 g[0]; CB8 0x5: s0 g[4]; CC3 0x43: s0 g[3]; CC3 0x59: s0 g[2]; CC3 0x61: s0 g[1]; CC3 0x62: s3 g[0]

Items by code: CC3 0x11 += 0x1 (0x823b40, owner s3[7]); CC3 0x11 += 0x1 (0x823c20, owner s3[7]); CC3 0x14 += 0x1 (0x823c20, owner s3[7])

#### AREA08 (overlay id 8, subs 0, 1, 2, 3, lock byte D_00810849, door table 0x24e070)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0+1 | [0] | 001BC350 (hinged) | 0x03 | 0x04 | room move e4 / e3 | none |
| 0+1 | [1] | 001BC350 (hinged) | 0x03 | 0x05 | room move e2 / e1 | none |
| 0+1 | [2] | 001BB860 (slider) | 0x16 | 0x81 | AREA08 e0 s=2 | D_00810849 bit 1 |
| 0+1 | [6] | 0x823870 (overlay) | 0x08 | 0x02 | room move e7 / e6 | see text |
| 0+1 | [8] | 001BD560 (lift) | 0x0d | 0x80 | AREA03 e1 s=2 | see text |
| 2 | [2] | 001BB860 (slider) | 0x08 | 0x87 | AREA08 e5 s=0 | none |
| 2 | [3] | 001BB860 (slider) | 0x08 | 0x88 | AREA08 e0 s=3 | none |
| 3 | [5] | 001BC350 (hinged) | 0x03 | 0x06 | room move e2 / e1 | none |
| 3 | [6] | 001BB860 (slider) | 0x08 | 0x83 | AREA08 e1 s=2 | none |

Lock-bit writers: s0[4] switch 0015A070 model 0x05 bit 1; s1[4] switch 0015A070 model 0x05 bit 1; s3[8] panel 00159210 (00157860 arg 0) model 0x2c bit 0

Flags / counters written by code: counter 0x17 = 0x1 (0x824210, owner s0[12], s1[12]); counter 0x17 = 0xff (0x824210, owner s0[12], s1[12]); counter 0x18 = 0xff (0x824790, owner s3[2]); flag 0x18 = 0xff (0x824870, owner s3[2]); counter 0x18 = 0xff (0x824870, owner s3[2])

Flags / counters written by scripts: flag 0x17 = 0x1 (script 0x825880 op06/0, started by 0x823630); flag 0x17 = 0xff (script 0x825e60 op07/5, started by 0x824210); flag 0x18 = 0x1 (script 0x826420 op06/0, started by 0x824790); flag 0x18 = 0xff (script 0x826420 op07/5, started by 0x824790); flag 0x18 = 0x1 (script 0x8268e0 op06/0, started by 0x824870); flag 0x25 = 0x1 (script 0x8268e0 op06/0, started by 0x824870); flag 0x25 = 0xff (script 0x8268e0 op07/5, started by 0x824870); flag 0x0 = 0xff (script 0x827170 op07/5, started by 0x8249f0); flag 0x0 = 0xff (script 0x827330 op07/5, started by 0x8249f0)

Flags tested (001BA1C0): 0x17, 0x18

Pickups: C64 0x10: s0 g[3]; C64 0x12: s3 g[3], s3 g[5]; C64 0x15: s3 g[2]; C64 0x21: s3 g[4]; C64 0x9: s0 g[2]; CB8 0x6: s0 g[6]; CC3 0x44: s0 g[4]; CC3 0x45: s0 g[5]; CC3 0x5b: s0 g[0]; CC3 0x64: s0 g[1]; CC3 0x65: s3 g[0]; CC3 0x66: s3 g[1]

Items by code: CC3 0x63 += 0x1 (0x824210, owner s0[12], s1[12]); CC3 0x13 += 0x1 (0x824790, owner s3[2]); CC3 0x13 += 0x1 (0x824870, owner s3[2]); CC3 0x14 += 0x1 (0x824870, owner s3[2])

#### AREA11 (overlay id 9, subs 0, lock byte D_0081084C, door table 0x2755f8)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [0] | 001BC350 (hinged) | 0x03 | 0x00 | room move e2 / e1 | none |

Other area-change sites: 0x823c40 (owner s0[8]) -> 001B0C60(area 0x1, s=0x0, e=0x4); 0x827630 (owner s0[1], s0[2]) -> 001B0C60(area 0x1, s=0x1, e=0x4); 0x827630 (owner s0[1], s0[2]) -> 001B0C60(area ?, s=?, e=0x4)

Lock-bit writers: s0[18] panel 00159210 (00157860 arg 0) model 0x24 bit 7

Flags / counters written by code: counter 0x3b = 0x11 (0x823910, owner s0[8]); counter 0x0 = ? (0x823910, owner s0[8]); counter 0x30 = 0xff (0x823ce0, owner s0[11]); counter 0x39 = 0xff (0x823e80, owner s0[10]); flag 0x3a = 0xff (0x823ff0, owner s0[16]); flag 0x3a = 0x1 (0x8251e0, owner s0[17]); counter 0x3b = 0x10 (0x825500, owner s0[12]); counter 0x3b = 0x20 (0x825600, owner s0[12]); counter 0x3b = 0xff (0x8256d0, owner s0[12]); counter 0x3c = 0x1 (0x8257a0, owner s0[13]); counter 0x0 = ? (0x827630, owner s0[1], s0[2]); counter 0x0 = ? (0x827630, owner s0[1], s0[2]); counter 0x62 = ? (0x827b10, owner s0[19])

Flags / counters written by scripts: flag 0x0 = 0x1 (script 0x8283d0 op06/0, started by 0x823910); counter 0x3b = 0x1 (script 0x828990 op06/3, started by 0x823910); flag 0x0 = 0xff (script 0x828a10 op07/5, started by 0x823c40); flag 0x30 = 0xff (script 0x828c70 op07/5, started by 0x823ce0); flag 0x39 = 0x1 (script 0x828fc0 op06/0, started by 0x823e80); flag 0x39 = 0xff (script 0x828fc0 op07/5, started by 0x823e80); flag 0x3b = 0x1 (script 0x8294c0 op06/0, started by 0x825500); flag 0x3b = 0xff (script 0x8294c0 op07/5, started by 0x825500); flag 0x3c = 0x1 (script 0x829e80 op06/0, started by 0x8257a0); flag 0x3c = 0x1 (script 0x829e80 op06/0, started by 0x8257a0)

Flags tested (001BA1C0): 0x0, 0x30, 0x39, 0x3b, 0x3c

Pickups: C64 0x10: s0 g[5]; C64 0x1b: s0 g[0]; C64 0x1e: s0 g[1], s0 g[3]; C64 0x1f: s0 g[2]; CB8 0x8: s0 g[6]; CC3 0x32: s0 g[4]

Items by code: CC3 0x1a += 0x1 (0x823ce0, owner s0[11]); CC3 0x0 += 0x1 (0x823e80, owner s0[10]); CC3 0x1 += 0x1 (0x825500, owner s0[12])

#### AREA13 (overlay id 10, subs 0, lock byte D_0081084E, door table 0x24e0a0)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [8] | 001BC350 (hinged) | 0x03 | 0x01 | room move e2 / e1 | none |
| 0 | [10] | 001BD560 (lift) | 0x0b | 0x80 | AREA04 e7 s=D_00810730[4] | flag 0x14 key (first Use sets it to 1, runs on at 0xFF) |
| 0 | [15] | 001BD9F0 (door) | 0x03 | 0x02 | room move e8 / e3 | see text |
| 0 | [16] | 001BD9F0 (door) | 0x04 | 0x02 | room move e8 / e3 | see text |
| 0 | [17] | 0x823580 (overlay) | 0x03 | 0x03 | room move e9 / e4 | see text |
| 0 | [20] | 001BC350 (hinged) | 0x03 | 0x04 | room move e5 / e10 | none |

Lock-bit writers: s0[60] panel 00159210 (00157860 arg 0) model 0x2c bit 0

Flags / counters written by code: flag 0x19 = 0x1 (0x823830, owner s0[3]); counter 0x19 = 0x1 (0x823830, owner s0[3]); counter 0x1c = ? (0x824160, owner s0[44]); flag 0x1c = 0x1 (0x824180, owner s0[44]); counter 0x1c = ? (0x824180, owner s0[44]); counter 0x1c = ? (0x824390, owner s0[44]); counter 0x1c = ? (0x824390, owner s0[44]); counter 0x1c = ? (0x824520, owner s0[44]); counter 0x42 = 0xff (0x824a80, owner s0[7]); counter 0x61 = ? (0x826850, owner s0[62], s0[63]); counter 0x61 = ? (0x826850, owner s0[62], s0[63]); counter 0x1c = ? (0x827150, owner s0[45], s0[46]); counter 0x1c = ? (0x827150, owner s0[45], s0[46]); counter 0x1c = ? (0x827150, owner s0[45], s0[46]); counter 0x5b = 0xff (0x827f90, owner —)

Flags / counters written by scripts: flag 0x19 = 0xff (script 0x82a360 op07/5, started by 0x823830); counter 0x1a = 0xff (script 0x82a770 op06/3, started by 0x823a40); flag 0x1a = 0xff (script 0x82a770 op07/5, started by 0x823a40); flag 0x1c = 0xff (script 0x82c110 op07/5, started by 0x824960); flag 0x42 = 0xff (script 0x82c510 op07/5, started by 0x824a80); flag 0x0 = 0xff (script 0x82ce10 op07/5, started by 0x826ff0); flag 0x0 = 0xff (script 0x82cfd0 op07/5, started by 0x826ff0)

Flags tested (001BA1C0): 0x1a, 0x1b, 0x1c, 0x42

Pickups: C64 0x15: s0 g[1], s0 g[9]; C64 0x1e: s0 g[8]; C64 0x20: s0 g[7]; C64 0x22: s0 g[3]; C64 0x27: s0 g[5]; C64 0x38: s0 g[10]; C64 0x4: s0 g[2]; C64 0xc: s0 g[0]; CB8 0x9: s0 g[15]; CC3 0x26: s0 g[12]; CC3 0x27: s0 g[13]; CC3 0x3d: s0 g[14]; CC3 0x51: s0 g[6]; CC3 0x52: s0 g[4]

Items by code: CC3 0x9 += 0x1 (0x823830, owner s0[3]); CC3 0x4f += 0x1 (0x823830, owner s0[3]); C64 0x1a += 0x1 (0x823a10, owner s0[4])

#### AREA14 (overlay id 11, subs 0, lock byte D_0081084F, door table 0x275600)

Other area-change sites: 0x825c40 (owner s0[6]) -> 001B0C60(area 0x11, s=0x0, e=0x0)

Flags / counters written by code: flag 0x2c = 0xff (0x823760, owner s0[5]); counter 0x2c = 0xff (0x823760, owner s0[5]); counter 0x3d = 0xff (0x8238e0, owner s0[0]); counter 0x3e = 0x1 (0x823b30, owner s0[2]); counter 0x3f = 0xff (0x823e30, owner s0[4])

Flags / counters written by scripts: flag 0x0 = 0xff (script 0x826800 op07/5, started by 0x823760); flag 0x3d = 0xff (script 0x826c10 op07/5, started by 0x8238e0); flag 0x3e = 0xff (script 0x826fc0 op07/5, started by 0x823b30); flag 0x3f = 0xff (script 0x827590 op07/5, started by 0x823e30)

Flags tested (001BA1C0): 0x2c, 0x3d, 0x3f

Pickups: C64 0x13: s0 g[0]; C64 0x15: s0 g[3], s0 g[7]; C64 0x16: s0 g[9]; C64 0x19: s0 g[1]; C64 0x1e: s0 g[4], s0 g[6]; C64 0x21: s0 g[2], s0 g[8]; C64 0x2e: s0 g[10]; CC3 0x2c: s0 g[5]; CC3 0x2d: s0 g[12]; CC3 0x46: s0 g[11]

Items by code: CC3 0x16 += 0x1 (0x8238e0, owner s0[0]); C64 0xf += 0x1 (0x823b30, owner s0[2]); CC3 0x17 += 0x1 (0x823b30, owner s0[2]); CC3 0x18 += 0x1 (0x823e30, owner s0[4])

#### AREA15 (overlay id 12, subs 0, 1, lock byte D_00810850, door table 0x24e0b8)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [13] | 001BB860 (slider) | 0x08 | 0x01 | room move e2 / e1 | none |
| 0 | [14] | 001BC350 (hinged) | 0x03 | 0x80 | AREA19 e4 s=1 | none |
| 1 | [9] | 001BC350 (hinged) | 0x03 | 0x82 | AREA19 e5 s=1 | none |

Other area-change sites: 0x823c80 (owner s1[4], s1[6]) stores B5 = 0xf, B6 = 0x0, B7 = 0x0, B8 = 0x1

Lock-bit writers: s0[26] panel 00159210 (00157860 arg 0) model 0x2c bit 0; 0x824b40 (owner s0[5], s0[6]) stores D_00810847

Flags / counters written by code: counter 0x22 = 0x1 (0x8236b0, owner s0[0]); flag 0x23 = 0x1 (0x8239f0, owner s1[4], s1[6]); counter 0x23 = 0x1 (0x8239f0, owner s1[4], s1[6]); counter 0x23 = 0x2 (0x823b40, owner s1[4], s1[6]); counter 0x23 = 0x2 (0x823b40, owner s1[4], s1[6]); counter 0x23 = 0x2 (0x823b40, owner s1[4], s1[6]); counter 0x23 = 0xff (0x823c80, owner s1[4], s1[6]); counter 0x24 = 0x1 (0x824240, owner s0[1], s0[2]); counter 0x24 = 0x2 (0x824350, owner s0[1], s0[2]); counter 0x27 = 0x1 (0x824b40, owner s0[5], s0[6]); counter 0x2c = 0x1 (0x824e50, owner s0[8], s0[9])

Flags / counters written by scripts: flag 0x22 = 0xff (script 0x826e70 op07/5, started by 0x8236b0); counter 0x23 = 0x3 (script 0x8277c0 op06/3, started by 0x823c80); flag 0x23 = 0xff (script 0x8277c0 op07/5, started by 0x823c80); flag 0x24 = 0xff (script 0x827d70 op07/5, started by 0x824240); flag 0x27 = 0x1 (script 0x828ca0 op06/0, started by 0x824b40); flag 0x27 = 0xff (script 0x828ca0 op07/5, started by 0x824b40); flag 0x2c = 0x1 (script 0x829330 op06/0, started by 0x824e50); flag 0x2c = 0x1 (script 0x829330 op06/0, started by 0x824e50)

Flags tested (001BA1C0): 0x22, 0x23, 0x25, 0x26, 0x28, 0x29, 0x2a, 0x2c, 0x30

Pickups: C64 0x10: s1 g[0]; C64 0x11: s1 g[7]; C64 0x15: s1 g[5]; C64 0x16: s1 g[3], s1 g[4]; C64 0x1e: s0 g[2]; C64 0x20: s0 g[4]; C64 0x21: s1 g[1]; C64 0x22: s1 g[2]; C64 0x35: s1 g[6]; CC3 0x2a: s0 g[3]; CC3 0x55: s0 g[1]; CC3 0x56: s0 g[0]; CC3 0x57: s0 g[5]

Items by code: CC3 0xd += 0x1 (0x8236b0, owner s0[0]); C64 0x6 += 0x1 (0x823c80, owner s1[4], s1[6]); CC3 0xe += 0x1 (0x823c80, owner s1[4], s1[6]); C64 0x25 += 0x1 (0x824240, owner s0[1], s0[2]); CC3 0xf += 0x1 (0x824350, owner s0[1], s0[2]); CC3 0x1c += 0x1 (0x824b40, owner s0[5], s0[6]); CC3 0x15 += 0x1 (0x824e50, owner s0[8], s0[9]); C64 0x2b -= 0x1 (001C47E0, 0x824b40, owner s0[5], s0[6])

Sub-state writes: D_00810730[0] = 0x82 (0x823c80, owner s1[4], s1[6]); D_00810730[1] = 0x81 (0x823c80, owner s1[4], s1[6]); D_00810730[2] = 0x82 (0x823c80, owner s1[4], s1[6]); D_00810730[4] = 0x81 (0x823c80, owner s1[4], s1[6]); D_00810730[6] = 0x81 (0x823c80, owner s1[4], s1[6])

#### AREA16 (overlay id 13, subs 0, 1, lock byte D_00810851, door table 0x24e0d0)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [1] | 001BB860 (slider) | 0x08 | 0x80 | AREA16 e0 s=1 | none |
| 0 | [3] | 001BB860 (slider) | 0x16 | 0x01 | room move e2 / e3 | D_00810851 bit 1 |
| 0 | [7] | 001BB860 (slider) | 0x3d | 0x82 | AREA06 e3 s=D_00810730[6] | none |
| 1 | [0] | 001BB860 (slider) | 0x08 | 0x83 | AREA16 e1 s=0 | none |
| 1 | [1] | 001BB860 (slider) | 0x08 | 0x04 | room move e1 / e2 | none |
| 1 | [2] | 001BB860 (slider) | 0x08 | 0x05 | room move e4 / e3 | none |
| 1 | [3] | 001BC350 (hinged) | 0x03 | 0x06 | room move e6 / e5 | none |

Lock-bit writers: s0[2] panel 00158EC0 (00157860 arg 1) model 0x23 bit 1; s1[6] panel 00159210 (00157860 arg 0) model 0x2c bit 0

Flags / counters written by code: counter 0x31 = 0x1 (0x824540, owner s1[39]); counter 0x31 = 0xff (0x824690, owner s1[11]); counter 0x31 = ? (0x824690, owner s1[11]); flag 0x31 = ? (0x824690, owner s1[11]); counter 0x31 = 0xff (0x824690, owner s1[11]); flag 0x31 = 0xff (0x824690, owner s1[11]); counter 0x33 = ? (0x825800, owner s0[10]); counter 0x33 = 0xff (0x825800, owner s0[10]); flag 0x33 = 0xff (0x825800, owner s0[10]); counter 0x33 = ? (0x8261a0, owner s0[11]); counter 0x33 = ? (0x8263d0, owner s0[13]); counter 0x33 = ? (0x8263d0, owner s0[13])

Flags / counters written by scripts: counter 0x31 = 0x2 (script 0x829970 op06/3, started by 0x824690)

Flags tested (001BA1C0): 0x31

Pickups: C64 0x10: s1 g[1]; C64 0x11: s0 g[3], s1 g[8]; C64 0x15: s1 g[10]; C64 0x16: s1 g[2]; C64 0x1e: s1 g[0], s1 g[3]; C64 0x1f: s0 g[2], s1 g[11]; C64 0x20: s1 g[7]; C64 0x21: s1 g[9]; C64 0xe: s0 g[0]; CB8 0x3: s1 g[13]; CB8 0x7: s1 g[12]; CC3 0x67: s0 g[1]; CC3 0x68: s1 g[5]; CC3 0x69: s1 g[6]; CC3 0x6a: s1 g[4]

Items by code: CC3 0x1d += 0x1 (0x824540, owner s1[39]); CC3 0x1e += 0x1 (0x824690, owner s1[11])

#### AREA17 (overlay id 14, subs 0, lock byte D_00810852, door table 0x275604)

Other area-change sites: 0x825470 (owner s0[38]) -> 001B0C60(area 0xb, s=0x0, e=0x3)

Flags / counters written by code: counter 0x2d = 0x3 (0x823ba0, owner s0[30]); counter 0x2d = 0x4 (0x823ba0, owner s0[30]); flag 0x2e = 0x1 (0x8245a0, owner s0[33]); counter 0x2e = 0xff (0x824700, owner s0[34]); counter 0x2f = 0x1 (0x824700, owner s0[34]); flag 0x2f = 0x1 (0x824700, owner s0[34]); counter 0x2f = 0xff (0x824700, owner s0[34]); counter 0x2f = ? (0x825ea0, owner s0[36])

Flags / counters written by scripts: flag 0x2d = 0x1 (script 0x826a20 op06/0, started by 0x823ba0); counter 0x2d = 0x1 (script 0x826a20 op06/3, started by 0x823ba0); counter 0x2d = 0x2 (script 0x826a20 op06/3, started by 0x823ba0); flag 0x2d = 0x1 (script 0x826a20 op06/0, started by 0x823ba0); counter 0x2d = 0x9 (script 0x8270a0 op06/3, started by 0x823ba0); flag 0x2d = 0xff (script 0x8270a0 op07/5, started by 0x823ba0); counter 0x2e = 0x1 (script 0x8275e0 op06/3, started by 0x824700); flag 0x2e = 0xff (script 0x8275e0 op07/5, started by 0x824700); counter 0x2f = 0x3 (script 0x828050 op06/3, started by 0x824700); flag 0x2f = 0xff (script 0x828050 op07/5, started by 0x824700); flag 0x30 = 0x1 (script 0x8284f0 op06/0, started by 0x825470); flag 0x30 = 0x1 (script 0x8284f0 op06/0, started by 0x825470)

Flags tested (001BA1C0): 0x2d, 0x2e, 0x2f

Pickups: C64 0x1e: s0 g[5]; C64 0x1f: s0 g[0]; C64 0x3a: s0 g[1]; C64 0x3b: s0 g[2]; CC3 0x2e: s0 g[3]

Items by code: C64 0x2b += 0x1 (0x824700, owner s0[34]); CC3 0x19 += 0x1 (0x824700, owner s0[34]); C64 0x28 -= 0x1 (001C47E0, 0x824700, owner s0[34])

#### AREA18 (overlay id 15, subs 0, lock byte D_00810853, door table 0x275608)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [18] | 001BB860 (slider) | 0x08 | 0x00 | room move e2 / e1 | none |

Lock-bit writers: s0[21] panel 00159210 (00157860 arg 0) model 0x2c bit 0

Pickups: C64 0x11: s0 g[4]; C64 0x14: s0 g[3]; C64 0x1e: s0 g[2]; C64 0x28: s0 g[1]; CC3 0x5a: s0 g[0]

#### AREA19 (overlay id 16, subs 0, 1, lock byte D_00810854, door table 0x24e0f0)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [22] | 001BB860 (slider) | 0x16 | 0x00 | room move e6 / e5 | D_00810854 bit 0 |
| 0 | [25] | 001BB860 (slider) | 0x16 | 0x81 | AREA03 e1 s=D_00810730[3] | D_00810854 bit 1 |
| 0 | [27] | 0x823580 (overlay) | 0x15 | 0x02 | room move e1 / e2 | see text |
| 0 | [31] | 001BC350 (hinged) | 0x03 | 0x04 | room move e3 / e4 | none |
| 0 | [32] | 001BC350 (hinged) | 0x03 | 0x05 | room move e8 / e7 | none |
| 1 | [49] | 001BC350 (hinged) | 0x15 | 0x03 | room move e2 / e3 | D_00810854 bit 3 |
| 1 | [50] | 001BC350 (hinged) | 0x03 | 0x86 | AREA15 e0 s=0 | none |
| 1 | [51] | 001BC350 (hinged) | 0x03 | 0x87 | AREA15 e0 s=1 | none |
| 1 | [52] | 001BC350 (hinged) | 0x03 | 0x08 | room move e0 / e1 | none |

Lock-bit writers: s0[21] reader 00158810 (001576E0) model 0x13 bit 0; s0[24] panel 00158EC0 (00157860 arg 1) model 0x22 bit 1; s1[48] seal 001581A0 model 0x0e bit 3; 0x825930 (owner s0[7]) stores D_00810854

Flags / counters written by code: counter 0x1d = ? (0x825240, owner s0[6]); counter 0x1d = ? (0x825240, owner s0[6]); counter 0x1d = 0xff (0x825420, owner s0[6]); counter 0x1e = ? (0x825930, owner s0[7]); counter 0x1e = 0x1 (0x825ab0, owner s0[7]); flag 0x1f = 0xff (0x826100, owner s0[18]); flag 0x20 = 0xff (0x826570, owner s1[38]); counter 0x20 = 0xff (0x826570, owner s1[38]); counter 0x21 = ? (0x826840, owner s1[37]); counter 0x21 = ? (0x826840, owner s1[37]); counter 0x21 = ? (0x826c10, owner s1[36]); flag 0x21 = 0xff (0x826c10, owner s1[36]); counter 0x45 = 0x80 (0x827790, owner s0[10]); counter 0x46 = 0xff (0x8279e0, owner s1[34]); counter 0x46 = 0x1 (0x827b10, owner s1[34]); counter 0x60 = ? (0x829a70, owner s1[46])

Flags / counters written by scripts: flag 0x1d = 0x1 (script 0x82b6e0 op06/0, started by 0x825240); flag 0x1d = 0xff (script 0x82ba00 op07/5, started by 0x825420); flag 0x1e = 0x1 (script 0x82bd90 op06/0, started by 0x825930); counter 0x1e = 0x3 (script 0x82bd90 op06/3, started by 0x825930); counter 0x1e = 0x4 (script 0x82bd90 op06/3, started by 0x825930); counter 0x1e = 0x3 (script 0x82bd90 op06/3, started by 0x825930); counter 0x1e = 0xff (script 0x82bd90 op06/3, started by 0x825930); flag 0x1e = 0xff (script 0x82bd90 op07/5, started by 0x825930); counter 0x20 = 0x1 (script 0x82ca20 op06/3, started by 0x826570); flag 0x20 = 0xff (script 0x82ca20 op07/5, started by 0x826570); flag 0x26 = 0x1 (script 0x82d590 op06/0, started by 0x827550); flag 0x26 = 0xff (script 0x82d590 op07/5, started by 0x827550); flag 0x45 = 0xff (script 0x82dd10 op07/5, started by 0x827790); flag 0x46 = 0x1 (script 0x82e090 op06/0, started by 0x8279e0); flag 0x46 = 0xff (script 0x82e090 op07/5, started by 0x8279e0)

Flags tested (001BA1C0): 0x1d, 0x1e, 0x20, 0x23, 0x25, 0x26, 0x46

Pickups: C64 0x10: s0 g[0]; C64 0x11: s0 g[7]; C64 0x12: s0 g[2]; C64 0x15: s0 g[9], s0 g[12]; C64 0x16: s0 g[5], s1 g[1]; C64 0x1c: s1 g[0]; C64 0x1d: s0 g[4]; C64 0x1e: s0 g[8], s0 g[14], s1 g[3]; C64 0x1f: s0 g[15], s1 g[7]; C64 0x20: s1 g[6]; C64 0x3: s0 g[1]; C64 0x37: s0 g[10]; C64 0xd: s0 g[3]; CB8 0x2: s0 g[16]; CC3 0x28: s0 g[11]; CC3 0x29: s0 g[13]; CC3 0x3e: s1 g[5]; CC3 0x3f: s1 g[2]; CC3 0x53: s0 g[6]; CC3 0x54: s1 g[4]

Items by code: CC3 0xa += 0x1 (0x823580, owner s0[27]); C64 0x24 += 0x1 (0x827550, owner s1[53]); CC3 0xc += 0x1 (0x827790, owner s0[10]); CC3 0xb += 0x1 (0x8279e0, owner s1[34]); C64 0x25 -= ? (001C47E0, 0x829a70, owner s1[46])

#### AREA20 (overlay id 17, subs 0, lock byte D_00810855, door table 0x27560c)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [2] | 001BC350 (hinged) | 0x03 | 0x80 | AREA04 e5 s=D_00810730[4] | none |

Other area-change sites: 0x823be0 (owner s0[32]) -> 001B0C60(area 0x15, s=0x0, e=0x3)

Flags / counters written by code: counter 0x34 = 0x1 (0x823b00, owner s0[32]); counter 0x34 = 0x2 (0x823be0, owner s0[32])

Flags / counters written by scripts: flag 0x34 = 0x1 (script 0x826830 op06/0, started by 0x823b00)

Flags tested (001BA1C0): 0x34

Pickups: C64 0x10: s0 g[7], s0 g[8]; C64 0x15: s0 g[1]; C64 0x16: s0 g[0]; C64 0x1f: s0 g[3], s0 g[4], s0 g[5]; C64 0x34: s0 g[6]; CC3 0x2f: s0 g[9]; CC3 0x47: s0 g[10]; CC3 0x6b: s0 g[2]

#### AREA21 (overlay id 18, subs 0, lock byte D_00810856, door table 0x24e118)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [27] | 0x826e60 (overlay) | 0x15 | 0x00 | room move e1 / e2 | see text |
| 0 | [30] | 0x826bb0 (overlay) | 0x15 | 0x01 | room move e3 / e4 | see text |
| 0 | [33] | 0x826e60 (overlay) | 0x15 | 0x02 | room move e5 / e6 | see text |

Other area-change sites: 0x827040 (owner s0[53]) stores B8 = 0x1

Lock-bit writers: s0[38] panel 00159210 (00157860 arg 0) model 0x2c bit 0; 0x826310 (owner s0[43], s0[44]) stores D_00810856 = 0xff; 0x826310 (owner s0[43], s0[44]) stores D_00810856 = 0x0; 0x826bb0 (owner s0[30]) stores D_00810841 [indexed]; 0x826bb0 (owner s0[30]) stores D_00810841 [indexed]; 0x827250 (owner s0[53]) stores D_00810856 = 0xff

Flags / counters written by code: counter 0x34 = 0xff (0x8261a0, owner s0[42]); counter 0x35 = ? (0x826310, owner s0[43], s0[44]); counter 0x35 = ? (0x826310, owner s0[43], s0[44]); counter 0x36 = 0x11 (0x826840, owner s0[45]); counter 0x36 = 0xff (0x8268f0, owner s0[45]); counter 0x36 = 0x10 (0x8269a0, owner s0[45]); flag 0x37 = ? (0x827040, owner s0[53]); counter 0x37 = 0xff (0x827040, owner s0[53])

Flags / counters written by scripts: flag 0x34 = 0xff (script 0x82b240 op07/5, started by 0x8261a0); flag 0x35 = 0xff (script 0x82b5e0 op07/5, started by 0x826310); counter 0x36 = 0x4 (script 0x82bec0 op06/3, started by 0x826840); counter 0x36 = 0x3 (script 0x82c5c0 op06/3, started by 0x8268f0); flag 0x36 = 0xff (script 0x82c5c0 op07/5, started by 0x8268f0); flag 0x37 = 0xff (script 0x82cbd0 op07/5, started by 0x827040)

Flags tested (001BA1C0): 0x34, 0x35, 0x36

Pickups: C64 0x14: s0 g[3]; C64 0x16: s0 g[0], s0 g[1]; C64 0x1e: s0 g[4]; C64 0x1f: s0 g[8]; C64 0x20: s0 g[6]; CC3 0x30: s0 g[5]; CC3 0x6c: s0 g[2]

#### AREA22 (overlay id 19, subs 0, lock byte D_00810857, door table 0x24e130)

| Subs | Record | Behaviour | Model | Id | Destination | Gate |
|---|---:|---|---:|---:|---|---|
| 0 | [6] | 001BC350 (hinged) | 0x03 | 0x80 | AREA04 e1 s=D_00810730[4] | none |
| 0 | [7] | 001BB860 (slider) | 0x09 | 0x01 | room move e1 / e2 | none |
| 0 | [8] | 001BB860 (slider) | 0x09 | 0x82 | AREA01 e6 s=D_00810730[1] | none |
| 0 | [10] | 001BB860 (slider) | 0x16 | 0x03 | room move e3 / e4 | D_00810857 bit 3 |

Lock-bit writers: s0[9] reader 00158810 (001576E0) model 0x12 bit 3

Pickups: C64 0x11: s0 g[3], s0 g[4], s0 g[5]; C64 0x20: s0 g[1]; C64 0x22: s0 g[0]; CC3 0x4c: s0 g[2]


<!-- world_graph tables end -->

## 7. Derived progression order

The world graph as edges (area A -> B: the site, then its gate). "Open"
means no lock or story test was found on the site itself.

| From | To | Site | Gate (evidence) | Played |
|---|---|---|---|---|
| AREA11 | AREA01 e4 | 0x823C40 [8], 0x827630 [1] / [2] | the first level (measured) | played (first level, beat 15) |
| AREA01 | AREA00 e0 | [12] 0x823580 (sub 0), [9] (sub 1) | open (measured) | played (third level, a01_07) |
| AREA00 | AREA01 e0 | [52] 0x823580 / [15] (sub 2) | open (measured) | played (third level, a00_10) |
| AREA01 | AREA02 e0 | [14] / [11] model 0x15 | D_00810842 bit 1, set by AREA02's seal [21] 001582E0 (from the AREA02 side) | not played (the locked door measured, a01r_s1) |
| AREA01 | AREA02 e1 sub 1 | [16] / [12] | open (measured, fourth level) | played (fourth level, a01r_03) |
| AREA02 | AREA01 e3 / e5 | [22] model 0x15 (bit 0, the same seal) / sub 1 [5] | seal / open | not played |
| AREA02 | AREA04 e0 | [25] model 0x17 | D_00810843 bit 3: the panel [24] 00158EC0 (item 0x1B) (measured, fourth level) | played (fourth level, a02_05) |
| AREA04 | AREA02 e4 | [35] | open | not played |
| AREA04 | AREA22 e0 | [37] | open (measured) | played (fifth level, a04_05) |
| AREA22 | AREA04 e1 / AREA01 e6 | [6] / [8] | open (measured) | both played (a22b_01, a22_s3; a22_02) |
| AREA01 | AREA22 e5 / AREA06 e0 | [18] / [19] | open (measured) | both played (a01v_02, a01u_s1; a01u_02) |
| AREA06 | AREA01 e7 | [1] | open (measured) | played (a06_s0, a06b_01) |
| AREA04 | room behind [45] (NPC [2], lift [56]) | [45] 0x823700 (sub 0), [51] (sub 1) | D_00810845 bit 5: only the AREA06 keypad page (00207350 slot 0) sets it | played ([45], a04b_00) |
| AREA04 | AREA13 e0 | lift [51] model 0x0B | its buttons [53] / [54] (001BC960, id 7; BM, also read from the original instructions, section 3): D_00810845 bit 7 (the reader [50] model 0x2F: item 0x23); the inner [54] then needs flag 0x12 = 0xFF, or counter 0x12 = 0x10 (set by [3]'s first stage 0x824830 at the end of script 0x828450, which the reader's flag 0x12 = 1 starts) with D_008106C0 null or its record's conditions; first ride: flag 0x14 1 -> [4]'s script -> 0xFF (measured, a04b_04: counter 0x12 = 0x10, flag 0x12 = 1, D_008106C0 = 0) | played (a04b_04) |
| AREA04 | AREA07 e0 | lift [56] model 0x0D (behind [45]) | its buttons [58] / [59] (001BDFC0, id 0): D_00810845 bit 0 = the socket [55] 00159E70 with item 0x29 | not played (inferred) |
| AREA07 | AREA04 e10 | lift [0] | its buttons (id 0xFF, inferred never by bit) | not played (inferred) |
| AREA04 | AREA03 e0 sub 1 | [38] model 0x15 (sub 0) / [44] model 0x03 (sub 1) | sub 0: D_00810845 bit 2, set only by AREA03's seal [23] 00158430 (from the AREA03 side); sub 1: open | not played (inferred) |
| AREA04 | AREA20 e0 | [42] / [48] model 0x15 | D_00810845 bit 4: AREA06 sub 1's 0x825E20 (flag 0x31) | not played (inferred) |
| AREA13 | AREA04 e7 | lift [10] model 0x0B | its buttons [12] / [13] (id 0xFF: 001BC960 tests no lock bit; the inner one plays script 0x24E1A0 at the AREA13 entry 0 arrival; BM, also ins). Measured (a13b_04 / a13b_05): the outer [12] facing -x at (652.7, 1238.6) took Use with the lift's +0x0B = 0, then +0x0B 0 -> 2 -> 3; the inner [13] facing +x (the player at (639.7, 160, 1276.4) in the frame the Use was taken), the request 04 FF 07 01, AREA04 entry 7 | played (a13b_05) |
| AREA13 | AREA19 e9 / e10 sub 0 | 00193EB0 (the player's event code 6..9 / 0x2C / 0x2D at y <= 159 with spawn entry 4 / 6, resp. 5 / 7) | door [17] (0x823580, ins) from the east (entry 4) and the hatch [62] (0x826850, ins): item 0x27 (pickup g[5] in the same room) makes the hatch usable (class 0x84), its Use opens it (counter 0x61 bit 0), Use facing +z starts its ladder, and the descent reaches y 143.1 (measured, a13_05). [15] (001BD9F0 model 3) and [17] are locked only while flag 0x1C == 1 (NM / ins); flag 0x1C stayed 0 on this route. The holes [5] / [6] (0x823BC0, ins) are examine points that end once item 0x27 is held or flag 0x1B is 0xFF. Door [20]'s north side (entry 10), the only way to hole [6] / hatch [63], was reached neither on foot from the field (section 8c) nor after [44]'s event, the boom and [7] (section 8d); it is reached from the region south of the pipe fence over the big building's south roof (a stair, ladders of attribute 0x32 and one running jump; section 8e), then door [20] (001BC350, BM, no lock) to entry 5 and the hatch [63] (counter 0x61 bit 1, item 0x27); its ladder gives the request 13 00 0A 01 (measured, a19b_00) | e9 played (a13_05, hole [5]); e10 played (a19b_00, hatch [63]) |
| AREA19 | AREA13 e6 / e7, AREA19 sub 1 e6 / 7 / 8 | 00196970 (climbing above a height in five circles) | the circles are the ladders at AREA19 entries 9 (-> AREA13 e6), 10 (-> e7), 11, 12 and 13 (-> sub 1 e8 / e7 / e6) (BM). Measured (a13b_00): Use facing +z at entry 9's foot, the stick up, the request 0D 00 06 01 at y >= 284.5; (a19c_06) Use facing -z at the foot of the ladder at z 859.5 (the y-210 floor behind door [27]), the stick up, the request 13 01 07 01 at y 314 | e6 played (a13b_00); sub 1 e7 played (a19c_06); the rest not played |
| AREA19 | AREA03 e1 | [25] model 0x16 | D_00810854 bit 1: the panel [24] 00158EC0 model 0x22 (item 0x1B). The panel stands at the north end of the y-265 walkway behind entry 0, which the entry-9 rooms do not reach (section 8c) | not played (inferred) |
| AREA03 | AREA19 e0 | sub 0 [15] | open | not played (inferred) |
| AREA03 | AREA04 e2 | sub 1 [24] model 0x15 | D_00810844 bit 0: the seal [23] 00158430 (a hit; it also opens AREA04's [38]) | not played (inferred) |
| AREA03 | AREA08 e0 | sub 2 lift [7] model 0x0D | its buttons [9] / [10] (001BDFC0, id 7): D_00810844 bit 7 = the socket s2[6] with item 0x2A, given by AREA03 sub 1 [22] 0x8235A0 | not played (inferred) |
| AREA08 | AREA03 e1 sub 2 | lift [8] | as AREA07 [0] | not played (inferred) |
| AREA19 sub 1 | AREA15 e0 sub 0 / sub 1 | [50] / [51] | open; sub 1 is reached by the ladders at entries 11..13 (00196970), not from the entry-9 rooms (section 8c). The two doors open from the landings (y 450 / 500) of a stair tower (x 897..990, z 942..985) whose door from sub 1's hall, [49] (model 0x15, lock bit 3), carries the seal [48] (001581A0, facing the tower's inside); hits from the hall side did not take (section 8f). The tower's ground floor holds the top of the ladder from sub 0's y-265 deck (sub 1 entry 8; sub 1 grid nodes 145 / 146) (read) | not played (inferred) |
| AREA15 | AREA19 e4 / e5 sub 1 | sub 0 [14] / sub 1 [9] | open | not played (inferred) |
| AREA15 sub 1 | AREA15 sub 0 e0, and the world's subs | 0x823C80 | flag 0x22 set (AREA15 sub 0 [0]'s script 0x826E70) and 0x23 clear; counter 0x23's steps (ins) | not played (inferred) |
| AREA00 sub 2 | AREA14 e0 | [28] 0x826790 | Use with item 0x26 (AREA00 sub 2 g[3]) | not played (inferred) |
| AREA14 / AREA18 | AREA18 e0 / AREA14 e1 | 00190F20 | in AREA14 the player inside quad D_0024A4B0 / in AREA18 the player's x (+0xA0) <= 285 (BM, section 5) | not played (inferred) |
| AREA14 | AREA17 e0 | [6]'s script 0x828120, op09 0x825C40 | the owner's condition not read | not played (inferred) |
| AREA17 | AREA11 e3 | [38] 0x825470 | counter 0x5D (scan) | not played (inferred) |
| AREA06 | AREA16 e0 | [3] model 0x3E | D_00810847 bit 2: AREA15's 0x824B40 only | not played (inferred) |
| AREA16 | AREA06 e3 | [7] model 0x3D | open | not played (inferred) |
| AREA20 | AREA04 e5 / AREA21 e3 | [2] / [32] 0x823BE0 | open / counter 0x34 and a quad | not played (inferred) |
| AREA21 | the ending | [53] 0x827040 | counter 0x37 (flag 0x36's event) | not played (inferred) |

**The order the code implies** (each step: what gates it, and where that
gate is set). The recorded levels one to seven play AREA11 -> AREA01 ->
AREA00 -> AREA01 -> AREA02 -> AREA04 -> AREA22 -> AREA01 upper floor ->
AREA06 (the port's `docs/FIRST_LEVEL_ROUTE.md` .. `SEVENTH_LEVEL_ROUTE.md`).
From there:

1. **AREA06 keypad before anything else in AREA04.** The only open doors
   in AREA04's sub 0 lead back to AREA02 and AREA22; [45] needs bit 5,
   which only the AREA06 keypad sets; the lifts need item 0x23 (the NPC
   behind [45]) or item 0x29 (sub 1 only, below); [38] needs AREA03's
   seal; [42] needs AREA06 sub 1. So the keypad (AREA06 sub 0; sub 1 has no
   keypad [6] and no beam [11]) is the next gate. (inferred from the table)
2. **Back east over the beam without its collapse.** After the keypad
   the beam [11] (C) starts its collapse script when the player walks onto
   its west end (+5 below 2 or 29..34), and the pit it drops the player
   into has only door [3], which needs D_00810847 bit 2 from AREA15
   (below): with that gate after AREA13 / AREA19 / AREA15, whose way in
   needs item 0x23, which needs the keypad's bit, the collapse after the
   keypad is a dead end in this order. A running jump (+5 = 6) is not in
   the beam's test. (inferred; measured in the eighth level: the jump
   works and nothing else changes)
3. **AREA04: the NPC [2], the reader [50], the event [3], the lift [51]
   to AREA13.** Item 0x23 comes only from the NPC (C: 0x824490); the
   reader model 0x2F uses it for D_00810845 bit 7 and flag 0x12 = 1. [3]
   (C: 0x8246B0) runs while flag 0x12 is non-zero: its first stage
   0x824830 plays script 0x828450, then spawns groups 0x826790 (a
   creature) and 0x8267F0 and sets counter 0x12 = 0x10; its second stage
   0x824930 runs only for counter 0x12 = 1, 2, 0x20 or 0xFF and ends with
   flag 0x12 = 0xFF. The inner call button of lift [51] needs bit 7 and
   either flag 0x12 = 0xFF or counter 0x12 = 0x10 (with D_008106C0 null,
   or that record's +4 >= 2 or +0xB0 < 554.0; section 3), so the lift is
   open once the first stage ends: the second stage is not on the way.
   (C / BM / ins; measured in the eighth level, a04b_04)
4. **AREA13, then AREA19.** AREA13's only exits are the lift back and the
   falls into AREA19 (00193EB0). AREA13's flag 0x1A event ([4] 0x823A40,
   item 0x1A) is also the flag that removes the AREA04 NPC (0x824320
   frees itself when flag 0x1A is set, C). (scan + C; played in the ninth
   level, section 8: the lobby, door [8] ([3]'s scene), door [14] outside,
   door [17] from the east ([4]'s scene, item 0x1A), item 0x27, the hatch
   [62] and its ladder into AREA19 entry 9)
5. **AREA19 -> AREA03 -> AREA08, or AREA19 sub 1 -> AREA15.** Both are
   open from AREA19 (the panel [24] needs one battery, item 0x1B). AREA03's
   seal [23] opens the way back to AREA04 ([24], [38]); AREA03 sub 1's
   [22] gives item 0x2A for the lift to AREA08. AREA07 and AREA08 share one
   layout and both set flag 0x25, which AREA19 sub 1's [53] 0x827550 (item
   0x24) and AREA15 test. (inferred) **Revised by the tenth level
   (section 8c):** AREA19 is not one open space. Entry 9 (the hatch [62]
   route, played) reaches only its own rooms (played as far as the room
   behind door [32]; a collision scan, a lead, links them to neither entry
   0 nor entry 10), whose one other door [22] needs item 0x24 at the
   reader [21] (code); the panel [24], door [27] and the sub-1 ladders lie
   in the parts behind entries 0, 1 / 2 and 10. Door [27] opens only with
   D_00810854 bit 2, which [7]'s 0x825930 sets after the sequence that
   [7]'s 0x825AB0 starts only when the spawn entry byte D_00810702 is 0xA
   (code). 00193EB0 (NM) sends the player to AREA19 entry 0xA from AREA13
   entry 5 / 7 (AREA13's hatch [63], the other fall) (code). **Inferred,
   not read:** that this arrival is the only way D_00810702 becomes 0xA.
   D_00810702 also changes inside an area (001AD010's room-move path
   stores D_008106B7 into it when D_008106B8 == 2; measured 9 -> 8 in the
   duct, a19_00 f1453); AREA19's door table has no room move to entry 10
   (its room moves give entries 0..8), but the other in-area triggers that
   can set D_008106B7 in AREA19 were not enumerated. On that inference,
   step 5 hangs on reaching AREA13's hatch [63] (door [20]'s side of
   AREA13), which the tenth level did not find a way to on foot.
   **Played further in the eleventh level (section 8d):** AREA13's [44]
   (0x823E90, C) is a machine that takes a battery (00184D20, BM: class 4
   model 0x25 accepts items 0x1B..0x1D); its event (flag 0x1C 1 -> 0xFF,
   the step controller [45] 0x827150, C) opens the roof's north part (the
   [49]..[54] owners end) and, through [47]'s cell-directory toggles
   (0019C6F0, NM), the fallen boom from [44]'s platform south-west over the
   pipe fence; the boom passes [7]'s area (0x824A80, C: counter / flag 0x42,
   the group 0x82A230) and ends in the region south of the fence. Door
   [20]'s side was still not reached (inferred lead: a strip at y 186..211
   east of the grid wall at x 831, above door [20]'s region; port
   ELEVENTH_LEVEL_ROUTE.md section 6).
   **Played further in the twelfth level (section 8e):** door [20]'s north
   side is reached from the region south of the fence over the big
   building's south roof, then the hatch [63] and its fall to AREA19 entry
   0xA; [7]'s first sequence (0x825AB0) ran (counter 0x1E = 1), and [6]'s
   first stage (flag 0x1D, counter 0x1D, the 0x82A590 creature). The
   inference above (that this arrival is how D_00810702 becomes 0xA) is
   consistent with the capture but still not proven exclusive. Door [27]
   still waits for [7]'s Use (0x825930), not reached.
   **Played further in the thirteenth level (section 8f):** [7]'s room is
   reached by two bars (attribute 0x34): the one over the y-172 floor, whose
   swing at its end releases the player onto the y-195 platform, and the
   one from that platform into the room. The room is [6]'s second-stage
   area (flag / counter 0x1D = 0xFF); [7]'s Use sets D_00810854 bit 2 (flag
   0x1E 0xFF; [9] then turns off the water surface behind door [27]); door
   [27] leads west to the room whose ladder at z 859.5 is 00196970's circle
   for sub 1 entry 7. So step 5's second branch (sub 1 -> AREA15) is open
   from here as far as sub 1's hall; its doors to AREA15 lie in a stair
   tower entered from sub 0's deck (section 8f, read).
6. **AREA15 twice.** Sub 0's [0] sets flag 0x22; sub 1's [4] / [6]
   (flag 0x22 set, 0x23 clear) run counter 0x23 up to 0x823C80, which
   gives C64 0x06 and CC3 0x0E, rewrites the subs of AREA00 (2), AREA01
   (1), AREA02 (2), AREA04 (1) and AREA06 (1) and sends the player to
   AREA15 sub 0. (ins)
7. **The second half.** AREA00 sub 2 -> AREA14 (item 0x26 from AREA00 sub
   2's g[3]) -> AREA18 (item 0x28, g[1]) -> AREA17 (0x824700 takes 0x28 and
   gives 0x2B) -> AREA11 entry 3 (flag 0x30 1, then AREA11's [11] 0x823CE0
   script sets it to 0xFF) -> ... -> AREA15's room (0x824990 exists only
   with flag 0x30 = 0xFF and until flag 0x2A; at entry 1 its 0x824B40
   script takes item 0x2B and sets D_00810847 bit 2) -> AREA06 sub 1 door
   [3] -> AREA16 (flag 0x31) -> AREA06 sub 1's 0x825E20 (flag 0x31: flag
   0x2A, D_00810845 bit 4) -> AREA04 sub 1 [48] -> AREA20 -> AREA21 -> the
   ending. AREA04 sub 1's [1] 0x825040 gives item 0x29 (the socket [55] and
   the lift [56] to AREA07). (ins / C / scan; the order within this part is
   inferred from which gate each step opens)

The story-flag numbers follow the same order (0x10 the beam, 0x11 / 0x12
the NPC and the reader, 0x14 the lift, 0x15 AREA07, 0x17 / 0x18 AREA08,
0x19..0x1C AREA13, 0x1D..0x21 AREA19, 0x22..0x27 AREA15, 0x2D..0x30
AREA17, 0x31 / 0x33 AREA16, 0x34..0x37 AREA20 / AREA21). That is a
pattern, not evidence, and is not used for any edge.

## 8. Measured (eighth level)

The port's `docs/EIGHTH_LEVEL_ROUTE.md` played section 7's steps 2 and 3
from the seventh level's a06_03 (groups `a06b`, `a01v`, `a22b`, `a04b` of
`tools/route_capture.py`). Measured:

- The running jump from AREA06's west pad (z -592) over the beam's quad
  lands on its east part with D_00810768 still 0 (a06b_00); door [1] is
  open with bit 5 set (a06b_01). Step 2 holds.
- AREA01's gap node g[36] took its bit-5 branch at the load (D_00810766 =
  0xFF, behaviour 001BF6B0, group 0x829110); the gap is crossed north by a
  running jump; doors [18], AREA22's [7] and [6] are open (a01v, a22b).
- AREA04's door [45] opens with bit 5 (a04b_00); the NPC [2]'s script ends
  with D_00810C87 = 1 and counter 0x11 = 1 (a04b_01); the reader [50] sets
  D_00810845 bit 7 and flag 0x12 = 1, and [3]'s first stage runs (script
  0x828450, counter 0x12 = 0x10, the creature and bugs of groups 0x826790 /
  0x8267F0) (a04b_03). Step 3 holds up to the lift.
- The lift [51]'s doors open after the reader (the outer button [53]
  to state 1 sub-state 5, a04b_03). With counter 0x12 = 0x10, flag 0x12 =
  1 and D_008106C0 = 0, the inner button [54] took Use with the player
  facing -x (a04b_04): its script, then the lift's +0x0B 3 -> 4, flag
  0x14 0 -> 1, [4]'s script 0x828BE0, flag 0x14 = 0xFF, the request
  0D FF 00 01 and AREA13 entry 0. [3]'s second stage (0x824930) did not
  run and the creature was left alive. Step 3 holds up to AREA13.

## 8b. Measured (ninth level)

The port's `docs/NINTH_LEVEL_ROUTE.md` played section 7's step 4 from the
AREA13 arrival (group `a13` of `tools/route_capture.py`, six beats). Measured:

- The lift car is left through its opening at z about 1258 (a13_00); door
  [8] from the north: entry 2; [3] (0x823700) starts its scene at once
  (flag 0x19 1 -> 0xFF, counter 0x19 = 1, a document page).
- The button [16] (001BD9F0 model 4) takes Use facing -z: entry 8, outside
  (a13_01). Outside, the idle player plays cold clips and the health falls
  by 1 (45 -> 44 in a13_02, the only loss on the route).
- Door [17] from the east (entry 4) with flag 0x1C = 0; [4] (0x823A40)
  starts its scene at the arrival: item 0x1A (D_00810C7E = 1, an equipment
  item), flag and counter 0x1A = 0xFF (a13_02).
- The pickup g[5] gives item 0x27 (a13_03); both holes [5] / [6] end and
  both hatches [62] / [63] become class 0x84.
- The hatch [62]: its descriptor 0x82CDD0 (720, 160.5, 1253.7, radius 10,
  yaw -3.072) takes Use with the player facing +z; script 0x82CA50, state 2,
  counter 0x61 = 1 (a13_04). Use facing +z in the opening: the ladder
  (action 0x16), y 143.1 at (720, 1259.6), 00193EB0's request 13 00 09 01
  (event code 8, spawn entry 4), **AREA19 sub 0 entry 9** on a ladder at
  (710, 281.4, 1266.1); the stick held down climbs to (710, 240, 1256.8)
  (a13_05). Step 4 holds for hole [5] and entry 9.
- Counter 0x1C was already 1 at the arrival: [44] 0x823E90's stage 0
  (0x824160, C) increments it at the load. [44] (stage 1, Use) was not
  played; flag 0x1C stayed 0, so doors [15] / [17] were never refused.

## 8c. Measured (tenth level)

The port's `docs/TENTH_LEVEL_ROUTE.md` played from the ninth level's end
(a13_05: AREA19 entry 9, the ladder's foot) with groups `a19` and `a13b` of
`tools/route_capture.py`. Measured:

- **The entry-9 rooms.** The ladder room (y 240) is joined to the room
  behind door [32] (y 200, entries 7 / 8) by a duct: its entry square
  (grid attribute 0x37, x 697..705, z 1220..1230) takes Cross facing -z
  (actions 0x2C, 0x2D, a01_s5's duct kind); the crawl runs south to (703,
  220, 1068) and east out of an exit square into the room, and the spawn
  entry byte D_00810702 becomes 8 inside the duct (a19_00). Back the same
  way it becomes 9 again (a19_02). An exploration walk south down the ramp
  corridor east of the duct stopped at (731.7, 242, 1217.4); a collision
  reachability scan of a19_00's capture (a lead) reaches door [32]'s north
  side (entry 7, y 200) from the ladder room that way, so the stop does
  not prove the corridor closed. Door [32] (001BC350 model 0x03, no lock
  test) was never opened. In the room the pickup g[2] (00219550) gave
  D_00810CA8 0 -> 12 with its page (a19_01, which ends at the Use spot
  (744.7, 200, 1057.7)). The reader [21] and door [22] (the platform at y
  220 east of the room) were not played: no walk toward them is kept, and
  the same scan (a lead) reaches that platform with the reader and entry 5
  from the room, so the refusal is probably playable. Their gate is code:
  001576E0 (BM) takes model 0x13's Use only with D_00810C88 (item 0x24)
  non-zero, and D_00810C88 was 0 throughout. The scan links neither the
  ladder room nor the room behind door [32] to entry 0 or entry 10.
- **Back to AREA13.** Use facing +z at the ladder's foot, the stick up:
  00196970's request 0D 00 06 01 (circle 0, y >= 284.5), AREA13 entry 6
  under the open hatch [62]; the player climbs out on his own (actions
  0x43, 0x18) to (720, 160, 1252.3) (a13b_00). In this AREA13 load the
  freed [3] / [4] shift every later pool node down (the a13 owner table
  does not apply).
- **AREA13 -> AREA04 e7.** Door [17] from inside facing +x (entry 9,
  a13b_01), outside to door [14]'s button [15] facing +z (entry 3,
  a13b_02), door [8] facing +z (entry 1, a13b_03), the lift [10]'s outer
  button [12] and inner button [13] (a13b_04, a13b_05): the request 04 FF
  07 01 and AREA04 entry 7 at (570.7, 54.9, 244.6), where the AREA04
  inner button's arrival script runs. Inside the car a bite (action 0x3E)
  raised the infection 80 -> 90 (a13b_05 f365).
- **The cold.** With item 0x1A held, the health stayed 44 through 2,089
  frames outdoors (exploration) and through a13b_01..02; 0015D100 (C)
  returns before its 360-frame drain while D_00810C7E != 0.
- **AREA13's east side not reached.** The hatch [63] room (entry 5) is
  entered only through door [20] from its north side (entry 10, (1064,
  160, 889)). Walks from the outdoor field toward it stopped at a fence
  of 40-unit cell-world walls along the pipes south of the field, at x
  about 797..826 east of door [17] and [44], and at z about 1269 north of
  door [17] (exploration). The roof ladder at the building's south-east
  corner (grid wall attribute 0x32 at x 720..727.4, z 1208.6, y 160..215)
  takes Cross facing +z and climbs to the roof (y 215) (a13b_s0); a walk
  east on the roof stopped at a railing at (773.2, 215, 1226.4). A
  reachability scan of the captured collision (scratch, a lead only) joins
  the east side to the field only downhill (a drop off the slope wall near
  (830, 1160)).
- **Census** (`route_census.py tenth-delta`): the a19 / a13b replays run at
  least 18 functions (8,640 bytes) that no earlier level ran: 12 AREA19
  overlay functions, 1 AREA13 one and 5 boot ones (port
  TENTH_LEVEL_ROUTE.md section 5). It is a lower bound (measured gaps,
  port section 5.1): group a13b arms only AREA13, so a13b_00's AREA19
  frames (f1..f425, the climb and 00196970's request) were not measured
  for AREA19 code, nor was a13_05's AREA19 arrival (the ninth level's pass
  armed AREA13; the init 0x8250C0 runs there); and seven AREA13 functions
  (1,240 bytes) whose addresses AREA19 code spent at a13b_00 f1..f2 could
  not be seen in a13b_00's AREA13 frames.

## 8d. Measured (eleventh level)

The port's `docs/ELEVENTH_LEVEL_ROUTE.md` played group `a13c` of
`tools/route_capture.py` (7 beats) from the tenth level's a13b_01 (outside
door [17], entry 9, infection 80). The a13b trip down to AREA04 entry 7
(a13b_02..05) is a detour on this route: played on from a13b_05
(exploration) a bite at the AREA04 lift raised the infection to 100, and
with it at 100 the health fell 44 -> 9 in about 6,200 frames. Measured:

- **The recharger [60]** (00159210 model 0x2C; 00157860, BM) took Use
  facing +x with the charge D_00810CB2 at 0 (capacity 12): script 0x247420,
  the battery page's request with D_008106B1 bit 6, Yes: 0 -> 12 at 2 per
  20 frames (a13c_00).
- **[44]** (model 0x25, at (798.4, 215, 1149.5), reached from the roof's
  south-east corner over the walkway, area 0x82E140) took Use facing -z at
  (805.3, 214.4, 1153.2): script 0x82B090, then the request 01 84
  (D_008106B0 = 1, D_008106B1 = +0x34 + 0x80 with +0x34 = 4); the battery
  page's confirmation, Yes: the charge 12 -> 4 at 2 per 30 frames, the
  owner's +0xB = 5; script 0x82B2D0 and flag 0x1C (D_00810774) = 1;
  D_008107F4 0x01 -> 0x12 -> 0x52 ([44] step 2, [45] ORs 0x10 and 0x40)
  (a13c_02).
- **The blast** (a13c_03): with the player back on the walkway (inside
  0x82E140, above y 210) at [44]'s 490-frame check, script 0x82B3D0
  (callback 0x8246D0) runs and its scene leaves the player in the field at
  (750, 161.5, 1147.8); D_008107F4 bit 5 (0x20) at [45]'s count 0x1F3
  (a13c_03 f471, 499 counters after 0x52) and [49] ends over f471..f473
  (its state byte 0x64 -> 0x01 -> 0x03, then the header cleared);
  D_008107F4 0x73 / 0x74 ([44] steps 3 / 4);
  D_00810833 = 0xFF, script 0x82C110, flag 0x1C = 0xFF with the player put
  at (696, 159.7, 1102); [44] ends. Health 44 -> 29 by two hits in the
  field (source not read). Exploration: a player left at [44]'s Use point
  died there (health 44 -> 0 in one frame); when, relative to bit 5, is
  unverified (the scratch evidence was deleted; port doc section 6).
- **The collision change** (kept snapshots a13c_02 before, a13c_03 ..
  a13c_06 after; the exploration RAM agrees): [47]'s 0019C6F0 calls flip
  bit 0x40000000 of the cell-list directory's entry words. The scratchpad
  word 0x70003250 holds the directory's address (0x018321C0 here); entry
  n's word is at that address + 4 + 4n, so EE RAM 0x018321C4 (entry 0, key
  0x20: one wall on [44]'s platform) 0xC000015C -> 0x8000015C, enabled, and
  0x018321C8 (entry 1, key 0x1F: two walls x 800..825, z 1085..1168, y
  148..208, at the boom's north-east end) 0x8000025C -> 0xC000025C,
  disabled. Walks north on the
  roof that stopped before the event pass after it.
- **The cure** (a13c_04): the roof's north part, a ramp down to its y-210
  part and a switchback stair north of the lobby lead to the ground north
  of the building; the pickup g[3] (00219550) gives item 0x22 (D_00810C86)
  with its HEALING page (002160B0, BM, kind 4): used, health 29 -> 100 and
  infection 80 -> 0.
- **The boom and [7]** (a13c_05, a13c_06): back up to [44]'s platform; its
  south end drops onto the boom's north-east end (809.5, 208.6, 1130.0);
  down the boom's top (cell 44, y 205.7 -> 179.3) [7]'s area 0x82E220 is
  entered at (703.5, 189.7, 1031.7): counter 0x42 (D_0081081A) = 0xFF,
  script 0x82C510, flag 0x42 (D_0081079A) = 0xFF and 001B6660(0x82A230)
  (the census sees 0x141D20 from that frame); off the boom's south-west
  end the slope leads south (y 181 -> 153) into the region south of the
  pipe fence, at (637.5, 154.5, 890.5).
- **Not reached:** door [20]'s north side (entry 10). Walks east in the
  south region stopped at (739.7, 162.4, 822.8) and (795.5, 158.1, 790.8);
  a reachability scan (a lead) enters door [20]'s region (x 990..1087, z
  836..983) only by drops from a strip at y 186..211 (x 831..1106, z
  836..1038) that it joins to neither the platform nor the south region
  (x 552..820); the grid wall at x 831 (z 980..1095) has the surface
  attribute 0x51 (collides only for query id 0, decomp FINDINGS.md). Past
  its north end a walk from [44]'s platform reached (837.0, 202.8, 1108.4)
  on the plateau east of it, then stopped against a diagonal pair of grid
  walls (about (832, 1100) to (880, 1035)) and a post (exploration).
- **Census** (`route_census.py eleventh-delta`): 43 functions (27,492
  bytes) that no earlier level ran: 25 boot, 18 AREA13 (port
  ELEVENTH_LEVEL_ROUTE.md section 5), among them 0x828500, 0x828C60,
  0x828E10 and 0x828F40, which have no static reference in the overlay.
  None of the 18 has a port translation; the port's AREA13 module
  (`em_level9_port_area13.c` / `em_level9_port_turret.c`, LEVEL9_PORT.md)
  already calls 0x824390, 0x824520, 0x824960 and 0x826610 as untranslated
  hooks and reaches [45]'s step functions (table 0x82D190) through its
  callback.

## 8e. Measured (twelfth level)

The port's `docs/TWELFTH_LEVEL_ROUTE.md` played groups `a13d` (8 beats,
AREA13) and `a19b` (3 beats, AREA19) of `tools/route_capture.py` from the
eleventh level's a13c_06 (the region south of the pipe fence). Measured:

- **The south roof.** The way to door [20]'s north side lies in AREA13's
  grid collision (attribute 0x32 = a ladder for 0015D4C0's Use, NM;
  attribute 0x35 = a stair): a switchback stair south of the big building
  (y 160 -> 179.5 -> 203.5, x 744..787, z 814..835), the ladder at x 798.5
  to the roof corridor (y 240, z 805..835), the first block's west ladder
  (x 888.5) to its top (y 280), a running jump (0015EC50, NM: action 0x0C,
  a13d_03 f215..f262) onto the second block (x 960..990), whose west face
  has the ladder's rails and top railing but no attribute-0x32 node (Cross
  there started no climb, exploration), its east ladder (x 991.4) down,
  and the ladder at z 836.4 down to y 176.5 on door [20]'s side (a13d_00 ..
  a13d_05). One hit on the corridor (health 100 -> 90).
- **Door [20] and the hatch [63].** Door [20] (001BC350, BM, model 0x03, no
  lock test) from its north side facing -z: entry 5 (a13d_06). The hatch
  [63] (0x826850, C; descriptor 0x82CDF0 (1071.5, 160.5, 844.9), radius 10,
  yaw -1.606): Use facing +x with item 0x27 held: script 0x82CA50, counter
  0x61 (D_00810839) 1 -> 3 (a13d_07).
- **AREA19 entry 10.** Use facing +x in the open hatch: its ladder; at y
  143.1 00193EB0 (NM) requests 13 00 0A 01 (spawn entry 5); AREA19 sub 0
  entry 10 on a ladder at (1077.2, 250.1, 845). [7] (0x8257C0, C) with
  D_008107F6 == 0 runs 0x825AB0 (C): its three scripts (3B8D = 2 for 1,249
  frames, f486..f1734) take the player down the ladder to (1086.5, 185,
  845) and end with counter 0x1E (D_008107F6) = 1 at f1735, the frame
  3B8D returns to 0 (a19b_00). D_00810854 stayed 0.
- **[6]'s first stage.** Along the east wall's ledge (attribute 0x39: the
  walk turns into a wall-side shuffle, action 0x2F), a ledge climb onto the
  platform at y 190 and the ladder at z 974 to y 220 (a19b_01, a19b_02):
  the ladder's top is inside [6]'s area 0x82BD00 above y 210, so 0x825240
  (C) runs script 0x82B6E0 (flag 0x1D (D_00810775) = 1) and at its end
  sets D_008107F5 |= 1 | 2 (counter 0x1D = 3) and spawns group 0x82A590
  (one 0012E3A0 creature at (917, 210, 993); D_008106C0 = its node).
- **Not reached:** [7]'s Use. The y-195 platform with the bar to [7]'s
  room (attribute 0x3A under an attribute-0x34 bar, the pole entry) is
  not reached from the y-172 floor: the slope between them (grid nodes
  153 / 158, 45 degrees, downhill toward the floor) carries the authored
  class 0x1000, the class whose floor contact enters the player's slide
  (00175CF0 / 001796C0, port PLAYER_CLIMB_SLIDE.md; in AREA11 exactly the
  slide hill): by that rule a floor contact on it (attribute 0, not 0x39)
  starts a slide along its downhill heading, toward the floor (read, not
  played). The exploration walks toward
  it stopped on the floor at z 972.1..972.4, about 4 units short of its
  foot (z 968), one with a knock-back (health 90 -> 85); what stopped them
  is not read. Leads from AREA19's grid (not
  played): attribute-0x32 ladders at nodes 884..886, 1016..1018, 959 / 960
  and 694 / 695; one of them (1016..1018) joins a y-265 deck to a y-230
  floor (node 192) over the x / z of [7]'s room (port
  TWELFTH_LEVEL_ROUTE.md section 7).
- **Census** (`route_census.py twelfth-delta`): at least 39 functions
  (18,004 bytes) that no earlier level ran: a13d 9 boot (no AREA13 overlay
  function is new), a19b 23 boot and 7 AREA19 overlay (among them [7]'s
  0x825930 and [6]'s 0x825420). a19b arms only AREA19, so a19b_00's AREA13
  frames (the hatch's ladder) were not measured for AREA13 code, and AREA13
  code hit four one-shot AREA19 breakpoints at a19b_00 f1 (0x823580,
  0x824BE0, 0x826840, 0x8293B0), which a19b_00's AREA19 frames therefore
  did not measure: the function at 0x826840 (748 bytes, link name
  func_overlay_AREA19_00826800) may have run there unseen, and 0x824BE0's
  first run may be earlier than the measured a19b_02 f1204 (port
  TWELFTH_LEVEL_ROUTE.md section 5). **Closed in section 8f:** an
  overlay-swap re-run of a19b_00 (boot + AREA13, swapped to AREA19 at the
  overlay change) measured neither 0x826840 nor 0x824BE0 there and no new
  AREA13 function in its AREA13 frames, so the twelfth level's counts are
  exact (a19b 30 new, 14,944 bytes; the level 39, 18,004 bytes).

## 8f. Measured (thirteenth level)

The port's `docs/THIRTEENTH_LEVEL_ROUTE.md` played group `a19c` (8 beats,
AREA19) of `tools/route_capture.py` from the twelfth level's a19b_02 (the
top of the ladder at z 974). Measured:

- **The bar over the y-172 floor and its swing.** The y-195 platform's
  slope stops a walk from the y-172 floor at z 972.4 (exploration). The
  attribute-0x34 bar at y 253.4 (x 1012.9..1022.9, z 972.4..1039.9) is
  entered from its attribute-0x3A pad on the walkway (0015D4C0 case 0x3A,
  NM: the hang, +5 0xF then 0x10 / 00169730, NM); with no wall ahead at its
  south end, 00169730's stick-forward case starts 0016A4B0 (NM, action
  0x28, the swing), and Use in the swing releases at the swing's end
  (speed D_00248630[+25C], up to 0.8): the player flew south and landed on
  the platform at (1017.9, 195, 929.7) (a19c_00).
- **[7]'s room.** The second bar (y 223.2, x 890.7..1003.2) from the
  platform's pad west to the room; the drop (00169730's use case) put the
  player in [6]'s area 0x82BD40 (x 885..933, z 912..959, y 190..200):
  0x825420 (C) ran script 0x82BA00 (6,962 frames, timed message pages)
  and set flag 0x1D (D_00810775) = counter 0x1D (D_008107F5) = 0xFF
  (a19c_01).
- **[7]'s Use** (Use north of [7] facing -z): script 0x82BD90; flag 0x1E
  (D_00810776) 1, counter 0x1E 3, 4, 5, 3, 0xFF; D_00810854 = 0x04 (door
  [27]'s bit 2); flag 0x1E 0xFF (a19c_02). [9] (0x825C70, C) had enabled
  the kind-0x5B cell 2 (a water surface at y 191 over the room behind door
  [27], key 0x21 of 0019C6F0) and disabled it at flag 0x1E 0xFF (cell
  directory bit 30, measured before and after).
- **Door [27] and sub 1.** Back over the bar, down the slope's slide, up
  to the walkway, down to the y-190 platform, along the east ledge to door
  [27]'s east side; its Use gave entry 2 (13 00 02 13, a19c_05). West
  across the room behind it and up its stair to the y-210 floor; the
  ladder at z 859.5 (00196970's circle) gave the request 13 01 07 01 at y
  314 and AREA19 sub 1 entry 7 (a19c_06); sub 1's door [52] (001BC350, BM,
  room move id 8) gave entry 1, where [34] (0x8279E0, C) ran script
  0x82E090 (flag 0x46 and counter 0x46 = 0xFF, item CC3 0x0B by code)
  (a19c_07). One hit on the ladder (health 90 -> 72).
- **Not reached: AREA15.** Sub 1's doors [50] / [51] open from the
  landings of a stair tower whose door from the hall, [49], is locked (bit
  3) by the seal [48] (001581A0, facing the tower's inside); the light and
  heavy melee and gunfire from the hall side left it intact (exploration;
  the hall side is a grid wall at z 939 that holds the player 6.1 from the
  seal's centre). The tower's ground floor holds the top of the ladder from
  sub 0's y-265 deck (sub 1 entry 8, read); sub 1's ladder down at x
  848..862 lies above sub 0's entry 13 (the top of the ladder to the deck)
  but how its descent hands over to sub 0 was not read; its top enclosure
  opens under sub 1's lift [36] (0x826C10, C).
- **Census** (`route_census.py thirteenth-delta`): 36 functions (23,628
  bytes) that no earlier level ran: 22 boot and 14 AREA19 overlay (among
  them 0016A4B0 / 0016A8B0, the swing, and ten of sub 1's placement
  behaviours, 0x826840 included).

## 9. Open

1. AREA19 is played through [7]'s room, [7]'s Use, door [27] and into sub
   1 as far as its hall (section 8f); AREA15 is not reached: sub 1's stair
   tower with doors [50] / [51] is entered, by the reading of section 8f,
   from sub 0's y-265 deck (sub 1 entry 8); a way from sub 1 down to the
   deck (the ladder at x 848..862 under the lift [36]) and how the deck's
   parts join were not read (port THIRTEENTH_LEVEL_ROUTE.md section 7). Section 7 steps 5..7
   are derived, not played beyond that. The conditions of AREA14's
   [6] (the owner of script 0x828120), AREA17's counter 0x5D (written by
   boot 0016BC40, not read) and AREA21's ending were read only as far as
   the tables show.
2. The lifts' return trips (AREA07 [0], AREA08 [8]) are not
   played; 001BC960's id-0xFF path is read (section 3), 001BDFC0's is not.
   Which actor writes counter 0x12 = 1 at AREA04's lift (the only direct
   store is 00131B10's state 0 with the actor's +0x0D bit 1 set, BM; it
   ran in no a04b replay) and whether [3]'s second stage (flag 0x12 =
   0xFF) is meant to run before the lift.
3. The AREA11 exits' second 001B0C60 call in 0x827630 (area `?` in the
   scan) is the first level's; not re-read here.
4. The scan does not follow computed addresses: a story byte written
   through a pointer other than the D_00810841 / D_00810758 / D_008107D8
   indexed forms would be missed.
