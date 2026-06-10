// INCLUDE_ASM func_001549C0  (vram 0x001549C0, 1344 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (resolved s33 — docs/FINDINGS.md "KIND-0xE COMPANION
// RESOLVED"): ACTIVE tick of the kind-0xE tendril field. Sub-state
// +0x05: 0 SCAN (gate func_00154460 = player in 3x pad footprint;
// anchor = player XZ/pad Y; scatter 12 targets on ring 5.5+-2.0
// (pair idx 0) / 7.0+-2.5 (idx 1) around the player, validity =
// func_001545B0 0.92x pad ellipse; phase rand 48..127, girth from
// D_0026D320[(seed++)&3]; any valid -> sound 0x42D range 300);
// 1 DEPLOY ramp +37/tick cap 300 for 8 ticks; 2 HOLD until player
// leaves the anchor (distXZ^2 >= 4 idx0 / 16 idx1) or the Y band
// (3+recY) or a non-gameplay frame; 3 RETRACT -37/tick, 8 ticks;
// 4 -> sub 0 rescan. Tail: blend RGB +0x80..88 toward (6,92,1)/128
// green by parent pad phase (+0x20 -> parent +0x80), func_001C6380,
// sub!=0 -> renderer func_00154F00. Never reads HP/mailbox.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001549C0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
