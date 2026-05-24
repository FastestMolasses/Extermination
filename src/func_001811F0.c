// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001749A0(int, int, int, int);

asm void func_001811F0(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x908202f1
    addiu      $a2, $zero, 0x1
    .word 0x14460043
    .word 0x70808628
    addiu      $a1, $zero, 0x2
    .word 0xa20502f1
    .word 0x9203025c
    addiu      $v0, $zero, 0x3
    .word 0x1462001f
    nop
    .word 0x9202023f
    .word 0x1446000a
    nop
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xC5
    jal        func_001749A0
    .word 0x70003628
    .word 0x3c033f33
    .word 0x34633333
    .word 0x10000073
    .word 0xae03026c
    .word 0x1445000a
    .word 0x3c023f80
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xC5
    jal        func_001749A0
    .word 0x70003628
    .word 0x3c033f80
    .word 0x10000069
    .word 0xae03026c
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xC7
    jal        func_001749A0
    .word 0x70003628
    .word 0x3c033f80
    .word 0x10000061
    .word 0xae03026c
    .word 0x9202023f
    .word 0x1446000a
    nop
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xC4
    jal        func_001749A0
    .word 0x70003628
    .word 0x3c033f33
    .word 0x34633333
    .word 0x10000055
    .word 0xae03026c
    .word 0x1445000a
    .word 0x3c023f80
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xC4
    jal        func_001749A0
    .word 0x70003628
    .word 0x3c033f80
    .word 0x1000004b
    .word 0xae03026c
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xC6
    jal        func_001749A0
    .word 0x70003628
    .word 0x3c033f80
    .word 0x10000043
    .word 0xae03026c
    .word 0xa20602f1
    .word 0x9203025c
    addiu      $v0, $zero, 0x3
    .word 0x14620020
    nop
    .word 0x9203023f
    .word 0x1466000b
    addiu     $v0, $zero, 0x2
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xBD
    jal        func_001749A0
    .word 0x70003628
    .word 0x3c033f33
    .word 0x34633333
    .word 0x10000032
    .word 0xae03026c
    addiu      $v0, $zero, 0x2
    .word 0x1462000a
    .word 0x3c023f80
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xBD
    jal        func_001749A0
    .word 0x70003628
    .word 0x3c033f80
    .word 0x10000027
    .word 0xae03026c
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xBF
    jal        func_001749A0
    .word 0x70003628
    .word 0x3c033f80
    .word 0x1000001f
    .word 0xae03026c
    .word 0x9203023f
    .word 0x1466000b
    addiu     $v0, $zero, 0x2
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xBE
    jal        func_001749A0
    .word 0x70003628
    .word 0x3c033f33
    .word 0x34633333
    .word 0x10000013
    .word 0xae03026c
    addiu      $v0, $zero, 0x2
    .word 0x1462000a
    .word 0x3c023f80
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xBE
    jal        func_001749A0
    .word 0x70003628
    .word 0x3c033f80
    .word 0x10000008
    .word 0xae03026c
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xC0
    jal        func_001749A0
    .word 0x70003628
    .word 0x3c033f80
    .word 0xae03026c
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
