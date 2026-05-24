// Simple nonleaf asm void
extern void func_001D25F0(int, int, int, int);
extern void func_001D2610(int, int, int, int);
extern void func_001D2830(int, int, int, int);

asm void func_001D2880(void) {
    addiu      $sp, $sp, -0x10
    lui        $v0, (0x43F00000 >> 16)
    mtc1       $v0, $f12
    sq         $ra, 0x0($sp)
    jal        func_001D25F0
    nop
    paddub     $a0, $zero, $zero
    jal        func_001D2830
    paddub    $a1, $zero, $zero
    addiu      $a0, $zero, 0x2
    jal        func_001D2830
    paddub    $a1, $zero, $zero
    addiu      $a0, $zero, 0x1
    jal        func_001D2830
    paddub    $a1, $zero, $zero
    addiu      $a0, $zero, 0x24
    jal        func_001D2830
    paddub    $a1, $zero, $zero
    addiu      $a0, $zero, 0x20
    jal        func_001D2830
    paddub    $a1, $zero, $zero
    addiu      $a0, $zero, 0x21
    jal        func_001D2830
    paddub    $a1, $zero, $zero
    addiu      $a0, $zero, 0x22
    jal        func_001D2830
    paddub    $a1, $zero, $zero
    mtc1       $zero, $f12
    jal        func_001D2610
    nop
    lq         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x10
}
