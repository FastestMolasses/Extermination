# AREA14 overlay — decompilation status (lane A03C, 2026-10-01)

## What AREA14 is

AREA14 is reached from AREA00 sub 2 (the [28] 0x826790 request with item
0x26) and from AREA18 by the boot function 00190F20; its [6] script callback
0x825C40 requests AREA17 (docs/WORLD_GRAPH.md, section 5 and section 7 step
7, inferred, not played). Its door table has one empty record. This lane did
not run the game; the behaviour below is read from the byte-matched C.

The code module is `OVERLAY/AREA14.BIN`: overlay id 11, text 0x2C40, data
0x2300, bss 0x140C00, file 20352 bytes, one sub-state. Like every overlay it
is linked at 0x00823500 but runs 0x40 higher, so splat and link names are
the runtime address minus 0x40. The boot area dispatcher func_001E7780 calls
the init at runtime 0x823740 (link 00823700, earlier C).

## Split handling

`overlay_match.py list AREA14` regroups the 16 splat pieces into 14
functions. Two groups hold one later piece each, fake splits at an
intra-overlay call target (`[fill] 2 splat piece(s) absorbed by 2 compiled
function(s)`). A scan for code after a return inside a slot
(build/a03c/hidden.py) finds no second function. AREA14 has no jump table.

## Status

14 functions: 12 are compiled C that links byte-identical (10 new in this
lane, plus the earlier init 00823700 and 00823880); 1 is NEARMISS readable
C, linked from its splat .s; the entry pad stays an asm body. The asm-body
file 008254F0 was replaced by C.

| link name | runtime | size | status | reached by (static, `area_overview.py --area 14`) |
|---|---|---|---|---|
| func_overlay_AREA14_00823500 | 0x823540 | 0x4 | asm pad | nop sled, no code |
| overlay_AREA14_func_00823540 | 0x823580 | 0x1B4 | C, byte-identical | sub0 place [35], [36] |
| func_overlay_AREA14_00823700 | 0x823740 | 0x20 | C, byte-identical (earlier; init) | boot 0x1E7780 |
| func_overlay_AREA14_00823720 | 0x823760 | 0x160 | C, byte-identical | sub0 place [5] |
| func_overlay_AREA14_00823880 | 0x8238C0 | 0x20 | C, byte-identical (earlier) | script 0x826800 op09 record 0x826A00 |
| func_overlay_AREA14_008238A0 | 0x8238E0 | 0x244 | C, byte-identical | sub0 place [0] |
| func_overlay_AREA14_00823AF0 | 0x823B30 | 0x300 | C, byte-identical | sub0 place [2] |
| func_overlay_AREA14_00823DF0 | 0x823E30 | 0x18C | NEARMISS 95.96% | sub0 place [4] |
| func_overlay_AREA14_00823F80 | 0x823FC0 | 0x1568 | C, byte-identical | sub0 deferred group 0x826180[n] x4 |
| func_overlay_AREA14_008254F0 | 0x825530 | 0x4D4 | C, byte-identical (was asm body) (absorbs 00825530) | call from 0x823FC0 |
| func_overlay_AREA14_008259D0 | 0x825A10 | 0x84 | C, byte-identical (absorbs 00825A10) | call from 0x823FC0 |
| func_overlay_AREA14_00825A60 | 0x825AA0 | 0x198 | C, byte-identical | sub0 deferred group 0x826180[n] x4 |
| func_overlay_AREA14_00825C00 | 0x825C40 | 0x38C | C, byte-identical | script 0x828120 op09 record 0x828320 |
| func_overlay_AREA14_00825F90 | 0x825FD0 | 0x1A8 | C, byte-identical | sub0 place [6] |

Every new file is mwcc 2.3.3 (`// COMPILER: mwcc233`, `-O4,p`,
`-sdatathreshold 0`; `-sdatathreshold 4` for the AREA01-family functions
that read D_00275B40). Each source header describes what the instructions
do.

## Behaviour (from the byte-matched C)

- **[0] 0x8238E0.** State 3 at once when flag 0x3D is set
  (func_001BA1C0(self, 0x3D)); otherwise, with D_00810354 < 435 inside the
  80-byte area 0x826F10 (func_001B1EA0 kind 5, copied to the stack), script
  0x826C10; at its end +0x2E = 0xFFFF, func_001C4760(0x16, 1), D_00810815 =
  0xFF (counter 0x3D) and func_001FAE70(0). Its frame calls pause while
  D_70003B92 is set in that step.
- **[2] 0x823B30.** State 3 when flag 0x3F is set. While D_00810816
  (counter 0x3E) is 0: with D_00810354 >= 675 inside the area 0x828400,
  script 0x826FC0; at its end D_00810816 = 1, func_001C67E0(self, 0, 0, 0),
  func_001C47A0(0xF, 1), func_001C4760(0x17, 1), func_001FAE70(0). After
  that +0xB bit 2 starts script 0x827340, whose end calls
  func_001C67E0(self, 0, 20.0, 0.0).
- **[4] 0x823E30 (NEARMISS).** State 3 when flag 0x3F is set or D_00810C8C
  is 0. At spawn entry 1 (D_00810702 == 1) script 0x827490 with a 360-frame
  func_00183160(2, 0.0) phase, then func_00183160(0, 0.0) and script
  0x827590; at its end func_001C4760(0x18, 1) and D_00810817 = 0xFF
  (counter 0x3F).
