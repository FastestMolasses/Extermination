// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001AF890(void *);

// mwcc delays addiu v1, zero, -1 until after sw zero, 0x90(s0); expected has it before.
// asm void needed to match instruction ordering.
asm void func_001CA770(char *a0) {
    .word 0x27BDFFE0  // addiu sp, sp, -0x20
    .word 0x7FBF0010  // sq ra, 0x10(sp)
    .word 0x7FB00000  // sq s0, 0x0(sp)
    .word 0x70808628  // paddub s0, a0, zero
    .word 0x8C840090  // lw a0, 0x90(a0)
    .word 0x10800006  // beqz a0, +6
    .word 0x00000000  // nop
    jal func_001AF890
    .word 0x00000000  // nop
    .word 0x2403FFFF  // addiu v1, zero, -1
    .word 0xAE000090  // sw zero, 0x90(s0)
    .word 0xA6030094  // sh v1, 0x94(s0)
    .word 0x7BBF0010  // lq ra, 0x10(sp)
    .word 0x7BB00000  // lq s0, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0020  // addiu sp, sp, 0x20
}
