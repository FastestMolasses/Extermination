// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001157F0(int, int, int, int);

asm void func_0011A8C8(void) {
    addiu $sp, $sp, -0x10
    daddu $a2, $a1, $zero
    sd $ra, 0x0($sp)
    slti $v0, $a0, 0x10
    .word 0x1040000b
    addiu $v1, $zero, -0x1
    lui $v0, (0x1FFFFF >> 16)
    ori $v0, $v0, (0x1FFFFF & 0xFFFF)
    sltu $v0, $v0, $a2
    .word 0x14400007
    ld $ra, 0x0($sp)
    daddu $a1, $a0, $zero
    daddu $a3, $zero, $zero
    jal func_001157F0
    addiu $a0, $zero, 0x4D
    daddu $v1, $zero, $zero
    ld $ra, 0x0($sp)
    daddu $v0, $v1, $zero
    jr $ra
    addiu $sp, $sp, 0x10
}
