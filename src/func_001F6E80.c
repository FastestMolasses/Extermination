// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001F66F0(int, int, int, int);
extern void func_001F6D60(int, int, int, int);

asm void func_001F6E80(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_001F6D60
    nop
    paddub $a0, $v0, $zero
    .word 0x10800003
    nop
    jal func_001F66F0
    nop
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
