// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1FA790(int, int, int, int);
extern void func_1FABB0(int, int, int, int);

asm void func_overlay_AREA08_00824420(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_1FABB0
    nop
    addiu $a1, $zero, 0x11
    jal func_1FA790
    paddub $a0, $zero, $zero
    lq $ra, 0x0($sp)
    addiu $v0, $zero, 0x1
    jr $ra
    addiu $sp, $sp, 0x10
}
