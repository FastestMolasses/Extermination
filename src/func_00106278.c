// Hybrid: branches/j as .word, jal with extern decls
extern void func_00105370(int, int, int, int);
extern void func_00106070(int, int, int, int);
extern void func_00106540(int, int, int, int);
extern void func_00106948(int, int, int, int);

asm void func_00106278(void) {
    addiu      $sp, $sp, -0x90
    sd         $s2, 0x20($sp)
    sd         $s7, 0x70($sp)
    daddu      $s2, $a0, $zero
    sd         $s6, 0x60($sp)
    daddu      $s7, $a1, $zero
    sd         $s5, 0x50($sp)
    daddu      $s6, $8, $zero
    sd         $s4, 0x40($sp)
    daddu      $s5, $9, $zero
    sd         $s3, 0x30($sp)
    daddu      $s4, $10, $zero
    sd         $s0, 0x0($sp)
    daddu      $s3, $a3, $zero
    sd         $s1, 0x10($sp)
    daddu      $s0, $a2, $zero
    sd         $ra, 0x80($sp)
    jal        func_00106540
    addiu     $a0, $zero, 0x2
    .word 0x12000007
    daddu     $s1, $v0, $zero
    .word 0x12200006
    daddu     $a3, $zero, $zero
    jal        func_00106948
    daddu     $a0, $s0, $zero
    .word 0x10000002
    daddu     $a3, $v0, $zero
    daddu      $a3, $zero, $zero
    daddu      $a1, $s0, $zero
    daddu      $a2, $s1, $zero
    daddu      $a0, $s2, $zero
    jal        func_00106070
    daddu     $8, $s4, $zero
    .word 0x12c00004
    nop
    jal        func_00105370
    nop
    sw         $v0, 0x0($s7)
    jal        func_00106540
    addiu     $a0, $zero, 0x2
    .word 0x12600007
    daddu     $s1, $v0, $zero
    .word 0x12200006
    daddu     $a3, $zero, $zero
    jal        func_00106948
    daddu     $a0, $s3, $zero
    .word 0x10000002
    daddu     $a3, $v0, $zero
    daddu      $a3, $zero, $zero
    .word 0x12a00004
    daddu     $a1, $s3, $zero
    lw         $v0, 0x4($s2)
    sra        $v0, $v0, 1
    sw         $v0, 0x4($s2)
    daddu      $a2, $s1, $zero
    daddu      $8, $s4, $zero
    jal        func_00106070
    addiu     $a0, $s2, 0x4
    .word 0x12a00004
    nop
    lw         $v0, 0x4($s2)
    sll        $v0, $v0, 1
    sw         $v0, 0x4($s2)
    .word 0x12c00005
    ld        $ra, 0x80($sp)
    jal        func_00105370
    nop
    sw         $v0, 0x4($s7)
    ld         $ra, 0x80($sp)
    ld         $s7, 0x70($sp)
    ld         $s6, 0x60($sp)
    ld         $s5, 0x50($sp)
    ld         $s4, 0x40($sp)
    ld         $s3, 0x30($sp)
    ld         $s2, 0x20($sp)
    ld         $s1, 0x10($sp)
    ld         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x90
}
