// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00121AF8(int, int, int, int);
extern void func_00121E30(int, int, int, int);
extern void func_00121EB8(int, int, int, int);

asm void func_00122868(void) {
    addiu $sp, $sp, -0x80
    sd $s0, 0x10($sp)
    daddu $s0, $a1, $zero
    sd $s5, 0x60($sp)
    sd $s4, 0x50($sp)
    daddu $s5, $a3, $zero
    sd $s3, 0x40($sp)
    daddu $s4, $a2, $zero
    sd $s2, 0x30($sp)
    sd $s1, 0x20($sp)
    sd $ra, 0x70($sp)
    jal func_00121AF8
    addiu $a1, $zero, 0x1
    daddu $s1, $v0, $zero
    dsra32 $a1, $s0, 0
    lui $v0, (0x7FFFFFFF >> 16)
    lui $v1, (0xFFFFF >> 16)
    ori $v0, $v0, (0x7FFFFFFF & 0xFFFF)
    lui $a0, (0xFFFF0000 >> 16)
    dsrl32 $a0, $a0, 0
    and $v0, $a1, $v0
    and $s0, $s0, $a0
    dsll32 $v0, $v0, 0
    ori $v1, $v1, (0xFFFFF & 0xFFFF)
    or $s0, $s0, $v0
    and $a1, $a1, $v1
    addiu $s3, $s1, 0x14
    dsrl32 $s2, $s0, 20
    .word 0x12400004
    sw $a1, 0x4($sp)
    lui $v0, (0x100000 >> 16)
    or $v0, $a1, $v0
    sw $v0, 0x4($sp)
    dsll32 $v0, $s0, 0
    dsra32 $v0, $v0, 0
    .word 0x10400019
    sw $v0, 0x0($sp)
    jal func_00121EB8
    daddu $a0, $sp, $zero
    daddu $a1, $v0, $zero
    .word 0x10a0000a
    lw $v0, 0x4($sp)
    negu $a0, $a1
    lw $v1, 0x0($sp)
    sllv $v0, $v0, $a0
    or $v1, $v1, $v0
    sw $v1, 0x14($s1)
    lw $v0, 0x4($sp)
    srlv $v0, $v0, $a1
    .word 0x10000003
    sw $v0, 0x4($sp)
    lw $v0, 0x0($sp)
    sw $v0, 0x14($s1)
    lw $v0, 0x4($sp)
    addiu $v1, $zero, 0x1
    addiu $a0, $zero, 0x2
    movn $v1, $a0, $v0
    sw $v0, 0x4($s3)
    daddu $s0, $v1, $zero
    .word 0x10000009
    sw $v1, 0x10($s1)
    ori $a0, $sp, 0x4
    jal func_00121EB8
    addiu $s0, $zero, 0x1
    lw $a0, 0x4($sp)
    addiu $v1, $zero, 0x1
    sw $v1, 0x10($s1)
    addiu $a1, $v0, 0x20
    sw $a0, 0x14($s1)
    .word 0x12400006
    addiu $v0, $a1, -0x433
    addiu $v1, $zero, 0x35
    addu $v0, $s2, $v0
    subu $v1, $v1, $a1
    .word 0x10000009
    sw $v0, 0x0($s4)
    addiu $v1, $a1, -0x432
    sll $v0, $s0, 2
    sw $v1, 0x0($s4)
    addu $v0, $v0, $s3
    jal func_00121E30
    lw $a0, -0x4($v0)
    sll $v1, $s0, 5
    subu $v1, $v1, $v0
    sw $v1, 0x0($s5)
    daddu $v0, $s1, $zero
    ld $ra, 0x70($sp)
    ld $s5, 0x60($sp)
    ld $s4, 0x50($sp)
    ld $s3, 0x40($sp)
    ld $s2, 0x30($sp)
    ld $s1, 0x20($sp)
    ld $s0, 0x10($sp)
    jr $ra
    addiu $sp, $sp, 0x80
}
