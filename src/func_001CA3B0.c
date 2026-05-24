// Simple nonleaf asm void
extern void func_0011DE90(int, int, int, int);
extern void func_0011E2A8(int, int, int, int);

asm void func_001CA3B0(void) {
    addiu      $sp, $sp, -0x40
    sq         $ra, 0x30($sp)
    sq         $s0, 0x20($sp)
    swc1       $f24, 0x10($sp)
    lui        $v0, (0x40000000 >> 16)
    swc1       $f23, 0xC($sp)
    swc1       $f22, 0x8($sp)
    mtc1       $v0, $f0
    swc1       $f21, 0x4($sp)
    div.s      $f22, $f12, $f0
    swc1       $f20, 0x0($sp)
    paddub     $s0, $a0, $zero
    mov.s      $f21, $f13
    mov.s      $f24, $f14
    nop
    nop
    jal        func_0011E2A8
    mov.s     $f12, $f22
    mov.s      $f20, $f0
    lui        $v0, (0x40000000 >> 16)
    mtc1       $v0, $f0
    nop
    div.s      $f23, $f21, $f0
    nop
    nop
    jal        func_0011E2A8
    mov.s     $f12, $f23
    mov.s      $f21, $f0
    lui        $v0, (0x40000000 >> 16)
    mtc1       $v0, $f0
    nop
    div.s      $f24, $f24, $f0
    nop
    nop
    jal        func_0011E2A8
    mov.s     $f12, $f24
    mov.s      $f12, $f22
    jal        func_0011DE90
    mov.s     $f22, $f0
    mov.s      $f12, $f23
    jal        func_0011DE90
    mov.s     $f23, $f0
    mov.s      $f12, $f24
    jal        func_0011DE90
    mov.s     $f24, $f0
    mul.s      $f1, $f22, $f24
    mul.s      $f4, $f0, $f24
    mul.s      $f2, $f0, $f21
    mula.s     $f23, $f1
    msub.s     $f0, $f20, $f2
    mul.s      $f3, $f22, $f21
    swc1       $f0, 0x0($s0)
    mula.s     $f23, $f2
    madd.s     $f0, $f20, $f1
    swc1       $f0, 0x4($s0)
    mula.s     $f20, $f4
    msub.s     $f0, $f23, $f3
    mula.s     $f23, $f4
    swc1       $f0, 0x8($s0)
    madd.s     $f0, $f20, $f3
    swc1       $f0, 0xC($s0)
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
