// CFLAGS: -O4,p -sdatathreshold 4
// asm void: 2.3.1 dead instruction (addiu v0,a2,0x1 after b+addiu delay) not emitted by
// pure C. gp_rel and hi/lo hardcoded .word. Byte-identical at link time.
extern signed char D_00275B30;
extern int D_00281CF0[0x40];

asm int func_001FA5A0(int a0) {
    .word 0x838687C0  // lb a2, %gp_rel(D_00275B30)(gp) [hardcoded]
    .word 0x3C020028  // lui v0, %hi(D_00281CF0) [hardcoded]
    .word 0x24431CF0  // addiu v1, v0, %lo(D_00281CF0) [hardcoded]
    .word 0x2402FFFF  // addiu v0, zero, -0x1
    .word 0x00062880  // sll a1, a2, 2
    .word 0x00652821  // addu a1, v1, a1
    .word 0x8CA30000  // lw v1, 0x0(a1)
    .word 0x10620004  // beq v1, v0, .L001FA5D0 (+4*4=+0x10)
    .word 0x24C20001  // addiu v0, a2, 0x1 (delay slot)
    .word 0x10000006  // b .L001FA5E0 (+6*4=+0x18)
    .word 0x24020001  // addiu v0, zero, 0x1 (delay slot)
    .word 0x24C20001  // addiu v0, a2, 0x1 (DEAD -- 2.3.1 dead instruction)
    // .L001FA5D0:
    .word 0xACA40000  // sw a0, 0x0(a1)
    .word 0x3042000F  // andi v0, v0, 0xF
    .word 0xA38287C0  // sb v0, %gp_rel(D_00275B30)(gp) [hardcoded]
    .word 0x24020001  // addiu v0, zero, 0x1
    // .L001FA5E0:
    .word 0x03E00008  // jr ra
    .word 0x00000000  // nop (delay slot)
}
