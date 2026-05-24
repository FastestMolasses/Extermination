// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void DisableDmacHandler(int, int, int, int);
extern void GsPutIMR(int, int, int, int);
extern void iGsPutIMR(int, int, int, int);

asm void func_00206970(void) {
    addiu      $sp, $sp, -0x40
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x70c08e28
    .word 0x1e200003
    .word 0x7fb00000
    .word 0x10000015
    .word 0x70001628
    .word 0xafa50030
    .word 0xafa40034
    .word 0xafb10038
    .word 0x70002628
    jal        DisableDmacHandler
    .word 0xafa0003c
    addiu      $a0, $sp, 0x30
    jal        iGsPutIMR
    addiu     $a1, $zero, 0x1
    .word 0x70408628
    jal        GsPutIMR
    .word 0x72002628
    nop
    nop
    nop
    .word 0x0441fffa
    nop
    jal        DisableDmacHandler
    .word 0x70002628
    .word 0x72201628
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x40
}
