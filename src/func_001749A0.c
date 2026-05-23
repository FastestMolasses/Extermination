// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001C67E0(int, int, int, int);

asm void func_001749A0(void) {
    addiu $sp, $sp, -0x10
    .word 0x14c00008
    sq $ra, 0x0($sp)
    lh $v0, 0x20C($a0)
    dsll32 $v1, $a1, 16
    dsra32 $v1, $v1, 16
    .word 0x14620003
    nop
    .word 0x10000006
    addiu $v0, $zero, 0x1
    sh $a1, 0x20C($a0)
    mtc1 $zero, $f13
    jal func_001C67E0
    lh $a1, 0x20C($a0)
    paddub $v0, $zero, $zero
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
