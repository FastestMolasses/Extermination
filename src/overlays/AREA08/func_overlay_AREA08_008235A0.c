// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4

asm void func_overlay_AREA08_008235A0(void) {
    lw $v1, 0x110($a0)
    lui $v0, (0x3E4CCCCD >> 16)
    ori $v0, $v0, (0x3E4CCCCD & 0xFFFF)
    mtc1 $v0, $f1
    mtc1 $zero, $f0
    lwc1 $f2, 0x7C($v1)
    add.s $f1, $f2, $f1
    swc1 $f1, 0x7C($v1)
    lw $v0, 0x110($a0)
    lwc1 $f1, 0x7C($v0)
    c.le.s $f1, $f0
    nop
    .word 0x45010004
    paddub $v0, $zero, $zero
    .word 0x10000002
    addiu $v0, $zero, 0x1
    paddub $v0, $zero, $zero
    jr $ra
    nop
}
