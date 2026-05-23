// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DF78(int, int, int, int);

asm void func_001B34F0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lwc1 $f4, 0x0($a2)
    lwc1 $f3, 0x0($a0)
    lwc1 $f1, 0x0($a1)
    lwc1 $f2, 0x8($a2)
    lwc1 $f0, 0x8($a0)
    sub.s $f3, $f4, $f3
    mul.s $f1, $f1, $f1
    sub.s $f0, $f2, $f0
    mula.s $f3, $f3
    madd.s $f0, $f0, $f0
    c.lt.s $f1, $f0
    nop
    .word 0x45000003
    paddub $s0, $a1, $zero
    .word 0x1000000d
    paddub $v0, $zero, $zero
    lwc1 $f1, 0x4($a2)
    lwc1 $f0, 0x4($a0)
    jal func_0011DF78
    sub.s $f12, $f1, $f0
    lwc1 $f1, 0x4($s0)
    c.le.s $f0, $f1
    nop
    .word 0x45010004
    addiu $v0, $zero, 0x1
    .word 0x10000002
    paddub $v0, $zero, $zero
    addiu $v0, $zero, 0x1
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
