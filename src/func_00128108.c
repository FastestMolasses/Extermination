// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001277B0(int, int, int, int);

asm void func_00128108(void) {
    addiu $sp, $sp, -0x20
    addiu $v0, $zero, 0x3
    srl $v1, $a0, 31
    sd $ra, 0x10($sp)
    sw $v0, 0x0($sp)
    .word 0x14800004
    sw $v1, 0x4($sp)
    addiu $v0, $zero, 0x2
    .word 0x10000020
    sw $v0, 0x0($sp)
    addiu $v0, $zero, 0x1E
    .word 0x1060000a
    sw $v0, 0x8($sp)
    lui $v0, (0x80000000 >> 16)
    .word 0x14820005
    negu $v0, $a0
    lui $at, (0xCF000000 >> 16)
    mtc1 $at, $f0
    .word 0x10000019
    ld $ra, 0x10($sp)
    .word 0x10000002
    sw $v0, 0xC($sp)
    sw $a0, 0xC($sp)
    lw $a2, 0xC($sp)
    lui $v0, (0x3FFFFFFF >> 16)
    ori $v0, $v0, (0x3FFFFFFF & 0xFFFF)
    sltu $v0, $v0, $a2
    .word 0x1440000d
    lui $a1, (0x3FFFFFFF >> 16)
    lw $a0, 0x8($sp)
    ori $a1, $a1, (0x3FFFFFFF & 0xFFFF)
    nop
    sll $v1, $a2, 1
    addiu $a0, $a0, -0x1
    daddu $a2, $v1, $zero
    sltu $v0, $a1, $v1
    nop
    .word 0x1040fffa
    nop
    sw $a0, 0x8($sp)
    sw $v1, 0xC($sp)
    jal func_001277B0
    daddu $a0, $sp, $zero
    ld $ra, 0x10($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
