// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4

asm void overlay_AREA08_func_00823540(void) {
    lw $v1, 0x110($a0)
    lui $v0, (0x3E4CCCCD >> 16)
    ori $v0, $v0, (0x3E4CCCCD & 0xFFFF)
    mtc1 $v0, $f1
    lui $v0, (0xC1100000 >> 16)
    lwc1 $f2, 0x7C($v1)
    mtc1 $v0, $f0
    nop
    sub.s $f1, $f2, $f1
    swc1 $f1, 0x7C($v1)
    lw $v0, 0x110($a0)
    lwc1 $f1, 0x7C($v0)
    c.lt.s $f1, $f0
    nop
    .word 0x45000004
    paddub $v0, $zero, $zero
    .word 0x10000002
    addiu $v0, $zero, 0x1
    paddub $v0, $zero, $zero
    jr $ra
    nop
}
