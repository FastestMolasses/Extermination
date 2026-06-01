// Simple nonleaf asm void
extern void func_00122BB8(int, int, int, int);
extern void float_to_int(int, int, int, int);

asm void func_001E00F0(void) {
    addiu      $sp, $sp, -0x50
    sq         $ra, 0x40($sp)
    sq         $s2, 0x30($sp)
    sq         $s1, 0x20($sp)
    sq         $s0, 0x10($sp)
    paddub     $s1, $a0, $zero
    paddub     $s0, $a1, $zero
    paddub     $s2, $a2, $zero
    jal        func_00122BB8
    swc1      $f20, 0x0($sp)
    mtc1       $v0, $f0
    nop
    cvt.s.w    $f1, $f0
    lui        $v0, (0x30000000 >> 16)
    mtc1       $v0, $f0
    jal        func_00122BB8
    mul.s     $f20, $f0, $f1
    mtc1       $v0, $f0
    lui        $9, (0x41800000 >> 16)
    cvt.s.w    $f1, $f0
    lui        $v1, (0x30000000 >> 16)
    addiu      $v0, $zero, 0x80
    sw         $v0, 0x0($s1)
    sw         $v0, 0x4($s1)
    mtc1       $v1, $f0
    sw         $v0, 0x8($s1)
    mul.s      $f4, $f0, $f1
    lui        $v1, (0x41A00000 >> 16)
    lui        $v0, (0xC1200000 >> 16)
    sw         $s2, 0xC($s1)
    lui        $8, (0x45000000 >> 16)
    mtc1       $v1, $f0
    mtc1       $v0, $f3
    mul.s      $f2, $f0, $f20
    lwc1       $f1, 0x0($s0)
    lui        $a3, (0x44000000 >> 16)
    lui        $a2, (0x3F000000 >> 16)
    lui        $v1, (0x43600000 >> 16)
    add.s      $f2, $f3, $f2
    lui        $v0, (0x3F800000 >> 16)
    mtc1       $9, $f5
    nop
    mul.s      $f0, $f0, $f4
    mul.s      $f2, $f5, $f2
    add.s      $f6, $f1, $f2
    div.s      $f6, $f6, $f5
    mtc1       $8, $f4
    mtc1       $a3, $f2
    sub.s      $f6, $f6, $f4
    div.s      $f6, $f6, $f2
    add.s      $f0, $f3, $f0
    mtc1       $a2, $f3
    mul.s      $f1, $f5, $f0
    add.s      $f6, $f6, $f3
    swc1       $f6, 0x10($s1)
    lwc1       $f2, 0x4($s0)
    mtc1       $v1, $f0
    nop
    add.s      $f1, $f2, $f1
    div.s      $f1, $f1, $f5
    sub.s      $f1, $f1, $f4
    div.s      $f1, $f1, $f0
    add.s      $f1, $f1, $f3
    swc1       $f1, 0x14($s1)
    sw         $v0, 0x18($s1)
    jal        float_to_int
    lwc1      $f12, 0x0($s0)
    sw         $v0, 0x20($s1)
    jal        float_to_int
    lwc1      $f12, 0x4($s0)
    sw         $v0, 0x24($s1)
    jal        float_to_int
    lwc1      $f12, 0x8($s0)
    addiu      $v1, $zero, 0xFF
    sw         $v0, 0x28($s1)
    sw         $v1, 0x2C($s1)
    lq         $ra, 0x40($sp)
    lq         $s2, 0x30($sp)
    lq         $s1, 0x20($sp)
    lq         $s0, 0x10($sp)
    lwc1       $f20, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x50
}
