// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0011B498(int, int, int, int);

asm void func_0011B4C8(void) {
    addiu      $sp, $sp, -0x20
    .word 0xffb00000
    .word 0xffbf0010
    jal        func_0011B498
    daddu     $s0, $a0, $zero
    addiu      $v1, $zero, 0x2
    .word 0x14430003
    .word 0x3c021000
    .word 0x10000021
    daddu     $v0, $zero, $zero
    .word 0x3c031000
    .word 0x34423040
    .word 0x34633050
    .word 0x8c440000
    .word 0x3c071000
    .word 0x34e73060
    .word 0x3c091000
    .word 0xae040000
    .word 0x35293070
    .word 0x3c081000
    .word 0x3c0a1000
    .word 0x8c620000
    .word 0x35083020
    .word 0x354a3080
    .word 0x3c061000
    .word 0xae020004
    .word 0x34c63090
    .word 0x3c051000
    addiu      $v0, $zero, 0x1
    .word 0x8ce40000
    .word 0x34a530a0
    .word 0xae040008
    .word 0x8d230000
    .word 0xae03000c
    .word 0x8d040000
    .word 0xae040010
    .word 0x8d430000
    .word 0xae030014
    .word 0x8cc40000
    .word 0xae040018
    .word 0x8ca30000
    .word 0xae03001c
    .word 0xdfbf0010
    .word 0xdfb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
