// CFLAGS: -O4,p -sdatathreshold 0
// asm int: lui/addiu %hi/%lo hardcoded .word; lh in jr delay slot.
// Byte-identical at link time.
extern short *D_00248AB0[];

asm int func_0017B460(int a0, int a1) {
    .word 0x3C020024  // lui v0, %hi(D_00248AB0) [hardcoded]
    .word 0x24428AB0  // addiu v0, v0, %lo(D_00248AB0) [hardcoded]
    .word 0x00042080  // sll a0, a0, 2   [a0 *= 4]
    .word 0x00441021  // addu v0, v0, a0 [v0 = &D_00248AB0[a0]]
    .word 0x8C420000  // lw v0, 0(v0)    [v0 = D_00248AB0[a0]]
    .word 0x00051840  // sll v1, a1, 1   [v1 = a1*2]
    .word 0x00431021  // addu v0, v0, v1 [v0 += a1*2]
    .word 0x03E00008  // jr ra
    .word 0x84420000  // lh v0, 0(v0)    [delay: v0 = *(short*)v0]
}
