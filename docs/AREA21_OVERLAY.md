# AREA21 overlay — decompilation status (lane A03C, 2026-10-01)

## What AREA21 is

AREA21 is reached from AREA20 ([32] 0x823BE0, counter 0x34 and a quad; AREA21
entry 3) and holds the game's ending request ([53] 0x827040, counter 0x37:
B8 = 1 with D_70003B93 = 1 or 2) (docs/WORLD_GRAPH.md, sections 5 and 7 step
7, inferred, not played). Its three doors are room moves inside AREA21. This
lane did not run the game; the behaviour below is read from the C.

The code module is `OVERLAY/AREA21.BIN`: overlay id 18, text 0x6E40, data
0x3900, bss 0x140C00, file 42880 bytes, one sub-state (placement table
0x82D110). Like every overlay it is linked at 0x00823500 but runs 0x40
higher, so splat and link names are the runtime address minus 0x40. The boot
area dispatcher func_001E7780 calls the init at runtime 0x826180 (link
00826140, earlier C).

## Split handling

`overlay_match.py list AREA21` regroups the 61 splat pieces into 49
functions. Eleven groups hold later pieces, all fake splits at an
intra-overlay call target 0x40 into the called function (`[fill] 12 splat
piece(s) absorbed by 11 compiled function(s)`; 00829F00 absorbs 00829F10 and
00829F40). A scan of every slot for code after a return (build/a03c/hidden.py)
finds no second function.

Four functions are jump-table dispatchers. Three link from C with
`tools/overlay/jt_pin.py` placing the compiled table at the original address
(`[pin] func_overlay_AREA21_00826B70: .rodata 0x18 bytes at runtime
0x0082dc00`, `00826E20: 0x0082dc20`, `00827600: 0x20 bytes at 0x0082dc60`);
`overlay_match.py check` reports them as 99.98 / 99.99 (`rodata-needs-pin`).
The fourth, 0x8273C0 (table 0x82DC40), is NEARMISS, so its table stays in
the data assembly.

## Status

49 functions: 42 are compiled C that links byte-identical (38 new in this
lane, plus the earlier 00823A60, 00825C20 and the init 00826140, and
0x825C70, a NEARMISS until lane OVLC found its lever, below); 3 are
NEARMISS readable C, linked from their splat .s; 3 are not decompiled
(splat assembly, below); the entry pad stays an asm body. The asm-body files
00826060, 00826290, 00828310, 008283D0, 00829550, 00829ED0 and 00829F00 were
replaced by C.

