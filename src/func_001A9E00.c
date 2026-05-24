// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0011DE90(int, int, int, int);
extern void func_0011E2A8(int, int, int, int);
extern void func_0011E620(int, int, int, int);
extern void func_0011E748(int, int, int, int);
extern void func_001B1470(int, int, int, int);

asm void func_001A9E00(void) {
    addiu      $sp, $sp, -0x40
    .word 0x7fbf0030
    .word 0x7fb10020
    .word 0x7fb00010
    .word 0xe7b60008
    .word 0xe7b50004
    .word 0xe7b40000
    .word 0xc48300a0
    .word 0xc4a200b0
    .word 0xc48100a8
    .word 0xc4a000b8
    .word 0x70808e28
    .word 0x70a08628
    sub.s      $f20, $f3, $f2
    sub.s      $f21, $f1, $f0
    mula.s     $f20, $f20
    jal        func_0011E748
    madd.s    $f12, $f21, $f21
    .word 0x8e230030
    .word 0x8e040030
    .word 0xc4620000
    .word 0xc4810000
    add.s      $f22, $f2, $f1
    c.le.s     $f0, $f22
    nop
    .word 0x45000036
    nop
    .word 0xc4630004
    .word 0xc62200a4
    .word 0xc60100b4
    mtc1       $zero, $f0
    .word 0x3c034000
    mtc1       $v1, $f4
    nop
    div.s      $f3, $f3, $f4
    add.s      $f2, $f2, $f3
    sub.s      $f2, $f2, $f1
    c.lt.s     $f2, $f0
    nop
    .word 0x45000002
    nop
    neg.s      $f2, $f2
    .word 0xc4800004
    .word 0x3c034000
    mtc1       $v1, $f1
    nop
    div.s      $f0, $f0, $f1
    nop
    add.s      $f0, $f3, $f0
    c.le.s     $f2, $f0
    nop
    .word 0x4500001c
    nop
    .word 0x92030003
    .word 0x14600019
    nop
    mov.s      $f12, $f20
    jal        func_0011E620
    mov.s     $f13, $f21
    jal        func_001B1470
    mov.s     $f12, $f0
    mov.s      $f20, $f0
    jal        func_0011DE90
    mov.s     $f12, $f20
    mov.s      $f12, $f20
    jal        func_0011E2A8
    mul.s     $f20, $f22, $f0
    mul.s      $f1, $f22, $f0
    .word 0xc62000a0
    sub.s      $f0, $f0, $f1
    .word 0xe60000b0
    .word 0xc62000a8
    sub.s      $f0, $f0, $f20
    .word 0xe60000b8
    .word 0x92230000
    andi       $v1, $v1, 0x4
    .word 0x14600003
    nop
    addiu      $v1, $zero, 0x1
    .word 0xa203000b
    .word 0x7bbf0030
    .word 0x7bb10020
    .word 0x7bb00010
    .word 0xc7b60008
    .word 0xc7b50004
    .word 0xc7b40000
    jr         $ra
    addiu     $sp, $sp, 0x40
}
