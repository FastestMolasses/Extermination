// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001232E0(int, int, int, int);
extern void func_001CBE10(int, int, int, int);

asm void func_001CC170(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    jal func_001232E0
    paddub $s0, $a0, $zero
    paddub $v1, $v0, $zero
    .word 0x1060000d
    paddub $a1, $zero, $zero
    lbu $a0, 0x0($s0)
    sltiu $v0, $a0, 0x20
    .word 0x54400006
    addiu $v1, $v1, -0x1
    jal func_001CBE10
    nop
    addu $a1, $a1, $v0
    nop
    addiu $v1, $v1, -0x1
    .word 0x1460fff6
    addiu $s0, $s0, 0x1
    nop
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    paddub $v0, $a1, $zero
    jr $ra
    addiu $sp, $sp, 0x20
}
