// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001FE530(int, int, int, int);

asm void func_001FE660(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x70802e28
    .word 0x10000007
    .word 0x70008628
    .word 0x70002628
    jal        func_001FE530
    .word 0x70003628
    addiu      $s0, $s0, 0x1
    .word 0x70402e28
    nop
    .word 0x80a20000
    .word 0x1440fff9
    .word 0x70002628
    .word 0x72001628
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
