// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_overlay_AREA15_00824E50(int, int, int, int);
extern void func_overlay_AREA15_00825030(int, int, int, int);

asm void func_overlay_AREA15_00824DC0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lbu $a1, 0xD($a0)
    addiu $v1, $zero, 0x59
    .word 0x14a30003
    paddub $s0, $a0, $zero
    jal func_overlay_AREA15_00824E50
    nop
    lbu $a0, 0xD($s0)
    addiu $v1, $zero, 0x50
    .word 0x14830003
    nop
    jal func_overlay_AREA15_00825030
    paddub $a0, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
