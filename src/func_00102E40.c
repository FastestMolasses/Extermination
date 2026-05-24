// Hybrid: branches/j as .word, jal with extern decls
extern void func_00102948(int, int, int, int);
extern void func_00102948(int, int, int, int);

asm void func_00102E40(void) {
    addiu      $sp, $sp, -0x50
    sd         $s0, 0x0($sp)
    daddu      $s0, $a0, $zero
    sd         $s3, 0x30($sp)
    sd         $s2, 0x20($sp)
    daddu      $s3, $8, $zero
    sd         $s1, 0x10($sp)
    daddu      $s2, $a3, $zero
    sd         $ra, 0x40($sp)
    jal        func_00102948
    daddu     $s1, $a2, $zero
    daddu      $a1, $s1, $zero
    jal        func_00102948
    addiu     $a0, $s0, 0x10
    daddu      $a1, $s2, $zero
    jal        func_00102948
    addiu     $a0, $s0, 0x20
    addiu      $a0, $s0, 0x30
    daddu      $a1, $s3, $zero
    ld         $ra, 0x40($sp)
    ld         $s3, 0x30($sp)
    ld         $s2, 0x20($sp)
    ld         $s1, 0x10($sp)
    ld         $s0, 0x0($sp)
    j         func_00102948
    addiu     $sp, $sp, 0x50
}
