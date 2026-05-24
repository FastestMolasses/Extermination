// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00121870(int, int, int, int);

asm void func_00207150(void) {
    addiu      $sp, $sp, -0xA0
    .word 0x7fbf0090
    .word 0x7fbe0080
    .word 0x7fb70070
    .word 0x7fb60060
    .word 0x7fb50050
    .word 0x7fb40040
    .word 0x7fb30030
    .word 0x7fb20020
    .word 0x7fb10010
    .word 0x7fb00000
    addu       $v0, $a1, $a3
    addu       $s0, $9, $11
    slt        $at, $v0, $s0
    .word 0x70c0ae28
    .word 0x7100a628
    .word 0x71409628
    .word 0x70a0b628
    .word 0x7080be28
    .word 0x71209e28
    .word 0x10200003
    .word 0x71608e28
    .word 0x1000002c
    .word 0x70001628
    slt        $v0, $s3, $s6
    .word 0x14400010
    subu      $fp, $s6, $s3
    .word 0x72802e28
    jal        func_00121870
    .word 0x72c03628
    addu       $a1, $s4, $s6
    subu       $a2, $s3, $s6
    jal        func_00121870
    .word 0x72a02628
    addu       $v0, $s5, $s3
    subu       $a0, $v0, $s6
    .word 0x72402e28
    jal        func_00121870
    .word 0x72203628
    .word 0x1000001b
    .word 0x72001628
    subu       $fp, $s6, $s3
    slt        $v0, $s1, $fp
    .word 0x14400010
    .word 0x72802e28
    .word 0x72802e28
    jal        func_00121870
    .word 0x72603628
    addu       $a0, $s7, $s3
    .word 0x72402e28
    jal        func_00121870
    .word 0x73c03628
    addu       $v0, $s2, $s6
    subu       $a1, $v0, $s3
    subu       $a2, $s1, $fp
    jal        func_00121870
    .word 0x72a02628
    .word 0x10000008
    nop
    .word 0x72802e28
    jal        func_00121870
    .word 0x72603628
    addu       $a0, $s7, $s3
    .word 0x72402e28
    jal        func_00121870
    .word 0x72203628
    .word 0x72001628
    .word 0x7bbf0090
    .word 0x7bbe0080
    .word 0x7bb70070
    .word 0x7bb60060
    .word 0x7bb50050
    .word 0x7bb40040
    .word 0x7bb30030
    .word 0x7bb20020
    .word 0x7bb10010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0xA0
}
