// Hybrid: branches/j as .word, jal with extern decls
extern void func_00102A60(int, int, int, int);
extern void func_00102BB0(int, int, int, int);
extern void func_00102B08(int, int, int, int);

asm void func_00102C58(void) {
    addiu      $sp, $sp, -0x30
    sd         $s1, 0x10($sp)
    sd         $s0, 0x0($sp)
    daddu      $s1, $a2, $zero
    sd         $ra, 0x20($sp)
    daddu      $s0, $a0, $zero
    jal        func_00102A60
    lwc1      $f12, 0x8($s1)
    lwc1       $f12, 0x4($s1)
    daddu      $a0, $s0, $zero
    jal        func_00102BB0
    daddu     $a1, $s0, $zero
    daddu      $a0, $s0, $zero
    lwc1       $f12, 0x0($s1)
    ld         $ra, 0x20($sp)
    daddu      $a1, $a0, $zero
    ld         $s1, 0x10($sp)
    ld         $s0, 0x0($sp)
    j         func_00102B08
    addiu     $sp, $sp, 0x30
}
