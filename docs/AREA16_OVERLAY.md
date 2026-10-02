# AREA16 overlay — decompilation status (lane A15C, 2026-09-28)

## What AREA16 is

AREA16 is behind AREA06's door [3]. SEVENTH_LEVEL_ROUTE.md gives that door
as leading to AREA16 entry 0, gated on bit 2 of D_00810847, which the AREA15
overlay sets (docs/AREA15_OVERLAY.md). It has not been played or captured.

The code module is `OVERLAY/AREA16.BIN`: overlay id 13, text 0x4F40, data
0x4080, bss 0x140C00, file 0x9000 bytes. Like every overlay it is linked at
0x00823500 but runs 0x40 higher, so splat and link names are the runtime
address minus 0x40. The boot area dispatcher func_001E7780 calls the init
at runtime 0x824420 (link 008243E0; boot symbol
`area_dispatch_off0F20_state1000`, from `area_overview.py --area 16`).

## Split handling

`overlay_match.py list AREA16` regroups the 37 splat pieces into 30 groups.
Two of them hold two real functions each. A scan for code after a return
inside a slot (build/a15c/hidden.py) finds them:

- **Slot 00825630 (0xC0 bytes, pieces 00825630, 00825660, 00825670,
  008256E0).** It holds the function at link 00825630 (runtime 0x825670,
  0x64 bytes) and a function at link 008256A0 (runtime 0x8256E0, 0x50
  bytes) that no splat piece starts. 0x824690 and 0x824E10 call it by its
  runtime address 0x8256E0, so splat opened a piece at link 0x8256E0 instead,
  near that function's end. The pieces 00825660 and 00825670 are splat splits
  at a branch and at the jal target 0x825670.
- **Slot 008256F0 (0xD0 bytes, pieces 008256F0, 00825730, 00825780).** It
  holds the function at link 008256F0 (runtime 0x825730, 0x44 bytes) and
  one at link 00825740 (runtime 0x825780, 0x74 bytes). The latter is the
  target of the jal to 0x825780 from 0x824E10.

Both files (`func_overlay_AREA16_00825630.c` and `..._008256F0.c`) define
both functions. Each function compiles to exactly the original bytes,
checked per `.text` section with build/a04c/chk3.py. fill_overlay.py
merges the two `.text` sections of each object (lane DFIX) and absorbs the
later pieces. The old hybrid asm body `func_overlay_AREA16_008256E0.c` held
only the epilogue of the 0x8256E0 function, so it was deleted.

Two other intra-text references are data labels inside functions:
`D_overlay_AREA16_00825260` (the runtime address of link 00825220, stored
as a behaviour by 0x824690) and `D_overlay_AREA16_00828200` (link
008281C0, stored by 0x8273F0 and 0x827B70). The C keeps splat's names. When
the defining piece is compiled C, link_overlay.py defines them as absolute
symbols.

AREA16 has no jump tables, so jt_pin.py has nothing to pin. Seven splat
pieces are absorbed by four compiled objects.

## Status

32 functions: 27 are compiled C that links byte-identical (25 new, plus the
earlier init 008243E0 and 00825620). 4 are NEARMISS readable C, linked from
their splat .s. The entry pad stays an asm body. The hybrid asm files
008255F0, 008256E0 (deleted), 008256F0 and 00826BE0 were replaced by C.

