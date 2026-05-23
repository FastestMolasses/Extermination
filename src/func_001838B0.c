// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001662D0(int, int, int, int);
extern void func_00174A50(int, int, int, int);

asm void func_001838B0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    jal func_001662D0
    paddub $s0, $a0, $zero
    lbu $a0, 0x4($s0)
    addiu $v1, $zero, 0x1
    .word 0x1483000b
    nop
    addiu $v0, $zero, 0x4
    sb $v0, 0x4($s0)
    lui $v0, (0x41000000 >> 16)
    mtc1 $v0, $f12
    sb $zero, 0x5($s0)
    paddub $a0, $s0, $zero
    jal func_00174A50
    sb $zero, 0x6($s0)
    lh $v1, 0x20C($s0)
    sh $v1, 0x1F2($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
