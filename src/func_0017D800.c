// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001281C0(int, int, int, int);

asm void func_0017D800(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x3c023f19
    .word 0x7fb00000
    .word 0x3442999a
    .word 0xac8202e4
    .word 0xc4820258
    .word 0xc48102e4
    .word 0x3c023f80
    mtc1       $v0, $f0
    .word 0x3c017000
    .word 0x70808628
    div.s      $f1, $f2, $f1
    c.lt.s     $f1, $f0
    nop
    .word 0x45000003
    .word 0xe4213a20
    .word 0x3c017000
    .word 0xac223a20
    .word 0xc60100b4
    .word 0xc6000258
    .word 0x3c017000
    add.s      $f0, $f0, $f1
    .word 0xe6000258
    jal        func_001281C0
    .word 0xc42c3a20
    .word 0xa6020028
    .word 0xc60002e0
    .word 0x3c017000
    addiu      $v1, $zero, 0x1
    .word 0xe60002f4
    .word 0xc60002e8
    .word 0xe60002f8
    .word 0xc60202f4
    .word 0xc60100b0
    .word 0xc4203a20
    sub.s      $f1, $f2, $f1
    .word 0x3c017000
    div.s      $f0, $f1, $f0
    .word 0xe60002e0
    .word 0xc60202f8
    .word 0xc60100b8
    .word 0xc4203a20
    sub.s      $f1, $f2, $f1
    div.s      $f0, $f1, $f0
    .word 0xe60002e8
    .word 0xa203025f
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
