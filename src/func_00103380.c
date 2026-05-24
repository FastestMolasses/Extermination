// CFLAGS: -O4,p -sdatathreshold 0

asm void func_00103380(void) {
    lui      $v0, 0x1000
    lui      $a0, 0x1000
    ori      $v0, $v0, 0x3830
    ori      $a0, $a0, 0x3820
    sw       $zero, 0x0($v0)
    lui      $v1, 0x1000
    sw       $zero, 0x0($a0)
    ori      $v1, $v1, 0x3810
    addiu    $v0, $zero, 0x1
    sw       $v0, 0x0($v1)
    cfc2.ni  $8, $vi28
    ori      $8, $8, 0x2
    ctc2.ni  $8, $vi28
    sync.p
    lui      $a0, 0x24
    lui      $a1, 0x1000
    addiu    $a0, $a0, 0x1110
    ori      $a1, $a1, 0x4000
    lq       $v0, 0x0($a0)
    sq       $v0, 0x0($a1)
    lq       $v1, 0x10($a0)
    jr       $ra
    sq       $v1, 0x0($a1)
}
