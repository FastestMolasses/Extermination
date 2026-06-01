// Hybrid: branches/j as .word, jal with extern decls
extern void block_copy(int, int, int, int);
extern void func_00121AF8(int, int, int, int);
extern void func_00121BA0(int, int, int, int);

asm void func_00121BD0(void) {
    addiu      $sp, $sp, -0x60
    daddu      $8, $a2, $zero
    sd         $s4, 0x40($sp)
    daddu      $9, $zero, $zero
    sd         $s3, 0x30($sp)
    daddu      $s4, $a0, $zero
    sd         $s1, 0x10($sp)
    daddu      $s3, $a3, $zero
    sd         $ra, 0x50($sp)
    daddu      $s1, $a1, $zero
    sd         $s0, 0x0($sp)
    addiu      $10, $s1, 0x14
    sd         $s2, 0x20($sp)
    daddu      $a3, $10, $zero
    lw         $s2, 0x10($s1)
    nop
    lw         $v1, 0x0($a3)
    addiu      $9, $9, 0x1
    slt        $a2, $9, $s2
    .word 0x3064ffff
    mult       $v0, $a0, $8
    srl        $v1, $v1, 16
    mult       $v1, $v1, $8
    addu       $a0, $v0, $s3
    srl        $a1, $a0, 16
    addu       $v1, $v1, $a1
    .word 0x3084ffff
    sll        $v0, $v1, 16
    srl        $s3, $v1, 16
    addu       $v0, $v0, $a0
    sw         $v0, 0x0($a3)
    .word 0x14c0fff0
    addiu     $a3, $a3, 0x4
    .word 0x1260001b
    daddu     $v0, $s1, $zero
    lw         $v0, 0x8($s1)
    slt        $v0, $s2, $v0
    .word 0x14400012
    sll       $v0, $s2, 2
    lw         $a1, 0x4($s1)
    daddu      $a0, $s4, $zero
    jal        func_00121AF8
    addiu     $a1, $a1, 0x1
    lw         $a2, 0x10($s1)
    daddu      $s0, $v0, $zero
    addiu      $a1, $s1, 0xC
    addiu      $a0, $s0, 0xC
    sll        $a2, $a2, 2
    jal        block_copy
    addiu     $a2, $a2, 0x8
    daddu      $a1, $s1, $zero
    jal        func_00121BA0
    daddu     $a0, $s4, $zero
    daddu      $s1, $s0, $zero
    addiu      $10, $s1, 0x14
    sll        $v0, $s2, 2
    addu       $v0, $10, $v0
    addiu      $s2, $s2, 0x1
    sw         $s3, 0x0($v0)
    sw         $s2, 0x10($s1)
    daddu      $v0, $s1, $zero
    ld         $ra, 0x50($sp)
    ld         $s4, 0x40($sp)
    ld         $s3, 0x30($sp)
    ld         $s2, 0x20($sp)
    ld         $s1, 0x10($sp)
    ld         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x60
}
