// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00229960(int, int, int, int);
extern void func_0022A020(int, int, int, int);

asm void func_00229A00(void) {
    addiu      $sp, $sp, -0x10
    .word 0x7fbf0000
    .word 0x90830005
    addiu      $v0, $zero, 0x2
    .word 0x1062000f
    nop
    addiu      $v0, $zero, 0x1
    .word 0x1062000a
    addiu     $v0, $zero, 0x3
    .word 0x10600003
    nop
    .word 0x1000000d
    .word 0x70001628
    jal        func_00229960
    nop
    .word 0x10000008
    nop
    addiu      $v0, $zero, 0x3
    .word 0x10000005
    .word 0xa0820000
    jal        func_0022A020
    nop
    .word 0x10000003
    .word 0x7bbf0000
    .word 0x70001628
    .word 0x7bbf0000
    jr         $ra
    addiu     $sp, $sp, 0x10
}
