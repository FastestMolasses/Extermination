// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00204140(int, int, int, int);
extern void func_002041A0(int, int, int, int);

asm void func_00204250(void) {
    addiu      $sp, $sp, -0x10
    .word 0x7fbf0000
    .word 0xac80000c
    .word 0xac800010
    addiu      $v1, $zero, 0x1
    .word 0xac800014
    .word 0xac830044
    .word 0xac800058
    .word 0x70801628
    .word 0xac80005c
    addiu      $a0, $zero, -0x1
    .word 0x70002e28
    .word 0x1000000f
    .word 0x70003628
    .word 0x8c430050
    addiu      $a1, $a1, 0x1
    addu       $v1, $v1, $a2
    .word 0xfc640000
    .word 0x8c430050
    addu       $v1, $v1, $a2
    .word 0xfc640008
    .word 0x8c430050
    addu       $v1, $v1, $a2
    .word 0xac600010
    .word 0x8c430050
    addu       $v1, $v1, $a2
    addiu      $a2, $a2, 0x18
    .word 0xac600014
    .word 0x8c430054
    slt        $v1, $a1, $v1
    .word 0x1460ffef
    nop
    .word 0x70005628
    .word 0x70004628
    .word 0x1000000d
    .word 0x70004e28
    .word 0x8c430000
    .word 0x8c440004
    addiu      $a2, $zero, 0x3
    addiu      $a3, $zero, 0x80
    addu       $v1, $v1, $8
    dsll32     $a1, $v1, 4
    addu       $a0, $a0, $9
    jal        func_002041A0
    dsrl32    $a1, $a1, 4
    addiu      $8, $8, 0x800
    addiu      $9, $9, 0x10
    addiu      $10, $10, 0x1
    .word 0x8c430008
    slt        $v1, $10, $v1
    .word 0x1460fff1
    nop
    .word 0x8c450004
    sll        $v1, $10, 4
    addiu      $a2, $zero, 0x2
    .word 0x70003e28
    addu       $a0, $a1, $v1
    dsll32     $a1, $a1, 4
    jal        func_002041A0
    dsrl32    $a1, $a1, 4
    .word 0x3c011001
    .word 0xac20b420
    .word 0x8c430000
    .word 0x3c011001
    addiu      $a0, $zero, 0x5
    dsll32     $v1, $v1, 4
    dsrl32     $v1, $v1, 4
    .word 0xac23b410
    .word 0x8c420004
    .word 0x3c011001
    dsll32     $v0, $v0, 4
    dsrl32     $v0, $v0, 4
    jal        func_00204140
    .word 0xac22b430
    .word 0x7bbf0000
    addiu      $v0, $zero, 0x1
    jr         $ra
    addiu     $sp, $sp, 0x10
}
