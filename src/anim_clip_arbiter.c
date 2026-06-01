// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void anim_clip_init(int, int, int, int);

asm void anim_clip_arbiter(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s1, $a0, $zero
    jal anim_clip_init
    paddub $s0, $a1, $zero
    lh $v0, 0x20C($s1)
    dsll32 $v1, $s0, 16
    dsra32 $v1, $v1, 16
    .word 0x10620005
    paddub $v0, $zero, $zero
    addiu $v0, $zero, 0x1
    .word 0x10000002
    sh $s0, 0x20C($s1)
    paddub $v0, $zero, $zero
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
