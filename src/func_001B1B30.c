// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001B1630(int, int, int, int);
extern void func_001B1B70(int, int, int, int);

asm void func_001B1B30(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    jal func_001B1630
    paddub $s0, $a0, $zero
    sb $v0, 0x1($s0)
    lbu $v0, 0x1($s0)
    .word 0x10400003
    nop
    jal func_001B1B70
    paddub $a0, $s0, $zero
    lbu $v0, 0x1($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
