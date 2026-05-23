// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001157F0(int, int, int, int);

asm void func_0011A5C8(void) {
    addiu $sp, $sp, -0x10
    addiu $v1, $zero, -0x1
    sltiu $v0, $a0, 0x30
    .word 0x10400007
    sd $ra, 0x0($sp)
    daddu $a1, $a0, $zero
    daddu $a2, $zero, $zero
    addiu $a0, $zero, 0x3F
    jal func_001157F0
    daddu $a3, $zero, $zero
    daddu $v1, $zero, $zero
    ld $ra, 0x0($sp)
    daddu $v0, $v1, $zero
    jr $ra
    addiu $sp, $sp, 0x10
}
