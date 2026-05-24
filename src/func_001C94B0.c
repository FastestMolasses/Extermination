// Simple nonleaf asm void
extern void func_00102918(int, int, int, int);
extern void func_001029C0(int, int, int, int);
extern void func_00102A60(int, int, int, int);
extern void func_00102B08(int, int, int, int);
extern void func_00102BB0(int, int, int, int);

asm void func_001C94B0(void) {
    addiu      $sp, $sp, -0x50
    sq         $ra, 0x40($sp)
    sq         $s3, 0x30($sp)
    sq         $s2, 0x20($sp)
    sq         $s1, 0x10($sp)
    sq         $s0, 0x0($sp)
    paddub     $s2, $a1, $zero
    paddub     $s1, $a2, $zero
    paddub     $s0, $a3, $zero
    jal        func_001029C0
    paddub    $s3, $a0, $zero
    lwc1       $f12, 0x0($s1)
    paddub     $a0, $s3, $zero
    jal        func_00102B08
    paddub    $a1, $s3, $zero
    lwc1       $f12, 0x4($s1)
    paddub     $a0, $s3, $zero
    jal        func_00102BB0
    paddub    $a1, $s3, $zero
    lwc1       $f12, 0x8($s1)
    paddub     $a0, $s3, $zero
    jal        func_00102A60
    paddub    $a1, $s3, $zero
    lqc2       $vf4, 0x0($s3)
    lqc2       $vf5, 0x10($s3)
    lqc2       $vf6, 0x20($s3)
    lqc2       $vf7, 0x30($s3)
    lqc2       $vf8, 0x0($s0)
    .word 0x4bc82118  /* was: vmulx.xyz  $vf4, $vf4, $vf8x */
    .word 0x4bc82959  /* was: vmuly.xyz  $vf5, $vf5, $vf8y */
    .word 0x4bc8319a  /* was: vmulz.xyz  $vf6, $vf6, $vf8z */
    sqc2       $vf4, 0x0($s3)
    sqc2       $vf5, 0x10($s3)
    sqc2       $vf6, 0x20($s3)
    sqc2       $vf7, 0x30($s3)
    paddub     $a2, $s2, $zero
    paddub     $a0, $s3, $zero
    jal        func_00102918
    paddub    $a1, $s3, $zero
    lq         $ra, 0x40($sp)
    lq         $s3, 0x30($sp)
    lq         $s2, 0x20($sp)
    lq         $s1, 0x10($sp)
    lq         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x50
}
