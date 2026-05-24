// CFLAGS: -O4,p -sdatathreshold 0
// asm void: dead addiu v1,zero,2 at [40] after bne delay slot; bne delay hoisted.
// lui/addiu %hi/%lo hardcoded .word. Byte-identical at link time.
extern char D_008102B0[];

asm void func_001831F0(int a0) {
    .word 0x3C030081  // lui v1, %hi(D_008102B0) [hardcoded]
    .word 0x14800005  // bnez a0, case_nonzero (offset +5)
    .word 0x246602B0  // addiu a2, v1, %lo(D_008102B0) [hardcoded, delay slot -- a2 = D_008102B0]
    // case 0 (a0==0):
    .word 0x2403FFFF  // addiu v1, zero, -1
    .word 0xA0C0023F  // sb zero, 0x23F(a2)
    .word 0x1000000B  // b exit (offset +11)
    .word 0xACC3024C  // sw v1, 0x24C(a2) (delay)
    // case_nonzero (a0!=0):
    .word 0x24050001  // addiu a1, zero, 0x1
    .word 0x14850006  // bne a0, a1, case_else (offset +6)
    .word 0x24030002  // addiu v1, zero, 2 (delay -- pre-hoisted for case1 taken path)
    // case 1 (a0==1, not-taken):
    .word 0x24030002  // addiu v1, zero, 2 (DEAD -- copy of delay slot)
    .word 0xA0C3023F  // sb v1, 0x23F(a2)
    .word 0x10000004  // b exit (offset +4)
    .word 0xACC0024C  // sw zero, 0x24C(a2) (delay)
    // case_else (a0!=1, taken at bne):
    .word 0x24030002  // addiu v1, zero, 2
    .word 0xA0C3023F  // sb v1, 0x23F(a2)
    .word 0xACC5024C  // sw a1, 0x24C(a2) [a1=1 from earlier]
    // exit:
    .word 0x03E00008  // jr ra
    .word 0x00000000  // nop (delay)
}
