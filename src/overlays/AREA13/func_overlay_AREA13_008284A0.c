// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4

asm void func_overlay_AREA13_008284A0(void) {
    lbu $v1, 0x0($a1)
    andi $v1, $v1, 0x2
    .word 0x14600003
    nop
    addiu $v1, $zero, 0x2
    sb $v1, 0x4($a0)
    jr $ra
    nop
}
