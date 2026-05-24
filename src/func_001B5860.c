// Hybrid: branches/j as .word, jal with extern decls
extern void func_001108E8(int, int, int, int);
extern void func_00121A28(int, int, int, int);

asm void func_001B5860(void) {
    addiu      $sp, $sp, -0x50
    sq         $ra, 0x40($sp)
    sq         $s3, 0x30($sp)
    sq         $s2, 0x20($sp)
    sq         $s1, 0x10($sp)
    sq         $s0, 0x0($sp)
    paddub     $s3, $a2, $zero
    paddub     $s0, $a1, $zero
    addiu      $a2, $zero, 0xC
    paddub     $s2, $a3, $zero
    paddub     $s1, $8, $zero
    jal        func_00121A28
    paddub    $a1, $zero, $zero
    addiu      $a2, $zero, 0x2C
    paddub     $a0, $s0, $zero
    jal        func_00121A28
    paddub    $a1, $zero, $zero
    sw         $s1, 0x0($s0)
    sw         $s3, 0x4($s0)
    addiu      $v0, $zero, 0x80
    sw         $s2, 0x8($s0)
    sb         $v0, 0x24($s0)
    sb         $v0, 0x25($s0)
    lw         $a2, 0x0($s0)
    paddub     $a0, $s3, $zero
    jal        func_001108E8
    paddub    $a1, $s2, $zero
    paddub     $a0, $zero, $zero
    addu       $v1, $s0, $a0
    addiu      $a0, $a0, 0x1
    sb         $zero, 0x18($v1)
    slti       $v1, $a0, 0x6
    nop
    .word 0x1460fffa
    nop
    addiu      $v1, $zero, 0x1
    sb         $zero, 0x1E($s0)
    addiu      $a1, $zero, 0x2
    sb         $v1, 0x1F($s0)
    addiu      $a0, $zero, 0xFF
    addu       $v1, $s0, $a1
    addiu      $a1, $a1, 0x1
    sb         $a0, 0x1E($v1)
    slti       $v1, $a1, 0x6
    nop
    .word 0x1460fffa
    nop
    lq         $ra, 0x40($sp)
    lq         $s3, 0x30($sp)
    lq         $s2, 0x20($sp)
    lq         $s1, 0x10($sp)
    lq         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x50
}
