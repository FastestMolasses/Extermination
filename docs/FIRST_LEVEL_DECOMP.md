# First-level decompilation coverage

Lane report, 2026-10-01 (Claude, first-level chain, decomp lane). This document lists every
original function on the first-level census (the port's `docs/FIRST_LEVEL_CENSUS.md`: the 1184
functions executed from the title's NEW GAME through AREA11 to Roger's encounter, boot ELF and
AREA11 overlay, boundary rows included) whose decomp source was still assembly or missing when
this lane started, and what it is now. It contains addresses, names, measurements and
descriptions only: no original instructions.

The decomp's C is the port's ground truth. Every row below was read from the original
instructions (the local splat disassembly) and, where they exist, cross-checked against the
port's oracle-verified translations. A review of this lane found 9 bodies that did not say what
the instructions do (mostly calls that dropped argument registers the callee reads); they were
corrected and every lane file was then re-scanned for callee arity (section 8).

## 1. Result

Starting point (source markers of the 1184 census functions on 2026-10-01, before this lane):
708 compiled C, 273 NEARMISS C, 194 CodeWarrior `asm` bodies, 4 `INCLUDE_ASM` stubs and 5 rows
without a file of their own (four AREA11 overlay labels that are part of the preceding C unit,
and `_iSignalSema`, whose source is `src/iSignalSema.c`).

The 198 assembly / missing functions now stand as follows (section 3 has the row for each; two
collision companions were promoted to byte-matched C during the C10 pass of section 7, and four
more after the review fixes of section 8):

| Now | Functions | Meaning |
|---|---:|---|
| C, byte-matched | 70 | The asm body was replaced by C that objdiff rates 100% (built by `tools/decomp/build.py`, boot ELF byte-identical). |
| asm body + companion C | 102 | The asm body stays (it links byte-identically and is a matched unit); readable C with a NEARMISS header is in `src/readable/<f>.c`, and the asm file ends with a pointer to it. |
| NEARMISS C | 2 | Former `INCLUDE_ASM` stubs, now `// NEARMISS` files (readable C; the linker uses the `.s`). |
| C already | 5 | `func_00109F90` (byte-matched C whose empty asm statement is only a scheduling barrier; it was mis-sorted as asm) and the four absorbed AREA11 overlay labels. |
| asm, no C form | 23 | Hand-written code with no C equivalent: syscall stubs, interrupt and cache maintenance, VU0 / VU1 control registers, and the 16 libmpeg MMI image routines (title movie only). Section 3 describes each. |
| not done | 1 | `sub__0000000000000000Inf`, newlib's formatted output (5400 bytes, title-path debug text only). Still `INCLUDE_ASM`. |

Census-wide afterwards: 778 compiled C, 275 NEARMISS C, 102 asm bodies with companion C,
22 asm bodies with no C form, 2 `INCLUDE_ASM` stubs and the 5 rows above. Every census function
except the 24 hand-written / one undone now has readable C.

Gate: `tools/decomp/build.py build` and `tools/verify_all.py` (section 5). Boot ELF
byte-identical, 19/19 overlays, matched_code not reduced.

The functions the PCSX2 capture lanes found off the main route (aiming, the exit and the AREA01
arrival, damage, branches, options: the C10 census deltas) are covered in section 7.

### How the asm-bodied functions were handled

A function whose `src/<f>.c` is a CodeWarrior `asm` body is already a matched unit. Turning it
into a `// NEARMISS` file would drop it from matched_code (the lane may not let the matched count
drop) and remove an asm body the user decided to keep (CLAUDE.md, "leave as is for now"). So:

- at 100% the asm body is replaced by the C (normal promotion);
- otherwise the readable C goes to `src/readable/<f>.c` with the standard NEARMISS header
  (objdiff %, compiler, divergence) and a registry row in `docs/NEARMISS.md` ("Companion C in
  src/readable/"). `build.py` and `link.py` only glob `src/*.c`, so these files are never
  compiled or linked. The asm file gets one trailing comment line pointing at its companion.

### Link provenance

`tools/decomp/audit_link_provenance.py` after the final build (section 3, column "links as"):
a byte-matched C file links from the compiled object unless `tools/decomp/fill_unmatched.py`
forces the `.s`. 16 of the 70 matched functions are still on the stale
`SIZE_DRIFT_FORCE_ASM` list there (entries written for earlier, non-matching C), so they link
from the original assembly although their C is byte-identical. This lane may not edit
`fill_unmatched.py`; removing those entries is listed in section 6.

## 2. Method

1. **List.** The census rows (`build/s87/census/route_functions.json`, 1184 functions) were
   matched against today's `src/` markers: an `// INCLUDE_ASM` first line, a CodeWarrior
   `asm` function body, a `// NEARMISS` first line, or ordinary C. 194 asm bodies, 4 stubs and 5
   rows without their own file were left.
2. **Decode.** m2c (`tools/m2c`, cleaned by `tools/match/m2c_clean.py`, jump tables through
   `tools/match/jtbl_prep.py`) gave a first draft; every function was then rewritten by hand
   against the original instructions (argument registers, stack arguments, call arity, field
   widths, loop shape). Where the port has an oracle-verified translation (collision, shadow
   decal, pose host, effect manager, SDK VU0 routines) its semantics were cross-checked.
3. **Match.** Private scratch harness in `build/fld/` (ignored): compile with the per-file
   compiler (mwcc 2.3.3 for game code, ee-gcc 2.9-991111-01 for the SDK; mwcc 991202 / 2.4 swept
   where useful), assemble the normalised expected object exactly as `build.py` does (VU0
   fix-ups, jump-table rodata, scratchpad symbols including per-file `// SPAD:`), objdiff, and
   iterate with the idioms of section 4. A handful of source variants per function; register
   permutation walls were not chased with the permuter.
4. **Install.** Byte-matched C replaced the asm body. The rest became NEARMISS files (former
   stubs) or `src/readable/` companions (former asm bodies). `tools/check_no_disassembly.py` is
   clean on every touched file.

## 3. Coverage table

One row per census function that was assembly or missing on 2026-10-01. "objdiff %" is the
function's C measured against the original (100.00 = byte-identical; companions and NEARMISS files
are measured on the scratch build). "links as" is the provenance audit after the final build:
compiled C, compiled asm body (the CodeWarrior `asm` unit), or original assembly (.s).

