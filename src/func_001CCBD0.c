// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void DisableDmacHandler(int, int, int, int);
extern void dmac_channel_base(int, int, int, int);
extern void dma_kick(int, int, int, int);
extern void func_00102468(int, int, int, int);
extern void func_001CCE80(int, int, int, int);

asm void func_001CCBD0(void) {
    addiu      $sp, $sp, -0x1F0
    .word 0x7fbf0060
    .word 0x7fb40050
    .word 0x7fb30040
    .word 0x7fb20030
    .word 0x7080a628
    .word 0x7fb10020
    addiu      $a0, $zero, 0x1
    .word 0x70a09e28
    .word 0x70c09628
    jal        dmac_channel_base
    .word 0x7fb00010
    slt        $at, $s3, $s4
    .word 0x14200024
    .word 0x70408628
    addiu      $s1, $sp, 0xE0
    addiu      $v0, $zero, 0x10
    addiu      $10, $zero, 0x8
    addiu      $a0, $sp, 0x70
    addiu      $a2, $zero, 0x1
    .word 0xffa20000
    .word 0x72802e28
    .word 0x70003e28
    .word 0x70004628
    .word 0x70004e28
    jal        func_001CCE80
    .word 0x71405e28
    .word 0x70001e28
    addu       $v0, $s1, $v1
    addiu      $v1, $v1, 0x4
    .word 0xac520000
    slti       $v0, $v1, 0x100
    nop
    .word 0x1440fffa
    nop
    jal        DisableDmacHandler
    .word 0x70002628
    addiu      $a1, $sp, 0x70
    addiu      $a2, $zero, 0x17
    jal        dma_kick
    .word 0x72002628
    .word 0x72002628
    .word 0x70002e28
    jal        func_00102468
    .word 0x70003628
    addiu      $s4, $s4, 0x1
    slt        $at, $s3, $s4
    .word 0x5020ffe1
    addiu     $v0, $zero, 0x10
    nop
    .word 0x7bbf0060
    .word 0x7bb40050
    .word 0x7bb30040
    .word 0x7bb20030
    .word 0x7bb10020
    .word 0x7bb00010
    jr         $ra
    addiu     $sp, $sp, 0x1F0
}
