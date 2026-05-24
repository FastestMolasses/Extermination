// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00199FA0(int, int, int, int);

asm void func_00178390(void) {
    addiu      $sp, $sp, -0x40
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x70808628
    addiu      $a0, $sp, 0x20
    jal        func_00199FA0
    addiu     $a1, $sp, 0x30
    .word 0x1040001d
    .word 0x70001628
    .word 0x3c017000
    .word 0x8c2331d0
    .word 0x3c023fc0
    mtc1       $v0, $f3
    .word 0x3c0241a4
    mtc1       $v0, $f0
    .word 0xc4620024
    .word 0x3c017000
    .word 0xc42131b0
    addiu      $v0, $zero, 0x1
    mul.s      $f2, $f3, $f2
    .word 0x3c017000
    add.s      $f1, $f1, $f2
    .word 0xe60102e0
    .word 0x8c2331d0
    .word 0xc462002c
    .word 0x3c017000
    .word 0xc42131b8
    mul.s      $f2, $f3, $f2
    add.s      $f1, $f1, $f2
    .word 0xe60102e8
    .word 0xc7a10034
    sub.s      $f0, $f1, $f0
    .word 0xe60002e4
    .word 0xc60000c4
    .word 0x10000002
    .word 0xe6000218
    .word 0x70001628
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x40
}
