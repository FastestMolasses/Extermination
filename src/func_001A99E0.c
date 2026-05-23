// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DE90(int, int, int, int);
extern void func_0011DF78(int, int, int, int);
extern void func_0011E2A8(int, int, int, int);
extern void func_0011E748(int, int, int, int);
extern void func_001B1470(int, int, int, int);

asm void func_001A99E0(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x30($sp)
    sq $s1, 0x20($sp)
    sq $s0, 0x10($sp)
    swc1 $f22, 0x8($sp)
    swc1 $f21, 0x4($sp)
    swc1 $f20, 0x0($sp)
    lwc1 $f3, 0xB0($a0)
    lwc1 $f2, 0xB0($a1)
    lwc1 $f1, 0xB8($a0)
    lwc1 $f0, 0xB8($a1)
    paddub $s1, $a0, $zero
    paddub $s0, $a1, $zero
    sub.s $f20, $f3, $f2
    sub.s $f21, $f1, $f0
    mula.s $f20, $f20
    jal func_0011E748
    madd.s $f12, $f21, $f21
    lw $a0, 0x30($s1)
    lui $v1, (0x42860000 >> 16)
    mtc1 $v1, $f1
    lwc1 $f2, 0x0($a0)
    add.s $f1, $f1, $f2
    c.le.s $f0, $f1
    nop
    .word 0x45000028
    nop
    lwc1 $f1, 0xB4($s1)
    lwc1 $f0, 0xB4($s0)
    jal func_0011DF78
    sub.s $f12, $f1, $f0
    lui $v1, (0x41000000 >> 16)
    mtc1 $v1, $f1
    nop
    c.le.s $f0, $f1
    nop
    .word 0x4500001d
    nop
    jal func_0011DE90
    lwc1 $f12, 0x38($s0)
    lwc1 $f12, 0x38($s0)
    jal func_0011E2A8
    mul.s $f22, $f20, $f0
    mul.s $f1, $f21, $f0
    lwc1 $f0, 0xB0($s0)
    add.s $f1, $f22, $f1
    add.s $f0, $f0, $f1
    swc1 $f0, 0xB0($s1)
    jal func_0011E2A8
    lwc1 $f12, 0x38($s0)
    neg.s $f0, $f0
    lwc1 $f12, 0x38($s0)
    jal func_0011DE90
    mul.s $f20, $f20, $f0
    mul.s $f1, $f21, $f0
    lwc1 $f0, 0xB8($s0)
    add.s $f1, $f20, $f1
    add.s $f0, $f0, $f1
    swc1 $f0, 0xB8($s1)
    lwc1 $f0, 0x38($s0)
    lwc1 $f1, 0xC4($s1)
    jal func_001B1470
    add.s $f12, $f1, $f0
    swc1 $f0, 0xC4($s1)
    lui $at, (0x70003B88 >> 16)
    sh $zero, (0x70003B88 & 0xFFFF)($at)
    lq $ra, 0x30($sp)
    lq $s1, 0x20($sp)
    lq $s0, 0x10($sp)
    lwc1 $f22, 0x8($sp)
    lwc1 $f21, 0x4($sp)
    lwc1 $f20, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
