// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001E2290(int, int, int, int);
extern void func_001EF9D0(int, int, int, int);

asm void func_001F0120(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a1, $zero
    paddub $s1, $a0, $zero
    jal func_001E2290
    paddub $a0, $s0, $zero
    .word 0x1040000c
    paddub $v0, $zero, $zero
    lui $v0, (0x3F800000 >> 16)
    mtc1 $v0, $f12
    paddub $a1, $zero, $zero
    lui $v0, (0x80000010 >> 16)
    jal func_001EF9D0
    ori $a0, $v0, (0x80000010 & 0xFFFF)
    .word 0x10400004
    nop
    sb $s0, 0xD($v0)
    lw $v1, 0x14($s1)
    sw $v1, 0x24($v0)
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
