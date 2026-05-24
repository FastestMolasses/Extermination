// Hybrid: branches/j as .word, jal with extern decls
extern void func_001157F0(int, int, int, int);

asm void func_0011A788(void) {
    addiu      $sp, $sp, -0x10
    lui        $v1, (0x1FFFFF >> 16)
    sd         $ra, 0x0($sp)
    .word 0x3463ffff
    lw         $a2, 0x8($a0)
    sltu       $v0, $v1, $a2
    .word 0x1440000f
    addiu     $8, $zero, -0x1
    lw         $a3, 0xC($a0)
    sltu       $v0, $v1, $a3
    .word 0x1440000c
    ld        $ra, 0x0($sp)
    lw         $a1, 0x0($a0)
    slti       $v0, $a1, 0x10
    .word 0x10400009
    daddu     $v0, $8, $zero
    lw         $v0, 0x4($a0)
    sll        $a1, $a1, 24
    addiu      $a0, $zero, 0x48
    jal        func_001157F0
    or        $a1, $a1, $v0
    daddu      $8, $zero, $zero
    ld         $ra, 0x0($sp)
    daddu      $v0, $8, $zero
    jr         $ra
    addiu     $sp, $sp, 0x10
}
