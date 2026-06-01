// Simple nonleaf asm void
extern void float_to_int(int, int, int, int);
extern void func_00207F80(int, int, int, int);

asm void func_0020CCB0(void) {
    addiu      $sp, $sp, -0x20
    sq         $ra, 0x10($sp)
    sq         $s0, 0x0($sp)
    lbu        $a0, 0x6($a0)
    lui        $v0, (0x41800000 >> 16)
    addiu      $s0, $zero, 0xFD
    mtc1       $v0, $f0
    addiu      $v1, $zero, 0x14F
    movn       $s0, $v1, $a0
    addiu      $v0, $s0, 0x700
    mtc1       $v0, $f1
    nop
    cvt.s.w    $f1, $f1
    jal        float_to_int
    mul.s     $f12, $f0, $f1
    addiu      $v1, $s0, 0x70C
    mtc1       $v1, $f0
    paddub     $s0, $v0, $zero
    cvt.s.w    $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $v0, $f0
    jal        float_to_int
    mul.s     $f12, $f0, $f1
    paddub     $a3, $v0, $zero
    lui        $v0, (0x80CE6000 >> 16)
    addiu      $a0, $zero, 0x1
    paddub     $a1, $s0, $zero
    ori        $a2, $zero, 0x85E0
    ori        $8, $zero, 0x8640
    jal        func_00207F80
    .word 0x34496000
    lq         $ra, 0x10($sp)
    lq         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x20
}
