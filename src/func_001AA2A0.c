// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0011E748(int, int, int, int);

asm void func_001AA2A0(void) {
    addiu      $sp, $sp, -0x40
    .word 0x7fbf0030
    .word 0x7fb10020
    .word 0x7fb00010
    .word 0xe7b40000
    .word 0xc4a300b0
    .word 0xc48200a0
    .word 0xc4a100b8
    .word 0xc48000a8
    mov.s      $f20, $f12
    .word 0x70808e28
    .word 0x70a08628
    sub.s      $f2, $f3, $f2
    sub.s      $f0, $f1, $f0
    mula.s     $f2, $f2
    jal        func_0011E748
    madd.s    $f12, $f0, $f0
    c.le.s     $f0, $f20
    .word 0x3c017000
    .word 0x45010003
    .word 0xe4203a20
    .word 0x1000003d
    .word 0x70001628
    .word 0xc62100a4
    .word 0xc60200b4
    mtc1       $zero, $f0
    nop
    sub.s      $f1, $f2, $f1
    c.lt.s     $f1, $f0
    nop
    .word 0x4500001a
    nop
    .word 0x92030003
    addiu      $v0, $zero, 0x8
    .word 0x10620005
    .word 0x3c02c100
    addiu      $v0, $zero, 0x4
    .word 0x1462000b
    .word 0x3c02c150
    .word 0x3c02c100
    mtc1       $v0, $f0
    nop
    c.lt.s     $f1, $f0
    nop
    .word 0x45000026
    addiu     $v0, $zero, 0x1
    .word 0x10000024
    .word 0x70001628
    .word 0x3c02c150
    mtc1       $v0, $f0
    nop
    c.lt.s     $f1, $f0
    nop
    .word 0x4500001c
    nop
    .word 0x1000001b
    .word 0x70001628
    .word 0x92030003
    addiu      $v0, $zero, 0x8
    .word 0x10620005
    .word 0x3c0241f0
    addiu      $v0, $zero, 0x4
    .word 0x1462000b
    .word 0x3c024120
    .word 0x3c0241f0
    mtc1       $v0, $f0
    nop
    c.le.s     $f1, $f0
    nop
    .word 0x4501000c
    nop
    .word 0x1000000b
    .word 0x70001628
    .word 0x3c024120
    mtc1       $v0, $f0
    nop
    c.le.s     $f1, $f0
    nop
    .word 0x45010003
    nop
    .word 0x10000002
    .word 0x70001628
    addiu      $v0, $zero, 0x1
    .word 0x7bbf0030
    .word 0x7bb10020
    .word 0x7bb00010
    .word 0xc7b40000
    jr         $ra
    addiu     $sp, $sp, 0x40
}
