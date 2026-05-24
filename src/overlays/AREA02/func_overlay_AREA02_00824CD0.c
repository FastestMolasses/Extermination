// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1026A0(int, int, int, int);
extern void func_overlay_AREA02_00824C40(int, int, int, int);

asm void func_overlay_AREA02_00824CD0(void) {
    jal func_1026A0
    addiu $a0, $sp, 0x40
    lw $v0, 0x110($s0)
    addiu $a2, $sp, 0x30
    addiu $s0, $sp, 0x50
    addiu $a1, $v0, 0x90
    jal func_1026A0
    paddub $a0, $s0, $zero
    jal func_overlay_AREA02_00824C40
    addiu $a0, $sp, 0x40
    jal func_overlay_AREA02_00824C40
    paddub $a0, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x60
}
