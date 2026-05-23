// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011A070(int, int, int, int);

asm void func_0016C520(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    lb $a0, 0x31B($a0)
    addiu $v1, $zero, -0x1
    .word 0x10830006
    nop
    jal func_0011A070
    nop
    addiu $v1, $zero, -0x1
    sb $v1, 0x31B($s0)
    sb $zero, 0x31A($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
