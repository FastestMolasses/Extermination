// Simple nonleaf asm void
extern void func_00123750(int, int, int, int);

asm void func_00122B18(void) {
    addiu      $sp, $sp, -0x80
    sd         $a2, 0x50($sp)
    sd         $a3, 0x58($sp)
    daddu      $a2, $a1, $zero
    sd         $ra, 0x0($sp)
    addiu      $a3, $sp, 0x50
    sd         $8, 0x60($sp)
    sd         $9, 0x68($sp)
    sd         $10, 0x70($sp)
    sd         $11, 0x78($sp)
    jal        func_00123750
    lw        $a1, 0x8($a0)
    ld         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x80
}
