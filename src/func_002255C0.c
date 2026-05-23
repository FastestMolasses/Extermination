// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(int, int, int, int);
extern void func_0021C350(int, int, int, int);
extern void func_0021D2E0(int, int, int, int);
extern void func_0021D490(int, int, int, int);

asm void func_002255C0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lbu $a1, 0x6($a0)
    addiu $v1, $zero, 0x2
    .word 0x10a30039
    paddub $s0, $a0, $zero
    addiu $v1, $zero, 0x1
    .word 0x10a30027
    nop
    .word 0x10a00003
    nop
    .word 0x10000036
    lq $ra, 0x10($sp)
    lwc1 $f1, 0x224($s0)
    mtc1 $zero, $f0
    nop
    c.eq.s $f1, $f0
    nop
    .word 0x45010008
    nop
    lui $v0, (0x43960000 >> 16)
    mtc1 $v0, $f12
    addiu $a1, $zero, 0x146
    jal func_001FBD50
    paddub $a2, $zero, $zero
    jal func_0021C350
    paddub $a0, $s0, $zero
    lwc1 $f1, 0x220($s0)
    mtc1 $zero, $f0
    nop
    c.le.s $f1, $f0
    nop
    .word 0x45000020
    nop
    lbu $v1, 0x6($s0)
    lui $v0, (0x43960000 >> 16)
    mtc1 $v0, $f12
    addiu $a1, $zero, 0x156
    paddub $a0, $s0, $zero
    addiu $v0, $v1, 0x1
    sb $v0, 0x6($s0)
    paddub $a2, $zero, $zero
    jal func_001FBD50
    sb $zero, 0x7($s0)
    addiu $v1, $zero, 0x10
    .word 0x10000013
    sh $v1, 0x28($s0)
    lh $a1, 0x28($s0)
    addiu $v1, $a1, -0x1
    .word 0x14a0000f
    sh $v1, 0x28($s0)
    lbu $v0, 0x6($s0)
    addiu $v0, $v0, 0x1
    jal func_0021D490
    sb $v0, 0x6($s0)
    addiu $a0, $zero, 0x1
    addiu $a1, $zero, 0xEE
    addiu $a2, $zero, 0x3C
    jal func_001B61C0
    paddub $a3, $a0, $zero
    .word 0x10000004
    nop
    addiu $a1, $zero, 0x78
    jal func_0021D2E0
    paddub $a2, $zero, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
