// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4

asm void func_overlay_AREA01_00824F70(void) {
    nop
    c.le.s $f0, $f1
    nop
    .word 0x45010004
    addiu $v0, $zero, 0x1
    .word 0x10000002
    paddub $v0, $zero, $zero
    addiu $v0, $zero, 0x1
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