| function | address | bytes | first beat | now | objdiff % | compiler | links as | note |
|---|---|---|---|---|---:|---|---|---|
| func_001000B0 | 0x001000B0 | 0x8 | S0_title | asm, no C form | — |  | original assembly (.s) | SDK syscall stub (the kernel call number in v1, then syscall); no C form. |
| vu1_cold_start | 0x00100278 | 0x68 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written VU1 start-up (COP2 control-register and VU1 memory writes, privileged sync); no C form. |
| func_001006D8 | 0x001006D8 | 0x1E4 | S1_newgame_load | asm body + companion C | 84.05 | ee-gcc | compiled asm body |  |
| func_001008C0 | 0x001008C0 | 0x104 | S1_newgame_load | asm body + companion C | 47.63 | ee-gcc | compiled asm body |  |
| func_001015A8 | 0x001015A8 | 0x88 | S0_title | asm body + companion C | 49.26 | ee-gcc | compiled asm body |  |
| func_00101810 | 0x00101810 | 0x88 | S0_title | asm body + companion C | 49.26 | ee-gcc | compiled asm body |  |
| func_001026A0 | 0x001026A0 | 0x2C | S1_newgame_load | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_001026D0 | 0x001026D0 | 0x44 | S0_title | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_00102718 | 0x00102718 | 0x1C | S1_newgame_load | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_00102738 | 0x00102738 | 0x24 | S2_opening | asm body + companion C | 24.44 | mwcc 2.3.3 | compiled asm body |  |
| func_00102760 | 0x00102760 | 0x34 | S1_newgame_load | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_00102798 | 0x00102798 | 0x44 | S1_newgame_load | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_001027E0 | 0x001027E0 | 0x6C | S1_newgame_load | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_00102850 | 0x00102850 | 0x20 | 01_battery | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_001028B8 | 0x001028B8 | 0x14 | S2_opening | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_001028D0 | 0x001028D0 | 0x14 | S1_newgame_load | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_00102900 | 0x00102900 | 0x18 | S1_newgame_load | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_00102918 | 0x00102918 | 0x2C | S1_newgame_load | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_00102948 | 0x00102948 | 0xC | S0_title | asm body + companion C | 96.67 | ee-gcc | compiled asm body |  |
| copy_qw4 | 0x00102958 | 0x24 | S0_title | asm body + companion C | 94.67 | ee-gcc | compiled asm body | mwcc gives 96.22; ee-gcc is the SDK compiler of record. |
| func_001029C0 | 0x001029C0 | 0x28 | S0_title | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_00102A60 | 0x00102A60 | 0xA4 | S1_newgame_load | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body | Companion states the original's pi/2 fold, series and signed sqrt sine (valid for angle in [-pi, pi]). |
| func_00102B08 | 0x00102B08 | 0xA8 | S1_newgame_load | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body | As func_00102A60. |
| func_00102BB0 | 0x00102BB0 | 0xA8 | S1_newgame_load | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body | As func_00102A60. |
| func_00103230 | 0x00103230 | 0x18 | S2_opening | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_00104378 | 0x00104378 | 0x74 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_001043F0 | 0x001043F0 | 0x94 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104488 | 0x00104488 | 0xB4 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104540 | 0x00104540 | 0xCC | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104610 | 0x00104610 | 0xAC | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_001046C0 | 0x001046C0 | 0xB8 | S0_title | asm, no C form | — |  | original assembly (.s) | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104778 | 0x00104778 | 0xF8 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104870 | 0x00104870 | 0xFC | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104970 | 0x00104970 | 0xA4 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104A18 | 0x00104A18 | 0xAC | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104AC8 | 0x00104AC8 | 0xE4 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104BB0 | 0x00104BB0 | 0xE4 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104C98 | 0x00104C98 | 0xDC | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104D78 | 0x00104D78 | 0xD0 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104E48 | 0x00104E48 | 0x128 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_00104F70 | 0x00104F70 | 0x114 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written SDK libmpeg MMI image routine (128-bit loads, funnel shifts and packed byte / halfword expands and packs between the IPU output and the frame buffer); no C form. Title movie path only (the port plays movies with its own decoder). |
| func_001060F8 | 0x001060F8 | 0x17C | S0_title | C, byte-matched | 100.00 | ee-gcc | compiled C | Review fix: 10th argument and the 7-argument func_00106278 call restored (74.60 -> 100). |
| func_00108608 | 0x00108608 | 0x34 | S0_title | asm body + companion C | 78.62 | ee-gcc | compiled asm body |  |
| func_00108640 | 0x00108640 | 0x1C | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00108660 | 0x00108660 | 0x98 | S0_title | asm body + companion C | 78.95 | ee-gcc | compiled asm body |  |
| func_00108790 | 0x00108790 | 0x54 | S0_title | asm body + companion C | 94.00 | ee-gcc | compiled asm body |  |
| func_00108AA0 | 0x00108AA0 | 0x2F0 | S0_title | asm body + companion C | 65.91 | ee-gcc | compiled asm body |  |
| func_001095F0 | 0x001095F0 | 0xA8 | S0_title | asm body + companion C | 50.10 | ee-gcc | compiled asm body |  |
| func_00109A30 | 0x00109A30 | 0xC | S0_title | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_00109AF8 | 0x00109AF8 | 0x24 | S0_title | asm body + companion C | 73.89 | ee-gcc | compiled asm body | mwcc gives 75.33; ee-gcc is the SDK compiler of record. |
| func_00109B20 | 0x00109B20 | 0x50 | S0_title | asm body + companion C | 72.00 | ee-gcc | compiled asm body |  |
| func_00109F90 | 0x00109F90 | 0x3C | S0_title | C (already) | 100.00 |  | compiled asm body | Already byte-matched C before this lane (an empty asm statement is only a scheduling barrier). |
| func_0010A4D8 | 0x0010A4D8 | 0x20 | S0_title | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| _iSignalSema | 0x0010B850 | 0x10 | S0_title | asm, no C form | — |  | compiled asm body | SDK syscall stub iSignalSema (src/iSignalSema.c, kernel call -0x43); no C form. The census names it _iSignalSema. |
| func_0010BCD0 | 0x0010BCD0 | 0x60 | S0_title | asm body + companion C | 49.50 | ee-gcc | compiled asm body |  |
| func_0010BF18 | 0x0010BF18 | 0x14 | S0_title | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_0010C020 | 0x0010C020 | 0xA4 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written kernel cache maintenance: walks the 64-byte data-cache lines (both ways) by index, reading each tag through COP0 and invalidating lines whose address lies in [start, end]; cache instructions have no C form. |
| func_0010C0C8 | 0x0010C0C8 | 0x74 | S0_title | asm body + companion C | 14.14 | ee-gcc | original assembly (.s) |  |
| func_0010C290 | 0x0010C290 | 0x64 | S0_title | asm body + companion C | 27.68 | ee-gcc | original assembly (.s) |  |
| func_0010C2F8 | 0x0010C2F8 | 0x64 | S0_title | asm body + companion C | 27.68 | ee-gcc | original assembly (.s) |  |
| func_0010C360 | 0x0010C360 | 0x64 | S0_title | asm body + companion C | 27.68 | ee-gcc | original assembly (.s) |  |
| func_0010C3C8 | 0x0010C3C8 | 0x64 | S0_title | asm body + companion C | 27.68 | ee-gcc | original assembly (.s) |  |
| func_0010DFD8 | 0x0010DFD8 | 0xAC | S0_title | asm, no C form | — |  | compiled asm body | Hand-written kernel cache maintenance: writes back and invalidates the data-cache lines covering [addr, addr + size) eight lines per step; cache instructions have no C form. |
| func_0010E270 | 0x0010E270 | 0xA4 | S0_title | asm body + companion C | 55.56 | ee-gcc | compiled asm body |  |
| func_0010E3A8 | 0x0010E3A8 | 0xB4 | S0_title | asm body + companion C | 43.91 | ee-gcc | compiled asm body |  |
| func_0010EA60 | 0x0010EA60 | 0x3C | S0_title | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_00119810 | 0x00119810 | 0x14 | S0_title | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_00119978 | 0x00119978 | 0x18 | S0_title | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_0011A788 | 0x0011A788 | 0x68 | S0_title | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_0011A830 | 0x0011A830 | 0x14 | S0_title | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_0011A9D8 | 0x0011A9D8 | 0x14 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written: sets bit 1 (VU0 reset) of the VU0 control register FBRST through COP2 control-register moves; no C form. |
| func_0011AE88 | 0x0011AE88 | 0x14 | S0_title | asm, no C form | — |  | compiled asm body | Hand-written: sets bit 9 (VU1 reset) of FBRST through COP2 control-register moves; no C form. |
| func_0011B328 | 0x0011B328 | 0x14 | S0_title | asm body + companion C | 68.00 | ee-gcc | compiled asm body |  |
| func_0011B5E0 | 0x0011B5E0 | 0x24 | S0_title | asm body + companion C | 82.22 | ee-gcc | compiled asm body |  |
| func_0011B910 | 0x0011B910 | 0x24 | S0_title | asm body + companion C | 82.22 | ee-gcc | compiled asm body |  |
| func_0011B9E0 | 0x0011B9E0 | 0x20 | S0_title | asm body + companion C | 76.25 | ee-gcc | compiled asm body |  |
| func_0011CB90 | 0x0011CB90 | 0x138 | S0_title | asm body + companion C | 84.29 | ee-gcc | compiled asm body |  |
| func_0011CCC8 | 0x0011CCC8 | 0x158 | S2_opening | asm body + companion C | 89.70 | ee-gcc | compiled asm body |  |
| func_0011D770 | 0x0011D770 | 0x104 | S2_opening | asm body + companion C | 94.23 | ee-gcc | compiled asm body |  |
| func_0011DE90 | 0x0011DE90 | 0xE8 | S3_first_control_idle | asm body + companion C | 92.33 | ee-gcc | original assembly (.s) |  |
| func_0011DF78 | 0x0011DF78 | 0x1C | S1_newgame_load | asm body + companion C | 85.00 | ee-gcc | compiled asm body |  |
| func_0011E080 | 0x0011E080 | 0x24 | S0_title | asm body + companion C | 28.89 | ee-gcc | compiled asm body |  |
| func_0011E2A8 | 0x0011E2A8 | 0xF0 | S2_opening | asm body + companion C | 92.58 | ee-gcc | original assembly (.s) |  |
| func_0011E398 | 0x0011E398 | 0x88 | S2_opening | asm body + companion C | 86.91 | ee-gcc | compiled asm body |  |
| func_001205D8 | 0x001205D8 | 0x3D8 | S0_title | asm body + companion C | 96.21 | ee-gcc | compiled asm body |  |
| func_001216B8 | 0x001216B8 | 0x3C | S0_title | asm body + companion C | 42.67 | ee-gcc | compiled asm body |  |
| func_001216F8 | 0x001216F8 | 0xE0 | S0_title | asm body + companion C | 10.41 | ee-gcc | compiled asm body |  |
| block_copy | 0x00121870 | 0xB0 | S0_title | asm body + companion C | 54.86 | ee-gcc | compiled asm body |  |
| func_00121920 | 0x00121920 | 0x104 | S0_title | asm body + companion C | 51.82 | ee-gcc | compiled asm body |  |
| func_00121A28 | 0x00121A28 | 0xC0 | S0_title | asm body + companion C | 60.40 | ee-gcc | compiled asm body |  |
| vtable_a0_at_0011FE90_off24 | 0x00122DE8 | 0x80 | S0_title | asm body + companion C | 72.19 | ee-gcc | compiled asm body |  |
| func_00122EF0 | 0x00122EF0 | 0x12C | 01_battery | asm body + companion C | 19.99 | ee-gcc | compiled asm body |  |
| func_00123168 | 0x00123168 | 0x114 | 01_battery | asm body + companion C | 9.13 | ee-gcc | compiled asm body |  |
| func_001232E0 | 0x001232E0 | 0x138 | S2_opening | asm body + companion C | 11.33 | ee-gcc | compiled asm body |  |
| sub__0000000000000000Inf | 0x00123750 | 0x1518 | S0_title | INCLUDE_ASM (not done) | — |  | original assembly (.s) | Not decompiled in this lane: libc formatted output (newlib vfprintf, 5400 bytes), title-path debug text only; remains INCLUDE_ASM. |
| func_00125F48 | 0x00125F48 | 0x60 | S0_title | asm body + companion C | 72.29 | ee-gcc | compiled asm body |  |
| func_00126AB8 | 0x00126AB8 | 0x12C | 03_panel_power | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_00126BE8 | 0x00126BE8 | 0x9C | 03_panel_power | asm body + companion C | 97.44 | ee-gcc | compiled asm body |  |
| func_00127398 | 0x00127398 | 0x114 | 03_panel_power | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_00127758 | 0x00127758 | 0x54 | 03_panel_power | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_001277B0 | 0x001277B0 | 0x10C | 03_panel_power | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_001278C0 | 0x001278C0 | 0x90 | S1_newgame_load | asm body + companion C | 86.22 | ee-gcc | compiled asm body |  |
| float_to_int | 0x001281C0 | 0x8C | S1_newgame_load | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_00128250 | 0x00128250 | 0x98 | S1_newgame_load | C, byte-matched | 100.00 | ee-gcc | original assembly (.s) |  |
| func_00128350 | 0x00128350 | 0x40 | 03_panel_power | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_0015B530 | 0x0015B530 | 0xDC | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_0015D000 | 0x0015D000 | 0xF8 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_0015DEC0 | 0x0015DEC0 | 0x4C | 05_boxes | asm body + companion C | 82.89 | mwcc 2.3.3 | compiled asm body |  |
| func_00163B40 | 0x00163B40 | 0xCC | 10_cage_roof_roger | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_00176BE0 | 0x00176BE0 | 0x98 | 01_battery | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00177460 | 0x00177460 | 0xA8 | 05_boxes | asm body + companion C | 86.67 | mwcc 2.3.3 | original assembly (.s) |  |
| func_00178B90 | 0x00178B90 | 0x330 | 00_panel_no_battery | asm body + companion C | 99.10 | mwcc 2.3.3 | compiled asm body |  |
| func_0017DEB0 | 0x0017DEB0 | 0xBC | 05_boxes | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_00181110 | 0x00181110 | 0x6C | 10_cage_roof_roger | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00182430 | 0x00182430 | 0x440 | 00_panel_no_battery | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_0018C4B0 | 0x0018C4B0 | 0xE8 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_0018C6A0 | 0x0018C6A0 | 0x1A8 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_00191390 | 0x00191390 | 0x108 | S2_opening | asm body + companion C | 96.97 | mwcc 2.3.3 | compiled asm body |  |
| func_00192010 | 0x00192010 | 0x1B8 | 06_hill_slide | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_0019A310 | 0x0019A310 | 0x130 | S2_opening | asm body + companion C | 88.71 | mwcc 2.3.3 | original assembly (.s) |  |
| func_0019ED80 | 0x0019ED80 | 0x418 | S2_opening | asm body + companion C | 96.20 | mwcc 2.3.3 | original assembly (.s) |  |
| func_0019F330 | 0x0019F330 | 0x348 | 05_boxes | asm body + companion C | 96.20 | mwcc 2.3.3 | original assembly (.s) |  |
| func_001A4650 | 0x001A4650 | 0x1E0 | 00_panel_no_battery | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Promoted in the C10 pass (section 7): inputs as // SPAD symbols, outputs literal. |
| func_001A4D10 | 0x001A4D10 | 0x38C | 04_elevator_ride | asm body + companion C | 88.68 | mwcc 2.3.3 | original assembly (.s) | Companion improved from 75.57 in the C10 pass (section 7). |
| func_001A5760 | 0x001A5760 | 0x14C | 05_boxes | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Promoted in the C10 pass (section 7). |
| func_001AF470 | 0x001AF470 | 0x148 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001AF780 | 0x001AF780 | 0x38 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001B1020 | 0x001B1020 | 0x84 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001B12B0 | 0x001B12B0 | 0xCC | 00_panel_no_battery | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_001B1380 | 0x001B1380 | 0x68 | S2_opening | asm body + companion C | 88.46 | mwcc 2.3.3 | compiled asm body |  |
| func_001B1EA0 | 0x001B1EA0 | 0x294 | S2_opening | asm body + companion C | 92.91 | mwcc 2.3.3 | original assembly (.s) |  |
| func_001B5940 | 0x001B5940 | 0x228 | S0_title | asm body + companion C | 97.43 | mwcc 2.3.3 | compiled asm body |  |
| func_001B5C90 | 0x001B5C90 | 0x2C | 00_panel_no_battery | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001B5CC0 | 0x001B5CC0 | 0xAC | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001B5D70 | 0x001B5D70 | 0x50 | 00_panel_no_battery | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001B5E20 | 0x001B5E20 | 0x114 | 03_panel_power | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001B5F40 | 0x001B5F40 | 0x280 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_001BB0E0 | 0x001BB0E0 | 0x228 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Review fix: func_001BA580(a, cmd[2]) gets the command id (99.89 -> 100). |
| func_001C5680 | 0x001C5680 | 0xE0 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Review fix: func_001C2360(node) gets the node (98.93 -> 100). |
| func_001C5760 | 0x001C5760 | 0xF4 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Review fix: func_001C22A0(node) gets the node (99.02 -> 100). |
| bone_init_default_1 | 0x001C62C0 | 0xB8 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| anim_eval_skeleton | 0x001C6DA0 | 0x680 | S2_opening | NEARMISS C | 11.24 | mwcc 2.3.3 | original_assembly_nearmiss |  |
| func_001C8710 | 0x001C8710 | 0xA4 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| build_trs_matrix | 0x001C94B0 | 0xB8 | S2_opening | asm body + companion C | 3.15 | mwcc 2.3.3 | compiled asm body |  |
| func_001C9E40 | 0x001C9E40 | 0x25C | 00_panel_no_battery | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_001CBA50 | 0x001CBA50 | 0x1C4 | 01_battery | asm body + companion C | 94.80 | mwcc 2.3.3 | compiled asm body |  |
| func_001CC170 | 0x001CC170 | 0x64 | S2_opening | asm body + companion C | 100.00 | mwcc 2.3.3 | compiled asm body |  |
| func_001CC1E0 | 0x001CC1E0 | 0x1CC | S2_opening | asm body + companion C | 94.37 | mwcc 2.3.3 | compiled asm body |  |
| func_001CCB10 | 0x001CCB10 | 0xC0 | S0_title | asm body + companion C | 95.73 | mwcc 2.3.3 | compiled asm body | Packet and palette as one 0x170-byte DMA source. |
| func_001CCBD0 | 0x001CCBD0 | 0xE8 | S0_title | asm body + companion C | 90.69 | mwcc 2.3.3 | compiled asm body | Packet and palette as one 0x170-byte DMA source (84.67 -> 90.69). |
| func_001CCE80 | 0x001CCE80 | 0xEC | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001CF470 | 0x001CF470 | 0x3F4 | 02_elevator_refusal | asm body + companion C | 72.32 | mwcc 2.3.3 | original assembly (.s) |  |
| func_001CF870 | 0x001CF870 | 0xF4 | 02_elevator_refusal | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001CF970 | 0x001CF970 | 0xE4 | 02_elevator_refusal | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_001D04B0 | 0x001D04B0 | 0x90 | S2_opening | asm body + companion C | 81.67 | mwcc 2.3.3 | compiled asm body |  |
| func_001D2590 | 0x001D2590 | 0x58 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001D2830 | 0x001D2830 | 0x4C | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Review fix: the second argument (set / clear) passes through to both callees. |
| func_001D2910 | 0x001D2910 | 0x4C | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001D4960 | 0x001D4960 | 0x70 | S2_opening | asm body + companion C | 90.18 | mwcc 2.3.3 | compiled asm body |  |
| func_001D6F60 | 0x001D6F60 | 0x94 | S2_opening | asm body + companion C | 98.65 | mwcc 2.3.3 | compiled asm body |  |
| func_001D7080 | 0x001D7080 | 0x78 | S1_newgame_load | asm body + companion C | 96.83 | mwcc 2.3.3 | compiled asm body |  |
| func_001D8270 | 0x001D8270 | 0xC4 | S2_opening | asm body + companion C | 95.92 | mwcc 2.3.3 | compiled asm body |  |
| func_001F0A60 | 0x001F0A60 | 0x6AC | S3_first_control_idle | NEARMISS C | 17.97 | mwcc 2.3.3 | original_assembly_nearmiss | Review fix: the packet's first quadword is cleared before the VIF word. |
| func_001F4D40 | 0x001F4D40 | 0xE0 | S2_opening | asm body + companion C | 98.93 | mwcc 2.3.3 | compiled asm body |  |
| func_001F54E0 | 0x001F54E0 | 0x15C | S2_opening | asm body + companion C | 98.51 | mwcc 2.3.3 | compiled asm body |  |
| func_001FBDB0 | 0x001FBDB0 | 0xC4 | 09_fence_door | asm body + companion C | 98.78 | mwcc 2.3.3 | original assembly (.s) | Review fix: func_00119890(1, voice). |
| func_001FE480 | 0x001FE480 | 0x30 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001FE530 | 0x001FE530 | 0x124 | S2_opening | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_00200780 | 0x00200780 | 0xA8 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00203A10 | 0x00203A10 | 0x6C | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00203B20 | 0x00203B20 | 0x50 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00203BA0 | 0x00203BA0 | 0x8C | S0_title | asm body + companion C | 72.57 | mwcc 2.3.3 | compiled asm body |  |
| func_002040E0 | 0x002040E0 | 0x5C | S0_title | asm body + companion C | 33.48 | mwcc 2.3.3 | compiled asm body |  |
| func_00204140 | 0x00204140 | 0x5C | S0_title | asm body + companion C | 33.48 | mwcc 2.3.3 | compiled asm body |  |
| func_002041D0 | 0x002041D0 | 0x74 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00204250 | 0x00204250 | 0x138 | S0_title | asm body + companion C | 100.00 | mwcc 2.3.3 | compiled asm body |  |
| func_00204390 | 0x00204390 | 0xF4 | S0_title | asm body + companion C | 99.84 | mwcc 2.3.3 | compiled asm body |  |
| func_00204700 | 0x00204700 | 0xD0 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_002047D0 | 0x002047D0 | 0x30C | S0_title | asm body + companion C | 75.30 | mwcc 2.3.3 | compiled asm body |  |
| func_00204D60 | 0x00204D60 | 0x130 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_00204E90 | 0x00204E90 | 0x1C0 | S0_title | asm body + companion C | 75.23 | mwcc 2.3.3 | compiled asm body |  |
| func_00205240 | 0x00205240 | 0x4BC | S0_title | asm body + companion C | 82.26 | mwcc 2.3.3 | compiled asm body | Review fix: rewritten from the instructions (full helper arguments, column-major upload; 43.95 -> 82.26). |
| func_00205700 | 0x00205700 | 0x34 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00205B00 | 0x00205B00 | 0xC0 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00205BC0 | 0x00205BC0 | 0x9C | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00205C60 | 0x00205C60 | 0x20 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00205D40 | 0x00205D40 | 0x90 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00205E10 | 0x00205E10 | 0x20 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00205E30 | 0x00205E30 | 0x10 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00205F20 | 0x00205F20 | 0x20 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00205F40 | 0x00205F40 | 0x10 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00205F90 | 0x00205F90 | 0x7C | S0_title | asm body + companion C | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_002065D0 | 0x002065D0 | 0x19C | S0_title | asm body + companion C | 92.11 | mwcc 2.3.3 | compiled asm body |  |
| func_00206770 | 0x00206770 | 0x98 | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00207150 | 0x00207150 | 0x13C | S0_title | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_0021B1B0 | 0x0021B1B0 | 0x344 | S1_newgame_load | asm body + companion C | 95.22 | mwcc 2.3.3 | compiled asm body |  |
| func_0021BB00 | 0x0021BB00 | 0x13C | S2_opening | asm body + companion C | 95.32 | mwcc 2.3.3 | compiled asm body |  |
| func_00224290 | 0x00224290 | 0x160 | 10_cage_roof_roger | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_002243F0 | 0x002243F0 | 0x204 | 12_crevice_jump | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) |  |
| func_overlay_AREA11_00823910 | 0x00823950 | 0x220 | 10_cage_roof_roger | C (already) | 100.00 |  | overlay (link_overlay) | Absorbed into the preceding overlay C unit (a split splat label; docs/AREA11_OVERLAY.md), already byte-identical C. |
| func_overlay_AREA11_00825500 | 0x00825540 | 0xC0 | S2_opening | C (already) | 100.00 |  | overlay (link_overlay) | Absorbed into the preceding overlay C unit (a split splat label; docs/AREA11_OVERLAY.md), already byte-identical C. |
| func_overlay_AREA11_00825600 | 0x00825640 | 0x90 | 10_cage_roof_roger | C (already) | 100.00 |  | overlay (link_overlay) | Absorbed into the preceding overlay C unit (a split splat label; docs/AREA11_OVERLAY.md), already byte-identical C. |
| func_overlay_AREA11_008256D0 | 0x00825710 | 0x90 | 11_crevice_prompt | C (already) | 100.00 |  | overlay (link_overlay) | Absorbed into the preceding overlay C unit (a split splat label; docs/AREA11_OVERLAY.md), already byte-identical C. |

