# AREA11 overlay — decompilation status (lane A11C, 2026-09-30)

## What AREA11 is

AREA11 is the first level: New Game starts there, and the port's first-level
work (the port's docs/FIRST_LEVEL_AUDIT.md) is measured against it. The code
module is `OVERLAY/AREA11.BIN`: overlay id 9, text 0x4C40, data 0x2B80, bss
0x140C00, file 0x7800 bytes. Like every overlay it is linked at 0x00823500
but runs 0x40 higher (the MWo3 header is loaded first), so splat and link
names are the runtime address minus 0x40. Data symbol names are runtime
addresses (a data address in the code reads the file byte at address -
0x823500). The boot area dispatcher calls the init at runtime 0x8237C0
(link 00823780).

Before this lane 23 of the 26 functions were assembly (`[asm]` or a hybrid
`asm void` body). The init 00823780 and the two script wrappers 008258C0 /
008258E0 were already C.

## Split handling

`overlay_match.py list AREA11` regroups the 34 splat pieces into 26
functions. Eight groups have a second piece: the intra-overlay call targets
0x823910, 0x823B70, 0x823C40, 0x825500, 0x825600, 0x8256D0, 0x826F30 and
0x827400 are function starts at runtime, so splat opened a piece 0x40 into
the function at the same link address. The compiled objects absorb them
(`[fill] 8 splat piece(s) absorbed by 8 compiled function(s)`). Route
censuses key overlay hits by splat piece, so a piece can be reported as a
"new function" when execution passes through it: the BRANCH lane's census
(docs/CAPTURES_C10.md, br_14) lists runtime 0x823BB0, which is splat's
piece `func_overlay_AREA11_00823B70`, 0x40 into the talk branch at runtime
0x823B70 (`func_overlay_AREA11_00823B30`), not a separate function. The port
keys it to its 0x823B70 translation. A scan of
every slot for code after a return (lane A15C's build/a15c/hidden.py) finds
no two-function slot.

One jump table: 0x823FF0 (the truck) dispatches `count & 15` through the
8-entry table at runtime 0x82ABC0. That function is NEARMISS (below), so the
table is linked from the data section as before; `jt_pin.py` pins nothing.

The C keeps splat's names for intra-overlay code references: calls go to
`func_overlay_AREA11_00823910` etc., and the flame stores the address of the
callback at runtime 0x823580 as `overlay_AREA11_func_00823540 + 0x40`
(declared as a `char[]`, so the addend folds into the address pair). The
checker `overlay_match.py` does not resolve the name `overlay_AREA11_func_*`,
so the lane checked with a wrapper that adds that pattern
(build/a11c/om.py); the link resolves it to the defining object.

## Status

26 functions: 23 are compiled C that links byte-identical (20 new in this
lane, plus the earlier init and the two wrappers). 2 are NEARMISS readable C,
linked from their splat .s. The entry pad stays an asm body (user decision
2026-09-23). The hybrid asm body of 00826EF0 was replaced by C.

| link name | runtime | size | status | reached by (static, `area_overview.py --area 11`) |
|---|---|---|---|---|
| func_overlay_AREA11_00823500 | 0x823540 | 0x4 | asm pad | - |
| overlay_AREA11_func_00823540 | 0x823580 | 0x6C | C, byte-identical | code pointer stored by 0x8235F0 at +0x34 |
| func_overlay_AREA11_008235B0 | 0x8235F0 | 0x1D0 | C, byte-identical | sub0 place[7] (flame) |
| func_overlay_AREA11_00823780 | 0x8237C0 | 0x20 | C, byte-identical (earlier; init) | boot area dispatcher |
| func_overlay_AREA11_008237A0 | 0x8237E0 | 0x130 | C, byte-identical | sub0 place[8] (Roger) |
| func_overlay_AREA11_008238D0 | 0x823910 | 0x254 | C, byte-identical (absorbs 00823910) | call from 0x8237E0 |
| func_overlay_AREA11_00823B30 | 0x823B70 | 0xD0 | C, byte-identical (absorbs 00823B70) | call from 0x8237E0 |
| func_overlay_AREA11_00823C00 | 0x823C40 | 0xA0 | C, byte-identical (absorbs 00823C40) | call from 0x8237E0 |
| func_overlay_AREA11_00823CA0 | 0x823CE0 | 0x1A0 | C, byte-identical | sub0 place[11] |
| func_overlay_AREA11_00823E40 | 0x823E80 | 0x168 | C, byte-identical | sub0 place[10] (opening) |
| func_overlay_AREA11_00823FB0 | 0x823FF0 | 0x11F0 | NEARMISS 99.99% | sub0 place[16] (truck) |
| func_overlay_AREA11_008251A0 | 0x8251E0 | 0x204 | C, byte-identical | sub0 place[17] |
| func_overlay_AREA11_008253B0 | 0x8253F0 | 0x10C | C, byte-identical | sub0 place[12] |
| func_overlay_AREA11_008254C0 | 0x825500 | 0xF4 | C, byte-identical (absorbs 00825500) | call from 0x8253F0 |
| func_overlay_AREA11_008255C0 | 0x825600 | 0xD0 | C, byte-identical (absorbs 00825600) | call from 0x8253F0 |
| func_overlay_AREA11_00825690 | 0x8256D0 | 0xD0 | C, byte-identical (absorbs 008256D0) | call from 0x8253F0 |
| func_overlay_AREA11_00825760 | 0x8257A0 | 0x154 | C, byte-identical | sub0 place[13] |
| func_overlay_AREA11_008258C0 | 0x825900 | 0x20 | C, byte-identical (earlier) | script 0x829E80 op09 |
| func_overlay_AREA11_008258E0 | 0x825920 | 0x20 | C, byte-identical (earlier) | script 0x829E80 op09 |
| func_overlay_AREA11_00825900 | 0x825940 | 0x15EC | C, byte-identical | sub0 deferred group 0x828180[7] (gun) |
| func_overlay_AREA11_00826EF0 | 0x826F30 | 0x4D0 | C, byte-identical (was hybrid asm; absorbs 00826F30) | call from 0x825940 |
| func_overlay_AREA11_008273C0 | 0x827400 | 0x84 | C, byte-identical (absorbs 00827400) | call from 0x825940 |
| func_overlay_AREA11_00827450 | 0x827490 | 0x198 | C, byte-identical | sub0 deferred group 0x828180[8] (cable) |
| func_overlay_AREA11_008275F0 | 0x827630 | 0x4DC | C, byte-identical | sub0 place[1], place[2] (fan pair) |
| func_overlay_AREA11_00827AD0 | 0x827B10 | 0x534 | C, byte-identical | sub0 place[19] |
| func_overlay_AREA11_00828010 | 0x828050 | 0x120 | NEARMISS 94.17% | script 0x82A750 op09 |

Every new file is mwcc 2.3.3 (`// COMPILER: mwcc233`, `-O4,p`,
`-sdatathreshold 0`). The exceptions are 0x823FF0, 0x825940 and 0x826F30,
which read the gp-relative D_00275B40 and use `-sdatathreshold 4`; they
declare the other small externs as arrays larger than 4 bytes so they stay
absolute. The names in parentheses are the port's names; each source header
describes what the instructions do.

## Shared code with AREA01

A normalised instruction comparison against every overlay with committed C
(build/a11c/sib.py) found AREA01 twins for four functions:

- 0x825940 (gun) is AREA01 0x826D40 with two differences: the
  func_001BA1C0 flag id is 0x30 (AREA01: 6), and when the second probe
  func_0019B6C0 fails, AREA11 spawns effect 0x8000002C for a hit face whose
  +0x1A kind is 5 and 0x80000003 otherwise (AREA01 always 0x80000003). The
  kind is read (as a signed 16-bit value) before the probe.
- 0x826F30 (probe) is AREA01 0x8282F0 except for instruction scheduling of
  the func_001CD520 call (see Matching notes).
- 0x827400 is AREA01 0x8287C0; 0x827490 (cable) matches AREA01 0x828850 in
  shape (it was written from the listing and matched directly).

## Behaviour (summary; the source headers list every branch)

- **0x8237E0 (Roger)** dispatches +4. State 1 picks 0x823910 while
  D_008107D8 is 0, 0x823C40 when its bit 7 is set, else 0x823B70. 0x823910
  sets D_008107D8 |= 1 at the end of script 0x8283D0 (started when the player
  enters the area 0x82AB80); 0x823C40 ends with func_001B0C60(1, 0, 4).
- **0x823FF0 (truck)**: state 4 waits until the player's ground actor
  (D_008104C4) has +0xD == 9 and D_008102BA is set, shakes for 46 frames
  through the jump table, then state 1 runs 119 frames of steps with dust
  effects and sounds 0x454 / 0x455, and sets D_00810792 = 0xFF. 0x8251E0
  starts camera script 0x8292C0 when the player is inside two x/z bands and
  sets D_00810792 = 1 at its end.
- **0x8253F0** runs the three beats 0x825500 / 0x825600 / 0x8256D0 by
  D_00810813 (0/1, 0x10/0x11, 0x20); each waits for the player's y and an
  area and advances D_00810813 to 0x10, 0x20, 0xFF.
- **0x827630 (fans)**: the unit +0x2E == 1 blows the player (+0x224 = 5,
  state 3) or calls func_001B0C60(1, 1, 4) / sets bit 7 of D_008107D8 from
  a box in front of it.
- **0x827B10** is a switch whose flag is bit +0x2E of
  D_00810841[D_00810700]; with the flag set its script end toggles
  D_0081083A and moves itself and the heights at 0x82A7C4 / 0x82AB14 /
  0x82A844 / 0x82A944 between 190 and 230. 0x828050 is the script callback
  that moves +0xB4, the player's y (D_00810354) and D_008105E4 by
  +-0.26666668 per frame for 150 frames.

## Matching notes (mwcc 2.3.3)

- **Switch compare order is the source case order reversed.** 0x8251E0
  (cases 0, 4, 1, 3/default) and 0x823FF0 (cases 0, 4, 1, 2, 3/default) test
  3 first and 0 last only with the cases written in that source order.
- **`1U << n` for a bit test** (0x827B10): `D_00810841[D_00810700] & (1U <<
  +0x2E)` gives the original register choice (the AREA04 0x823700 spelling);
  `1 <<` (99.40) and every operand order do not.
- **Prototype order between int and float arguments** (0x826F30): the
  original evaluates the packed colour before the three floats, so
  func_001CD520 is declared with the colour before them (the registers are
  the same either way). With that and an int-staged second float (idiom-31:
  `ti = 3; s = (float)ti;`) the function matches (92.96 -> 100).
- **`int k = (short)byte`** (0x825940): the original sign-extends the kind
  with a 64-bit shift pair once and compares the saved register; a `short`
  local re-extends at the compare.
- **Struct member for a counter** (0x8235F0): the count at +0x210 decrements
  through the +0x1F0 block pointer only when the block is a struct
  (`Flame *blk`); with `((int *)blk)[8]` mwcc forms a separate address.
- **`!x` for a flag toggle** (0x827B10): `D_0081083A = !D_0081083A` gives
  the original set-if-nonzero plus exclusive-or; `== 0` gives the other form.
- **Staged float operand** (0x823FF0): `x = F(0x2E0); F(0x100) = F(0xB0) +
  x;` and `x = F(0x2DC); x = F(0xB8) + x; F(0x108) = x;` give the original
  load order; the global add `D_00810350[2] += D_700038A0[2]` before the
  0x700031F0 store keeps the loads above that store.
- idiom-24 (0x823910, 0x823B70: the `20.0, 0` clip argument), the hoisted
  talk block (`talk = self + 0x1F0` at the top: 0x823B70, 0x823C40, ...),
  `unsigned short` 0xFFFF stores, and branch-arm duplication of the colour
  block (0x827B10) are the known levers.

### NEARMISS functions

- **0x823FF0 (99.99%).** One pair of argument moves in the count & 15 == 0
  case: the original sets a1 (self + 0x1F0) before the func_00102B08 call
  and a0 (self + 0xD0) in its delay slot; mwcc 2.3.3 sets a0 first. Casts
  on the second argument move it ahead of the float constant instead
  (99.82); locals, staged floats, pointer spellings and the shared block
  local do not reorder it. Everything else, the jump table included, is
  equal.
- **0x828050 (94.17%).** The original loads 1 into v0 again in the default
  case's branch slot (mwcc 2.3.3 reuses the 1 the state compare left there),
  keeps the 150-frame test in $at with a 0 / 1 return pair (mwcc selects
  with movn, or keeps the compare in v0), and assigns the FPRs of the two
  global adds differently. Every return shape with and without a result
  variable, the 27 spellings of the three adds, arrays, mwcc 2.4 and 991202
  were tried.
- Both are equivalent to the original: the instruction-by-instruction
  review (`overlay_match.py check --show` through build/a11c/om.py) shows
  only the residuals above; stores, calls, branch conditions and return
  values agree on every path.

## Verification

- `overlay_match.py check AREA11 src/overlays/AREA11/*.c` (through
  build/a11c/om.py): 24 files 100.00 BYTE-IDENTICAL (the pad, the 3 earlier
  files and 20 new), the 2 NEARMISS files as above.
- `python3 tools/check_no_disassembly.py src/overlays/AREA11/*.c`: clean.
- Bounded mutation sweep (single-constant edits in scratch copies,
  build/a11c/mut): 0x827630 wait 60 -> 61 (99.99), 0x8235F0 phase step
  0.025 -> 0.026 (99.98), 0x823CE0 flag 0x30 -> 0x31 (99.98). None stays
  byte-identical.
- Under the decomp build lock: `compile_overlay_src.py AREA11`, then
  `tools/overlay/build.py --area AREA11 --no-extract --no-yaml --no-splat`:
  PASS, `[fill] done: 0 code assembled, 24 copied from obj/, 2 cached`;
  AREA11.BIN equals the extracted file (30720 bytes, full file). Every linked
  object's `.text` equals the compiled `.text` outside relocation fields,
  zero-padded to its slot (build/a11c/prov.py). `config/overlays/AREA11.lds`
  was regenerated and gains only absolute definitions of new externs.

## Gate

One locked run (build/a11c/gate.sh full; lock acquired 15:19:07, done
15:34:06; log build/a11c/gate_full.log): `compile_overlay_src.py` and
`tools/overlay/build.py --area AREA11 --no-extract --no-yaml --no-splat`
("1/1 overlays passed"), full `tools/decomp/build.py build` (rc 0, 2211
units), then `tools/verify_all.py`: all six stages PASS. The boot ELF is
byte-identical (0x175b00 loadable bytes), 19/19 overlays pass, matched_code
is 98.62% (2152/2211), and glTF, selftest and gs-offset pass. Overlay C is
not an objdiff unit, so matched_code does not count this lane. Three source
headers were corrected after that compile (comments only), so the overlay
compile and link ran again under the lock with the final files (15:34:21 -
15:34:28, build/a11c/gate.log): PASS, full file byte-identical, and every
linked object equals its compiled `.text`.

## Cross-check against the port

Each function was compared with the port translation that claims its
runtime address (read only; the port was not edited). Agreement unless
listed:

| runtime | port translation | result |
|---|---|---|
| 0x8235F0 | em_area11_effect.c (+ em_area11_effect_runtime.c) | fixed in port A11FIX (was: agrees on the state machine; the port's runtime does not bind the loop sound (001FC3C0), the publication (001B17A0), the +0x30 / +0x34 stores or the 0x823580 callback (documented port gaps)) |
| 0x823580 | em_area11_effect_contact (em_area11_effect.c) | fixed in port A11FIX (was: agrees (returns 1 where the original spawns 0x80000027 itself); not called anywhere in the port (em_collision_world.c leaves the class-0xD +0x34 behaviour unported)) |
| 0x8237C0 | em_slg_008237C0 (em_startup_load_gaps.c) | agrees |
| 0x8237E0 | em_roger_actor_008237E0_init + em_roger_tick (em_roger.c) | agrees |
| 0x823910 / 0x823B70 / 0x823C40 | em_roger_tick (em_roger.c) | agrees |
| 0x823CE0 | em_flag30_manager_tick (em_security_gun.c) | agrees |
| 0x823E80 | tick_opening + em_area11_opening_state1 | fixed in port A11FIX (was **disagrees**: state 0 also calls em_pickup_prop_retire(actor->pos), which the original does not (its state 0 is 001B0FD0, 001C6380, +4 = 1, +0 = 1); +4 values other than 0..3 return silently in the original, the port faults (and 2 / 3 fault as "not bound")) |
| 0x823FF0 / 0x8251E0 | em_truck_original.c | agrees |
| 0x8253F0 / 0x825500 / 0x825600 / 0x8256D0 | em_director_original.c | agrees |
| 0x8257A0 | em_manager_008257A0.c | fixed in port A11FIX (was: state 1 (area 0x82ACA0, script 0x829E80, func_001DFE40, D_00810814 = 1) is not translated: the port faults) |
| 0x825940 | em_security_gun.c (0, 0x64, 2, 3) + em_security_gun_rest.c (4, 1) | agrees on lifecycles 0, 0x64, 2, 3, 4 and the fire path of 1 (flag 0x30, the kind-5 branch); the aim part of lifecycle 1 was not re-read line by line (it has the instruction oracle tools/test_security_gun_rest_reference.py) |
| 0x826F30 / 0x827400 | em_security_gun_rest.c (sight, shot) | agrees |
| 0x827490 | em_gun_cable_tick (em_security_gun.c) | agrees |
| 0x827630 | em_fan_original.c | agrees |
| 0x827B10 | em_elevator.c, em_indicator_child.c, em_area11_interaction_host.c | fixed in port A11FIX (was: agrees, with one possible difference: the original reads the flag bit D_00810841[D_00810700] & (1 << +0x2E) three times per tick (phase-0 script choice, phase-1 completion, the colour ramp after the +0x4C call); the port reads `powered()` once before the tick, so a callee that changes the bit within a tick would diverge) |
| 0x828050 | em_elevator_motion_tick (em_elevator.c) | agrees |
| 0x825900 / 0x825920 | em_area11_script_host (A11FIX) | bound as the script host's op09 callbacks |

Port headers that say "no decomp C exists" for 0x825940 / 0x827490 /
0x823CE0 (em_security_gun.h) are now out of date.

Port chain step A11FIX (2026-10-02, branch c11-t1, merged onto port main)
fixed every disagreement listed above and read every other translation line
by line against this C:

- 0x823E80 is now one translation, em_area11_opening_tick. Its
  em_pickup_prop_retire call is removed; states 2 / 3 free the record and
  other states return. An original-instruction oracle covers it (port
  `make test-area11-opening-reference`).
- 0x8257A0's state 1 is translated and bound: the quad 0x82ACA0, the script
  0x829E80 with its op09 callbacks 0x825900 / 0x825920, 001DFE40 and
  D_00810814.
- 0x8235F0 runs on its record. 001FC3C0 / 001FC520, 001B17A0 and the
  +0x30 / +0x34 stores are bound, and 0x823580 is the class-0xD +0x34
  behaviour. Its 001EFE00(0x80000027) at the player faults (the port's
  DAMAGE step).
- 0x827B10 reads its power bit at its three points.
- 0x823CE0 is bound.
- em_security_gun.h's note is corrected.
- Two more disagreements were found and fixed: 0x823FF0's arm-tick store of
  0x70003A20 was not modelled, and the port's label "director beat 3" for
  0x829E80 was wrong (0x8257A0 starts it).
- Left as fail-stops, both unreachable in the first level: 0x827B10's free
  states, and its 001B0FD0 refusal ordering (the original stores the
  heights before the call, the port after).

## Binding

This lane edited nothing in the port. The C above (the NEARMISS bodies
included) is the ground truth for the port's AREA11 owners.

## Known gaps

- Two NEARMISS functions (above); the entry pad is an asm body.
- The data section (0x2B80 bytes) is linked from splat's data assembly.
- The verification cites git-ignored scratch (build/a11c: om.py, var.py,
  sib.py, prov.py, gate.sh, gate.log, decomp_build.log); keep it while this
  doc cites it. Running `tools/area_overview.py --area 11` for the "reached
  by" column rewrote build/s87/area11/{overview.json,tables.md}.
