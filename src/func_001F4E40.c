// Hybrid: branches/j as .word, jal with extern decls
extern void func_00122BB8(int, int, int, int);
extern void func_001CD520(int, int, int, int);

asm void func_001F4E40(void) {
    addiu      $sp, $sp, -0x50
    sq         $ra, 0x40($sp)
    sq         $s2, 0x30($sp)
    sq         $s1, 0x20($sp)
    sq         $s0, 0x10($sp)
    swc1       $f20, 0x0($sp)
    lui        $at, (0x70003B68 >> 16)
    lw         $v0, (0x70003B68 & 0xFFFF)($at)
    lw         $s0, 0xC($a1)
    paddub     $s2, $a0, $zero
    mov.s      $f20, $f12
    mult       $v0, $v0, $a2
    andi       $v0, $v0, 0x1FF
    addiu      $v0, $v0, -0x100
    .word 0x04410002
    paddub    $s1, $a1, $zero
    negu       $v0, $v0
    mult       $s0, $s0, $v0
    jal        func_00122BB8
    srl       $s0, $s0, 8
    sra        $v0, $v0, 23
    andi       $v0, $v0, 0xFF
    mult       $a0, $s0, $v0
    lw         $a1, 0x4($s1)
    lw         $v1, 0x0($s1)
    paddub     $a2, $s2, $zero
    lw         $v0, 0x8($s1)
    srl        $a0, $a0, 8
    addu       $s0, $s0, $a0
    mov.s      $f12, $f20
    srl        $s0, $s0, 1
    paddub     $a0, $zero, $zero
    mult       $a3, $v0, $s0
    mov.s      $f13, $f20
    lui        $v0, (0x3FC00000 >> 16)
    mtc1       $v0, $f14
    srl        $v0, $a3, 7
    mult       $a3, $a1, $s0
    sll        $8, $v0, 16
    lui        $v0, (0x20045B05 >> 16)
    .word 0x34425b05
    mult       $v1, $v1, $s0
    srl        $a3, $a3, 7
    sll        $a3, $a3, 8
    or         $8, $8, $a3
    srl        $v1, $v1, 7
    or         $8, $8, $v1
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0x9942
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x1EF0
    addiu      $a1, $zero, 0x2
    jal        func_001CD520
    or        $a3, $v0, $v1
    lq         $ra, 0x40($sp)
    lq         $s2, 0x30($sp)
    lq         $s1, 0x20($sp)
    lq         $s0, 0x10($sp)
    lwc1       $f20, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x50
}
