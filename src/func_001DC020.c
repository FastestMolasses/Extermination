// Simple nonleaf asm void
extern void float_to_int(int, int, int, int);
extern void func_00207F80(int, int, int, int);

asm void func_001DC020(void) {
    addiu      $sp, $sp, -0xA0
    sq         $ra, 0x90($sp)
    lui        $v0, (0x3F10B216 >> 16)
    sq         $s6, 0x80($sp)
    .word 0x3442b216
    sq         $s5, 0x70($sp)
    mtc1       $v0, $f1
    sq         $s4, 0x60($sp)
    sq         $s3, 0x50($sp)
    lui        $v0, (0x3EDE9BD4 >> 16)
    sq         $s2, 0x40($sp)
    .word 0x34429bd4
    mtc1       $v0, $f0
    sq         $s1, 0x30($sp)
    sq         $s0, 0x20($sp)
    swc1       $f24, 0x10($sp)
    swc1       $f23, 0xC($sp)
    swc1       $f22, 0x8($sp)
    swc1       $f21, 0x4($sp)
    mul.s      $f0, $f0, $f12
    swc1       $f20, 0x0($sp)
    lui        $v0, (0xC2F40000 >> 16)
    lui        $v1, (0x43800000 >> 16)
    paddub     $s0, $a0, $zero
    add.s      $f20, $f1, $f0
    mtc1       $v0, $f0
    mtc1       $v1, $f1
    nop
    mul.s      $f0, $f0, $f20
    jal        float_to_int
    add.s     $f12, $f1, $f0
    paddub     $s3, $v0, $zero
    lui        $v0, (0x435E0000 >> 16)
    mtc1       $v0, $f1
    lui        $v0, (0xC2EE0000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f0, $f0, $f20
    jal        float_to_int
    add.s     $f12, $f1, $f0
    paddub     $s4, $v0, $zero
    lui        $v0, (0x43800000 >> 16)
    mtc1       $v0, $f1
    lui        $v0, (0x42F40000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f0, $f0, $f20
    jal        float_to_int
    add.s     $f12, $f1, $f0
    paddub     $s5, $v0, $zero
    lui        $v0, (0x435E0000 >> 16)
    mtc1       $v0, $f1
    lui        $v0, (0x42EE0000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f0, $f0, $f20
    jal        float_to_int
    add.s     $f12, $f1, $f0
    paddub     $s6, $v0, $zero
    addiu      $v0, $s3, 0x700
    mtc1       $v0, $f0
    nop
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f22, $f0, $f1
    jal        float_to_int
    mov.s     $f12, $f22
    paddub     $s2, $v0, $zero
    sra        $v0, $s4, 1
    addiu      $v0, $v0, 0x790
    mtc1       $v0, $f0
    nop
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f24, $f0, $f1
    jal        float_to_int
    mov.s     $f12, $f24
    paddub     $s1, $v0, $zero
    addiu      $v0, $s5, 0x700
    mtc1       $v0, $f0
    nop
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f21, $f0, $f1
    jal        float_to_int
    mov.s     $f12, $f21
    addiu      $v1, $s4, 0x2
    sra        $v1, $v1, 1
    addiu      $v1, $v1, 0x790
    mtc1       $v1, $f0
    paddub     $s4, $v0, $zero
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    jal        float_to_int
    mul.s     $f12, $f0, $f1
    paddub     $8, $v0, $zero
    lui        $v0, (0x801996FF >> 16)
    paddub     $a1, $s2, $zero
    paddub     $a2, $s1, $zero
    paddub     $a3, $s4, $zero
    paddub     $a0, $s0, $zero
    jal        func_00207F80
    .word 0x344996ff
    jal        float_to_int
    mov.s     $f12, $f22
    paddub     $s2, $v0, $zero
    sra        $v0, $s6, 1
    addiu      $v0, $v0, 0x790
    mtc1       $v0, $f0
    nop
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f23, $f0, $f1
    jal        float_to_int
    mov.s     $f12, $f23
    paddub     $s1, $v0, $zero
    jal        float_to_int
    mov.s     $f12, $f21
    addiu      $v1, $s6, -0x2
    sra        $v1, $v1, 1
    addiu      $v1, $v1, 0x790
    mtc1       $v1, $f0
    paddub     $s4, $v0, $zero
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    jal        float_to_int
    mul.s     $f12, $f0, $f1
    paddub     $8, $v0, $zero
    lui        $v0, (0x801996FF >> 16)
    paddub     $a1, $s2, $zero
    paddub     $a2, $s1, $zero
    paddub     $a3, $s4, $zero
    paddub     $a0, $s0, $zero
    jal        func_00207F80
    .word 0x344996ff
    jal        float_to_int
    mov.s     $f12, $f22
    paddub     $s4, $v0, $zero
    jal        float_to_int
    mov.s     $f12, $f24
    addiu      $v1, $s3, 0x702
    mtc1       $v1, $f0
    paddub     $s2, $v0, $zero
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    jal        float_to_int
    mul.s     $f12, $f0, $f1
    paddub     $s1, $v0, $zero
    jal        float_to_int
    mov.s     $f12, $f23
    paddub     $8, $v0, $zero
    lui        $v0, (0x801996FF >> 16)
    paddub     $a1, $s4, $zero
    paddub     $a2, $s2, $zero
    paddub     $a3, $s1, $zero
    paddub     $a0, $s0, $zero
    jal        func_00207F80
    .word 0x344996ff
    jal        float_to_int
    mov.s     $f12, $f21
    mov.s      $f12, $f24
    jal        float_to_int
    paddub    $s3, $v0, $zero
    addiu      $v1, $s5, 0x6FE
    mtc1       $v1, $f0
    paddub     $s2, $v0, $zero
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    jal        float_to_int
    mul.s     $f12, $f0, $f1
    mov.s      $f12, $f23
    jal        float_to_int
    paddub    $s1, $v0, $zero
    paddub     $8, $v0, $zero
    lui        $v0, (0x801996FF >> 16)
    paddub     $a1, $s3, $zero
    paddub     $a2, $s2, $zero
    paddub     $a3, $s1, $zero
    paddub     $a0, $s0, $zero
    jal        func_00207F80
    .word 0x344996ff
    lui        $v0, (0x43800000 >> 16)
    mtc1       $v0, $f1
    lui        $v0, (0xC1C00000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f0, $f0, $f20
    jal        float_to_int
    add.s     $f12, $f1, $f0
    paddub     $s3, $v0, $zero
    lui        $v0, (0x435E0000 >> 16)
    mtc1       $v0, $f1
    lui        $v0, (0xC0C00000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f0, $f0, $f20
    jal        float_to_int
    add.s     $f12, $f1, $f0
    paddub     $s4, $v0, $zero
    lui        $v0, (0x43800000 >> 16)
    mtc1       $v0, $f1
    lui        $v0, (0x41C00000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f0, $f0, $f20
    jal        float_to_int
    add.s     $f12, $f1, $f0
    paddub     $s5, $v0, $zero
    lui        $v0, (0x435E0000 >> 16)
    mtc1       $v0, $f1
    lui        $v0, (0x40C00000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f0, $f0, $f20
    jal        float_to_int
    add.s     $f12, $f1, $f0
    paddub     $s6, $v0, $zero
    addiu      $v0, $s3, 0x700
    mtc1       $v0, $f0
    nop
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f21, $f0, $f1
    jal        float_to_int
    mov.s     $f12, $f21
    paddub     $s2, $v0, $zero
    sra        $v0, $s4, 1
    addiu      $v0, $v0, 0x790
    mtc1       $v0, $f0
    nop
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f23, $f0, $f1
    jal        float_to_int
    mov.s     $f12, $f23
    paddub     $s1, $v0, $zero
    addiu      $v0, $s5, 0x700
    mtc1       $v0, $f0
    nop
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f20, $f0, $f1
    jal        float_to_int
    mov.s     $f12, $f20
    addiu      $v1, $s4, 0x2
    sra        $v1, $v1, 1
    addiu      $v1, $v1, 0x790
    mtc1       $v1, $f0
    paddub     $s4, $v0, $zero
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    jal        float_to_int
    mul.s     $f12, $f0, $f1
    paddub     $8, $v0, $zero
    lui        $v0, (0x801996FF >> 16)
    paddub     $a1, $s2, $zero
    paddub     $a2, $s1, $zero
    paddub     $a3, $s4, $zero
    paddub     $a0, $s0, $zero
    jal        func_00207F80
    .word 0x344996ff
    jal        float_to_int
    mov.s     $f12, $f21
    paddub     $s2, $v0, $zero
    sra        $v0, $s6, 1
    addiu      $v0, $v0, 0x790
    mtc1       $v0, $f0
    nop
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    nop
    mul.s      $f22, $f0, $f1
    jal        float_to_int
    mov.s     $f12, $f22
    paddub     $s1, $v0, $zero
    jal        float_to_int
    mov.s     $f12, $f20
    addiu      $v1, $s6, -0x2
    sra        $v1, $v1, 1
    addiu      $v1, $v1, 0x790
    mtc1       $v1, $f0
    paddub     $s4, $v0, $zero
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    jal        float_to_int
    mul.s     $f12, $f0, $f1
    paddub     $8, $v0, $zero
    lui        $v0, (0x801996FF >> 16)
    paddub     $a1, $s2, $zero
    paddub     $a2, $s1, $zero
    paddub     $a3, $s4, $zero
    paddub     $a0, $s0, $zero
    jal        func_00207F80
    .word 0x344996ff
    jal        float_to_int
    mov.s     $f12, $f21
    paddub     $s4, $v0, $zero
    jal        float_to_int
    mov.s     $f12, $f23
    addiu      $v1, $s3, 0x702
    mtc1       $v1, $f0
    paddub     $s2, $v0, $zero
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    jal        float_to_int
    mul.s     $f12, $f0, $f1
    paddub     $s1, $v0, $zero
    jal        float_to_int
    mov.s     $f12, $f22
    paddub     $8, $v0, $zero
    lui        $v0, (0x801996FF >> 16)
    paddub     $a1, $s4, $zero
    paddub     $a2, $s2, $zero
    paddub     $a3, $s1, $zero
    paddub     $a0, $s0, $zero
    jal        func_00207F80
    .word 0x344996ff
    jal        float_to_int
    mov.s     $f12, $f20
    mov.s      $f12, $f23
    jal        float_to_int
    paddub    $s3, $v0, $zero
    addiu      $v1, $s5, 0x6FE
    mtc1       $v1, $f0
    paddub     $s2, $v0, $zero
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    jal        float_to_int
    mul.s     $f12, $f0, $f1
    mov.s      $f12, $f22
    jal        float_to_int
    paddub    $s1, $v0, $zero
    paddub     $8, $v0, $zero
    lui        $v0, (0x801996FF >> 16)
    paddub     $a0, $s0, $zero
    paddub     $a1, $s3, $zero
    paddub     $a2, $s2, $zero
    paddub     $a3, $s1, $zero
    jal        func_00207F80
    .word 0x344996ff
    lq         $ra, 0x90($sp)
    lq         $s6, 0x80($sp)
    lq         $s5, 0x70($sp)
    lq         $s4, 0x60($sp)
    lq         $s3, 0x50($sp)
    lq         $s2, 0x40($sp)
    lq         $s1, 0x30($sp)
    lq         $s0, 0x20($sp)
    lwc1       $f24, 0x10($sp)
    lwc1       $f23, 0xC($sp)
    lwc1       $f22, 0x8($sp)
    lwc1       $f21, 0x4($sp)
    lwc1       $f20, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0xA0
}
