// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001157F0(int, int, int, int);

asm void func_0011A888(void) {
    addiu $sp, $sp, -0x10
    ori $v0, $zero, 0xFFFF
    sltu $v0, $v0, $a0
    .word 0x14400007
    sd $ra, 0x0($sp)
    dsll32 $a1, $a0, 0
    dsra32 $a1, $a1, 0
    daddu $a2, $zero, $zero
    addiu $a0, $zero, 0x4C
    jal func_001157F0
    daddu $a3, $zero, $zero
    ld $ra, 0x0($sp)
    daddu $v0, $zero, $zero
    jr $ra
    addiu $sp, $sp, 0x10
}
