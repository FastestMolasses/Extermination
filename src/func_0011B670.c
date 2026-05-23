// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011B6E0(int, int, int, int);

asm void func_0011B670(void) {
    addiu $sp, $sp, -0x10
    sd $ra, 0x0($sp)
    jal func_0011B6E0
    nop
    addiu $v1, $zero, 0x1
    .word 0x14430006
    daddu $v0, $zero, $zero
    lui $v1, (0x10003810 >> 16)
    addiu $a0, $zero, 0x8
    ori $v1, $v1, (0x10003810 & 0xFFFF)
    addiu $v0, $zero, 0x1
    sw $a0, 0x0($v1)
    ld $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
