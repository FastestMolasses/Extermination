// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00110AB8(int, int, int, int);

asm void func_00110B80(void) {
    addiu $sp, $sp, -0x10
    sd $ra, 0x0($sp)
    jal func_00110AB8
    nop
    daddu $a0, $v0, $zero
    addiu $v1, $zero, 0x6
    lbu $v0, 0x70($a0)
    .word 0x14430006
    ld $ra, 0x0($sp)
    lbu $v1, 0x71($a0)
    addiu $v0, $zero, 0x2
    .word 0x54620002
    lbu $v0, 0x70($a0)
    addiu $v0, $zero, 0x5
    jr $ra
    addiu $sp, $sp, 0x10
}
