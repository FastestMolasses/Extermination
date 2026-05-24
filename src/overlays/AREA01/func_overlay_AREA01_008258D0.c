// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4

asm void func_overlay_AREA01_008258D0(void) {
    lwc1 $f1, 0x3C($a0)
    lui $v0, (0x43FC8000 >> 16)
    ori $v0, $v0, (0x43FC8000 & 0xFFFF)
    mtc1 $v0, $f0
    nop
    c.le.s $f1, $f0
    nop
    .word 0x45000004
    paddub $v0, $zero, $zero
    .word 0x10000002
    addiu $v0, $zero, 0x1
    paddub $v0, $zero, $zero
    jr $ra
    nop
}
