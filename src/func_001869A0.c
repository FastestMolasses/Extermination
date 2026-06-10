// INCLUDE_ASM func_001869A0  (vram 0x001869A0, 180 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001869A0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// SEMANTICS (2026-06-10 weapon-system pass): missile-launcher fire —
// sub-weapon 1 fire-event arm of the weapon-actor dispatcher
// func_00188630. Spawns a class-1 actor (func_001AFA90(1)): model byte
// +0x03 = 3, behavior +0x10 = func_0018AF50 (guided projectile:
// per-frame steer + segment queries), position +0xB0..B8 and direction
// +0xC0..C8 copied from the gun actor; +0x24 = locked target
// (D_008106E0) when the player context D_008104E0 == 0xC and aim option
// D_00810CA4 == 1, else NULL (unguided). Returns 0.
//
// MATCH ATTEMPT 2026-06-10, wall #13 (delay-slot fill): readable C
// reaches 90.4% — residuals are mwcc filling the `beqz v0` slot with
// the safe `li v1,3` (CW leaves a nop) plus the downstream one-slot
// shift. Same wall as func_001872C0; analysis there.
