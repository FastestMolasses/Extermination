# AREA01 overlay C, lane A01C (2026-09-28)

This is the state after the lane that follows commit bdd40fb. Addresses are
runtime addresses. Splat and link names are 0x40 lower (see docs/OVERLAYS.md).
Every C file is mwcc 2.3.3 (`// COMPILER: mwcc233`).

| Runtime | File | Status | objdiff (overlay_match.py) |
|---|---|---|---|
| 0x823580 | overlay_AREA01_func_00823540.c | NEARMISS (jump table) | 99.99 |
| 0x823A90 | func_overlay_AREA01_00823A50.c | byte-identical, links from C | 100 (was NEARMISS 99.87) |
| 0x823CD0 | func_overlay_AREA01_00823C90.c | NEARMISS (two jump tables) | 99.98 |
| 0x824340 | func_overlay_AREA01_00824300.c | NEARMISS (jump table) | 99.99 |
| 0x824770 | func_overlay_AREA01_00824730.c | byte-identical, links from C | 100 (was NEARMISS 99.08) |
| 0x826D40 | func_overlay_AREA01_00826D00.c | byte-identical, links from C | 100 (was assembly) |
| 0x8282F0 | func_overlay_AREA01_008282B0.c | byte-identical, links from C | 100 (was NEARMISS 89.86) |

AREA01 now has 37 of its 41 functions as linked C, 3 NEARMISS, and the
asm-void entry pad. `link_overlay.py AREA01` passes: the whole file is
byte-identical, and 38 objects are copied from `obj/`. The four new objects
were compared with their linked filler objects. They differ only in the
GPREL16 fields that were applied beforehand and in the zero padding at the
end.

## Why the three dispatchers stay NEARMISS

For 0x823580, 0x823CD0 and 0x824340, the instructions compiled from the C
are byte-identical. The jump tables are byte-identical too, but only when two
conditions hold:

- the table is placed at the address that the original's lui/addiu pair
  builds;
- each entry is resolved at its runtime address (link + 0x40).

The overlay link does neither. `link_overlay.py` puts a compiled `.rodata`
after the data section, and it resolves the table address and the entries at
link addresses, which are 0x40 below the values the original stores. A tool
change would turn all three into linked C. The change would place a compiled
local jump table at the runtime address the original code uses, and resolve
its `.text` entries with a +0x40 bias. The other option is to link overlays
at load + 0x40. This lane did not make either change, because
`tools/overlay/` is outside its scope. The check used a scratch resolver,
`build/a01c/jtcheck.py`, which is not committed.

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
