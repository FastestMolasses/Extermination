// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00102948(int, int, int, int);
extern void func_002082B0(int, int, int, int);

asm void func_001DC890(void) {
    .word 0x3c023f00
    mtc1       $v0, $f1
    addiu      $sp, $sp, -0x90
    mul.s      $f0, $f1, $f12
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x3c024700
    add.s      $f2, $f1, $f0
    .word 0x3c03430e
    .word 0xafa20030
    .word 0xafa20034
    mtc1       $v1, $f1
    addiu      $s1, $sp, 0x50
    mul.s      $f2, $f2, $f1
    .word 0xafa00038
    .word 0x3c0243b4
    .word 0xafa2003c
    .word 0x3c024040
    mtc1       $v0, $f0
    .word 0xe7a20040
    add.s      $f0, $f0, $f2
    .word 0xe7a20044
    .word 0x70808628
    .word 0x3c0742aa
    .word 0xe7a00048
    .word 0xe7a0004c
    .word 0x3c064343
    .word 0xae270000
    .word 0x3c03434d
    .word 0xafa60054
    .word 0x3c024300
    .word 0xafa30058
    addiu      $a0, $sp, 0x60
    .word 0x72202e28
    jal        func_00102948
    .word 0xafa2005c
    addiu      $a0, $sp, 0x70
    jal        func_00102948
    .word 0x72202e28
    addiu      $a0, $sp, 0x80
    jal        func_00102948
    .word 0x72202e28
    addiu      $a1, $sp, 0x30
    jal        func_002082B0
    .word 0x72002628
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x90
}
