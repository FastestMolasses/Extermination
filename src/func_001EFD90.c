// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00102948(int, int, int, int);
extern void func_001EF9D0(int, int, int, int);

asm void func_001EFD90(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lwc1 $f12, 0xC($a2)
    paddub $s2, $a1, $zero
    jal func_001EF9D0
    paddub $s1, $a2, $zero
    paddub $s0, $v0, $zero
    .word 0x1200000a
    paddub $v0, $s0, $zero
    addiu $a0, $s0, 0xB0
    jal func_00102948
    paddub $a1, $s2, $zero
    addiu $a0, $s0, 0xC0
    jal func_00102948
    paddub $a1, $s1, $zero
    lui $v0, (0x3F800000 >> 16)
    sw $v0, 0xBC($s0)
    paddub $v0, $s0, $zero
    lq $ra, 0x30($sp)
    lq $s2, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
