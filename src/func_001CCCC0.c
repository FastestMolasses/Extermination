// Simple nonleaf asm void
extern void DisableDmacHandler(int, int, int, int);
extern void dmac_channel_base(int, int, int, int);
extern void dma_kick(int, int, int, int);
extern void func_00102468(int, int, int, int);

asm void func_001CCCC0(void) {
    addiu      $sp, $sp, -0x220
    sq         $ra, 0x10($sp)
    sq         $s0, 0x0($sp)
    sq         $zero, 0x20($sp)
    lui        $v0, (0x11000000 >> 16)
    sw         $v0, 0x28($sp)
    lui        $v0, (0x50000017 >> 16)
    .word 0x34420017
    sw         $v0, 0x2C($sp)
    lui        $v0, (0x10000000 >> 16)
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0x8016
    or         $v0, $v0, $v1
    sd         $v0, 0x30($sp)
    addiu      $v0, $zero, 0xE
    sd         $v0, 0x38($sp)
    addiu      $v0, $zero, 0x18
    sd         $v0, 0x48($sp)
    addiu      $v0, $zero, 0x19
    sd         $zero, 0x40($sp)
    sd         $v0, 0x58($sp)
    addiu      $v0, $zero, 0x6
    sd         $zero, 0x50($sp)
    sd         $v0, 0x68($sp)
    lui        $v1, (0x20000006 >> 16)
    .word 0x34620006
    dsll32     $v0, $v0, 0
    or         $a0, $v1, $v0
    addiu      $v0, $zero, 0x7
    sd         $a0, 0x60($sp)
    sd         $v0, 0x78($sp)
    sd         $a0, 0x70($sp)
    addiu      $v0, $zero, 0x14
    sd         $v0, 0x88($sp)
    addiu      $v0, $zero, 0x15
    sd         $zero, 0x80($sp)
    sd         $v0, 0x98($sp)
    addiu      $v0, $zero, 0x16
    sd         $zero, 0x90($sp)
    sd         $v0, 0xA8($sp)
    dsll32     $v1, $v1, 0
    addiu      $v0, $zero, 0x17
    sd         $v1, 0xA0($sp)
    sd         $v0, 0xB8($sp)
    addiu      $v0, $zero, 0x8
    sd         $v1, 0xB0($sp)
    sd         $v0, 0xC8($sp)
    addiu      $v0, $zero, 0x9
    sd         $zero, 0xC0($sp)
    sd         $v0, 0xD8($sp)
    addiu      $v0, $zero, 0x34
    sd         $zero, 0xD0($sp)
    sd         $v0, 0xE8($sp)
    addiu      $v0, $zero, 0x35
    sd         $zero, 0xE0($sp)
    sd         $v0, 0xF8($sp)
    addiu      $v0, $zero, 0x36
    sd         $zero, 0xF0($sp)
    sd         $v0, 0x108($sp)
    addiu      $v0, $zero, 0x37
    sd         $zero, 0x100($sp)
    sd         $v0, 0x118($sp)
    addiu      $v0, $zero, 0x40
    sd         $zero, 0x110($sp)
    sd         $v0, 0x128($sp)
    lui        $v0, (0xE00000 >> 16)
    dsll32     $v1, $v0, 0
    lui        $v0, (0x2000000 >> 16)
    or         $v1, $v0, $v1
    addiu      $v0, $zero, 0x41
    sd         $v1, 0x120($sp)
    sd         $v0, 0x138($sp)
    sd         $v1, 0x130($sp)
    addiu      $v0, $zero, 0x42
    sd         $v0, 0x148($sp)
    addiu      $v1, $zero, 0x48
    addiu      $v0, $zero, 0x43
    sd         $v1, 0x140($sp)
    sd         $v0, 0x158($sp)
    addiu      $v0, $zero, 0x47
    sd         $v1, 0x150($sp)
    sd         $v0, 0x168($sp)
    lui        $v0, (0x5000C >> 16)
    .word 0x3442000c
    sd         $v0, 0x160($sp)
    sd         $v1, 0x178($sp)
    sd         $v0, 0x170($sp)
    addiu      $v0, $zero, 0x4A
    sd         $v0, 0x188($sp)
    addiu      $v0, $zero, 0x4B
    sd         $zero, 0x180($sp)
    sd         $v0, 0x198($sp)
    addiu      $a0, $zero, 0x1
    jal        dmac_channel_base
    sd        $zero, 0x190($sp)
    paddub     $s0, $v0, $zero
    jal        DisableDmacHandler
    paddub    $a0, $zero, $zero
    addiu      $a1, $sp, 0x20
    addiu      $a2, $zero, 0x18
    jal        dma_kick
    paddub    $a0, $s0, $zero
    paddub     $a0, $s0, $zero
    paddub     $a1, $zero, $zero
    jal        func_00102468
    paddub    $a2, $zero, $zero
    lq         $ra, 0x10($sp)
    lq         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x220
}
