// Simple nonleaf asm void
extern void func_0011DE90(int, int, int, int);
extern void func_0011E2A8(int, int, int, int);
extern void func_001790B0(int, int, int, int);

asm void func_00179150(void) {
    addiu      $sp, $sp, -0x30
    sq         $ra, 0x20($sp)
    sq         $s0, 0x10($sp)
    swc1       $f20, 0x0($sp)
    lwc1       $f12, 0x9C($a0)
    jal        func_0011DE90
    paddub    $s0, $a0, $zero
    lwc1       $f1, 0x38($s0)
    lwc1       $f12, 0xC4($s0)
    jal        func_0011E2A8
    mul.s     $f20, $f1, $f0
    mul.s      $f1, $f0, $f20
    lwc1       $f0, 0xB0($s0)
    add.s      $f0, $f0, $f1
    swc1       $f0, 0xB0($s0)
    jal        func_0011DE90
    lwc1      $f12, 0x9C($s0)
    lwc1       $f1, 0x38($s0)
    lwc1       $f12, 0xC4($s0)
    jal        func_0011DE90
    mul.s     $f20, $f1, $f0
    mul.s      $f1, $f0, $f20
    paddub     $a0, $s0, $zero
    lwc1       $f0, 0xB8($s0)
    add.s      $f0, $f0, $f1
    jal        func_001790B0
    swc1      $f0, 0xB8($s0)
    lq         $ra, 0x20($sp)
    lq         $s0, 0x10($sp)
    lwc1       $f20, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x30
}
