// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001031E0(int, int, int, int);
extern void func_00182870(int, int, int, int);
extern void func_001EFD90(int, int, int, int);

asm void func_0017DEB0(void) {
    addiu      $sp, $sp, -0x30
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x70808628
    jal        func_00182870
    .word 0x70002e28
    .word 0x9204023a
    addiu      $v1, $zero, 0x6
    .word 0x50830005
    .word 0x3c028000
    addiu      $v1, $zero, 0x5
    .word 0x14830008
    nop
    .word 0x3c028000
    addiu      $a1, $s0, 0xB0
    addiu      $a2, $s0, 0xC0
    jal        func_001EFD90
    .word 0x34440028
    .word 0x10000019
    .word 0x7bbf0010
    .word 0x9203023c
    .word 0x1060000d
    nop
    addiu      $a0, $sp, 0x20
    jal        func_001031E0
    addiu     $a1, $s0, 0xB0
    .word 0xc6000250
    .word 0x3c028000
    addiu      $a2, $s0, 0xC0
    addiu      $a1, $sp, 0x20
    .word 0x34440016
    jal        func_001EFD90
    .word 0xe7a00024
    .word 0x10000009
    nop
    .word 0x9203023d
    .word 0x14600006
    nop
    .word 0x3c028000
    addiu      $a1, $s0, 0xB0
    addiu      $a2, $s0, 0xC0
    jal        func_001EFD90
    .word 0x34440011
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x30
}
