// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4

asm void func_overlay_AREA21_008283D0(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x30($sp)
    sq $s1, 0x20($sp)
    sq $s0, 0x10($sp)
    swc1 $f22, 0x8($sp)
    swc1 $f21, 0x4($sp)
    swc1 $f20, 0x0($sp)
    lw $v0, 0x118($a0)
    paddub $s0, $a1, $zero
    paddub $s1, $a0, $zero
    lwc1 $f20, 0x7C($v0)
    lwc1 $f21, 0x80($v0)
    lwc1 $f22, 0x84($v0)
    sw $zero, 0x7C($v0)
    lw $v0, 0x118($a0)
    sw $zero, 0x80($v0)
}
