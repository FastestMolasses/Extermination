// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001C4720(int, int, int, int);
extern void func_001C4760(int, int, int, int);
extern void func_001C47A0(int, int, int, int);

asm void func_001B6EA0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lbu $v0, 0x3($a0)
    .word 0x14400007
    addiu $a1, $zero, 0x1
    lhu $a0, 0x2E($a0)
    jal func_001C47A0
    addiu $a1, $zero, 0x1
    .word 0x1000000b
    lq $ra, 0x0($sp)
    addiu $a1, $zero, 0x1
    .word 0x14450005
    nop
    jal func_001C4720
    lhu $a0, 0x2E($a0)
    .word 0x10000003
    nop
    jal func_001C4760
    lhu $a0, 0x2E($a0)
    lq $ra, 0x0($sp)
    addiu $v0, $zero, 0x1
    jr $ra
    addiu $sp, $sp, 0x10
}
