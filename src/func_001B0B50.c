// CFLAGS: -O4,p -sdatathreshold 0
// asm void: 2.3.1 dead instruction (andi v1,a0,2 after b+sb delay) and beqz delay slot
// pre-hoisted for .L001B0B78 path. lui/addiu hardcoded .word. Byte-identical at link time.
extern int D_008106C8;
extern unsigned char D_008106BE;

asm void func_001B0B50(void) {
    .word 0x3C010081  // lui at, %hi(D_008106C8) [hardcoded]
    .word 0x8C2406C8  // lw a0, %lo(D_008106C8)(at) [hardcoded]
    .word 0x30830001  // andi v1, a0, 0x1
    .word 0x10600006  // beqz v1, .L001B0B78 (+6*4=+0x18)
    .word 0x30830002  // andi v1, a0, 0x2 (delay -- pre-hoisted for .L001B0B78 path)
    .word 0x24030001  // addiu v1, zero, 0x1
    .word 0x3C010081  // lui at, %hi(D_008106BE) [hardcoded]
    .word 0x1000000A  // b .L001B0B98 (+0xA*4=+0x28)
    .word 0xA02306BE  // sb v1, %lo(D_008106BE)(at) [hardcoded, delay slot]
    .word 0x30830002  // andi v1, a0, 0x2 (DEAD -- 2.3.1 dead instruction)
    // .L001B0B78:
    .word 0x10600005  // beqz v1, .L001B0B90 (+5*4=+0x14)
    .word 0x00000000  // nop (delay slot)
    .word 0x24030081  // addiu v1, zero, 0x81
    .word 0x3C010081  // lui at, %hi(D_008106BE) [hardcoded]
    .word 0x10000003  // b .L001B0B98 (+3*4=+0xC)
    .word 0xA02306BE  // sb v1, %lo(D_008106BE)(at) [hardcoded, delay slot]
    // .L001B0B90:
    .word 0x3C010081  // lui at, %hi(D_008106BE) [hardcoded]
    .word 0xA02006BE  // sb zero, %lo(D_008106BE)(at) [hardcoded]
    // .L001B0B98:
    .word 0x03E00008  // jr ra
    .word 0x00000000  // nop (delay slot)
}
