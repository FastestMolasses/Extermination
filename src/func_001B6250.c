// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00111018(int, int, int, int);

asm void func_001B6250(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lbu $v1, 0x12($a0)
    .word 0x1060000c
    paddub $a2, $a0, $zero
    lbu $v1, 0x16($a2)
    .word 0x10600009
    nop
    sb $zero, 0x16($a2)
    sh $zero, 0x28($a2)
    sb $zero, 0x18($a2)
    sb $zero, 0x19($a2)
    lw $a0, 0x4($a2)
    lw $a1, 0x8($a2)
    jal func_00111018
    addiu $a2, $a2, 0x18
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
