// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_122BB8(int, int, int, int);

asm void func_overlay_AREA21_00829ED0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    addiu $s0, $a0, 0x1F0
    jal func_122BB8
    sw $zero, 0x240($a0)
    sw $v0, 0x54($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
