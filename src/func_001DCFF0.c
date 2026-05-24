// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00102948(int, int, int, int);
extern void func_001D1F80(int, int, int, int);
extern void func_002082B0(int, int, int, int);

asm void func_001DCFF0(void) {
    addiu      $sp, $sp, -0x90
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x3c024700
    .word 0xafa20030
    .word 0xafa20034
    .word 0xafa00038
    .word 0x3c0243b4
    .word 0xafa2003c
    .word 0x3c024348
    .word 0xafa20040
    .word 0xafa20044
    .word 0x3c02434b
    .word 0xafa20048
    .word 0xafa2004c
    andi       $v0, $a1, 0xFF
    .word 0x04400004
    .word 0x70808e28
    mtc1       $v0, $f0
    .word 0x10000008
    cvt.s.w   $f0, $f0
    srl        $v1, $v0, 1
    andi       $v0, $v0, 0x1
    or         $v1, $v1, $v0
    mtc1       $v1, $f0
    nop
    cvt.s.w    $f0, $f0
    add.s      $f0, $f0, $f0
    addiu      $s0, $sp, 0x50
    srl        $v0, $a1, 8
    andi       $v0, $v0, 0xFF
    .word 0x04400004
    .word 0xe6000000
    mtc1       $v0, $f0
    .word 0x10000008
    cvt.s.w   $f0, $f0
    srl        $v1, $v0, 1
    andi       $v0, $v0, 0x1
    or         $v1, $v1, $v0
    mtc1       $v1, $f0
    nop
    cvt.s.w    $f0, $f0
    add.s      $f0, $f0, $f0
    srl        $v0, $a1, 16
    andi       $v0, $v0, 0xFF
    .word 0x04400004
    .word 0xe7a00054
    mtc1       $v0, $f0
    .word 0x10000008
    cvt.s.w   $f0, $f0
    srl        $v1, $v0, 1
    andi       $v0, $v0, 0x1
    or         $v1, $v1, $v0
    mtc1       $v1, $f0
    nop
    cvt.s.w    $f0, $f0
    add.s      $f0, $f0, $f0
    srl        $v0, $a1, 24
    andi       $v0, $v0, 0xFF
    .word 0x04400004
    .word 0xe7a00058
    mtc1       $v0, $f0
    .word 0x10000008
    cvt.s.w   $f0, $f0
    srl        $v1, $v0, 1
    andi       $v0, $v0, 0x1
    or         $v1, $v1, $v0
    mtc1       $v1, $f0
    nop
    cvt.s.w    $f0, $f0
    add.s      $f0, $f0, $f0
    addiu      $a0, $sp, 0x60
    .word 0xe7a0005c
    jal        func_00102948
    .word 0x72002e28
    addiu      $a0, $sp, 0x70
    jal        func_00102948
    .word 0x72002e28
    addiu      $a0, $sp, 0x80
    jal        func_00102948
    .word 0x72002e28
    addiu      $a2, $zero, 0x1
    .word 0x72202628
    jal        func_001D1F80
    .word 0x70002e28
    addiu      $a1, $sp, 0x30
    jal        func_002082B0
    .word 0x72202628
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x90
}
