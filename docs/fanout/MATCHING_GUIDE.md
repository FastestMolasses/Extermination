

Seventh batch, same wave (10 byte-matched, all compiled_object_ordinary_c;
two NEARMISS bodies corrected):

- **Wrong callee arity / body**: func_0021FB40 (func_001749A0's fourth
  float argument; func_00178B90 takes (self, 1)), func_001BF3C0
  (func_001B1020 takes the actor first; D_00275630 is a gp-relative
  short[4]), func_001FA790 (func_00128250 is `__fixunssfsi(float)`; a
  4-byte table stride, not 16), func_001BAD40 (case 5 returns
  `obj[4] >= 2`), func_001612D0 (one-argument func_00184BA0, sibling
  func_0016FCF0's shape), func_00187780 (one `float col[2][4]` array, a
  450.0 constant), func_00133640 (scratchpad vector order).
- **Twins**: func_00221FC0 (twin func_00222580), func_0014DC30 (twin
  func_0014D7C0's volatile arrays; a 0x3D0EFA35 ease rate), func_001C7EB0
  (one shared index / result variable; break-terminated cases).
- NEARMISS bodies corrected: func_0015A070 88.90 -> 99.35, func_0022B420
  87.95 -> 99.59 (an inverted cancel return, wrong countdown field).
- One 100% candidate was rejected: func_0015A070 matched only with a
  permuter-made `(0, *(p = ...))` comma expression.
- fill_unmatched.py: func_001BF3C0 dropped from GPREL_FORCE_ASM.
- Two more functions joined the compiler-mix class (func_0022DCD0: 991202's
  relocated-`lui` speculation into switch slots with 2.3.3's loop
  scheduling; func_001E7D20).
