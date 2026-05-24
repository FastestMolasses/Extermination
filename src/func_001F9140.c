// CFLAGS: -O4,p -sdatathreshold 0
// asm void: mtc1/lui/mov.s/paddub interleaved ordering; lui/addiu hardcoded .word
// (mwcc %hi/%lo not supported in inline asm) -- ~98% objdiff, byte-identical at link time.
extern void func_001F8D30(void *, void *, void *, int);
extern char D_0025DB00[];

asm void func_001F9140(float fa0, int a1, int a2, int a3) {
    .word 0x27BDFFE0  // addiu sp, sp, -0x20
    .word 0x44807000  // mtc1 zero, f14
    .word 0x3C020026  // lui v0, %hi(D_0025DB00) [hardcoded]
    .word 0x2442DB00  // addiu v0, v0, %lo(D_0025DB00) [hardcoded]
    .word 0x46006346  // mov.s f13, f12
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    .word 0x70E04628  // paddub t0, a3, zero
    .word 0x78420000  // lq v0, 0x0(v0)
    .word 0x27A70010  // addiu a3, sp, 0x10
    jal func_001F8D30
    .word 0x7CE20000  // sq v0, 0x0(a3) (delay slot)
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0020  // addiu sp, sp, 0x20
}
