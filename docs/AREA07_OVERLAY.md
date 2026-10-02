# AREA07 overlay — decompilation status (lane OVLC, 2026-10-01)

## What AREA07 is

AREA07 is the area behind the AREA04 lift [56] (entry 0, item 0x29 at the
socket [55]); its lift [0] goes back to AREA04 entry 10, and AREA07 and
AREA08 share one layout (docs/WORLD_GRAPH.md, the AREA07 table and section
7 step 5, inferred, not played). It has four sub-states (lock byte
D_00810848) and writes counter / flag 0x15 and flag 0x25. This lane did not
run the game; the behaviour below is read from the C.

The code module is `OVERLAY/AREA07.BIN`: overlay id 7, text 0x2640, data
0x2500, bss 0x140C00, file 19328 bytes. Like every overlay it is linked at
0x00823500 but runs 0x40 higher, so splat and link names are the runtime
address minus 0x40 (data symbols carry their runtime address). The boot
area dispatcher func_001E7780 calls the init at runtime 0x823970 (link
00823930, earlier C).

## Split handling

`overlay_match.py list AREA07` regroups the 16 splat pieces into 11
functions. Four groups hold one later piece each, fake splits at an
intra-overlay call target 0x40 into the called function (`[fill] 4 splat
piece(s) absorbed by 4 compiled function(s)`). The entry-pad group also
holds the piece 00823540, the empty function at runtime 0x823580 (earlier
C, `overlay_AREA07_func_00823540.c`; code pointer of 0x823590). AREA07 has
no jump table.

## Status

12 functions: 10 are compiled C that links byte-identical (8 from this
lane: 7 new files plus 00825430 converted from an asm body; and 2 earlier
files: the empty stub 00823540 and the init), 1 is NEARMISS readable C linked from its splat .s, and
the entry pad stays an asm body. The hybrid asm-body file 00825430 was
replaced by C.

| link name | runtime | size | status | reached by (static, area_overview.py --area 07) |
|---|---|---|---|---|
| func_overlay_AREA07_00823500 | 0x823540 | 0x4 | asm pad | nop sled, no code |
| overlay_AREA07_func_00823540 | 0x823580 | 0x8 | C, byte-identical (earlier; empty) | code pointer in 0x823590 |
| func_overlay_AREA07_00823550 | 0x823590 | 0x3DC | NEARMISS 99.88% | - (no static reference) |
| func_overlay_AREA07_00823930 | 0x823970 | 0x20 | C, byte-identical (earlier; init) | boot 0x1E7780 |
| func_overlay_AREA07_00823950 | 0x823990 | 0x1A4 | C, byte-identical | sub3 place [7] |
| func_overlay_AREA07_00823B00 | 0x823B40 | 0xDC | C, byte-identical (absorbs 00823B40) | call from 0x823990 |
| func_overlay_AREA07_00823BE0 | 0x823C20 | 0x17C | C, byte-identical (absorbs 00823C20) | call from 0x823990 |
| func_overlay_AREA07_00823D60 | 0x823DA0 | 0x1D8 | C, byte-identical | sub0+1 place [17], [18] |
| func_overlay_AREA07_00823F40 | 0x823F80 | 0x14F0 | C, byte-identical | sub2 deferred group 0x825F80 [1], [3], [5], [7] |
| func_overlay_AREA07_00825430 | 0x825470 | 0x4D0 | C, byte-identical (was asm body) (absorbs 00825470) | call from 0x823F80 |
| func_overlay_AREA07_00825900 | 0x825940 | 0x84 | C, byte-identical (absorbs 00825940) | call from 0x823F80 |
| func_overlay_AREA07_00825990 | 0x8259D0 | 0x198 | C, byte-identical | sub2 deferred group 0x825F80 [2], [4], [6], [8] |

The static scan needed a wrapper (build/ovlc/ov_wrap.py) because some sub
placement tables of AREA07 point outside the static image; unreadable bytes
were read as 0xFF, so tables that live only in RAM are missing from the
"reached by" column. Every new file is mwcc 2.3.3 (`// COMPILER: mwcc233`,
`-O4,p`, `-sdatathreshold 0`; `-sdatathreshold 4` for 0x823DA0, which takes
the gp-relative address of D_00275970, and for the AREA01-family functions
that read D_00275B40). Each source header describes what the instructions
do.

## Behaviour (from the C)

- **[7] 0x823990 (sub 3) and its halves.** While D_0081076D (counter 0x15)
  is 0 a func_001F4BF0 marker (0, 0x80, 0, 0x80) shows at the object;
  until D_00810770 is 0xFF the first half 0x823B40 runs (on +0xB bit 2
  func_001B6F00(self, (0, -6, 0, 1), pi) and script 0x8264B0; at its end
  func_001C4760(0x11, 1) and D_008107ED = 0xFF), afterwards 0x823C20
  (script 0x826930; once D_0081077D is set one more func_001B6F00 at
  (0, -8, 0, 1); at its end func_001C4760(0x11, 1), func_001C4760(0x14, 1),
  func_001FABB0(), func_001FB0B0(9), D_0081076D = D_008107ED = 0xFF). Spawn
  gives +0 = 2 when flag 0x15 is already set.
