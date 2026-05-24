// CFLAGS: -O4,p -sdatathreshold 0
// asm void: lui v0, D_008106B0 is interleaved before sb 0x3B92 (not where pure C places it).
// All lui/addiu hardcoded .word. Byte-identical at link time.
extern char D_008106B0[0x80];
extern void func_00121A28(char *, int, int);
extern void func_001FC9B0(void);

asm void func_001AFCF0(void) {
    .word 0x27BDFFF0  // addiu sp, sp, -0x10
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xA4203B84  // sh zero, 0x3B84(at)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xA0203B93  // sb zero, 0x3B93(at)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xA0203B8C  // sb zero, 0x3B8C(at)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xA0203B8D  // sb zero, 0x3B8D(at)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xA0203B8E  // sb zero, 0x3B8E(at)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xA0203B8F  // sb zero, 0x3B8F(at)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xA0203B91  // sb zero, 0x3B91(at)
    .word 0x3C017000  // lui at, 0x7000
    .word 0x3C020081  // lui v0, %hi(D_008106B0) [hardcoded -- interleaved here]
    .word 0xA0203B92  // sb zero, 0x3B92(at)
    .word 0x3C017000  // lui at, 0x7000
    .word 0x244406B0  // addiu a0, v0, %lo(D_008106B0) [hardcoded]
    .word 0x24060048  // addiu a2, zero, 0x48
    .word 0x70002E28  // paddub a1, zero, zero
    jal func_00121A28
    .word 0xAC203258  // sw zero, 0x3258(at) (delay slot)
    jal func_001FC9B0
    .word 0x00000000  // nop (delay slot)
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0010  // addiu sp, sp, 0x10 (delay slot)
}
