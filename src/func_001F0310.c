// Simple nonleaf asm void
extern void func_001F03D0(int, int, int, int);
extern void func_001F3FA0(int, int, int, int);

asm void func_001F0310(void) {
    addiu      $sp, $sp, -0x10
    sq         $ra, 0x0($sp)
    jal        func_001F3FA0
    nop
    jal        func_001F03D0
    paddub    $a0, $zero, $zero
    jal        func_001F03D0
    addiu     $a0, $zero, 0x1
    jal        func_001F03D0
    addiu     $a0, $zero, 0x3
    jal        func_001F03D0
    addiu     $a0, $zero, 0x4
    jal        func_001F03D0
    addiu     $a0, $zero, 0x5
    jal        func_001F03D0
    addiu     $a0, $zero, 0x6
    lq         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x10
}
