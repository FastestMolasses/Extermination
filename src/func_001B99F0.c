// INCLUDE_ASM func_001B99F0  (vram 0x001B99F0, 428 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// ---------------------------------------------------------------------------
// 2026-06-10 attempt: WALL-BLOCKED at ~93.5% genuine rows (2 attempts).
// This unit is TWO source functions (door-script opcode handlers, see
// docs/FINDINGS.md "DOOR SCRIPTS DECODED"):
//   func_001B99F0 (op 0x09 CALL NATIVE, 3 instrs): tail-jump to rec[+0x4].
//     NEW DATUM (idiom 19): mwcc 2.3 DOES emit the MIPS sibling-call shape
//     `lw v0,0x4(a2); jr v0; nop` BYTE-IDENTICAL for
//     `return ((int(*)(...))rec[1])(actor, blk, rec);` — a tail call whose
//     args pass through unchanged. Tail-call functions are matchable.
//   func_001B9A00 (op 0x0A PLAYER ANIM, ftab entry = func_001B99F0+0x10 —
//     the "mid-function entry"): dense switch on rec[+0x8], 9 cases via
//     jtbl_0026E0B0. The jtbl case-0 entry lands MID-BLOCK in case 4's
//     body — reproduced naturally by C `case 4: ... /*fallthrough*/ case 0:`.
//     mwcc emits each function in its own .text section; layout/padding to
//     the +0x10 entry happens at link.
// Residual walls (best shape preserved below):
//  (a) DENSE-SWITCH DISPATCH ORDER (2nd confirmation of the func_001BA080
//      datum): mwcc [lui jtbl, sll idx, addiu %lo, addu]; CW [lui, addiu,
//      sll, addu]. Compiler-internal. Also means the compiled object would
//      carry its OWN local jump table — linking it would break the ELF's
//      data region even if .text matched, so this stays a stub.
//  (b) prologue swap: CW [addiu s0,v1,%lo(player); sltiu; beqz; paddub
//      s1,a2 (slot)]; mwcc [paddub s1,a2; sltiu; beqz; addiu s0 (slot)] —
//      delay-slot fill choice, not C-addressable (explicit `r = rec` copy
//      after the player statement does not move it).
//  (c) case 2: CW materializes `addiu v0,-1` BEFORE `sh zero,0x1F2`; mwcc
//      sinks it next to its store (idiom-12b const-between-statements did
//      NOT take here).
//  (d) case 5: call-arg setup order `addiu a1` before `paddub a0` (CW) vs
//      a0-first (mwcc); idiom-12c arg-as-own-statement did not flip it.
// POSITIVE datum: case 6 (camera-angle reset, 2 loads + 3 stores on five
// DISTINCT globals D_00810350/354/358/360/368) matched ONLY after declaring
// all five `volatile` — pinning CW's sequential load->store order and the
// f0 register reuse. This refines the func_001AAD00 negative datum: for a
// SHORT mixed block, volatile across distinct objects DOES hold program
// order in mwcc 2.3.
// Also: split-register reloc pairs (`lui v0; addiu s0,v0,%lo`) ARE emitted
// by mwcc when the destination is a saved register or a second live temp
// (prologue + cases 4/1 all matched) — the same-register-pair divergence
// seen in func_001BC150 is specific to short-lived temp webs.
//
// Best attempt (~14 diff rows of 108: walls (a-d) + 2 layout-artifact nops):
//
// extern unsigned char D_008102B0[];
// extern void *D_0028A490[];
// extern void *D_0028A580;
// extern volatile float D_00810350;
// extern volatile int D_00810354;
// extern volatile float D_00810358;
// extern volatile float D_00810360;
// extern volatile float D_00810368;
// extern void func_00102948(void *dst, void *src);
// extern void func_00182F90(void *player, void *pt);
// extern void func_001798D0(void *player);
//
// int func_001B99F0(void *actor, void *blk, unsigned int *rec) {
//     return ((int (*)(void *, void *, unsigned int *))rec[1])(actor, blk, rec);
// }
//
// int func_001B9A00(void *actor, void *blk, unsigned char *rec) {
//     unsigned char *player;
//
//     player = D_008102B0;
//     switch (*(int *)(rec + 0x8)) {
//     case 4:                       /* bind container + mode 3, fall into 0 */
//         *(void **)(player + 0x40) = D_0028A490[*(int *)(rec + 0x1C)];
//         player[0x2F3] = 3;
//     case 0:                       /* anim id + rate, +0x1F4 = 1.0f */
//         *(short *)(player + 0x1F2) = *(short *)(rec + 0x14);
//         *(float *)(player + 0x1F8) = *(float *)(rec + 0xC);
//         *(int *)(player + 0x1F4) = 0x3F800000;
//         break;
//     case 1:                       /* container + id + mode 1 */
//         *(void **)(player + 0x40) = D_0028A490[*(int *)(rec + 0x1C)];
//         *(short *)(player + 0x1F2) = *(short *)(rec + 0x14);
//         player[0x2F3] = 1;
//         *(float *)(player + 0x1F4) = *(float *)(rec + 0xC);
//         *(int *)(player + 0x200) = 0;
//         break;
//     case 2:                       /* RESET: default container */
//         *(short *)(player + 0x1F2) = 0;
//         *(short *)(player + 0x20C) = -1;
//         player[0x2F3] = 3;
//         *(void **)(player + 0x40) = D_0028A580;
//         break;
//     case 3:                       /* wait player anim done */
//         return (*(int *)(player + 0x200) & 0x1000) ? 1 : 0;
//     case 5:                       /* move-to rec[+0x30], rec[+0x34] -= 11 */
//         func_00102948(rec + 0x30, *(unsigned char **)(player + 0x114) + 0xC0);
//         *(float *)(rec + 0x34) -= 11.0f;
//         func_00182F90(player, rec + 0x30);
//         return 1;
//     case 6:                       /* camera angle reset */
//         D_00810350 = D_00810360;
//         D_00810354 = 0;
//         D_00810358 = D_00810368;
//         break;
//     case 7:                       /* id/rate/+0x1F4 from rec */
//         *(short *)(player + 0x1F2) = *(short *)(rec + 0x14);
//         *(float *)(player + 0x1F8) = *(float *)(rec + 0xC);
//         *(float *)(player + 0x1F4) = *(float *)(rec + 0x10);
//         break;
//     case 8:                       /* player re-init */
//         func_001798D0(D_008102B0);
//         break;
//     }
//     return 1;
// }
// ---------------------------------------------------------------------------
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001B99F0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
