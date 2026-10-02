# AREA17 overlay — decompilation status (lane OVLC, 2026-10-01)

## What AREA17 is

AREA17 is reached from AREA14 (its [6] script callback 0x825C40 requests
AREA17 entry 0) and leaves to AREA11 entry 3 through [38] 0x825470; its
NPC [34] 0x824700 takes item 0x28 and gives item 0x2B, and it writes the
counters / flags 0x2D..0x30 (docs/WORLD_GRAPH.md, the AREA17 table and
section 7 step 7, inferred, not played). One sub-state, lock byte
D_00810852. This lane did not run the game; the behaviour below is read
from the C.

The code module is `OVERLAY/AREA17.BIN`: overlay id 14, text 0x2D40, data
0x2C00, bss 0x140C80, file 22912 bytes. Linked at 0x00823500, it runs 0x40
higher (link names are the runtime address minus 0x40; data symbols carry
their runtime address). The boot area dispatcher func_001E7780 calls the
init at runtime 0x823B80 (link 00823B40, earlier C).

## Split handling

`overlay_match.py list AREA17` finds 24 functions in 24 splat pieces; no
fake splits (no function is called 0x40 into its body) and no jump table.

## Status

24 functions: 22 are compiled C that links byte-identical (20 new in this
lane, plus the earlier init 00823B40 and the script callback 00824240), 1 is
NEARMISS readable C linked from its splat .s, and the entry pad stays an
asm body.

| link name | runtime | size | status | reached by (static, area_overview.py --area 17) |
|---|---|---|---|---|
| func_overlay_AREA17_00823500 | 0x823540 | 0x4 | asm pad | nop sled, no code |
| overlay_AREA17_func_00823540 | 0x823580 | 0x58 | C, byte-identical | - (no static reference) |
| func_overlay_AREA17_008235A0 | 0x8235E0 | 0x110 | C, byte-identical | - (no static reference) |
| func_overlay_AREA17_008236B0 | 0x8236F0 | 0x110 | C, byte-identical | - (no static reference) |
| func_overlay_AREA17_008237C0 | 0x823800 | 0x58 | C, byte-identical | - (no static reference) |
| func_overlay_AREA17_00823820 | 0x823860 | 0x168 | C, byte-identical | - (no static reference) |
| func_overlay_AREA17_00823990 | 0x8239D0 | 0x1A4 | C, byte-identical | group 0x826890 [4]..[7] |
| func_overlay_AREA17_00823B40 | 0x823B80 | 0x20 | C, byte-identical (earlier; init) | boot 0x1E7780 |
| func_overlay_AREA17_00823B60 | 0x823BA0 | 0x3CC | C, byte-identical | sub0 place [30] |
| func_overlay_AREA17_00823F30 | 0x823F70 | 0x310 | C, byte-identical | script 0x8270A0 op09 record 0x827220 |
| func_overlay_AREA17_00824240 | 0x824280 | 0x4C | C, byte-identical (earlier) | script 0x8270A0 op09 record 0x8272E0 |
| func_overlay_AREA17_00824290 | 0x8242D0 | 0x104 | C, byte-identical | sub0 place [39] |
| func_overlay_AREA17_008243A0 | 0x8243E0 | 0x1B4 | C, byte-identical | sub0 place [31] |
| func_overlay_AREA17_00824560 | 0x8245A0 | 0x158 | C, byte-identical | sub0 place [33] |
| func_overlay_AREA17_008246C0 | 0x824700 | 0x5F0 | C, byte-identical | sub0 place [34] |
| func_overlay_AREA17_00824CB0 | 0x824CF0 | 0x150 | C, byte-identical | sub0 place [42] |
| func_overlay_AREA17_00824E00 | 0x824E40 | 0x29C | C, byte-identical | sub0 place [40] |
| func_overlay_AREA17_008250A0 | 0x8250E0 | 0xCC | C, byte-identical | sub0 place [41] |
| func_overlay_AREA17_00825170 | 0x8251B0 | 0x2BC | C, byte-identical | group 0x827F40 [4] |
| func_overlay_AREA17_00825430 | 0x825470 | 0xE4 | C, byte-identical | sub0 place [38] |
| func_overlay_AREA17_00825520 | 0x825560 | 0x934 | NEARMISS 99.41% | sub0 place [5], [6], [7], [8] |
| func_overlay_AREA17_00825E60 | 0x825EA0 | 0x214 | C, byte-identical | sub0 place [36] |
| func_overlay_AREA17_00826080 | 0x8260C0 | 0xA4 | C, byte-identical | sub0 deferred group 0x826280 [4] |
| func_overlay_AREA17_00826130 | 0x826170 | 0xBC | C, byte-identical | sub0 place [37] |

