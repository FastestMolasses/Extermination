// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011FEE8(int, int, int, int);
extern void func_00120058(int, int, int, int);

asm void func_0011FF58(void) {
    addiu $sp, $sp, -0x30
    sd $s1, 0x10($sp)
    sd $ra, 0x20($sp)
    daddu $s1, $a0, $zero
    sd $s0, 0x0($sp)
    lw $v0, 0x38($s1)
    .word 0x14400007
    addiu $s0, $s1, 0x1D8
    jal func_00120058
    nop
    .word 0x10000004
    lw $v1, 0x4($s0)
    daddu $v1, $v0, $zero
    daddu $s0, $v1, $zero
    lw $v1, 0x4($s0)
    addiu $v1, $v1, -0x1
    .word 0x04600006
    lw $a0, 0x8($s0)
    lh $v0, 0xC($a0)
    .word 0x1040000f
    addiu $v1, $v1, -0x1
    .word 0x0461fffc
    addiu $a0, $a0, 0x58
    lw $v0, 0x0($s0)
    .word 0x1440fff4
    daddu $v1, $v0, $zero
    daddu $a0, $s1, $zero
    jal func_0011FEE8
    addiu $a1, $zero, 0x4
    .word 0x1440ffee
    sw $v0, 0x0($s0)
    addiu $v1, $zero, 0xC
    daddu $v0, $zero, $zero
    .word 0x10000011
    sw $v1, 0x0($s1)
    addiu $v0, $zero, 0x1
    addiu $v1, $zero, -0x1
    sh $v0, 0xC($a0)
    sh $v1, 0xE($a0)
    daddu $v0, $a0, $zero
    sw $s1, 0x54($a0)
    sw $zero, 0x0($a0)
    sw $zero, 0x8($a0)
    sw $zero, 0x4($a0)
    sw $zero, 0x10($a0)
    sw $zero, 0x14($a0)
    sw $zero, 0x18($a0)
    sw $zero, 0x30($a0)
    sw $zero, 0x34($a0)
    sw $zero, 0x44($a0)
    sw $zero, 0x48($a0)
    ld $ra, 0x20($sp)
    ld $s1, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
