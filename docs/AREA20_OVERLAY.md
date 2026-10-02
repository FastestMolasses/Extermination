# AREA20 overlay — decompilation status (lane OVLC, 2026-10-01)

## What AREA20 is

AREA20 is reached from AREA04 sub 1 ([42] / [48], gated by D_00810845 bit
4, which AREA06 sub 1's 0x825E20 sets with flag 0x31); its door [2] leads
back to AREA04 entry 5 and its [32] 0x8239B0 requests AREA21 entry 3
(counter 0x34 and a quad) (docs/WORLD_GRAPH.md, the AREA20 table and
section 7 step 7, inferred, not played). One sub-state, lock byte
D_00810855. This lane did not run the game; the behaviour below is read
from the C.

The code module is `OVERLAY/AREA20.BIN`: overlay id 17, text 0x2BC0, data
0x1E00, bss 0x140C00, file 18944 bytes. Linked at 0x00823500, it runs 0x40
higher (link names are the runtime address minus 0x40; data symbols carry
their runtime address). The boot area dispatcher func_001E7780 calls the
init at runtime 0x823990 (link 00823950, earlier C).

## Split handling

`overlay_match.py list AREA20` regroups the 19 splat pieces into 15
functions. Four groups hold one later piece each, fake splits at an
intra-overlay call target (`[fill] 4 splat piece(s) absorbed by 4 compiled
function(s)`). AREA20 has no jump table.

## Status

15 functions: 14 are compiled C that links byte-identical (13 new in this
lane plus the init); the entry pad stays an asm body. The hybrid asm-body
files 00823BA0 and 00825970 were replaced by C.

| link name | runtime | size | status | reached by (static, area_overview.py --area 20) |
|---|---|---|---|---|
| func_overlay_AREA20_00823500 | 0x823540 | 0x4 | asm pad | nop sled, no code |
| overlay_AREA20_func_00823540 | 0x823580 | 0x74 | C, byte-identical | - (no static reference) |
| func_overlay_AREA20_008235C0 | 0x823600 | 0x58 | C, byte-identical | - (no static reference) |
| func_overlay_AREA20_00823620 | 0x823660 | 0x120 | C, byte-identical | - (no static reference) |
| func_overlay_AREA20_00823740 | 0x823780 | 0xB4 | C, byte-identical | - (no static reference) |
| func_overlay_AREA20_00823800 | 0x823840 | 0x14C | C, byte-identical | - (no static reference) |
| func_overlay_AREA20_00823950 | 0x823990 | 0x20 | C, byte-identical (earlier; init) | boot 0x1E7780 |
| func_overlay_AREA20_00823970 | 0x8239B0 | 0x150 | C, byte-identical | sub0 place [32] |
| func_overlay_AREA20_00823AC0 | 0x823B00 | 0xD4 | C, byte-identical (absorbs 00823B00) | call from 0x8239B0 |
| func_overlay_AREA20_00823BA0 | 0x823BE0 | 0x144 | C, byte-identical (was asm body) (absorbs 00823BE0) | call from 0x8239B0 |
| func_overlay_AREA20_00823CF0 | 0x823D30 | 0x710 | C, byte-identical | sub0 place [19]..[31] |
| func_overlay_AREA20_00824400 | 0x824440 | 0x1568 | C, byte-identical | sub0 deferred group 0x826100 [12], [14] |
| func_overlay_AREA20_00825970 | 0x8259B0 | 0x4D4 | C, byte-identical (was asm body) (absorbs 008259B0) | call from 0x824440 |
| func_overlay_AREA20_00825E50 | 0x825E90 | 0x84 | C, byte-identical (absorbs 00825E90) | call from 0x824440 |
| func_overlay_AREA20_00825EE0 | 0x825F20 | 0x198 | C, byte-identical | sub0 deferred group 0x826100 [13], [15] |

Every new file is mwcc 2.3.3 (`-O4,p`, `-sdatathreshold 0`;
`-sdatathreshold 4` for the AREA14-family 0x824440 and 0x8259B0, which read
D_00275B40).

## Behaviour (from the C)

- **[32] 0x8239B0, the AREA21 request (counter 0x34).** Spawn: state 3 when
  flag 0x34 is set, else the model setup (func_001B10B0 0x5B). While
  D_0081080C (counter 0x34) is 0, 0x823B00 talks: func_001FABB0(), script
  0x826830, at its end func_001FB0B0(0) and D_0081080C = 1. Then 0x823BE0
  waits for the player in the area 0x8273B0 (func_001B1EA0 kind 4, a
  64-byte area copied to the stack): D_0081080C = 2, script 0x8270B0, and
  at its end func_001B0C60(0x15, 0, 3) (AREA21 entry 3). A func_001F9100
  marker sits at the object each frame.
- **[19]..[31] 0x823D30, breakable objects.** A shot (+0x36) breaks them
  (effect 4 on the hit when func_0019A570 finds one along +-4 in y); the
  burst plays effects 0x80000013 and 0x8000001C (kinds 0x18 / 0x2A, sound
  0x1A1, which end there) or 0x8000002E (sound 0x19F); the others fall with
  a random heading (kind 10: any of 16 directions; kind 12: +0xC4 plus up
  to 31 degrees), speed and lift from the tables 0x827E70 / 0x827E80,
  stopping at walls (func_0019AD00) and bursting again on landing
  (func_0019AB20, sound 0x1A0).
- **The AREA14 family.** 0x824440 is the AREA14 0x823FC0 C with one change:
  a func_0019B6C0 miss after a hit record of kind 8 gives effect 0x80000067
  (AREA14: kind 5, 0x8000002C). 0x8259B0 / 0x825E90 / 0x825F20 are the
  AREA14 0x825530 / AREA19 0x829840 / AREA01 0x828850 C with the colour
  vectors 0x827E90 / 0x827EA0.
- **Effects** with no static reference: 0x823580 / 0x823600 (effects each
  frame), 0x823660 / 0x823840 (func_001D04B0 packets, tables 0x826680,
  0x826710 / 0x8267A0 by +0x94), and the emitter 0x823780 (effect 1 on
  +0xD0 every 5th frame, every 25th while func_001C6190 is in 151..449; the
  AREA21 0x8237B0 shape).

## Matching notes (mwcc 2.3.3)

- **Twins first.** Four functions are the AREA14 / AREA19 / AREA01 C with
  the data symbols renamed (build/ovlc/twin.py aligns the two splat files
  and maps the symbols); 0x824440 also needed the two immediates above.
- **A local for one product** (`float a = (float)(rand & 0x1F); ...
  3.1415927f * a / 180.0f`) gives 0x823D30's early +0xC4 load.
- **`v > 150`** (not `v >= 151`) gives 0x823780's compare into the
  assembler temporary.
- **The area struct copy** (`Area area = D_overlay_AREA20_008273B0;`,
  16 floats, 16-byte aligned) reproduces 0x823BE0's quadword copy.

## Verification

- `overlay_match.py check AREA20 src/overlays/AREA20/*.c`: all 15 files
  100.00 BYTE-IDENTICAL (13 new, the init and the pad's object).
- `python3 tools/check_no_disassembly.py src/overlays/AREA20/*.c`: clean.
- Mutation: 0x823D30 gravity 0.06 -> 0.07 loses byte identity.
- Under the decomp build lock: `compile_overlay_src.py AREA20` (15 objects),
  then `tools/overlay/build.py --area AREA20 --no-extract --no-yaml
  --no-splat`: PASS, AREA20.BIN equals the extracted file (18944 bytes,
  full file). Every linked filler object's `.text` equals the compiled
  `.text` outside relocation fields (build/ovlc/prov.log).
  `config/overlays/AREA20.lds` was regenerated.

## Gate

See "Gate" in docs/AREA17_OVERLAY.md.

## Binding

The native port has no AREA20 module, and this lane edited nothing in the
port. For a port binding the C above is the ground truth.

## Known gaps

- The entry pad is an asm body; the data section (0x1E00 bytes) is linked
  from splat's data assembly.
- No AREA20 capture exists; the AREA21 request and the breakables are read
  from the code only.
- The verification cites git-ignored scratch in build/ovlc (see
  docs/AREA07_OVERLAY.md).
