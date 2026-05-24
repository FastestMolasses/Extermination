// CFLAGS: -O4,p -sdatathreshold 0
// asm void: 2.3.1 dead instruction (addiu v0,zero,0x1 after b+paddub delay slot); beql
// delay slot has addiu v0,zero,0x1. lui/addiu hardcoded. Byte-identical at link time.
extern short D_00810CAC;
extern short D_00810CAE;

asm int func_0017B420(void) {
    .word 0x3C010081  // lui at, %hi(D_00810CAC) [hardcoded]
    .word 0x84220CAC  // lh v0, %lo(D_00810CAC)(at) [hardcoded]
    .word 0x5040000A  // beql v0, zero, .L0017B454 (+0xA*4=+0x28)
    .word 0x24020001  // addiu v0, zero, 0x1 (delay slot)
    .word 0x2442FFFF  // addiu v0, v0, -0x1
    .word 0x3C010081  // lui at, %hi(D_00810CAC) [hardcoded]
    .word 0xA4220CAC  // sh v0, %lo(D_00810CAC)(at) [hardcoded]
    .word 0x24020064  // addiu v0, zero, 0x64
    .word 0x3C010081  // lui at, %hi(D_00810CAE) [hardcoded]
    .word 0xA4220CAE  // sh v0, %lo(D_00810CAE)(at) [hardcoded]
    .word 0x10000002  // b .L0017B454 (+2*4=+0x8)
    .word 0x70001628  // paddub v0, zero, zero (delay slot)
    .word 0x24020001  // addiu v0, zero, 0x1 (DEAD -- 2.3.1 dead instruction)
    // .L0017B454:
    .word 0x03E00008  // jr ra
    .word 0x00000000  // nop (delay slot)
}
