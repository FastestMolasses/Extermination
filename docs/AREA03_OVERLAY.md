# AREA03 overlay — decompilation status (lane A03C, 2026-10-01)

## What AREA03 is

AREA03 is reached from AREA19 through the panel [24] door (docs/WORLD_GRAPH.md,
section 7 step 5, inferred, not played). Its own door table leads back to AREA19
(sub 0 [15]), to AREA04 entry 2 (sub 1 [24], lock bit 0 of D_00810844) and,
by the sub 2 lift [7], to AREA08. This lane did not run the game; the behaviour
below is read from the byte-matched C.

The code module is `OVERLAY/AREA03.BIN`: overlay id 4, text 0x33C0, data
0x2800, bss 0x140C00, file 23552 bytes, three sub-states (placement tables
0x8277F0 / 0x827B60 / 0x828090). Like every overlay it is linked at 0x00823500
but runs 0x40 higher, so splat and link names are the runtime address minus
0x40. A data address in the code is the runtime address; it reads the file
byte at (address - 0x823500).

## Split handling

`overlay_match.py list AREA03` regroups the 23 splat pieces into 19 functions.
The pad group holds the entry pad and the earlier init C
(`overlay_AREA03_func_00823540.c`). Three groups hold one later piece each,
fake splits at an intra-overlay call target 0x40 into the called function
(`[fill] 3 splat piece(s) absorbed by 3 compiled function(s)`). A scan of
every slot for code after a return (build/a03c/hidden.py) finds no second
function.

Two functions are jump-table dispatchers: 0x825980 and 0x825DF0 switch on
D_00275CA4 (cases 0..5; 0 / 5, 1 / 4 and 2 / 3 / other share a body), with
the original tables at 0x829080 and 0x8290A0. `tools/overlay/jt_pin.py`
places the compiled tables there (`[pin] func_overlay_AREA03_00825940:
.rodata 0x18 bytes at runtime 0x00829080`, `... 00825DB0: ... 0x008290a0`).
`overlay_match.py check` reports them as 99.99 (`rodata-needs-pin`); the
linked overlay is the proof.

## Status

19 functions: all are compiled C that links byte-identical (14 new in this
lane, plus the earlier init 00823540, 008262F0 and 00826820). The entry pad
stays an asm body (user decision 2026-09-23). The asm-body files 00824CF0,
00826840 and 00826870 were replaced by C. No NEARMISS.

| link name | runtime | size | status | reached by (static, `area_overview.py --area 3`) |
|---|---|---|---|---|
| func_overlay_AREA03_00823500 | 0x823540 | 0x4 | asm pad | nop sled, no code |
| overlay_AREA03_func_00823540 | 0x823580 | 0x20 | C, byte-identical (earlier; init) | - |
| func_overlay_AREA03_00823560 | 0x8235A0 | 0x1D8 | C, byte-identical | sub1 place [22] |
| func_overlay_AREA03_00823740 | 0x823780 | 0x84 | C, byte-identical (absorbs 00823780) | call from 0x823810 |
| func_overlay_AREA03_008237D0 | 0x823810 | 0x114 | C, byte-identical | sub1 deferred group 0x826BC0[3] |
| func_overlay_AREA03_008238F0 | 0x823930 | 0x13F4 | C, byte-identical | sub0 deferred group 0x826900[4], [6] |
| func_overlay_AREA03_00824CF0 | 0x824D30 | 0x4D0 | C, byte-identical (was asm body) (absorbs 00824D30) | call from 0x823930 |
| func_overlay_AREA03_008251C0 | 0x825200 | 0x84 | C, byte-identical (absorbs 00825200) | call from 0x823930 |
| func_overlay_AREA03_00825250 | 0x825290 | 0x198 | C, byte-identical | sub0 deferred group 0x826900[5], [7] |
| func_overlay_AREA03_008253F0 | 0x825430 | 0x2A8 | C, byte-identical | sub0 place [7] |
| func_overlay_AREA03_008256A0 | 0x8256E0 | 0x298 | C, byte-identical | sub0 place [8] |
| func_overlay_AREA03_00825940 | 0x825980 | 0x470 | C, byte-identical at link (jump table pinned) | sub0 place [9] |
| func_overlay_AREA03_00825DB0 | 0x825DF0 | 0x478 | C, byte-identical at link (jump table pinned) | sub0 place [10] |
| func_overlay_AREA03_00826230 | 0x826270 | 0xB8 | C, byte-identical | script 0x8282D0 op09 record 0x828750 |
| func_overlay_AREA03_008262F0 | 0x826330 | 0x10 | C, byte-identical (earlier) | script 0x8282D0 op09 records x5 |
| func_overlay_AREA03_00826300 | 0x826340 | 0x290 | C, byte-identical | sub0 place [11] |
| func_overlay_AREA03_00826590 | 0x8265D0 | 0x290 | C, byte-identical | sub0 place [12] |
| func_overlay_AREA03_00826820 | 0x826860 | 0x18 | C, byte-identical (earlier) | scripts 0x8282D0 / 0x828CD0 op09 x4 |
| func_overlay_AREA03_00826840 | 0x826880 | 0x2C | C, byte-identical (was asm body) | scripts 0x8282D0 / 0x828CD0 op09 x3 |
| func_overlay_AREA03_00826870 | 0x8268B0 | 0x2C | C, byte-identical (was asm body) | scripts 0x8282D0 / 0x828CD0 op09 x2 |

