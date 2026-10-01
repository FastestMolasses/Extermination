# AREA13 overlay — decompilation status (lane A13C, 2026-09-30)

## What AREA13 is

AREA13 is reached from AREA04 by its lift [51] (docs/WORLD_GRAPH.md). Its
only exits are the lift back and the falls into AREA19 entry 9 / 10
(WORLD_GRAPH.md, route note 4). The ninth-level route played its lobby,
door [8], door [14], door [17] from the east, item 0x27, the hatch [62] and
its ladder into AREA19 (group `a13` of `tools/route_capture.py`). This lane
did not run the game; the behaviour below is read from the byte-matched C.

The code module is `OVERLAY/AREA13.BIN`: overlay id 10, text 0x67C0, data
0x4580, bss 0x140C00, file 44416 bytes. Like every overlay it is linked at
0x00823500 but runs 0x40 higher (the MWo3 header is loaded first), so splat
and link names are the runtime address minus 0x40. A data address in the
code is the runtime address; it reads the file byte at (address -
0x823500). The boot area dispatcher func_001E7780 calls the init at
runtime 0x8236E0 (link 008236A0, earlier C).

## Split handling

`overlay_match.py list AREA13` regroups the 63 splat pieces into 49
functions. Thirteen groups hold later pieces, all fake splits: an
intra-overlay call target 0x40 into the called function, which splat opens
as a piece and the link absorbs (`[fill] 14 splat piece(s) absorbed by 13
compiled function(s)`). 00824140 absorbs two (00824160 and 00824180). A
scan of every slot for code after a return (build/a13c/hidden.py) finds no
second function in any slot.

One function is a jump-table dispatcher: 0x824180 (link 00824140), a
seven-case switch whose table the original keeps at 0x82E200. The compiled
C carries the table in its own .rodata; `tools/overlay/jt_pin.py` places it
at that address at link (`[pin] func_overlay_AREA13_00824140: .rodata 0x1c
bytes at runtime 0x0082e200`). `overlay_match.py check` reports such a
function as 99.98 (`rodata-needs-pin`); the linked overlay is the proof.

## Status

49 functions: 46 are compiled C that links byte-identical (41 new in this
lane, including the dispatcher above, plus 5 earlier files). 2 are NEARMISS
readable C, linked from their splat .s. The entry pad stays an asm body
(user decision 2026-09-23). The six asm-body files 008239D0, 00823B80,
00826100, 00826F80, 008284A0 and 008299A0 were replaced by C.

