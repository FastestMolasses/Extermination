// Simple nonleaf asm void
extern void func_00122BB8(int, int, int, int);
extern void func_001CD520(int, int, int, int);

asm void func_001F4D40(void) {
    addiu      $sp, $sp, -0x50
    sq         $ra, 0x40($sp)
    sq         $s2, 0x30($sp)
    sq         $s1, 0x20($sp)
    sq         $s0, 0x10($sp)
    swc1       $f21, 0x4($sp)
    swc1       $f20, 0x0($sp)
    lw         $s1, 0xC($a1)
    paddub     $s2, $a0, $zero
    mov.s      $f21, $f12
    mov.s      $f20, $f13
    jal        func_00122BB8
    paddub    $s0, $a1, $zero
    sra        $v0, $v0, 23
    andi       $v0, $v0, 0xFF
    mult       $a0, $s1, $v0
    lw         $v1, 0x8($s0)
    lw         $a3, 0x0($s0)
    addiu      $a1, $zero, 0x2
    mov.s      $f14, $f20
    sll        $v0, $s1, 1
    addu       $v0, $v0, $s1
    mov.s      $f12, $f21
    srl        $a0, $a0, 8
    addu       $10, $v0, $a0
    mov.s      $f13, $f21
    srl        $10, $10, 2
    mult       $v1, $v1, $10
    lw         $v0, 0x4($s0)
    paddub     $a2, $s2, $zero
    paddub     $a0, $zero, $zero
    srl        $v1, $v1, 7
    sll        $8, $v1, 16
    mult       $9, $v0, $10
    lui        $v0, (0x20045B05 >> 16)
    .word 0x34425b05
    dsll32     $v1, $v0, 0
    mult       $v0, $a3, $10
    srl        $a3, $9, 7
    sll        $a3, $a3, 8
    or         $8, $8, $a3
    srl        $v0, $v0, 7
    or         $8, $8, $v0
    ori        $v0, $zero, 0x9942
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x1EF0
    jal        func_001CD520
    or        $a3, $v0, $v1
    lq         $ra, 0x40($sp)
    lq         $s2, 0x30($sp)
    lq         $s1, 0x20($sp)
    lq         $s0, 0x10($sp)
    lwc1       $f21, 0x4($sp)
    lwc1       $f20, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x50
}
