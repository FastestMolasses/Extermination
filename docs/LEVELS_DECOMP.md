# Later-level decompilation coverage

Lane report, 2026-10-01 (Claude, level side track, decomp lane LDEC). This document lists every
boot-ELF function that the level census deltas of levels 2..11 recorded as executed, that is not
on the first-level census, and whose decomp source was still assembly or missing when this lane
started, and what it is now. It contains addresses, names, measurements and descriptions only:
no original instructions.

The first-level functions (the port's `docs/FIRST_LEVEL_CENSUS.md`, 1184 functions, and the C10
capture-lane rows of `docs/FIRST_LEVEL_DECOMP.md` section 7) belong to the first-level chain's
decomp lane and were not touched. Overlay functions of the deltas are covered by the per-area
overlay documents (`docs/AREAxx_OVERLAY.md`).

## 1. Result

Inputs: the census deltas `build/s87/census/{a01, a00, a02, a04, a22, a01u, a06, a06b, a01v,
a04b, a13, a19, a13b, a13c, a22b}_delta.json` (both their `functions` and `new_functions` lists;
`a22b` adds no function the others lack). Union of boot functions: 1637. Not on the first-level
census nor in `docs/FIRST_LEVEL_DECOMP.md`: 473. Their source markers on 2026-10-01 before this
lane: 254 compiled C, 174 NEARMISS C, 43 CodeWarrior `asm` bodies, 1 `INCLUDE_ASM` stub
(`func_001F6FB0`) and 1 row without a file of its own (`func_001C2FF0`, the splat false-split
tail of `func_001C2770`, whose NEARMISS C already covers it).

The 45 assembly / stub / missing rows now stand as follows (section 3 has one row each):

| Now | Functions | Meaning |
|---|---:|---|
| C, byte-matched | 31 | The asm body was replaced by C that objdiff rates 100% in the build's own report (`tools/decomp/build.py`, boot ELF byte-identical). |
| asm body + companion C | 12 | The asm body stays (it links byte-identically and is a matched unit); readable C with a NEARMISS header is in `src/readable/<f>.c`, and the asm file ends with a pointer to it (the convention of `docs/FIRST_LEVEL_DECOMP.md`). |
| NEARMISS C | 1 | `func_001F6FB0`, the former `INCLUDE_ASM` stub, is now a `// NEARMISS` file (readable C; the linker uses the `.s`). |
| covered already | 1 | `func_001C2FF0` (see above). |

Every one of the 45 now has readable C. All 31 byte-matched functions link from their compiled
C: 19 did at once, and the other 12, which `tools/decomp/fill_unmatched.py` still forced to the
original assembly through stale `SIZE_DRIFT_FORCE_ASM` entries, were released by lane DFIX the
same day (section 6).