| link name | runtime | size | status | reached by (static, `area_overview.py --area 13`) |
|---|---|---|---|---|
| func_overlay_AREA13_00823500 | 0x823540 | 0x4 | asm pad | nop sled, no code |
| overlay_AREA13_func_00823540 | 0x823580 | 0x158 | C, byte-identical | sub0 place [17] |
| func_overlay_AREA13_008236A0 | 0x8236E0 | 0x20 | C, byte-identical (earlier) | boot 0x1e7780 (area_dispatch_off01E0_state0D00) |
| func_overlay_AREA13_008236C0 | 0x823700 | 0x12C | C, byte-identical | sub0 place [3] |
| func_overlay_AREA13_008237F0 | 0x823830 | 0x104 | C, byte-identical (absorbs 00823830) | call from 0x823700 |
| func_overlay_AREA13_00823900 | 0x823940 | 0xD0 | C, byte-identical (absorbs 00823940) | call from 0x823700 |
| func_overlay_AREA13_008239D0 | 0x823A10 | 0x24 | C, byte-identical (was asm body) | script 0x82a770 op09 record 0x82aa30 |
| func_overlay_AREA13_00823A00 | 0x823A40 | 0x174 | C, byte-identical | sub0 place [4] |
| func_overlay_AREA13_00823B80 | 0x823BC0 | 0x4C | C, byte-identical (was asm body) | sub0 place [5], [6] |
| func_overlay_AREA13_00823BD0 | 0x823C10 | 0x138 | C, byte-identical (absorbs 00823C10) | call from 0x823bc0 |
| func_overlay_AREA13_00823D10 | 0x823D50 | 0x138 | C, byte-identical (absorbs 00823D50) | call from 0x823bc0 |
| func_overlay_AREA13_00823E50 | 0x823E90 | 0x148 | C, byte-identical | sub0 place [44] |
| func_overlay_AREA13_00823FA0 | 0x823FE0 | 0x7C | C, byte-identical | - (no static reference) |
| func_overlay_AREA13_00824020 | 0x824060 | 0x7C | C, byte-identical (absorbs 00824060) | call from 0x824390 |
| func_overlay_AREA13_008240A0 | 0x8240E0 | 0x7C | C, byte-identical (absorbs 008240E0) | call from 0x824390 |
| func_overlay_AREA13_00824120 | 0x824160 | 0x18 | C, byte-identical (earlier) | call from 0x823e90 |
| func_overlay_AREA13_00824140 | 0x824180 | 0x208 | C, byte-identical at link (jump table pinned) (absorbs 00824160, 00824180) | call from 0x823e90 |
| func_overlay_AREA13_00824350 | 0x824390 | 0x18C | C, byte-identical (absorbs 00824390) | call from 0x823e90 |
| func_overlay_AREA13_008244E0 | 0x824520 | 0x1AC | C, byte-identical (absorbs 00824520) | call from 0x823e90 |
| func_overlay_AREA13_00824690 | 0x8246D0 | 0xE0 | C, byte-identical | script 0x82b3d0 op09 record 0x82b550 |
| func_overlay_AREA13_00824770 | 0x8247B0 | 0x10C | C, byte-identical | script 0x82b810 op09 record 0x82b9d0 |
| func_overlay_AREA13_00824880 | 0x8248C0 | 0x94 | C, byte-identical | script 0x82b3d0 op09 record 0x82b690; script 0x82b810 op09 record 0x82bb10 |
| func_overlay_AREA13_00824920 | 0x824960 | 0x88 | C, byte-identical (absorbs 00824960) | call from 0x823e90 |
| func_overlay_AREA13_008249B0 | 0x8249F0 | 0x84 | C, byte-identical (absorbs 008249F0) | call from 0x824390 |
| func_overlay_AREA13_00824A40 | 0x824A80 | 0x12C | C, byte-identical | sub0 place [7] |
| func_overlay_AREA13_00824B70 | 0x824BB0 | 0x1590 | C, byte-identical | sub0 deferred group 0x829d00[n] x4 |
| func_overlay_AREA13_00826100 | 0x826140 | 0x4D0 | C, byte-identical (was asm body) (absorbs 00826140) | call from 0x824bb0 |
| func_overlay_AREA13_008265D0 | 0x826610 | 0x84 | C, byte-identical (absorbs 00826610) | call from 0x824bb0 |
| func_overlay_AREA13_00826660 | 0x8266A0 | 0x1AC | C, byte-identical | sub0 deferred group 0x829d00[n] x4 |
| func_overlay_AREA13_00826810 | 0x826850 | 0x760 | C, byte-identical | sub0 place [62], [63] |
| func_overlay_AREA13_00826F70 | 0x826FB0 | 0x10 | C, byte-identical (earlier) | script 0x82ca50 op09 record 0x82cd10 |
| func_overlay_AREA13_00826F80 | 0x826FC0 | 0x2C | C, byte-identical (was asm body) | script 0x82ca50 op09 record 0x82cc90 |
| func_overlay_AREA13_00826FB0 | 0x826FF0 | 0x158 | C, byte-identical | sub0 place [58], [59] |
| func_overlay_AREA13_00827110 | 0x827150 | 0xADC | C, byte-identical | sub0 place [45], [46] |
| func_overlay_AREA13_00827BF0 | 0x827C30 | 0x19C | C, byte-identical | step table 0x82D190[0], called by 0x827150 |
| func_overlay_AREA13_00827D90 | 0x827DD0 | 0x2C | C, byte-identical (earlier) | step table 0x82D190[1], called by 0x827150 |
| func_overlay_AREA13_00827DC0 | 0x827E00 | 0x120 | C, byte-identical | step table 0x82D190[2], called by 0x827150 |
| func_overlay_AREA13_00827EE0 | 0x827F20 | 0x70 | C, byte-identical | step table 0x82D190[3], called by 0x827150 |
| func_overlay_AREA13_00827F50 | 0x827F90 | 0x548 | NEARMISS 98.46% | step table 0x82D190[4], called by 0x827150 |
| func_overlay_AREA13_008284A0 | 0x8284E0 | 0x20 | C, byte-identical (was asm body) | code pointer in 0x828500 |
| func_overlay_AREA13_008284C0 | 0x828500 | 0x758 | C, byte-identical | - (no static reference) |
| func_overlay_AREA13_00828C20 | 0x828C60 | 0x1AC | C, byte-identical | - (no static reference) |
| func_overlay_AREA13_00828DD0 | 0x828E10 | 0x11C | C, byte-identical | - (no static reference) |
| func_overlay_AREA13_00828EF0 | 0x828F30 | 0x8 | C, byte-identical (earlier; empty) | code pointer in 0x828f40 |
| func_overlay_AREA13_00828F00 | 0x828F40 | 0x358 | C, byte-identical | - (no static reference) |
| func_overlay_AREA13_00829260 | 0x8292A0 | 0xF8 | C, byte-identical | sub0 place [49], [50], [51], [52], [53], [54] |
| func_overlay_AREA13_00829360 | 0x8293A0 | 0x63C | C, byte-identical | sub0 place [47], [48] |
| func_overlay_AREA13_008299A0 | 0x8299E0 | 0xB8 | C, byte-identical (was asm body) | sub0 deferred group 0x829d00[n] |
| func_overlay_AREA13_00829A60 | 0x829AA0 | 0x1F8 | NEARMISS 94.23% | - (no static reference) |

