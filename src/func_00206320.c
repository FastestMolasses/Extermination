// Simple nonleaf asm void
extern void func_0011A770(int, int, int, int);
extern void func_0011A7F0(int, int, int, int);
extern void func_0011A888(int, int, int, int);
extern void func_0011A918(int, int, int, int);

asm void func_00206320(void) {
    addiu      $sp, $sp, -0x20
    sq         $ra, 0x10($sp)
    sq         $s0, 0x0($sp)
    paddub     $s0, $a0, $zero
    addiu      $a0, $zero, 0x1
    paddub     $a1, $zero, $zero
    jal        func_0011A918
    paddub    $a2, $zero, $zero
    addiu      $a0, $zero, 0x2
    paddub     $a1, $zero, $zero
    jal        func_0011A918
    paddub    $a2, $zero, $zero
    jal        func_0011A888
    addiu     $a0, $zero, 0x3
    jal        func_0011A7F0
    paddub    $a0, $zero, $zero
    jal        func_0011A7F0
    addiu     $a0, $zero, 0x1
    jal        func_0011A770
    nop
    sw         $zero, 0x0($s0)
    sw         $zero, 0x2C($s0)
    sw         $zero, 0x34($s0)
    sw         $zero, 0x38($s0)
    sw         $zero, 0x40($s0)
    sw         $zero, 0x54($s0)
    sw         $zero, 0x4C($s0)
    sw         $zero, 0x50($s0)
    lq         $ra, 0x10($sp)
    lq         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x20
}
