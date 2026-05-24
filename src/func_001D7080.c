// CFLAGS: -O4,p -sdatathreshold 4
// asm void: gp_rel load and 64-bit register manipulation (dsll32/or);
// lw gp_rel hardcoded .word -- byte-identical at link time.
extern int *D_00275670;

asm void func_001D7080(int a0, int a1, float fa0) {
    .word 0x00041880  // sll v1, a0, 2
    .word 0x8F848300  // lw a0, %gp_rel(D_00275670)(gp) [hardcoded]
    .word 0x3C025000  // lui v0, 0x5000
    .word 0x34470002  // ori a3, v0, 0x0002
    .word 0x24080010  // addiu t0, zero, 0x10
    .word 0x24090003  // addiu t1, zero, 0x3
    .word 0x00645021  // addu t2, v1, a0
    .word 0x8D420010  // lw v0, 0x10(t2)
    .word 0x3C031000  // lui v1, 0x1000
    .word 0x0003203C  // dsll32 a0, v1, 0
    .word 0x34038001  // ori v1, zero, 0x8001
    .word 0x00643025  // or a2, v1, a0
    .word 0x2404000E  // addiu a0, zero, 0xE
    .word 0xA0480003  // sb t0, 0x3(v0)
    .word 0x8D480010  // lw t0, 0x10(t2)
    .word 0x24030001  // addiu v1, zero, 0x1
    .word 0xAD000004  // sw zero, 0x4(t0)
    .word 0x8D480010  // lw t0, 0x10(t2)
    .word 0xA5090000  // sh t1, 0x0(t0)
    .word 0x8D490010  // lw t1, 0x10(t2)
    .word 0x25280040  // addiu t0, t1, 0x40
    .word 0xAD480010  // sw t0, 0x10(t2)
    .word 0x7D200010  // sq zero, 0x10(t1)
    .word 0xAD27001C  // sw a3, 0x1C(t1)
    .word 0xFD260020  // sd a2, 0x20(t1)
    .word 0xFD240028  // sd a0, 0x28(t1)
    .word 0xAD250030  // sw a1, 0x30(t1)
    .word 0xE52C0034  // swc1 f12, 0x34(t1)
    .word 0x03E00008  // jr ra
    .word 0xFD230038  // sd v1, 0x38(t1)
}
