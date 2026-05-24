// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0011B3F8(int, int, int, int);

asm void func_0011B390(void) {
    addiu      $sp, $sp, -0x10
    .word 0x1480000b
    .word 0xffbf0000
    jal        func_0011B3F8
    nop
    .word 0x3c03ffff
    .word 0x3c041000
    .word 0x3463fffb
    .word 0x34843010
    and        $v0, $v0, $v1
    .word 0xac820000
    .word 0x1000000b
    .word 0xdfbf0000
    addiu      $v0, $zero, 0x1
    .word 0x14820008
    .word 0xdfbf0000
    jal        func_0011B3F8
    nop
    .word 0x3c031000
    ori        $v0, $v0, 0x4
    .word 0x34633010
    .word 0xac620000
    .word 0xdfbf0000
    jr         $ra
    addiu     $sp, $sp, 0x10
}
