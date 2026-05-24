// Simple nonleaf asm void
extern void EndOfHeap(int, int, int, int);
extern void func_00204250(int, int, int, int);

asm void func_002041D0(void) {
    addiu      $sp, $sp, -0x40
    sq         $ra, 0x10($sp)
    sq         $s0, 0x0($sp)
    dsll32     $v1, $a2, 4
    dsrl32     $v1, $v1, 4
    lui        $v0, (0x20000000 >> 16)
    sw         $a1, 0x0($a0)
    or         $v0, $v1, $v0
    sw         $v0, 0x4($a0)
    sw         $a3, 0x8($a0)
    sll        $v0, $a3, 11
    sw         $v0, 0x18($a0)
    sw         $8, 0x50($a0)
    addiu      $v0, $zero, 0x1
    sw         $9, 0x54($a0)
    paddub     $s0, $a0, $zero
    sw         $v0, 0x28($sp)
    addiu      $a0, $sp, 0x20
    jal        EndOfHeap
    sw        $v0, 0x24($sp)
    sw         $v0, 0x40($s0)
    jal        func_00204250
    paddub    $a0, $s0, $zero
    sd         $zero, 0x48($s0)
    lq         $ra, 0x10($sp)
    lq         $s0, 0x0($sp)
    addiu      $v0, $zero, 0x1
    jr         $ra
    addiu     $sp, $sp, 0x40
}
