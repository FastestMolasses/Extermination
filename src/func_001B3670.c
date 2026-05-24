// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00102738(int, int, int, int);
extern void func_00102760(int, int, int, int);
extern void func_001028B8(int, int, int, int);
extern void func_00102948(int, int, int, int);
extern void func_0011E420(int, int, int, int);
extern void func_001B1380(int, int, int, int);
extern void func_001B15D0(int, int, int, int);
extern void func_001B2B10(int, int, int, int);
extern void func_001B39F0(int, int, int, int);

asm void func_001B3670(void) {
    addiu      $sp, $sp, -0x90
    .word 0x7fbf0060
    .word 0x7fb50050
    .word 0x7fb40040
    .word 0x7fb30030
    .word 0x7fb20020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x70a0ae28
    addiu      $s1, $sp, 0x74
    .word 0xafa00070
    addiu      $s0, $sp, 0x78
    .word 0xae200000
    .word 0x3c023f80
    addiu      $s2, $sp, 0x7C
    .word 0xae020000
    addiu      $a1, $sp, 0x70
    .word 0x70c09e28
    .word 0x7080a628
    .word 0xae420000
    jal        func_001B2B10
    .word 0x70a03628
    addiu      $a0, $sp, 0x80
    jal        func_00102948
    .word 0x72a02e28
    addiu      $a0, $sp, 0x80
    .word 0x70802e28
    jal        func_00102760
    .word 0xafa00084
    addiu      $a0, $sp, 0x70
    jal        func_00102738
    addiu     $a1, $sp, 0x80
    jal        func_0011E420
    mov.s     $f12, $f0
    .word 0x3c024016
    .word 0x3442cbe4
    mtc1       $v0, $f1
    nop
    c.le.s     $f0, $f1
    nop
    .word 0x4500000b
    nop
    addiu      $a0, $sp, 0x80
    addiu      $a2, $s4, 0xB0
    jal        func_001028B8
    .word 0x70802e28
    .word 0xc68c00c4
    addiu      $a0, $sp, 0x80
    jal        func_001B1380
    addiu     $a1, $s4, 0xB0
    .word 0x1000001c
    .word 0x7bbf0060
    .word 0xae000000
    .word 0xae200000
    .word 0xafa00070
    .word 0x3c023f80
    addiu      $a0, $s4, 0xB0
    .word 0x72602e28
    jal        func_001B15D0
    .word 0xae420000
    .word 0x3c024000
    mtc1       $v0, $f1
    addiu      $a1, $sp, 0x70
    div.s      $f0, $f0, $f1
    .word 0x72802628
    .word 0x70a03628
    jal        func_001B2B10
    .word 0xe6000000
    addiu      $a0, $sp, 0x70
    addiu      $a2, $s4, 0xB0
    jal        func_001028B8
    .word 0x70802e28
    addiu      $a1, $sp, 0x70
    addiu      $a2, $sp, 0x80
    jal        func_001B39F0
    .word 0x72802628
    dsll32     $v0, $v0, 24
    dsra32     $v0, $v0, 24
    .word 0x7bbf0060
    .word 0x7bb50050
    .word 0x7bb40040
    .word 0x7bb30030
    .word 0x7bb20020
    .word 0x7bb10010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x90
}
