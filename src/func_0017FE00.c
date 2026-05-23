// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001749A0(int, int, int, int);

asm void func_0017FE00(void) {
    addiu $sp, $sp, -0x10
    .word 0x14a0000e
    sq $ra, 0x0($sp)
    lbu $v0, 0x2F1($a0)
    .word 0x14400007
    addiu $a1, $zero, 0xF9
    addiu $a1, $zero, 0xF3
    jal func_001749A0
    paddub $a2, $zero, $zero
    .word 0x10000012
    lq $ra, 0x0($sp)
    addiu $a1, $zero, 0xF9
    jal func_001749A0
    paddub $a2, $zero, $zero
    .word 0x1000000c
    nop
    lbu $v0, 0x2F1($a0)
    .word 0x14400007
    addiu $a1, $zero, 0xFC
    addiu $a1, $zero, 0xF6
    jal func_001749A0
    paddub $a2, $zero, $zero
    .word 0x10000004
    nop
    addiu $a1, $zero, 0xFC
    jal func_001749A0
    paddub $a2, $zero, $zero
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
