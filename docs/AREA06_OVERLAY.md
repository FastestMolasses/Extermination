# AREA06 overlay — decompilation status (lane A06C, 2026-09-28)

## What AREA06 is

AREA06 is the snow area whose door [19] in AREA01 (upper floor, entry 6)
leads to AREA06 entry 0 (the port's docs/SIXTH_LEVEL_ROUTE.md; game order
AREA11 -> AREA01 -> AREA00 -> AREA01 with the bridge lowered -> AREA02 ->
AREA04 -> AREA22 -> AREA01 entry 6 -> door [19] -> AREA06). It is not part
of the first level: docs/FINDINGS.md ("Area 6 != Area 11") records that
AREA06 renders chunk10 and AREA11 chunk15, and that the old port
`scene_snow` had wrongly taken its pickups, light rig, camera region and one
examine object from AREA06. Those were removed there in s78. The "AREA06
part" examine row in FINDINGS (record [6], behaviour 0x824340) is AREA06's
own wall switch. The first level does not run it.

The code module is `OVERLAY/AREA06.BIN`: overlay id 6, text 0x2AC0, data
0x2F00, bss 0x14AC80, file 0x5A00 bytes. Like every overlay it is linked at
0x00823500 but runs 0x40 higher (the MWo3 header is loaded first). Splat and
link names are therefore the runtime address minus 0x40. A data address in
the code reads the file byte at (address - 0x823500) at runtime. The boot
area dispatcher func_001E7780 calls the init at runtime 0x824290 for keys
0x600 and 0x601 (sub-states 0 and 1). In the boot symbol list that address
is named `area_dispatch_off0D90_state0600`.

## Split handling

AREA06 needs none. The 10 splat pieces are the 10 real functions
(`overlay_match.py list AREA06`). No overlay code calls another overlay
function, so splat made no fake split. There is no jump table
(`jt_pin.py` has nothing to pin) and no multi-function slot. The only
intra-overlay code address is a data reference. func_overlay_AREA06_00823B10
stores the value 0x823B10 at +0x34 of its record. Splat names that value
after the piece linked there (00823B10 itself), but at runtime 0x823B10
holds func_overlay_AREA06_00823AD0. The C keeps splat's name, as the
AREA04 files do for intra-overlay references, and the header says what
runs there.

## Status

9 of the 10 functions are compiled C that links byte-identical. One is
NEARMISS readable C, and the entry pad stays an asm body (user decision
2026-09-23).

| link name | runtime | size | status | reached by (arrival capture) |
|---|---|---|---|---|
| func_overlay_AREA06_00823500 | 0x823540 | 0x4 | asm pad (entry nop, unchanged) | - |
| overlay_AREA06_func_00823540 | 0x823580 | 0x58C | C, byte-identical (new) | sub0 place[9], sub1 place[28]; live |
| func_overlay_AREA06_00823AD0 | 0x823B10 | 0x40 | C, byte-identical (new; replaces the hybrid asm body) | code pointer stored by 0x823B50 |
| func_overlay_AREA06_00823B10 | 0x823B50 | 0x58C | NEARMISS C, 97.12% (linked from splat .s) | sub0 place[8], sub1 place[27]; live |
| func_overlay_AREA06_008240A0 | 0x8240E0 | 0x1A4 | C, byte-identical (new) | no placement; two live spawned nodes |
| func_overlay_AREA06_00824250 | 0x824290 | 0x30 | C, byte-identical (earlier, default compiler) | boot func_001E7780, keys 0x600/0x601 |
| func_overlay_AREA06_00824280 | 0x8242C0 | 0x80 | C, byte-identical (new) | script 0x826D40, op09 record 0x826EC0 |
| func_overlay_AREA06_00824300 | 0x824340 | 0x214 | C, byte-identical (new) | sub0 place[6]; live at (-306, 68, -650) |
| func_overlay_AREA06_00824520 | 0x824560 | 0x18BC | C, byte-identical (new) | sub0 place[11]; live at (-239.9, 50, -584) |
| func_overlay_AREA06_00825DE0 | 0x825E20 | 0x178 | C, byte-identical (new) | sub1 place[44] |

