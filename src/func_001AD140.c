// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001FABB0(int, int, int, int);
extern void func_001FBC50(int, int, int, int);
extern void func_001FC9B0(int, int, int, int);

asm void func_001AD140(void) {
    addiu      $sp, $sp, -0x10
    .word 0x7fbf0000
    .word 0x3c017000
    .word 0x8c223b6c
    addiu      $a0, $zero, 0x3
    addiu      $v1, $zero, 0x2
    .word 0xa0440008
    .word 0x3c017000
    .word 0x8c223b6c
    .word 0xa0430009
    .word 0x3c017000
    .word 0x8c223b6c
    .word 0xa040000a
    .word 0x3c017000
    .word 0x8c223b6c
    jal        func_001FC9B0
    .word 0xa040000b
    jal        func_001FBC50
    nop
    jal        func_001FABB0
    nop
    .word 0x7bbf0000
    jr         $ra
    addiu     $sp, $sp, 0x10
}
