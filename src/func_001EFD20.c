// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00102948(int, int, int, int);
extern void func_001EF9D0(int, int, int, int);

asm void func_001EFD20(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    lui $v0, (0x3F800000 >> 16)
    mtc1 $v0, $f12
    paddub $s1, $a1, $zero
    jal func_001EF9D0
    sq $s0, 0x0($sp)
    paddub $s0, $v0, $zero
    .word 0x1200000b
    paddub $v0, $s0, $zero
    addiu $a0, $s0, 0xB0
    jal func_00102948
    paddub $a1, $s1, $zero
    sw $zero, 0xC4($s0)
    sw $zero, 0xC0($s0)
    sw $zero, 0xCC($s0)
    sw $zero, 0xC8($s0)
    lui $v0, (0x3F800000 >> 16)
    sw $v0, 0xBC($s0)
    paddub $v0, $s0, $zero
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