Every new file is mwcc 2.3.3 (`// COMPILER: mwcc233`, `-O4,p`,
`-sdatathreshold 0`, except `-sdatathreshold 4` for 00824300). The
"reached by" column comes from `tools/area_overview.py --area 6 --ram`
over the AREA06 arrival capture (see Verification). Each source header
describes what the instructions do. Header roles are not placement labels
and were not played.

## Behaviour (from the byte-matched C)

- **0x824290 (init)** sets D_00275C2C = 1, D_00275C28 = 0x20, D_00275C20 =
  0x828F00 (the first byte after the file image, where .bss begins),
  D_00275C24 = 0 and D_00275C1C = 0x832F80 (.bss + 0xA080). Unlike AREA22's
  init, it sets up a one-record D_00275C20 array, as AREA04's does.
- **0x824340** is the examine wall switch recorded in docs/FINDINGS.md (use
  desc D_00275940 = {10, 10}). Bit 5 of D_00810845 selects script 0x827040;
  when it is clear the switch uses 0x826D40. With the bit set and
  D_00810C87 != 0 the switch goes inert (sub-state 2). When armed (bit 2
  of +0xB) it calls func_001B6F00(self, (0, 0, 9.5, 1) at 0x700038A0,
  pi) and waits for the script. It re-primes after each script. With the bit set
  it first calls func_001C4760(7, 1) once while D_00810CCA == 0.
- **0x8242C0** is called from script 0x826D40. Step 0 sets D_008106C5 = 1.
  Step 1 calls func_001FABB0 and func_001FB0B0(0) when bit 5 of D_00810845
  is set, then returns 1.
- **0x823580** and **0x823B50** are two timed spawners. They run from
  records [9] and [8] in sub 0 and from [28] and [27] in sub 1. The first
  alternates a 240-frame calm phase and a 90-frame active phase
  (func_0019C6F0(0xD, 0/1) at each change). It spawns class 0x80000042 at
  the runtime point (-255, 20, -635). In the active phase it also spawns
  class 0x8000003B with a matrix built from the offset between that point
  and a random point in x -255..-225, z -765..-595 (func_001028D0,
  func_00102760, func_001CD390), passing the offset length at +0x1F4.
  It also plays random sounds 0x41D..0x41F and calls func_001EA210(0, 2.0f).
  The second cycles three phases with period tables {150, 240, 120},
  {90, 90, 60} and {90, 120, 180}. It spawns classes 2, 3 and 4 at one of
  three points near (-353, 75, -611). Phase 2 also spawns class 0x8000003B.
  In phase 2 its +0 byte is 2 while the 60-frame timer that the 0x823B10
  callback sets is running, else 1.
- **0x8240E0** animates a spawned object through func_001D04B0 with one of
  the tables 0x826B00 / 0x826B90 / 0x826C20 (+0x826CB0), chosen by +0xD. Its
  parameter runs 0 to 1.1 in 0.015 steps, and then the object ends (state
  3). Two such nodes were live in the arrival capture and neither came
  from a placement record. A class-2/3/4 spawn by 0x823B50 is one possible
  origin, but this was not traced.
- **0x824560** (6.3 KB) is a record-driven object. It reads its pose and
  model id from its own placement record. It watches two 4-point polygons
  (runtime 0x827640 / 0x827680) against the player position D_00810350
  with func_001B1EA0 (player y >= 55). State 4 swings the object (+0x2DC
  phase) and starts 240-frame runs with class 0x8000001F spawns. In state
  1 sub 0, the polygon-2 trigger (with D_008106B9 == 0 and D_008102B5 < 2
  or in 29..34) starts script 0x827180. It also sets D_00810768 = 1, saves
  the player's y/z, and places the player at (-270, 55, -583) facing +-pi/2.
  It plays func_001FBD50(self, 0x8CC, 0, 300.0f). Sub 1 then drives
  D_00810354 (player y) from the object's motion. It counts down and then
  up through fixed spawn points (0x80000015 / 0x80000021), and ends by
  restoring the pose and going to state 2 (func_001AF800,
  func_001CB5B0(+9)). State 2 sets D_00810768 = 0xFF and spawns six
  0x80000015 objects around the object. The source header lists every
  branch. None of this was played.
