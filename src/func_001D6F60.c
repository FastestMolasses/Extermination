// CFLAGS: -O4,p -sdatathreshold 4
// asm void: 64-bit register manipulation (dsll32/dsra32/or), gp_rel load;
// lw gp_rel hardcoded .word -- byte-identical at link time.
extern int *D_00275670;

asm void func_001D6F60(int a0, int a1, int a2, int a3) {
    .word 0x00041880  // sll v1, a0, 2
    .word 0x0006203C  // dsll32 a0, a2, 0
    .word 0x8F868300  // lw a2, %gp_rel(D_00275670)(gp) [hardcoded]
    .word 0x3C025000  // lui v0, 0x5000
    .word 0x344A0004  // ori t2, v0, 0x0004
    .word 0x24070010  // addiu a3, zero, 0x10
    .word 0x240C0005  // addiu t4, zero, 0x5
    .word 0x00666821  // addu t5, v1, a2
    .word 0x8DA20010  // lw v0, 0x10(t5)
    .word 0x3C031000  // lui v1, 0x1000
    .word 0x0003303C  // dsll32 a2, v1, 0
    .word 0x34038003  // ori v1, zero, 0x8003
    .word 0x00664825  // or t1, v1, a2
    .word 0x2408000E  // addiu t0, zero, 0xE
    .word 0xA0470003  // sb a3, 0x3(v0)
    .word 0x8DAB0010  // lw t3, 0x10(t5)
    .word 0x2407003F  // addiu a3, zero, 0x3F
    .word 0x24060006  // addiu a2, zero, 0x6
    .word 0x0004203F  // dsra32 a0, a0, 0
    .word 0xAD600004  // sw zero, 0x4(t3)
    .word 0x8DAB0010  // lw t3, 0x10(t5)
    .word 0x2403003B  // addiu v1, zero, 0x3B
    .word 0xA56C0000  // sh t4, 0x0(t3)
    .word 0x8DAC0010  // lw t4, 0x10(t5)
    .word 0x258B0060  // addiu t3, t4, 0x60
    .word 0xADAB0010  // sw t3, 0x10(t5)
    .word 0x7D800010  // sq zero, 0x10(t4)
    .word 0xAD8A001C  // sw t2, 0x1C(t4)
    .word 0xFD890020  // sd t1, 0x20(t4)
    .word 0xFD880028  // sd t0, 0x28(t4)
    .word 0xFD800030  // sd zero, 0x30(t4)
    .word 0xFD870038  // sd a3, 0x38(t4)
    .word 0xFD850040  // sd a1, 0x40(t4)
    .word 0xFD860048  // sd a2, 0x48(t4)
    .word 0xFD840050  // sd a0, 0x50(t4)
    .word 0x03E00008  // jr ra
    .word 0xFD830058  // sd v1, 0x58(t4)
}
