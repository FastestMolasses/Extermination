// INCLUDE_ASM func_001B41F0  (vram 0x001B41F0, 1564 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// SEMANTICS (resolved — docs/FINDINGS.md "WEAPON SYSTEM" §7 / "ENEMY AI
// ARCHITECTURE" §5): the HIT APPLICATION —
// func_001B41F0(victim, hitPos, dir, flags, amount, type). Dispatches on
// victim model byte +0x03 via jtbl_0026DDF0 (bound sltiu 0x14; out of
// range -> FX id 0x80000007). Case families: pick the per-model
// blood/impact FX id into s0 (0x80000024/25/34/35/...); some models get
// special handling (model with +0x0D==3 and player-yaw D_00810354 < 47.0
// just deflects: func_00102948(victim+0x70, hitPos), return 0; one case
// builds a reflected spray from D_00810350 via func_001028D0 +
// func_00102760 normalize + FX 0x80000026 at spad D_70003600/10 and
// returns 1 without a mailbox write). Common legs: write the damage
// mailbox `victim+0x36 = flags ? (type | amount*5 | 0x8000) : (type |
// amount)`, ping +0x00 = 3, copy hit dir -> victim+0x70
// (func_001031E0), then either the armored leg (FX 0x80000076 + sound
// 0x15D when flags) or FX s0 + ricochet sound 0x15A/0x15B picked by
// rand bit 13 (func_00122BB8 >> 13 & 1) via func_001FC580; the wide leg
// also builds the deflection vec at D_70003600 from yaw+pi
// (func_001B1470 sin) and a rand-byte spray pair written to
// 0x70003680/84/88 (three func_00122BB8 draws, sra 17/13/19 & 0xFF
// / 255.0). Returns 1 on applied hit, 0 on deflect.
//
// WALL — recognized from the disassembly, NO attempt burned
// (2026-06-10): three independent confirmed-wall families saturate it:
//  1. dead `b`; <dup-instr> x3 (mwcc always coalesces): dead
//     `or v0,s1,s2` at 0x1B43A0, dead `addiu a1,zero,0x15B` at
//     0x1B459C and 0x1B47D4 (dup'd sound ids below `b` merges).
//  2. wall #13: `bc1f .L001B42AC; nop` at 0x1B4290 with a safe
//     fall-through candidate (`addiu a0,s6,0x70`), plus further nop'd
//     beqz slots before safe addiu/lui candidates.
//  3. saved-register allocation ORDER: CW maps params a0..t1 ->
//     s6,s5,s4,s3,s2,s1 (reverse) with s0 reserved for the
//     late-initialized FX-id local; mwcc allocates by web start
//     (emission order is a2->s4 first), so the documented
//     either-registers-or-placement dilemma applies to SEVEN webs at
//     once. The giant duplicated spray block (0x1B43B0 vs 0x1B45BC,
//     ~80 rows, duplicated in source) is plain C, and the jtbl switch
//     shape itself is fine — but 1+2+3 cap any attempt well below
//     match; do not retry from C until those walls crack.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001B41F0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
