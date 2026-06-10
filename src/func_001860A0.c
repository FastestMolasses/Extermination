// INCLUDE_ASM func_001860A0  (vram 0x001860A0, 288 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (resolved — docs/FINDINGS.md "WEAPON SYSTEM" §7): world-hit
// surface-impact resolver on the bullet path. func_001860A0(a, b) tail-
// passes its args to the ray query func_0019B6C0(a, b); on miss returns
// 0. On hit: copies the spad hit point into the FX position
// func_001031E0(&D_70003620, &D_700031B0), builds the FX normal vec4 at
// 0x70003630..3C from hit-record (*0x700031D0) +0x24/+0x28/+0x2C with
// w = 1.0f, then keys the impact on the record's surface-type byte
// +0x1A: 0x5A -> FX func_001EFD90(0x8000002C, pos, normal); 0x5B -> FX
// 0x80000026 + flash func_001E8B90(&pos, 1.0f in $f12); 0x5C -> FX
// 0x80000067; returns 1. (NOTE: FINDINGS §7 says "tracer" — this is
// actually the surface-FX leg; the surface codes here are 0x5A/0x5B/0x5C,
// vs the 2-4/5/8 decimal codes quoted in §7's world-hit summary.)
//
// WALL — recognized from the disassembly, NO attempt burned
// (2026-06-10): the body contains THREE instances of the confirmed
// "dead `b`; <dup-instr>" CW 2.3.1 artifact that mwcc 2.3 always
// coalesces (PROGRESS "CONFIRMED COMPILER WALLS"): the dead
// `addiu v0,zero,0x5B` at 0x186138 and `addiu v0,zero,0x5C` at
// 0x18617C (dup'd compare-chain constants below `b .L001861A8` /
// `b .L001861A4`), and the dead `paddub v0,zero,zero` at 0x1861B0
// (dup of the miss-path return-0 below `b .L001861B8`). The bne
// compare chain itself (consts staged in the delay slots) is the
// idiom-14-adjacent shape and likely reproducible via switch, but the
// three dead dups cap any attempt at ~96% — do not retry from C.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001860A0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
