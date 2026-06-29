// CFLAGS: -O4,p -sdatathreshold 0
// asm void: 2.3.1 dead instruction (addiu a1,zero,0xEB after b+lq) and bnez delay slot
// pre-hoisted for taken path. Byte-identical at link time.
extern void func_001749A0(int, int, int);

asm void func_0017FD40(unsigned char *a0) {
    .word 0x27BDFFF0  // addiu sp, sp, -0x10
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    .word 0x908202F1  // lbu v0, 0x2F1(a0)
    .word 0x14400007  // bnez v0, taken (+7*4=+0x1C)
    .word 0x240500EB  // addiu a1, zero, 0xEB (delay -- pre-hoisted for taken path)
    .word 0x240500E9  // addiu a1, zero, 0xE9 (not-taken)
    jal func_001749A0
    .word 0x70003628  // paddub a2, zero, zero (delay slot)
    .word 0x10000005  // b exit (+5*4=+0x14)
    .word 0x7BBF0000  // lq ra, 0x0(sp) (delay slot)
    .word 0x240500EB  // addiu a1, zero, 0xEB (DEAD -- 2.3.1 dead instruction)
    // taken:
    jal func_001749A0
    .word 0x70003628  // paddub a2, zero, zero (delay slot)
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    // exit:
    .word 0x03E00008  // jr ra
    .word 0x27BD0010  // addiu sp, sp, 0x10 (delay slot)
}
