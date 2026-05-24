// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1FBD50(int, int, int, int);

asm void func_overlay_AREA13_00826F80(void) {
    addiu $sp, $sp, -0x10
    lui $v0, (0x43960000 >> 16)
    mtc1 $v0, $f12
    addiu $a1, $zero, 0x40D
    sq $ra, 0x0($sp)
    jal func_1FBD50
    paddub $a2, $zero, $zero
    lq $ra, 0x0($sp)
    addiu $v0, $zero, 0x1
    jr $ra
    addiu $sp, $sp, 0x10
}
