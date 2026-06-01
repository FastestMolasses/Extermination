// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001278C0(int, int, int, int);

asm void float_to_int(void) {
    addiu $sp, $sp, -0x30
    sd $ra, 0x20($sp)
    addiu $a0, $sp, 0x10
    swc1 $f12, 0x10($sp)
    jal func_001278C0
    daddu $a1, $sp, $zero
    lw $v1, 0x0($sp)
    xori $v0, $v1, 0x2
    .word 0x10400003
    sltiu $v0, $v1, 0x2
    .word 0x10400003
    xori $v0, $v1, 0x4
    .word 0x10000013
    daddu $v0, $zero, $zero
    .word 0x10400005
    lw $a0, 0x8($sp)
    .word 0x0480fffb
    slti $v0, $a0, 0x1F
    .word 0x14400007
    addiu $v1, $zero, 0x1E
    lw $v1, 0x4($sp)
    lui $v0, (0x7FFFFFFF >> 16)
    ori $v0, $v0, (0x7FFFFFFF & 0xFFFF)
    lui $a0, (0x80000000 >> 16)
    .word 0x10000007
    movn $v0, $a0, $v1
    lw $v0, 0xC($sp)
    subu $v1, $v1, $a0
    lw $a1, 0x4($sp)
    srlv $v0, $v0, $v1
    negu $a0, $v0
    movn $v0, $a0, $a1
    ld $ra, 0x20($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
