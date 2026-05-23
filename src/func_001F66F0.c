// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001D80B0(int, int, int, int);

asm void func_001F66F0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    .word 0x1600000d
    nop
    .word 0x1000000f
    lq $ra, 0x10($sp)
    lw $a0, 0x24($s0)
    addiu $v1, $zero, -0x1
    .word 0x50830006
    addiu $s0, $s0, 0x28
    jal func_001D80B0
    nop
    addiu $v1, $zero, -0x1
    sw $v1, 0x24($s0)
    addiu $s0, $s0, 0x28
    nop
    lh $v1, 0x0($s0)
    .word 0x0461fff4
    nop
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
