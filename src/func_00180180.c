// CFLAGS: -O4,p -sdatathreshold 0
// Hybrid asm void (template func_0017FE00): real mnemonics where mwcc accepts
// them, .word for PC-relative branches. The duplicated addiu a1 after b+lq is
// CW 2.3.1 dead-instruction; unreachable but byte-present. Byte-identical at link.
extern void func_001749A0(int, int, int, int);

asm void func_00180180(void) {
    addiu $sp, $sp, -0x10
    .word 0x14a0000e
    sq $ra, 0x0($sp)
    lbu $v0, 0x2F1($a0)
    .word 0x14400007
    addiu $a1, $zero, 0xAE
    addiu $a1, $zero, 0xA8
    jal func_001749A0
    paddub $a2, $zero, $zero
    .word 0x10000012
    lq $ra, 0x0($sp)
    addiu $a1, $zero, 0xAE
    jal func_001749A0
    paddub $a2, $zero, $zero
    .word 0x1000000c
    nop
    lbu $v0, 0x2F1($a0)
    .word 0x14400007
    addiu $a1, $zero, 0xB1
    addiu $a1, $zero, 0xAB
    jal func_001749A0
    paddub $a2, $zero, $zero
    .word 0x10000004
    nop
    addiu $a1, $zero, 0xB1
    jal func_001749A0
    paddub $a2, $zero, $zero
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
