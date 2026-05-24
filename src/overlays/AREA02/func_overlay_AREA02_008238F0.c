// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_overlay_AREA02_00823980(int, int, int, int);
extern void func_overlay_AREA02_00824020(int, int, int, int);

asm void func_overlay_AREA02_008238F0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lbu $a2, 0x2($a0)
    addiu $a1, $zero, -0xE1
    addiu $v1, $zero, 0x9
    and $a1, $a2, $a1
    .word 0x14a30006
    addiu $v1, $zero, 0x4
    jal func_overlay_AREA02_00823980
    nop
    .word 0x10000007
    lq $ra, 0x0($sp)
    addiu $v1, $zero, 0x4
    .word 0x14a30003
    nop
    jal func_overlay_AREA02_00824020
    nop
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
