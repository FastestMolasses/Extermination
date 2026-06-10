// INCLUDE_ASM func_00154740  (vram 0x00154740, 628 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (resolved s33 — docs/FINDINGS.md "KIND-0xE COMPANION
// RESOLVED"): INIT of the kind-0xE tendril field (state 0 of
// func_001546C0). Binds the spike mesh func_001CA5E0(self,
// *D_0028A4E4 = D_0028A490[slot 0x15] = chunk03/f13_id15.bin, anim
// mode 0xA -> +0x4C = func_001CB1F0); bone-count/matrix-slot
// boilerplate (func_001C6150 -> +0xC, func_001AF780 -> +0x110,
// pool-short D_00275BCC -> state 3 bail, anim_bone_array_setup,
// bone_init_default_1); state=1, +0x00=2, +0x30=&D_00275450;
// RGB mult +0x80/84/88 = 1.0 defaults then per-room tint from
// D_00246800 (22 x 8B {key AREA<<8|ROOM, u8 c0..c3}): scratch+0x1F0
// +0..8 = c0..c2 raw floats, +0xC and actor +0x8C = c3/128, actor
// +0x80..88 = c0..c2/128; +0x38 = 1.0f; clears the 12 record valid
// flags (+0x1F0+0x84 + i*0xA).
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_00154740.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
