// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0021BB00(int, int, int, int);
extern void func_0021BC40(int, int, int, int);

asm void func_0021BED0(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x3c017000
    lbu        $v0, (0x70003B8D & 0xFFFF)($at)
    .word 0x14400020
    .word 0x70808628
    lwc1       $f1, 0x220($s0)
    mtc1       $zero, $f0
    nop
    c.le.s     $f1, $f0
    nop
    .word 0x4501001a
    addiu     $v0, $zero, 0x1
    lbu        $v0, 0x0($s0)
    addiu      $v1, $zero, 0x1
    .word 0x14430015
    nop
    lbu        $v0, 0x4($s0)
    .word 0x14430012
    nop
    lbu        $v0, 0x236($s0)
    .word 0x1440000f
    nop
    jal        func_0021BB00
    nop
    .word 0x1440000b
    nop
    jal        func_0021BC40
    .word 0x72002628
    .word 0x14400007
    nop
    lbu        $v0, 0xA($s0)
    .word 0x10400004
    nop
    lh         $v0, 0x20E($s0)
    .word 0x10400005
    .word 0x70001628
    addiu      $v0, $zero, 0x1
    .word 0x10000003
    .word 0x7bbf0010
    .word 0x70001628
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