matched_code is unchanged by this lane (2155/2211, 98.65%): the replaced asm bodies were matched
units already. (Lane DFIX's correction of func_001BBD20 later made it 2156/2211, 98.66%; section 6.)

## 2. Method

1. **List.** Union of the delta files' boot rows, minus `build/s87/census/route_functions.json`
   (the first-level census) and the function rows of `docs/FIRST_LEVEL_DECOMP.md`; each left row
   classified by its `src/<f>.c` marker.
2. **Decode.** m2c (`tools/m2c`, cleaned by `tools/match/m2c_clean.py`) gave a first draft; every
   function was rewritten by hand against the original instructions (argument registers the callee
   reads, stack frames, field widths, branch shapes) with the callee prototypes taken from
   already-matched callers. The port files that cite each address (section 3, port
   cross-reference) were read as a cross-check of the semantics only; a port label is not
   evidence.
3. **Match.** Private scratch harness `build/ldec/` (ignored): mwcc 2.3.3 for game code, ee-gcc
   2.9-991111-01 for the SDK libm / libc rows, mwcc 991202 / 2.4 swept where useful; the expected
   object is normalised exactly as `build.py` does (scratchpad symbols, per-file `// SPAD:`).
   A handful of source variants per function (the idioms of section 4); no permuter.
4. **Install.** 100% C replaced the asm body; the rest became companions (asm bodies) or a
   NEARMISS file (the stub). `tools/check_no_disassembly.py` is clean on every touched file.

## 3. Coverage table

"first beat" is the first census beat (game order) that ran the function. "objdiff %" is the C
measured against the original (100.00 = byte-identical in the build's objdiff report; companions
and the NEARMISS file are measured on the scratch build). "links as" is
`tools/decomp/audit_link_provenance.py` after the final build.

| function | address | bytes | first beat | now | objdiff % | compiler | links as | note |
|---|---|---|---|---|---:|---|---|---|
| func_0011C128 | 0x0011C128 | 0x39C | a01r_00_to_train_room | asm body + companion C | 87.62 | ee-gcc | original assembly (.s; SIZE_DRIFT_FORCE_ASM) | ee-gcc register allocation and the schedule of the reduction (semantics follow the instructions, incl. the path for tiny x) |
| func_0011E0A8 | 0x0011E0A8 | 0x9C | a04b_03_reader | asm body + companion C | 78.21 | ee-gcc | compiled asm body | ee-gcc keeps x in a GPR and fills the likely-branch slot with the integral store; register choice |
| func_00123020 | 0x00123020 | 0x144 | a06_02_keypad | asm body + companion C | 0.00 | ee-gcc | compiled asm body | hand-written MMI strcmp (parallel subtract / pack of quadwords); companion states the algorithm |
| func_0012B850 | 0x0012B850 | 0x118 | a01v_01_gap_jump | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Linked from C since lane DFIX removed its stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_0012D850 | 0x0012D850 | 0xE8 | a00_09_ne_room_out | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Linked from C since lane DFIX removed its stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_0012E3A0 | 0x0012E3A0 | 0x1B8 | a04b_03_reader | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00133DB0 | 0x00133DB0 | 0x78 | a04b_03_reader | asm body + companion C | 88.00 | mwcc 2.3.3 | compiled asm body | the empty-then branch shape around +0x54 and the speculated constant in the last compare |
| func_00142330 | 0x00142330 | 0x184 | a13c_05_boom | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Linked from C since lane DFIX removed its stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_0015B030 | 0x0015B030 | 0xF8 | a01_07_level_exit | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Linked from C since lane DFIX removed its stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_0015B610 | 0x0015B610 | 0x160 | a01_04_return_north | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Linked from C since lane DFIX removed its stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_00163D50 | 0x00163D50 | 0x13C | a01_04_return_north | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Linked from C since lane DFIX removed its stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_001790B0 | 0x001790B0 | 0x9C | a01_s5_duct | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_0017F240 | 0x0017F240 | 0xE0 | a01_00_train_room | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00181430 | 0x00181430 | 0xAC | a06_s1_bar | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Linked from C since lane DFIX removed its stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_00183250 | 0x00183250 | 0x1A0 | a01_04_return_north | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Linked from C since lane DFIX removed its stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_00183440 | 0x00183440 | 0x94 | a13_05_shaft | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00191120 | 0x00191120 | 0xEC | a01_00_train_room | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Linked from C since lane DFIX removed its stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_00193D90 | 0x00193D90 | 0x118 | a00_09_ne_room_out | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Linked from C since lane DFIX removed its stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_0019F680 | 0x0019F680 | 0xA8 | a06_s1_bar | asm body + companion C | 97.10 | mwcc 2.3.3 | compiled asm body | load order of the two table bases, register choice in the element loads; CW alignment nop before the shared return |
| func_001A8E80 | 0x001A8E80 | 0xBC | a02_02_ladder_escape | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001AA640 | 0x001AA640 | 0xB8 | a02_01_switch | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001B2BF0 | 0x001B2BF0 | 0x104 | a04b_03_reader | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Linked from C since lane DFIX removed its stale SIZE_DRIFT_FORCE_ASM entry (section 6). Per-file `// SPAD: 0x700031D0`. |
| func_001B7670 | 0x001B7670 | 0x60 | a00_04_cage_terminal | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001BDE60 | 0x001BDE60 | 0x158 | a04b_04_lift | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001BE5F0 | 0x001BE5F0 | 0xC4 | a04_05_progression_exit | asm body + companion C | 92.35 | mwcc 2.3.3 | compiled asm body | no dead join-head copy after the final b; zone pointer register |
| func_001C2FF0 | 0x001C2FF0 | 0x9F8 | a00_02_south_route | covered by func_001C2770 | — | mwcc 2.3.3 | — | Splat false-split tail of func_001C2770; its readable C is the NEARMISS file src/func_001C2770.c (one function, 0x1278 bytes). No file of its own. |
| func_001C48C0 | 0x001C48C0 | 0x9C | a02_05_progression_exit | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001C4960 | 0x001C4960 | 0x98 | a02_00_duct | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001C4AF0 | 0x001C4AF0 | 0xAC | a01r_03_door16 | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001C4BA0 | 0x001C4BA0 | 0x108 | a13_05_shaft | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001C5050 | 0x001C5050 | 0x58 | a00_10_progression_exit | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001D0400 | 0x001D0400 | 0xAC | a01_07_level_exit | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001D0D60 | 0x001D0D60 | 0x1BC | a01_s5_duct | asm body + companion C | 94.32 | mwcc 2.3.3 | original assembly (.s; SIZE_DRIFT_FORCE_ASM) | FPR coloring of the blend weights (f20/f21 swapped) and the cvt schedule |
| func_001D6DD0 | 0x001D6DD0 | 0x90 | a00_09_ne_room_out | asm body + companion C | 40.6 | mwcc 2.3.3 | original assembly (.s; SIZE_DRIFT_FORCE_ASM) | packed-word schedule + registers (991202 69.0) |
| func_001D7000 | 0x001D7000 | 0x7C | a00_09_ne_room_out | asm body + companion C | 98.39 | mwcc 2.3.3 | compiled asm body | register allocation |
| func_001DEDB0 | 0x001DEDB0 | 0x28 | a00_09_ne_room_out | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Linked from C since lane DFIX removed its stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_001DEE80 | 0x001DEE80 | 0x34 | a00_09_ne_room_out | asm body + companion C | 78.54 | mwcc 2.3.3 | compiled asm body | IPA wall (a1 kept in a1 across the same-TU leaf func_001DEDB0) |
| func_001DEEC0 | 0x001DEEC0 | 0x20 | a00_09_ne_room_out | asm body + companion C | 66.38 | mwcc 2.3.3 | compiled asm body | IPA wall (same) |
| func_001EFFD0 | 0x001EFFD0 | 0x88 | a00_02_south_route | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001F4E40 | 0x001F4E40 | 0xF4 | a02_01_switch | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001F6FB0 | 0x001F6FB0 | 0x800 | a00_09_ne_room_out | NEARMISS C (former INCLUDE_ASM stub) | 62.77 | mwcc 2.3.3 | original assembly (.s; NEARMISS) | VU0 macro code written as C float arithmetic; register allocation and schedule |
| func_00207CA0 | 0x00207CA0 | 0x24 | a06_02_keypad | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00207CD0 | 0x00207CD0 | 0x24 | a06_02_keypad | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_0021BD60 | 0x0021BD60 | 0xE0 | a00_04_cage_terminal | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_0021BE40 | 0x0021BE40 | 0x90 | a04b_03_reader | asm body + companion C | 90.83 | mwcc 2.3.3 | original assembly (.s; SIZE_DRIFT_FORCE_ASM) | branch layout of the float test |

### Port cross-reference

Port files (`../extermination-port/src/game/`) whose text cites each address (first three by name;
a citation is a pointer for review, not evidence of a verified translation):

- func_0011C128: em_area02_misc
- func_0011E0A8: em_level8_port_creature
- func_00123020: em_area06_port
- func_0012B850: em_area01_exita, em_level8_port_gap
- func_0012D850: em_area01_exita, em_area00_low
- func_0012E3A0: em_level8_port_creature
- func_00133DB0: em_level8_port_creature
- func_00142330: none
- func_0015B030: em_area01_exitb
- func_0015B610: em_area01_render_frame, em_area11_bindings, em_level_smoke_test
- func_00163D50: em_player_fall
- func_001790B0: em_player_closure_0e_18
- func_0017F240: em_player_closure_0e_18, em_player, em_player_hang
- func_00181430: em_player_closure_10_12_19
- func_00183250: em_area01_math_player, em_area01_render_frame, em_area11_bindings
- func_00183440: em_area01_render_frame, em_level9_port_exit, em_player_stage_live
- func_00191120: em_area06_port_node, em_camera_follow_original
- func_00193D90: em_camera_area11_specials, em_camera_leftovers, em_camera
- func_0019F680: em_area06_port, em_area06_port_node
- func_001A8E80: em_area02_misc, em_coll_list_passes, em_collision_world
- func_001AA640: em_area02_misc
- func_001B2BF0: em_level8_port_probe, em_level8_port_creature
- func_001B7670: em_area00_low
- func_001BDE60: em_level8_port_lift
- func_001BE5F0: em_area04_port, em_level9_port_anim
- func_001C48C0: em_area02_math
- func_001C4960: em_area02_math
- func_001C4AF0: em_area02_math
- func_001C4BA0: em_level9_port_anim
- func_001C5050: em_area00_world
- func_001D0400: em_area01_exitb, em_area01_exita
- func_001D0D60: em_area01_math_owner, em_area01_room, em_area04_port
- func_001D6DD0: em_area00_hud, em_area00_fx_gs
- func_001D7000: em_area00_hud, em_static_world
- func_001DEDB0: em_area00_hud, em_render_context
- func_001DEE80: em_area00_low, em_area00_hud, em_render_context
- func_001DEEC0: em_area00_hud, em_area00_low, em_render_context
- func_001EFFD0: em_area00_fx_spawn, em_area00_low, em_level8_port_creature
- func_001F4E40: em_area02_misc, em_area02_math
- func_001F6FB0: em_area00_fx_glow
- func_00207CA0: em_area06_port
- func_00207CD0: em_area06_port
- func_0021BD60: em_area00_low, em_area00_world
- func_0021BE40: em_level10_port_boot, em_level8_port_creature

## 4. Levers used

All are from `docs/fanout/MATCHING_GUIDE.md` and `docs/FIRST_LEVEL_DECOMP.md` section 4; none is
new, but these rows confirm them on later-level code:

- **Relocated scratchpad externs keep branch delay slots empty.** A literal `0x7000xxxx` address
  lets mwcc speculate its upper-half load into a conditional branch's delay slot; naming the
  scratchpad byte as `extern` (global `_SPAD_SYMS` 0x70003B8D / 0x70003B91, or a per-file
  `// SPAD: 0x700031D0`) gives the original's empty slots: func_001B7670 (45.21 -> 100),
  func_001B2BF0 (96.77 -> 100).
- **`case K: default:`** keeps the compare of a value that shares the default body and the
  original's dead constant in the first branch's delay slot: func_001DEDB0 (87.80 -> 100),
  func_001C4960 (`case 3: default:`).
- **Pointer-typed prototypes** for pass-through arguments: func_001C5050 (90.91 -> 100, the
  argument order of func_001D7FA0 follows the pointer types).
- **Negated tests / branch arms in the original's order**: func_00191120 (`if (!(d <= step))`,
  80.42 -> 100), func_0015B030 (`if (!(x < 2)) ... else ...`, 90.32 -> 100), func_0012E3A0 (split
  `||` into two early returns, 97.64 -> 100), func_0021BD60 (one combined condition with the
  shared return-1 tail, 72.68 -> 100), func_0017F240 (`x != (z = 0.0f)`, idiom-23, 93.84 -> 100).
