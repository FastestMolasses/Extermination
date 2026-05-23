// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001157F0(int, int, int, int);

asm void func_0011A960(void) {
    addiu $sp, $sp, -0x10
    ori $v0, $zero, 0xFFFF
    sd $ra, 0x0($sp)
    sltu $v0, $v0, $a1
    .word 0x1440000e
    addiu $v1, $zero, -0x1
    lui $v0, (0x1FFFFF >> 16)
    ori $v0, $v0, (0x1FFFFF & 0xFFFF)
    sltu $v0, $v0, $a2
    .word 0x1440000a
    ld $ra, 0x0($sp)
    slti $v0, $a0, 0x2
    .word 0x10400007
    daddu $a3, $a0, $zero
    dsll32 $a1, $a1, 0
    dsra32 $a1, $a1, 0
    jal func_001157F0
    addiu $a0, $zero, 0x4F
    daddu $v1, $zero, $zero
    ld $ra, 0x0($sp)
    daddu $v0, $v1, $zero
    jr $ra
    addiu $sp, $sp, 0x10
}
