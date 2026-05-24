// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_overlay_AREA21_00826310(int, int, int, int);
extern void func_overlay_AREA21_008264D0(int, int, int, int);

asm void func_overlay_AREA21_00826290(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lbu $v0, 0x3($a0)
    .word 0x14400005
    nop
    jal func_overlay_AREA21_00826310
    nop
    .word 0x10000004
    lq $ra, 0x0($sp)
    jal func_overlay_AREA21_008264D0
    nop
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
