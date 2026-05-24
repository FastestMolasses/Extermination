// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions, `la $r, SYM' for %hi/%lo pairs.
// CFLAGS: -O4,p -sdatathreshold 0
extern int D_overlay_AREA19_0082BD00;

asm void func_overlay_AREA19_00825200(void) {
    addiu $sp, $sp, -0x70
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    la $v1, D_overlay_AREA19_0082BD00
    sq $s0, 0x0($sp)
    lq $9, 0x0($v1)
    lq $8, 0x10($v1)
    lq $a3, 0x20($v1)
    lq $a1, 0x30($v1)
    addiu $a2, $sp, 0x30
    paddub $s1, $a0, $zero
    sq $9, 0x0($a2)
    sq $8, 0x10($a2)
    sq $a3, 0x20($a2)
    sq $a1, 0x30($a2)
}
