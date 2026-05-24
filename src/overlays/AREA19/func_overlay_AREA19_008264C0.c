// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1FBD50(int, int, int, int);

asm void func_overlay_AREA19_008264C0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    lui $v0, (0x43960000 >> 16)
    sq $s0, 0x0($sp)
    mtc1 $v0, $f12
    addiu $a1, $zero, 0x135
    paddub $s0, $a0, $zero
    jal func_1FBD50
    paddub $a2, $zero, $zero
    sw $v0, 0x2EC($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    addiu $v0, $zero, 0x1
    jr $ra
    addiu $sp, $sp, 0x20
}
