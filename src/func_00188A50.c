// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00188AC0(int, int, int, int);
extern void func_00188B80(int, int, int, int);
extern void func_00188C70(int, int, int, int);

asm void func_00188A50(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lbu $a1, 0xD($a0)
    addiu $v1, $zero, 0x15
    .word 0x10a30010
    nop
    addiu $v1, $zero, 0x10
    .word 0x10a30009
    nop
    .word 0x10a00003
    nop
    .word 0x1000000c
    lq $ra, 0x0($sp)
    jal func_00188AC0
    nop
    .word 0x10000007
    nop
    jal func_00188B80
    nop
    .word 0x10000003
    nop
    jal func_00188C70
    nop
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
