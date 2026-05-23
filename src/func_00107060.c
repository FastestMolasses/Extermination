// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00106830(int, int, int, int);
extern void func_00106948(int, int, int, int);

asm void func_00107060(void) {
    addiu $sp, $sp, -0x10
    .word 0x10000004
    sd $ra, 0x0($sp)
    nop
    jal func_00106830
    nop
    jal func_00106948
    addiu $a0, $zero, 0x1
    .word 0x1440fffb
    addiu $a0, $zero, 0x8
    ld $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
