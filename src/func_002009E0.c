// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void DisableDmacHandler(int, int, int, int);
extern void func_00121A28(int, int, int, int);

asm void func_002009E0(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lw $s1, 0x14($a0)
    paddub $s0, $a0, $zero
    paddub $s2, $a1, $zero
    jal DisableDmacHandler
    addiu $a0, $zero, 0x2
    .word 0x12200006
    nop
    addu $s0, $s0, $s2
    paddub $a0, $s0, $zero
    paddub $a2, $s1, $zero
    jal func_00121A28
    paddub $a1, $zero, $zero
    lq $ra, 0x30($sp)
    lq $s2, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
