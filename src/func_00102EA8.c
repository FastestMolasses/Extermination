// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001026D0(int, int, int, int);
extern void func_001029C0(int, int, int, int);

asm void func_00102EA8(void) {
    addiu      $sp, $sp, -0xA0
    neg.s      $f0, $f17
    .word 0xe7b40060
    neg.s      $f20, $f18
    .word 0xc7a100a0
    .word 0xe7b50068
    add.s      $f0, $f0, $f18
    mul.s      $f21, $f1, $f19
    .word 0xffb00040
    mul.s      $f20, $f20, $f19
    daddu      $s0, $a0, $zero
    mul.s      $f17, $f17, $f1
    .word 0xe7ba0090
    neg.s      $f19, $f19
    .word 0xe7b90088
    mul.s      $f21, $f21, $f0
    .word 0xe7b80080
    add.s      $f20, $f20, $f17
    .word 0xe7b70078
    add.s      $f19, $f19, $f1
    .word 0xe7b60070
    mov.s      $f22, $f12
    mov.s      $f24, $f13
    mov.s      $f23, $f14
    mov.s      $f26, $f15
    nop
    nop
    div.s      $f21, $f21, $f19
    nop
    nop
    div.s      $f20, $f20, $f19
    .word 0xffbf0050
    jal        func_001029C0
    mov.s     $f25, $f16
    .word 0x3c013f80
    mtc1       $at, $f0
    daddu      $a0, $sp, $zero
    .word 0xe6160014
    .word 0xe6160000
    .word 0xae000028
    .word 0xae00003c
    .word 0xe600002c
    jal        func_001029C0
    .word 0xe6000038
    daddu      $a0, $s0, $zero
    .word 0xe7b80000
    .word 0xe7b70014
    daddu      $a1, $sp, $zero
    .word 0xe7b50028
    daddu      $a2, $a0, $zero
    .word 0xe7ba0030
    .word 0xe7b90034
    jal        func_001026D0
    .word 0xe7b40038
    .word 0xdfbf0050
    .word 0xdfb00040
    .word 0xc7ba0090
    .word 0xc7b90088
    .word 0xc7b80080
    .word 0xc7b70078
    .word 0xc7b60070
    .word 0xc7b50068
    .word 0xc7b40060
    jr         $ra
    addiu     $sp, $sp, 0xA0
}
