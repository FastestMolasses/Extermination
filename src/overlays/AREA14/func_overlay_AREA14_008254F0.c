// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4

asm void func_overlay_AREA14_008254F0(void) {
    addiu $sp, $sp, -0x80
    sq $ra, 0x50($sp)
    sq $s4, 0x40($sp)
    sq $s3, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lui $v0, (0x42700000 >> 16)
    lui $at, (0x700038A0 >> 16)
    sw $v0, (0x700038A0 & 0xFFFF)($at)
    lui $v0, (0x40400000 >> 16)
    sw $v0, 0x60($sp)
    lui $v0, (0xC0000000 >> 16)
    sw $v0, 0x64($sp)
    sw $zero, 0x68($sp)
    lui $v0, (0x3F800000 >> 16)
}
