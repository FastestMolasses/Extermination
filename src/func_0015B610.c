// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00174A50(int, int, int, int);
extern void func_00182B30(int, int, int, int);
extern void func_00182D70(int, int, int, int);
extern void func_00183240(int, int, int, int);
extern void func_00183250(int, int, int, int);
extern void func_001833F0(int, int, int, int);
extern void func_00183440(int, int, int, int);
extern void func_001834E0(int, int, int, int);

asm void func_0015B610(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x3c017000
    .word 0x90233b8d
    .word 0x10600026
    .word 0x70808628
    andi       $a1, $v1, 0xFF
    addiu      $v1, $zero, 0x4
    .word 0x10a30022
    nop
    jal        func_00182B30
    nop
    .word 0x1440001e
    nop
    .word 0x92030005
    addiu      $v0, $zero, 0x3
    .word 0x1462000a
    addiu     $v0, $zero, 0x1
    addiu      $v0, $zero, 0x4
    .word 0xa2020004
    addiu      $v0, $zero, 0xC
    .word 0xa2020005
    addiu      $v0, $zero, 0x17
    .word 0xa2000006
    .word 0x1000000e
    .word 0xa20201f0
    addiu      $v0, $zero, 0x1
    .word 0x14620006
    addiu     $v0, $zero, 0x4
    .word 0x3c024100
    mtc1       $v0, $f12
    jal        func_00174A50
    .word 0x72002628
    addiu      $v0, $zero, 0x4
    .word 0xa2020004
    .word 0xa2000005
    addiu      $v0, $zero, 0x41
    .word 0xa2000006
    .word 0xa20201f0
    jal        func_00182D70
    .word 0x72002628
    .word 0x1000002a
    .word 0x7bbf0010
    .word 0x92040005
    addiu      $v1, $zero, 0x4
    .word 0x50830023
    .word 0x72002628
    addiu      $v1, $zero, 0x3
    .word 0x5083001b
    .word 0x72002628
    addiu      $v1, $zero, 0x2
    .word 0x50830013
    .word 0x72002628
    addiu      $v1, $zero, 0x1
    .word 0x5083000b
    .word 0x72002628
    .word 0x10800004
    .word 0x72002628
    .word 0x10000018
    nop
    .word 0x72002628
    jal        func_00183240
    nop
    .word 0x10000013
    nop
    .word 0x72002628
    jal        func_00183250
    nop
    .word 0x1000000e
    nop
    .word 0x72002628
    jal        func_001833F0
    nop
    .word 0x10000009
    nop
    .word 0x72002628
    jal        func_00183440
    nop
    .word 0x10000004
    nop
    .word 0x72002628
    jal        func_001834E0
    nop
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
