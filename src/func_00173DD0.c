// INCLUDE_ASM func_00173DD0  (vram 0x00173DD0, 140 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (FINDINGS "KNIFE/MELEE DECODED"): heavy-stab in-swing yaw
// steer (knife SQUARE attack, mode 0x22 states 2/3): while the stab
// swings, steer the player yaw +0xC4 toward the goal heading +0x218 at
// pi * D_002486F0[gait] / 180 rad/tick (D_002486F0 = {0, 0.5, 1.0,
// 2.0}f rate table indexed by gait byte +0x23F; the table is copied to
// the stack as one lq/sq quadword first), gated on
// func_00174AC0(self, 2); the steer helper is func_001B12B0(goal,
// cur, rate) -> new yaw.
//
// WALL-BLOCKED at 74.51% (2026-06-10, 2 attempts + 2 form probes; the
// whole if-body 0x30..0x58 and the branch/epilogue rows match
// byte-identically — the % is row-shift dominated). Residuals are two
// catalogued scheduler walls:
// (a) reloc-pair interleave: CW materializes &D_002486F0 as a 3-instr
//     chain `lui v0 %hi / [sq ra; sq s0 interleaved] / addiu v0 %lo;
//     lq v0,0(v0)` with the dst addr `addiu v1,sp,0x20` AFTER the lq;
//     mwcc folds the %lo into the load (`lui at; lq v1,0(at)`) for a
//     single-use address in every form tried (bare extern, pointer
//     statement idiom 12a, struct-field access, idiom-7 fake-param
//     value) and never interleaves a pair (documented: "mwcc treats
//     the reloc pair atomically"; the split-register pair needs a
//     saved-reg/second-live-temp destination, pool-order datum).
// (b) FP-stall scheduling before the call: CW emits `div.s f14; nop;
//     nop; jal func_001B12B0; nop` (explicit div-latency padding);
//     mwcc sinks the div.s into the jal delay slot (3 rows).
// Matched along the way: beqz delay-slot nop SURVIVES (mwcc did not
// fill from the safe lui fall-through here — counter-datum to wall
// #13's "always fills when a safe candidate exists"), pi/180.0f via
// plain float literals (lui/ori + lui forms), `mul.s f1,f2,f1` from
// pi-first source order (idiom 2), rates[] on-stack indexing
// (sll/addu sp/lwc1 0x20(v0)) byte-exact.
//
// void func_00173DD0(unsigned char *self) {
//     float rates[4];
//     *(uint128 *)rates = D_002486F0;
//     if (func_00174AC0(self, 2)) {
//         *(float *)(self + 0xC4) = func_001B12B0(
//             *(float *)(self + 0x218), *(float *)(self + 0xC4),
//             (3.1415927410125732f * rates[self[0x23F]]) / 180.0f);
//     }
// }
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_00173DD0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
