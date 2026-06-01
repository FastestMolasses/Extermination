// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void anim_clip_init(int, int, int, int);

asm void func_001BFF90(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lh $a3, 0x2($a1)
    dsll32 $v1, $a2, 16
    dsra32 $v1, $v1, 16
    .word 0x10e30007
    nop
    lui $v0, (0x40800000 >> 16)
    sh $a2, 0x2($a1)
    mtc1 $zero, $f13
    mtc1 $v0, $f12
    jal anim_clip_init
    paddub $a1, $a2, $zero
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
