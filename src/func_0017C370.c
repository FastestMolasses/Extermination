// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001749A0(int, int, int, int);
extern void func_0017B490(int, int, int, int);
extern void func_001B1470(int, int, int, int);

asm void func_0017C370(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    lbu $a0, 0x1F0($a0)
    addiu $v1, $zero, 0x6
    .word 0x10830004
    nop
    addiu $v1, $zero, 0x7
    .word 0x1483001f
    nop
    lbu $v1, 0x1F1($s0)
    addiu $v0, $zero, 0x3
    .word 0x14620008
    nop
    lbu $a2, 0x235($s0)
    addiu $a1, $zero, 0x2
    paddub $a0, $s0, $zero
    jal func_0017B490
    paddub $a3, $zero, $zero
    .word 0x10000006
    nop
    lbu $a2, 0x235($s0)
    addiu $a1, $zero, 0x4
    paddub $a0, $s0, $zero
    jal func_0017B490
    paddub $a3, $zero, $zero
    mtc1 $zero, $f12
    addiu $a2, $zero, 0x1
    paddub $a1, $v0, $zero
    jal func_001749A0
    paddub $a0, $s0, $zero
    lwc1 $f1, 0xC4($s0)
    lui $v0, (0x40490FDB >> 16)
    ori $v0, $v0, (0x40490FDB & 0xFFFF)
    mtc1 $v0, $f0
    jal func_001B1470
    add.s $f12, $f0, $f1
    swc1 $f0, 0xC4($s0)
    lui $v1, (0x41000000 >> 16)
    sw $v1, 0x1FC($s0)
    sw $zero, 0x38($s0)
    sw $zero, 0x240($s0)
    sb $zero, 0x25C($s0)
    sb $zero, 0x25E($s0)
    sw $zero, 0x260($s0)
    sw $zero, 0x264($s0)
    sw $zero, 0x268($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