Every new file is mwcc 2.3.3 (`// COMPILER: mwcc233`, `-O4,p`,
`-sdatathreshold 0`; `-sdatathreshold 4` where the function reads a
gp-relative global: D_00275B40, D_00275CA0, D_00275CA4, D_00275900 or
D_00275908). Each source header describes what the instructions do; roles
are read from the code, not from placement labels.

## Behaviour (from the byte-matched C)

- **D_00275CA0 and the 104.5-unit move.** 0x826860 (earlier C) negates
  D_00275CA0 (0 <-> 1). The callback 0x826270 of script 0x8282D0 sets
  D_00810702 = 3 and adds 104.5 to D_00810354, D_008105D4 and D_008105E4
  when D_00275CA0 is set, else sets D_00810702 = 2 and subtracts 104.5. The
  sub 0 spawn entries 2 (554, -25, 417.5) and 3 (554, 79.5, 417.5) differ
  only in y, by 104.5 (`area_overview.py` spawn table).
- **[7] 0x825430 / [8] 0x8256E0.** Each moves the +0x7C of the objects at
  *(D_00275B40 + 4) / + 8 between 0 and -5 / +5 over 60 frames when
  D_00275CA0 changes ([7]: -5 / +5 while it is set; [8]: while it is clear),
  calling func_001B17A0 only in its resting state. [7] also sets its scale
  (+0x80..+0x88) to 1.5.
- **[9] 0x825980 / [10] 0x825DF0, [11] 0x826340 / [12] 0x8265D0.** +0xB
  bit 2 starts a script: 0x8282D0 for [9] / [10]; 0x828BD0 or 0x828CD0 for
  [11] / [12] by D_00275CA0 (swapped between the two); each then calls
  func_001B6F00(self, (0, 0, 6 or 5.6, 1), -2.9670596) and func_001FBD50(self, 0x19A, 0, 300.0) 120 frames into the script.
  Each draws a func_001F4BF0 marker at a fixed point with colour (0x80, 0,
  0, 0x80) or (0, 0x80, 0, 0x80) chosen by D_00275CA0 (by D_70003B92 in [9]
  before its script); [10] sets D_00275CA4 = 0 at its script start and end.
  [9] / [10] also place a func_001F4E20 point chosen by D_00275CA4 while
  D_008106A0 is outside (-140, 40) / (-130, 50) degrees. 0x826330 (earlier C)
  sets D_00275CA4 from its third argument's +8.
- **[22] 0x8235A0 and the sub 1 group 0x826BC0[3] 0x823810.** On +0xB bit
  2, 0x823810 sets D_00810781 = 1 (flag 0x29 of WORLD_GRAPH.md)
  and, once D_00810781 == 0xFF, goes to state 2 (func_001B1190(+0x9A),
  func_001AFC10). [22] starts script 0x827170 when D_00810781 == 1
  and, with +0xB bit 2 and D_00810781 == 0xFF, script 0x827630; at a script
  end the first time it calls func_001C47A0(0x2A, 1) and func_001C4760(0x12,
  1) and sets D_00810801 = 0xFF (counter 0x29) and +2 = 0x84. WORLD_GRAPH.md
  reads these two calls as the item C64 0x2A / CC3 0x12 gifts of
  0x8235A0.
