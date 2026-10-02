# AREA08 overlay — decompilation status (lane OVLC, 2026-10-01)

## What AREA08 is

AREA08 is reached by the AREA03 sub 2 lift [7] (item 0x2A from AREA03 sub
1's [22]); its lift [8] returns to AREA03 entry 1, and it shares one layout
with AREA07 (docs/WORLD_GRAPH.md, the AREA08 table and section 7 step 5,
inferred, not played). It has four sub-states (lock byte D_00810849) and
writes counters / flags 0x17, 0x18 and flag 0x25. This lane did not run the
game; the behaviour below is read from the C.

The code module is `OVERLAY/AREA08.BIN`: overlay id 8, text 0x1BC0, data
0x2E00, bss 0x140C00, file 18944 bytes. Linked at 0x00823500, it runs 0x40
higher (link names are the runtime address minus 0x40; data symbols carry
their runtime address). The boot area dispatcher func_001E7780 calls the
init at runtime 0x8241F0 (link 008241B0, earlier C).

## Split handling

`overlay_match.py list AREA08` regroups the 21 splat pieces into 15
functions. Five groups hold one later piece each, fake splits at an
intra-overlay call target (`[fill] 5 splat piece(s) absorbed`). The
entry-pad group also holds the piece 00823540, a real leaf function at
runtime 0x823580 (`overlay_AREA08_func_00823540.c`, checked against the
original bytes with build/ovlc/piece.py since it is not a group head). The
earlier asm-body file for the piece 00824490 was removed: the C of
00824450 covers it. AREA08 has no jump table.

## Status

16 functions (15 groups plus the leaf in the pad group): 15 are compiled C
that links byte-identical (14 new in this lane plus the init); the entry
pad stays an asm body. The hybrid asm-body files 00823540, 008235A0,
00824420, 00824450 and 00824490 were replaced by C.

| link name | runtime | size | status | reached by (static, area_overview.py --area 08) |
|---|---|---|---|---|
| func_overlay_AREA08_00823500 | 0x823540 | 0x4 | asm pad | nop sled, no code |
| overlay_AREA08_func_00823540 | 0x823580 | 0x54 | C, byte-identical (was asm body) | - (no static reference) |
| func_overlay_AREA08_008235A0 | 0x8235E0 | 0x4C | C, byte-identical (was asm body) | script 0x825880 op09 record 0x825A00 |
| func_overlay_AREA08_008235F0 | 0x823630 | 0x178 | C, byte-identical (absorbs 00823630) | call from 0x823870 |
| func_overlay_AREA08_00823770 | 0x8237B0 | 0xBC | C, byte-identical (absorbs 008237B0) | call from 0x823870 |
| func_overlay_AREA08_00823830 | 0x823870 | 0x250 | C, byte-identical | sub0+1 place [6] (door, room move e7 / e6) |
| func_overlay_AREA08_00823A80 | 0x823AC0 | 0x730 | C, byte-identical | sub2 place [0] |
| func_overlay_AREA08_008241B0 | 0x8241F0 | 0x20 | C, byte-identical (earlier; init) | boot 0x1E7780 |
| func_overlay_AREA08_008241D0 | 0x824210 | 0x244 | C, byte-identical | sub0+1 place [12] |
| func_overlay_AREA08_00824420 | 0x824460 | 0x2C | C, byte-identical (was asm body) | script 0x825AA0 op09 record 0x825B60 |
| func_overlay_AREA08_00824450 | 0x824490 | 0x144 | C, byte-identical (was asm body) (absorbs 00824490) | call from 0x824210 |
| func_overlay_AREA08_008245A0 | 0x8245E0 | 0x1A4 | C, byte-identical | sub3 place [2] |
| func_overlay_AREA08_00824750 | 0x824790 | 0xDC | C, byte-identical (absorbs 00824790) | call from 0x8245E0 |
| func_overlay_AREA08_00824830 | 0x824870 | 0x17C | C, byte-identical (absorbs 00824870) | call from 0x8245E0 |
| func_overlay_AREA08_008249B0 | 0x8249F0 | 0x1C4 | C, byte-identical | sub0+1 place [27], [28], [29] |
| func_overlay_AREA08_00824B80 | 0x824BC0 | 0x4FC | C, byte-identical | sub3 place [4] |

As for AREA07, the static scan ran through build/ovlc/ov_wrap.py (bytes
outside the static image read as 0xFF), so placements that live only in
RAM are missing. Every new file is mwcc 2.3.3 (`-O4,p`, `-sdatathreshold
0`; `-sdatathreshold 4` for 0x8249F0 and 0x824BC0, which use the
gp-relative D_00275990 / D_00275998 / D_00275B40).

## Behaviour (from the C)

- **Twins of AREA07.** 0x8245E0 / 0x824790 / 0x824870 are the AREA07
  0x823990 / 0x823B40 / 0x823C20 C with flag 0x18, func_001C4760(0x13, 1),
  scripts 0x826420 / 0x8268E0, D_008107F0 and the roles of D_0081076D /
  D_00810770 swapped; 0x8249F0 is the AREA07 0x823DA0 shape with +0xB0
  against 300 (scripts 0x827170 / 0x827330).
- **[6] 0x823870, the door to e7 / e6.** While D_008107EF is 0 it is a
  talk door: at spawn entry 3 the 0x8237B0 ambience (every 20th frame
  sound 0x906 when +0x2A equals 1 + 3 * rand / 2^15 and D_0081076F is 0)
  and the AREA04 0x823580 opener 0x823630 (script 0x825880); once
  D_008107EF is set it runs the common door steps func_001BB560 /
  001BB7C0 / 001BC150 / 001BB7F0.
- **[12] 0x824210 (counter 0x17).** After flag 0x17 nothing; else, once
  D_0081076F is set: script 0x825AA0 (callback 0x824460: func_001FABB0,
  func_001FA790(0, 0x11)), at its end D_008107EF = 1 and D_008106C0 =
  func_001B6660(group 0x825420); then at spawn entry 6 script 0x825E60,
  during which 0x824490 plays a +0x28 timeline of func_001C67E0 motions
  and a slide of +0.07 / -0.07 per frame from frame 3100; at its end
  func_001C4760(0x63, 1) and D_008107EF = 0xFF.
- **sub 2 [0] 0x823AC0** is an emitter between two point pairs (effects
  0x8000003B and 0x80000042, random timers 2 + rand % 2 and 12 + rand % 12)
  with a random sound 0x41D / 0x41E / 0x41F every 100 frames.
- **sub 3 [4] 0x824BC0** draws five flame columns (table 0x827DD0) with a
  looping sound 0x910 held in +0x2EC through the func_001FC3C0 voice
  pattern, and lowers the D_00275B40 record's +0x74 through func_001B1470.
- **0x823580 / 0x8235E0** move the +0x110 object's +0x7C by -0.2 / +0.2 and
  report its limit (-9.0 / 0.0); 0x8235E0 is a callback of script
  0x825880.

## Matching notes (mwcc 2.3.3)

- **A one-case switch for the D_008107EF test** in 0x823870 and 0x824210
  (`switch (D_008107EF) { case 0: ... default: ... }`) gives the original's
  `beqz` + `b` layout; an if / else places the blocks the other way.
- **A callee declared with one parameter** (0x8237B0 called as
  `(self)`) leaves the a1 copy for the next call in the test slot and the
  jal slot empty, as the original.
- **`S16(0x2A) > 4` with the reloading `S16(0x2A) = S16(0x2A) + 1`**
  gives the original's compare into the assembler temporary and its
  branch-likely (0x8237B0). The `>` / `<=` spellings put the compare in the
  assembler temporary, `<` / `>=` in a v register.
- **Extern arrays for D_70003B68 / D_70003B8A** leave the slots of the two
  `% 10` tests empty in 0x824BC0 (literal addresses let mwcc speculate the
  `lui` into them); the `(int *)(blk) + 0x3F` slot pointer and a declaration
  order found by search (6 of 400 orders match) give the original's
  registers.
- **Int-staged zero** for the last func_001C67E0 call of 0x824490 (f13
  before f12), as in AREA21.

## Verification

- `overlay_match.py check AREA08 src/overlays/AREA08/*.c`: 13 group heads
  100.00 BYTE-IDENTICAL (plus the init); `overlay_AREA08_func_00823540`
  equals the original 0x54 bytes (build/ovlc/piece.py).
- `python3 tools/check_no_disassembly.py src/overlays/AREA08/*.c`: clean.
- Mutation: 0x824490 frame 2000 -> 2001 loses byte identity (99.99).
- Under the decomp build lock: `compile_overlay_src.py AREA08` (16 objects),
  then `tools/overlay/build.py --area AREA08 --no-extract --no-yaml
  --no-splat`: PASS, AREA08.BIN equals the extracted file (18944 bytes,
  full file). Every linked filler object's `.text` equals the compiled
  `.text` outside relocation fields (build/ovlc/prov.log).
  `config/overlays/AREA08.lds` was regenerated.

## Gate

See "Gate" in docs/AREA17_OVERLAY.md.

## Binding

The native port has no AREA08 module, and this lane edited nothing in the
port. For a port binding the C above is the ground truth.

## Known gaps

- The entry pad is an asm body; the data section (0x2E00 bytes) is linked
  from splat's data assembly.
- No AREA08 capture exists; the behaviour is read from the code only.
- The verification cites git-ignored scratch in build/ovlc (see
  docs/AREA07_OVERLAY.md).
