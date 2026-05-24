// CFLAGS: -O4,p -sdatathreshold 0
// asm void: lui v0, 0x4188 (float 17.0 constant) at [8] — mwcc interleaves
// lui v0, %hi(D_008105E0) there instead. lui/addiu %hi/%lo hardcoded .word.
// Byte-identical at link time.
extern void func_00102948(int, int);
extern float D_008105E0;

asm void func_00191530(int a0, int a1) {
    .word 0x27BDFFE0  // addiu sp, sp, -0x20
    .word 0x7FBF0010  // sq ra, 0x10(sp)
    .word 0x7FB00000  // sq s0, 0x0(sp)
    .word 0x70808628  // paddub s0, a0, zero  [save a0]
    .word 0x26040020  // addiu a0, s0, 0x20   [a0 + 0x20]
    jal func_00102948
    .word 0x24A500A0  // addiu a1, a1, 0xA0   [delay: a1 + 0xA0]
    .word 0xC6010024  // lwc1 f1, 0x24(s0)    [f1 = *(float*)(a0+0x24)]
    .word 0x3C024188  // lui v0, 0x4188       [17.0f integer hi]
    .word 0x44820000  // mtc1 v0, f0          [f0 = 17.0f]
    .word 0x26050020  // addiu a1, s0, 0x20   [a1 = orig_a0+0x20]
    .word 0x3C020081  // lui v0, %hi(D_008105E0) [hardcoded]
    .word 0x46000800  // add.s f0, f1, f0     [f0 = f1 + 17.0f]
    .word 0x244405E0  // addiu a0, v0, %lo(D_008105E0) [hardcoded]
    jal func_00102948
    .word 0xE6000024  // swc1 f0, 0x24(s0)    [delay: *(float*)(a0+0x24) = f0]
    .word 0x7BBF0010  // lq ra, 0x10(sp)
    .word 0x7BB00000  // lq s0, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0020  // addiu sp, sp, 0x20 (delay)
}