| link name | runtime | size | status | reached by (static, `area_overview.py --area 21`) |
|---|---|---|---|---|
| func_overlay_AREA21_00823500 | 0x823540 | 0x4 | asm pad | nop sled, no code |
| overlay_AREA21_func_00823540 | 0x823580 | 0x224 | C, byte-identical | group 0x82B500 x2 |
| func_overlay_AREA21_00823770 | 0x8237B0 | 0xA4 | C, byte-identical | group 0x82BA70 x12 |
| func_overlay_AREA21_00823820 | 0x823860 | 0x78 | C, byte-identical | group 0x82BA70 x3 |
| func_overlay_AREA21_008238A0 | 0x8238E0 | 0x12C | C, byte-identical | group 0x82BA70 x3 |
| func_overlay_AREA21_008239D0 | 0x823A10 | 0x8C | C, byte-identical | group 0x82B500 |
| func_overlay_AREA21_00823A60 | 0x823AA0 | 0x8 | C, byte-identical (earlier) | group 0x82BA70 |
| func_overlay_AREA21_00823A70 | 0x823AB0 | 0x8C | C, byte-identical | group 0x82BA70 |
| func_overlay_AREA21_00823B00 | 0x823B40 | 0x868 | not decompiled (asm) | - (no static reference) |
| func_overlay_AREA21_00824370 | 0x8243B0 | 0x93C | not decompiled (asm) | sub0 place [52] |
| func_overlay_AREA21_00824CB0 | 0x824CF0 | 0x2F0 | C, byte-identical | - (no static reference) |
| func_overlay_AREA21_00824FA0 | 0x824FE0 | 0xB18 | not decompiled (asm) | - (no static reference) |
| func_overlay_AREA21_00825AC0 | 0x825B00 | 0x160 | NEARMISS 97.59% | - (no static reference) |
| func_overlay_AREA21_00825C20 | 0x825C60 | 0x8 | C, byte-identical (earlier) | code pointer in 0x825C70 |
| func_overlay_AREA21_00825C30 | 0x825C70 | 0x42C | C, byte-identical (lane OVLC; was NEARMISS 99.23%) | - (no static reference) |
| func_overlay_AREA21_00826060 | 0x8260A0 | 0xD4 | C, byte-identical (was asm body) (absorbs 008260A0) | call from 0x8297C0 |
| func_overlay_AREA21_00826140 | 0x826180 | 0x20 | C, byte-identical (earlier; init) | boot 0x1E7780 |
| func_overlay_AREA21_00826160 | 0x8261A0 | 0x124 | C, byte-identical | sub0 place [42] |
| func_overlay_AREA21_00826290 | 0x8262D0 | 0x38 | C, byte-identical (was asm body) | sub0 place [43], [44] |
| func_overlay_AREA21_008262D0 | 0x826310 | 0x1C0 | C, byte-identical (absorbs 00826310) | call from 0x8262D0 |
| func_overlay_AREA21_00826490 | 0x8264D0 | 0x218 | C, byte-identical (absorbs 008264D0) | call from 0x8262D0 |
| func_overlay_AREA21_008266B0 | 0x8266F0 | 0x148 | C, byte-identical | sub0 place [45] |
| func_overlay_AREA21_00826800 | 0x826840 | 0xA8 | C, byte-identical (absorbs 00826840) | call from 0x8266F0 |
| func_overlay_AREA21_008268B0 | 0x8268F0 | 0xA8 | C, byte-identical (absorbs 008268F0) | call from 0x8266F0 |
| func_overlay_AREA21_00826960 | 0x8269A0 | 0x48 | C, byte-identical | script 0x82BEC0 op09 record 0x82C380 |
| func_overlay_AREA21_008269B0 | 0x8269F0 | 0x1BC | C, byte-identical | script 0x82BEC0 op09 record 0x82C340 |
| func_overlay_AREA21_00826B70 | 0x826BB0 | 0x2A8 | C, byte-identical at link (jump table pinned) | sub0 place [30] (door 1) |
| func_overlay_AREA21_00826E20 | 0x826E60 | 0x1D8 | C, byte-identical at link (jump table pinned) | sub0 place [27], [33] (doors 0, 2) |
| func_overlay_AREA21_00827000 | 0x827040 | 0x210 | C, byte-identical | sub0 place [53] |
| func_overlay_AREA21_00827210 | 0x827250 | 0x6C | C, byte-identical | script 0x82CBD0 op09 record 0x82CDD0 |
| func_overlay_AREA21_00827280 | 0x8272C0 | 0xFC | C, byte-identical | sub0 place [47] |
| func_overlay_AREA21_00827380 | 0x8273C0 | 0x280 | NEARMISS 96.50% | sub0 place [49], [50] |
| func_overlay_AREA21_00827600 | 0x827640 | 0x234 | C, byte-identical at link (jump table pinned) | sub0 place [51] |
| func_overlay_AREA21_00827840 | 0x827880 | 0x1A8 | C, byte-identical | sub0 place [56] |
| func_overlay_AREA21_008279F0 | 0x827A30 | 0x294 | C, byte-identical | sub0 place [57] |
| func_overlay_AREA21_00827C90 | 0x827CD0 | 0x1A0 | C, byte-identical | sub0 place [58] |
| func_overlay_AREA21_00827E30 | 0x827E70 | 0x4D4 | C, byte-identical | sub0 place [59], [60] |
| func_overlay_AREA21_00828310 | 0x828350 | 0xC0 | C, byte-identical (was asm body) | sub0 deferred group 0x82A380 |
| func_overlay_AREA21_008283D0 | 0x828410 | 0x2F0 | C, byte-identical (was asm body) (absorbs 00828410) | call from 0x828700 |
| func_overlay_AREA21_008286C0 | 0x828700 | 0x77C | C, byte-identical | sub0 place [61] |
| func_overlay_AREA21_00828E40 | 0x828E80 | 0xAC | C, byte-identical (absorbs 00828E80) | call from 0x828700 |
| func_overlay_AREA21_00828EF0 | 0x828F30 | 0x3D8 | C, byte-identical | code pointer in 0x828E80 |
| func_overlay_AREA21_008292D0 | 0x829310 | 0x1B0 | C, byte-identical (absorbs 00829310) | calls from 0x828700, 0x8297C0 |
| func_overlay_AREA21_00829480 | 0x8294C0 | 0xD0 | C, byte-identical (absorbs 008294C0) | call from 0x8297C0 |
| func_overlay_AREA21_00829550 | 0x829590 | 0x230 | C, byte-identical (was asm body) (absorbs 00829590) | call from 0x8297C0 |
| func_overlay_AREA21_00829780 | 0x8297C0 | 0x74C | C, byte-identical | sub0 place [62] |
| func_overlay_AREA21_00829ED0 | 0x829F10 | 0x2C | C, byte-identical (was asm body) | call from 0x8297C0 |
| func_overlay_AREA21_00829F00 | 0x829F40 | 0x28C | C, byte-identical (was asm body) (absorbs 00829F10, 00829F40) | call from 0x8297C0 |
| func_overlay_AREA21_0082A190 | 0x82A1D0 | 0x190 | NEARMISS 97.50% | sub0 place [54], [55] |

