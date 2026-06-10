// INCLUDE_ASM func_001872C0  (vram 0x001872C0, 132 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001872C0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// SEMANTICS (2026-06-10 weapon-system pass): grenade-launcher fire —
// sub-weapon 4 fire-event arm of the weapon-actor dispatcher
// func_00188630. Spawns a class-1 actor (func_001AFA90(1)): model byte
// +0x03 = 3, behavior +0x10 = func_0018B3E0 (ballistic grenade
// projectile), param +0x0D = D_0081070B (area sub-byte), position
// +0xB0..B8 from the gun actor's launch point (+0xB0), initial VELOCITY
// +0x70..78 from the gun's aim direction vector (+0xC0). Returns 0.
//
// MATCH ATTEMPT 2026-06-10, wall #13 (delay-slot fill): the readable C
//   a = func_001AFA90(1); if (a) { a[3] = 3; ... }
// reaches 93.6% — the sole residual is mwcc filling the `beqz v0`
// delay slot with the safe `li v1,3` (CW 2.3.1 leaves a nop, filling
// only from the taken path). Every fall-through candidate here is a
// hoistable constant materialization, so the nop is unreachable from C.
