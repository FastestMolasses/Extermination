

Ninth batch, same wave (10 byte-matched, all compiled_object_ordinary_c;
six NEARMISS bodies corrected):

- **Wrong bodies**: func_0017D080 (a failed first probe returns 0; a 0x46
  hit skips only when the facing flag is clear), func_001CC8A0 (a
  nine-argument func_001CCE80; a pointer table), func_00110048 (RPC
  arguments in the wrong order, a wrong status tail), func_00110198, both
  ee-gcc.
- **Shapes**: func_00132490 (the table switch's default leaves with
  `goto done` past the integration loop), func_00229A70, func_001416D0
  (PI/4, not PI; one exit), func_00159970 (twin func_00159620),
  func_00109FD0 (default-then-overwrite tests become conditional moves,
  ee-gcc), func_001F5040 (switch label order read off the reversed compare
  chain).
- **mwcc 2.3.3 switch rule**: the compare chain is emitted in reverse source
  order and the case bodies in source order, so both orders read off the
  target fix the source order (func_00189EC0, func_001F5040).
- NEARMISS bodies corrected: func_0017C860 (82.66 -> 95.95), func_001F28D0
  (82.98 -> 98.80, an inverted latch test), func_00109E68 (80.71 -> 91.07),
  func_0022E7F0 (80.03 -> 99.87, instruction-identical; only `+8` relocations
  differ), func_001575E0 (78.93 -> 92.28), func_00189EC0 (73.00 -> 99.93).
- fill_unmatched.py: func_00159970 dropped from GPREL_FORCE_ASM.
- Not accepted: func_001DB480 reaches 100 only with four inline VU0 `asm`
  statements (row loads); left for a user decision. func_001CE300 is
  hand-written VU0 macro code (asm-void class).
