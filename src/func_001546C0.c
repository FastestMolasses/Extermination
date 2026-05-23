// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00154740(int, int, int, int);
extern void func_001549C0(int, int, int, int);
extern void func_001AFC10(int, int, int, int);

asm void func_001546C0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lbu $a3, 0x4($a0)
    addiu $a2, $zero, 0x3
    .word 0x10e60015
    addiu $a1, $a0, 0x1F0
    addiu $v1, $zero, 0x2
    .word 0x10e30010
    nop
    addiu $v1, $zero, 0x1
    .word 0x10e30009
    nop
    .word 0x10e00003
    nop
    .word 0x1000000e
    lq $ra, 0x0($sp)
    jal func_00154740
    nop
    .word 0x10000009
    nop
    jal func_001549C0
    nop
    .word 0x10000005
    nop
    .word 0x10000003
    sb $a2, 0x4($a0)
    jal func_001AFC10
    nop
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
