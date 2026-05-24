// Simple nonleaf asm void
extern void func_0010D2C8(int, int, int, int);

asm void func_0010D890(void) {
    addiu      $sp, $sp, -0x90
    sd         $a1, 0x58($sp)
    sd         $ra, 0x0($sp)
    addiu      $a1, $sp, 0x58
    sd         $a2, 0x60($sp)
    sd         $a3, 0x68($sp)
    sd         $8, 0x70($sp)
    sd         $9, 0x78($sp)
    sd         $10, 0x80($sp)
    jal        func_0010D2C8
    sd        $11, 0x88($sp)
    ld         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x90
}
