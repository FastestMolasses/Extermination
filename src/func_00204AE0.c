// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void RFU063(int, int, int, int);
extern void func_00204140(int, int, int, int);

asm void func_00204AE0(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x70808628
    jal        func_00204140
    addiu     $a0, $zero, 0x5
    .word 0x3c011001
    .word 0xac20b420
    .word 0x3c011001
    .word 0xac20b410
    .word 0x3c011001
    .word 0xac20b430
    jal        RFU063
    .word 0x8e040040
    .word 0x7bbf0010
    .word 0x7bb00000
    addiu      $v0, $zero, 0x1
    jr         $ra
    addiu     $sp, $sp, 0x20
}
