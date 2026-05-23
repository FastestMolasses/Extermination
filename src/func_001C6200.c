// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001029C0(int, int, int, int);

asm void func_001C6200(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s2, $a0, $zero
    paddub $s0, $zero, $zero
    .word 0x1000001c
    paddub $s1, $s2, $zero
    lw $v0, 0x110($s1)
    addiu $a0, $zero, -0x1
    addiu $v1, $zero, 0x1000
    sh $a0, 0x64($v0)
    lw $v0, 0x110($s1)
    sh $v1, 0x88($v0)
    lw $v0, 0x110($s1)
    sh $v1, 0x8A($v0)
    lw $v0, 0x110($s1)
    sh $v1, 0x8C($v0)
    lw $v0, 0x110($s1)
    sw $zero, 0x7C($v0)
    lw $v0, 0x110($s1)
    sw $zero, 0x80($v0)
    lw $v0, 0x110($s1)
    sw $zero, 0x84($v0)
    lw $v0, 0x110($s1)
    sw $zero, 0x70($v0)
    lw $v0, 0x110($s1)
    sw $zero, 0x74($v0)
    lw $v0, 0x110($s1)
    sw $zero, 0x78($v0)
    jal func_001029C0
    lw $a0, 0x110($s1)
    addiu $s1, $s1, 0x4
    addiu $s0, $s0, 0x1
    nop
    lbu $v1, 0xC($s2)
    slt $v1, $s0, $v1
    .word 0x1460ffe2
    nop
    lq $ra, 0x30($sp)
    lq $s2, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
