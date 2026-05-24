// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001B1190(int);
extern void func_001AFC10(char *);

// mwcc uses $a0 for the sw zero, 0x20 store instead of $s0 from C — asm void required.
asm void func_00131E80(char *a0) {
    .word 0x27BDFFE0  // addiu sp, sp, -0x20
    .word 0x7FBF0010  // sq ra, 0x10(sp)
    .word 0x7FB00000  // sq s0, 0x0(sp)
    .word 0x8C830020  // lw v1, 0x20(a0)
    .word 0x10600004  // beqz v1, +4 instrs
    .word 0x70808628  // paddub s0, a0, zero
    .word 0x24020003  // addiu v0, zero, 0x3
    .word 0xA0620004  // sb v0, 0x4(v1)
    .word 0xAE000020  // sw zero, 0x20(s0)
    jal func_001B1190
    .word 0x9204009A  // lbu a0, 0x9A(s0)
    jal func_001AFC10
    .word 0x72002628  // paddub a0, s0, zero
    .word 0x7BBF0010  // lq ra, 0x10(sp)
    .word 0x7BB00000  // lq s0, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0020  // addiu sp, sp, 0x20
}
