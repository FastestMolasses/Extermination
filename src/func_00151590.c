// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001516B0(int, int, int, int);
extern void func_00151770(int, int, int, int);
extern void func_00152CC0(int, int, int, int);
extern void func_00152E80(int, int, int, int);
extern void func_001F91C0(int, int, int, int);

asm void func_00151590(void) {
    addiu      $sp, $sp, -0x30
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x90830004
    .word 0x70808e28
    addiu      $v0, $zero, 0x3
    .word 0x1062001a
    addiu     $s0, $s1, 0x1F0
    addiu      $v0, $zero, 0x2
    .word 0x10620013
    .word 0x72002e28
    addiu      $v0, $zero, 0x1
    .word 0x1062000b
    .word 0x72002e28
    .word 0x10600004
    .word 0x72002e28
    .word 0x10000015
    .word 0x82020051
    .word 0x72002e28
    jal        func_001516B0
    nop
    .word 0x1000000f
    nop
    .word 0x72002e28
    jal        func_00151770
    nop
    .word 0x1000000a
    nop
    .word 0x72002e28
    jal        func_00152CC0
    nop
    .word 0x10000005
    nop
    jal        func_00152E80
    .word 0x72002e28
    .word 0x1000001e
    .word 0x7bbf0020
    .word 0x82020051
    .word 0x10400003
    nop
    addiu      $v0, $v0, -0x1
    .word 0xa2020051
    .word 0x82020052
    .word 0x10400003
    nop
    addiu      $v0, $v0, -0x1
    .word 0xa2020052
    .word 0x86020042
    .word 0x10400003
    nop
    addiu      $v0, $v0, -0x1
    .word 0xa6020042
    .word 0x82020050
    .word 0x10400003
    nop
    addiu      $v0, $v0, -0x1
    .word 0xa2020050
    .word 0x86020044
    .word 0x10400004
    .word 0x72202628
    addiu      $v0, $v0, -0x1
    .word 0xa6020044
    .word 0x72202628
    jal        func_001F91C0
    nop
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x30
}
