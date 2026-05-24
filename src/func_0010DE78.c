// Simple nonleaf asm void
extern void func_0010DD00(int, int, int, int);

asm void func_0010DE78(void) {
    daddu      $v0, $a2, $zero
    daddu      $v1, $a3, $zero
    daddu      $11, $8, $zero
    addiu      $sp, $sp, -0x10
    daddu      $10, $9, $zero
    daddu      $a2, $a1, $zero
    sd         $ra, 0x0($sp)
    daddu      $a3, $v0, $zero
    daddu      $8, $v1, $zero
    daddu      $9, $11, $zero
    jal        func_0010DD00
    addiu     $a1, $zero, 0x1
    ld         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x10
}
