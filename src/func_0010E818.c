// Hybrid: branches/j as .word, jal with extern decls
extern void func_0010C710(int, int, int, int);

asm void func_0010E818(void) {
    lw         $a1, 0x34($a0)
    lw         $a2, 0x40($a1)
    lw         $v0, 0xC($a2)
    .word 0x54400003
    lw        $v0, 0x10($a2)
    .word 0x10000002
    sw        $a1, 0xC($a2)
    sw         $a1, 0x3C($v0)
    sw         $a1, 0x10($a2)
    lw         $v0, 0x14($a0)
    lw         $v1, 0x1C($a0)
    sw         $v0, 0x20($a1)
    sw         $v1, 0x1C($a1)
    lw         $v0, 0x20($a0)
    sw         $v0, 0x24($a1)
    lw         $v1, 0x24($a0)
    sw         $v1, 0xC($a1)
    lw         $v0, 0x28($a0)
    sw         $v0, 0x28($a1)
    lw         $v1, 0x2C($a0)
    sw         $v1, 0x2C($a1)
    lw         $v0, 0x30($a0)
    sw         $v0, 0x30($a1)
    lw         $v1, 0x10($a0)
    sw         $v1, 0x34($a1)
    lw         $a0, 0x0($a2)
    .word 0x04800006
    nop
    lw         $v0, 0x4($a2)
    .word 0x14400003
    nop
    j         func_0010C710
    nop
    jr         $ra
    nop
}
