// INCLUDE_ASM func_00187DE0  (vram 0x00187DE0, 180 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (s37, FINDINGS "FOOTSTEP SURFACE TABLE"): WATER-ENTRY
// one-shot, called by the footing update func_00175900 on first
// contact with floor attr 0x5B (after the depth probe set +0x23C to
// 1 shallow / 2 deep): splash effect 0x80000016 at the probe hit
// point (spad 0x700031B0), loop level func_001E8B90(pos, 5.0) unless
// D_00810700 == 0x15, then func_001FB9F0(0xCA shallow / 0xDB deep,
// 0x1000, 0x1000, 0x1000).
//
// MATCHING: wall #13 — the D_00810700 compare's beq;nop slot has a
// safe fall-through candidate (the 5.0f lui), which mwcc hoists into
// the slot; plus the +0x23C branch duplicates the 0x1000 constant
// into the slot AND both block heads (CW dup'd-head shape). Assessed
// s37, no compile attempt.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_00187DE0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
