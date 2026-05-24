// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions, `la $r, SYM' for %hi/%lo pairs.
// CFLAGS: -O4,p -sdatathreshold 0
extern int D_overlay_AREA19_0082BD40;

asm void func_overlay_AREA19_008253E0(void) {
    addiu $sp, $sp, -0x70
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    la $v0, D_overlay_AREA19_0082BD40
    sq $s0, 0x0($sp)
    lq $8, 0x0($v0)
    lq $a3, 0x10($v0)
    lq $a2, 0x20($v0)
    lq $v1, 0x30($v0)
    paddub $s1, $a0, $zero
    addiu $9, $sp, 0x30
    addiu $a0, $s1, 0xB0
    sq $8, 0x0($9)
    lui $v0, (0x830000 >> 16)
    sq $a3, 0x10($9)
}
