// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00153A10(int, int, int, int);
extern void func_00153A90(int, int, int, int);
extern void func_00153B50(int, int, int, int);
extern void func_00153EA0(int, int, int, int);
extern void func_001B5360(int, int, int, int);

asm void func_00153950(void) {
    addiu      $sp, $sp, -0x30
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x90850004
    .word 0x70808e28
    addiu      $v1, $zero, 0x3
    .word 0x10a3001a
    addiu     $s0, $s1, 0x1F0
    addiu      $v1, $zero, 0x2
    .word 0x50a30013
    .word 0x72002e28
    addiu      $v1, $zero, 0x1
    .word 0x50a3000b
    .word 0x72002e28
    .word 0x10a00004
    .word 0x72002e28
    .word 0x10000015
    .word 0x82030010
    .word 0x72002e28
    jal        func_00153A10
    nop
    .word 0x1000000f
    nop
    .word 0x72002e28
    jal        func_00153A90
    nop
    .word 0x1000000a
    nop
    .word 0x72002e28
    jal        func_00153B50
    nop
    .word 0x10000005
    nop
    jal        func_00153EA0
    .word 0x72002e28
    .word 0x10000007
    .word 0x7bbf0020
    .word 0x82030010
    .word 0x10600003
    nop
    jal        func_001B5360
    .word 0x72202628
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x30
}
