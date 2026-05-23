// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00126AB8(int, int, int, int);

asm void func_00127500(void) {
    addiu $sp, $sp, -0x30
    addiu $v0, $zero, 0x3
    srl $v1, $a0, 31
    sd $ra, 0x20($sp)
    sw $v0, 0x0($sp)
    .word 0x14800004
    sw $v1, 0x4($sp)
    addiu $v0, $zero, 0x2
    .word 0x10000020
    sw $v0, 0x0($sp)
    addiu $v0, $zero, 0x3C
    .word 0x1060000a
    sw $v0, 0x8($sp)
    lui $v0, (0x80000000 >> 16)
    .word 0x14820005
    negu $v0, $a0
    ori $v0, $zero, 0xC1E0
    dsll32 $v0, $v0, 16
    .word 0x10000019
    ld $ra, 0x20($sp)
    .word 0x10000002
    sd $v0, 0x10($sp)
    sd $a0, 0x10($sp)
    ld $a1, 0x10($sp)
    addiu $v0, $zero, -0x1
    dsrl $v0, $v0, 4
    sltu $v0, $v0, $a1
    .word 0x1440000d
    lw $a0, 0x8($sp)
    addiu $a2, $zero, -0x1
    dsrl $a2, $a2, 4
    nop
    dsll $v1, $a1, 1
    addiu $a0, $a0, -0x1
    daddu $a1, $v1, $zero
    sltu $v0, $a2, $v1
    nop
    .word 0x1040fffa
    nop
    sw $a0, 0x8($sp)
    sd $v1, 0x10($sp)
    jal func_00126AB8
    daddu $a0, $sp, $zero
    ld $ra, 0x20($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
