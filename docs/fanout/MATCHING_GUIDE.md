

Tenth batch, same wave (8 byte-matched, all compiled_object_ordinary_c;
three NEARMISS bodies corrected):

- **Wrong bodies / arities**: func_001DDB70 (63.66 -> 100: fields the
  original never writes, three callee arities), func_001BB860 (57.68 ->
  100: two real switches, one-argument func_001AFC10), func_001861C0
  (62.91 -> 100: a 260.0 constant, six-argument func_001B41F0, three branch
  corrections), func_0014FBC0 (72.16 -> 100), func_0010F3B0 (ee-gcc; a
  64-bit mode shift), func_00114C30 (ee-gcc; the request block is a
  0x414-byte struct).
- **Shapes**: func_0020D930 (half-open angle-range chains; explicit
  soft-float calls to func_00100130 / func_00128350, since plain `>=` makes
  mwcc call its own helpers), func_001FAFD0 (one early return for both
  gates; a negated `&&` chain keeps the three area compares).
- NEARMISS bodies corrected: func_0016F600 (79.81 -> 98.99), func_001E3D90
  (74.39 -> 96.81), func_001EB250 (64.54 -> 99.90).
- fill_unmatched.py: func_001DDB70 and func_0014FBC0 dropped from
  GPREL_FORCE_ASM.
- Rule found: mwcc 2.3.3 never moves a load above a store to a different
  literal address; when the target does, the source read the value into a
  local first or the address is a named extern.
- Tooling lever found (not applied): func_0010E088 and func_0010EAA0 match
  when assembled with the ee-gcc 2.96 gas (tools/eegcc/ee-gcc2.96), which
  fills a call's delay slot with the following `ei`; the 2.9 gas never
  fills call slots. It would need a per-file assembler option, since the
  matched ee-gcc corpus relies on unfilled slots.
