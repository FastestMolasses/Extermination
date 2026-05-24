// Simple nonleaf asm void
extern void func_00108AA0(int, int, int, int);

asm void func_00108D90(void) {
    addiu      $sp, $sp, -0x10
    daddu      $a3, $zero, $zero
    sd         $ra, 0x0($sp)
    jal        func_00108AA0
    addiu     $8, $zero, -0x1
    ld         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x10
}
