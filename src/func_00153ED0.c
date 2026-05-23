// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001C67E0(int, int, int, int);

asm void func_00153ED0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lh $a3, 0x0($a1)
    dsll32 $v1, $a2, 16
    dsra32 $v1, $v1, 16
    .word 0x10e30005
    nop
    sh $a2, 0x0($a1)
    mtc1 $zero, $f13
    jal func_001C67E0
    paddub $a1, $a2, $zero
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