- **0x825E20** (sub1 record [44]) waits for func_001BA1C0(self, 0x31) with
  D_70003B8D != 4. It then runs script 0x8276C0, and at the end sets
  D_00810802 = 0xFF and bit 4 of D_00810845 and calls func_001FB0B0(0x15)
  and func_001C4760(0x1F, 1). An initial func_001BA1C0(self, 0x2A) makes
  it inert.

## Matching notes (mwcc 2.3.3)

- **Placement record arithmetic (0x824560).** The record address must be
  written as int arithmetic,
  `(unsigned char *)(self[0x9A] * 0x28 + (int)D_0024D7C0[D_00810700][D_00810701])`.
  Pointer forms (`row + uid * 0x28`, `&row[uid * 0x28]`, a 0x28-byte
  struct index) put the add operands in the other order (91.03%) or
  schedule the row load differently (97.36%).
- **Pointer before the copies (0x824560).** `blk = self + 0x1F0;` must come
  before the two 64-byte polygon copies. Otherwise the copy temporaries
  get different registers and the switch constant moves (99.71 -> 100).
- **Relocated scratchpad externs, per access (idiom-32).** In 0x824560, the
  state-4 `D_700038A0[0] -= 46.0f` and the state-1 `D_70003680 = ...` store
  must be relocated externs. The literal spelling stores before the
  (non-aliasing) area-byte loads (97.69 -> 98.13 -> 99.71). In 0x823580
  the `% 100` test reads `D_70003B68` as an extern. The literal is copied
  into a delay slot.
- **Player y extern (0x824560 state 1 sub 0).** The `y < 55` test reads
  `D_00810354` as an extern (the literal `lui` is speculated into the sub
  dispatch slot). The other D_0081035x accesses stay literal.
- **idiom-28**: `D_008102B5 <= 1` and `*cnt > 1` take `$at`, as the original
  does.
- **0x823AD0** passes its second argument through to func_001EFE00
  (`func_001EFE00(0x80000044, arg)`). The original never sets $a1; the
  first try, which passed `self`, compiled 4 bytes longer (0x44).
- **0x824340** writes the scratch vector through literal addresses. It
  passes the relocated `D_700038A0` as the argument. Array-element stores
  compute a base register (96.70%).

### The NEARMISS: func_overlay_AREA06_00823B10 (97.12%)

The rest of the function matches. Its case-1 and case-2/3 exits are
`return`, the reverse of idiom-27. `break` there suppresses the original's
delay-slot speculation of the case-0 dispatch, the `% 48` test and the
`% 4` sound switch (94.52 -> 96.83). `fired = 1;` goes after the
class-0x8000003B check (96.83 -> 97.12). The remaining difference is the
prologue. The three 12-byte period tables are copied to stack arrays with
one 64-bit load/store and one 32-bit word. The original moves that word
through a general register. mwcc 2.3.3, 2.4 and 991202 move it through an
FPR. They do so for every spelling tried: struct assignment, a local
initializer, `const`, a union, `#pragma pack(4)` with a `long long` head,
an 8-byte-aligned struct, a `volatile` member, and a 64-bit field plus int
field copy, which gives the right loads but separate address registers.
An element-wise int copy gives three words. The register choice of the
copy shifts with it. A scan of every splat file (boot and 19 overlays)
finds no other 12-byte copy of this shape, matched or not, so no matched
sibling can pin the spelling. The file is committed as readable C with a
`// NEARMISS` first line. `compile_overlay_src.py` skips it, and the
overlay links that function from its splat .s.

## Verification

- `overlay_match.py check AREA06 src/overlays/AREA06/*.c`: 9 files
  100.00 BYTE-IDENTICAL, `func_overlay_AREA06_00823B10` 97.12 (NEARMISS).
