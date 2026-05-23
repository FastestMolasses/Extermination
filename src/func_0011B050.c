// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011ADD0(int, int, int, int);
extern void func_0011AE40(int, int, int, int);

asm void func_0011B050(void) {
    addiu $sp, $sp, -0x130
    sd $s2, 0xB0($sp)
    sd $ra, 0x120($sp)
    daddu $s2, $a0, $zero
    sd $fp, 0x110($sp)
    sd $s7, 0x100($sp)
    sd $s6, 0xF0($sp)
    sd $s5, 0xE0($sp)
    sd $s4, 0xD0($sp)
    sd $s3, 0xC0($sp)
    sd $s1, 0xA0($sp)
    jal func_0011ADD0
    sd $s0, 0x90($sp)
    .word 0x14400049
    daddu $v0, $zero, $zero
    addiu $v0, $sp, 0x50
    daddu $s1, $zero, $zero
    sw $v0, 0x84($sp)
    addiu $s4, $sp, 0x10
    addiu $v0, $sp, 0x60
    addiu $s5, $sp, 0x20
    sw $v0, 0x80($sp)
    addiu $s6, $sp, 0x30
    addiu $s7, $sp, 0x40
    addiu $fp, $sp, 0x70
    addiu $s3, $s2, 0x21C
    daddu $s0, $s2, $zero
    nop
    addiu $a1, $s1, 0x400
    daddu $a0, $sp, $zero
    andi $a1, $a1, 0xFFFF
    jal func_0011AE40
    addiu $s1, $s1, 0x1
    lq $v0, 0x0($sp)
    sltiu $v1, $s1, 0x20
    sq $v0, 0x0($s0)
    .word 0x1460fff7
    addiu $s0, $s0, 0x10
    daddu $a0, $sp, $zero
    jal func_0011AE40
    addiu $a1, $zero, 0x430
    daddu $s0, $s3, $zero
    lw $v0, 0x0($sp)
    daddu $a0, $s4, $zero
    addiu $a1, $zero, 0x431
    daddu $s1, $zero, $zero
    jal func_0011AE40
    sw $v0, 0x200($s2)
    lw $v0, 0x10($sp)
    daddu $a0, $s5, $zero
    addiu $a1, $zero, 0x432
    jal func_0011AE40
    sw $v0, 0x204($s2)
    lw $v0, 0x20($sp)
    daddu $a0, $s6, $zero
    addiu $a1, $zero, 0x434
    jal func_0011AE40
    sw $v0, 0x208($s2)
    lw $v0, 0x30($sp)
    daddu $a0, $s7, $zero
    addiu $a1, $zero, 0x435
    jal func_0011AE40
    sw $v0, 0x20C($s2)
    lw $v0, 0x40($sp)
    addiu $a1, $zero, 0x436
    lw $a0, 0x84($sp)
    jal func_0011AE40
    sw $v0, 0x210($s2)
    lw $v0, 0x50($sp)
    addiu $a1, $zero, 0x437
    lw $a0, 0x80($sp)
    jal func_0011AE40
    sw $v0, 0x214($s2)
    lw $v0, 0x60($sp)
    sw $v0, 0x218($s2)
    nop
    addiu $a1, $s1, 0x420
    daddu $a0, $fp, $zero
    andi $a1, $a1, 0xFFFF
    jal func_0011AE40
    addiu $s1, $s1, 0x1
    lhu $v0, 0x70($sp)
    sltiu $v1, $s1, 0x10
    sh $v0, 0x0($s0)
    .word 0x1460fff7
    addiu $s0, $s0, 0x2
    addiu $v0, $zero, 0x1
    ld $ra, 0x120($sp)
    ld $fp, 0x110($sp)
    ld $s7, 0x100($sp)
    ld $s6, 0xF0($sp)
    ld $s5, 0xE0($sp)
    ld $s4, 0xD0($sp)
    ld $s3, 0xC0($sp)
    ld $s2, 0xB0($sp)
    ld $s1, 0xA0($sp)
    ld $s0, 0x90($sp)
    jr $ra
    addiu $sp, $sp, 0x130
}
