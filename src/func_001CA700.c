// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001AF780(int, int, int, int);
extern void func_001D0690(int, int, int, int);

asm void func_001CA700(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lw $v0, 0x90($a0)
    paddub $s1, $a1, $zero
    paddub $s0, $a2, $zero
    .word 0x14400007
    paddub $s2, $a0, $zero
    jal func_001AF780
    nop
    .word 0x14400003
    sw $v0, 0x90($s2)
    .word 0x10000006
    paddub $v0, $zero, $zero
    sh $s0, 0x94($s2)
    addiu $a0, $v0, 0x70
    jal func_001D0690
    sw $s1, 0x60($v0)
    addiu $v0, $zero, 0x1
    lq $ra, 0x30($sp)
    lq $s2, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
