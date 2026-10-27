

Fourth batch, same wave (13 byte-matched, all compiled_object_ordinary_c):

- **Wrong callee arity or arguments** (the queue was ordered by an automated
  scan of NEARMISS externs against byte-matched definitions):
  func_00114708 (63.02 -> 100: func_0010E8A8 takes nine arguments, the
  receive buffer was missing; the request block is a struct written field by
  field), func_00114630 (the same request block as a struct, ee-gcc),
  func_0016BE40 (func_001751A0 takes the actor only), func_001582E0
  (func_001B0FD0 / func_001AFC10 take self only), func_001383C0 and
  func_00147390 (func_001B2140 takes the entity, not the constant 1; outer
  mode switch spelled like the twin func_0013D2D0), func_001399F0
  (func_001EFF10's seventh float argument, plus a wrong store target),
  func_0014A620 (state handlers take (self, other); the dirty flag is cleared
  before the call), func_00135870 (anim_clip_init arguments, four-argument
  func_001FBD50), func_001B0EA0 (twin func_001C22A0's declarations).
- **Wrong bodies**: func_001634A0 (an early-return condition was reversed),
  func_00224600 (state-0 guard shape; each arm stores the next state).
- **Twin spelling** again: func_001424C0 from func_00147B50 (inline random
  calls, `> 2`, `(signed char)flag & 0x80`, idiom-32, idiom-24 for 0.0).
- `break` vs `return` keeps cracking switches (func_001634A0 here).

Parked in batches 3-4 (best candidates not committed): func_00139240 99.71,
func_00139E00 99.81 (func_001B1270 argument order), func_001D1C50 99.93
(one add's operand order), func_002034C0 99.85 (the instructions match; the
two stores relocate against D_007A5570+0x10 / +0x20 where splat names
D_007A5580 / D_007A5590: a symbol-naming change would make it 100),
bone_root_pulse 99.97 (one FP register; its build/jtblrodata file must be
generated before a match can score 100), func_001647D0 99.75, func_0015D4C0
98.92, func_00128C10 98.57, func_00137EB0 98.80, func_001A5C30 97.83,
func_001F9820 97.78, func_0012FC10 97.81, func_002101C0 96.18 (needs mwcc
2.3.3's argument order and 991202's delay-slot fills at once),
func_00112F98 96.96, func_0010E8A8 99.89 (ee-gcc store order).
