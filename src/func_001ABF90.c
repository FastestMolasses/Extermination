// Simple nonleaf asm void
extern void func_00207D00(int, int, int, int);
extern void func_00207E40(int, int, int, int);

asm void func_001ABF90(void) {
    addiu      $sp, $sp, -0x50
    sq         $ra, 0x40($sp)
    sq         $s3, 0x30($sp)
    sq         $s2, 0x20($sp)
    sq         $s1, 0x10($sp)
    paddub     $s3, $a0, $zero
    sq         $s0, 0x0($sp)
    paddub     $s2, $a1, $zero
    addiu      $a0, $zero, 0x1
    paddub     $s1, $a2, $zero
    paddub     $s0, $a3, $zero
    jal        func_00207D00
    paddub    $a1, $zero, $zero
    addiu      $a3, $zero, 0x100
    lui        $v0, (0x80808080 >> 16)
    addiu      $a0, $zero, 0x1
    addiu      $a1, $zero, 0x7000
    paddub     $10, $s1, $zero
    ori        $a2, $zero, 0x8100
    paddub     $8, $a3, $zero
    jal        func_00207E40
    .word 0x34498080
    addiu      $a3, $zero, 0x100
    lui        $v0, (0x80808080 >> 16)
    addiu      $a0, $zero, 0x1
    paddub     $10, $s0, $zero
    ori        $a1, $zero, 0x8000
    ori        $a2, $zero, 0x8100
    paddub     $8, $a3, $zero
    jal        func_00207E40
    .word 0x34498080
    addiu      $a3, $zero, 0x100
    lui        $v0, (0x80808080 >> 16)
    addiu      $a0, $zero, 0x1
    addiu      $a1, $zero, 0x7000
    addiu      $a2, $zero, 0x7900
    paddub     $10, $s3, $zero
    paddub     $8, $a3, $zero
    jal        func_00207E40
    .word 0x34498080
    addiu      $a3, $zero, 0x100
    lui        $v0, (0x80808080 >> 16)
    addiu      $a0, $zero, 0x1
    addiu      $a2, $zero, 0x7900
    paddub     $10, $s2, $zero
    ori        $a1, $zero, 0x8000
    paddub     $8, $a3, $zero
    jal        func_00207E40
    .word 0x34498080
    lq         $ra, 0x40($sp)
    lq         $s3, 0x30($sp)
    lq         $s2, 0x20($sp)
    lq         $s1, 0x10($sp)
    lq         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x50
}
