// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4

asm void func_overlay_AREA21_00829F00(void) {
    addiu $sp, $sp, -0xF0
    sq $ra, 0x40($sp)
    sq $s2, 0x30($sp)
    sq $s1, 0x20($sp)
}