- **idiom-28 strict relational respell**: func_00163D50 (`> 1` for `>= 2`), func_0021BD60
  (`== 0x21 || == 0x22` for the unsigned range test).
- **Declaration order / compound spellings**: func_001790B0 (`int i` before the table pointer,
  `<= 6`), func_00183440 (`*p += 1.0f` then re-read), func_001F4E40 (one variable updated in
  place, `a *= t; a >>= 8; ...`), func_001A8E80 (the y difference built in a local with `-=`),
  func_00183250 (an int-returning prototype cast to short at the call, so the original's
  sign-extension appears).
- **Struct member for a doubleword at an offset**: func_00207CA0 / func_00207CD0 (the 64-bit
  texture word read as `p->q78`, 20.33 -> 100).

## 5. Gate and open items

Final run (build lock held, 2026-10-01 19:52-20:05): `tools/decomp/build.py build` and `objdiff`
rc 0, `.venv/bin/python3 tools/verify_all.py` all six stages PASS: boot ELF byte-identical
(0x175b00 loadable bytes), 19/19 overlays, matched_code 98.65% (2155/2211), glTF, anim self-test,
GS offset. The build's objdiff report rates all 31 byte-matched functions 100.0.
`audit_link_provenance.py`: no copied-text, relocation or pinned-rodata mismatches; the 19
byte-matched functions not on a force list link from their compiled C with copied text and
relocations equal to the prepared object.

