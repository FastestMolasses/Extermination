// CFLAGS: -O4,p -sdatathreshold 4
// asm void: 64-bit register manipulation (dsll32/dsra32/or), gp_rel load, return via v0;
// lw gp_rel hardcoded .word -- byte-identical at link time.
extern int *D_00275670;

asm void *func_001D6DD0(int a0, int a1, int a2, int a3) {
    .word 0x0006103C  // dsll32 v0, a2, 0
    .word 0x0005183C  // dsll32 v1, a1, 0
    .word 0x0002103F  // dsra32 v0, v0, 0
    .word 0x00043880  // sll a3, a0, 2
    .word 0x0003183F  // dsra32 v1, v1, 0
    .word 0x0002103C  // dsll32 v0, v0, 0
    .word 0x00622025  // or a0, v1, v0
    .word 0x8F838300  // lw v1, %gp_rel(D_00275670)(gp) [hardcoded]
    .word 0x3C025000  // lui v0, 0x5000
    .word 0x240A0010  // addiu t2, zero, 0x10
    .word 0x24080003  // addiu t0, zero, 0x3
    .word 0x00675821  // addu t3, v1, a3
    .word 0x8D690010  // lw t1, 0x10(t3)
    .word 0x34470002  // ori a3, v0, 0x0002
    .word 0x3C021000  // lui v0, 0x1000
    .word 0x0002183C  // dsll32 v1, v0, 0
    .word 0x34028001  // ori v0, zero, 0x8001
    .word 0x00433025  // or a2, v0, v1
    .word 0xA12A0003  // sb t2, 0x3(t1)
    .word 0x8D620010  // lw v0, 0x10(t3)
    .word 0x2405000E  // addiu a1, zero, 0xE
    .word 0x24030018  // addiu v1, zero, 0x18
    .word 0xAC400004  // sw zero, 0x4(v0)
    .word 0x8D620010  // lw v0, 0x10(t3)
    .word 0xA4480000  // sh t0, 0x0(v0)
    .word 0x8D630010  // lw v1, 0x10(t3)
    .word 0x25280040  // addiu t0, v1, 0x40
    .word 0xAD680010  // sw t0, 0x10(t3)
    .word 0x7D200010  // sq zero, 0x10(v1)
    .word 0xAD27001C  // sw a3, 0x1C(v1)
    .word 0xFD260020  // sd a2, 0x20(v1)
    .word 0xFD250028  // sd a1, 0x28(v1)
    .word 0xFD240030  // sd a0, 0x30(v1)
    .word 0x25220010  // addiu v0, v1, 0x10
    .word 0x03E00008  // jr ra
    .word 0xFD230038  // sd v1, 0x38(v1)
}
