// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00122BB8(int, int, int, int);

asm void func_00150DA0(void) {
    addiu      $sp, $sp, -0x30
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x80a30065
    addiu      $v0, $zero, 0x2
    .word 0x70808e28
    .word 0x10620035
    .word 0x70a08628
    jal        func_00122BB8
    nop
    sra        $v0, $v0, 13
    andi       $v0, $v0, 0xFF
    mtc1       $v0, $f0
    .word 0x3c04437f
    cvt.s.w    $f2, $f0
    .word 0x3c0243d3
    .word 0x34438000
    .word 0x3c024387
    mtc1       $a0, $f1
    mtc1       $v1, $f0
    nop
    div.s      $f1, $f2, $f1
    mul.s      $f1, $f0, $f1
    mtc1       $v0, $f0
    nop
    sub.s      $f0, $f1, $f0
    jal        func_00122BB8
    .word 0xe62000b8
    sra        $v1, $v0, 19
    andi       $v1, $v1, 0xFF
    mtc1       $v1, $f0
    .word 0x3c04437f
    cvt.s.w    $f2, $f0
    .word 0x3c034316
    .word 0x3c017000
    mtc1       $a0, $f1
    mtc1       $v1, $f0
    nop
    div.s      $f1, $f2, $f1
    mul.s      $f0, $f0, $f1
    .word 0xe4203a20
    .word 0x82030065
    .word 0x10600009
    nop
    .word 0x3c017000
    .word 0xc4203a20
    .word 0x3c03437a
    mtc1       $v1, $f1
    nop
    add.s      $f0, $f1, $f0
    .word 0x1000002d
    .word 0xe62000b0
    .word 0x3c017000
    .word 0xc4203a20
    .word 0x3c03c348
    mtc1       $v1, $f1
    nop
    add.s      $f0, $f1, $f0
    .word 0x10000025
    .word 0xe62000b0
    jal        func_00122BB8
    nop
    sra        $v0, $v0, 14
    andi       $v0, $v0, 0xFF
    mtc1       $v0, $f0
    .word 0x3c04437f
    cvt.s.w    $f2, $f0
    .word 0x3c02442c
    .word 0x34438000
    .word 0x3c02c375
    mtc1       $a0, $f1
    mtc1       $v1, $f0
    nop
    div.s      $f1, $f2, $f1
    mul.s      $f1, $f0, $f1
    mtc1       $v0, $f0
    nop
    add.s      $f0, $f0, $f1
    jal        func_00122BB8
    .word 0xe62000b0
    sra        $v1, $v0, 17
    andi       $v1, $v1, 0xFF
    mtc1       $v1, $f0
    .word 0x3c05437f
    cvt.s.w    $f2, $f0
    .word 0x3c044348
    .word 0x3c03c387
    mtc1       $a1, $f1
    mtc1       $a0, $f0
    nop
    div.s      $f1, $f2, $f1
    mul.s      $f1, $f0, $f1
    mtc1       $v1, $f0
    nop
    add.s      $f0, $f0, $f1
    .word 0xe62000b8
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x30
}
