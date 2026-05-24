// Hybrid: branches/j as .word, jal with extern decls
extern void func_00106278(int, int, int, int);
extern void func_00106948(int, int, int, int);
extern void func_00106278(int, int, int, int);

asm void func_001060F8(void) {
    addiu      $sp, $sp, -0xA0
    addiu      $v0, $zero, 0x1
    sd         $fp, 0x80($sp)
    sd         $s7, 0x70($sp)
    daddu      $fp, $a2, $zero
    sd         $s6, 0x60($sp)
    sd         $s5, 0x50($sp)
    daddu      $s6, $a0, $zero
    sd         $s4, 0x40($sp)
    daddu      $s5, $11, $zero
    sd         $s3, 0x30($sp)
    daddu      $s4, $a1, $zero
    sd         $s2, 0x20($sp)
    daddu      $s3, $10, $zero
    sd         $s0, 0x0($sp)
    sd         $ra, 0x90($sp)
    daddu      $s0, $a3, $zero
    sd         $s1, 0x10($sp)
    lw         $s2, 0xA0($sp)
    .word 0x1502000c
    lw        $s7, 0xA8($sp)
    .word 0x55200030
    sll       $s0, $s0, 3
    .word 0x5640002e
    sll       $s0, $s0, 3
    jal        func_00106948
    addiu     $a0, $zero, 0x1
    sll        $v1, $s0, 2
    addu       $v1, $v1, $fp
    sw         $v0, 0x8($v1)
    .word 0x10000026
    sw        $v0, 0x0($v1)
    addiu      $a0, $zero, 0x1
    jal        func_00106948
    sll       $s1, $s0, 3
    sll        $s0, $s0, 2
    addu       $a0, $s6, $s1
    addu       $s0, $s0, $fp
    daddu      $a1, $s4, $zero
    sw         $v0, 0x0($s0)
    daddu      $a2, $s3, $zero
    daddu      $a3, $s5, $zero
    daddu      $8, $s2, $zero
    daddu      $9, $s7, $zero
    jal        func_00106278
    daddu     $10, $zero, $zero
    jal        func_00106948
    addiu     $a0, $zero, 0x1
    addiu      $s1, $s1, 0x10
    sw         $v0, 0x8($s0)
    addu       $a0, $s6, $s1
    daddu      $a1, $s4, $zero
    daddu      $a2, $s3, $zero
    daddu      $a3, $s5, $zero
    daddu      $8, $s2, $zero
    daddu      $9, $s7, $zero
    ld         $ra, 0x90($sp)
    daddu      $10, $zero, $zero
    ld         $fp, 0x80($sp)
    ld         $s7, 0x70($sp)
    ld         $s6, 0x60($sp)
    ld         $s5, 0x50($sp)
    ld         $s4, 0x40($sp)
    ld         $s3, 0x30($sp)
    ld         $s2, 0x20($sp)
    ld         $s1, 0x10($sp)
    ld         $s0, 0x0($sp)
    j         func_00106278
    addiu     $sp, $sp, 0xA0
    sll        $s0, $s0, 3
    daddu      $a1, $s4, $zero
    addu       $s0, $s6, $s0
    daddu      $a2, $s3, $zero
    daddu      $a3, $s5, $zero
    daddu      $8, $s2, $zero
    daddu      $9, $s7, $zero
    daddu      $a0, $s0, $zero
    jal        func_00106278
    daddu     $10, $zero, $zero
    lw         $v0, 0x0($s0)
    lw         $v1, 0x4($s0)
    sw         $v0, 0x10($s0)
    sw         $v1, 0x14($s0)
    ld         $ra, 0x90($sp)
    ld         $fp, 0x80($sp)
    ld         $s7, 0x70($sp)
    ld         $s6, 0x60($sp)
    ld         $s5, 0x50($sp)
    ld         $s4, 0x40($sp)
    ld         $s3, 0x30($sp)
    ld         $s2, 0x20($sp)
    ld         $s1, 0x10($sp)
    ld         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0xA0
}