| link name | runtime | size | status | reached by (static) |
|---|---|---|---|---|
| func_overlay_AREA16_00823500 | 0x823540 | 0x4 | asm pad | - |
| overlay_AREA16_func_00823540 | 0x823580 | 0x214 | C, byte-identical | group 0x829750 x5 |
| func_overlay_AREA16_00823760 | 0x8237A0 | 0x208 | NEARMISS 98.27% | - (no static reference) |
| func_overlay_AREA16_00823970 | 0x8239B0 | 0x370 | C, byte-identical | - |
| func_overlay_AREA16_00823CE0 | 0x823D20 | 0x244 | NEARMISS 99.34% | - |
| func_overlay_AREA16_00823F30 | 0x823F70 | 0x1A0 | C, byte-identical | - |
| func_overlay_AREA16_008240D0 | 0x824110 | 0x308 | C, byte-identical | - |
| func_overlay_AREA16_008243E0 | 0x824420 | 0x20 | C, byte-identical (earlier; init) | boot 0x1E7780 |
| func_overlay_AREA16_00824400 | 0x824440 | 0xF8 | C, byte-identical | sub1 place |
| func_overlay_AREA16_00824500 | 0x824540 | 0x148 | C, byte-identical (absorbs 00824540) | call from 0x824440 |
| func_overlay_AREA16_00824650 | 0x824690 | 0x778 | NEARMISS 99.04% | sub1 place |
| func_overlay_AREA16_00824DD0 | 0x824E10 | 0x44C | C, byte-identical | groups 0x829660, 0x829750 |
| func_overlay_AREA16_00825220 | 0x825260 | 0xAC | C, byte-identical | code pointer in 0x824690 |
| func_overlay_AREA16_008252D0 | 0x825310 | 0x314 | NEARMISS 92.05% | sub1 place |
| func_overlay_AREA16_008255F0 | 0x825630 | 0x24 | C, byte-identical (was hybrid asm) | script 0x829970 op09 x2 |
| func_overlay_AREA16_00825620 | 0x825660 | 0xC | C, byte-identical (earlier) | call from 0x824440 |
| func_overlay_AREA16_00825630 | 0x825670 | 0x64 | C, byte-identical (two-function slot) | calls from 0x8256E0, 0x825780 |
| func_overlay_AREA16_008256A0 | 0x8256E0 | 0x50 | C, byte-identical (same file) | calls from 0x824690, 0x824E10 |
| func_overlay_AREA16_008256F0 | 0x825730 | 0x44 | C, byte-identical (was hybrid asm; two-function slot) | calls from 0x824690, 0x824E10 |
| func_overlay_AREA16_00825740 | 0x825780 | 0x74 | C, byte-identical (same file) | call from 0x824E10 |
| func_overlay_AREA16_008257C0 | 0x825800 | 0x998 | C, byte-identical | sub0 place |
| func_overlay_AREA16_00826160 | 0x8261A0 | 0x224 | C, byte-identical | sub0 place |
| func_overlay_AREA16_00826390 | 0x8263D0 | 0x674 | C, byte-identical | sub0 place |
| func_overlay_AREA16_00826A10 | 0x826A50 | 0x1CC | C, byte-identical (absorbs 00826A50) | call from 0x825800 |
| func_overlay_AREA16_00826BE0 | 0x826C20 | 0x2C | C, byte-identical (was hybrid asm) | scripts 0x82A1C0 / 0x82A3C0 op09 |
| func_overlay_AREA16_00826C10 | 0x826C50 | 0x3D8 | C, byte-identical | sub1 place x3 |
| func_overlay_AREA16_00826FF0 | 0x827030 | 0x3C0 | C, byte-identical | sub1 place x2 |
| func_overlay_AREA16_008273B0 | 0x8273F0 | 0x774 | C, byte-identical | sub0 place, sub1 place x15 |
| func_overlay_AREA16_00827B30 | 0x827B70 | 0x164 | C, byte-identical | sub1 place |
| func_overlay_AREA16_00827CA0 | 0x827CE0 | 0x2B4 | C, byte-identical | sub1 place x2 |
| func_overlay_AREA16_00827F60 | 0x827FA0 | 0x25C | C, byte-identical | sub1 place x5 |
| func_overlay_AREA16_008281C0 | 0x828200 | 0x204 | C, byte-identical | code pointer in 0x8273F0, 0x827B70 |

New files are mwcc 2.3.3 `-O4,p`, at `-sdatathreshold 0`. The exceptions
are 0x827030, 0x8263D0 and 0x8273F0, which read the gp-relative
D_00275B40 and use `-sdatathreshold 4`. They declare every other extern as
an array so it stays absolute. The "reached by" column is static
(`tools/area_overview.py --area 16`). The five effect functions
0x8237A0..0x824110 have no static reference in the placement, group,
script or call tables that the tool reads (they may be spawned through the
spawn-entry or nest tables).

## Behaviour (from the C)

- **0x825800** moves the player's height (D_00810358, with
  D_700031F0 = 1) and plays sound 0x924 and effect 0x826A50. In state 1
  it switches to sequence 0xC8 (script 0x82A3C0) when
  (D_0081080B & 7) >= 3, or else to sequence 0xC9 (script 0x82A1C0) when
  the height is below 455 with a downward speed (+0x2E4 < -0.055 with
  D_008102B5 == 1, or +0x2E4 < -1.35). 0xC9 returns to state 1 when its
  script ends. In 0xC8, sub-state 1 waits for +0x36 to become nonzero
  (after the rise script); it then goes to state 2 and sets
  D_0081080B = D_0081078B = 0xFF, +0x2E0 = 452 and +0x2DC = 5. State 2
  then emits effects.
