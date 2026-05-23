// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001205D8(int, int, int, int);

asm void func_001235D8(void) {
    addiu $sp, $sp, -0x20
    sd $s0, 0x0($sp)
    sd $ra, 0x10($sp)
    daddu $s0, $a1, $zero
    lw $v0, 0x8($s0)
    .word 0x14400003
    daddu $v0, $zero, $zero
    .word 0x10000005
    sw $zero, 0x4($s0)
    jal func_001205D8
    daddu $a1, $s0, $zero
    sw $zero, 0x4($s0)
    sw $zero, 0x8($s0)
    ld $ra, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