Every new file is mwcc 2.3.3 (`// COMPILER: mwcc233`, `-O4,p`,
`-sdatathreshold 0`; `-sdatathreshold 4` where the function reads a
gp-relative global: D_00275B40, D_00275CA8, D_002759B8, D_002759C0 or
D_002759C8). "Reached by" comes from the static tables of
`tools/area_overview.py --area 13` (placement records, deferred groups,
script op09 records, direct calls); the five "step table" rows are the
words of 0x82D190, which 0x827150 indexes by its step. Each source header
describes what the instructions do; roles are read from the code, not from
placement labels.

## Behaviour (from the byte-matched C)

- **The [44] event (0x823E90) and D_008107F4.** State 1 of [44] runs one
  step per value of D_008107F4 & 0xF: 0x824160 (0), 0x824180 (1), 0x824390
  (2), 0x824520 (3), 0x824960 (4). Each step ends by adding 1 to
  D_008107F4. Step 1 is the conversation: on +0xB bit 2 it starts script
  0x82B090 and then 0x82B2D0; with +0xB bit 0 also set it skips to
  0x82B2D0 and sets D_00810774 (flag 0x1C) = 1. At the end of 0x82B090
  it sets D_008106B1 = +0x34 + 0x80, D_008106B0 = 1 and D_008106D0 =
  +0x14.
  Step 2 waits 490 frames, calls 0x8249F0 (+0x36 = 1 on up to 12 chained
  objects with +2 & 0x1F == 4, +3 == 0xA and +0xB4 >= 215) and starts
  script 0x82B3D0 or 0x82B810 when D_00810774 != 0xFF and the player
  stands in the area 0x82E140 or 0x82E180 above y 210 (0x8240E0 /
  0x824060), else script 0x82BC90.
  The callbacks 0x8246D0 / 0x8247B0 / 0x8248C0 move D_00810350..58 frame
  by frame during those scripts. Step 3 releases scripts 0x82BD10 /
  0x82BDD0 / 0x82BE90 as the owner's +0x28 reaches 2 / 3 / 4, then calls
  func_001FABB0() and func_001FA790(0, 0x12). Step 4 waits for
  D_00810833 != 0, runs script 0x82C110 and ends [44] (state 3).
- **The step controller [45] / [46] (0x827150).** It publishes its work
  block in D_00275CA8 every frame and, for +0xD 0x11, waits for
  D_008107F4 bit 1, ORs 0x10, then ORs 0x40 every frame and runs the five
  step handlers of 0x82D190 against a frame count and a per-step limit.
  Step 0 (0x827C30) ORs 0x20 into D_008107F4 at count 0x1F3; step 4
  (0x827F90) sets D_00810833 = 0xFF when the object's +0x3C falls to 4,
  which releases [44]'s step 4. While the step is below 4 it draws up to
  four markers (model 0x3F5) and sparks at the points 0x82D1B0; steps 0
  and 2 lower the count of markers left. When D_00810774 is 0xFF in state
  0, the object takes +0xD 0x12 and model 0x12 instead, and then only
  animates while the flag stays 0xFF.
