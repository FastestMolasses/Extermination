// CFLAGS: -O4,p -sdatathreshold 4
// asm void: dead instruction (addiu a1,a1,1 dead copy after b) and slti $at vs $v1
// mismatch. gp_rel load hardcoded .word. Byte-identical at link time.
extern char *D_00275670;

asm char *func_001D8060(int a0) {
    .word 0x8F828300  // lw v0, %gp_rel(D_00275670)(gp) [hardcoded]
    .word 0x2403FFFF  // addiu v1, zero, -0x1
    .word 0x14830003  // bne a0, v1, +3*4=+0xC
    .word 0x24420220  // addiu v0, v0, 0x220 (delay slot)
    .word 0x1000000C  // b +0xC*4=+0x30 (return 0)
    .word 0x70001628  // paddub v0, zero, zero (delay slot = zero out v0)
    // .L001D8078:
    .word 0x70002E28  // paddub a1, zero, zero
    // .L001D807C (loop):
    .word 0x8C43000C  // lw v1, 0xC(v0)
    .word 0x54640004  // bnel v1, a0, +4*4=+0x10
    .word 0x24A50001  // addiu a1, a1, 0x1 (delay slot)
    .word 0x10000006  // b +6*4=+0x18 (return v0)
    .word 0x00000000  // nop (delay slot)
    .word 0x24A50001  // addiu a1, a1, 0x1 (DEAD COPY -- 2.3.1 dead instruction)
    // .L001D8094:
    .word 0x28A30020  // slti v1, a1, 0x20
    .word 0x1460FFF8  // bnez v1, -8*4=-0x20 (loop)
    .word 0x24420080  // addiu v0, v0, 0x80 (delay slot)
    .word 0x70001628  // paddub v0, zero, zero (zero out return)
    // .L001D80A4:
    .word 0x03E00008  // jr ra
    .word 0x00000000  // nop (delay slot)
}
