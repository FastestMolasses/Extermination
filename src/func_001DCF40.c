// Simple nonleaf asm void
extern void func_001D1F80(int, int, int, int);
extern void func_00207F80(int, int, int, int);

asm void func_001DCF40(void) {
    addiu      $sp, $sp, -0x30
    sq         $ra, 0x20($sp)
    sq         $s1, 0x10($sp)
    sq         $s0, 0x0($sp)
    paddub     $s0, $a1, $zero
    addiu      $a2, $zero, 0x1
    paddub     $s1, $a0, $zero
    jal        func_001D1F80
    paddub    $a1, $zero, $zero
    addiu      $a1, $zero, 0x7730
    addiu      $a2, $zero, 0x7F10
    addiu      $a3, $zero, 0x7770
    paddub     $a0, $s1, $zero
    ori        $8, $zero, 0x80D0
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a1, $zero, 0x7A70
    addiu      $a2, $zero, 0x7F90
    addiu      $a3, $zero, 0x7AB0
    paddub     $a0, $s1, $zero
    ori        $8, $zero, 0x8050
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a2, $zero, 0x7F90
    paddub     $a0, $s1, $zero
    ori        $a1, $zero, 0x8550
    ori        $a3, $zero, 0x8590
    ori        $8, $zero, 0x8050
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a2, $zero, 0x7F10
    paddub     $a0, $s1, $zero
    paddub     $9, $s0, $zero
    ori        $a1, $zero, 0x8890
    ori        $a3, $zero, 0x88D0
    jal        func_00207F80
    ori       $8, $zero, 0x80D0
    lq         $ra, 0x20($sp)
    lq         $s1, 0x10($sp)
    lq         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x30
}
