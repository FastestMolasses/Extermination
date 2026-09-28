# AREA02 overlay — decompilation status (lane A02C, 2026-09-28)

> **Update 2026-09-28:** 008254E0 now links from C, so all 16 non-pad AREA02 functions link from compiled C. Statements below that call it NEARMISS describe the state before that fix.


AREA02 is the level after the AREA01 revisit (game order AREA11 -> AREA01 ->
AREA00 -> AREA01 with the bridge lowered -> AREA02). Its code module is
`OVERLAY/AREA02.BIN` (overlay id 3, text 0x2140, data 0x3C00, bss
0x281800). Like every overlay it is linked at 0x00823500 but runs 0x40
higher (the MWo3 header is loaded first), so splat and link names are the
runtime address minus 0x40, and an intra-overlay call lands 0x40 into the
callee. `tools/overlay/overlay_match.py list AREA02` regroups the 27 splat
pieces into 17 real functions (10 are split in two: the 9 whose later piece
the link absorbs, plus 008254E0/00825520; no dead pieces).

No AREA02 route capture exists yet, so every function was done; the order
was the file order.

## Status

Of the 17 functions, 16 have C and 1 is the asm entry pad. 15 link from
their compiled C objects; the 16th C file (008254E0) is byte-identical but
marked NEARMISS, so the link assembles it from its splat pieces (below).
The file is byte-identical (`tools/overlay/build.py --area AREA02
--no-extract --no-yaml --no-splat`: PASS, "16 copied from obj/" = the 15
compiled C objects plus the asm pad's object, one jump table pinned;
full-file comparison PASS). Every C file is mwcc 2.3.3
(`// COMPILER: mwcc233`) except the older area init 008238C0 (default
compiler); 00823500 is an asm pad, not C.

| link name | runtime | size | status |
|---|---|---|---|
| func_overlay_AREA02_00823500 | 0x823540 | 0x4 | asm pad (entry nop, unchanged) |
| overlay_AREA02_func_00823540 | 0x823580 | 0x380 | C, byte-identical (new) |
| func_overlay_AREA02_008238C0 | 0x823900 | 0x2C | C, byte-identical (earlier, area init, default compiler) |
| func_overlay_AREA02_008238F0 | 0x823930 | 0x50 | C, byte-identical (was hybrid asm) |
| func_overlay_AREA02_00823940 | 0x823980 | 0x3EC | C, byte-identical (new; absorbs 00823980) |
| func_overlay_AREA02_00823D30 | 0x823D70 | 0x2B0 | C, byte-identical (new) |
| func_overlay_AREA02_00823FE0 | 0x824020 | 0x2C8 | C, byte-identical (new; absorbs 00824020) |
| func_overlay_AREA02_008242B0 | 0x8242F0 | 0x510 | C, byte-identical with its jump table pinned by the link (new; absorbs 008242F0) |
| func_overlay_AREA02_008247C0 | 0x824800 | 0x104 | C, byte-identical (was hybrid asm; absorbs 00824800) |
| func_overlay_AREA02_008248D0 | 0x824910 | 0x1A8 | C, byte-identical (new; absorbs 00824910) |
| func_overlay_AREA02_00824A80 | 0x824AC0 | 0x178 | C, byte-identical (new; absorbs 00824AC0) |
| func_overlay_AREA02_00824C00 | 0x824C40 | 0x88 | C, byte-identical (was two hybrid asm pieces; absorbs 00824C40) |
| func_overlay_AREA02_00824C90 | 0x824CD0 | 0x80 | C, byte-identical (was hybrid asm for the second piece; absorbs 00824CD0) |
| func_overlay_AREA02_00824D10 | 0x824D50 | 0x244 | C, byte-identical (new; absorbs 00824D50) |
| func_overlay_AREA02_00824F60 | 0x824FA0 | 0x154 | C, byte-identical (new) |
| func_overlay_AREA02_008250C0 | 0x825100 | 0x420 | C, byte-identical (new) |
| func_overlay_AREA02_008254E0 | 0x825520 | 0x130 | C, links from C since 2026-09-28 (fill_overlay.py now absorbs the text-end zero pad; see FINDINGS 2026-09-28) |

Totals (17 functions): 15 link from compiled C (14 new or promoted in this
lane plus the earlier area init 008238C0), 1 byte-identical C file kept out
of the link by its NEARMISS marker (008254E0), 1 asm pad (00823500). The hybrid asm files for the pieces 00824C40 and 00824CD0 were
removed; their owners' C covers them.

The role lines in each source header describe what the instructions do.
They are not placement labels: which placement record uses which function
has not been captured.

### The last function (0x825520) and fill_overlay.py

`func_overlay_AREA02_008254E0` compiles to exactly the original 0x130 bytes
(`overlay_match.py check AREA02`: 100.00, BYTE-IDENTICAL). It is the
overlay's last function and covers two splat pieces. `fill_overlay.py`
(`plan_absorption`) lets a compiled object absorb the following pieces only
when it ends within 16 bytes of the pieces' slots, and the last slot runs on
through the 0x30-byte zero pad up to the text end (link 0x825640). The
check stops the fill. The file therefore starts with `// NEARMISS`, so
`compile_overlay_src.py` skips it and the link takes the function from its
two splat pieces; the overlay stays byte-identical. To link it from C,
`plan_absorption` must accept a remainder that is all text-end padding for
the last function. That tool is outside this lane; with the rule in place,
removing the marker should pass unchanged.