- **[5] 0x823760.** With flag 0x2C set: func_001F6B00() and
  func_001B6660(group 0x8265A0) at once; else script 0x826800, the same two
  calls after 400 frames (or at the script end), +0x2E = 0xFFFF and
  D_00810784 = D_00810804 = 0xFF.
- **[6] 0x825FD0 and its callback 0x825C40.** [6] sets D_700038A0 = (221.6,
  695.1, 1181, 1), calls func_00158590(self, 1 or 0, -2) by D_00810C8C (with
  +0 = 1 / 2) and starts script 0x828120 on +0xB bit 2. The callback
  0x825C40 counts 80 frames, copies D_00810360 to D_008105E0 (+6 in y) and
  D_008105D0 to the record's +0x10, calls func_001FB9F0(0x451, 0x1000,
  0x1000, 0x1000); then +0x38 grows by 0.01 a frame up to 2.0 and is added
  to self +0xB4, func_00183010(D_008102B0, (0, +0x38, 0, 1)) runs each
  frame, and D_008105D0 is blended towards the record's +0x10 over 180
  frames; above D_00810364 810 it calls func_001AEDE0(4, 0) and
  func_001B0C60(0x11, 0, 0) (the AREA17 request of WORLD_GRAPH.md), and it
  returns 1 above 900.
- **[35] / [36] 0x823580.** A func_001CFB50 packet (group 0x8266E0) each
  frame, framed by func_0021B9A0(2, 0, 0) / (3, 0, 700) / (1, 0, 0).
- **The AREA01 family.** 0x823FC0 is the AREA19 0x827DD0 C with one change
  (a func_0019B6C0 miss gives effect 0x8000002C when the earlier hit record's
  +0x1A was 5, else 0x80000003); 0x825530 is the AREA13 0x826140 C with the
  func_001CD520 call spelled colour first with plain 3.0f sizes; 0x825A10 /
  0x825AA0 are the AREA19 0x829840 / 0x8298D0 C.

## Matching notes (mwcc 2.3.3)

- **idiom-24 int staging for a non-zero float.** In 0x823580 the 700.0 of
  func_0021B9A0(3, 0, 700) is staged through an int (`int k = 700; float y
  = (float)k;`), which puts f13 before f12 as the original does.
- **idiom-25 truthiness** for the `x - 1.0 == 0` test in 0x825C40 (`if
  (!x)`), giving the value as the first `c.eq.s` operand.
- **The area argument cast** `(void *)D_overlay_AREA14_00828400` keeps a2
  before a1 in func_001B1EA0 (AREA13 note).

### NEARMISS 0x823E30 (95.96%, C 0x10 longer)

The original fills the state-dispatch and test delay slots by moving the
successor's first instruction (the 0x3F argument and the 0x810C8C /
0x810702 address halves) and leaves no copy at the label; mwcc 2.3.3 fills
the same slots and re-emits each filler dead at the label (idiom-13b), four
extra words. Literal addresses for the three globals raised it from 94.75
(the extern spelling left the test slots empty). The 991202 build drops the
dead copies but fills the three jal slots differently (87.88); mwcc 2.4 gives
the same 95.96. Equivalence: the instruction-by-instruction diff
(`overlay_match.py check AREA14 .. --show`) shows only the dead copies and
the branch offsets they shift; calls, stores and control flow are equal.

Lane DMATCH (2026-10-02) swept it again (mwcc 991202 / 2.3.3 / 2.4): no
change; still NEARMISS (docs/LEVELS_DECOMP.md section 7).

## Verification

- `overlay_match.py check AREA14 src/overlays/AREA14/*.c`: 12 files 100.00
  BYTE-IDENTICAL, 0x823E30 95.96 (NEARMISS).
- `python3 tools/check_no_disassembly.py src/overlays/AREA14/*.c`: clean.
- Bounded mutation sweep (build/a03c/mut): 0x825FD0 221.6 -> 221.7 (99.99).
- Under the decomp build lock: `compile_overlay_src.py AREA14` (13 objects,
  1 NEARMISS skipped), then `tools/overlay/build.py --area AREA14
  --no-extract --no-yaml --no-splat`: PASS, AREA14.BIN equals the extracted
  file (20352 bytes, full file). Every linked filler object's `.text` equals
  the compiled `.text` outside relocation fields (build/a03c/prov.log).
  `config/overlays/AREA14.lds` was regenerated (absolute definitions of the
  new externs).

## Gate

See "Gate" in docs/AREA21_OVERLAY.md.

## Binding

The native port has no AREA14 module, and this lane edited nothing in the
port. For a port binding the C above is the ground truth; the NEARMISS body
is correct apart from the listed slot filling.

## Known gaps

- One NEARMISS function (above). The entry pad is an asm body.
- The data section (0x2300 bytes) is linked from splat's data assembly.
- No AREA14 capture exists; the area tests of [0] / [2] and the [6]
  callback's request are read from the code only.
- The verification cites git-ignored scratch in build/a03c and build/a14c
  (gate scripts, logs, prov.py, prov.log, hidden.py, mut/, try/); they must
  not be deleted while this doc cites them.
