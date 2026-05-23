// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00122BE8(int, int, int, int);

asm void func_00122D80(void) {
    addiu $sp, $sp, -0x20
    daddu $v0, $a1, $zero
    sd $s0, 0x0($sp)
    daddu $a3, $a2, $zero
    sd $ra, 0x10($sp)
    daddu $s0, $a0, $zero
    daddu $a2, $v0, $zero
    lw $a0, 0x54($s0)
    jal func_00122BE8
    lh $a1, 0xE($s0)
    dsll32 $v1, $v0, 0
    dsra32 $v1, $v1, 0
    .word 0x04620005
    lhu $v0, 0xC($s0)
    lw $v0, 0x50($s0)
    addu $v0, $v0, $v1
    .word 0x10000003
    sw $v0, 0x50($s0)
    andi $v0, $v0, 0xEFFF
    sh $v0, 0xC($s0)
    ld $ra, 0x10($sp)
    daddu $v0, $v1, $zero
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