### Jump table

0x8242F0 dispatches kind 7's +6 through an 8-entry table at runtime
0x829200. `link_overlay.py` (with `tools/overlay/jt_pin.py`) placed the
compiled `.rodata` at link 0x8291C0 inside the data section and confirmed it
in the ld map ("1 compiled table(s) placed at their original addresses").

## Matching notes (mwcc 2.3.3)

- **`return;` after an inner switch stops jump threading (new).** In
  0x823D70, the blocks of an if / else-if chain that each hold a `switch
  (self[5])` must end with `return;`. With only `break`, mwcc threads the
  switch exits to the function end. The original branches to a join that
  holds a second branch to the end, and it has no dead copy of the
  flag-address upper half at the case-0 target. Adding `return;` gives
  both (93.43 -> 100). A small test confirmed it: with the same
  structure, `break` gives no branch-to-branch, while `return;` keeps
  both joins and drops the dead copy.
- **Flag bytes: literal or extern, per function.** D_00810761 and
  D_008107E1 are literal addresses in 0x823D70, where their upper half
  fills branch slots. They are externs in 0x823980, 0x824020 and 0x825100,
  where the slots stay empty.
- **Scratchpad vectors:** 0x823580 needs the 0x700038B0 stores through the
  extern (the literal lets mwcc hoist the upper half into the compare
  slots). 0x823980 needs all eight 0x700038A0/B0 stores through externs,
  in source order. 0x825100 stores both floats and words to 0x700038B0,
  so that vector is one extern union with a float view and a word view.
  Casting the float extern to `int *` for the word stores changes the
  addressing (95.51 -> 100 with the union).
- **One array for the position block (0x825520).** D_00810350, D_00810358
  and D_00810374 are written as `D_00810350[0]`, `[2]` and `[9]`. As three
  separate externs, mwcc moves the D_00810374 load above the D_00810358
  store. The 0x700031F0 store must be an extern, so the D_00810350 load can
  be scheduled above it.
- **idiom-31 in 0x8242F0.** The rate argument of two func_001B12B0 turns
  (kind 7 step 4 and kind 8 step 1) is `(float)one * K`; the original
  materializes the rate before the goal there and only there. The 900.0
  volume of the second 0x8B3/0x8B4 sound and the 300.0 volume of 0x8B5 are
  `(float)n` from block-local ints. The same function needs the counter
  pointer declared before the talk pointer (saved-register order).
- **A local array keeps a stack address in a saved register (0x824CD0).**
  The two transformed points are `Vec4 w[2]` and the second is used as
  `q = &w[1]`. Two separate locals, or a pointer to a separate local, are
  folded back to sp-relative adds.
- **Loop declaration order (0x824910):** declare the counter before the
  record pointer and initialise both in the `for`.
- **`+=` for the accumulate in 0x824AC0** (idiom-21 operand order).
- **0x823930** passes the masked and the raw +2 byte as extra arguments
  (a1, a2) to both callees. The callees read only a0, but the original
  computes the two values in those registers.
- **Model setup (0x824800)** is the func_001B0DC0 shape (comma-init `for`,
  `unsigned char n`), with `-sdatathreshold 2` so the bone cap D_00275BCC
  is gp-relative and D_0028A59C / D_0028A6E8 are absolute.
- **0x824D50** is written with the distance test as the outer `if` and a
  shared `return 0;`. The inverted early return leaves a filled delay slot
  that the original does not have.

## Cross-reference

The native port's `src/game/em_examine.c` cites AREA02 0x824FA0 as the
office examine: archetype 3, descriptor D_002758E0, script started and
pumped through func_001BA1A0/func_001BA1F0, re-armed on completion. The
byte-matched C of 0x824FA0 shows that shape. State 0 sets +8 = 3 and
+0x30 = &D_002758E0. State 1 sets +0 = 2 or 1 from func_001BA1C0(self, 9).
Sub-state +5 0 waits for +0xB bit 2 and starts script 0x827670. +5 1
clears +0xB and +5 when the script ends. The port file was not edited
(outside this lane).

## Verification

- `overlay_match.py check AREA02 src/overlays/AREA02/*.c`: 16 of the 17
  files at 100.00 BYTE-IDENTICAL (this count includes the asm pad
  00823500 and the NEARMISS 008254E0). The 17th, 0x8242F0, scores 99.99
  there; its only difference is the unresolved table address, which the
  link pins.
- `compile_overlay_src.py AREA02` + `build.py --area AREA02 --no-extract
  --no-yaml --no-splat` (under the decomp build lock): PASS, full file
  byte-identical, 16 objects copied from `obj/` (15 compiled C + the asm
  pad), 2 pieces assembled for 008254E0, 9 splat pieces absorbed.
  This regenerated `config/overlays/AREA02.lds`.
- `python3 tools/check_no_disassembly.py src/overlays/AREA02/*.c`: clean.
- Full `tools/decomp/build.py build` (rc 0, 2211 units) + `tools/verify_all.py`
  under the lock: all six stages PASS. The boot ELF is byte-identical,
  19/19 overlays pass, matched_code is 98.60% (2150/2211), and glTF,
  selftest and gs-offset pass. Overlay C is not an objdiff unit, so
  matched_code does not change.
