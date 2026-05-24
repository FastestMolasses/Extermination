// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_11A070(int, int, int, int);

asm void func_overlay_AREA19_00826500(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lw $a0, 0x2EC($a0)
    addiu $v0, $zero, -0x1
    .word 0x10820003
    nop
    jal func_11A070
    nop
    lq $ra, 0x0($sp)
    addiu $v0, $zero, 0x1
    jr $ra
    addiu $sp, $sp, 0x10
}
