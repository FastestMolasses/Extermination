// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001B61C0(int, int, int, int);

asm void func_0015D000(void) {
    addiu $sp, $sp, -0x20
    mtc1 $zero, $f0
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lwc1 $f1, 0x220($a0)
    c.eq.s $f1, $f0
    nop
    .word 0x45010032
    paddub $s0, $a0, $zero
    lui $v1, (0x41200000 >> 16)
    mtc1 $v1, $f0
    nop
    c.le.s $f1, $f0
    nop
    .word 0x45000013
    lui $v1, (0x420C0000 >> 16)
    lh $v1, 0x210($s0)
    .word 0x14600006
    nop
    addiu $a1, $zero, 0xE0
    addiu $a2, $zero, 0x4
    paddub $a0, $zero, $zero
    jal func_001B61C0
    paddub $a3, $zero, $zero
    lh $v1, 0x210($s0)
    addiu $v1, $v1, 0x1
    sh $v1, 0x210($s0)
    lh $v1, 0x210($s0)
    slti $at, $v1, 0x3D
    .word 0x1420001c
    nop
    .word 0x1000001a
    sh $zero, 0x210($s0)
    lui $v1, (0x420C0000 >> 16)
    mtc1 $v1, $f0
    nop
    c.le.s $f1, $f0
    nop
    .word 0x45000012
    nop
    lh $v1, 0x210($s0)
    .word 0x14600006
    nop
    addiu $a1, $zero, 0xD0
    addiu $a2, $zero, 0x4
    paddub $a0, $zero, $zero
    jal func_001B61C0
    paddub $a3, $zero, $zero
    lh $v1, 0x210($s0)
    addiu $v1, $v1, 0x1
    sh $v1, 0x210($s0)
    lh $v1, 0x210($s0)
    slti $at, $v1, 0x79
    .word 0x14200004
    nop
    .word 0x10000002
    sh $zero, 0x210($s0)
    sh $zero, 0x210($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
