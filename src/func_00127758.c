// Simple nonleaf asm void
extern void func_00126BE8(int, int, int, int);
extern void func_00128320(int, int, int, int);

asm void func_00127758(void) {
    addiu      $sp, $sp, -0x40
    sd         $a0, 0x20($sp)
    daddu      $a1, $sp, $zero
    sd         $ra, 0x30($sp)
    jal        func_00126BE8
    addiu     $a0, $sp, 0x20
    ld         $v0, 0x10($sp)
    lui        $v1, (0x3FFFFFFF >> 16)
    .word 0x3463ffff
    lw         $a0, 0x0($sp)
    dsll       $8, $v0, 2
    dsra32     $8, $8, 0
    lw         $a1, 0x4($sp)
    and        $v0, $v0, $v1
    ori        $a3, $8, 0x1
    lw         $a2, 0x8($sp)
    jal        func_00128320
    movz      $a3, $8, $v0
    ld         $ra, 0x30($sp)
    jr         $ra
    addiu     $sp, $sp, 0x40
}
