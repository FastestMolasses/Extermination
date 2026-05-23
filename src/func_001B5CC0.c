// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011E748(int, int, int, int);

asm void func_001B5CC0(void) {
    andi $v0, $a0, 0xFF
    addiu $v0, $v0, -0x80
    mult $v1, $v0, $v0
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    andi $v0, $a1, 0xFF
    addiu $v0, $v0, -0x80
    mult1 $v0, $v0, $v0
    addu $v0, $v1, $v0
    mtc1 $v0, $f0
    jal func_0011E748
    cvt.s.w $f12, $f0
    lui $v0, (0x42400000 >> 16)
    mtc1 $v0, $f1
    nop
    c.le.s $f0, $f1
    nop
    .word 0x45000004
    lui $v0, (0x42B00000 >> 16)
    .word 0x10000014
    paddub $v0, $zero, $zero
    lui $v0, (0x42B00000 >> 16)
    mtc1 $v0, $f1
    nop
    c.le.s $f0, $f1
    nop
    .word 0x45000004
    lui $v0, (0x42F40000 >> 16)
    .word 0x1000000b
    addiu $v0, $zero, 0x1
    lui $v0, (0x42F40000 >> 16)
    mtc1 $v0, $f1
    nop
    c.le.s $f0, $f1
    nop
    .word 0x45000004
    addiu $v0, $zero, 0x3
    .word 0x10000002
    addiu $v0, $zero, 0x2
    addiu $v0, $zero, 0x3
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
