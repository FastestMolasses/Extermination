// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00109A40(int, int, int, int);
extern void func_00206BE0(int, int, int, int);

asm void func_00206CC0(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s1, $a0, $zero
    jal func_00206BE0
    paddub $s0, $zero, $zero
    .word 0x14400007
    paddub $v0, $s0, $zero
    jal func_00109A40
    paddub $a0, $s1, $zero
    .word 0x10400002
    nop
    addiu $s0, $zero, 0x1
    paddub $v0, $s0, $zero
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
