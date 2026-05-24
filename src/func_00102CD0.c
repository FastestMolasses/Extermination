// Simple nonleaf asm void
extern void func_00102718(int, int, int, int);
extern void func_00102760(int, int, int, int);
extern void func_001027E0(int, int, int, int);
extern void func_00102918(int, int, int, int);
extern void func_001029C0(int, int, int, int);

asm void func_00102CD0(void) {
    addiu      $sp, $sp, -0xB0
    sd         $s4, 0x90($sp)
    daddu      $s4, $a0, $zero
    sd         $s3, 0x80($sp)
    sd         $s2, 0x70($sp)
    daddu      $s3, $a1, $zero
    sd         $s1, 0x60($sp)
    daddu      $s2, $a2, $zero
    daddu      $s1, $a3, $zero
    sd         $s0, 0x50($sp)
    sd         $ra, 0xA0($sp)
    jal        func_001029C0
    daddu     $a0, $sp, $zero
    addiu      $s0, $sp, 0x40
    daddu      $a1, $s1, $zero
    daddu      $a0, $s0, $zero
    jal        func_00102718
    daddu     $a2, $s2, $zero
    daddu      $a1, $s0, $zero
    jal        func_00102760
    daddu     $a0, $sp, $zero
    addiu      $s0, $sp, 0x20
    daddu      $a1, $s2, $zero
    jal        func_00102760
    daddu     $a0, $s0, $zero
    daddu      $a1, $s0, $zero
    addiu      $a0, $sp, 0x10
    jal        func_00102718
    daddu     $a2, $sp, $zero
    daddu      $a2, $s3, $zero
    daddu      $a0, $sp, $zero
    jal        func_00102918
    daddu     $a1, $sp, $zero
    daddu      $a0, $s4, $zero
    jal        func_001027E0
    daddu     $a1, $sp, $zero
    ld         $ra, 0xA0($sp)
    ld         $s4, 0x90($sp)
    ld         $s3, 0x80($sp)
    ld         $s2, 0x70($sp)
    ld         $s1, 0x60($sp)
    ld         $s0, 0x50($sp)
    jr         $ra
    addiu     $sp, $sp, 0xB0
}
