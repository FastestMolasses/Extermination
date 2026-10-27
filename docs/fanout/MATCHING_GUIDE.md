

Fifth batch, same wave (12 byte-matched, all compiled_object_ordinary_c):

- **`break` instead of `return`** again: func_00167C80, func_001BE6C0.
  Not every switch wants it:
  func_001921D0, anim_frame_top_a and func_0013CD50 keep real `return`s.
- **libgcc fp-bit soft-float** (ee-gcc): func_00127230 is `__divdf3`, written
  in fp-bit's own shape (inlined `_fpdiv_parts`, isnan/isinf/iszero helpers,
  unpack by address, pack with one argument), like the matched `__mulsf3`
  func_00127C48.
- **Callee arity / arguments**: func_00141D20 and func_001B2140's callers
  (func_001B2140 takes the entity), func_001C02E0 (one-argument
  func_001AFA90 / func_001B6660), func_001BE6C0 (func_001B1020 takes self
  first; gp-relative D_00275618 under `-sdatathreshold 8`).
- **Compiler**: func_001B2140 is mwcc 2.3.3, not 991202 (its neighbours
  are 2.3.3; 991202 orders the jump-table address build differently).
- **idiom-24** again: func_001921D0 (2-degree rate; a 10.0 / 15.0 pair),
  func_00196CE0 (a 10.0 in one case only).
- **Shapes**: func_001B7700 (the outer if/else chain is a `switch`, cases
  0 and 2 share a body, like its neighbours func_001B73A0 / func_001B7840),
  func_0014CDD0 (block order, re-read fields, `(short)` before the mask),
  func_0013CD50 (an explicit `case 3: return 3;`, declaration order,
  pointer-first increments).
- anim_frame_top_a needs seven level-seed globals declared volatile to keep
  the original store order (plain stores reach 96.97%); noted as a
  readability cost.
- fill_unmatched.py: func_001BE6C0 and func_001C02E0 dropped from
  GPREL_FORCE_ASM.
