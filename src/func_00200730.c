// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00112D18(int, int, int, int);
extern void func_00113680(int, int, int, int);

asm void func_00200730(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_00112D18
    addiu $a0, $zero, 0x1
    .word 0x14400009
    paddub $v0, $zero, $zero
    jal func_00113680
    nop
    addiu $a0, $zero, 0x1
    addiu $v1, $zero, 0x2
    movn $a0, $v1, $v0
    .word 0x10000002
    paddub $v0, $a0, $zero
    paddub $v0, $zero, $zero
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
