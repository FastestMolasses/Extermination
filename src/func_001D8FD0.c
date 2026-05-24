// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001B0070(int, int, int, int);
extern void func_001D7B30(int, int, int, int);
extern void func_0021B8E0(int, int, int, int);
extern void func_0021B970(int, int, int, int);
extern void func_0021BA80(int, int, int, int);

asm void func_001D8FD0(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    jal        func_001D7B30
    .word 0x7fb00000
    jal        func_001B0070
    .word 0x70408628
    andi       $v0, $v0, 0x80
    .word 0x1040000c
    nop
    .word 0x3c0242dc
    mtc1       $zero, $f12
    mtc1       $v0, $f13
    jal        func_0021B970
    nop
    .word 0x70002628
    .word 0x70002e28
    jal        func_0021BA80
    .word 0x70003628
    .word 0x10000008
    nop
    .word 0xc60d0008
    jal        func_0021B970
    .word 0xc60c0004
    .word 0x8e050010
    .word 0x8e060014
    jal        func_0021BA80
    .word 0x8e04000c
    jal        func_0021B8E0
    nop
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
