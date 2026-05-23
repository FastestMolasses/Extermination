// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00120B38(int, int, int, int);
extern void func_00124EF8(int, int, int, int);

asm void func_00122DE8(void) {
    addiu $sp, $sp, -0x40
    sd $s2, 0x20($sp)
    sd $s1, 0x10($sp)
    daddu $s2, $a2, $zero
    sd $s0, 0x0($sp)
    sd $ra, 0x30($sp)
    daddu $s0, $a0, $zero
    lhu $v0, 0xC($s0)
    andi $v0, $v0, 0x100
    .word 0x10400006
    daddu $s1, $a1, $zero
    lw $a0, 0x54($s0)
    daddu $a2, $zero, $zero
    lh $a1, 0xE($s0)
    jal func_00120B38
    addiu $a3, $zero, 0x2
    lhu $v0, 0xC($s0)
    daddu $a2, $s1, $zero
    lh $a1, 0xE($s0)
    daddu $a3, $s2, $zero
    andi $v0, $v0, 0xEFFF
    lw $a0, 0x54($s0)
    jal func_00124EF8
    sh $v0, 0xC($s0)
    ld $ra, 0x30($sp)
    dsll32 $v0, $v0, 0
    dsra32 $v0, $v0, 0
    ld $s2, 0x20($sp)
    ld $s1, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
