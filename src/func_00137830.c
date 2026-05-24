// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00137920(int, int, int, int);
extern void func_00137A50(int, int, int, int);
extern void func_00137C80(int, int, int, int);
extern void func_00137E80(int, int, int, int);

asm void func_00137830(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x90850004
    addiu      $v1, $zero, 0x3
    .word 0x10a3001a
    addiu     $s0, $a0, 0x1F0
    addiu      $v1, $zero, 0x2
    .word 0x50a30013
    .word 0x72002e28
    addiu      $v1, $zero, 0x1
    .word 0x50a3000b
    .word 0x72002e28
    .word 0x10a00004
    .word 0x72002e28
    .word 0x10000015
    .word 0x82030042
    .word 0x72002e28
    jal        func_00137920
    nop
    .word 0x1000000f
    nop
    .word 0x72002e28
    jal        func_00137A50
    nop
    .word 0x1000000a
    nop
    .word 0x72002e28
    jal        func_00137C80
    nop
    .word 0x10000005
    nop
    jal        func_00137E80
    .word 0x72002e28
    .word 0x10000016
    .word 0x7bbf0010
    .word 0x82030042
    .word 0x10600003
    nop
    addiu      $v1, $v1, -0x1
    .word 0xa2030042
    .word 0x82030041
    .word 0x10600003
    nop
    addiu      $v1, $v1, -0x1
    .word 0xa2030041
    .word 0x82030040
    .word 0x10600003
    nop
    addiu      $v1, $v1, -0x1
    .word 0xa2030040
    .word 0x82030044
    .word 0x10600003
    nop
    addiu      $v1, $v1, -0x1
    .word 0xa2030044
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