- **0x827030** writes +0x84 = 5 and +0x80 = -2.5 into the
  D_00275B40[1..26] / [4..22] objects, adds a phase that turns with +0x2A,
  and moves the player's z by +-1/6 per frame inside one of two boxes
  chosen by +3.
- **0x8263D0** drives the D_00275B40[2] +0x80 value between -1.5 and 0 over
  the frames 121..139 of its script, and toggles bit 0 of D_0081080B.
- **0x824690** keeps three func_001C5570 attachments (kinds 0x31 / 0x32 / 0x33). When D_008104C4 is this object
  and D_00810809 < 2 it starts script 0x829970. At the script end it
  spawns a class 9 object with behaviour 0x825260 and a class 4 object with
  the boot behaviour func_001C4820. It then sets D_00810809 = D_00810789 =
  0xFF and calls func_001AF800 and func_001CB5B0(+9).
- **0x8273F0** moves the D_00275B40[2] +0x80 value by +0x2DC per frame for
  200 frames and keeps a class 8 child (behaviour 0x828200) at the same
  offset; 0x827B70 spawns the same child; 0x828200 draws 4, 4 or 16 parts
  from tables by +0xD. **0x827CE0 /
  0x827FA0** follow the +0x54-th +0x18 ancestor and spawn one record of
  the D_0024D820 table (area D_00810700, row +0x56, column +0xE).

## Matching notes (mwcc 2.3.3)

- **Declaration order as a register lever.** 0x8239B0 matched only after
  a search over declaration orders (build/a15c/perm.py: every permutation
  of the locals, including the `blk` pointer). Two orders out of 120
  byte-match.
- **One variable per use.** 0x824E10 needed a separate variable for the
  second attachment pointer (`c2`), which fixed the a0/a1 choice. 0x825800
  needed a separate pointer (`p2`) in state 2, which is also read for the
  0x700038A8 store. With the shared `p`, mwcc drops the pointer and
  addresses through self.
- **Chained stores.** In 0x825800 the D_00810360 -> D_008105E0 / D_00810200
  copies are `D_00810200[i] = D_008105E0[i] = D_00810360[i];`. That gives
  the original's descending FPR choice. 0x82A5E0/0x82A5F0 are one array
  (`D_overlay_AREA16_0082A5E0[0..6]`), which keeps the store order.
- **Compound assignment reloads.** `x -= c` reloads x, while `x = x - c`
  reuses the just-computed value (0x824110, 0x8239B0, 0x8263D0).
- **0x28-byte records.** The D_0024D7C0 lookups read the
  halfword at record * 0x28 + 0x2E, written as `[idx + 1].id` of a 0x28-byte
  struct whose id is at +6. The mask test in 0x827CE0 / 0x827FA0 is
  `(*(short *)(e + 4) & ~0xE0) == 2`.
- **`(short)*(int *)(self + 0x1F0)`** in 0x827030. The int direction is
  read back as its low halfword, which gives the original's halfword load.
- **Staged operands.** 0x827B70 stages `d = +0x2E0; d = +0xB4 - d`. 0x825800
  and 0x8237A0 use int-staged float arguments (4096.0 and 7.0). 0x823F70 /
  0x8239B0 use the AREA00 `r = (float)...; r = r / 65535.0f; r += 0.0001f;
  seed = seed * 37 + 11;` random idiom. The spawner 0x824540 copies its
  six-point area with a 16-byte-aligned struct (the AREA00 0x825E80 idiom).
- The `+ 0x1F0` pointer idiom (`(int *)(self + 0x1F0) + k` assigned to a
  local) reproduces the original's separate add-then-offset address in
  0x824690, 0x8273F0, 0x825800, 0x824E10, 0x8263D0 and 0x825730.

### NEARMISS functions

- **0x8237A0 (98.27%).** The state byte is loaded into a2 before the self
  copy in the original, and into a0 after it in mwcc 2.3.3. Extra
  parameters, a switch local and every declaration order fail to change
  it.
- **0x823D20 (99.34%).** Saved-register assignment in the state-1 loop. All
  5040 declaration orders and separate loop variables were tried.
- **0x825310 (92.05%).** List scheduling in the state-1 packet loop: the
  float constants and the two 0x82A140 / 0x82A148 stores are ordered
  differently. 720 declaration orders, statement orders and a chained
  store were tried.
