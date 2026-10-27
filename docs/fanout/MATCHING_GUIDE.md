

Eighth batch, same wave (10 byte-matched, all compiled_object_ordinary_c;
two NEARMISS bodies corrected):

- **Library shapes (ee-gcc)**: func_00120B98 is newlib's `__smakebuf`
  (one `||` failure test, one-argument tty probe, chained base/cursor
  store); func_00107E88 is void and tail-calls (the "return 1" was the
  compare constant).
- **Wrong arity / types**: func_002280F0 (six-argument func_001FEC20),
  func_00171320, func_001723D0, func_00188430 (twin func_0018A8D0's
  `short` count), func_001AB430, func_0019A910 (one-argument
  func_001A6AD0), sub_BASCUS_97112_DS00_00_EX_DATA_00 (typed save-task
  record).
- **Shapes**: func_001FECB0 (a switch with the flag store in each arm).
- NEARMISS bodies corrected: func_001DFF70 91.97 -> 98.67 (case 0 sets
  state 1), func_001E4CE0 83.90 -> 98.59 (func_001CD2B0 takes four floats).
- Tooling gaps found (not changed): gs_readback_queue_run is
  instruction-identical but splat folds the VBLANK handler at 0x1AB140
  into its symbol; func_0022E7F0 (99.87, held for the next batch) is
  instruction-identical but objdiff counts `D_700038A0+8` relocations
  against splat's bare 0x700038A8 literals; func_002034C0 likewise
  differs only in relocation symbol names.
