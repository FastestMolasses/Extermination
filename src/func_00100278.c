// CFLAGS: -O4,p -sdatathreshold 0
extern int D_00241020[4];

asm void func_00100278(void) {
    lui      $v0, 0x1000
    addiu    $a3, $zero, 1
    ori      $v0, $v0, 0x3c10
    lui      $v1, 0x1000
    sw       $a3, 0x0($v0)
    ori      $v1, $v1, 0x3c20
    addiu    $v0, $zero, 2
    daddu    $a0, $zero, $zero
    sw       $v0, 0x0($v1)
    sync
    cfc2.ni  $a0, $vi28
    ori      $a0, $a0, 0x200
    ctc2.ni  $a0, $vi28
    sync.p
    lui      $a1, 0x24
    lui      $a2, 0x1000
    addiu    $a1, $a1, 0x1020
    ori      $a2, $a2, 0x5000
    lq       $a0, 0x0($a1)
    lui      $v1, 0x1000
    ori      $v1, $v1, 0x3000
    sq       $a0, 0x0($a2)
    lq       $v0, 0x10($a1)
    sq       $v0, 0x0($a2)
    jr       $ra
    sw       $a3, 0x0($v1)
}
