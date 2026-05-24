// Simple nonleaf asm void
extern void func_001C7420(int, int, int, int);
extern void func_001D1F80(int, int, int, int);
extern void func_001D2830(int, int, int, int);
extern void func_001D2910(int, int, int, int);
extern void func_001D38F0(int, int, int, int);
extern void func_001D8C20(int, int, int, int);

asm void func_001CB4F0(void) {
    addiu      $sp, $sp, -0x40
    sq         $ra, 0x30($sp)
    sq         $s2, 0x20($sp)
    sq         $s1, 0x10($sp)
    paddub     $s2, $a0, $zero
    paddub     $s1, $a1, $zero
    paddub     $a0, $zero, $zero
    jal        func_001D2910
    sq        $s0, 0x0($sp)
    paddub     $s0, $v0, $zero
    paddub     $a0, $zero, $zero
    jal        func_001D2830
    paddub    $a1, $zero, $zero
    jal        func_001D8C20
    addiu     $a0, $zero, 0x1
    addiu      $a1, $zero, 0x3F5
    paddub     $a0, $s2, $zero
    jal        func_001C7420
    paddub    $a2, $zero, $zero
    addiu      $a1, $zero, 0x1
    paddub     $a0, $zero, $zero
    jal        func_001D1F80
    paddub    $a2, $zero, $zero
    jal        func_001D38F0
    paddub    $a0, $s1, $zero
    jal        func_001D8C20
    paddub    $a0, $zero, $zero
    paddub     $a1, $s0, $zero
    jal        func_001D2830
    paddub    $a0, $zero, $zero
    lq         $ra, 0x30($sp)
    lq         $s2, 0x20($sp)
    lq         $s1, 0x10($sp)
    lq         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x40
}
