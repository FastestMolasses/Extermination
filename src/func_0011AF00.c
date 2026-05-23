// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011AFD0(int, int, int, int);
extern void func_0011B018(int, int, int, int);
extern void func_0011B028(int, int, int, int);

asm void func_0011AF00(void) {
    addiu $sp, $sp, -0x20
    sd $s0, 0x0($sp)
    sd $ra, 0x10($sp)
    jal func_0011AFD0
    daddu $s0, $zero, $zero
    daddu $v1, $v0, $zero
    slti $v0, $v1, 0x4
    .word 0x10400008
    daddu $v0, $s0, $zero
    .word 0x18600007
    ld $ra, 0x10($sp)
    jal func_0011B028
    addiu $s0, $zero, 0x1
    jal func_0011B018
    daddu $a0, $v0, $zero
    daddu $v0, $s0, $zero
    ld $ra, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
