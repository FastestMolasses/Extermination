// Simple nonleaf asm void
extern void func_00122BB8(int, int, int, int);
extern void func_001CD520(int, int, int, int);

asm void func_001F4BF0(void) {
    addiu      $sp, $sp, -0x40
    sq         $ra, 0x30($sp)
    sq         $s2, 0x20($sp)
    sq         $s1, 0x10($sp)
    sq         $s0, 0x0($sp)
    lw         $s0, 0xC($a1)
    paddub     $s2, $a0, $zero
    jal        func_00122BB8
    paddub    $s1, $a1, $zero
    sra        $v0, $v0, 23
    andi       $v0, $v0, 0xFF
    mult       $a1, $s0, $v0
    lw         $a0, 0x4($s1)
    lw         $v1, 0x0($s1)
    paddub     $a2, $s2, $zero
    lw         $v0, 0x8($s1)
    srl        $a1, $a1, 8
    addu       $s0, $s0, $a1
    srl        $s0, $s0, 1
    mult       $a1, $v0, $s0
    lui        $v0, (0x40400000 >> 16)
    mtc1       $v0, $f13
    srl        $a1, $a1, 7
    sll        $8, $a1, 16
    mult       $a1, $a0, $s0
    lui        $v0, (0x3FC00000 >> 16)
    mtc1       $v0, $f14
    mov.s      $f12, $f13
    mult       $v0, $v1, $s0
    paddub     $a0, $zero, $zero
    srl        $v1, $a1, 7
    sll        $v1, $v1, 8
    or         $8, $8, $v1
    srl        $v0, $v0, 7
    or         $8, $8, $v0
    lui        $v0, (0x20045B05 >> 16)
    .word 0x34425b05
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0x9942
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x1EF0
    addiu      $a1, $zero, 0x2
    jal        func_001CD520
    or        $a3, $v0, $v1
    lq         $ra, 0x30($sp)
    lq         $s2, 0x20($sp)
    lq         $s1, 0x10($sp)
    lq         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x40
}
