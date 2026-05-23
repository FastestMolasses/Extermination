// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DF78(int, int, int, int);
extern void func_0011E748(int, int, int, int);

asm void func_001BE5F0(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lui $at, (0x70003B8D >> 16)
    lbu $v0, (0x70003B8D & 0xFFFF)($at)
    paddub $s2, $a0, $zero
    paddub $s1, $a1, $zero
    .word 0x10400003
    paddub $s0, $a2, $zero
    .word 0x1000001f
    paddub $v0, $zero, $zero
    lwc1 $f3, 0xA0($s2)
    lwc1 $f2, 0xB0($s1)
    lwc1 $f1, 0xA8($s2)
    lwc1 $f0, 0xB8($s1)
    sub.s $f2, $f3, $f2
    sub.s $f0, $f1, $f0
    mula.s $f2, $f2
    jal func_0011E748
    madd.s $f12, $f0, $f0
    lw $v0, 0x18($s0)
    lwc1 $f1, 0x0($v0)
    c.le.s $f0, $f1
    nop
    .word 0x45010003
    nop
    .word 0x1000000e
    paddub $v0, $zero, $zero
    lwc1 $f1, 0xA4($s2)
    lwc1 $f0, 0xB4($s1)
    jal func_0011DF78
    sub.s $f12, $f1, $f0
    lw $v0, 0x18($s0)
    lwc1 $f1, 0x4($v0)
    c.le.s $f0, $f1
    nop
    .word 0x45010004
    addiu $v0, $zero, 0x1
    .word 0x10000002
    paddub $v0, $zero, $zero
    addiu $v0, $zero, 0x1
    lq $ra, 0x30($sp)
    lq $s2, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
