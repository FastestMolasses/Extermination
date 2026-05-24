// CFLAGS: -O4,p -sdatathreshold 0
// asm void: 2.3.1 dead instruction (paddub v0,zero,zero at end of b+nop) not emitted by pure C.
// Byte-identical at link time.
extern int func_001D2730(int);
extern int func_001E0C80(int);

asm int func_001D2830(int a0) {
    .word 0x27BDFFF0  // addiu sp, sp, -0x10
    .word 0x28810020  // slti at, a0, 0x20
    .word 0x10200005  // beqz at, +5*4=+0x14
    .word 0x7FBF0000  // sq ra, 0x0(sp) (delay slot)
    jal func_001D2730
    .word 0x00000000  // nop (delay slot)
    .word 0x1000000A  // b +0xA*4=+0x28
    .word 0x7BBF0000  // lq ra, 0x0(sp) (delay slot)
    // .L001D2850:
    .word 0x28810040  // slti at, a0, 0x40
    .word 0x10200006  // beqz at, +6*4=+0x18
    .word 0x70001628  // paddub v0, zero, zero (delay slot)
    jal func_001E0C80
    .word 0x00000000  // nop (delay slot)
    .word 0x10000002  // b +2*4=+0x8
    .word 0x00000000  // nop (delay slot)
    .word 0x70001628  // paddub v0, zero, zero (DEAD -- 2.3.1 dead instruction)
    // .L001D2870:
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    // .L001D2874:
    .word 0x03E00008  // jr ra
    .word 0x27BD0010  // addiu sp, sp, 0x10 (delay slot)
}
