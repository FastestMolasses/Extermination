// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00120B38(int, int, int, int);

asm void func_00122E68(void) {
    addiu $sp, $sp, -0x20
    daddu $v0, $a1, $zero
    sd $s0, 0x0($sp)
    daddu $a3, $a2, $zero
    sd $ra, 0x10($sp)
    daddu $s0, $a0, $zero
    daddu $a2, $v0, $zero
    lw $a0, 0x54($s0)
    jal func_00120B38
    lh $a1, 0xE($s0)
    daddu $a0, $v0, $zero
    addiu $v0, $zero, -0x1
    .word 0x14820003
    lhu $v0, 0xC($s0)
    .word 0x10000005
    andi $v0, $v0, 0xEFFF
    dsll32 $v1, $a0, 0
    dsra32 $v1, $v1, 0
    sw $v1, 0x50($s0)
    ori $v0, $v0, 0x1000
    sh $v0, 0xC($s0)
    ld $ra, 0x10($sp)
    daddu $v0, $a0, $zero
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
