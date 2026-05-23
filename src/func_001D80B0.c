// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001D8060(int, int, int, int);

asm void func_001D80B0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_001D8060
    nop
    .word 0x10400004
    nop
    addiu $v1, $zero, -0x1
    sw $zero, 0x2C($v0)
    sw $v1, 0xC($v0)
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
