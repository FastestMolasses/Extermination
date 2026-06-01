// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011E0A8(int, int, int, int);
extern void float_to_int(int, int, int, int);

asm void func_0014E890(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lb $v0, 0x5C($a1)
    paddub $s1, $a0, $zero
    .word 0x10400010
    paddub $s0, $a1, $zero
    lwc1 $f2, 0x40($s0)
    lui $v0, (0x3E4CCCCD >> 16)
    ori $v0, $v0, (0x3E4CCCCD & 0xFFFF)
    mtc1 $v0, $f1
    lui $v0, (0x40000000 >> 16)
    add.s $f1, $f2, $f1
    mtc1 $v0, $f0
    nop
    c.lt.s $f1, $f0
    nop
    .word 0x45010011
    swc1 $f1, 0x40($s0)
    sb $zero, 0x5C($s0)
    .word 0x1000000e
    sw $v0, 0x40($s0)
    lwc1 $f2, 0x40($s0)
    lui $v0, (0x3D4CCCCD >> 16)
    ori $v0, $v0, (0x3D4CCCCD & 0xFFFF)
    mtc1 $v0, $f1
    lui $v0, (0x3F800000 >> 16)
    sub.s $f1, $f2, $f1
    mtc1 $v0, $f0
    nop
    c.le.s $f1, $f0
    nop
    .word 0x45000002
    swc1 $f1, 0x40($s0)
    sw $v0, 0x40($s0)
    lwc1 $f0, 0x40($s0)
    lui $v0, (0x45800000 >> 16)
    mtc1 $v0, $f1
    addiu $a0, $sp, 0x3C
    jal func_0011E0A8
    mul.s $f12, $f1, $f0
    jal float_to_int
    lwc1 $f12, 0x3C($sp)
    lw $v1, 0x13C($s1)
    sh $v0, 0x8A($v1)
    jal float_to_int
    lwc1 $f12, 0x3C($sp)
    lw $a1, 0x13C($s1)
    lui $v1, (0x3F800000 >> 16)
    mtc1 $v1, $f2
    addiu $a0, $sp, 0x3C
    lui $v1, (0x45800000 >> 16)
    sh $v0, 0x8C($a1)
    lwc1 $f1, 0x40($s0)
    mtc1 $v1, $f0
    nop
    div.s $f1, $f2, $f1
    nop
    nop
    jal func_0011E0A8
    mul.s $f12, $f0, $f1
    jal float_to_int
    lwc1 $f12, 0x3C($sp)
    lw $v1, 0x140($s1)
    sh $v0, 0x8A($v1)
    jal float_to_int
    lwc1 $f12, 0x3C($sp)
    lw $v1, 0x140($s1)
    sh $v0, 0x8C($v1)
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
