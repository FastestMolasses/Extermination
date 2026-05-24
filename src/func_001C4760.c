// CFLAGS: -O4,p -sdatathreshold 0
// asm void: slti uses v0 (not at); paddub v0,zero,zero return zeroing in delay slot.
// lui/addiu hardcoded .word. Byte-identical at link time.
extern unsigned char D_00810CC3[];
extern volatile char D_008106B0;
extern volatile char D_008106B1;

asm int func_001C4760(int a0, int a1) {
    .word 0x3C020081  // lui v0, %hi(D_00810CC3) [hardcoded]
    .word 0x24420CC3  // addiu v0, v0, %lo(D_00810CC3) [hardcoded]
    .word 0x00443021  // addu a2, v0, a0
    .word 0x90C30000  // lbu v1, 0x0(a2)
    .word 0x28820020  // slti v0, a0, 0x20 (uses v0, not at)
    .word 0x00651821  // addu v1, v1, a1
    .word 0x14400006  // bnez v0, .L001C4794 (+6*4=+0x18)
    .word 0xA0C30000  // sb v1, 0x0(a2) (delay slot)
    .word 0x24020003  // addiu v0, zero, 0x3
    .word 0x3C010081  // lui at, %hi(D_008106B0) [hardcoded]
    .word 0xA02206B0  // sb v0, %lo(D_008106B0)(at) [hardcoded]
    .word 0x3C010081  // lui at, %hi(D_008106B1) [hardcoded]
    .word 0xA02406B1  // sb a0, %lo(D_008106B1)(at) [hardcoded]
    // .L001C4794:
    .word 0x03E00008  // jr ra
    .word 0x70001628  // paddub v0, zero, zero (delay slot -- zeros return value)
}
