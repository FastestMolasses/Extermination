// Per-frame CATEGORY-LIST PUBLISH: pushes the actor onto its per-class
// pointer list (see func_001AAD00 for the swap) and, if class-flag 0x80
// (interactive) is set, also onto the interactive list via func_001B1DE0.
// Dispatch on class (low 5 bits of byte +0x02): 1 -> func_001B1C60,
// 2/0xA -> func_001B1CA0, 4 -> func_001B1D20, 7 -> func_001B1D60,
// 0xD -> func_001B1DA0.
//
// READABLE-C ATTEMPT (2026-06-10): 91.53% — NEW IDIOM discovered: a C
// `switch` makes mwcc reproduce CW's beql-compare-chain switch lowering
// (beql per case with the arg paddub in the likely slot, shared-label
// pair as beql+beq to an out-of-line paddub stub, out-of-line case
// blocks), with the COMPARE CHAIN in REVERSE source-case order and the
// CASE BLOCKS in source-case order. Source case order 1, [0xA,2], 4, 7,
// 0xD reproduced the whole structure. An if-else chain does NOT (mwcc
// inlines the calls behind bne-skips, 49.6%). Residual 5 rows are the
// two documented walls: (a) dead `paddub a0,s0` block-head dups (each
// beql already dups the paddub into its likely slot; CW keeps the dead
// block-head copy, mwcc coalesces — rows before the case-1/4/7/D jals);
// (b) the case-0xA beq's delay slot: CW leaves a nop (can't put paddub
// there: it would clobber a0=c on fall-through), mwcc fills it with the
// next compare's safe `addiu v1,zero,1` (delay-slot-fill wall #13).
// Attempt source:
//
//   void func_001B1B70(unsigned char *self) {
//       int c;
//       if (self[0x2] & 0x80) func_001B1DE0(self);
//       c = self[0x2] & ~0xE0;
//       switch (c) {
//       case 1:            func_001B1C60(self); break;
//       case 0xA: case 2:  func_001B1CA0(self); break;
//       case 4:            func_001B1D20(self); break;
//       case 7:            func_001B1D60(self); break;
//       case 0xD:          func_001B1DA0(self); break;
//       }
//   }
//
// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001B1C60(int, int, int, int);
extern void func_001B1CA0(int, int, int, int);
extern void func_001B1D20(int, int, int, int);
extern void func_001B1D60(int, int, int, int);
extern void func_001B1DA0(int, int, int, int);
extern void func_001B1DE0(int, int, int, int);

asm void func_001B1B70(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x90830002
    andi       $v1, $v1, 0x80
    .word 0x10600003
    .word 0x70808628
    jal        func_001B1DE0
    nop
    .word 0x92050002
    addiu      $a0, $zero, -0xE1
    addiu      $v1, $zero, 0xD
    and        $a0, $a1, $a0
    .word 0x50830027
    .word 0x72002628
    addiu      $v1, $zero, 0x7
    .word 0x5083001f
    .word 0x72002628
    addiu      $v1, $zero, 0x4
    .word 0x50830017
    .word 0x72002628
    addiu      $v1, $zero, 0x2
    .word 0x5083000f
    .word 0x72002628
    addiu      $v1, $zero, 0xA
    .word 0x1083000b
    nop
    addiu      $v1, $zero, 0x1
    .word 0x10830004
    .word 0x72002628
    .word 0x10000019
    .word 0x7bbf0010
    .word 0x72002628
    jal        func_001B1C60
    nop
    .word 0x10000013
    nop
    .word 0x72002628
    jal        func_001B1CA0
    nop
    .word 0x1000000e
    nop
    .word 0x72002628
    jal        func_001B1D20
    nop
    .word 0x10000009
    nop
    .word 0x72002628
    jal        func_001B1D60
    nop
    .word 0x10000004
    nop
    .word 0x72002628
    jal        func_001B1DA0
    nop
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
