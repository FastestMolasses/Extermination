// CFLAGS: -O4,p -sdatathreshold 0
// asm void: compiler uses $at for slti temp but expected uses $v1; lui/addiu hardcoded
// (mwcc %hi/%lo not in inline asm); beql/slti/bnez branches hardcoded.
// All byte-identical at link time.
extern void func_001AF800(unsigned char *);
extern unsigned char D_0028B020[];

asm void func_001AFEB0(void) {
    .word 0x27BDFFD0  // addiu sp, sp, -0x30
    .word 0x7FBF0020  // sq ra, 0x20(sp)
    .word 0x7FB10010  // sq s1, 0x10(sp)
    .word 0x7FB00000  // sq s0, 0x0(sp)
    .word 0x3C030029  // lui v1, %hi(D_0028B020) [hardcoded]
    .word 0x2470B020  // addiu s0, v1, %lo(D_0028B020) [hardcoded]
    .word 0x70008E28  // paddub s1, zero, zero
    // loop:
    .word 0x92030000  // lbu v1, 0x0(s0)
    .word 0x50600004  // beql v1, zero, +4 (skip jal+addiu)
    .word 0x26310001  // addiu s1, s1, 0x1 (delay slot)
    jal func_001AF800
    .word 0x72002628  // paddub a0, s0, zero (delay slot)
    .word 0x26310001  // addiu s1, s1, 0x1
    // end loop:
    .word 0x2A230018  // slti v1, s1, 0x18
    .word 0x1460FFF8  // bnez v1, -8*4=-0x20 (loop back)
    .word 0x261002F0  // addiu s0, s0, 0x2F0 (delay slot)
    .word 0x7BBF0020  // lq ra, 0x20(sp)
    .word 0x7BB10010  // lq s1, 0x10(sp)
    .word 0x7BB00000  // lq s0, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0030  // addiu sp, sp, 0x30
}
