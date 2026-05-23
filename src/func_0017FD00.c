// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001749A0(int, int, int, int);

asm void func_0017FD00(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lbu $v0, 0x2F1($a0)
    .word 0x14400007
    addiu $a1, $zero, 0xEA
    addiu $a1, $zero, 0xE8
    jal func_001749A0
    paddub $a2, $zero, $zero
    .word 0x10000005
    lq $ra, 0x0($sp)
    addiu $a1, $zero, 0xEA
    jal func_001749A0
    paddub $a2, $zero, $zero
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
