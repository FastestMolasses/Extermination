# AREA00 overlay — decompilation status

AREA00 is the area the AREA01 shaft door leads to (AREA00 sub 0 entry 0, see
FINDINGS "Exit to AREA00"). Its code module is `OVERLAY/AREA00.BIN` (text 0x3A40
bytes). Like every overlay it is linked at 0x00823500 but runs 0x40 higher (the
MWo3 header is loaded first), so splat names are the runtime address minus 0x40
and an intra-overlay call target lands 0x40 into the callee
(`tools/overlay/overlay_match.py list AREA00` regroups the pieces).

AREA00 has 36 splat pieces in 34 real functions; only two pieces were fake
splits (00825E80 inside 00825E40, 00825FC0 inside 00825F80). Both are now
covered by one compiled object each (`filler/_absorbed.json`).

## Status (2026-09-28, lane A00C)

No AREA00 route capture exists yet, so the order was largest-first. All 34
functions have C. The overlay link is byte-identical (`tools/overlay/build.py
--area AREA00`: PASS; `verify_all`: 19/19 overlays, boot ELF byte-identical,
2150/2211 matched_code, unchanged because overlay C is not an objdiff unit).

| link name | runtime | size | status |
|---|---|---|---|
| func_overlay_AREA00_00823500 | 0x823540 | 0x4 | asm pad (entry nop) |
| overlay_AREA00_func_00823540 | 0x823580 | 0x294 | NEARMISS: jump table; text and table byte-identical under runtime placement |
| func_overlay_AREA00_008237E0 | 0x823820 | 0x430 | NEARMISS 94.22% (func_001CD520 argument scheduling) |
| func_overlay_AREA00_00823C10 | 0x823C50 | 0x98 | C, byte-identical |
| func_overlay_AREA00_00823CB0 | 0x823CF0 | 0x11C | C, byte-identical |
| func_overlay_AREA00_00823DD0 | 0x823E10 | 0x98 | C, byte-identical |
| func_overlay_AREA00_00823E70 | 0x823EB0 | 0x274 | NEARMISS 94.38% (argument scheduling) |
| func_overlay_AREA00_008240F0 | 0x824130 | 0x78 | C, byte-identical |
| func_overlay_AREA00_00824170 | 0x8241B0 | 0x608 | NEARMISS 99.84% (argument-register choice in one loop) |
| func_overlay_AREA00_00824780 | 0x8247C0 | 0x8 | C, byte-identical (earlier) |
| func_overlay_AREA00_00824790 | 0x8247D0 | 0x3E0 | NEARMISS 99.74% (one delay slot) |
| func_overlay_AREA00_00824B70 | 0x824BB0 | 0x248 | C, byte-identical |
| func_overlay_AREA00_00824DC0 | 0x824E00 | 0x3C | C, byte-identical (earlier) |
| func_overlay_AREA00_00824E00 | 0x824E40 | 0x58 | C, byte-identical (was asm) |
| func_overlay_AREA00_00824E60 | 0x824EA0 | 0x2D0 | C, byte-identical |
| func_overlay_AREA00_00825130 | 0x825170 | 0x270 | NEARMISS: jump table; text and table byte-identical under runtime placement |
| func_overlay_AREA00_008253A0 | 0x8253E0 | 0x94 | C, byte-identical (was asm) |
| func_overlay_AREA00_00825440 | 0x825480 | 0x174 | C, byte-identical |
| func_overlay_AREA00_008255C0 | 0x825600 | 0x318 | C, byte-identical |
| func_overlay_AREA00_008258E0 | 0x825920 | 0x35C | C, byte-identical |
| func_overlay_AREA00_00825C40 | 0x825C80 | 0xEC | C, byte-identical |
| func_overlay_AREA00_00825D30 | 0x825D70 | 0x104 | C, byte-identical |
| func_overlay_AREA00_00825E40 | 0x825E80 | 0x13C | C, byte-identical (was asm; absorbs 00825E80) |
| func_overlay_AREA00_00825F80 | 0x825FC0 | 0xEC | C, byte-identical (absorbs 00825FC0) |
| func_overlay_AREA00_00826070 | 0x8260B0 | 0x40 | C, byte-identical (earlier) |
| func_overlay_AREA00_008260B0 | 0x8260F0 | 0xE8 | C, byte-identical |
| func_overlay_AREA00_008261A0 | 0x8261E0 | 0xEC | C, byte-identical |
| func_overlay_AREA00_00826290 | 0x8262D0 | 0xEC | C, byte-identical |
| func_overlay_AREA00_00826380 | 0x8263C0 | 0x2D4 | C, byte-identical |
| func_overlay_AREA00_00826660 | 0x8266A0 | 0xE8 | C, byte-identical |
| func_overlay_AREA00_00826750 | 0x826790 | 0x44C | C, byte-identical |
| func_overlay_AREA00_00826BA0 | 0x826BE0 | 0xC4 | C, byte-identical |
| func_overlay_AREA00_00826C70 | 0x826CB0 | 0x10 | C, byte-identical (earlier) |
| func_overlay_AREA00_00826C80 | 0x826CC0 | 0x294 | C, byte-identical |

Totals: 27 byte-identical C (28 compiled objects with the pad), 6 NEARMISS
(2 of them only blocked by the jump-table link), 1 asm pad. Every compiled
object is what the link uses (`fill_overlay.py`: 28 copied from obj/).

The role lines in each source header describe what the instructions do; they
are not placement labels. Which placement record uses which function has not
been captured yet.

## Matching notes (mwcc 2.3.3, all AREA00 functions)

- The `+4` state switches compare cases in reverse source order: write
  `case 0, 1, 2, 3` to get the 3, 2, 1, 0 compare chain.
- When the original keeps `self + 0x1F0` in a saved register, write every
  access of that block through the pointer (`blk + off`); mwcc folds
  `self + 0x1F0 + off` otherwise. To keep two separate adds for a second
  derived pointer, use int-pointer arithmetic (`(int *)(self + 0x1F0) + 0x3F`).
- Random fraction with the seed in a register (0x824BB0, 0x8247D0):
  `r = (float)((seed >> 16) & 0xFFFF); r = r / 65535.0f; r += 0.0001f;
  seed = seed * 37 + 11;` then pass `r`. Other orders change the FP registers.
- A scratchpad global written as an extern symbol keeps its `lui` out of a
  branch delay slot; a raw `*(T *)0x7000xxxx` lets mwcc hoist it. Pick the form
  the original shows (0x824EA0 needed the symbol).
- `if ((e = f()) != 0)` tests the return register before the copy to a saved
  register; `e = f(); if (e != 0)` tests the copy.
- A flags byte read into an `unsigned char` local adds a mask; use `int`.
- A store that must survive dead-store elimination (0x823EB0) is a
  `volatile` scratchpad store.
- 0x8247D0 stores the runtime address of the empty function at link 0x824780
  (0x8247C0) as data. A C reference to the function would link 0x40 low; the
  NEARMISS source uses the absolute name `D_008247C0`.
- Local 16-byte-aligned struct copies (`lq`/`sq`) come from a struct with
  `__attribute__((aligned(16)))` assigned from an extern of that type.

## Jump tables

0x823580 and 0x825170 dispatch `+5` through tables at 0x82D400 (8 entries)
and 0x82D420 (7 entries). Their C was checked with a scratch resolver that
places the compiled `.rodata` at the table address and resolves the entries at
link + 0x40: text and table are both byte-identical. They stay NEARMISS until
the overlay link can place a compiled jump table (same blocker as AREA01).
