// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0010E338(int, int, int, int);
extern void func_0010DE78(int, int, int, int);

asm void func_0010E460(void) {
    addiu      $sp, $sp, -0x20
    .word 0xffb00000
    daddu      $s0, $a0, $zero
    .word 0xffbf0010
    jal        func_0010E338
    daddu     $a0, $a1, $zero
    .word 0x8e050014
    .word 0x3c038000
    .word 0x8e04001c
    .word 0x3463000c
    .word 0xac450014
    addiu      $a2, $zero, 0x40
    .word 0xac44001c
    daddu      $a1, $v0, $zero
    .word 0xac430020
    .word 0x3c048000
    .word 0xdfbf0010
    .word 0x34840008
    .word 0x8e090028
    .word 0x8e070020
    .word 0x8e080024
    .word 0xdfb00000
    j         func_0010DE78
    addiu     $sp, $sp, 0x20
}
