// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001B1C60(int, int, int, int);
extern void func_001B1CA0(int, int, int, int);
extern void func_001B1D20(int, int, int, int);
extern void func_001B1D60(int, int, int, int);
extern void func_001B1DA0(int, int, int, int);
extern void func_001B1DE0(int, int, int, int);

asm void func_001B1B70(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x90830002
    andi       $v1, $v1, 0x80
    .word 0x10600003
    .word 0x70808628
    jal        func_001B1DE0
    nop
    .word 0x92050002
    addiu      $a0, $zero, -0xE1
    addiu      $v1, $zero, 0xD
    and        $a0, $a1, $a0
    .word 0x50830027
    .word 0x72002628
    addiu      $v1, $zero, 0x7
    .word 0x5083001f
    .word 0x72002628
    addiu      $v1, $zero, 0x4
    .word 0x50830017
    .word 0x72002628
    addiu      $v1, $zero, 0x2
    .word 0x5083000f
    .word 0x72002628
    addiu      $v1, $zero, 0xA
    .word 0x1083000b
    nop
    addiu      $v1, $zero, 0x1
    .word 0x10830004
    .word 0x72002628
    .word 0x10000019
    .word 0x7bbf0010
    .word 0x72002628
    jal        func_001B1C60
    nop
    .word 0x10000013
    nop
    .word 0x72002628
    jal        func_001B1CA0
    nop
    .word 0x1000000e
    nop
    .word 0x72002628
    jal        func_001B1D20
    nop
    .word 0x10000009
    nop
    .word 0x72002628
    jal        func_001B1D60
    nop
    .word 0x10000004
    nop
    .word 0x72002628
    jal        func_001B1DA0
    nop
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
