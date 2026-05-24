// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0010E338(int, int, int, int);

asm void func_0010E368(void) {
    addiu      $sp, $sp, -0x10
    .word 0x04a00005
    .word 0xffbf0000
    .word 0x8c820020
    slt        $v0, $a1, $v0
    .word 0x54400005
    .word 0x8c83001c
    jal        func_0010E338
    nop
    .word 0x10000004
    .word 0xdfbf0000
    sll        $v0, $a1, 6
    addu       $v0, $v1, $v0
    .word 0xdfbf0000
    jr         $ra
    addiu     $sp, $sp, 0x10
}
