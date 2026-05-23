// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001193A8(int, int, int, int);
extern void func_00119528(int, int, int, int);

asm void func_001194B8(void) {
    addiu $sp, $sp, -0x50
    sd $s3, 0x30($sp)
    sd $s2, 0x20($sp)
    daddu $s3, $a0, $zero
    sd $s1, 0x10($sp)
    daddu $s2, $a2, $zero
    daddu $s1, $a1, $zero
    sd $s0, 0x0($sp)
    sd $ra, 0x40($sp)
    daddu $a1, $s2, $zero
    jal func_00119528
    daddu $a0, $s1, $zero
    daddu $s0, $v0, $zero
    addiu $v0, $zero, -0x1
    .word 0x12020004
    daddu $a0, $s3, $zero
    lw $a2, 0x4($s1)
    jal func_001193A8
    daddu $a1, $s2, $zero
    daddu $v0, $s0, $zero
    ld $ra, 0x40($sp)
    ld $s3, 0x30($sp)
    ld $s2, 0x20($sp)
    ld $s1, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x50
}
