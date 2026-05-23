// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001D2730(int, int, int, int);
extern void func_001E0C80(int, int, int, int);

asm void func_001D2830(void) {
    addiu $sp, $sp, -0x10
    slti $at, $a0, 0x20
    .word 0x10200005
    sq $ra, 0x0($sp)
    jal func_001D2730
    nop
    .word 0x1000000a
    lq $ra, 0x0($sp)
    slti $at, $a0, 0x40
    .word 0x10200006
    paddub $v0, $zero, $zero
    jal func_001E0C80
    nop
    .word 0x10000002
    nop
    paddub $v0, $zero, $zero
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