Every new file is mwcc 2.3.3 (`// COMPILER: mwcc233`, `-O4,p`,
`-sdatathreshold 0`). Each source header describes what the instructions do.

## Behaviour (from the C)

- **[30] 0x823BA0, the arrival sequence (counter 0x2D).** Spawns script
  0x826A20; D_00810805 then drives it: group 0x8263C0 into D_008106C0
  when the counter is first set, group 0x826420 / func_001F6B60 /
  func_0019C6F0(0xE / 0xF, 1) and D_00810805 = 3 at 2, script 0x8270A0
  at 8. A skip (func_001BA1F0 bit 1) jumps to the same end state at once:
  D_00810805 = 4, the player at (156.5, 220, 300) heading 2.1118484 and the
  camera D_008105D0 / D_008105E0 at (116.4, 259, 324.1) / (156.5, 237, 300).
  The callback 0x823F70 of script 0x8270A0 circles the camera around the
  midpoint of the player and the point 0x827520 (0.008726646 rad a frame,
  rising 0.1) and calls func_001AEDE0(1, 1) after 380 frames.
- **[34] 0x824700, the exchange NPC.** With D_00810786 == 1 (set by [33]
  0x8245A0 when, after flag 0x2D, the player enters the area 0x827530) it
  runs script 0x8275E0 and then sets D_00810806 = 0xFF. Otherwise
  D_00810807 sequences it: 0, the player in the area 0x8283A0, script
  0x827A40, func_001C47E0(0x28, 1) (item 0x28 taken), D_00810807 = 1;
  1, on talk script 0x827E00; 2 / 3, script 0x828050 with D_00810787 = 1,
  then func_001C47A0(0x2B, 1) (item 0x2B given), func_001C4760(0x19, 1),
  group 0x826560, D_00810807 = 0xFF.
- **[36] 0x825EA0** reacts to talk with D_00810807 = 2; at D_00810807 == 3 it
  counts past 139 frames, firing on the 140th count (or until D_00810787 is 0xFF) and then frees the child
  it created with func_001C5570.
- **[38] 0x825470, the exit.** Once D_00810835 is set: script 0x8284F0,
  then func_001B0C60(0xB, 0, 3) (AREA11 entry 3).
- **[42] 0x824CF0** shows the func_001F5940 marker 7 at a fixed offset for
  frames 567..745 and 1106..1124 of the D_00810806 == 1 phase; [40]
  0x824E40 the marker 1 at six fixed points; [31] 0x8243E0 / [39] 0x8242D0
  / [41] 0x8250E0 / [37] 0x826170 are flag-gated models (0x2D / 0x2E /
  0x2F).
- **[5]..[8] 0x825560 (NEARMISS)** are breakable objects of the AREA20
  0x823D30 family with an extra shot reaction and a landing check in area
  0x15.
