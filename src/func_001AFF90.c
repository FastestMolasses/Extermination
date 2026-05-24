// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001AF800(int, int, int, int);

asm void func_001AFF90(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x8c900014
    .word 0xac800014
    jal        func_001AF800
    .word 0x72002628
    .word 0xae000000
    .word 0xae000004
    .word 0xae000008
    .word 0xae00000c
    .word 0xa6000036
    .word 0xa2000098
    addiu      $a1, $s0, 0x1F0
    .word 0xae000090
    .word 0x70002628
    addiu      $a0, $a0, 0x1
    .word 0xaca00000
    slti       $v1, $a0, 0x40
    addiu      $a1, $a1, 0x4
    nop
    .word 0x1460fffa
    nop
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