- **Objects keyed to D_008107F4 bits.** [49]..[54] (0x8292A0) end once bit
  5 (0x20) is set; [47] / [48] (0x8293A0) switch between the two poses of
  their placement record by bit 6 (0x40) and toggle func_0019C6F0(0x1F /
  0x20).
- **[3] (0x823700) and D_008107F1.** While D_008107F1 is 0: with the
  player at y <= 170 while D_00810702 == 2, script 0x82A360; at its end
  func_001C4760(9, 1), func_001C4760(0x4F, 1) and D_008107F1 = 1. While
  D_008107F1 is 1 it is an examine point with script 0x82A620.
- **[4] (0x823A40).** Unless flag 0x1A is set, script 0x82A770 when the
  player is in the area 0x82E100 at y <= 170; its callback 0x823A10 calls
  func_001C47A0(0x1A, 1).
- **Door [17] (0x823580).** Standard door steps (func_001BBDA0 /
  001BBE40 / 001BC0E0 / 001BC240 / 001BC290 / 001BC300); while
  D_00810774 == 1 it sets its +0 to 2 and its owner's +0xB to 0.
- **The hatches [62] / [63] (0x826850).** The one with z > 1000 uses
  D_00810839 bit 0, the other bit 1. With its bit set it takes +0xD = 0xD
  and state 2 at once. Otherwise +0xB bit 2 starts script 0x82CA50 (positions written
  into 0x82CAB0 / 0x82CAC0 / 0x82CB00 first); at its end the hatch takes
  model 0xD and ORs its bit into D_00810839. Four lights mark its
  corners while closed.
- **The AREA01 twins.** 0x824BB0, 0x826140, 0x826610 and 0x8266A0 are
  the AREA01 functions 0x826D40, 0x8282F0, 0x8287C0 and 0x828850 with
  the differences listed in their headers (no flag 6 / state 0x64 logic,
  D_00810702 < 8 waits, one extra probe-miss effect).
- **Unreached code.** 0x823FE0 (an area test for 0x82E1C0), the thrown
  piece 0x828500, the effects 0x828C60 / 0x828E10 / 0x828F40 and the GS
  packet 0x829AA0 have no static reference in the overlay; who uses them
  is not traced.

## Matching notes (mwcc 2.3.3)

- **Literal address versus extern.** Where the original leaves a branch
  slot empty but mwcc hoists a literal's upper half into it, the read is
  spelled through an extern; where the original reloads after a store,
  the store goes through an extern and the read through a literal (or the
  reverse). Both spellings of the same variable appear in one function
  (0x826850 reads D_00810C8B both ways; 0x8292A0 reads D_008107F4 both
  ways).
- **Scratchpad stores through extern arrays** (`D_700038A0[k]`,
  `D_700036A0[13]`, `D_700038B0[1]`) let loads move above stores the way
  the original does; literal-address stores act as alias barriers.
- **Argument order.** func_001B1EA0's area argument is written
  `(void *)D_overlay_AREA13_0082E100`; the cast keeps the original's order
  of setting a2 before a1 (0x823A40, 0x823FE0, 0x824060, 0x8240E0,
  0x824A80).
- **Byte externs with `-sdatathreshold 4`** must be unsized arrays read as
  `[0]` (`D_00810700[]`, `D_70003A20[]`), otherwise they turn gp-relative.
- **Placement record pose.** `(unsigned char *)(self[0x9A] * 0x28 +
  (int)D_0024D7C0[D_00810700][D_00810701])` (0x827150, 0x8293A0).