- **0x824690 (99.04%, 0x10 short).** The original keeps an explicit compare
  for D_00810809 == 1 that branches to the default body. mwcc 2.3.3 merges
  case 1 into default for every label order tried. The original also pads
  the D_70003B88 loop with one nop.
- **Equivalence evidence.** The lane's scratch multiset audit
  (build/a15c/audit.py: registers normalised, branches other than jal,
  nops and FP/GPR moves dropped) cannot see branch conditions, targets or
  register data flow, so it does not show equivalence. The evidence is the
  round-7 review, which diffed each of the four against the original
  instruction by instruction (re-run at the round-7 close with
  `overlay_match.py check --show`, from a copy whose work directory is
  scratch). Each body is equivalent: 0x8237A0 is a consistent register
  swap (the state byte's register and the self copy's order); 0x823D20 a
  consistent swap of two saved registers plus one moved copy; 0x825310
  store and constant reordering with different register choices in the
  packet loop; 0x824690 the case-1 compare that goes to the default body,
  the loop's nop and the branch-offset shifts they cause.
- **Lane DMATCH (2026-10-02).** 0x8237A0: a volatile state read does not
  move the state byte's register; 0x824690: case 1 with its own copy of the
  default body is not merged (91.81-98.06); 0x823D20 and 0x8237A0 got
  500 s of the decomp-permuter each. All four stay NEARMISS
  (docs/LEVELS_DECOMP.md section 7).

## Verification

- `overlay_match.py check AREA16 src/overlays/AREA16/*.c`: 23 single-function
  files 100.00 BYTE-IDENTICAL. The two-function files are checked per
  function (build/a04c/chk3.py): 4 x BYTE-IDENTICAL. The earlier 008243E0
  and 00825620 are also byte-identical. The 4 NEARMISS files score as above.
- `python3 tools/check_no_disassembly.py src/overlays/AREA16/*.c`: clean.
- Bounded mutation sweep: 0x827030 +0x1F4 init 1/6 -> 0.2 (99.99),
  0x828200 part count 16 -> 15 (99.99). Neither stays byte-identical.
- Under the lock: `compile_overlay_src.py AREA16`, then
  `tools/overlay/build.py --area AREA16 --no-extract --no-yaml --no-splat`:
  PASS. AREA16.BIN equals the extracted file (36864 bytes). Every filler
  `.text` equals the compiled `.text` outside relocation fields. For the
  two-function objects this holds after laying out both sections at
  16-byte alignment (build/a15c/prov.py). `config/overlays/AREA16.lds` was
  regenerated and gains only the absolute definitions of new externs.

## Gate

One locked run (build/a15c/gate.sh full; lock acquired 21:54:11, done
22:06:29; log
build/a15c/gate_full.log): `compile_overlay_src.py` and `tools/overlay/build.py
--area ... --no-extract --no-yaml --no-splat` for AREA15 and AREA16 (both
"1/1 overlays passed"), full `tools/decomp/build.py build` (rc 0, 2211
units), then `tools/verify_all.py`: all six stages PASS. The boot ELF is
byte-identical (0x175b00 loadable bytes), 19/19 overlays pass, matched_code
is 98.62% (2152/2211), and glTF, selftest and gs-offset pass. Overlay C is
not an objdiff unit, so matched_code does not count this lane. The source
headers were reworded after that compile step (comments only), so the
overlay compile and link were run again under the lock with the final
files: AREA15 and AREA16 PASS, full files byte-identical.

## Binding

The port has no AREA16 module, and this lane edited nothing in the port.
The C above (the NEARMISS bodies included) is the ground truth for a
binding.

## Known gaps

- Four NEARMISS functions (above). The entry pad is an asm body.
- The data section (0x4080 bytes) is linked from splat's data assembly.
- AREA16 is not captured. No placement was seen running. The spawn origin
  of the five effect functions 0x8237A0..0x824110 is open.
- The four NEARMISS files are registered in `docs/NEARMISS.md` (overlay
  rows, added at the round-7 close).
- The verification above cites git-ignored scratch scripts
  (build/a15c/{hidden,prov,audit,perm,gate}.py/.sh, build/a15c/gate_full.log
  and lane A04C's build/a04c/chk3.py). They are receipts a committed doc
  cites, so they must not be deleted. Moving the reusable checks (the scan
  for code after a return inside a slot, and the per-.text check of
  two-function objects) into tools/overlay/ is open.