## 4. Levers used

The matching guide (`docs/fanout/MATCHING_GUIDE.md`) idioms that closed functions here, and the
new findings of this lane. Measurements are objdiff on a private scratch build (`build/fld/`).

**New in this lane**

- **Dead join-head copies come from else-if chains.** CodeWarrior leaves a dead copy of a join
  block's first instruction after an unconditional branch; mwcc 2.3.3 keeps it too when the
  source is an `if / else if / else` chain (or a ternary) instead of early returns or a result
  variable. func_001D2910 / func_001D2830 (84.21 -> 100), func_00182430 (every surface case
  as `if (k == 3) ... else if (k == 2) ... else ...`, 78.40 -> 100), func_001B5C90 (74 -> 100),
  func_001B5D70 (70.20 -> 100). Earlier asm headers said "dead instruction not emitted by pure
  C"; that was wrong for these shapes.
- **Compute a value inside the branch that uses it** to reproduce a dead copy at the head of the
  else block: func_00207150 (`rest = n1 - sn1` inside the else, 98.73 -> 100).
- **Reverse declaration order of leaf temporaries** to swap two caller-saved registers:
  bone_init_default_1 (`char *rest; char *p;`, 97.61 -> 100); cached reads declared in reverse
  order fixed func_00204390's register swap (99.43 -> 99.84).
