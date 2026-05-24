// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_overlay_AREA01_00826200(int, int, int, int);
extern void func_overlay_AREA01_00826440(int, int, int, int);

asm void func_overlay_AREA01_00826160(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lbu $v1, 0x4($a0)
    .word 0x14600003
    paddub $s0, $a0, $zero
    addiu $v1, $zero, 0x1
    sb $v1, 0x0($s0)
    lbu $a0, 0xD($s0)
    addiu $v1, $zero, 0x2
    .word 0x14830003
    nop
    jal func_overlay_AREA01_00826200
    paddub $a0, $s0, $zero
    lbu $a0, 0xD($s0)
    addiu $v1, $zero, 0x3
    .word 0x14830003
    nop
    jal func_overlay_AREA01_00826440
    paddub $a0, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
