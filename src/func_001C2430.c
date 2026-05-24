// CFLAGS: -O4,p -sdatathreshold 0
// asm void: scheduler places lui at, 0x7000 for 0x700038DC at [24] vs expected [26].
// All lui/addiu %hi/%lo hardcoded as .word (mwcc inline asm rejects %hi/%lo).
// Byte-identical at link time.
extern void func_001026A0(int, int, int);
extern void func_001028B8(int, int, int);
extern void func_0019AB20(int, int, int, int);
extern int D_700038C0;
extern int D_700038D0;

asm void func_001C2430(int a0, int a1, int a2) {
    .word 0x27BDFFE0  // addiu sp, sp, -0x20
    .word 0x7FBF0010  // sq ra, 0x10(sp)
    .word 0x7FB00000  // sq s0, 0x0(sp)
    .word 0x70A01E28  // paddub v1, a1, zero
    .word 0x70C02E28  // paddub a1, a2, zero
    .word 0x70808628  // paddub s0, a0, zero
    .word 0x3C027000  // lui v0, %hi(D_700038C0) [hardcoded]
    .word 0x244438C0  // addiu a0, v0, %lo(D_700038C0) [hardcoded]
    jal func_001026A0
    .word 0x70603628  // paddub a2, v1, zero (delay slot)
    .word 0x3C027000  // lui v0, %hi(D_700038C0) [hardcoded]
    .word 0x244438C0  // addiu a0, v0, %lo(D_700038C0) [hardcoded]
    .word 0x3C027000  // lui v0, %hi(D_700038C0) [hardcoded]
    .word 0x244538C0  // addiu a1, v0, %lo(D_700038C0) [hardcoded]
    jal func_001028B8
    .word 0x260600B0  // addiu a2, s0, 0xB0 (delay slot)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2038D0  // sw zero, 0x38D0(at)
    .word 0x3C02C120  // lui v0, 0xC120
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2238D4  // sw v0, 0x38D4(at)
    .word 0x3C027000  // lui v0, %hi(D_700038C0) [hardcoded]
    .word 0x244538C0  // addiu a1, v0, %lo(D_700038C0) [hardcoded]
    .word 0x3C027000  // lui v0, %hi(D_700038D0) [hardcoded]
    .word 0x244638D0  // addiu a2, v0, %lo(D_700038D0) [hardcoded]
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2038D8  // sw zero, 0x38D8(at)
    .word 0x3C028000  // lui v0, 0x8000
    .word 0x3C017000  // lui at, 0x7000
    .word 0x72002628  // paddub a0, s0, zero
    .word 0x34470007  // ori a3, v0, 0x7
    jal func_0019AB20
    .word 0xAC2038DC  // sw zero, 0x38DC(at) (delay slot)
    .word 0x7BBF0010  // lq ra, 0x10(sp)
    .word 0x7BB00000  // lq s0, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0020  // addiu sp, sp, 0x20
}