- **Ternary min in the `(max > x) ? x : max` spelling** gives the original's true-likely branch
  with the move in its slot and the dead move after it: func_0018C6A0, func_0018C4B0,
  func_00192010 (with the stores written in each arm and `x += ...` / `target += ...` compound
  assignments).
- **ee-gcc sibling calls need a void wrapper**: `void f(a) { g(K, a, 0, 0); }` gives the `j`
  tail call; `return g(...)` does not (func_00119810 / 00119978 / 0011A830, 0 -> 100).
- **libgcc soft-float predicates as static inline functions** (`x->class == 2` returned from a
  helper) reproduce ee-gcc's xori-and-branch compare of the class: float_to_int,
  func_00128250, func_001277B0, func_00126AB8, func_00127398 (all 100). The pack routines store
  through a float / double bitfield union (`fraction : 23, exp : 8, sign : 1`), which is where the
  `(garbage & 0xFF800000) | ...` insertion pattern comes from.
- **GS register packers**: OR the fields in the original's nesting (innermost pair first, each
  field cast to `unsigned long long` on its own): func_00205B00 / 00205BC0 / 00205D40 (0 -> 100).
  A GS register halfword update is a 64-bit bitfield store: func_00205700 (DISPFB FBP, 100).
- **A register argument the callee never reads is not an argument**: func_001F54E0's callback
  got its a1 from an unrelated float-constant load (96.67 -> 98.51 once dropped).
