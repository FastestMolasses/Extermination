// Simple nonleaf asm void
extern void func_001F0720(int, int, int, int);
extern void func_001F40C0(int, int, int, int);
extern void func_001F5C20(int, int, int, int);
extern void func_001F6210(int, int, int, int);
extern void func_001F6BB0(int, int, int, int);
extern void func_001F6EB0(int, int, int, int);

asm void func_001F0360(void) {
    addiu      $sp, $sp, -0x10
    sq         $ra, 0x0($sp)
    jal        func_001F6210
    nop
    jal        func_001F5C20
    nop
    jal        func_001F6BB0
    nop
    jal        func_001F6EB0
    nop
    jal        func_001F40C0
    nop
    jal        func_001F0720
    paddub    $a0, $zero, $zero
    jal        func_001F0720
    addiu     $a0, $zero, 0x1
    jal        func_001F0720
    addiu     $a0, $zero, 0x3
    jal        func_001F0720
    addiu     $a0, $zero, 0x4
    jal        func_001F0720
    addiu     $a0, $zero, 0x5
    jal        func_001F0720
    addiu     $a0, $zero, 0x6
    lq         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x10
}
