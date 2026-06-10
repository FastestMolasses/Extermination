// INCLUDE_ASM func_001AFC10  (vram 0x001AFC10, 140 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001AFC10.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// BEST READABLE-C ATTEMPT (2026-06-10): 92.29%, wall-blocked. Rows 0-25
// (calls, free-list push via gp-rel D_00275BC4/BC8, header clears)
// match byte-exactly. The residual is the tail-zeroing loop, blocked on
// TWO documented walls at once: (a) slt-into-branch regalloc - CW
// `slti $v1, $a1, 0x40; bnez $v1` vs mwcc's `$at` (confirmed-genuine
// wall; an explicit `cond = i < 0x40;` local changes nothing); (b)
// branch-delay-slot fill #13 - CW leaves `addiu $a2,4; nop; bnez; nop`
// while mwcc sinks the p-advance into the bnez delay slot (it is a safe
// fall-through candidate, so the fill cannot be forbidden from C).
// do-while and goto-shaped loop layouts produce identical code.
//
// Attempt source (compiles, logic verified against the .s):
//
// // Actor FREE (despawn back to pool). Takes the canonical pointer from
// // +0x14 (clearing the caller's copy), releases the bone slots
// // (func_001AF800) and unlinks from the active doubly-linked list
// // (func_001AFBC0), then pushes the actor onto the free singly-linked
// // list (head D_00275BC4, next +0x1C, count D_00275BC8), clears the
// // header words +0x00/04/08/0C, +0x36 h, +0x98 b, +0x90 w, and zeroes
// // the 0x100-byte per-behavior scratch tail +0x1F0..+0x2F0.
// extern void func_001AF800(unsigned char *actor);  // bone-slot release
// extern void func_001AFBC0(unsigned char *actor);  // active-list unlink
// extern unsigned char *D_00275BC4;  // free-list head
// extern short D_00275BC8;           // free count
//
// void func_001AFC10(unsigned char *handle) {
//     unsigned char *self;
//     int i;
//     int *p;
//
//     self = *(unsigned char **)(handle + 0x14);
//     *(int *)(handle + 0x14) = 0;
//     func_001AF800(self);
//     func_001AFBC0(self);
//     D_00275BC8 += 1;
//     *(unsigned char **)(self + 0x1C) = D_00275BC4;
//     D_00275BC4 = self;
//     *(int *)(self + 0x0) = 0;
//     *(int *)(self + 0x4) = 0;
//     *(int *)(self + 0x8) = 0;
//     *(int *)(self + 0xC) = 0;
//     *(short *)(self + 0x36) = 0;
//     self[0x98] = 0;
//     *(int *)(self + 0x90) = 0;
//     p = (int *)(self + 0x1F0);
//     i = 0;
//     do {
//         i += 1;
//         *p = 0;
//         p += 1;
//     } while (i < 0x40);
// }