Every new file is mwcc 2.3.3 (`// COMPILER: mwcc233`, `-O4,p`,
`-sdatathreshold 0`; `-sdatathreshold 4` for 0x8272C0 and 0x8273C0, which
read D_00275B40). Each source header describes what the instructions do;
roles are read from the code, not from placement labels.

## Behaviour (from the C)

- **D_0081080E (counter 0x36) sequence.** [45] 0x8266F0 dispatches by
  D_0081080E: 1 / 4 / 0x10 / 0x11 -> 0x826840, 2 / 3 / 0xFF -> 0x8268F0.
  0x826840 starts script 0x82BEC0 at D_0081080E == 1 (with
  func_0019C6F0(0x1C, 0)) and sets 0x11 at its end; its callback 0x8269A0
  sets 0x10 (and D_008106C0 = func_001B6660(group 0x82A5A0)); 0x8268F0
  starts script 0x82C5C0 at 2 and sets 0xFF at its end. D_0081080E == 0x10
  starts the counts of [56] 0x827880 (255 frames), [57] 0x827A30 (65),
  [59] / [60] 0x827E70 (220) and the 255-frame end of [61] 0x828700; at
  D_0081080E == 3, [58] 0x827CD0 counts 830 frames and then calls
  func_0019C6F0 for 0x10, 0x13, 0x14, 0x16..0x1B, and the door [30]
  0x826BB0 sets its lock bit (+0x34) of D_00810841[D_00810700] at frame
  1492 and clears it at 2000.
- **[43] / [44] 0x8262D0 and D_00810856 (the lock byte).** 0x826310 (+3 ==
  0) sets D_00810856 = 0xFF at load, ORs 1 / 2 into D_0081080D when the
  arrival entry D_00810702 is 1 / 5, and with both bits starts script
  0x82B5E0 at entry 6 or 2; its end sets D_008106C0 = func_001B6660(group
  0x82A540), D_00810856 = 0 and func_001FB0B0(0xC). 0x8264D0 (+3 != 0)
  waits for flag 0x34, then is a +0xB bit 2 script point (0x82B8A0) until
  D_0081078D is set.
- **[42] 0x8261A0.** Script 0x82B240 once D_0081078C (flag 0x34) is set;
  at its end D_0081080C = 0xFF (counter 0x34).
- **[53] 0x827040 (the ending request).** After flag 0x36, D_008106C0 =
  func_001B6660(group 0x82A600); once D_0081080F (counter 0x37) is set,
  D_0081078F = 1, script 0x82CBD0 and func_001D2830(0x24, 1); at the
  script's end (result r) func_001AEDE0(4, 0), D_70003B93 = 2 when r == 3
  else 1, func_001FBC50(), the D_00810858 / D_008104D0 / D_0081085C /
  D_008104D8 values, D_0081080F = 0xFF and D_008106B8 = 1. Its script
  callback 0x827250 sets the same four values, calls func_0015C700 /
  func_0015C750 / func_001B81D0 on D_008102B0 and sets D_00810856 = 0xFF.
- **[61] 0x828700 and [62] 0x8297C0.** Both are keyed to the object
  D_008102B0 (+0x230 == 0x28, +0x1F0 == 0x39 resp. 0x38, +0x1F1 == 1).
  While active they move +0xC4 and the +0x118 model's +0x70 through
  0x829310 (a rate-limited step on D_00810E64 / D_00810E65 with the table
  0x82DB10), clamped (+-0.75049156 and -0.17453294 .. 0.6108653 for [61];
  +-0.69813174 for [62]), and test D_00810E70 & (D_70003B74 | D_70003B78).
  With a set bit, [61] calls func_001FB9F0(0x454, ...) and spawns the
  0x828F30 object through 0x828E80; [62] adds to +0x22C and, while its +7
  is 0, calls func_001F4F40(2) / func_001F4010(3); above 900 it resets with
  D_008102B0
  +0x302 = 1, +0x22C = 1800 and the 0x829F40 phase (+0x22C counted down).
  0x8260A0 is called with +0x22C / 900 each frame below 900.