**Stale `SIZE_DRIFT_FORCE_ASM` entries: resolved** (lane DFIX, section 6). The 12 functions this
section listed (func_0012B850, func_0012D850, func_00142330, func_0015B030, func_0015B610,
func_00163D50, func_00181430, func_00183250, func_00191120, func_00193D90, func_001B2BF0,
func_001DEDB0) now link from their compiled C.

**Open.**

- Near-miss walls: func_001DEE80 / func_001DEEC0 are the intra-TU register analysis wall
  (they call the same-unit leaf func_001DEDB0; a static-helper link step would promote them);
  func_001D7000 (98.39) and func_0019F680 (97.10) are register allocation / load order;
  func_001D0D60, func_001BE5F0, func_0021BE40, func_00133DB0 are branch-shape or FPR colouring
  residuals; func_001D6DD0 is a schedule difference (mwcc 991202 measures 69.00 on the same C).
- func_00123020 (strcmp) is hand-written MMI code; the companion states the algorithm.
- func_001F6FB0 uses VU0 macro instructions; its NEARMISS C writes them as FPU arithmetic.
- The port's census for the later levels should pick up the "Decomp" status of these rows at
  its next recount.

## 6. Force-asm cleanup and NEARMISS body corrections (lane DFIX, 2026-10-01)

