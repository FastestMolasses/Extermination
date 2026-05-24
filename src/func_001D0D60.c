// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001281C0(int, int, int, int);

asm void func_001D0D60(void) {
    addiu      $sp, $sp, -0x60
    .word 0x7fbf0050
    .word 0x7fb30040
    .word 0x7fb20030
    .word 0x7fb10020
    .word 0x7fb00010
    .word 0xe7b50004
    .word 0xe7b40000
    .word 0xc4800008
    .word 0x70809e28
    add.s      $f0, $f0, $f12
    .word 0xe4800008
    .word 0x9082000c
    .word 0x1040001b
    .word 0x70008628
    .word 0x10000007
    .word 0xc6610008
    .word 0xc6600008
    addiu      $s0, $zero, 0x1000
    sub.s      $f0, $f0, $f20
    .word 0xe6600008
    nop
    .word 0xc6610008
    .word 0xc6740004
    c.lt.s     $f1, $f20
    nop
    .word 0x4500fff6
    nop
    .word 0x3c023f80
    mtc1       $v0, $f0
    jal        func_001281C0
    add.s     $f12, $f0, $f1
    mtc1       $v0, $f0
    nop
    cvt.s.w    $f0, $f0
    c.lt.s     $f0, $f20
    nop
    .word 0x4501001f
    .word 0x70408e28
    .word 0x1000001d
    .word 0x70008e28
    .word 0xc6600008
    .word 0xc6610004
    c.lt.s     $f0, $f1
    nop
    .word 0x45010006
    nop
    .word 0x3c023f80
    mtc1       $v0, $f0
    addiu      $s0, $zero, 0x1000
    sub.s      $f0, $f1, $f0
    .word 0xe6600008
    .word 0xc6610008
    .word 0x3c023f80
    mtc1       $v0, $f0
    jal        func_001281C0
    add.s     $f12, $f0, $f1
    mtc1       $v0, $f0
    .word 0xc6610004
    cvt.s.w    $f0, $f0
    c.lt.s     $f0, $f1
    nop
    .word 0x45010006
    .word 0x70408e28
    .word 0x3c023f80
    mtc1       $v0, $f0
    jal        func_001281C0
    sub.s     $f12, $f1, $f0
    .word 0x70408e28
    .word 0xc6740008
    jal        func_001281C0
    mov.s     $f12, $f20
    mtc1       $v0, $f0
    .word 0x8e720000
    cvt.s.w    $f0, $f0
    .word 0x3c023f80
    sub.s      $f21, $f20, $f0
    mtc1       $v0, $f0
    mov.s      $f12, $f20
    jal        func_001281C0
    sub.s     $f20, $f0, $f21
    sll        $v1, $v0, 3
    subu       $v0, $v1, $v0
    sll        $v0, $v0, 2
    addu       $a1, $s2, $v0
    sll        $v0, $s1, 3
    subu       $v0, $v0, $s1
    sll        $v0, $v0, 2
    addu       $a0, $s2, $v0
    .word 0x70001e28
    .word 0xc4a10000
    .word 0xc4800000
    addiu      $v1, $v1, 0x1
    slti       $v0, $v1, 0x7
    mula.s     $f20, $f1
    madd.s     $f0, $f21, $f0
    .word 0xe6600040
    addiu      $a0, $a0, 0x4
    addiu      $a1, $a1, 0x4
    .word 0x1440fff6
    addiu     $s3, $s3, 0x4
    .word 0x72001628
    .word 0x7bbf0050
    .word 0x7bb30040
    .word 0x7bb20030
    .word 0x7bb10020
    .word 0x7bb00010
    .word 0xc7b50004
    .word 0xc7b40000
    jr         $ra
    addiu     $sp, $sp, 0x60
}
