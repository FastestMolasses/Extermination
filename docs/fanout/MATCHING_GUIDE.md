

Sixth batch, same wave (10 byte-matched, all compiled_object_ordinary_c;
two NEARMISS bodies corrected):

- **libgcc fp-bit** (ee-gcc): func_00126F88 is `__muldf3` (59.59 -> 100),
  func_00127E40 is `__divsf3` (50.84 -> 100; it was filed as mwcc and
  declared `void`; the `sd` saves and 0x50 frame are ee-gcc's).
- **Renamed functions with jump tables**: tools/match/gen_jtbl_rodata.py only
  scanned `func_*` names, so anim_frame_top_b (and bone_root_pulse,
  gs_vram_readback, sub_EXTERMINATION, ...) never got a build/jtblrodata
  table and could not reach 100. targets() now includes renamed boot
  functions with a src/ file; anim_frame_top_b matched once its table was
  generated.
- **Wrong arguments / bodies**: func_002217C0, func_00153290 (copies into
  D_700036A0; a NULL ends the function), func_0018AF50, func_00143AF0 (four
  separate float stores), func_0016A4B0, func_001BECC0, func_001E55F0.
- Corrected NEARMISS bodies (still not byte-identical): func_001E3630
  93.92 -> 99.85 (wrong store targets, an inverted loop test, buffer size),
  func_001F88C0 93.45 -> 95.90 (a missing early return).
- **A compiler we do not have**: func_002101C0, func_001F88C0 and
  func_0020CDC0 each need mwcc 2.3.3's scheduling and argument order
  together with 991202's delay-slot moves (the instruction moved out of the
  branch target, no copy left behind). No byte-matched 2.3.3 or 991202
  function shows that combination (a scan of 985 + 721 targets); 2.4
  copies like 2.3.3. The original's own 2.3.1.01 build, or mwcc 3.0 / 3.0.1
  (configured in build.py but not installed), are the candidates.
- fill_unmatched.py: func_001BECC0 dropped from GPREL_FORCE_ASM.
