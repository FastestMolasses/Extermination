// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0019AD00(int, int, int, int);
extern void func_001B2D00(int, int, int, int);
extern void func_001B2F70(int, int, int, int);

asm void func_001B2B80(void) {
    addiu      $sp, $sp, -0x30
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x70c08628
    .word 0x3c023f80
    addiu      $a2, $zero, 0x7
    .word 0x70a08e28
    jal        func_0019AD00
    .word 0xaca2000c
    .word 0x10400007
    .word 0x72202628
    .word 0x72202628
    jal        func_001B2D00
    .word 0x72002e28
    .word 0x10000004
    ori       $v0, $v0, 0x4
    .word 0x72202628
    jal        func_001B2F70
    .word 0x72002e28
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x30
}
