// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00122BB8(int, int, int, int);

asm void func_00179B90(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_00122BB8
    nop
    andi $v0, $v0, 0x7
    slti $v1, $v0, 0x5
    .word 0x14600002
    nop
    addiu $v0, $v0, -0x5
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
