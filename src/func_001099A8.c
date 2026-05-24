// Simple nonleaf asm void
extern void func_00109CF8(int, int, int, int);

asm void func_001099A8(void) {
    addiu      $sp, $sp, -0x10
    lui        $v0, (0xFFFFFFF >> 16)
    sd         $ra, 0x0($sp)
    mult       $8, $a2, $a3
    .word 0x3442ffff
    sll        $a2, $a2, 4
    and        $a1, $a1, $v0
    lw         $v1, 0x40($a0)
    lui        $v0, (0x20000000 >> 16)
    sll        $a3, $a3, 4
    or         $a1, $a1, $v0
    sw         $a3, 0xE0($v1)
    sw         $a1, 0xD8($v1)
    sw         $8, 0xE4($v1)
    sw         $a2, 0xDC($v1)
    jal        func_00109CF8
    sw        $zero, 0xB0($v1)
    ld         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x10
}
