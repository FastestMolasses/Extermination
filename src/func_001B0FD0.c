// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001B0EA0(int, int, int, int);
extern void bone_init_default_1(int, int, int, int);

asm void func_001B0FD0(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    jal        func_001B0EA0
    .word 0x70808628
    .word 0x10400004
    .word 0x72002628
    .word 0x10000008
    addiu     $v0, $zero, 0x1
    .word 0x72002628
    jal        bone_init_default_1
    nop
    .word 0x92030004
    .word 0x70001628
    addiu      $v1, $v1, 0x1
    .word 0xa2030004
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
