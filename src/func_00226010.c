// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001FE920(int, int, int, int);
extern void func_001FE9A0(int, int, int, int);

asm void func_00226010(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    addiu $a0, $zero, 0x1
    addiu $a1, $s0, 0x50
    jal func_001FE9A0
    addiu $a2, $s0, 0x54
    .word 0x14400003
    nop
    .word 0x10000007
    paddub $v0, $zero, $zero
    lw $a1, 0x4C($s0)
    jal func_001FE920
    lw $a0, 0x48($s0)
    addiu $v1, $zero, 0x1
    movn $v1, $zero, $v0
    paddub $v0, $v1, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
