// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DF78(int, int, int, int);

asm void func_001AA640(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lwc1 $f1, 0xB0($a0)
    lwc1 $f0, 0xB0($a1)
    paddub $s1, $a0, $zero
    paddub $s0, $a1, $zero
    jal func_0011DF78
    sub.s $f12, $f1, $f0
    lui $v1, (0x42480000 >> 16)
    mtc1 $v1, $f1
    nop
    c.le.s $f0, $f1
    nop
    .word 0x45000019
    nop
    lwc1 $f1, 0xB8($s1)
    lwc1 $f0, 0xB8($s0)
    jal func_0011DF78
    sub.s $f12, $f1, $f0
    lui $v1, (0x42380000 >> 16)
    mtc1 $v1, $f1
    nop
    c.le.s $f0, $f1
    nop
    .word 0x4500000e
    nop
    lwc1 $f1, 0xB4($s1)
    lwc1 $f0, 0xB4($s0)
    jal func_0011DF78
    sub.s $f12, $f1, $f0
    lui $v1, (0x42200000 >> 16)
    mtc1 $v1, $f1
    nop
    c.le.s $f0, $f1
    nop
    .word 0x45000003
    nop
    addiu $v1, $zero, 0x1
    sh $v1, 0x36($s0)
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
