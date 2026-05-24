// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void _DisableDmac(int, int, int, int);

asm void func_0010C4A0(void) {
    addiu      $sp, $sp, -0x20
    .word 0xffbf0010
    .word 0xffb00000
    mfc0       $s0, $12 /* handwritten instruction */
    .word 0x3c020001
    and        $s0, $s0, $v0
    .word 0x12000008
    .word 0x3c030001
    di /* handwritten instruction */
    sync.p
    mfc0       $v0, $12 /* handwritten instruction */
    and        $v0, $v0, $v1
    nop
    .word 0x1440fffa
    nop
    jal        _DisableDmac
    nop
    sync
    .word 0x12000003
    .word 0xdfbf0010
    ei /* handwritten instruction */
    .word 0xdfbf0010
    .word 0xdfb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
