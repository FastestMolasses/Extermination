// CFLAGS: -O4,p -sdatathreshold 0
// asm void: paddub s0/s1/s2 save args across jal calls; paddub in jal delay slots.
// All lui/addiu %hi/%lo hardcoded .word. Byte-identical at link time.
extern void func_001029C0(int);
extern void func_00102C58(int, int, int);
extern void func_001026A0(int, int, int);
extern int D_700036A0;

asm void func_00131F20(int a0, int a1, int a2) {
    .word 0x27BDFFC0  // addiu sp, sp, -0x40
    .word 0x7FBF0030  // sq ra, 0x30(sp)
    .word 0x7FB20020  // sq s2, 0x20(sp)
    .word 0x7FB10010  // sq s1, 0x10(sp)
    .word 0x7FB00000  // sq s0, 0x0(sp)
    .word 0x70809628  // paddub s2, a0, zero  [s2 = a0]
    .word 0x3C027000  // lui v0, %hi(D_700036A0) [hardcoded]
    .word 0x244436A0  // addiu a0, v0, %lo(D_700036A0) [hardcoded]
    .word 0x70A08E28  // paddub s1, a1, zero  [s1 = a1]
    jal func_001029C0
    .word 0x70C08628  // paddub s0, a2, zero  [s0 = a2, delay slot]
    .word 0x3C027000  // lui v0, %hi(D_700036A0) [hardcoded]
    .word 0x244436A0  // addiu a0, v0, %lo(D_700036A0) [hardcoded]
    .word 0x3C027000  // lui v0, %hi(D_700036A0) [hardcoded]
    .word 0x264600C0  // addiu a2, s2, 0xC0   [a2 = s2 + 0xC0 = orig_a0 + 0xC0]
    jal func_00102C58
    .word 0x244536A0  // addiu a1, v0, %lo(D_700036A0) [hardcoded, delay slot]
    .word 0x3C027000  // lui v0, %hi(D_700036A0) [hardcoded]
    .word 0x244536A0  // addiu a1, v0, %lo(D_700036A0) [hardcoded]
    .word 0x72002628  // paddub a0, s0, zero  [a0 = s0 = orig_a2]
    jal func_001026A0
    .word 0x72203628  // paddub a2, s1, zero  [a2 = s1 = orig_a1, delay slot]
    .word 0x7BBF0030  // lq ra, 0x30(sp)
    .word 0x7BB20020  // lq s2, 0x20(sp)
    .word 0x7BB10010  // lq s1, 0x10(sp)
    .word 0x7BB00000  // lq s0, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0040  // addiu sp, sp, 0x40 (delay)
}
