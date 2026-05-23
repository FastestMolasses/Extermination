// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DF78(int, int, int, int);

asm void func_001CF870(void) {
    addiu $sp, $sp, -0x60
    sq $ra, 0x50($sp)
    sq $s4, 0x40($sp)
    sq $s3, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lwc1 $f12, 0x4C($a0)
    paddub $s0, $a1, $zero
    paddub $s2, $a2, $zero
    paddub $s4, $a0, $zero
    jal func_0011DF78
    paddub $s1, $zero, $zero
    sll $s3, $s2, 2
    addu $v0, $s3, $s4
    lwc1 $f1, 0x40($v0)
    c.le.s $f1, $f0
    nop
    .word 0x45010003
    addiu $s2, $v0, 0x40
    ori $v0, $s1, 0x1
    andi $s1, $v0, 0xFF
    jal func_0011DF78
    lwc1 $f12, 0x4C($s4)
    neg.s $f1, $f0
    lwc1 $f0, 0x0($s2)
    c.lt.s $f0, $f1
    nop
    .word 0x45000003
    nop
    ori $v0, $s1, 0x2
    andi $s1, $v0, 0xFF
    jal func_0011DF78
    lwc1 $f12, 0x4C($s0)
    addu $v0, $s3, $s0
    lwc1 $f1, 0x40($v0)
    c.le.s $f1, $f0
    nop
    .word 0x45010003
    addiu $s2, $v0, 0x40
    ori $v0, $s1, 0x10
    andi $s1, $v0, 0xFF
    jal func_0011DF78
    lwc1 $f12, 0x4C($s0)
    neg.s $f1, $f0
    lwc1 $f0, 0x0($s2)
    c.lt.s $f0, $f1
    nop
    .word 0x45000004
    paddub $v0, $s1, $zero
    ori $v0, $s1, 0x20
    andi $s1, $v0, 0xFF
    paddub $v0, $s1, $zero
    lq $ra, 0x50($sp)
    lq $s4, 0x40($sp)
    lq $s3, 0x30($sp)
    lq $s2, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x60
}
