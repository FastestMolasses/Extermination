// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00120F40(int, int, int, int);
extern void func_00121A28(int, int, int, int);

asm void func_0011FEE8(void) {
    addiu $sp, $sp, -0x40
    addiu $v0, $zero, 0x58
    sd $s1, 0x10($sp)
    sd $s2, 0x20($sp)
    daddu $s1, $a1, $zero
    mult $s2, $s1, $v0
    sd $s0, 0x0($sp)
    sd $ra, 0x30($sp)
    jal func_00120F40
    addiu $a1, $s2, 0xC
    daddu $s0, $v0, $zero
    .word 0x1200000a
    daddu $v0, $zero, $zero
    addiu $v0, $s0, 0xC
    sw $s1, 0x4($s0)
    sw $zero, 0x0($s0)
    daddu $a0, $v0, $zero
    daddu $a2, $s2, $zero
    sw $v0, 0x8($s0)
    jal func_00121A28
    daddu $a1, $zero, $zero
    daddu $v0, $s0, $zero
    ld $ra, 0x30($sp)
    ld $s2, 0x20($sp)
    ld $s1, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
