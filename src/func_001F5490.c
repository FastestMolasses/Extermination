// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001C22A0(int, int, int, int);
extern void func_001C6380(int, int, int, int);

asm void func_001F5490(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    jal        func_001C22A0
    .word 0x70808628
    .word 0x10400004
    .word 0x72002628
    .word 0x10000005
    addiu     $v0, $zero, 0x3
    .word 0x72002628
    jal        func_001C6380
    nop
    addiu      $v0, $zero, 0x1
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