- **idiom-24 int-staged float** for the second size of func_001CD520 in
  0x826140 (the only difference from the AREA01 twin's C).
- Declaration order sets the saved-register assignment; `return` versus
  `break` exits change slot filling and branch-likely use (bounded
  permutation scripts build/a13c/perm.py and exits.py).

### NEARMISS functions

- **0x827F90 (98.46%, C 0x10 shorter).** The five-spark loop of phase 1:
  the original counts it down from a trip count (4 + 1, tested for zero
  before the first pass), and keeps self in s1 where mwcc 2.3.3 uses s0.
  `for (i = 4; i >= 0; i--)` runs the same five passes. Count-up,
  count-down, `!= -1`, `> -1`, unsigned and trip-count spellings were
  tried (best 98.62, not kept because it only moves the size).
- **0x829AA0 (94.23%, C 0x4 longer).** The state byte is loaded into a2
  before the self copy in the original, which then fills the case-3 slot
  with the copy and uses branch-likely on case 2; mwcc 2.3.3 copies self
  first. This is the wall recorded for AREA16 0x8237A0.
- **Equivalence.** Both were diffed against the original instruction by
  instruction (`overlay_match.py check AREA13 .. --show`): the loop runs
  the same five passes with the same calls, and 0x829AA0 differs only in
  the dispatch prologue; stores, calls and control flow are equal.

## Verification

- `overlay_match.py check AREA13 src/overlays/AREA13/*.c`: 40 new files
  100.00 BYTE-IDENTICAL, 0x824180 at 99.98 (`rodata-needs-pin` only), the
  2 NEARMISS as above.
- `python3 tools/check_no_disassembly.py src/overlays/AREA13/*.c`: clean.
- Bounded mutation sweep (single-constant edits in scratch copies, build/a13c/mut):
  0x823E90 `& 0xF` -> `& 0x7` (99.99) and 0x827C30 `0x1DF` -> `0x1E0`
  (99.99). Neither stays byte-identical.
- Under the decomp build lock: `compile_overlay_src.py AREA13` (47 C objects
  with the pad, 2 NEARMISS skipped), then `tools/overlay/build.py --area
  AREA13 --no-extract --no-yaml --no-splat`: PASS, AREA13.BIN equals the
  extracted file (44416 bytes, full file). Every linked filler object's
  `.text` equals the compiled `.text` outside relocation fields, zero-padded
  to its slot (build/a13c/prov.py, log build/a13c/prov.log). This
  regenerated `config/overlays/AREA13.lds`: absolute definitions of the new
  externs and the pinned-table layout (the data object split around
  0x82E1C0..0x82E1DC link).

## Gate

One locked run for both areas (build/a13c/gate.sh full; lock acquired
16:19:38, done 16:31:43; log build/a13c/gate_full.log, decomp log
build/a13c/decomp_build.log): `compile_overlay_src.py` and
`tools/overlay/build.py --area ... --no-extract --no-yaml --no-splat` for
AREA13 and AREA19 (both "1/1 overlays passed", full files byte-identical),
full `tools/decomp/build.py build` (rc 0, 2211 units), then
`tools/verify_all.py`: all six stages PASS. The boot ELF is byte-identical
(0x175b00 loadable bytes), 19/19 overlays pass, matched_code is 98.62%
(2152/2211), and glTF, selftest and gs-offset pass. Overlay C is not an
objdiff unit, so matched_code does not count this lane. An earlier locked
overlay-only run (build/a13c/gate.log) gave the same PASS. After the
compile step only the NEARMISS file 0x823D10 of AREA19 changed (its if /
else arms and header; 92.35 -> 93.11); NEARMISS files are not compiled, so
the linked overlays are unaffected.

## Binding

The native port has no AREA13 module, and this lane edited nothing in the
port. For a port binding the C above is the ground truth; the NEARMISS
bodies are correct apart from the listed scheduling. The flag and counter
writes (D_008107F1, D_008107F4, D_00810774, D_00810833, D_00810839,
D_0081081A) are what docs/WORLD_GRAPH.md's AREA13 section lists by
address.

## Known gaps

- Two NEARMISS functions (above). The entry pad is an asm body.
- The data section (0x4580 bytes: scripts, the placement table 0x82D570,
  deferred groups 0x829D00 / 0x82A230) is linked from splat's data
  assembly, as in every overlay.
- Six functions have no static reference (Behaviour, last item); who
  spawns them is not traced.
- The verification cites git-ignored scratch in build/a13c (gate.sh, decomp_build.log,
  gate.log, gate_full.log, hidden.py, prov.py, prov.log, perm.py,
  exits.py, var.py, mut/). They are receipts a committed doc cites and
  must not be deleted.
