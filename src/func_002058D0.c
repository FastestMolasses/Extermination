// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00205BC0(int, int, int, int);
extern void func_00205C80(int, int, int, int);
extern void func_00205CD0(int, int, int, int);

asm void func_002058D0(void) {
    addiu      $sp, $sp, -0x40
    .word 0x7fbf0030
    .word 0x7fb10020
    .word 0x7fb00010
    .word 0x70a08e28
    .word 0x70c08628
    .word 0xffa00000
    addiu      $a1, $zero, 0x6
    addiu      $11, $zero, 0x1
    .word 0x70003628
    .word 0x70003e28
    .word 0x70004628
    .word 0x70004e28
    .word 0x70005628
    jal        func_00205BC0
    .word 0xffa00008
    .word 0x8e250000
    .word 0x8e260004
    .word 0x70402628
    jal        func_00205CD0
    .word 0x70003e28
    srl        $v1, $s0, 8
    andi       $a2, $v1, 0xFF
    srl        $v1, $s0, 16
    andi       $a3, $v1, 0xFF
    srl        $v1, $s0, 24
    andi       $a1, $s0, 0xFF
    andi       $8, $v1, 0xFF
    .word 0x70402628
    jal        func_00205C80
    .word 0x70004e28
    .word 0x8e280000
    .word 0x8e250008
    .word 0x8e260004
    .word 0x8e23000c
    .word 0x70402628
    .word 0x70003e28
    addu       $a1, $8, $a1
    jal        func_00205CD0
    addu      $a2, $a2, $v1
    .word 0x7bbf0030
    .word 0x7bb10020
    .word 0x7bb00010
    jr         $ra
    addiu     $sp, $sp, 0x40
}
