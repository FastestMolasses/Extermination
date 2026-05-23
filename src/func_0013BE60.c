// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DE90(int, int, int, int);
extern void func_0011E2A8(int, int, int, int);

asm void func_0013BE60(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lb $v1, 0x82($a1)
    paddub $s1, $a0, $zero
    .word 0x14600024
    paddub $s0, $a1, $zero
    lbu $a0, 0x4($s1)
    addiu $v1, $zero, 0x1
    .word 0x14830007
    nop
    lbu $a0, 0x5($s1)
    addiu $v1, $zero, 0x3
    .word 0x14830003
    nop
    .word 0x1000001b
    lq $ra, 0x20($sp)
    lwc1 $f1, 0x48($s0)
    lwc1 $f0, 0x44($s0)
    add.s $f0, $f0, $f1
    swc1 $f0, 0x44($s0)
    jal func_0011E2A8
    lwc1 $f12, 0x50($s0)
    lwc1 $f2, 0x4C($s0)
    lwc1 $f1, 0xB4($s1)
    mul.s $f0, $f2, $f0
    add.s $f0, $f1, $f0
    swc1 $f0, 0xB4($s1)
    jal func_0011E2A8
    lwc1 $f12, 0xC4($s1)
    lwc1 $f2, 0x44($s0)
    lwc1 $f1, 0xB0($s1)
    mul.s $f0, $f2, $f0
    add.s $f0, $f1, $f0
    swc1 $f0, 0xB0($s1)
    jal func_0011DE90
    lwc1 $f12, 0xC4($s1)
    lwc1 $f2, 0x44($s0)
    lwc1 $f1, 0xB8($s1)
    mul.s $f0, $f2, $f0
    add.s $f0, $f1, $f0
    swc1 $f0, 0xB8($s1)
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