- **[17] / [18] 0x823DA0 (subs 0 / 1).** A talk point: +0xB8 < 600 picks
  +0x30 = 0x827180 and a +0x9E / func_001B2140 gate, otherwise
  &D_00275970; on +0xB bit 2 script 0x8271A0 (D_00810358 < 600) or
  0x827360.
- **The AREA01 family in sub 2.** 0x823F80 is the AREA01 0x826D40 C with
  the story flag 0x15 used the other way round (set: state 0x64, which
  only runs func_001C6380 / func_001B17A0 / the +0x4C method; the end of
  states 4 and 1 sends it to 0x64 and zeroes the +0x220 child's +0xA0..
  +0xAC; state 2 turns back only while the flag is clear) and the AREA03
  0x823930 hit branch. 0x825470 is the AREA13 0x826140 probe, 0x825940 the
  AREA19 0x829840 helper, 0x8259D0 the AREA01 0x828850 owner.
- **0x823590 (NEARMISS)** is an effect object: two packets of table 0x826420
  a frame at seeded random phases while +5 is 0, then hidden while +5 is 1
  (+0 = 2). The timer +0x20C rises by 0.05 a frame in both phases and is
  reset to 0 at each switch. The drawing phase ends when the timer passes
  count + 1, the hidden phase when it passes count; the count +0x208 is the
  integer part (func_001281C0) of 6 + 18 * r for the drawing phase (set in
  state 0 and when the hidden phase ends) and of 6 + 9 * r for the hidden
  phase, r = func_00122BB8() / 2^31. In frames that is about 20 * (count + 1)
  drawn and about 20 * count hidden.

## Matching notes (mwcc 2.3.3)

The AREA13 / AREA14 / AREA19 / AREA21 notes apply. New or confirmed here:

- **Pointer-local placement steers register colouring.** 0x823F80 needs the
  +0x220 slot pointer as its own local; assigned at the start of the
  `+0x208 <= 0` path of state 4 and just before the flag test of state 1
  it lands in s2 as in the original (39 placements were scored; 31 match).
  Reusing the REC_A pointer local for it was a dead store.
- **Int-staged sizes in func_001CD520.** 0x825470 needs both the second
  size (3) and the depth (2) int-staged; the AREA13 spelling (only the
  second size) gives 96.62, the AREA14 one 95.98.
- **func_001F4BF0 takes two arguments.** The 1.0 left in a2 is the
  register of the D_700038AC store, not a third argument.
- **Arrays, not scalars, for D_00810358** under `-sdatathreshold 4`
  (a scalar float extern turns gp-relative).
- **Loop body as separate statements** (`f = (float)x; f = f / 65535.0f;
  f += 0.0001f;` and `f = base + f`) and a separate pointer base for the
  phase increment (`((float *)blk)[7] += 0.05f` then reading
  `*(float *)(blk + 0x1C)`) reproduce the original's loads and the reload
  in 0x823590.

### NEARMISS 0x823590 (99.88%, same size)

The three materialisations of 2^31 (the rand-fraction divisor) use a2 in
the original and a0 in mwcc 2.3.3; every instruction, its order and every
other register are equal. 420 declaration orders, a `(int)` cast in place
of the func_001281C0 call (mwcc then emits its own `fptosi` helper call),
prototype spellings, an inline helper and 33 placements of the owner
pointer were tried. Equivalence: the instruction-by-instruction diff
(`overlay_match.py check AREA07 .. --show`) shows only that register.

## Verification

- `overlay_match.py check AREA07 src/overlays/AREA07/*.c`: the 8 lane
  files (7 new plus 00825430) 100.00 BYTE-IDENTICAL, as are the earlier init
  and empty stub 00823540, 0x823590 99.88 (NEARMISS).
- `python3 tools/check_no_disassembly.py src/overlays/AREA07/*.c`: clean.
- Bounded mutation sweep (build/ovlc/mut): 0x823F80 state 0x64 -> 0x65 loses
  byte identity (see docs/AREA17_OVERLAY.md for the full sweep).
- Under the decomp build lock: `compile_overlay_src.py AREA07` (11 objects,
  1 NEARMISS skipped), then `tools/overlay/build.py --area AREA07
  --no-extract --no-yaml --no-splat`: PASS, AREA07.BIN equals the extracted
  file (19328 bytes, full file). Every linked filler object's `.text` equals
  the compiled `.text` outside relocation fields (build/ovlc/prov.py, log
  build/ovlc/prov.log). `config/overlays/AREA07.lds` was regenerated.

## Gate

See "Gate" in docs/AREA17_OVERLAY.md (one locked run for AREA07, AREA08,
AREA17, AREA20 and AREA21).

## Binding

The native port has no AREA07 module, and this lane edited nothing in the
port. For a port binding the C above is the ground truth; the NEARMISS body
is correct apart from the listed register.

## Known gaps

- One NEARMISS function (above). The entry pad is an asm body.
- The data section (0x2500 bytes) is linked from splat's data assembly.
- No AREA07 capture exists; the behaviour is read from the code only, and
  the "reached by" column lacks placements that live only in RAM.
- The verification cites git-ignored scratch in build/ovlc (gate.sh,
  gate logs, prov.py, prov.log, ov_wrap.py, mut/, w/, try/); it must not be
  deleted while this doc cites it.
