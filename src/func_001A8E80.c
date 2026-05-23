// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DF78(int, int, int, int);
extern void func_0011E748(int, int, int, int);

asm void func_001A8E80(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lwc1 $f3, 0xB0($a1)
    lwc1 $f2, 0xB0($a0)
    lwc1 $f1, 0xB8($a1)
    lwc1 $f0, 0xB8($a0)
    paddub $s1, $a0, $zero
    paddub $s0, $a1, $zero
    sub.s $f2, $f3, $f2
    sub.s $f0, $f1, $f0
    mula.s $f2, $f2
    jal func_0011E748
    madd.s $f12, $f0, $f0
    lw $a0, 0x30($s1)
    lui $v1, (0x40E00000 >> 16)
    mtc1 $v1, $f1
    lwc1 $f2, 0x0($a0)
    add.s $f1, $f1, $f2
    c.le.s $f0, $f1
    nop
    .word 0x45000013
    nop
    lwc1 $f2, 0xB4($s0)
    lui $v0, (0x40C00000 >> 16)
    mtc1 $v0, $f1
    lwc1 $f0, 0xB4($s1)
    add.s $f12, $f1, $f2
    jal func_0011DF78
    sub.s $f12, $f12, $f0
    lui $v1, (0x41000000 >> 16)
    mtc1 $v1, $f1
    nop
    c.le.s $f0, $f1
    nop
    .word 0x45000005
    nop
    addiu $v1, $zero, 0x14
    sh $v1, 0x36($s0)
    lui $at, (0x70003B88 >> 16)
    sh $zero, (0x70003B88 & 0xFFFF)($at)
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
