

Eleventh (final) batch, same wave (8 byte-matched, all
compiled_object_ordinary_c; three NEARMISS bodies corrected):

- func_00186A60 (a ternary clamp like twin func_0018C4B0; locals read before
  a literal-address store), func_0012E560 (both bit-0 paths store the same
  table; two-argument func_00102948), func_00171670 (twin func_00171320;
  case 1 increments the mode, case 12's test was inverted), func_00203E60
  (`int *` size table read as `arg2[0x14002]`; one capacity local),
  func_001551B0 (idiom-32 on 0x700038A4 / 0x700038B4), func_001C4A00
  (self-taking callees; idiom-32 on 0x70003B92), func_001AF2C0 (the
  0x008107xx destinations volatile), sub_The_size_of_work_area_is_too_sma
  (ee-gcc; the success path ends in the tail-call statement with no
  `return`, which is the only form ee-gcc turns into a sibling jump, so the
  original very likely had the same missing return).
- NEARMISS bodies corrected: anim_matrix_multi 96.60 -> 99.88 (three-float
  func_001B1560), func_00195130 88.48 -> 99.96 (-598.9 not -598.4; the -20
  limit stores 0.5*(t - lim)), func_00203D30 78.08 -> 99.19 (a missing
  first parameter; five-argument func_00206BA0).
- The 0x008107xx game-state globals needed `volatile` to keep store order in
  three byte-matched functions (anim_frame_top_a, func_001AB430,
  func_001AF2C0); the original likely declared that block volatile.
- fill_unmatched.py: func_0012E560 dropped from GPREL_FORCE_ASM.

### Wave summary and open decisions (2026-10-27)

134 NEARMISS functions byte-matched from readable C in eleven batches (one
subagent per function, every result re-measured, every batch gated by a
fresh build + verify_all + audit_link_provenance); about 25 NEARMISS bodies
corrected. The levers, by frequency: wrong callee prototypes / arity
(checked against each callee's definition in src/), idiom-32 scratchpad
externs, `break` vs `return` and switch case order, byte-matched twins'
spelling, idiom-24 int-staged float constants, wrong bodies. Rejected as not
honest C: permuter-made comma expressions and write-only locals.

Open, each needs a decision or a tool change before more functions can
match:
1. **Relocation naming** (func_002034C0 99.85, func_0022E7F0 99.87,
   func_001575E0): the instructions match, but the compiled object relocates
   against `D_x+n` where splat names the split symbol (or a bare literal).
   Normalising both sides to the containing data symbol + offset in the
   objdiff inputs would score them 100.
2. **Compiler mix** (func_002101C0, func_001F88C0, func_0020CDC0,
   func_0022DCD0, func_001E7D20): 2.3.3 scheduling with 991202's delay-slot
   moves. mwcc 3.0 / 3.0.1 are configured in build.py but not installed;
   the original 2.3.1.01 build would be the real answer.
3. **ee-gcc 2.96 assembler** (func_0010E088, func_0010EAA0): a per-file
   option to assemble with the 2.96 gas, which fills a call slot with `ei`.
4. **Inline VU0 asm** (func_001DB480): 100 with four row-load `asm`
   statements; not accepted without a user decision.
5. **Symbol split** (gs_readback_queue_run): the code is identical; splat
   folds the VBLANK handler at 0x1AB140 into the symbol.
6. Callee definitions that disagree with every byte-matched caller:
   func_0021BE40 (callers pass two arguments), func_0020CDA0,
   func_001FEC20 / func_001FECB0, func_00100130 / func_001000C0
   (pass-through doubles), anim_bone_array_setup (callers pass the count),
   func_0010BF10, func_001F4F90.

### Tooling batch: open decisions 1-5 (2026-10-27)

The user approved all five items above. Results:

1. **Relocation naming: done.** `tools/decomp/reloc_normalize.py` (tests:
   `tools/decomp/test_reloc_normalize.py`) reads the compiled object's
   HI16/LO16 relocations and respells the matching site of the expected `.s`
   to the compiled spelling (`D_x+off`, or a bare literal where the compiled
   side has none). It changes a site only when both sides resolve to the same
   address and the instruction words agree outside the immediate, so a real
   mismatch stays visible. `build.py compile` runs it after inject_relocs and
   reassembles the changed targets. func_0022E7F0 scored 100 with no source
   change; func_002034C0 and func_001575E0 followed with corrected bodies.
2. **Compiler mix: blocked.** No mwcc 3.0 / 3.0.1 / 2.3.1.01 build is on
   disk. The five functions stay NEARMISS until the user supplies one.
3. **ee-gcc 2.96 assembler: done.** `// EEGCC_AS: 2.96` in an eegcc file makes
   `tools/eegcc/ee-compile.sh` assemble with the 2.96 gas (it fills a call or
   return slot with `ei` or a preceding load; 2.9 never does). Matched
   func_0010EAA0 (C unchanged), func_0010E088 and func_00113478.
4. **Inline VU0 asm: done for func_001DB480 only** (user-approved). Four
   GCC-style row-load `asm` statements; with them mwcc stops strength-reducing
   the grid loop. Other VU0 macro-code functions stay NEARMISS.
5. **Symbol split: done.** `config/symbol_addrs.txt` names the VBLANK handler
   at 0x1AB140 `func_001AB140`; its existing CodeWarrior `asm` body moved
   unchanged to `src/func_001AB140.c`, and gs_readback_queue_run (the code was
   always identical) is a 100% unit. A re-run of `build.py setup` changed only
   those two `.s` files.

Also: `build.py setup` now runs splat as `python -m splat` with stdin closed.
The venv's `splat` launcher kept the shebang of the iCloud copy of the repo
it was created in, and one file read there blocked for two hours.

Byte-matched in this batch (14): func_0022E7F0, func_002034C0, func_001575E0
(relocation naming); func_0010EAA0, func_0010E088, func_00113478 (2.96 gas);
func_001DB480 (inline asm); gs_readback_queue_run (split); func_001AC7F0
(`break` not `return`, restoring three unreachable slot-fill leftovers),
anim_matrix_player (one variable carries the requested clip in both arms),
func_00107CF0 (`(int)(frac >> 1)`), func_0014C680 (state 0 falls through to
case 1; idiom-32), func_001B3F10 (idiom-32 query vectors), sub_pts_is_not_used
(ternary clamp).
