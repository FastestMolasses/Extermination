// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void Exit(int, int, int, int);

asm void func_0010BF38(void) {
    addiu $sp, $sp, -0x10
    addiu $v0, $zero, 0x1
    .word 0x14820003
    sd $ra, 0x0($sp)
    jal Exit
    daddu $a0, $a1, $zero
    ld $ra, 0x0($sp)
    daddu $v0, $zero, $zero
    jr $ra
    addiu $sp, $sp, 0x10
}
