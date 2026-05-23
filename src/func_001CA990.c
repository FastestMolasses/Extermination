// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001C7420(int, int, int, int);
extern void func_001CA7B0(int, int, int, int);
extern void func_001CA940(int, int, int, int);
extern void func_001D1F80(int, int, int, int);
extern void func_001D8C20(int, int, int, int);

asm void func_001CA990(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    paddub $s1, $a0, $zero
    paddub $a0, $a1, $zero
    jal func_001CA7B0
    sq $s0, 0x0($sp)
    paddub $s0, $v0, $zero
    .word 0x0600000e
    nop
    jal func_001D8C20
    paddub $a0, $zero, $zero
    addiu $a1, $zero, 0x3F5
    paddub $a0, $s1, $zero
    jal func_001C7420
    paddub $a2, $zero, $zero
    addiu $a1, $zero, 0x1
    paddub $a0, $zero, $zero
    jal func_001D1F80
    paddub $a2, $zero, $zero
    lw $a1, 0x44($s1)
    jal func_001CA940
    paddub $a0, $s0, $zero
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
