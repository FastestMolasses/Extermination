// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001749A0(int, int, int, int);

asm void func_001800C0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lbu $v0, 0x2F1($a0)
    .word 0x14400007
    addiu $a1, $zero, 0xA0
    addiu $a1, $zero, 0x9E
    jal func_001749A0
    addiu $a2, $zero, 0x1
    .word 0x10000005
    lq $ra, 0x0($sp)
    addiu $a1, $zero, 0xA0
    jal func_001749A0
    addiu $a2, $zero, 0x1
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
