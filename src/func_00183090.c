// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void bone_init_default_2(int, int, int, int);
extern void anim_clip_init(int, int, int, int);
extern void func_001D0C70(int, int, int, int);

asm void func_00183090(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lui $at, (0x70003B8F >> 16)
    lbu $v1, (0x70003B8F & 0xFFFF)($at)
    addiu $v0, $zero, 0x2
    .word 0x14620003
    paddub $s0, $a0, $zero
    jal func_001D0C70
    nop
    lbu $v0, 0x2F3($s0)
    .word 0x10400016
    andi $a0, $v0, 0xFF
    addiu $v0, $zero, 0x1
    .word 0x14820009
    addiu $v1, $zero, 0x3
    lh $a1, 0x1F2($s0)
    jal bone_init_default_2
    paddub $a0, $s0, $zero
    addiu $v0, $zero, 0x2
    sw $zero, 0x200($s0)
    sb $v0, 0x2F3($s0)
    .word 0x10000019
    addiu $v0, $zero, 0x1
    .word 0x14830017
    nop
    lh $a1, 0x1F2($s0)
    jal bone_init_default_2
    paddub $a0, $s0, $zero
    addiu $v0, $zero, 0x4
    sw $zero, 0x200($s0)
    sb $v0, 0x2F3($s0)
    .word 0x1000000f
    addiu $v0, $zero, 0x1
    lh $v1, 0x1F2($s0)
    lh $v0, 0x20C($s0)
    .word 0x14620003
    addiu $v0, $zero, 0x1
    .word 0x1000000a
    lq $ra, 0x10($sp)
    sh $v1, 0x20C($s0)
    lh $a1, 0x20C($s0)
    lwc1 $f12, 0x1F8($s0)
    mtc1 $zero, $f13
    jal anim_clip_init
    paddub $a0, $s0, $zero
    sw $zero, 0x200($s0)
    paddub $v0, $zero, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
