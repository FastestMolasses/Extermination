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
| overlay_AREA00_func_00823540 | 0x823580 | 0x294 | C, byte-identical; jump table pinned by the link (was NEARMISS, lane JTLINK) |
| func_overlay_AREA00_008237E0 | 0x823820 | 0x430 | C, byte-identical (was NEARMISS 94.22%, lane A00NM) |
| func_overlay_AREA00_00823C10 | 0x823C50 | 0x98 | C, byte-identical |
| func_overlay_AREA00_00823CB0 | 0x823CF0 | 0x11C | C, byte-identical |
| func_overlay_AREA00_00823DD0 | 0x823E10 | 0x98 | C, byte-identical |
| func_overlay_AREA00_00823E70 | 0x823EB0 | 0x274 | C, byte-identical (was NEARMISS 94.38%, lane A00NM) |
| func_overlay_AREA00_008240F0 | 0x824130 | 0x78 | C, byte-identical |
| func_overlay_AREA00_00824170 | 0x8241B0 | 0x608 | C, byte-identical (was NEARMISS 99.84%, lane A00NM) |
| func_overlay_AREA00_00824780 | 0x8247C0 | 0x8 | C, byte-identical (earlier) |
| func_overlay_AREA00_00824790 | 0x8247D0 | 0x3E0 | C, byte-identical (was NEARMISS 99.74%, lane A00NM) |
| func_overlay_AREA00_00824B70 | 0x824BB0 | 0x248 | C, byte-identical |
| func_overlay_AREA00_00824DC0 | 0x824E00 | 0x3C | C, byte-identical (earlier) |
| func_overlay_AREA00_00824E00 | 0x824E40 | 0x58 | C, byte-identical (was asm) |
| func_overlay_AREA00_00824E60 | 0x824EA0 | 0x2D0 | C, byte-identical |
| func_overlay_AREA00_00825130 | 0x825170 | 0x270 | C, byte-identical; jump table pinned by the link (was NEARMISS, lane JTLINK) |
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

Totals: 33 byte-identical C (34 compiled objects with the pad), 0 NEARMISS,
1 asm pad: the whole overlay except the entry pad links from C. Every
compiled object is what the link uses (`fill_overlay.py` copies obj/; the
four A00NM objects' linked .text equals the compiled .text plus zero
padding). Lane A00C ended at 27 and 6; lane JTLINK (2026-09-28) linked the
two jump-table dispatchers from C; lane A00NM (2026-09-28) matched the last
four (compile_overlay_src AREA00 + build.py --area AREA00: PASS, full file
byte-identical).

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
- A float min clamp whose bc1t delay slot the original leaves empty is the
  ternary `d = (d > 1.0f) ? 1.0f : d;` (0x8247D0, as in func_001CD2B0); the
  if-statement form lets mwcc 2.3.3 copy the join's first constant into the
  slot.
- Loop counters: 0x8241B0 uses one counter variable for all seven loops; a
  second counter changes the argument registers of the state 0 loop.
- func_001CD520 argument scheduling (0x823820): declare the colour word before
  the three floats, `(int, int, void *, u64, unsigned int rgba, float, float,
  float)`, the order the matched func_001F1F60 uses. Registers are the same
  (t0 and f12-f14); only the evaluation order changes.
- 0x823EB0: accumulate the colour word in one local
  (`c = (b >> 7) << 16; c |= (g >> 7) << 8; c |= r >> 7;`), and stage the 0.1
  argument of func_001CFA60 from a block-local integer
  (`int one = 1; float arg = (float)one * 0.1f;`, idiom-31) so the 0.1 is
  materialized before the 1.0.

## Jump tables

0x823580 and 0x825170 dispatch `+5` through tables at 0x82D400 (8 entries)
and 0x82D420 (7 entries). Both link from C. `tools/overlay/link_overlay.py`
(with `tools/overlay/jt_pin.py`, rule in docs/OVERLAYS.md section 6) places
each compiled `.rodata` at the table's original address, link 0x82D3C0 and
0x82D3E0 (confirmed by the ld map), and resolves the table address and the
entries at link + 0x40. `link_overlay.py AREA00` passes with the whole file
byte-identical.
