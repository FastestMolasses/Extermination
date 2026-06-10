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
// WALL-BLOCKED at 89.33% (2026-06-10, 4 attempts; ~140/157 rows
// byte-identical from the C below). What MATCHED: the whole tint-scan
// loop incl. the bnel likely-loop with the idx increment in its slot,
// all four u32->float conversion dances (live rows), the five 128.0f
// divides, gp-rel D_00275BCC/&D_00275450 (-sdatathreshold 4 with
// array-typed externs keeping the absolute globals absolute), the
// 0x3F800000 integer stores, the split-register pair for D_00246800,
// idiom-7 fake trailing params PIN rec->a2 and idx->a3 (works because
// their defs are an address materialization / constant, NOT memory
// loads — extends the pool-order datum), and the func_00179B90-style
// `t = idx + 1; idx = t; t = t < 0x16` kill-rename lands
// `sltiu v1` + `bnez v1` (CW's registers) for the loop guard.
// Residual ~17 rows, all catalogued walls:
// (a) dead-`b`-dup x5: the dead `paddub s2,zero,zero` (i=0 dup under
//     the bail beqz) and a dead `srl a0,v1,1` per conversion dance
//     (bltz-slot dup); mwcc coalesces all five.
// (b) wall #13 fills: mwcc sinks `sb v0,9(s1)` into the slot-loop
//     bnez slot and `addiu s0,0xA` into the clear-loop bnez slot
//     (loop-invariant store / always-executed advance are "safe");
//     CW leaves nops (4 nop rows).
// (c) temp-register POOL ORDER: slot-loop guard lbu/slt land v0/$at
//     (assign-back gives v0/v1) vs CW's v1/v0; tint header hi/lo land
//     a0/v1 vs CW's a1/a0 (sll/addu operands follow); clear-loop
//     counter lands v1 (or s1 when parked via `self = 0`) vs CW's a0
//     — no a0-homed dead variable exists (self is s1-homed here).
// (d) prologue interleave: CW puts `li a2,0xA` before the jal with
//     `paddub s1,a0` in the slot; mwcc saves s1 first and puts the li
//     in the slot (12c arg statement does not move it).
//
// extern int D_0028A4E4[]; extern short D_00275BCC;
// extern int D_00275450;
// extern unsigned char D_00810700[], D_00810701[], D_00246800[];
// /* fake trailing params rec/idx pin a2/a3 (idiom 7); callers pass
//    only (self, scr) */
// void func_00154740(unsigned char *self, unsigned char *scr,
//                    unsigned char *rec, unsigned int idx) {
//     int i; unsigned char *cur;
//     func_001CA5E0(self, D_0028A4E4[0], 0xA);
//     self[0xC] = func_001C6150(*(int *)(self + 0x44));
//     i = 0;
//     if (D_00275BCC < self[0xC]) { self[4] = 3; return; }
//     cur = self;
//     while (i < self[0xC]) {
//         *(int *)(cur + 0x110) = func_001AF780();
//         cur += 4; i += 1;
//     }
//     self[9] = self[0xC];
//     anim_bone_array_setup(self[0xC]);
//     bone_init_default_1(self);
//     self[4] = 1; self[0] = 2;
//     *(int **)(self + 0x30) = &D_00275450;
//     *(int *)(self + 0x80) = 0x3F800000;
//     *(int *)(self + 0x84) = 0x3F800000;
//     *(int *)(self + 0x88) = 0x3F800000;
//     {
//         unsigned int hi, key; int t;
//         hi = D_00810700[0];
//         rec = D_00246800;
//         idx = 0;
//         key = (hi << 8) + D_00810701[0];
//         for (;;) {
//             if (*(unsigned int *)rec == key) {
//                 *(float *)(scr + 0x0) = (float)(unsigned int)rec[4];
//                 *(float *)(scr + 0x4) = (float)(unsigned int)rec[5];
//                 *(float *)(scr + 0x8) = (float)(unsigned int)rec[6];
//                 *(float *)(scr + 0xC) = (float)(unsigned int)rec[7] / 128.0f;
//                 *(float *)(self + 0x80) = *(float *)(scr + 0x0) / 128.0f;
//                 *(float *)(self + 0x84) = *(float *)(scr + 0x4) / 128.0f;
//                 *(float *)(self + 0x88) = *(float *)(scr + 0x8) / 128.0f;
//                 *(float *)(self + 0x8C) = *(float *)(scr + 0xC);
//                 break;
//             }
//             t = idx + 1; idx = t;
//             t = (unsigned int)t < 0x16;
//             rec += 8;
//             if (!t) break;
//         }
//     }
//     *(int *)(self + 0x38) = 0x3F800000;
//     {
//         int t2;
//         self = (unsigned char *)0;
//         do {
//             t2 = (int)self + 1; self = (unsigned char *)t2;
//             *(short *)(scr + 0x84) = 0;
//             t2 = t2 < 0xC;
//             scr += 0xA;
//         } while (t2);
//     }
// }
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_00154740.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