- **Hoist a float conversion into its own local** to keep it in a saved FPR across calls:
  func_0021B1B0 (38.69 -> 95.22).
- **Vector temporaries as `struct { float v[3]; int w; }`** so the w = 0 stores address the
  stack slot directly: func_0019F330 / func_0019ED80 (89 / 76 -> 96).

**Guide idioms that applied**

- idiom-28 strict relational respell for the `$at` compare: func_001AF780 (`> 0x1E`, 80.36 ->
  100), func_0015D000 (`> 0x3C`, `> 0x78`).
- idiom-25 float truthiness (`if (x)`): func_0015D000, func_002243F0 (99.26 -> 100),
  func_00224290.
- idiom-32 / per-file `// SPAD:` scratchpad externs: func_00176BE0 (0x700031C0..C8, 0x700031D0),
  func_0015B530 (D_70003B8D), func_001AF470 (D_70003B74..7E), func_002243F0 (D_70003A20),
  func_001C5680 / func_001C5760 (D_70003B68).
- idiom-33 sparse-switch label order (compare chain reversed): func_0015B530, func_00163B40,
  func_001BB0E0, func_001CF470, func_00182430; one-case switch for a branch-to-body layout:
  func_001FBDB0.
- idiom-34 large struct offsets / ring-buffer structs: func_00203A10 (fields at 0x50000).
- Prototype hygiene (exact arity and types from already-matched callers): func_001B1020,
  func_001D04B0, func_001B5F40 (int, not short, for the libpad info result), func_00205240
  (every packet helper with its full prototype and all the register and stack arguments the
  original sets; the earlier unprototyped calls dropped them).
