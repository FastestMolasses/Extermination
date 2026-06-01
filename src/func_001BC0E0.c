// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001BA1F0(int, int, int, int);
extern void anim_advance_time(int, int, int, int);

asm void func_001BC0E0(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lb $v0, 0xC($a1)
    paddub $s1, $a0, $zero
    .word 0x10400006
    paddub $s0, $a1, $zero
    lui $v0, (0x3F800000 >> 16)
    mtc1 $v0, $f12
    jal anim_advance_time
    nop
    sh $v0, 0xE($s0)
    jal func_001BA1F0
    paddub $a0, $s1, $zero
    .word 0x10400004
    paddub $v0, $zero, $zero
    .word 0x10000002
    addiu $v0, $zero, 0x1
    paddub $v0, $zero, $zero
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
