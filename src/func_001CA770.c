// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001AF890(int, int, int, int);

asm void func_001CA770(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    lw $a0, 0x90($a0)
    .word 0x10800006
    nop
    jal func_001AF890
    nop
    addiu $v1, $zero, -0x1
    sw $zero, 0x90($s0)
    sh $v1, 0x94($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