- **Pass every argument register the callee reads.** A residual of one register choice or one
  branch is often an argument the C dropped: func_001BB0E0 (99.89), func_001C5680 (98.93),
  func_001C5760 (99.02) and func_001060F8 (74.60, with one store reordering) all reached 100% once
  the call passed what the original leaves in the argument register. Pass-through arguments
  (a value left untouched in a1 or f12 for the callee) must be named too: func_001D2830,
  func_00208AB0.

**Walls recorded (no source lever found)**

- **Intra-TU register analysis (IPA).** func_00205F90, func_001CC170, func_00204250 and
  func_001CC1E0 keep values in caller-saved registers across calls to a leaf in the same
  translation unit. Each reaches 100% objdiff (or close) with a `static` copy of the callee in
  the file, but `link.py` takes every `.text` section of the object, so the extra function would
  shift the image. Their C is kept as companions. A link step that drops a static helper's
  `.text` (or a `// STATIC_HELPER:` directive in `fill_unmatched.py`) would promote all four.
- (Withdrawn after review: the "branch-likely selection" wall of func_001C5680 / func_001C5760
  was a dropped argument. With func_001C2360(node) / func_001C22A0(node) both byte-match.)
- **Boolean lowering at a function's end** (func_001D8270, func_001B1380): the original's
  branch-to-exit with a dead constant load after it is not produced by any spelling tried.
- **Hand-written VU0 / MMI code**: the companions state the arithmetic; objdiff is near 0 by
  construction.

## 5. Gate

Final run after the C10 pass of section 7 (build lock held): `tools/decomp/build.py build`,
`tools/decomp/build.py objdiff`, `tools/verify_all.py` — all six stages PASS: boot ELF
byte-identical (0x175b00 loadable bytes), 19/19 overlays, matched_code 98.65% (functions
2155/2211, floor 95.0; 2152 before the pass, the 3 corrected C files are the gain), glTF, anim
self-test, GS offset. `tools/decomp/audit_link_provenance.py` on the same build: no copied-text
or relocation mismatches, no missing fillers; the "links as" columns come from it.

Re-run after the review fixes of section 8 (build lock held, 2026-10-01 13:12-13:25): `build.py
build` and `objdiff` rc 0, `verify_all.py` all six stages PASS — boot ELF byte-identical, 19/19
overlays, matched_code 98.65% (2155/2211; unchanged, because the four promoted functions were
already matched units as asm bodies), glTF, anim self-test, GS offset. The provenance audit shows
func_001BB0E0, func_001C5680, func_001C5760, func_001060F8, func_001D2830 and func_00208AB0
linking from compiled ordinary C, and no copied-text, relocation or pinned-rodata mismatches.

## 6. Open items

