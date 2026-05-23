// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DE90(int, int, int, int);
extern void func_0011E2A8(int, int, int, int);

asm void func_0014CD30(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lb $v1, 0x40($a1)
    paddub $s1, $a0, $zero
    .word 0x1460001b
    paddub $s0, $a1, $zero
    lwc1 $f1, 0x28($s0)
    lwc1 $f0, 0x24($s0)
    add.s $f0, $f0, $f1
    swc1 $f0, 0x24($s0)
    lwc1 $f1, 0x30($s0)
    lwc1 $f0, 0x2C($s0)
    add.s $f0, $f0, $f1
    swc1 $f0, 0x2C($s0)
    lwc1 $f1, 0x24($s0)
    lwc1 $f0, 0xB4($s1)
    add.s $f0, $f0, $f1
    swc1 $f0, 0xB4($s1)
    jal func_0011E2A8
    lwc1 $f12, 0xC4($s1)
    lwc1 $f2, 0x2C($s0)
    lwc1 $f1, 0xB0($s1)
    mul.s $f0, $f2, $f0
    add.s $f0, $f1, $f0
    swc1 $f0, 0xB0($s1)
    jal func_0011DE90
    lwc1 $f12, 0xC4($s1)
    lwc1 $f2, 0x2C($s0)
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
