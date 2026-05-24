// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00102760(int, int, int, int);
extern void func_00102798(int, int, int, int);
extern void func_00102900(int, int, int, int);

asm void func_00102D80(void) {
    addiu      $sp, $sp, -0x60
    .word 0xe7b40050
    .word 0x3c01bf80
    mtc1       $at, $f20
    .word 0xffb00010
    daddu      $s0, $a0, $zero
    .word 0xffb20030
    .word 0xffb10020
    daddu      $s2, $a3, $zero
    daddu      $s1, $a2, $zero
    mov.s      $f12, $f20
    .word 0xffbf0040
    jal        func_00102900
    daddu     $a0, $sp, $zero
    daddu      $a0, $s0, $zero
    jal        func_00102760
    daddu     $a1, $sp, $zero
    daddu      $a1, $s1, $zero
    mov.s      $f12, $f20
    jal        func_00102900
    daddu     $a0, $sp, $zero
    addiu      $a0, $s0, 0x10
    jal        func_00102760
    daddu     $a1, $sp, $zero
    daddu      $a1, $s2, $zero
    mov.s      $f12, $f20
    jal        func_00102900
    daddu     $a0, $sp, $zero
    addiu      $a0, $s0, 0x20
    jal        func_00102760
    daddu     $a1, $sp, $zero
    mtc1       $zero, $f0
    daddu      $a0, $s0, $zero
    .word 0x3c013f80
    mtc1       $at, $f1
    daddu      $a1, $a0, $zero
    .word 0xe6000030
    .word 0xe601003c
    .word 0xe6000038
    jal        func_00102798
    .word 0xe6000034
    .word 0xdfbf0040
    .word 0xdfb20030
    .word 0xdfb10020
    .word 0xdfb00010
    .word 0xc7b40050
    jr         $ra
    addiu     $sp, $sp, 0x60
}
