// CFLAGS: -O4,p -sdatathreshold 0
// asm void: instruction ordering and register allocation differ from pure C;
// lui/addiu hardcoded .word (mwcc %hi/%lo not supported in inline asm);
// beql/slti/bnez branches hardcoded. Byte-identical at link time.
extern void func_001026A0(int, int, int);
extern int func_0019AD00(int, int, int);
extern int D_00248970;
extern int D_700038A0;

asm int func_001790B0(int a0) {
    .word 0x27BDFFC0  // addiu sp, sp, -0x40
    .word 0x7FBF0030  // sq ra, 0x30(sp)
    .word 0x7FB20020  // sq s2, 0x20(sp)
    .word 0x7FB10010  // sq s1, 0x10(sp)
    .word 0x7FB00000  // sq s0, 0x0(sp)
    .word 0x3C020025  // lui v0, %hi(D_00248970) [hardcoded]
    .word 0x24518970  // addiu s1, v0, %lo(D_00248970) [hardcoded]
    .word 0xA0800314  // sb zero, 0x314(a0)
    .word 0x70809628  // paddub s2, a0, zero
    .word 0x70008628  // paddub s0, zero, zero
    // loop:
    .word 0x3C027000  // lui v0, %hi(D_700038A0) [hardcoded]
    .word 0x244438A0  // addiu a0, v0, %lo(D_700038A0) [hardcoded]
    .word 0x264500D0  // addiu a1, s2, 0xD0
    jal func_001026A0
    .word 0x72203628  // paddub a2, s1, zero (delay slot)
    .word 0x3C027000  // lui v0, %hi(D_700038A0) [hardcoded]
    .word 0x244538A0  // addiu a1, v0, %lo(D_700038A0) [hardcoded]
    .word 0x3C028000  // lui v0, 0x8000
    .word 0x72402628  // paddub a0, s2, zero
    jal func_0019AD00
    .word 0x34460007  // ori a2, v0, 0x7 (delay slot)
    .word 0x50400007  // beql v0, zero, +7*4=+0x1C
    .word 0x26100001  // addiu s0, s0, 0x1 (delay slot)
    .word 0x92420314  // lbu v0, 0x314(s2)
    .word 0x24030001  // addiu v1, zero, 0x1
    .word 0x02031804  // sllv v1, v1, s0
    .word 0x00431025  // or v0, v0, v1
    .word 0xA2420314  // sb v0, 0x314(s2)
    .word 0x26100001  // addiu s0, s0, 0x1
    // end loop:
    .word 0x2A010007  // slti at, s0, 0x7
    .word 0x1420FFEB  // bnez at, -21*4=-0x54 (loop back)
    .word 0x26310010  // addiu s1, s1, 0x10 (delay slot)
    .word 0x92420314  // lbu v0, 0x314(s2)
    .word 0x7BBF0030  // lq ra, 0x30(sp)
    .word 0x7BB10010  // lq s1, 0x10(sp)
    .word 0x7BB00000  // lq s0, 0x0(sp)
    .word 0x7BB20020  // lq s2, 0x20(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0040  // addiu sp, sp, 0x40
}
