// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001749A0(int, int, int, int);

asm void func_00181430(void) {
    addiu      $sp, $sp, -0x10
    .word 0x7fbf0000
    .word 0x908302f1
    addiu      $v0, $zero, 0x1
    .word 0x14620013
    nop
    .word 0x9083025c
    addiu      $v0, $zero, 0x3
    .word 0x10620009
    .word 0x3c023f80
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xC8
    jal        func_001749A0
    .word 0x70003628
    .word 0x10000019
    .word 0x7bbf0000
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xC9
    jal        func_001749A0
    .word 0x70003628
    .word 0x10000011
    nop
    .word 0x9083025c
    addiu      $v0, $zero, 0x3
    .word 0x10620009
    .word 0x3c023f80
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xC1
    jal        func_001749A0
    .word 0x70003628
    .word 0x10000006
    nop
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0xC2
    jal        func_001749A0
    .word 0x70003628
    .word 0x7bbf0000
    jr         $ra
    addiu     $sp, $sp, 0x10
}
