// Simple nonleaf asm void
extern void func_001D1F80(int, int, int, int);
extern void func_00207F80(int, int, int, int);

asm void func_001DCD40(void) {
    addiu      $sp, $sp, -0x30
    sq         $ra, 0x20($sp)
    sq         $s1, 0x10($sp)
    sq         $s0, 0x0($sp)
    paddub     $s0, $a1, $zero
    addiu      $a2, $zero, 0x1
    paddub     $s1, $a0, $zero
    jal        func_001D1F80
    paddub    $a1, $zero, $zero
    addiu      $a1, $zero, 0x72D0
    addiu      $a2, $zero, 0x7BA0
    addiu      $a3, $zero, 0x7730
    addiu      $8, $zero, 0x7BF0
    paddub     $a0, $s1, $zero
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a1, $zero, 0x72D0
    addiu      $a2, $zero, 0x7BF0
    addiu      $a3, $zero, 0x7320
    addiu      $8, $zero, 0x7EA0
    paddub     $a0, $s1, $zero
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a1, $zero, 0x7440
    addiu      $a2, $zero, 0x7CA0
    addiu      $a3, $zero, 0x75F0
    addiu      $8, $zero, 0x7CC0
    paddub     $a0, $s1, $zero
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a1, $zero, 0x7440
    addiu      $a2, $zero, 0x7CC0
    addiu      $a3, $zero, 0x7480
    addiu      $8, $zero, 0x7EA0
    paddub     $a0, $s1, $zero
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a2, $zero, 0x7BA0
    addiu      $8, $zero, 0x7BF0
    paddub     $a0, $s1, $zero
    ori        $a1, $zero, 0x88D0
    ori        $a3, $zero, 0x8D30
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a2, $zero, 0x7BF0
    addiu      $8, $zero, 0x7EA0
    paddub     $a0, $s1, $zero
    ori        $a1, $zero, 0x8CE0
    ori        $a3, $zero, 0x8D30
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a2, $zero, 0x7CA0
    addiu      $8, $zero, 0x7CC0
    paddub     $a0, $s1, $zero
    ori        $a1, $zero, 0x8A10
    ori        $a3, $zero, 0x8BC0
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a2, $zero, 0x7CC0
    addiu      $8, $zero, 0x7EA0
    paddub     $a0, $s1, $zero
    ori        $a1, $zero, 0x8B80
    ori        $a3, $zero, 0x8BC0
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a1, $zero, 0x72D0
    addiu      $a3, $zero, 0x7730
    paddub     $a0, $s1, $zero
    ori        $a2, $zero, 0x83E0
    ori        $8, $zero, 0x8430
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a1, $zero, 0x72D0
    addiu      $a3, $zero, 0x7320
    paddub     $a0, $s1, $zero
    ori        $a2, $zero, 0x8140
    ori        $8, $zero, 0x83E0
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a1, $zero, 0x7440
    addiu      $a3, $zero, 0x75F0
    paddub     $a0, $s1, $zero
    ori        $a2, $zero, 0x8320
    ori        $8, $zero, 0x8330
    jal        func_00207F80
    paddub    $9, $s0, $zero
    addiu      $a1, $zero, 0x7440
    addiu      $a3, $zero, 0x7480
    paddub     $a0, $s1, $zero
    ori        $a2, $zero, 0x8140
    ori        $8, $zero, 0x8320
    jal        func_00207F80
    paddub    $9, $s0, $zero
    paddub     $a0, $s1, $zero
    ori        $a1, $zero, 0x88D0
    ori        $a2, $zero, 0x83E0
    ori        $a3, $zero, 0x8D30
    ori        $8, $zero, 0x8430
    jal        func_00207F80
    paddub    $9, $s0, $zero
    paddub     $a0, $s1, $zero
    ori        $a1, $zero, 0x8CE0
    ori        $a2, $zero, 0x8140
    ori        $a3, $zero, 0x8D30
    ori        $8, $zero, 0x83E0
    jal        func_00207F80
    paddub    $9, $s0, $zero
    paddub     $a0, $s1, $zero
    ori        $a1, $zero, 0x8A10
    ori        $a2, $zero, 0x8320
    ori        $a3, $zero, 0x8BC0
    ori        $8, $zero, 0x8330
    jal        func_00207F80
    paddub    $9, $s0, $zero
    paddub     $a0, $s1, $zero
    paddub     $9, $s0, $zero
    ori        $a1, $zero, 0x8B80
    ori        $a2, $zero, 0x8140
    ori        $a3, $zero, 0x8BC0
    jal        func_00207F80
    ori       $8, $zero, 0x8320
    lq         $ra, 0x20($sp)
    lq         $s1, 0x10($sp)
    lq         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x30
}
