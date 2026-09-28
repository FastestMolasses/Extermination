# AREA04 overlay — decompilation status (lane A04C, 2026-09-28)

AREA04 is the level after AREA02 (game order AREA11 -> AREA01 -> AREA00 ->
AREA01 with the bridge lowered -> AREA02 -> AREA04). Its code module is
`OVERLAY/AREA04.BIN` (overlay id 5, text 0x30C0, data 0x6300, bss
0x14AC80). Like every overlay it is linked at 0x00823500 but runs 0x40
higher (the MWo3 header is loaded first), so splat and link names are the
runtime address minus 0x40, and an intra-overlay call lands 0x40 into the
callee. `tools/overlay/overlay_match.py list AREA04` regroups the 39 splat
pieces into 31 "functions": 8 of them hold a fake split (the later piece is
the call target 0x40 in), and one of those, 00825240, really holds two
functions (below). AREA04 has no jump tables (no register-indirect jump
other than the return), so `tools/overlay/jt_pin.py` has nothing to pin.

No AREA04 route capture was used; every function was done, in file order.

## Status

Of the 32 real functions, 31 have C that compiles byte-identical and 1 is
the asm entry pad. 29 C files link from their compiled objects (27 new in
this lane, plus 00823AD0 and 00824A00 from earlier). The 30th C file,
`func_overlay_AREA04_00825240.c`, holds two byte-identical functions and is
marked NEARMISS for a link-tool reason, so the link assembles them from
their two splat pieces. The hybrid asm files for 00824450 and 00825000 were
replaced by C.

| link name | runtime | size | pieces | status |
|---|---|---|---|---|
| func_overlay_AREA04_00823500 | 0x823540 | 0x4 | 00823500 | asm pad (entry nop, unchanged) |
| overlay_AREA04_func_00823540 | 0x823580 | 0x178 | 00823540+00823580 | C, byte-identical (new) |
| func_overlay_AREA04_008236C0 | 0x823700 | 0x220 | 008236C0 | C, byte-identical (new) |
| func_overlay_AREA04_008238E0 | 0x823920 | 0x7C | 008238E0 | C, byte-identical (new) |
| func_overlay_AREA04_00823960 | 0x8239A0 | 0xF0 | 00823960 | C, byte-identical (new) |
| func_overlay_AREA04_00823A50 | 0x823A90 | 0x80 | 00823A50 | C, byte-identical (new) |
| func_overlay_AREA04_00823AD0 | 0x823B10 | 0x30 | 00823AD0 | C, byte-identical (earlier, default compiler) |
| func_overlay_AREA04_00823B00 | 0x823B40 | 0x4C | 00823B00 | C, byte-identical (new) |
| func_overlay_AREA04_00823B50 | 0x823B90 | 0x344 | 00823B50 | C, byte-identical (new) |
| func_overlay_AREA04_00823EA0 | 0x823EE0 | 0x220 | 00823EA0 | C, byte-identical (new, sdatathreshold 4) |
| func_overlay_AREA04_008240C0 | 0x824100 | 0xE4 | 008240C0 | C, byte-identical (new) |
| func_overlay_AREA04_008241B0 | 0x8241F0 | 0x130 | 008241B0 | C, byte-identical (new) |
| func_overlay_AREA04_008242E0 | 0x824320 | 0x170 | 008242E0 | C, byte-identical (new) |
| func_overlay_AREA04_00824450 | 0x824490 | 0x158 | 00824450+00824490 | C, byte-identical (was hybrid asm; absorbs 00824490) |
| func_overlay_AREA04_008245B0 | 0x8245F0 | 0xBC | 008245B0+008245F0 | C, byte-identical (new; absorbs 008245F0) |
| func_overlay_AREA04_00824670 | 0x8246B0 | 0x178 | 00824670 | C, byte-identical (new) |
| func_overlay_AREA04_008247F0 | 0x824830 | 0xFC | 008247F0+00824830 | C, byte-identical (new; absorbs 00824830) |
| func_overlay_AREA04_008248F0 | 0x824930 | 0x10C | 008248F0+00824930 | C, byte-identical (new; absorbs 00824930) |
| func_overlay_AREA04_00824A00 | 0x824A40 | 0x48 | 00824A00 | C, byte-identical (earlier, default compiler) |
| func_overlay_AREA04_00824A50 | 0x824A90 | 0x330 | 00824A50 | C, byte-identical (new) |
| func_overlay_AREA04_00824D80 | 0x824DC0 | 0x130 | 00824D80 | C, byte-identical (new) |
| func_overlay_AREA04_00824EB0 | 0x824EF0 | 0x14C | 00824EB0 | C, byte-identical (new) |
| func_overlay_AREA04_00825000 | 0x825040 | 0x178 | 00825000+00825040 | C, byte-identical (was hybrid asm; absorbs 00825040) |
| func_overlay_AREA04_00825180 | 0x8251C0 | 0xB4 | 00825180+008251C0 | C, byte-identical (new; absorbs 008251C0) |
| func_overlay_AREA04_00825240 | 0x825280 | 0x8C | 00825240 | C, byte-identical, in the NEARMISS file (leaf) |
| func_overlay_AREA04_008252D0 | 0x825310 | 0x200 | inside 00825280 | C, byte-identical, in the NEARMISS file |
| func_overlay_AREA04_008254D0 | 0x825510 | 0x370 | 008254D0 | C, byte-identical (new) |
| func_overlay_AREA04_00825840 | 0x825880 | 0x27C | 00825840 | C, byte-identical (new, sdatathreshold 4) |
| func_overlay_AREA04_00825AC0 | 0x825B00 | 0x254 | 00825AC0 | C, byte-identical (new) |
| func_overlay_AREA04_00825D20 | 0x825D60 | 0x90 | 00825D20 | C, byte-identical (new) |
| func_overlay_AREA04_00825DB0 | 0x825DF0 | 0x2D0 | 00825DB0 | C, byte-identical (new) |
| func_overlay_AREA04_00826080 | 0x8260C0 | 0x4F8 | 00826080 | C, byte-identical (new; last function) |

