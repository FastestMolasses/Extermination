// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1C47A0(int, int, int, int);

asm void func_overlay_AREA13_008239D0(void) {
    addiu $sp, $sp, -0x10
    addiu $a0, $zero, 0x1A
    sq $ra, 0x0($sp)
    jal func_1C47A0
    addiu $a1, $zero, 0x1
    lq $ra, 0x0($sp)
    addiu $v0, $zero, 0x1
    jr $ra
    addiu $sp, $sp, 0x10
}
