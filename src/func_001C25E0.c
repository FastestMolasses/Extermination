// CFLAGS: -O4,p -sdatathreshold 0
// asm void: scheduler places paddub a0 before jal (at [34]) and in delay slot [37];
// pure C only puts it in the delay slot. All lui/addiu %hi/%lo hardcoded.
// Byte-identical at link time.
extern void func_001026A0(int, int, int);
extern void func_001028B8(int, int, int);
extern void func_0019B4C0(int, int, int, int);
extern int D_700038B0;
extern int D_700038C0;
extern int D_700038D0;

asm void func_001C25E0(int a0, int a1) {
    .word 0x27BDFFE0  // addiu sp, sp, -0x20
    .word 0x7FBF0010  // sq ra, 0x10(sp)
    .word 0x7FB00000  // sq s0, 0x0(sp)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2038B0  // sw zero, 0x38B0(at)
    .word 0x3C02C080  // lui v0, 0xC080
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2238B4  // sw v0, 0x38B4(at)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2038B8  // sw zero, 0x38B8(at)
    .word 0x3C023F80  // lui v0, 0x3F80
    .word 0x3C017000  // lui at, 0x7000
    .word 0xAC2238BC  // sw v0, 0x38BC(at)
    .word 0x70808628  // paddub s0, a0, zero
    .word 0x70A03628  // paddub a2, a1, zero
    .word 0x3C027000  // lui v0, %hi(D_700038C0) [hardcoded]
    .word 0x244438C0  // addiu a0, v0, %lo(D_700038C0) [hardcoded]
    jal func_001026A0
    .word 0x260500D0  // addiu a1, s0, 0xD0 (delay slot)
    .word 0x3C027000  // lui v0, %hi(D_700038C0) [hardcoded]
    .word 0x244438C0  // addiu a0, v0, %lo(D_700038C0) [hardcoded]
    .word 0x3C027000  // lui v0, %hi(D_700038C0) [hardcoded]
    .word 0x244538C0  // addiu a1, v0, %lo(D_700038C0) [hardcoded]
    jal func_001028B8
    .word 0x260600B0  // addiu a2, s0, 0xB0 (delay slot)
    .word 0x3C027000  // lui v0, %hi(D_700038D0) [hardcoded]
    .word 0x244438D0  // addiu a0, v0, %lo(D_700038D0) [hardcoded]
    .word 0x3C027000  // lui v0, %hi(D_700038B0) [hardcoded]
    .word 0x260500D0  // addiu a1, s0, 0xD0
    jal func_001026A0
    .word 0x244638B0  // addiu a2, v0, %lo(D_700038B0) [hardcoded, delay slot]
    .word 0x3C027000  // lui v0, %hi(D_700038C0) [hardcoded]
    .word 0x244538C0  // addiu a1, v0, %lo(D_700038C0) [hardcoded]
    .word 0x3C027000  // lui v0, %hi(D_700038D0) [hardcoded]
    .word 0x244638D0  // addiu a2, v0, %lo(D_700038D0) [hardcoded]
    .word 0x24070006  // addiu a3, zero, 0x6
    jal func_0019B4C0
    .word 0x72002628  // paddub a0, s0, zero (delay slot)
    .word 0x7BBF0010  // lq ra, 0x10(sp)
    .word 0x7BB00000  // lq s0, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0020  // addiu sp, sp, 0x20
}
