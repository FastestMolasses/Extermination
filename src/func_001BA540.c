// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001CA770(int, int, int, int);
extern void func_001D8BF0(int, int, int, int);

asm void func_001BA540(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lh $v0, 0x56($a0)
    .word 0x10400003
    paddub $s0, $a0, $zero
    jal func_001CA770
    nop
    paddub $a0, $s0, $zero
    jal func_001D8BF0
    paddub $a1, $zero, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