**Force-asm cleanup.** The 12 stale entries of section 5 were removed from
`SIZE_DRIFT_FORCE_ASM` in `tools/decomp/fill_unmatched.py` (nothing else in that file changed).
Proof (build lock held, 21:43-21:53): `tools/decomp/build.py build` rc 0; `.venv/bin/python
tools/verify_all.py` all six stages PASS (boot ELF byte-identical, 19/19 overlays, matched_code
98.65%, 2155/2211); `tools/decomp/audit_link_provenance.py`: each of the 12 has route
`compiled_object_ordinary_c` with the filler's text and relocations equal to the prepared
object, and no copied-text, relocation or pinned-rodata mismatch anywhere. objdiff rates each
of the 12 at 100.0 in that build.

**NEARMISS body corrections.** The decomp C that the level port lanes (port docs/LEVEL8..11_PORT.md,
AREA04_PORT.md) found wrong against the original instructions was corrected after the
instructions were re-read: func_00118418, func_001E4610, func_001424C0, func_001459A0,
func_0019A6F0, func_001CDDC0, func_001BD9F0, func_00196CE0, func_001C1030, func_00118790,
func_001305B0, func_001C1A80 (all stay NEARMISS) and func_001BBD20 (ordinary C, now objdiff
100%; it still links from the `.s` through its own `SIZE_DRIFT_FORCE_ASM` entry, which this
lane was not asked to remove: a candidate for the next cleanup). Each change, the scratch
original-instruction harness that checked it, and the points checked without a change are in
`docs/FINDINGS.md` "NEARMISS body corrections from the level side-track lanes". Final gate
(build lock held, 22:04-22:17): build rc 0, verify_all all six stages PASS (boot ELF
byte-identical, 19/19 overlays, matched_code 98.66%, 2156/2211: func_001BBD20 is the new
matched unit), audit_link_provenance with no mismatches.

**Review fix round (2026-10-01).** func_001305B0's state 1 keeps the original's re-read of
owner+0x30 for func_001B12B0 (objdiff 99.19; the intermediate direct-result form scored 99.00
with the same behaviour); func_001E4610's header no longer calls the func_001CE660 argument
loads a dead prefetch. FINDINGS wording made exact (the four outcomes the harness cannot reach,
the source of func_001CDDC0's S/T/Q rounding difference), plus pinned harness cases for the
review's named survivors and two equivalence arguments (FINDINGS, same section). Gate under the
build lock, 22:55-23:07: build rc 0, verify_all all six stages PASS (boot ELF byte-identical,
19/19 overlays, matched_code 98.66%, 2156/2211), audit_link_provenance with no copied-text,
relocation, pinned-rodata or missing-filler mismatch.
