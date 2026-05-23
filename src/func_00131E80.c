// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001AFC10(int, int, int, int);
extern void func_001B1190(int, int, int, int);

asm void func_00131E80(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lw $v1, 0x20($a0)
    .word 0x10600004
    paddub $s0, $a0, $zero
    addiu $v0, $zero, 0x3
    sb $v0, 0x4($v1)
    sw $zero, 0x20($s0)
    jal func_001B1190
    lbu $a0, 0x9A($s0)
    jal func_001AFC10
    paddub $a0, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
