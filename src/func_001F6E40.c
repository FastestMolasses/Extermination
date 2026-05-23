// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001F6640(int, int, int, int);
extern void func_001F66F0(int, int, int, int);
extern void func_001F6D60(int, int, int, int);

asm void func_001F6E40(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    jal func_001F6D60
    sq $s0, 0x0($sp)
    paddub $s0, $v0, $zero
    .word 0x12000005
    nop
    jal func_001F66F0
    paddub $a0, $s0, $zero
    jal func_001F6640
    paddub $a0, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
