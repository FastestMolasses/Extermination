// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1C4820(int, int, int, int);
extern void func_1C5C90(int, int, int, int);

asm void func_overlay_AREA01_00826CB0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lbu $v1, 0x3($a0)
    addiu $v0, $zero, 0x1
    .word 0x10620003
    nop
    .word 0x10000005
    nop
    jal func_1C5C90
    nop
    .word 0x10000004
    lq $ra, 0x0($sp)
    jal func_1C4820
    nop
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
