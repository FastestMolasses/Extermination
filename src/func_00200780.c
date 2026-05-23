// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00112440(int, int, int, int);
extern void func_00113280(int, int, int, int);

asm void func_00200780(void) {
    addiu $sp, $sp, -0x60
    sq $ra, 0x40($sp)
    sq $s3, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    addiu $v0, $zero, 0x1
    sb $zero, 0x5C($sp)
    sb $v0, 0x5D($sp)
    paddub $s1, $a0, $zero
    paddub $s0, $a1, $zero
    .word 0x04e10005
    sb $zero, 0x5E($sp)
    lw $v0, 0x4($s1)
    addiu $v0, $v0, 0x7FF
    .word 0x10000003
    srl $s2, $v0, 11
    addiu $v0, $a3, 0x7FF
    sra $s2, $v0, 11
    sra $s3, $a2, 11
    jal func_00113280
    paddub $a0, $zero, $zero
    .word 0x10000004
    lw $v0, 0x0($s1)
    jal func_00113280
    paddub $a0, $zero, $zero
    lw $v0, 0x0($s1)
    addiu $a3, $sp, 0x5C
    paddub $a1, $s2, $zero
    paddub $a2, $s0, $zero
    jal func_00112440
    addu $a0, $v0, $s3
    .word 0x1040fff7
    nop
    sll $v0, $s2, 11
    lq $ra, 0x40($sp)
    lq $s3, 0x30($sp)
    lq $s2, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x60
}
