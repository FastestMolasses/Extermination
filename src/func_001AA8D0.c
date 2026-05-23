// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DF78(int, int, int, int);
extern void func_0011E748(int, int, int, int);
extern void func_001B1470(int, int, int, int);

asm void func_001AA8D0(void) {
    addiu $sp, $sp, -0x60
    sq $ra, 0x50($sp)
    sq $s4, 0x40($sp)
    sq $s3, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lwc1 $f1, 0xA4($a0)
    lwc1 $f0, 0xB4($a1)
    paddub $s2, $a2, $zero
    paddub $s1, $a3, $zero
    paddub $s4, $a0, $zero
    paddub $s3, $a1, $zero
    addiu $s0, $zero, 0x2
    jal func_0011DF78
    sub.s $f12, $f1, $f0
    lui $v0, (0x40400000 >> 16)
    mtc1 $v0, $f1
    nop
    c.le.s $f0, $f1
    nop
    .word 0x45010003
    nop
    .word 0x1000004e
    paddub $v0, $zero, $zero
    lwc1 $f3, 0xB0($s3)
    lui $v0, (0x41500000 >> 16)
    mtc1 $v0, $f2
    lwc1 $f4, 0xA0($s4)
    lwc1 $f1, 0xA8($s4)
    lwc1 $f0, 0xB8($s3)
    add.s $f2, $f2, $f3
    sub.s $f2, $f4, $f2
    sub.s $f0, $f1, $f0
    mula.s $f2, $f2
    jal func_0011E748
    madd.s $f12, $f0, $f0
    lui $v0, (0x40600000 >> 16)
    mtc1 $v0, $f1
    nop
    c.le.s $f0, $f1
    nop
    .word 0x45010018
    addiu $v0, $zero, 0x2
    lwc1 $f3, 0xB0($s3)
    lui $v0, (0x41500000 >> 16)
    mtc1 $v0, $f2
    lwc1 $f4, 0xA0($s4)
    lwc1 $f1, 0xA8($s4)
    lwc1 $f0, 0xB8($s3)
    sub.s $f2, $f3, $f2
    addiu $s0, $zero, 0x3
    sub.s $f2, $f4, $f2
    sub.s $f0, $f1, $f0
    mula.s $f2, $f2
    jal func_0011E748
    madd.s $f12, $f0, $f0
    lui $v0, (0x40600000 >> 16)
    mtc1 $v0, $f1
    nop
    c.le.s $f0, $f1
    nop
    .word 0x45010003
    nop
    .word 0x10000025
    paddub $v0, $zero, $zero
    addiu $v0, $zero, 0x2
    .word 0x1602000e
    nop
    lwc1 $f1, 0xB0($s3)
    lui $v0, (0x4149999A >> 16)
    ori $v0, $v0, (0x4149999A & 0xFFFF)
    mtc1 $v0, $f0
    lui $v0, (0x3F800000 >> 16)
    add.s $f0, $f0, $f1
    swc1 $f0, 0x0($s2)
    lwc1 $f0, 0xA4($s4)
    swc1 $f0, 0x4($s2)
    lwc1 $f0, 0xB8($s3)
    swc1 $f0, 0x8($s2)
    .word 0x1000000d
    sw $v0, 0xC($s2)
    lwc1 $f1, 0xB0($s3)
    lui $v0, (0x4149999A >> 16)
    ori $v0, $v0, (0x4149999A & 0xFFFF)
    mtc1 $v0, $f0
    lui $v0, (0x3F800000 >> 16)
    sub.s $f0, $f1, $f0
    swc1 $f0, 0x0($s2)
    lwc1 $f0, 0xA4($s4)
    swc1 $f0, 0x4($s2)
    lwc1 $f0, 0xB8($s3)
    swc1 $f0, 0x8($s2)
    sw $v0, 0xC($s2)
    lwc1 $f1, 0xC4($s3)
    lui $v0, (0x40490FDB >> 16)
    ori $v0, $v0, (0x40490FDB & 0xFFFF)
    mtc1 $v0, $f0
    jal func_001B1470
    add.s $f12, $f0, $f1
    swc1 $f0, 0x0($s1)
    paddub $v0, $s0, $zero
    lq $ra, 0x50($sp)
    lq $s4, 0x40($sp)
    lq $s3, 0x30($sp)
    lq $s2, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x60
}
