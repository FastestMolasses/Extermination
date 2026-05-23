// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00102948(int, int, int, int);
extern void func_001EF9D0(int, int, int, int);

asm void func_001EFE00(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    paddub $s1, $a1, $zero
    addiu $a0, $sp, 0x30
    jal func_00102948
    addiu $a1, $s1, 0xB0
    lui $v0, (0x80000027 >> 16)
    ori $v0, $v0, (0x80000027 & 0xFFFF)
    .word 0x12020003
    nop
    .word 0x10000008
    lui $v0, (0x3F800000 >> 16)
    lwc1 $f1, 0x34($sp)
    lui $v0, (0x41200000 >> 16)
    mtc1 $v0, $f0
    nop
    add.s $f0, $f1, $f0
    swc1 $f0, 0x34($sp)
    lui $v0, (0x3F800000 >> 16)
    mtc1 $v0, $f12
    addiu $a1, $sp, 0x30
    jal func_001EF9D0
    paddub $a0, $s0, $zero
    paddub $s0, $v0, $zero
    .word 0x1200000a
    paddub $v0, $s0, $zero
    lw $v0, 0x14($s1)
    addiu $a0, $s0, 0xB0
    addiu $a1, $s1, 0xB0
    jal func_00102948
    sw $v0, 0x24($s0)
    addiu $a1, $s1, 0xC0
    jal func_00102948
    addiu $a0, $s0, 0xC0
    paddub $v0, $s0, $zero
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
