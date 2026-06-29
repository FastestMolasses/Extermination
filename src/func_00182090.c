// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001B12B0(int, int, int, int);

asm void func_00182090(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lbu $v0, 0x23F($a0)
    .word 0x1040000f
    paddub $s0, $a0, $zero
    lwc1 $f13, 0xC4($s0)
    lui $v0, (0x3D8EFA35 >> 16)
    ori $v0, $v0, (0x3D8EFA35 & 0xFFFF)
    mtc1 $v0, $f14
    jal func_001B12B0
    lwc1 $f12, 0x218($s0)
    swc1 $f0, 0xC4($s0)
    lwc1 $f1, 0x218($s0)
    c.eq.s $f0, $f1
    nop
    .word 0x45000004
    paddub $v0, $zero, $zero
    .word 0x10000002
    addiu $v0, $zero, 0x1
    paddub $v0, $zero, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
