// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0010F8F8(int, int, int, int);

asm void func_001FA6A0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_0010F8F8
    addiu $a0, $a0, 0x10
    andi $v1, $v0, 0xF
    .word 0x10600004
    nop
    addiu $v1, $zero, -0x10
    and $v0, $v0, $v1
    addiu $v0, $v0, 0x10
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