- **Stale force-list entries.** `tools/decomp/fill_unmatched.py` `SIZE_DRIFT_FORCE_ASM` still
  lists 16 of the functions byte-matched in section 3, and 7 of the C10 functions of section 7
  (func_00128390, func_001287F0, func_001639E0, func_0018C850, func_001A58B0, func_001F4F40,
  func_0021D1A0). The 16 of section 3 (column "links as" says "original assembly
  (.s)"): func_00128250, func_00163B40, func_0017DEB0, func_00182430, func_0018C4B0, func_0018C6A0, func_00192010, func_001B12B0, func_001B5F40, func_001C8710, func_001C9E40, func_001FE530, func_00204700, func_00204D60, func_00224290, func_002243F0. Their compiled C is 100% objdiff and
  the right size; removing the entries (a tools change, outside this lane) makes them link from
  C. Re-run the full gate after removing them.
- **Static helpers for intra-TU register analysis.** func_00205F90, func_001CC170,
  func_00204250 and func_001CC1E0 would link from C if the link step could drop the `.text` of a
  `static` callee copy (section 4).
- **sub__0000000000000000Inf** (newlib formatted output, 5400 bytes) is still `INCLUDE_ASM`.
- **23 hand-written functions** have no C form (section 3 describes each); the port replaces
  the movie, kernel and VU control ones with host services.
- The port's census (`../extermination-port/docs/FIRST_LEVEL_CENSUS.md`) shows the decomp
  status per row in its "Decomp" column; its next recount should pick these up.
- **Callee C files outside this lane with inexact prototypes** (found by the section 8 review;
  not edited here): func_001BA580's NEARMISS C narrows its second argument to `unsigned char`
  while the instructions compare the full sign-extended command id; func_00205A80's C declares
  four parameters and forwards a3, but the instructions read only the packet pointer;
  func_00205A90 / 00205EA0 / 00205EE0 / 00205A80 are written `void` although they tail-call
  func_00205A50 and return the advanced packet pointer, which every caller uses; func_001B61C0's
  C declares six parameters, of which the instructions read four.

## 7. C10 capture-lane deltas (aim / fire, exit and AREA01 arrival, damage, branches, options)

The PCSX2 capture lanes (docs/CAPTURES_C10.md) ran the first level off its main route and wrote
census deltas: functions that ran but are not in the 1184-row census. Files:
`build/aimfire/capture/census_delta.json` (AIM, 114 new), `build/c10/exit/census_delta.json`
(EXIT / EXITB, 75), `build/c10/damage/census_delta.json` (DMG / DMGB, 50),
`build/c10/branch/census_delta.json` (BR / BRB, 69) and `build/c10/options/census_delta.json`
(OPT / OPTB, 27). Together they name 281 distinct functions; 232 were already byte-matched C or
NEARMISS C. This section covers the other 49: 30 boot functions whose source was a CodeWarrior
`asm` body or an `INCLUDE_ASM` stub, 3 boot functions with ordinary C that did not match (so the
link used the `.s`), 1 address with no file, 13 AREA01 overlay functions and 2 AREA11 overlay
labels the deltas reported as overlay C or undecompiled.

### Result

| Now | Functions | Meaning |
|---|---:|---|
| C, byte-matched | 23 | objdiff 100%, right `.text` size, no data sections. 20 replaced an asm body; the 3 non-matching C files were corrected. 16 link from the compiled C; 7 still link from the `.s` because of stale `SIZE_DRIFT_FORCE_ASM` entries (section 6). |
| asm body + companion C | 9 | Readable C in `src/readable/<f>.c` (NEARMISS header, registry row in docs/NEARMISS.md), pointer comment at the end of the asm file. 3 of them are pure SDK VU0 leaves (0%). |
| NEARMISS C | 1 | func_001CE860 (former `INCLUDE_ASM`, the cable / trail ribbon packet with VU0 sections, 27.43%). |
| C already | 16 | 13 AREA01 overlay functions and 2 AREA11 labels are byte-identical overlay C; 0x001C0004 is an entry inside func_001BFFD0, covered by that function's NEARMISS C. |

So every function the C10 lanes added now has readable C; 23 of the 34 boot functions are
byte-identical. The census functions of section 3 were also revisited with the new scratchpad
lever below: func_001A4650 and func_001A5760 are now byte-matched C (their companions were
removed and their registry rows dropped) and func_001A4D10's companion went from 75.57% to
88.68%.

### Coverage table

"lanes" is the set of C10 lanes whose delta lists the function; "first beat" is the beat that
first ran it. objdiff on the private scratch build (`build/fld/`); "links as" from
`tools/decomp/audit_link_provenance.py` after the final build.

| function | address | bytes | first beat | lanes | now | objdiff % | compiler | links as | note |
|---|---|---|---|---|---|---:|---|---|---|
| func_00102870 | 0x00102870 | 0x20 | aim_00_r1_hold | AIM | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_001028E8 | 0x001028E8 | 0x14 | br_08_west_ladder_down | BRANCH | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_00102990 | 0x00102990 | 0x10 | aim_09_melee | AIM,BRANCH | asm body + companion C | 0.00 | mwcc 2.3.3 | compiled asm body |  |
| func_00123280 | 0x00123280 | 0x5C | opt_00_browse_close | OPTIONS | C, byte-matched | 100.00 | ee-gcc | compiled C |  |
| func_00128390 | 0x00128390 | 0x34 | exit_01_movie_arrival | EXIT | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) | Was C linked from asm; C corrected. Stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_001287F0 | 0x001287F0 | 0x38 | exit_01_movie_arrival | EXIT | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) | Was C linked from asm; C corrected. Stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_001639E0 | 0x001639E0 | 0x160 | dmg_06_crevice_fall | DAMAGE | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) | Stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_0017C370 | 0x0017C370 | 0xD0 | dmg_00_flame_hit | DAMAGE | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_00183AC0 | 0x00183AC0 | 0xB8 | aim_04_world_hit | AIM | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_0018C850 | 0x0018C850 | 0xCC | aim_01_r2_hold | AIM | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) | Stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_001A44B0 | 0x001A44B0 | 0x19C | br_08_west_ladder_down | BRANCH | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001A4830 | 0x001A4830 | 0x4E0 | br_08_west_ladder_down | BRANCH | asm body + companion C | 96.63 | mwcc 2.3.3 | compiled asm body |  |
| func_001A58B0 | 0x001A58B0 | 0x374 | dmg_07_pit_fall | DAMAGE,BRANCH | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) | Stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_001AA000 | 0x001AA000 | 0x138 | exit_01_movie_arrival | EXIT | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001BF630 | 0x001BF630 | 0x7C | exit_01_movie_arrival | EXIT | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001C0004 | 0x001C0004 | 0x2D4 | exit_01_movie_arrival | EXIT | C already (NEARMISS) | 97.24 | mwcc 2.3.3 | original assembly (.s) | Not a function of its own: an entry inside func_001BFFD0 (0x1BFFD0..0x1C02D8), whose NEARMISS C covers it. |
| func_001C2540 | 0x001C2540 | 0x98 | exit_01_movie_arrival | EXIT | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| bone_init_default_0 | 0x001C6200 | 0xB8 | br_04_crate_stack_break | BRANCH | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001CA3B0 | 0x001CA3B0 | 0x118 | aim_03_single_fire | AIM,BRANCH | asm body + companion C | 94.34 | mwcc 2.3.3 | compiled asm body |  |
| func_001CA4D0 | 0x001CA4D0 | 0x104 | aim_03_single_fire | AIM,BRANCH | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001CB480 | 0x001CB480 | 0x70 | br_01_map_item | BRANCH | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001CB4F0 | 0x001CB4F0 | 0x90 | aim_05_burst_fire | AIM | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001CE860 | 0x001CE860 | 0x678 | aim_11_cable_melee | AIM | NEARMISS C | 27.43 | mwcc 2.3.3 | original assembly (.s) |  |
| func_001D5A70 | 0x001D5A70 | 0x158 | exit_01_movie_arrival | EXIT | asm body + companion C | 43.80 | mwcc 2.3.3 | compiled asm body |  |
| func_001EFE00 | 0x001EFE00 | 0xAC | aim_11_cable_melee | AIM,DAMAGE | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001EFF10 | 0x001EFF10 | 0xC0 | aim_09_melee | AIM,BRANCH | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001F00A0 | 0x001F00A0 | 0x7C | aim_04_world_hit | AIM | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_001F4BF0 | 0x001F4BF0 | 0xC8 | exit_01_movie_arrival | EXIT | asm body + companion C | 99.70 | mwcc 2.3.3 | compiled asm body |  |
| func_001F4F40 | 0x001F4F40 | 0x48 | aim_03_single_fire | AIM | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) | Was C linked from asm; C corrected (func_001F5040 typed as the node worker it is). Stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_00208AB0 | 0x00208AB0 | 0x18 | aim_05_burst_fire | AIM,BRANCH | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C | Review fix: the float argument passes through in f12 to func_001D66A0. |
| func_0021BC40 | 0x0021BC40 | 0xC8 | dmg_00_flame_hit | DAMAGE | C, byte-matched | 100.00 | mwcc 2.3.3 | compiled C |  |
| func_0021C350 | 0x0021C350 | 0x94 | dmg_00_flame_hit | DAMAGE | asm body + companion C | 99.73 | mwcc 2.3.3 | compiled asm body |  |
| func_0021D1A0 | 0x0021D1A0 | 0xAC | dmg_00_flame_hit | DAMAGE | C, byte-matched | 100.00 | mwcc 2.3.3 | original assembly (.s) | Stale SIZE_DRIFT_FORCE_ASM entry (section 6). |
| func_00225720 | 0x00225720 | 0x2DC | dmg_05_load_screen | DAMAGE,OPTIONS | asm body + companion C | 99.92 | mwcc 2.3.3 | compiled asm body |  |
| overlay_AREA01_func_00823540 | 0x00823580 | 0x24C | exit_01_movie_arrival | EXIT,DAMAGE | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA01 overlay C, byte-identical (docs/AREA01_OVERLAY_C.md). |
| func_overlay_AREA01_00823A10 | 0x00823A50 | 0x34 | exit_01_movie_arrival | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA01 overlay C, byte-identical (docs/AREA01_OVERLAY_C.md). |
| func_overlay_AREA11_00823B70 | 0x00823BB0 | 0x90 | br_14_roger_talk | BRANCH | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | A split splat label inside func_overlay_AREA11_00823B30 (runtime 0x823B70), already byte-identical C (docs/AREA11_OVERLAY.md). |
| func_overlay_AREA11_00823C00 | 0x00823C40 | 0x40 | exit_00_departure | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA11 overlay C, byte-identical. |
| func_overlay_AREA11_00823C40 | 0x00823C80 | 0x60 | exit_00_departure | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | A split splat label inside func_overlay_AREA11_00823C00 (runtime 0x823C40), already byte-identical C (docs/AREA11_OVERLAY.md). |
| func_overlay_AREA01_00825310 | 0x00825350 | 0x160 | exit_01_movie_arrival | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA01 overlay C, byte-identical (docs/AREA01_OVERLAY_C.md). |
| func_overlay_AREA01_00825470 | 0x008254B0 | 0xD4 | exit_01_movie_arrival | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA01 overlay C, byte-identical (docs/AREA01_OVERLAY_C.md). |
| func_overlay_AREA01_00825700 | 0x00825740 | 0x1C4 | exit_01_movie_arrival | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA01 overlay C, byte-identical (docs/AREA01_OVERLAY_C.md). |
| func_overlay_AREA01_00826160 | 0x008261A0 | 0x60 | exit_01_movie_arrival | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA01 overlay C, byte-identical (docs/AREA01_OVERLAY_C.md). |
| func_overlay_AREA01_008261C0 | 0x00826200 | 0x23C | exit_01_movie_arrival | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA01 overlay C, byte-identical (docs/AREA01_OVERLAY_C.md). |
| func_overlay_AREA01_00826400 | 0x00826440 | 0x374 | exit_01_movie_arrival | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA01 overlay C, byte-identical (docs/AREA01_OVERLAY_C.md). |
| func_overlay_AREA01_00826780 | 0x008267C0 | 0x18C | exit_01_movie_arrival | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA01 overlay C, byte-identical (docs/AREA01_OVERLAY_C.md). |
| func_overlay_AREA01_00826CB0 | 0x00826CF0 | 0x44 | exit_01_movie_arrival | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA01 overlay C, byte-identical (docs/AREA01_OVERLAY_C.md). |
| func_overlay_AREA01_00826D00 | 0x00826D40 | 0x15A8 | exit_01_movie_arrival | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA01 overlay C, byte-identical (docs/AREA01_OVERLAY_C.md). |
| func_overlay_AREA01_00828810 | 0x00828850 | 0x198 | exit_01_movie_arrival | EXIT | C already | 100.00 | mwcc 2.3.3 | overlay (link_overlay) | AREA01 overlay C, byte-identical (docs/AREA01_OVERLAY_C.md). |

