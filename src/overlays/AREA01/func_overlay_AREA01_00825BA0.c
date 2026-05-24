// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions, `la $r, SYM' for %hi/%lo pairs.
// CFLAGS: -O4,p -sdatathreshold 0
extern int D_overlay_AREA01_0082B090;

asm void func_overlay_AREA01_00825BA0(void) {
    addiu $sp, $sp, -0x70
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    la $v0, D_overlay_AREA01_0082B090
    sq $s0, 0x0($sp)
    lq $8, 0x0($v0)
    lq $a3, 0x10($v0)
    lq $a1, 0x20($v0)
    lq $v1, 0x30($v0)
    addiu $a2, $sp, 0x30
    paddub $s1, $a0, $zero
    sq $8, 0x0($a2)
    sq $a3, 0x10($a2)
    sq $a1, 0x20($a2)
    sq $v1, 0x30($a2)
}
