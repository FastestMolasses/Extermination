// Hybrid: branches/j as .word, jal with extern decls
extern void func_0011FD88(int, int, int, int);
extern void func_001236D8(int, int, int, int);

asm void func_00123620(void) {
    addiu      $sp, $sp, -0x490
    addiu      $10, $zero, 0x400
    sd         $s1, 0x470($sp)
    addiu      $11, $sp, 0x60
    sd         $s0, 0x460($sp)
    daddu      $s1, $a0, $zero
    sd         $ra, 0x480($sp)
    daddu      $a0, $sp, $zero
    lhu        $v0, 0xC($s1)
    lw         $8, 0x54($s1)
    lhu        $9, 0xE($s1)
    andi       $v0, $v0, 0xFFFD
    lw         $a3, 0x1C($s1)
    lw         $v1, 0x24($s1)
    sw         $8, 0x54($sp)
    sh         $v0, 0xC($sp)
    sh         $9, 0xE($sp)
    sw         $a3, 0x1C($sp)
    sw         $v1, 0x24($sp)
    sw         $11, 0x10($sp)
    sw         $10, 0x14($sp)
    sw         $11, 0x0($sp)
    sw         $10, 0x8($sp)
    jal        func_001236D8
    sw        $zero, 0x18($sp)
    daddu      $s0, $v0, $zero
    .word 0x06000006
    lhu       $v0, 0xC($sp)
    jal        func_0011FD88
    daddu     $a0, $sp, $zero
    addiu      $v1, $zero, -0x1
    movn       $s0, $v1, $v0
    lhu        $v0, 0xC($sp)
    andi       $v0, $v0, 0x40
    .word 0x10400005
    daddu     $v0, $s0, $zero
    lhu        $v0, 0xC($s1)
    ori        $v0, $v0, 0x40
    sh         $v0, 0xC($s1)
    daddu      $v0, $s0, $zero
    ld         $ra, 0x480($sp)
    ld         $s1, 0x470($sp)
    ld         $s0, 0x460($sp)
    jr         $ra
    addiu     $sp, $sp, 0x490
}
