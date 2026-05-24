// CFLAGS: -O4,p -sdatathreshold 0
// asm void: beqz delay slot has nop (pure C omits it); lui/addiu hardcoded .word.
// Byte-identical at link time.
extern unsigned char D_008106C7;

asm void func_0016F5D0(char *a0) {
    .word 0x24030002  // addiu v1, zero, 0x2
    .word 0xA08301F1  // sb v1, 0x1F1(a0)
    .word 0xA0830318  // sb v1, 0x318(a0)
    .word 0xA08002F2  // sb zero, 0x2F2(a0)
    .word 0x3C010081  // lui at, %hi(D_008106C7) [hardcoded]
    .word 0x902306C7  // lbu v1, %lo(D_008106C7)(at) [hardcoded]
    .word 0x10600003  // beqz v1, .L0016F5F8 (+3*4=+0xC)
    .word 0x00000000  // nop (delay slot)
    .word 0x3C010081  // lui at, %hi(D_008106C7) [hardcoded]
    .word 0xA02006C7  // sb zero, %lo(D_008106C7)(at) [hardcoded]
    // .L0016F5F8:
    .word 0x03E00008  // jr ra
    .word 0x00000000  // nop (delay slot)
}
