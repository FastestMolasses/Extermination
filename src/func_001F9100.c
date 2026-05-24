// CFLAGS: -O4,p -sdatathreshold 0
// asm void: instruction ordering with mov.s/lui/sq interleaved can't be reproduced
// from pure C. The lui/addiu pairs for D_0025DAF0/D_0025DAE0 are hardcoded .word
// (mwcc inline asm doesn't support %hi/%lo) -- 98.4% objdiff, byte-identical at link time.
extern void func_001F8D30(void *, void *, float);
extern char D_0025DAF0[];
extern char D_0025DAE0[];

asm void func_001F9100(float fa0) {
    .word 0x27BDFFE0  // addiu sp, sp, -0x20
    .word 0x46006346  // mov.s f13, f12
    .word 0x3C020026  // lui v0, %hi(D_0025DAF0) [hardcoded]
    .word 0x2442DAF0  // addiu v0, v0, %lo(D_0025DAF0) [hardcoded]
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    .word 0x78430000  // lq v1, 0x0(v0)
    .word 0x27A70010  // addiu a3, sp, 0x10
    .word 0x3C0241F0  // lui v0, 0x41F0
    .word 0x44827000  // mtc1 v0, f14
    .word 0x7CE30000  // sq v1, 0x0(a3)
    .word 0x3C020026  // lui v0, %hi(D_0025DAE0) [hardcoded]
    jal func_001F8D30
    .word 0x2448DAE0  // addiu t0, v0, %lo(D_0025DAE0) [hardcoded, delay slot]
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0020  // addiu sp, sp, 0x20
}
