// CFLAGS: -O4,p -sdatathreshold 0
// asm void: lui/addiu hardcoded .word (mwcc %hi/%lo not supported in inline asm);
// bnez branch .word encoded; byte-identical at link time.
extern void func_001FC7B0(void *);
extern char D_00264BF0[];

asm void func_001FC770(void *a0, int a1, int a2, int a3) {
    .word 0x27BDFFD0  // addiu sp, sp, -0x30
    .word 0x3C020026  // lui v0, %hi(D_00264BF0) [hardcoded]
    .word 0x24424BF0  // addiu v0, v0, %lo(D_00264BF0) [hardcoded]
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    .word 0x78430000  // lq v1, 0x0(v0)
    .word 0x27A80010  // addiu t0, sp, 0x10
    .word 0xDC420010  // ld v0, 0x10(v0)
    .word 0x7D030000  // sq v1, 0x0(t0)
    .word 0x14E00002  // bnez a3, +2*4=+0x8
    .word 0xFD020010  // sd v0, 0x10(t0) (delay slot)
    .word 0x71003E28  // paddub a3, t0, zero
    jal func_001FC7B0
    .word 0x00000000  // nop (delay slot)
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0030  // addiu sp, sp, 0x30
}
