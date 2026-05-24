// Hybrid: branches/j as .word, jal with extern decls
extern void func_00102948(int, int, int, int);
extern void func_001EF9D0(int, int, int, int);

asm void func_001EFF10(void) {
    addiu      $sp, $sp, -0x80
    sq         $ra, 0x70($sp)
    sq         $s5, 0x60($sp)
    sq         $s4, 0x50($sp)
    sq         $s3, 0x40($sp)
    sq         $s2, 0x30($sp)
    sq         $s1, 0x20($sp)
    sq         $s0, 0x10($sp)
    swc1       $f20, 0x0($sp)
    paddub     $s5, $a1, $zero
    mov.s      $f20, $f12
    lui        $v0, (0x3F800000 >> 16)
    mtc1       $v0, $f12
    addiu      $a1, $s5, 0x30
    paddub     $s1, $a2, $zero
    paddub     $s4, $a3, $zero
    paddub     $s3, $8, $zero
    jal        func_001EF9D0
    paddub    $s2, $9, $zero
    paddub     $s0, $v0, $zero
    .word 0x12000011
    paddub    $v0, $s0, $zero
    paddub     $a1, $s1, $zero
    addiu      $s1, $s0, 0x1F0
    addiu      $a0, $s1, 0x10
    sw         $s5, 0x1F0($s0)
    jal        func_00102948
    swc1      $f20, 0x1FC($s0)
    addiu      $a0, $s1, 0x20
    jal        func_00102948
    paddub    $a1, $s4, $zero
    addiu      $a0, $s1, 0x30
    jal        func_00102948
    paddub    $a1, $s3, $zero
    addiu      $a0, $s1, 0x40
    jal        func_00102948
    paddub    $a1, $s2, $zero
    paddub     $v0, $s0, $zero
    lq         $ra, 0x70($sp)
    lq         $s5, 0x60($sp)
    lq         $s4, 0x50($sp)
    lq         $s3, 0x40($sp)
    lq         $s2, 0x30($sp)
    lq         $s1, 0x20($sp)
    lq         $s0, 0x10($sp)
    lwc1       $f20, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x80
}