Every new file is mwcc 2.3.3 (`// COMPILER: mwcc233`, `-O4,p`,
`-sdatathreshold 0` unless noted). The role lines in each source header
describe what the instructions do. They are not placement labels: which
placement record uses which function has not been captured.

### The two-function slot 00825240 and fill_overlay.py

splat has no piece at link 0x008252D0 (runtime 0x825310), because nothing
calls it directly, so the slot 00825240..008254D0 holds a 0x8C-byte leaf
(ending in its return and one pad word) and a separate 0x200-byte function
with its own frame. `overlay_match.py true_functions` groups both as one
0x290-byte function (its `check` therefore scores only the leaf against
0x290 bytes, 21.34, by construction). Both functions are in
`func_overlay_AREA04_00825240.c` and both compile to exactly the original
bytes: checked per function against the original, since mwcc 2.3.3 emits
one `.text` section per function (scratch checker, build/a04c/chk3.py:
"func_overlay_AREA04_00825240 0x8c BYTE-IDENTICAL,
func_overlay_AREA04_008252D0 0x200 BYTE-IDENTICAL").

The link cannot take this object yet. `fill_overlay.py` reads only the
first `.text` of an object (`_obj_text_size` = 0x8C), so `plan_absorption`
would absorb 00825280 (slot 0x250), find a 0x204-byte remainder and stop
the build. The file therefore starts with `// NEARMISS`; the link
assembles the two splat pieces and the overlay stays byte-identical. To
link it from C, `_obj_text_size` must sum all `.text` sections of an object
(with their 16-byte alignment, which here gives exactly 0x8C + 4 + 0x200 =
0x290). That tool is outside this lane; with that change, removing the
marker should pass unchanged.

## Matching notes (mwcc 2.3.3)

- **Byte flags gp-relative at threshold 4.** 0x823EE0 needs `-sdatathreshold
  4` for the gp-relative `*D_00275B40`, but then 1- and 2-byte externs
  (D_00810764, D_00810701, D_008107E4, D_70003B84) turn gp-relative too.
  Declaring them as unsized arrays (`extern unsigned char D_00810764[];`,
  used as `[0]`) keeps them absolute; D_0028A59C is `int[]` for the same
  reason. 0x825880 takes `&D_00275928` gp-relative (threshold 4).
- **Hoisted talk block.** Functions whose original computes `self + 0x1F0`
  into a register at entry (even when only one case uses it) need
  `unsigned char *blk = self + 0x1F0;` at the top (0x8241F0: 92.83 -> 100).
- **idiom-24 in 0x8251C0**: `func_001C67E0(self, 0, 20.0f, 0.0f)` with the
  zero moved into f13 first needs `int zi = 0; float z = (float)zi;`
  (99.56 -> 100). 0x8245F0 has the same call and matches plainly, because
  its byte stores are scheduled between.
- **Stack copies in 0x824A90.** Four 16-byte points are copied to locals.
  The original order needs the locals declared without initialisers and
  copied by statements after the pointer declarations (`pos`, `sel`), with
  `cam = D_008101E0` assigned after the fourth copy; declaration order
  `pos`, `sel`, `cam` gives the s0/s1/s2 assignment. The inner
  `if (x < 215) sel = a; else sel = b;` keeps the branch-likely shape. The
  six D_008105D0/E0 element updates are literal-address stores (the extern
  array form lets mwcc overlap them).
- **Scratchpad vectors through the extern arrays (0x825510, 0x825880).** The
  position and color stores to 0x700038A0/B0 must be `D_700038A0[i]` /
  `D_700038B0[i]` in natural order; literal-address stores schedule
  differently (94.53 -> 100).
- **Per-case derived pointer (0x825B00).** The stop branches write the
  offset through `a0 + 0xC` / `v1 = a0 + 0xC` and the direction through a
  pointer made before the inner switch: `int *w = (int *)(self + 0x1F0);
  int *dp = w + 1;` and `op = (float *)(w + 3);` assigned in each case.
  Other accesses stay `self + 0x1Fx`.
- **0x825DF0** uses `(int *)(self + 0x1F0) + 0x3F` for the spawned-object
  word at +0x2EC (the AREA00 0x826790 idiom).
- **Switch shapes.** Compare chains run in reverse source order; the ones
  with a trailing `b` to func_001AFC10 are `case 3: default:` (0x825880,
  0x825D60, 0x825DF0, 0x8260C0) or `case 2: case 3: default:` (0x825B00).
  In 0x8260C0 the countdown test is `if (n > 0) { ... } else state 0x64`.
- **Intra-overlay calls** use the runtime names (splat piece names), e.g.
  `func_overlay_AREA04_00823580`, `..._00824490`, `..._008245F0`,
  `..._00824830`, `..._00824930`, `..._00825040`, `..._008251C0`,
  `..._00825280`; the link defines absorbed piece names at their address.

## Verification

- `overlay_match.py check AREA04 src/overlays/AREA04/*.c`: 30 of 31 files at
  100.00 BYTE-IDENTICAL (includes the asm pad and the two earlier files);
  the 31st is the NEARMISS two-function file, both of whose functions are
  byte-identical per function (above).
- `python3 tools/check_no_disassembly.py src/overlays/AREA04/*.c`: clean.
- Mutation sweep (bounded, 4 single-constant edits in scratch copies:
  0x823A90 `% 50` -> 51, 0x8260C0 band 426 -> 425, 0x825B00 sound 0x451 ->
  0x450, 0x825510 317.0 -> 318.0): all four drop below BYTE-IDENTICAL (99.97,
  98.72, 99.99, 99.54). The first 0x451 edit hit the header comment and
  still matched, as it should; the sweep was rerun on the code occurrence.
- `compile_overlay_src.py AREA04` + `build.py --area AREA04 --no-extract
  --no-yaml --no-splat` (under the decomp build lock): PASS, text+data and
  full file byte-identical (37888 bytes). 7 splat pieces absorbed by
  compiled functions; 32 code objects linked = 30 compiled objects (29 C +
  the asm pad) and the 2 splat pieces of the NEARMISS slot. Each linked
  filler object's `.text` equals the compiled `.text` (10 exactly, 17 plus
  zero padding to the slot, 3 differing only in the pre-applied GPREL16
  fields of their gp-relative accesses: 00823AD0, 00823EA0, 00825840).
  This regenerated `config/overlays/AREA04.lds`.
- Full `tools/decomp/build.py build` (rc 0, 2211 units) + `tools/verify_all.py`
  under the lock: all six stages PASS. The boot ELF is byte-identical,
  19/19 overlays pass, matched_code is 98.61% (2151/2211), and glTF,
  selftest and gs-offset pass. Overlay C is not an objdiff unit, so
  matched_code does not count this lane (the change from 2150 comes from
  other lanes).

## Known gaps

- The two functions of slot 00825240 are byte-identical C but are linked
  from their splat pieces until `fill_overlay.py` handles an object with
  several `.text` sections (see above).
- Roles are read from the instructions only; no AREA04 route capture was
  used, so no function is tied to a placement record or a game event yet,
  and nothing here was checked against the running game. The native port
  has no AREA04 module; nothing in the port was edited.
