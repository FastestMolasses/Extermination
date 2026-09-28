# AREA01 overlay C, lane A01C (2026-09-28)

This is the state after the lane that follows commit bdd40fb. Addresses are
runtime addresses. Splat and link names are 0x40 lower (see docs/OVERLAYS.md).
Every C file is mwcc 2.3.3 (`// COMPILER: mwcc233`).

| Runtime | File | Status | objdiff (overlay_match.py) |
|---|---|---|---|
| 0x823580 | overlay_AREA01_func_00823540.c | byte-identical, links from C (jump table pinned) | 100 with the table placed (was NEARMISS 99.99) |
| 0x823A90 | func_overlay_AREA01_00823A50.c | byte-identical, links from C | 100 (was NEARMISS 99.87) |
| 0x823CD0 | func_overlay_AREA01_00823C90.c | byte-identical, links from C (two jump tables pinned) | 100 with the tables placed (was NEARMISS 99.98) |
| 0x824340 | func_overlay_AREA01_00824300.c | byte-identical, links from C (jump table pinned) | 100 with the table placed (was NEARMISS 99.99) |
| 0x824770 | func_overlay_AREA01_00824730.c | byte-identical, links from C | 100 (was NEARMISS 99.08) |
| 0x826D40 | func_overlay_AREA01_00826D00.c | byte-identical, links from C | 100 (was assembly) |
| 0x8282F0 | func_overlay_AREA01_008282B0.c | byte-identical, links from C | 100 (was NEARMISS 89.86) |

AREA01 now has 40 of its 41 functions as linked C plus the asm-void entry
pad, and no NEARMISS: every code object is copied from `obj/` (41 of 41).
`link_overlay.py AREA01` passes: the whole file is byte-identical. (Lane
JTLINK, 2026-09-28, promoted the three dispatchers; see the next section.)
When lane A01C finished, 38 objects were copied from `obj/`. The four new objects
were compared with their linked filler objects. They differ only in the
GPREL16 fields that were applied beforehand and in the zero padding at the
end.

## The three jump-table dispatchers link from C (lane JTLINK)

For 0x823580, 0x823CD0 and 0x824340 the instructions compiled from the C are
byte-identical, and so are the four jump tables (runtime 0x82CB80, 0x82CBA0,
0x82CBD0, 0x82CBF0), under two conditions: each table sits at the address the
original's lui/addiu pair builds, and each entry is resolved at its runtime
address (link + 0x40). Lane A01C found this with a scratch resolver and left
the three as NEARMISS because the overlay link did neither: it put a compiled
`.rodata` after the data section and resolved everything at link addresses,
0x40 low.

`tools/overlay/link_overlay.py` now does both (`tools/overlay/jt_pin.py`; the
rule is in docs/OVERLAYS.md section 6). It proves each table's runtime address
from the original instruction pair, requires the resolved table to equal the
original bytes, adds the +0x40 run bias to the table-address and entry
addends of a link copy of the object, and splits the data-section object so
the compiled table fills its original span. The ld map confirms the four
tables at link 0x82CB40, 0x82CB60, 0x82CB90 and 0x82CBB0. With the markers
removed, `link_overlay.py AREA01` passes (full file byte-identical). The
pre-change link tool on the same compiled objects fails (8 bytes: the
table-address %lo fields, 0x40 low), so the pass comes from the compiled
objects and the new placement.

## Idioms that closed the residuals (see docs/fanout/MATCHING_GUIDE.md)

- **idiom-32 (relocated scratchpad externs).** Three functions needed it:
  - 0x823A90: writing `D_700036A0[i] = D_700038A0[i]` restores the original
    load order A0/A4/A8.
  - 0x826D40: the state-1 reads of 0x70003610/0x70003618 keep the
    unfilled branch delay slots. The hit-block stores 0x700038A0..BC and the
    0x700031D8 load settle the a0/a1 colouring (found by a combination
    sweep).
  - 0x8282F0: most scratchpad accesses are externs. The four integer
    colour words and the 0x700031D4 record read only match as literal
    addresses.
- **idiom-31 (staged float argument).** 0x824770 needs
  `int degrees = 4; float rate = (float)degrees * 0.0174532925f;` for the
  first turn-rate argument.
- **idiom-24 (int-staged zero).** 0x824340 needs it for the second zero
  argument of `func_00128830` in state 2.
- **idiom-25 (float truthiness).** 0x823CD0 uses `if (x)` on +0xD8.
- **Statement-split random scaling** (as in src/func_001C1A80.c). 0x826D40
  writes it as `x = rand >> 16; x *= m; x >>= 15;`.
- **Pointer-to-record locals.** In 0x826D40 the side step reads
  `D_00275B40 + 8` once into `unsigned char **pa`, and the elevation step
  reads `D_00275B40` once into a local. The original re-reads the record
  pointer through those locals.
- **Branch layout.** mwcc lays out the then-block as the fall-through. The
  original's layouts need these forms:
  - `>` / `>=` / `!=` conditions with the swapped arms (`timer > 0x1E`,
    `f200 > 0xD`, `phase != -pi/2`);
  - `if (f) {...} else hit = 0;`;
  - `else if (in range) hit = 1; else hit = 2;`.

## Semantics now fixed by byte identity

The earlier NEARMISS header of 0x8282F0 described its hit classification as
reconstructed. That classification is now fixed by byte identity:

- **Code 1:** `func_0019A570` succeeds and either:
  - `func_00102738` of the offset is above 10000.0, or
  - 0x700031D8 is 1 and byte +3 of the 0x700031D4 record is in 0x10..0x13.
- **Code 2:** any other result after a hit, including the
  `func_0019AA80`-only case.
- **Code 0:** there was no hit. 0x700031D0..D8 are restored.

The return value is 0 unless the code is 2. For code 2 it is 2 when +0x204
equals 0x700031D4, and 1 otherwise. This matches the old C's logic.
