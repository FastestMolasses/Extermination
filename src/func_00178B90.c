// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DE90(int, int, int, int);
extern void func_0011DF78(int, int, int, int);
extern void func_0011E2A8(int, int, int, int);
extern void func_001281C0(int, int, int, int);
extern void func_001764E0(int, int, int, int);

asm void func_00178B90(void) {
    addiu $sp, $sp, -0x60
    sq $ra, 0x50($sp)
    sq $s3, 0x40($sp)
    sq $s2, 0x30($sp)
    sq $s1, 0x20($sp)
    sq $s0, 0x10($sp)
    swc1 $f23, 0xC($sp)
    swc1 $f22, 0x8($sp)
    swc1 $f21, 0x4($sp)
    swc1 $f20, 0x0($sp)
    lbu $v0, 0x25F($a0)
    lui $v1, (0x3F800000 >> 16)
    mtc1 $v1, $f20
    paddub $s0, $a1, $zero
    .word 0x14400022
    paddub $s1, $a0, $zero
    lbu $v1, 0x23B($s1)
    addiu $v0, $zero, 0x35
    .word 0x1462001e
    nop
    lwc1 $f1, 0x310($s1)
    lwc1 $f0, 0xC4($s1)
    jal func_0011DF78
    sub.s $f12, $f1, $f0
    lui $v0, (0x3FC90FDB >> 16)
    mov.s $f21, $f0
    ori $v0, $v0, (0x3FC90FDB & 0xFFFF)
    mtc1 $v0, $f0
    nop
    c.lt.s $f21, $f0
    nop
    .word 0x45000011
    nop
    jal func_0011DE90
    lwc1 $f12, 0x9C($s1)
    lui $v0, (0x3F800000 >> 16)
    mtc1 $v0, $f1
    lwc1 $f12, 0x9C($s1)
    sub.s $f1, $f1, $f0
    lui $v0, (0x3FC90FDB >> 16)
    ori $v0, $v0, (0x3FC90FDB & 0xFFFF)
    mtc1 $v0, $f0
    nop
    div.s $f0, $f1, $f0
    nop
    nop
    jal func_0011DE90
    mul.s $f20, $f21, $f0
    add.s $f20, $f0, $f20
    jal func_0011DF78
    lwc1 $f12, 0x38($s1)
    lui $v0, (0x40900000 >> 16)
    mtc1 $v0, $f1
    nop
    c.lt.s $f0, $f1
    nop
    .word 0x45000029
    nop
    lbu $v0, 0x25F($s1)
    .word 0x14400012
    nop
    jal func_0011E2A8
    lwc1 $f12, 0xC4($s1)
    lwc1 $f2, 0x38($s1)
    lwc1 $f1, 0xB0($s1)
    mul.s $f2, $f2, $f20
    mul.s $f0, $f2, $f0
    add.s $f0, $f1, $f0
    swc1 $f0, 0xB0($s1)
    jal func_0011DE90
    lwc1 $f12, 0xC4($s1)
    lwc1 $f2, 0x38($s1)
    lwc1 $f1, 0xB8($s1)
    mul.s $f2, $f2, $f20
    mul.s $f0, $f2, $f0
    add.s $f0, $f1, $f0
    .word 0x1000000f
    swc1 $f0, 0xB8($s1)
    jal func_0011E2A8
    lwc1 $f12, 0xC4($s1)
    lwc1 $f2, 0x38($s1)
    lwc1 $f1, 0xB0($s1)
    mul.s $f0, $f2, $f0
    add.s $f0, $f1, $f0
    swc1 $f0, 0xB0($s1)
    jal func_0011DE90
    lwc1 $f12, 0xC4($s1)
    lwc1 $f2, 0x38($s1)
    lwc1 $f1, 0xB8($s1)
    mul.s $f0, $f2, $f0
    add.s $f0, $f1, $f0
    swc1 $f0, 0xB8($s1)
    .word 0x12000064
    nop
    jal func_001764E0
    paddub $a0, $s1, $zero
    .word 0x10000061
    lq $ra, 0x50($sp)
    lwc1 $f23, 0x38($s1)
    mtc1 $zero, $f0
    nop
    c.lt.s $f23, $f0
    nop
    .word 0x45010006
    lui $v0, (0xC0800000 >> 16)
    lui $v0, (0x40800000 >> 16)
    mtc1 $v0, $f21
    .word 0x10000004
    nop
    lui $v0, (0xC0800000 >> 16)
    mtc1 $v0, $f21
    nop
    nop
    div.s $f12, $f23, $f21
    nop
    nop
    jal func_001281C0
    nop
    paddub $s3, $v0, $zero
    slt $at, $zero, $s3
    .word 0x10200027
    paddub $s2, $zero, $zero
    mul.s $f22, $f21, $f20
    lbu $v0, 0x25F($s1)
    .word 0x1440000e
    nop
    jal func_0011E2A8
    lwc1 $f12, 0xC4($s1)
    mul.s $f1, $f22, $f0
    lwc1 $f0, 0xB0($s1)
    add.s $f0, $f0, $f1
    swc1 $f0, 0xB0($s1)
    jal func_0011DE90
    lwc1 $f12, 0xC4($s1)
    mul.s $f1, $f22, $f0
    lwc1 $f0, 0xB8($s1)
    add.s $f0, $f0, $f1
    .word 0x1000000e
    swc1 $f0, 0xB8($s1)
    jal func_0011E2A8
    lwc1 $f12, 0xC4($s1)
    mul.s $f1, $f21, $f0
    lwc1 $f0, 0xB0($s1)
    add.s $f0, $f0, $f1
    swc1 $f0, 0xB0($s1)
    jal func_0011DE90
    lwc1 $f12, 0xC4($s1)
    mul.s $f1, $f21, $f0
    lwc1 $f0, 0xB8($s1)
    add.s $f0, $f0, $f1
    swc1 $f0, 0xB8($s1)
    nop
    .word 0x12000003
    sub.s $f23, $f23, $f21
    jal func_001764E0
    paddub $a0, $s1, $zero
    addiu $s2, $s2, 0x1
    slt $v0, $s2, $s3
    .word 0x1440ffdc
    nop
    lbu $v0, 0x25F($s1)
    .word 0x1440000f
    nop
    jal func_0011E2A8
    lwc1 $f12, 0xC4($s1)
    mul.s $f20, $f23, $f20
    lwc1 $f1, 0xB0($s1)
    mul.s $f0, $f20, $f0
    add.s $f0, $f1, $f0
    swc1 $f0, 0xB0($s1)
    jal func_0011DE90
    lwc1 $f12, 0xC4($s1)
    mul.s $f1, $f20, $f0
    lwc1 $f0, 0xB8($s1)
    add.s $f0, $f0, $f1
    .word 0x1000000d
    swc1 $f0, 0xB8($s1)
    jal func_0011E2A8
    lwc1 $f12, 0xC4($s1)
    mul.s $f1, $f23, $f0
    lwc1 $f0, 0xB0($s1)
    add.s $f0, $f0, $f1
    swc1 $f0, 0xB0($s1)
    jal func_0011DE90
    lwc1 $f12, 0xC4($s1)
    mul.s $f1, $f23, $f0
    lwc1 $f0, 0xB8($s1)
    add.s $f0, $f0, $f1
    swc1 $f0, 0xB8($s1)
    .word 0x12000003
    nop
    jal func_001764E0
    paddub $a0, $s1, $zero
    lq $ra, 0x50($sp)
    lq $s3, 0x40($sp)
    lq $s2, 0x30($sp)
    lq $s1, 0x20($sp)
    lq $s0, 0x10($sp)
    lwc1 $f23, 0xC($sp)
    lwc1 $f22, 0x8($sp)
    lwc1 $f21, 0x4($sp)
    lwc1 $f20, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x60
}
