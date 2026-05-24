// Simple nonleaf asm void
extern void func_001C7420(int, int, int, int);
extern void func_001D2830(int, int, int, int);
extern void func_001D2910(int, int, int, int);
extern void func_001D3BA0(int, int, int, int);
extern void func_001D8C20(int, int, int, int);

asm void func_001CB480(void) {
    addiu      $sp, $sp, -0x30
    sq         $ra, 0x20($sp)
    sq         $s1, 0x10($sp)
    paddub     $s1, $a0, $zero
    paddub     $a0, $zero, $zero
    jal        func_001D2910
    sq        $s0, 0x0($sp)
    addiu      $a0, $zero, 0x2
    jal        func_001D8C20
    paddub    $s0, $v0, $zero
    paddub     $a0, $zero, $zero
    jal        func_001D2830
    paddub    $a1, $zero, $zero
    addiu      $a1, $zero, 0x3F5
    addiu      $a2, $zero, 0x1
    jal        func_001C7420
    paddub    $a0, $s1, $zero
    lw         $a1, 0x44($s1)
    jal        func_001D3BA0
    addiu     $a0, $zero, 0x1
    paddub     $a1, $s0, $zero
    jal        func_001D2830
    paddub    $a0, $zero, $zero
    lq         $ra, 0x20($sp)
    lq         $s1, 0x10($sp)
    lq         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x30
}
