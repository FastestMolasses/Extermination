// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DF78(int, int, int, int);

asm void func_001A8CE0(void) {
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
    lw $v1, 0x30($s0)
    lwc1 $f1, 0x0($v1)
    c.le.s $f0, $f1
    nop
    .word 0x4500001c
    nop
    lwc1 $f1, 0xB8($s1)
    lwc1 $f0, 0xB8($s0)
    jal func_0011DF78
    sub.s $f12, $f1, $f0
    lw $v1, 0x30($s0)
    lwc1 $f1, 0x8($v1)
    c.le.s $f0, $f1
    nop
    .word 0x45000012
    nop
    lwc1 $f1, 0xB4($s1)
    lwc1 $f0, 0xB4($s0)
    jal func_0011DF78
    sub.s $f12, $f1, $f0
    lw $a0, 0x30($s0)
    lui $v1, (0x3F000000 >> 16)
    mtc1 $v1, $f1
    lwc1 $f2, 0x4($a0)
    add.s $f1, $f1, $f2
    c.le.s $f0, $f1
    nop
    .word 0x45000005
    nop
    addiu $v1, $zero, 0x1
    sb $v1, 0xA($s1)
    lui $at, (0x70003B86 >> 16)
    sh $zero, (0x70003B86 & 0xFFFF)($at)
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