- **0x828F30.** Moves along its +0xC0 direction (scaled to 8) for 32
  frames, then falls; a func_0019A570 hit or y < 10 ends it with effect
  0x80000069 / 0x8000005F, and a hit object of kind 2 and +0 == 1 gets
  func_001B41F0(..., 0x2000, 0x50), another kind +0x36 = 5.
- **Doors [27] / [33] 0x826E60, [30] 0x826BB0.** The AREA19 door steps
  without the D_00810CCD branch; script 0x82CB00 on step 1.
- **Groups 0x82B500 / 0x82BA70** (members of scripts 0x82B5E0 / 0x82C5C0):
  frame-counted effects (func_001EFD20 ids 0, 1, 0x80000047, 0x80000059),
  a func_001CD520 quad and func_001F4E20 points.
- **Unreached code.** 0x823B40, 0x824CF0, 0x824FE0, 0x825B00 and 0x825C70
  (with its code pointer 0x825C60) have no static reference in the tables
  `area_overview.py` reads.

## Matching notes (mwcc 2.3.3)

The AREA13 / AREA19 notes apply. New or confirmed here:

- **Int-staged non-zero floats (idiom-24 family).** func_001CFB50 constants
  in 0x824CF0 (1.0 and 5.0) and 0x829F40 (7.0), and the zero of
  func_001C67E0 in 0x8264D0, are staged through ints to get the original's
  materialisation order.
- **Float literals.** 1.308997f gives the original 0x3FA78D37 where
  1.3089969f gives 0x3FA78D36 (0x827040).
- **Compound assignment** for add.s operand order (`*(float *)(w + 0x10)
  += 1.0f` in 0x8269F0; `d = f(); d -= 0.01f` in 0x829310).
- **Shared tails.** 0x827E70's sub-states end in one func_001B1B70 / +0x4C
  tail (writing it per case gives 90.84).
- **Shared arrays for store order.** D_00810858 / D_0081085C as
  `D_00810850[2]` / `[3]` and D_008104D0 / D_008104D8 as `D_008104D0[0]` /
  `[2]` keep the zero stores after the 100.0 stores (0x827250, 0x827040).
- **Extern reads, literal stores** for D_00810702 / D_0081080D in 0x826310
  (AREA13 note).
- **Declaration order** puts the packet handle and the work pointer in the
  original's saved registers (0x824CF0, 0x829F40).

### NEARMISS functions

- **0x825B00 (97.59%, same size).** The AREA19 0x824A90 C with
  func_001CFAE0(pkt, 0, +0xD0, +0x1F0, +0x1F4, 1.0, 0.1): the original
  materialises 0.1 (f15) first and 1.0 (f14) after the +0x1F0 load; mwcc
  2.3.3 loads 1.0 first. Locals, int staging and argument spellings were
  tried.
- **0x825C70, resolved by lane OVLC (2026-10-01).** It was 99.23%: in the
  second r == 1 test the original leaves the branch slot empty and mwcc
  2.3.3 speculated an address half into it. The else branch read the hit
  record pointer as `*(unsigned char **)0x700031D0`; reading it through
  `extern unsigned char *D_700031D0;` keeps the slot empty (the extern /
  literal lever of the AREA14 notes), and the function is now
  byte-identical and compiled.
- **0x8273C0 (96.50%, same size).** The original builds the func_001FC3C0
  block pointer (self + 0x1F0) + 8 last, in the call's slot, and
  materialises 150.0 first (also in the jump-table default slot); mwcc 2.3.3
  computes the pointer first. Pointer locals, casts and int-staged floats
  were tried. Its sibling 0x827640 (same shape, no call) is byte-identical.
- **0x82A1D0, resolved by lane DMATCH (2026-10-02; was 97.50%).** The target
  +0x1F4 is read through a `volatile` load into a local before the
  func_0011DF78 call, which makes mwcc 2.3.3 reload +0xC4 after its store as
  the original does, and the new target is stored inside the compare
  (`(*target = ...) > +0xC4`), which keeps the stored value in its register
  as the original does. Byte-identical and compiled. The status table above
  still lists it as NEARMISS; this entry supersedes it (docs/LEVELS_DECOMP.md
  section 7). Two NEARMISS remain: 0x825B00 and 0x8273C0.
