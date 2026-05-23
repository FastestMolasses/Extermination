// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001FABB0(int, int, int, int);
extern void func_001FBC50(int, int, int, int);

asm void func_001FD470(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    andi $v1, $a0, 0x1
    .word 0x10600003
    paddub $s0, $a0, $zero
    jal func_001FBC50
    nop
    andi $v1, $s0, 0x2
    .word 0x10600003
    nop
    jal func_001FABB0
    nop
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
