// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DE90(int, int, int, int);
extern void func_0011E2A8(int, int, int, int);
extern void func_0011E620(int, int, int, int);
extern void func_0011E748(int, int, int, int);
extern void func_001B1470(int, int, int, int);

asm void func_001AA000(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x30($sp)
    sq $s1, 0x20($sp)
    sq $s0, 0x10($sp)
    swc1 $f22, 0x8($sp)
    swc1 $f21, 0x4($sp)
    swc1 $f20, 0x0($sp)
    paddub $s1, $a0, $zero
    lw $a0, 0xE4($a2)
    lw $v1, 0xE4($a3)
    .word 0x1483003b
    paddub $s0, $a1, $zero
    addiu $v1, $zero, 0x200
    .word 0x14830004
    nop
    lui $at, (0x70003B86 >> 16)
    .word 0x10000035
    sh $zero, (0x70003B86 & 0xFFFF)($at)
    lwc1 $f3, 0xB0($s1)
    lwc1 $f2, 0xB0($s0)
    lwc1 $f1, 0xB8($s1)
    lwc1 $f0, 0xB8($s0)
    sub.s $f20, $f3, $f2
    sub.s $f21, $f1, $f0
    mula.s $f20, $f20
    jal func_0011E748
    madd.s $f12, $f21, $f21
    lw $a0, 0x30($s1)
    lw $v1, 0x30($s0)
    lwc1 $f2, 0x0($a0)
    lwc1 $f1, 0x0($v1)
    add.s $f22, $f2, $f1
    c.le.s $f0, $f22
    nop
    .word 0x45000023
    nop
    lwc1 $f1, 0xB4($s1)
    lwc1 $f0, 0xB4($s0)
    sub.s $f0, $f1, $f0
    jal func_0011E748
    mul.s $f12, $f0, $f0
    lw $a0, 0x30($s1)
    lw $v1, 0x30($s0)
    lwc1 $f2, 0x4($a0)
    lwc1 $f1, 0x4($v1)
    add.s $f1, $f2, $f1
    c.le.s $f0, $f1
    nop
    .word 0x45000015
    nop
    mov.s $f12, $f20
    jal func_0011E620
    mov.s $f13, $f21
    jal func_001B1470
    mov.s $f12, $f0
    mov.s $f20, $f0
    jal func_0011DE90
    mov.s $f12, $f20
    mov.s $f12, $f20
    jal func_0011E2A8
    mul.s $f20, $f22, $f0
    mul.s $f1, $f22, $f0
    lui $at, (0x70003B86 >> 16)
    lwc1 $f0, 0xB0($s0)
    add.s $f0, $f0, $f1
    swc1 $f0, 0xB0($s1)
    lwc1 $f0, 0xB8($s0)
    add.s $f0, $f0, $f20
    swc1 $f0, 0xB8($s1)
    sh $zero, (0x70003B86 & 0xFFFF)($at)
    lq $ra, 0x30($sp)
    lq $s1, 0x20($sp)
    lq $s0, 0x10($sp)
    lwc1 $f22, 0x8($sp)
    lwc1 $f21, 0x4($sp)
    lwc1 $f20, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
