// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001662D0(int, int, int, int);

asm void func_00183440(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    addiu $v0, $zero, 0x2
    sb $v0, 0x23F($a0)
    addiu $v0, $zero, 0x1
    paddub $s0, $a0, $zero
    jal func_001662D0
    sw $v0, 0x24C($a0)
    lw $v1, 0x200($s0)
    andi $v1, $v1, 0x1000
    .word 0x10600015
    nop
    lwc1 $f2, 0x268($s0)
    lui $v1, (0x3F800000 >> 16)
    mtc1 $v1, $f1
    lui $v1, (0x40800000 >> 16)
    mtc1 $v1, $f0
    nop
    add.s $f1, $f2, $f1
    c.lt.s $f1, $f0
    nop
    .word 0x4501000a
    swc1 $f1, 0x268($s0)
    lui $at, (0x70003B8D >> 16)
    addiu $v1, $zero, 0x1
    sb $zero, (0x70003B8D & 0xFFFF)($at)
    sb $v1, 0x4($s0)
    addiu $v1, $zero, 0xC
    sb $v1, 0x5($s0)
    addiu $v1, $zero, 0x17
    sb $zero, 0x6($s0)
    sb $v1, 0x1F0($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
