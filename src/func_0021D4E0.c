// CFLAGS: -O4,p -sdatathreshold 0
// asm void: 2.3.1 dead instruction (lui v0, 0x8000 after b+lq delay slot) not emitted by
// pure C. Also bnez delay slot has lui v0 hoisted from branch target. Byte-identical at link time.
extern void func_001F0060(int, int);

asm void func_0021D4E0(unsigned char *a0) {
    .word 0x27BDFFF0  // addiu sp, sp, -0x10
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    .word 0x90820234  // lbu v0, 0x234(a0)
    .word 0x14400008  // bnez v0, .L0021D510 (+8*4=+0x20)
    .word 0x3C028000  // lui v0, 0x8000 (delay slot -- pre-loaded for .L0021D510 path)
    // not-taken path:
    .word 0x3C028000  // lui v0, 0x8000
    .word 0x34440061  // ori a0, v0, 0x61
    jal func_001F0060
    .word 0x70002E28  // paddub a1, zero, zero (delay slot)
    .word 0x10000006  // b +6*4=+0x18 (.L0021D520)
    .word 0x7BBF0000  // lq ra, 0x0(sp) (delay slot)
    .word 0x3C028000  // lui v0, 0x8000 (DEAD -- 2.3.1 dead instruction)
    // .L0021D510:
    .word 0x34440062  // ori a0, v0, 0x62
    jal func_001F0060
    .word 0x70002E28  // paddub a1, zero, zero (delay slot)
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    // .L0021D520:
    .word 0x03E00008  // jr ra
    .word 0x27BD0010  // addiu sp, sp, 0x10 (delay slot)
}
