// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001FB9F0(int, int, int, int);
extern void func_001FBF50(int, int, int, int);

asm void func_001FBD50(void) {
    addiu      $sp, $sp, -0x30
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x70a08628
    .word 0x70c03e28
    .word 0x3c024580
    mtc1       $v0, $f13
    addiu      $a1, $sp, 0x28
    jal        func_001FBF50
    addiu     $a2, $sp, 0x2C
    .word 0x10400009
    addiu     $v0, $zero, -0x1
    .word 0x8fa60028
    .word 0x8fa7002c
    addiu      $a1, $zero, 0x1000
    jal        func_001FB9F0
    .word 0x72002628
    .word 0x10000003
    .word 0x7bbf0010
    addiu      $v0, $zero, -0x1
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x30
}
