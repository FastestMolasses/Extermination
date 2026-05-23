// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001EF9D0(int, int, int, int);

asm void func_001F0060(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lui $v0, (0x3F800000 >> 16)
    paddub $s0, $a1, $zero
    mtc1 $v0, $f12
    jal func_001EF9D0
    paddub $a1, $zero, $zero
    .word 0x10400002
    nop
    sw $s0, 0x24($v0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
