// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void DisableDmacHandler(int, int, int, int);
extern void dmac_channel_base(int, int, int, int);
extern void dma_kick(int, int, int, int);
extern void func_00102468(int, int, int, int);
extern void func_001CCBD0(int, int, int, int);
extern void func_001CCE80(int, int, int, int);

asm void func_001CCB10(void) {
    addiu      $sp, $sp, -0x1B0
    .word 0x7fbf0020
    addiu      $a0, $zero, 0x1
    jal        dmac_channel_base
    .word 0x7fb00010
    addiu      $a0, $zero, 0x1B00
    addiu      $a1, $zero, 0x1B80
    .word 0x70408628
    jal        func_001CCBD0
    .word 0x70003628
    addiu      $v0, $zero, 0x10
    addiu      $10, $zero, 0x8
    addiu      $a0, $sp, 0x30
    addiu      $a1, $zero, 0x1B67
    addiu      $a2, $zero, 0x1
    .word 0xffa20000
    .word 0x70003e28
    .word 0x70004628
    .word 0x70004e28
    jal        func_001CCE80
    .word 0x71405e28
    addiu      $a3, $sp, 0xA0
    .word 0x70002e28
    .word 0x70003628
    .word 0x3c020010
    .word 0x34441010
    .word 0x3c038000
    addu       $v0, $a3, $a1
    addiu      $a1, $a1, 0x4
    .word 0xac460000
    addu       $a2, $a2, $a0
    slti       $v0, $a1, 0x100
    .word 0x1440fffa
    or        $a2, $a2, $v1
    jal        DisableDmacHandler
    .word 0x70002628
    addiu      $a1, $sp, 0x30
    addiu      $a2, $zero, 0x17
    jal        dma_kick
    .word 0x72002628
    .word 0x72002628
    .word 0x70002e28
    jal        func_00102468
    .word 0x70003628
    .word 0x7bbf0020
    .word 0x7bb00010
    jr         $ra
    addiu     $sp, $sp, 0x1B0
}
