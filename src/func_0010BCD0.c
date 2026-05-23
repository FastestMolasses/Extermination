// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void GetOsdConfigParam2(int, int, int, int);

asm void func_0010BCD0(void) {
    addiu $sp, $sp, -0x20
    sd $ra, 0x10($sp)
    daddu $a0, $sp, $zero
    jal GetOsdConfigParam2
    ori $a1, $sp, 0x8
    lui $v1, (0x1000F000 >> 16)
    sw $zero, 0x0($sp)
    ori $v1, $v1, (0x1000F000 & 0xFFFF)
    addiu $v0, $zero, 0x4
    sw $v0, 0x0($v1)
    lui $v0, (0x10010000 >> 16)
    lw $v0, -0x1000($v0)
    andi $v0, $v0, 0x4
    .word 0x14400004
    addiu $v0, $zero, 0x4
    lw $v0, 0x0($sp)
    .word 0x1040fff9
    addiu $v0, $zero, 0x4
    ld $ra, 0x10($sp)
    lui $at, (0x10010000 >> 16)
    sw $v0, -0x1000($at)
    ld $v0, 0x8($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
