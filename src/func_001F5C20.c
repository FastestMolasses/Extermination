// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001F5640(int, int, int, int);
extern void func_001F5940(int, int, int, int);

asm void func_001F5C20(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    jal func_001F5640
    sq $s0, 0x0($sp)
    paddub $s0, $v0, $zero
    .word 0x12000014
    nop
    .word 0x1000000f
    paddub $s1, $zero, $zero
    lwc1 $f0, 0xC($s0)
    lui $v0, (0x3F800000 >> 16)
    addiu $a1, $sp, 0x30
    swc1 $f0, 0x30($sp)
    lwc1 $f0, 0x10($s0)
    swc1 $f0, 0x34($sp)
    lwc1 $f0, 0x14($s0)
    swc1 $f0, 0x38($sp)
    sw $v0, 0x3C($sp)
    lh $a0, 0x4($s0)
    jal func_001F5940
    paddub $a2, $s1, $zero
    addiu $s0, $s0, 0x28
    addiu $s1, $s1, 0x1
    lh $v1, 0x0($s0)
    .word 0x0461fff0
    nop
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
