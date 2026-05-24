// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00102900(int, int, int, int);
extern void func_00103230(int, int, int, int);
extern void func_00121870(int, int, int, int);
extern void func_001281C0(int, int, int, int);

asm void func_001D0400(void) {
    addiu      $sp, $sp, -0x30
    .word 0x7fbf0020
    .word 0x7fb00010
    .word 0xe7b40000
    addiu      $a2, $zero, 0x90
    mov.s      $f20, $f12
    jal        func_00121870
    .word 0x70808628
    .word 0x72002628
    .word 0x72002e28
    jal        func_00103230
    mov.s     $f12, $f20
    addiu      $a0, $s0, 0x10
    mov.s      $f12, $f20
    jal        func_00103230
    .word 0x70802e28
    addiu      $a0, $s0, 0x40
    mov.s      $f12, $f20
    jal        func_00102900
    .word 0x70802e28
    addiu      $a0, $s0, 0x50
    mov.s      $f12, $f20
    jal        func_00102900
    .word 0x70802e28
    .word 0xc6000078
    mul.s      $f0, $f0, $f20
    .word 0xe6000078
    .word 0xc600007c
    mul.s      $f0, $f0, $f20
    .word 0xe600007c
    .word 0xc6000084
    mul.s      $f0, $f0, $f20
    .word 0xe6000084
    .word 0xc6000080
    cvt.s.w    $f0, $f0
    jal        func_001281C0
    mul.s     $f12, $f0, $f20
    .word 0xae020080
    .word 0x7bbf0020
    .word 0x7bb00010
    .word 0xc7b40000
    jr         $ra
    addiu     $sp, $sp, 0x30
}