- `python3 tools/check_no_disassembly.py src/overlays/AREA06/*.c`: clean.
- Mutation sweep (bounded, four single-constant edits in scratch copies
  under build/a06c/mut): 0x824560 phase step 0.08 -> 0.09, 0x823580
  `% 50` -> `% 51`, 0x824340 9.5 -> 9.0, 0x825E20 script id 0x31 -> 0x32.
  None stays byte-identical (objdiff 100.00 / 100.00 / 99.99 / 99.98, all
  without BYTE-IDENTICAL; objdiff rounds immediate-only changes).
- Under the decomp build lock: `compile_overlay_src.py AREA06` (8 C objects
  compiled, the NEARMISS skipped) + `tools/overlay/build.py --area AREA06
  --no-extract --no-yaml --no-splat`: PASS, and the rebuilt AREA06.BIN
  equals the extracted file (23040 bytes, `cmp`). Every linked filler
  object's `.text` equals the compiled `.text`. Seven are equal outright
  or with zero padding to their slot. 0x824290 differs only in its five
  pre-applied GPREL16 store fields, and 0x824340 only in its two
  gp-relative `&D_00275940` fields. The 00823B10 filler is the splat
  assembly. This regenerated `config/overlays/AREA06.lds`, which gains
  only the absolute definitions of the new externs.
- Captured RAM (build/s87/route_a01u/a01u_02_progression_exit, the AREA06
  arrival from AREA01's door [19]: area bytes 6/0, recorded by the
  a01u capture lane): the loaded image at 0x823500 equals AREA06.BIN
  (23040 bytes). D_00275C2C = 1, D_00275C28 = 0x20, D_00275C20 = 0x828F00,
  D_00275C24 = 0 and D_00275C1C = 0x832F80, which are the values the init
  C writes. D_00275C18 = 0. `area_overview.py --area 6 --ram` finds six
  live pool nodes running overlay code: 0x824340 (place[6]), 0x823B50
  (place[8]), 0x823580 (place[9]), 0x824560 (place[11]) and two 0x8240E0
  nodes. The capture is a single RAM image, not a trace.
- Full `tools/decomp/build.py build` + `tools/verify_all.py` under the
  lock: see "Gate".

## Gate

One locked run (build/a06c/gate.sh, 17:39-17:54): `compile_overlay_src.py
AREA06` (rc 0), `tools/overlay/build.py --area AREA06 --no-extract --no-yaml
--no-splat` ("1/1 overlays passed"), full `tools/decomp/build.py build`
(rc 0, 2211 units), then `tools/verify_all.py`: all six stages PASS. The
boot ELF is byte-identical (0x175b00 loadable bytes), 19/19 overlays
pass, matched_code is 98.62% (2152/2211), and glTF, selftest and gs-offset
pass. Overlay C is not an objdiff unit, so matched_code does not count
this lane.

## Binding

The native port has no AREA06 module, and this lane edited nothing in the
port. For a port binding, the C above is the ground truth. The init's five
globals match what the port already writes for other areas
(`src/game/em_area02_overlay.c`, `em_startup_load_gaps.c`). The examine
switch at 0x824340 is the object that FINDINGS once exported into
`scene_snow` by mistake. 00823B10 is NEARMISS, and its C is body-correct
except for the table-copy instruction choice. Its behaviour (the period
tables and spawns) needs no special handling.

## Known gaps

- func_overlay_AREA06_00823B10 is NEARMISS (above). The entry pad is an asm
  body.
- The data section (0x2F00 bytes: scripts 0x826D40 / 0x827040 / 0x827180 /
  0x8276C0, the placement tables 0x827AC0 / 0x8283D0, the polygons and
  point tables) is linked from splat's data assembly, as in every overlay.
- The roles were read from the instructions. Apart from the arrival
  capture's live-node list, no AREA06 behaviour was played or traced. In
  particular, the origin of the two live 0x8240E0 nodes and the meaning of
  func_0019C6F0(0xD, n), func_001EA210 and the D_00810768 / D_00810802 /
  D_008106C5 writes are open.
- `docs/NEARMISS.md` (the boot registry) was not edited (lane scope). The
  NEARMISS is recorded here.
