// CFLAGS: -O4,p -sdatathreshold 0
// asm void: register allocation order for paddub s0/s1/s2 differs from pure C; jal delay
// slots use paddub registers. All lui/addiu %hi/%lo hardcoded. Byte-identical at link time.
extern void func_001026A0(int, int, int);
extern void func_001028B8(int, int, int);
extern void func_0019B4C0(int, int, int, int);
extern int D_700038C0;
extern int D_700038D0;

asm void func_001C2540(int a0, int a1, int a2, int a3) {
    .word 0x27BDFFC0  // addiu sp, sp, -0x40
    .word 0x7FBF0030  // sq ra, 0x30(sp)
    .word 0x7FB20020  // sq s2, 0x20(sp)
    .word 0x7FB10010  // sq s1, 0x10(sp)
    .word 0x7FB00000  // sq s0, 0x0(sp)
    .word 0x70C08E28  // paddub s1, a2, zero
    .word 0x70809628  // paddub s2, a0, zero
    .word 0x3C027000  // lui v0, %hi(D_700038C0) [hardcoded]
    .word 0x70E08628  // paddub s0, a3, zero
    .word 0x70A03628  // paddub a2, a1, zero
    .word 0x244438C0  // addiu a0, v0, %lo(D_700038C0) [hardcoded]
    jal func_001026A0
    .word 0x72002E28  // paddub a1, s0, zero (delay slot)
    .word 0x3C027000  // lui v0, %hi(D_700038C0) [hardcoded]
    .word 0x244438C0  // addiu a0, v0, %lo(D_700038C0) [hardcoded]
    .word 0x3C027000  // lui v0, %hi(D_700038C0) [hardcoded]
    .word 0x244538C0  // addiu a1, v0, %lo(D_700038C0) [hardcoded]
    jal func_001028B8
    .word 0x264600B0  // addiu a2, s2, 0xB0 (delay slot)
    .word 0x3C027000  // lui v0, %hi(D_700038D0) [hardcoded]
    .word 0x244438D0  // addiu a0, v0, %lo(D_700038D0) [hardcoded]
    .word 0x72002E28  // paddub a1, s0, zero
    jal func_001026A0
    .word 0x72203628  // paddub a2, s1, zero (delay slot)
    .word 0x3C027000  // lui v0, %hi(D_700038C0) [hardcoded]
    .word 0x244538C0  // addiu a1, v0, %lo(D_700038C0) [hardcoded]
    .word 0x3C027000  // lui v0, %hi(D_700038D0) [hardcoded]
    .word 0x244638D0  // addiu a2, v0, %lo(D_700038D0) [hardcoded]
    .word 0x3C028000  // lui v0, 0x8000
    .word 0x72402628  // paddub a0, s2, zero
    jal func_0019B4C0
    .word 0x34470006  // ori a3, v0, 0x6 (delay slot)
    .word 0x7BBF0030  // lq ra, 0x30(sp)
    .word 0x7BB20020  // lq s2, 0x20(sp)
    .word 0x7BB10010  // lq s1, 0x10(sp)
    .word 0x7BB00000  // lq s0, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0040  // addiu sp, sp, 0x40 (delay slot)
}
