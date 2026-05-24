// Simple nonleaf asm void
extern void func_001236D8(int, int, int, int);

asm void func_00122CA8(void) {
    addiu      $sp, $sp, -0xE0
    daddu      $12, $a1, $zero
    lui        $v0, (0x7FFFFFFF >> 16)
    sw         $a0, 0x54($sp)
    .word 0x3442ffff
    daddu      $a1, $a2, $zero
    addiu      $v1, $zero, 0x208
    sd         $ra, 0x60($sp)
    sd         $a3, 0xB8($sp)
    daddu      $a0, $sp, $zero
    sd         $8, 0xC0($sp)
    addiu      $a2, $sp, 0xB8
    sd         $9, 0xC8($sp)
    sd         $10, 0xD0($sp)
    sd         $11, 0xD8($sp)
    sh         $v1, 0xC($sp)
    sw         $12, 0x10($sp)
    sw         $v0, 0x14($sp)
    sw         $12, 0x0($sp)
    jal        func_001236D8
    sw        $v0, 0x8($sp)
    lw         $v1, 0x0($sp)
    ld         $ra, 0x60($sp)
    sb         $zero, 0x0($v1)
    jr         $ra
    addiu     $sp, $sp, 0xE0
}
