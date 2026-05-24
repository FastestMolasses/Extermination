// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001FBD50(int, int, int, int);

asm void func_0021D490(void) {
    addiu      $sp, $sp, -0x10
    .word 0x7fbf0000
    .word 0x90820234
    .word 0x14400009
    .word 0x3c024396
    .word 0x3c024396
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0x14E
    jal        func_001FBD50
    .word 0x70003628
    .word 0x10000007
    .word 0x7bbf0000
    .word 0x3c024396
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0x14F
    jal        func_001FBD50
    .word 0x70003628
    .word 0x7bbf0000
    jr         $ra
    addiu     $sp, $sp, 0x10
}