- **The AREA01 family.** 0x823930 is the AREA19 0x827DD0 C except the hit
  branch (scratch D_70003600 / D_70003610; a hit of another kind always gets
  effect 0x80000003, no func_0019B6C0 retry). 0x824D30 is the AREA13
  0x826140 C, 0x825200 the AREA19 0x829840 C, 0x825290 the AREA19 0x8298D0
  C (each with this overlay's data labels).
- **0x826880 / 0x8268B0.** Script callbacks calling func_001FBD50(self,
  0x40B / 0x3F7, 0, 300.0) and returning 1.

## Matching notes (mwcc 2.3.3)

The levers of docs/AREA13_OVERLAY.md and AREA19_OVERLAY.md applied; specific
to AREA03:

- **Statement order around array stores.** 0x826270 matches with the
  D_00810702 store after the first float update (or later) and with
  D_008105D4 / D_008105E4 as `D_008105D0[1]` / `[5]` of one array, which
  keeps the 0x8105E4 load after the 0x8105D4 store.
- **Branch arm order.** In 0x825430 the count-down test is written `if
  (x > 0.0f) {move} else {stop}` (the original's fall-through arm).
- **Duplicated tail stores.** 0x826340 / 0x8265D0 store the colour's fourth
  word in each arm, as the original does.
- **Twins by diff.** `build/a03c/twins.py` scores each function's
  instruction stream against every other overlay; the four AREA01-family
  functions were taken from the AREA13 / AREA19 C and differ only where the
  diff showed (`build/a03c/sdiff2.py`).

## Verification

- `overlay_match.py check AREA03 src/overlays/AREA03/*.c`: 16 files 100.00
  BYTE-IDENTICAL, 0x825980 / 0x825DF0 at 99.99 (`rodata-needs-pin` only).
  The init 00823540 shares the pad's slot and is checked by the link.
- `python3 tools/check_no_disassembly.py src/overlays/AREA03/*.c`: clean.
- Bounded mutation sweep (build/a03c/mut): 0x826270 104.5 -> 105.5 (99.96);
  see docs/AREA21_OVERLAY.md and AREA14_OVERLAY.md for the other two. None
  stays byte-identical.
- Under the decomp build lock: `compile_overlay_src.py AREA03` (20 objects),
  then `tools/overlay/build.py --area AREA03 --no-extract --no-yaml
  --no-splat`: PASS, AREA03.BIN equals the extracted file (23552 bytes, full
  file). Every linked filler object's `.text` equals the compiled `.text`
  outside relocation fields, zero-padded to its slot (build/a03c/prov.py,
  log build/a03c/prov.log). `config/overlays/AREA03.lds` was regenerated
  (absolute definitions of the new externs and the two pinned tables).

## Gate

See "Gate" in docs/AREA21_OVERLAY.md (one locked run for AREA03, AREA14 and
AREA21).

## Binding

The native port has no AREA03 module, and this lane edited nothing in the
port. For a port binding the C above is the ground truth.

## Known gaps

- The data section (0x2800 bytes: scripts 0x827170 / 0x827630 / 0x8282D0 /
  0x828BD0 / 0x828CD0, placement tables, groups 0x826900 / 0x826BC0 /
  0x826FE0 / 0x827090 / 0x8270F0) is linked from splat's data assembly.
- No AREA03 capture exists; the order of the D_00275CA0 / D_00275CA4 /
  D_00810781 events on a played route is not measured, and which script
  steps call 0x826270 / 0x826330 / 0x826860 is read from the op09 records
  only.
- The verification cites git-ignored scratch in build/a03c (gate.sh,
  gate.log, gate_full.log, decomp_build.log, prov.py, prov.log, hidden.py,
  twins.py, sdiff2.py, mut/). They are receipts a committed doc cites and
  must not be deleted.