- **Effects.** 0x823580 / 0x823800 (effects 0 / 1 each frame), 0x8235E0 /
  0x8236F0 / 0x823860 (func_001D04B0 / func_001CFB50 packets), group
  0x826890's 0x8239D0 (func_001F4E20 points) and group 0x827F40's
  0x8251B0 (a part riding its owner's state).

## Matching notes (mwcc 2.3.3)

- **Literal-address stores keep the original store order.** In 0x823BA0
  the skip writes D_00810350 / D_00810374 / D_008105D0 / D_008105E0
  through `*(float *)0x...`; with extern arrays mwcc moves the
  D_008105E0 store up next to the equal D_00810350 value.
- **Extern arrays let mwcc reorder** where the original did: in 0x824700's
  marker block the original order comes from writing the D_700038A0
  updates before the D_700038B0 colour with extern arrays.
- **Assignment inside the operands** (`((ax = a[0]) + (px = ...)) / 2`)
  gives 0x823F70's FPR choice (found among 50 spellings).
- **Case order 0, 1, 2, 0x19, 3, 4, 5** in 0x823BA0 (mwcc compares in reverse
  source order), and 5, 4, 3, 0, default in 0x8251B0.
- **`(float *)` casts on the func_001F5940 argument** (0x824CF0, 0x824E40)
  set a1 before a0 as the original does.
- **`int *hp = (int *)(self + 0x1F0) + 0x3F;` before the test** (0x825EA0)
  reproduces the original's separate add-then-offset address.
- **`>` / `<=` forms** for the compares into the assembler temporary
  (0x8242D0, 0x824CF0).

### NEARMISS 0x825560 (99.41%, same size)

In state 1 the original loads 0x70003A20 before it stores the squared
0x70003A24 (50 * 50) and compares with the stored value (a `mov.s` of it);
mwcc 2.3.3 stores first and loads 0x70003A20 after, which also swaps two
FPRs. Assignment-in-condition, locals, operand orders and x * x spellings
were tried; the rest of the function (0x934 bytes) is equal. Equivalence:
the instruction-by-instruction diff shows only that block: eight consecutive
words (function offsets 0x210-0x22C: the int-to-FPR move target, the
placement of a no-op and of an upper-half constant load, the 0x70003A20 load, the multiply, the
0x70003A24 store and the compare operands). The two scratchpad addresses do
not overlap, and on both branch paths a call follows before any FPR in the
block is read again, so the behaviour is the same.

## Verification

- `overlay_match.py check AREA17 src/overlays/AREA17/*.c`: 20 new files
  100.00 BYTE-IDENTICAL (plus the two earlier ones), 0x825560 99.41
  (NEARMISS).
- `python3 tools/check_no_disassembly.py src/overlays/AREA17/*.c`: clean.
- Bounded mutation sweep (build/ovlc/mut): AREA17 0x823BA0 `D_00810805 ==
  8` -> 9, AREA17 0x824700 `func_001C47E0(0x28, 1)` -> 0x29, AREA08 0x824490
  frame 2000 -> 2001, AREA20 0x823D30 gravity 0.06 -> 0.07 and AREA07
  0x823F80 state 0x64 -> 0x65: every mutant loses byte identity
  (`overlay_match.py check` reports no BYTE-IDENTICAL; objdiff scores
  100.00 / 99.99 because it ignores immediates).
- Under the decomp build lock: `compile_overlay_src.py AREA17` (23 objects,
  1 NEARMISS skipped), then `tools/overlay/build.py --area AREA17
  --no-extract --no-yaml --no-splat`: PASS, AREA17.BIN equals the extracted
  file (22912 bytes, full file). Every linked filler object's `.text` equals
  the compiled `.text` outside relocation fields (build/ovlc/prov.log).
  `config/overlays/AREA17.lds` was regenerated.

## Gate

One locked run for the five areas this lane touched (build/ovlc/gate.sh
full with the default AREAS = AREA07 AREA08 AREA17 AREA20 AREA21; log
build/ovlc/gate_full.log, decomp log build/ovlc/decomp_build.log; an
earlier overlay-only run is build/ovlc/gate.log): `compile_overlay_src.py`
and `tools/overlay/build.py --area ... --no-extract --no-yaml --no-splat`
for each area (each "1/1 overlays passed", full files byte-identical), the
full `tools/decomp/build.py build` (rc 0, 2211 units) and
`tools/verify_all.py`: all six stages PASS (lock held 17:22 to 17:37). The
boot ELF is byte-identical (0x175b00 loadable bytes), 19/19 overlays pass,
matched_code is 98.65% (2155/2211), and glTF, selftest and gs-offset pass.
Overlay C is not an objdiff unit, so matched_code does not count this lane.

## Binding

The native port has no AREA17 module, and this lane edited nothing in the
port. For a port binding the C above is the ground truth; the NEARMISS body
is correct apart from the listed block.

## Known gaps

- One NEARMISS function (above). The entry pad is an asm body.
- The data section (0x2C00 bytes) is linked from splat's data assembly.
- No AREA17 capture exists; the sequence values are read from the code only.
- The verification cites git-ignored scratch in build/ovlc (see
  docs/AREA07_OVERLAY.md).
