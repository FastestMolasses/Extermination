// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_overlay_AREA13_00823C10(int, int, int, int);
extern void func_overlay_AREA13_00823D50(int, int, int, int);

asm void func_overlay_AREA13_00823B80(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lbu $a1, 0xD($a0)
    addiu $v1, $zero, 0x1
    .word 0x10a30009
    nop
    .word 0x10a00003
    nop
    .word 0x10000008
    lq $ra, 0x0($sp)
    jal func_overlay_AREA13_00823C10
    nop
    .word 0x10000003
    nop
    jal func_overlay_AREA13_00823D50
    nop
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
