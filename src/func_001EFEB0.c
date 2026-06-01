// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void copy_qw4(int, int, int, int);
extern void func_001EF9D0(int, int, int, int);

asm void func_001EFEB0(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    paddub $s1, $a1, $zero
    lui $v0, (0x3F800000 >> 16)
    mtc1 $v0, $f12
    addiu $a1, $s1, 0x30
    jal func_001EF9D0
    sq $s0, 0x0($sp)
    paddub $s0, $v0, $zero
    .word 0x12000005
    paddub $v0, $s0, $zero
    addiu $a0, $s0, 0xD0
    jal copy_qw4
    paddub $a1, $s1, $zero
    paddub $v0, $s0, $zero
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
