// CFLAGS: -O4,p -sdatathreshold 0
// asm void: mwcc fills beqz delay slot with addiu (omits the original nop);
// lui/addiu hardcoded .word (absolute addresses, no relocations needed).
// Byte-identical at link time.
extern int func_0019AB20(int, int, int, int);
extern void func_0019A310(int);
extern int D_700038A0;

asm int func_00179010(int a0) {
    .word 0x27BDFFE0  // addiu sp, sp, -0x20
    .word 0x7FBF0010  // sq ra, 0x10(sp)
    .word 0x7FB00000  // sq s0, 0x0(sp)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2038A0  // sw zero, 0x38A0(at)
    .word 0x3C02C0C0  // lui v0, 0xC0C0
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2238A4  // sw v0, 0x38A4(at)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2038A8  // sw zero, 0x38A8(at)
    .word 0x3C023F80  // lui v0, 0x3F80
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2238AC  // sw v0, 0x38AC(at)
    .word 0x3C027000  // lui v0, %hi(D_700038A0) [hardcoded]
    .word 0x244638A0  // addiu a2, v0, %lo(D_700038A0) [hardcoded]
    .word 0x70808628  // paddub s0, a0, zero
    .word 0x3C028000  // lui v0, 0x8000
    .word 0x260500B0  // addiu a1, s0, 0xB0
    jal func_0019AB20
    .word 0x34470006  // ori a3, v0, 0x6 (delay slot)
    .word 0x1040000B  // beqz v0, +0xB*4=+0x2C
    .word 0x00000000  // nop (delay slot)
    .word 0x24020001  // addiu v0, zero, 0x1
    .word 0xA202000A  // sb v0, 0xA(s0)
    .word 0x3C017000  // lui at, 0x7000
    .word 0x8C2231D0  // lw v0, 0x31D0(at)
    .word 0x2604009C  // addiu a0, s0, 0x9C
    .word 0x9042001A  // lbu v0, 0x1A(v0)
    jal func_0019A310
    .word 0xA202023B  // sb v0, 0x23B(s0) (delay slot)
    .word 0x10000003  // b +3*4=+0xC
    .word 0x9202000A  // lbu v0, 0xA(s0) (delay slot)
    // .L00179090:
    .word 0xA200023B  // sb zero, 0x23B(s0)
    .word 0x9202000A  // lbu v0, 0xA(s0)
    // .L00179098:
    .word 0x7BBF0010  // lq ra, 0x10(sp)
    .word 0x7BB00000  // lq s0, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0020  // addiu sp, sp, 0x20
}
