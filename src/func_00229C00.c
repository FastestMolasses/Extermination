// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00229A00(int, int, int, int);
extern void func_00229A70(int, int, int, int);

asm void func_00229C00(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x8c830010
    addiu      $v0, $zero, 0x2
    addiu      $v1, $v1, 0x1
    .word 0xac830010
    .word 0x90830003
    .word 0x1062000f
    .word 0x70808628
    addiu      $v0, $zero, 0x1
    .word 0x1062000a
    addiu     $v0, $zero, 0x3
    .word 0x10600003
    nop
    .word 0x1000000e
    .word 0x70001628
    jal        func_00229A70
    nop
    .word 0x10000009
    nop
    addiu      $v0, $zero, 0x3
    .word 0x10000006
    .word 0xa2020000
    jal        func_00229A00
    nop
    .word 0xa2020016
    .word 0x10000002
    .word 0x70001628
    .word 0x70001628
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
