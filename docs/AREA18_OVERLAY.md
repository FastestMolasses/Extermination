# AREA18 overlay — decompilation status (lane OVLC, 2026-10-01)

## What AREA18 is

AREA18 is the small room between AREA14 and AREA17: the boot function
00190F20 moves the player between AREA14 entry 1 and AREA18 entry 0 (in
AREA18 when the player's x, +0xA0, is at most 285), and its pickup g[1] is
item 0x28, which AREA17's NPC takes (docs/WORLD_GRAPH.md, the AREA18 table
and sections 5 / 7, inferred, not played). One sub-state, lock byte
D_00810853; its one door [18] is a room move. All of its behaviour runs in
boot code (doors, pickups, the panel [21]); the overlay holds no actor
code.

The code module is `OVERLAY/AREA18.BIN`: overlay id 15, text 0xC0, data
0x780, bss 0x140C00, file 2176 bytes.

## Status

`overlay_match.py list AREA18` finds one group: the 64-byte entry pad
(link 00823500, an asm body, as in every overlay) holding the piece
00823540, the init at runtime 0x823580 (`overlay_AREA18_func_00823540.c`,
earlier C: it stores 0x20 in D_00275C28, the data end 0x823D80 in
D_00275C1C and zero in D_00275C2C / D_00275C24, the same shape as the
other overlays' inits that the boot dispatcher func_001E7780 calls; the
static scan of area_overview.py lists only the pad group here, so the
caller is not resolved for AREA18). There is nothing else to decompile;
this lane changed no AREA18 file.

## Verification

The earlier C is unchanged; the full gate of this lane (docs/AREA17_OVERLAY.md,
"Gate") rebuilt all 19 overlays byte-identical through `tools/verify_all.py`.

## Known gaps

- The entry pad is an asm body (as in every overlay); the data section
  (0x780 bytes) is linked from splat's data assembly.
