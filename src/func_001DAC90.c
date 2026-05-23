// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001281C0(int, int, int, int);

asm void func_001DAC90(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lq $v0, 0x0($a3)
    paddub $s0, $a2, $zero
    paddub $s2, $a0, $zero
    sq $v0, 0x0($a0)
    lw $v0, 0xC($a3)
    .word 0x14400023
    paddub $s1, $a1, $zero
    lwc1 $f3, 0x0($s0)
    mtc1 $zero, $f0
    nop
    c.le.s $f3, $f0
    nop
    .word 0x4501000f
    nop
    mtc1 $v0, $f0
    lwc1 $f2, 0x8($s0)
    cvt.s.w $f1, $f0
    lui $v0, (0x40A66666 >> 16)
    ori $v0, $v0, (0x40A66666 & 0xFFFF)
    mtc1 $v0, $f0
    nop
    mul.s $f0, $f0, $f3
    add.s $f0, $f1, $f0
    jal func_001281C0
    add.s $f12, $f2, $f0
    addiu $v0, $v0, 0x10
    .word 0x1000000f
    sw $v0, 0xC($s2)
    mtc1 $v0, $f0
    lwc1 $f2, 0x8($s0)
    cvt.s.w $f1, $f0
    lui $v0, (0x3F000000 >> 16)
    mtc1 $v0, $f0
    nop
    mul.s $f0, $f0, $f3
    add.s $f0, $f1, $f0
    jal func_001281C0
    add.s $f12, $f2, $f0
    addiu $v0, $v0, 0x10
    .word 0x10000002
    sw $v0, 0xC($s2)
    sw $v0, 0xC($s2)
    lwc1 $f2, 0x0($s0)
    lui $v0, (0x41800000 >> 16)
    mtc1 $v0, $f5
    lui $a3, (0x3F000000 >> 16)
    lui $v0, (0x46000000 >> 16)
    lwc1 $f3, 0x0($s1)
    mul.s $f2, $f5, $f2
    lui $a2, (0x45600000 >> 16)
    add.s $f2, $f3, $f2
    mtc1 $v0, $f1
    mtc1 $a3, $f4
    div.s $f1, $f2, $f1
    lui $v0, (0x40800000 >> 16)
    mtc1 $v0, $f0
    nop
    sub.s $f0, $f1, $f0
    lui $v0, (0x41124925 >> 16)
    ori $v1, $v0, (0x41124925 & 0xFFFF)
    lui $v0, (0x3F800000 >> 16)
    add.s $f0, $f4, $f0
    swc1 $f0, 0x10($s2)
    lwc1 $f2, 0x4($s0)
    lwc1 $f3, 0x4($s1)
    mtc1 $a2, $f1
    mtc1 $v1, $f0
    mul.s $f2, $f5, $f2
    add.s $f2, $f3, $f2
    div.s $f1, $f2, $f1
    sub.s $f0, $f1, $f0
    add.s $f0, $f4, $f0
    swc1 $f0, 0x14($s2)
    sw $v0, 0x18($s2)
    jal func_001281C0
    lwc1 $f12, 0x0($s1)
    sw $v0, 0x20($s2)
    jal func_001281C0
    lwc1 $f12, 0x4($s1)
    sw $v0, 0x24($s2)
    jal func_001281C0
    lwc1 $f12, 0x8($s1)
    addiu $v1, $zero, 0xFF
    sw $v0, 0x28($s2)
    sw $v1, 0x2C($s2)
    lq $ra, 0x30($sp)
    lq $s2, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
