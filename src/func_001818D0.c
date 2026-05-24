// CFLAGS: -O4,p -sdatathreshold 0
// asm void: scheduler places paddub a0,s0,zero before jal (at [22]) vs in delay slot [22].
// All lui/addiu %hi/%lo hardcoded .word. Byte-identical at link time.
extern int func_001026A0(int, int, int);
extern int func_0019AD00(int, int, int);
extern int D_700038A0;
extern int D_700038B0;

asm int func_001818D0(int a0) {
    .word 0x27BDFFE0  // addiu sp, sp, -0x20
    .word 0x7FBF0010  // sq ra, 0x10(sp)
    .word 0x7FB00000  // sq s0, 0x0(sp)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2038A0  // sw zero, 0x38A0(at)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2038A4  // sw zero, 0x38A4(at)
    .word 0x3C0241A0  // lui v0, 0x41A0
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2238A8  // sw v0, 0x38A8(at)
    .word 0x70808628  // paddub s0, a0, zero
    .word 0x3C023F80  // lui v0, 0x3F80
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2238AC  // sw v0, 0x38AC(at)
    .word 0x3C027000  // lui v0, %hi(D_700038B0) [hardcoded]
    .word 0x244438B0  // addiu a0, v0, %lo(D_700038B0) [hardcoded]
    .word 0x3C027000  // lui v0, %hi(D_700038A0) [hardcoded]
    .word 0x260500D0  // addiu a1, s0, 0xD0
    jal func_001026A0
    .word 0x244638A0  // addiu a2, v0, %lo(D_700038A0) [hardcoded, delay slot]
    .word 0x3C027000  // lui v0, %hi(D_700038B0) [hardcoded]
    .word 0x244538B0  // addiu a1, v0, %lo(D_700038B0) [hardcoded]
    .word 0x24060007  // addiu a2, zero, 0x7
    jal func_0019AD00
    .word 0x72002628  // paddub a0, s0, zero (delay slot -- scheduler places here)
    .word 0x7BBF0010  // lq ra, 0x10(sp)
    .word 0x7BB00000  // lq s0, 0x0(sp)
    .word 0x24030001  // addiu v1, zero, 0x1
    .word 0x0002180A  // movz v1, zero, v0
    .word 0x70601628  // paddub v0, v1, zero
    .word 0x03E00008  // jr ra
    .word 0x27BD0020  // addiu sp, sp, 0x20 (delay slot)
}
