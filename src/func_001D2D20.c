// Simple nonleaf asm void
extern void func_001029C0(int, int, int, int);

asm void func_001D2D20(void) {
    addiu      $sp, $sp, -0x40
    sq         $ra, 0x30($sp)
    sq         $s0, 0x20($sp)
    swc1       $f24, 0x10($sp)
    swc1       $f23, 0xC($sp)
    swc1       $f22, 0x8($sp)
    swc1       $f21, 0x4($sp)
    swc1       $f20, 0x0($sp)
    mov.s      $f24, $f12
    mov.s      $f23, $f13
    mov.s      $f22, $f14
    mov.s      $f21, $f15
    mov.s      $f20, $f16
    jal        func_001029C0
    paddub    $s0, $a0, $zero
    lui        $v1, (0x3F000000 >> 16)
    mtc1       $v1, $f0
    lui        $a0, (0xC0000000 >> 16)
    mul.s      $f1, $f0, $f23
    lui        $v1, (0x3F800000 >> 16)
    mul.s      $f0, $f0, $f22
    div.s      $f1, $f24, $f1
    swc1       $f1, 0x0($s0)
    div.s      $f0, $f24, $f0
    swc1       $f0, 0x14($s0)
    mul.s      $f1, $f20, $f21
    mtc1       $a0, $f0
    add.s      $f2, $f20, $f21
    mul.s      $f0, $f0, $f1
    sub.s      $f3, $f20, $f21
    div.s      $f1, $f2, $f3
    div.s      $f0, $f0, $f3
    swc1       $f1, 0x28($s0)
    swc1       $f0, 0x38($s0)
    sw         $v1, 0x2C($s0)
    sw         $zero, 0x3C($s0)
    lq         $ra, 0x30($sp)
    lq         $s0, 0x20($sp)
    lwc1       $f24, 0x10($sp)
    lwc1       $f23, 0xC($sp)
    lwc1       $f22, 0x8($sp)
    lwc1       $f21, 0x4($sp)
    lwc1       $f20, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x40
}
