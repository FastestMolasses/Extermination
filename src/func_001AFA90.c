// INCLUDE_ASM func_001AFA90  (vram 0x001AFA90, 304 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001AFA90.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// BEST READABLE-C ATTEMPT (2026-06-10): 87.83%, wall-blocked. The whole
// pool-pop + init body (rows 20-70) matches byte-exactly, including the
// shared 0x3F800000 (1.0f) constant register, the -1/0x00010101 stores,
// and the class-2 D_00810701/02 lbu tail (extern unsigned char X[]
// declarations give the far lui-$at/%hi-%lo form). KEY STEERING: load
// `self = D_00275BC4;` as the FIRST statement, before computing
// c = (unsigned char)(cls & ~0xE0) — mwcc allocates saved regs by
// source-order web start, so this wins self->s0 / c->s1 (CW's choice);
// mwcc's scheduler then sinks the lw into the class-0xC bne's delay
// slot, so the early-emission cost is 1 row, not ~16. Residual rows are
// all documented walls: (a) prologue scheduling - CW puts the -0xE1
// materialization between addiu-sp and sq-ra and sinks `sq s0` into the
// bne delay slot, mwcc keeps saves contiguous and fills the delay with
// the lw; (b) delay-slot fill #13 on the reserve-check early-return
// (CW `beqz at; nop; b end; paddub v0=0` vs mwcc filling beqz's slot
// with the paddub and branching into epilogue+4 with `lq ra` in the b's
// slot); (c) the dead `b end; paddub v0,s0` + dead `paddub v0,zero`
// return merge (dead-dup wall - mwcc always coalesces; a goto-fail
// shape made it WORSE, 85.9%, by inverting the inner branch).
//
// Attempt source (compiles, logic verified against the .s):
//
// // Actor POOL ALLOC. Pops a 0x2F0-byte actor from the free list (head
// // D_00275BC4, count D_00275BC8) and initializes it: status +0x00 = 2,
// // class byte +0x02 (low 5 bits class, bits 5-7 flags), +0x14 canonical
// // self-pointer, scale +0x60..0x6C = 1.0 x4, +0x70/74 = 0, +0x78/7C = 1.0,
// // anim scale +0x80..0x8C = 1.0 x4, pos +0xB0..B8 = 0 / +0xBC = 1.0,
// // +0x94 = -1 h, +0x96 = 0 h, +0x99/9A/9C = 0, +0x30/52/54/56/58 = 0,
// // +0x5C = 0x00010101; links into the active list via func_001AFA50.
// // Class 0xC (transient FX) is refused when fewer than 10 actors remain
// // free (reserve). Class 2 additionally latches the area sub-state bytes
// // D_00810701/D_00810702 into +0x9D/+0x9E. Returns the actor or 0.
// extern void func_001AFA50(unsigned char *actor);  // active-list link
// extern unsigned char *D_00275BC4;   // free-list head
// extern short D_00275BC8;            // free count
// extern unsigned char D_00810701[];  // area sub-state
// extern unsigned char D_00810702[];
//
// unsigned char *func_001AFA90(int cls) {
//     unsigned char *self;
//     unsigned char c;
//
//     self = D_00275BC4;
//     c = (unsigned char)(cls & ~0xE0);
//     if (c == 0xC) {
//         if (D_00275BC8 < 10) {
//             return 0;
//         }
//     }
//     if (self != 0) {
//         D_00275BC8 -= 1;
//         D_00275BC4 = *(unsigned char **)(self + 0x1C);
//         *(unsigned char **)(self + 0x14) = self;
//         self[0x0] = 2;
//         self[0x2] = cls;
//         *(int *)(self + 0x60) = 0x3F800000;
//         *(int *)(self + 0x64) = 0x3F800000;
//         *(int *)(self + 0x68) = 0x3F800000;
//         *(int *)(self + 0x6C) = 0x3F800000;
//         *(int *)(self + 0x80) = 0x3F800000;
//         *(int *)(self + 0x84) = 0x3F800000;
//         *(int *)(self + 0x88) = 0x3F800000;
//         *(int *)(self + 0x8C) = 0x3F800000;
//         *(int *)(self + 0xB0) = 0;
//         *(int *)(self + 0xB4) = 0;
//         *(int *)(self + 0xB8) = 0;
//         *(int *)(self + 0xBC) = 0x3F800000;
//         *(int *)(self + 0x70) = 0;
//         *(int *)(self + 0x74) = 0;
//         *(int *)(self + 0x78) = 0x3F800000;
//         *(int *)(self + 0x7C) = 0x3F800000;
//         self[0x9A] = 0;
//         self[0x99] = 0;
//         *(short *)(self + 0x94) = -1;
//         *(short *)(self + 0x96) = 0;
//         func_001AFA50(self);
//         *(int *)(self + 0x30) = 0;
//         *(short *)(self + 0x54) = 0;
//         *(short *)(self + 0x56) = 0;
//         *(short *)(self + 0x52) = 0;
//         *(int *)(self + 0x58) = 0;
//         *(int *)(self + 0x5C) = 0x00010101;
//         self[0x9C] = 0;
//         if (c == 2) {
//             self[0x9D] = D_00810701[0];
//             self[0x9E] = D_00810702[0];
//         }
//         return self;
//     }
//     return 0;
// }
