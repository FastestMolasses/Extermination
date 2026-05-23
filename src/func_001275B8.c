// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00126BE8(int, int, int, int);

asm void func_001275B8(void) {
    addiu $sp, $sp, -0x40
    sd $a0, 0x20($sp)
    daddu $a1, $sp, $zero
    sd $ra, 0x30($sp)
    jal func_00126BE8
    addiu $a0, $sp, 0x20
    lw $v1, 0x0($sp)
    xori $v0, $v1, 0x2
    .word 0x10400003
    sltiu $v0, $v1, 0x2
    .word 0x10400003
    xori $v0, $v1, 0x4
    .word 0x10000015
    daddu $v0, $zero, $zero
    .word 0x10400005
    lw $a0, 0x8($sp)
    .word 0x0480fffb
    slti $v0, $a0, 0x1F
    .word 0x14400007
    addiu $v1, $zero, 0x3C
    lw $v1, 0x4($sp)
    lui $v0, (0x7FFFFFFF >> 16)
    ori $v0, $v0, (0x7FFFFFFF & 0xFFFF)
    lui $a0, (0x80000000 >> 16)
    .word 0x10000009
    movn $v0, $a0, $v1
    ld $v0, 0x10($sp)
    subu $v1, $v1, $a0
    dsrlv $v0, $v0, $v1
    lw $a0, 0x4($sp)
    dsll32 $v0, $v0, 0
    dsra32 $v0, $v0, 0
    negu $v1, $v0
    movn $v0, $v1, $a0
    ld $ra, 0x30($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