- **Equivalence.** Each was diffed against the original instruction by
  instruction (`overlay_match.py check AREA21 .. --show`): the differences
  are the constant / pointer scheduling and the reload-versus-forward
  listed; calls, stores and control flow are
  equal.

## Verification

- `overlay_match.py check AREA21 src/overlays/AREA21/*.c`: 38 files 100.00
  BYTE-IDENTICAL, 0x826BB0 / 0x826E60 / 0x827640 at 99.98-99.99
  (`rodata-needs-pin` only), the 4 NEARMISS as above. Lane OVLC
  (2026-10-01): 0x825C70 is now byte-identical (40 files 100.00 including
  it), 3 NEARMISS remain; its locked rebuild (`compile_overlay_src.py
  AREA21`, 43 objects, 3 NEARMISS skipped; `tools/overlay/build.py --area
  AREA21`: PASS, full file byte-identical) and the gate are recorded in
  docs/AREA17_OVERLAY.md, and build/ovlc/prov.log shows every linked
  AREA21 object equal to its compiled `.text` outside relocation fields.
- `python3 tools/check_no_disassembly.py src/overlays/AREA21/*.c`: clean.
- Bounded mutation sweep (build/a03c/mut): 0x827CD0 `0x33E` -> `0x33F`
  (99.99). With AREA03's 0x826270 104.5 -> 105.5 (99.96) and AREA14's
  0x825FD0 221.6 -> 221.7 (99.99), no mutant stays byte-identical.
- Under the decomp build lock: `compile_overlay_src.py AREA21` (42 objects,
  4 NEARMISS skipped), then `tools/overlay/build.py --area AREA21
  --no-extract --no-yaml --no-splat`: PASS, AREA21.BIN equals the extracted
  file (42880 bytes, full file). Every linked filler object's `.text`
  equals the compiled `.text` outside relocation fields (build/a03c/prov.py,
  log build/a03c/prov.log: 75 objects of AREA03 / AREA14 / AREA21, all
  zero non-relocation differences). `config/overlays/AREA21.lds` was
  regenerated (absolute definitions of the new externs, three pinned
  tables).

## Gate

One locked run for the three areas (build/a03c/gate.sh full with AREAS =
AREA03 AREA14 AREA21; lock acquired 06:53:31, done 07:04:56; log
build/a03c/gate_full.log, decomp log build/a03c/decomp_build.log):
`compile_overlay_src.py` and `tools/overlay/build.py --area ... --no-extract
--no-yaml --no-splat` for AREA03, AREA14 and AREA21 (each "1/1 overlays
passed", full files byte-identical), full `tools/decomp/build.py build`
(rc 0, 2211 units), then `tools/verify_all.py`: all six stages PASS. The
boot ELF is byte-identical (0x175b00 loadable bytes), 19/19 overlays pass,
matched_code is 98.62% (2152/2211), and glTF, selftest and gs-offset pass.
Overlay C is not an objdiff unit, so matched_code does not count this
lane. Earlier locked overlay-only runs (build/a03c/gate.log for AREA03 /
AREA14, build/a21c/gate.log for AREA21) gave the same PASS.

## Binding

The native port has no AREA21 module, and this lane edited nothing in the
port. For a port binding the C above is the ground truth; the NEARMISS
bodies are correct apart from the listed scheduling. The three functions
left in assembly (0x823B40, 0x8243B0 [52], 0x824FE0) have no C yet.

## Known gaps

- 0x823B40 (0x868), 0x8243B0 (0x93C, placement [52]: a 14-step
  func_001CFB50 packet loop over the tables 0x82AC90 / 0x82AD70 with
  per-step groups chosen by the step number, and flags from D_008101E4 /
  D_0081024E)
  and 0x824FE0 (0xB18) are not decompiled; they stay splat assembly.
- Three NEARMISS functions (above; 0x825C70 was resolved by lane OVLC).
  The entry pad is an asm body.
- The data section (0x3900 bytes) is linked from splat's data assembly.
- No AREA21 capture exists; the D_0081080E sequence, the D_008102B0 keys of
  [61] / [62] and the ending branch are read from the code only.
- The verification cites git-ignored scratch in build/a03c and build/a21c
  (gate scripts and logs, prov.py, prov.log, hidden.py, mut/, try/, nm/);
  they must not be deleted while this doc cites them.