### Levers found in this pass

- **Scratchpad inputs as symbols, outputs as literals.** The collision workers read the probe
  segment from 0x70003190..0x700031A8 and write the hit to 0x700030CA..0x700031B8. Naming the
  inputs as `D_7000xxxx` externs with a per-file `// SPAD:` directive, while the outputs stay
  `*(float *)0x7000xxxx` stores, reproduces both the original's scheduling (no `lui` hoisted into
  the branch slots of the range tests) and its store order (the literal stores are not reordered
  around each other). func_001A44B0 (99.17 -> 100), func_001A4650 (81.96 -> 100). All-literal or
  all-symbol spellings leave one of the two wrong. func_001A58B0 matches with all its scratchpad
  words as symbols.
- **Pass-through arguments.** func_001F00A0 forwards its own a0 / a1 untouched to
  func_001EF9D0(kind, pos, scale); declaring them as the callee's arguments instead of an unused
  parameter fixes the saved-register copy order (99.35 -> 100). The same reading gave
  func_001287F0 its fourth (float) parameter, passed through to anim_clip_init in f12.
- **One-case switch** for an `if` whose then-block the original reaches with a taken branch and
  a `b` over a dead copy: func_001EFE00 (93.95 -> 100). Case labels in reverse compare order
  (idiom-33) gave func_00183AC0 and func_001639E0.
- **`&&` chains with an else-return** reproduce the shared return-0 block before the body:
  func_001A4650, func_001A5760, func_001A44B0; the call result compared on its own line
  (`d = sqrtf(...); r = ...; if (!(d <= r)) return 0;`) keeps the sum after the call:
  func_001AA000 (76.59 -> 100).
- **`a1 == 0 ? x : y`** gives the original's `movn` with x in v0 (func_00128390, 90.38 -> 100);
  `a1 ? y : x` gives `movz`.

### Walls and residuals

- Prologue / argument-register colouring: func_00225720 (99.92, a2 vs a0 for one byte),
  func_0021C350 (99.73, one FPU pair), func_001F4BF0 (99.70, f12 / f13 of a shared 3.0).
- func_001CA3B0 (94.34): FPU colouring of the three half angles.
- func_001A4830 (96.63): the mixed scratchpad spelling lines the control flow up; one register
  pair and the reload order of the published hit remain.
- func_001CE860 (27.43), func_001D5A70 (43.80) and the three SDK VU0 leaves (0%): VU0 macro
  code; the C states the arithmetic.

## 8. Review fixes (2026-10-01)

A review of this lane re-measured every companion and NEARMISS body and read about 50 of them
line by line against the instructions. Byte identity, the matched count, link provenance and the
leak scan held, but 9 readable bodies did not say what the original does. All are corrected:

| function | defect | fix | objdiff before -> after |
|---|---|---|---|
| func_00205240 | packet helpers called without the register / stack arguments the original sets (DMA tag ids and QWCs, GIF NLOOP / EOP / FLG, TEX0 TW / TH / TFX, TEX1 MMIN); upload loops nested row-major | rewritten from the instructions with each helper's full prototype; x outside, y inside, src += 0x400 per block | 43.95 -> 82.26 (companion) |
| func_001060F8 | 10th parameter missing; func_00106278 called with 4 of its 7 arguments | `(p, a1, a2, a3, top, halve, 0)` at all three sites; slot stores in source order | 74.60 -> 100 (promoted) |
| func_001BB0E0 | func_001BA580 called without the command id the callee classifies | `func_001BA580(a, cmd[2])` | 99.89 -> 100 (promoted) |
| func_001FBDB0 | func_00119890 called without the voice it indexes | `func_00119890(1, voice)` | 98.78 -> 98.78 (companion) |
| func_001C5680 / func_001C5760 | func_001C2360 / func_001C22A0 called with no argument; the callee reads the node | `f(node)` | 98.93 / 99.02 -> 100 / 100 (promoted) |
| func_001F0A60 | the packet's first quadword was not cleared before the VIF DIRECT word | 16 bytes zeroed first | 13.88 -> 17.97 (NEARMISS) |
| func_001D2830 | the set / clear flag that passes through in a1 to both callees was hidden | `int func_001D2830(int id, int on)` | 100 -> 100 |
| func_00208AB0 | the float that passes through in f12 to func_001D66A0 was hidden | `void func_00208AB0(float *pos, int *a2, int *a3, float f)` | 100 -> 100 |

Minor fixes: func_001F4F40 declares func_001F5040 as the node worker it is (`void (char *)`);
the three rotation companions (func_00102A60 / 00102B08 / 00102BB0) now state the original's
pi/2 fold, series cosine and signed sqrt sine (valid for angle in [-pi, pi]; outside it the
sine keeps the angle's sign) instead of libm sinf / cosf; func_001CCB10 / func_001CCBD0 build
the packet and the palette in one 0x170-byte struct, which is the DMA source (func_001CCBD0
84.67 -> 90.69); copy_qw4 and func_00109AF8 quote the ee-gcc score (94.67 / 73.89; mwcc gives
96.22 / 75.33); `_iSignalSema` links as a compiled asm body (audit row `iSignalSema`). The audit
asked about func_0015D000, func_00224290 and func_002243F0: their four-argument calls of
func_001B61C0 are complete, because that callee reads only a0..a3 (its C declares two more
parameters that the instructions overwrite before use).

**Arity re-scan.** Every lane file (the 284 of the first pass, the C10 files and all
`src/readable/` companions) was then scanned two ways: (1) the argument registers the original
writes before each call against the C call's argument count, and (2) the argument registers
(a0..t3, f12..f19) each callee reads before writing them against the C call's argument count,
which also catches pass-through values. The callee side is a straight-line scan of the callee up
to its first call (branches not followed), so it is a screen, not a proof. The detector was validated on the old func_00208AB0
text (it flags the missing f12). After the fixes above it reports nothing; the remaining
type-(1) hits are temporaries in t0..t3 that the callee never reads.
