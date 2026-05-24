// Hybrid: branches/j as .word, jal with extern decls
extern void func_00100610(int, int, int, int);

asm void func_001006D8(void) {
    addiu      $sp, $sp, -0x70
    sll        $a2, $a2, 16
    sd         $s2, 0x20($sp)
    sll        $a1, $a1, 16
    sra        $s2, $a2, 16
    sd         $s4, 0x40($sp)
    addiu      $v0, $s2, 0x3F
    sra        $s4, $a1, 16
    sra        $v0, $v0, 6
    andi       $v1, $s4, 0xF
    andi       $v0, $v0, 0x3F
    sd         $s0, 0x0($sp)
    dsll       $v1, $v1, 24
    dsll       $v0, $v0, 16
    daddu      $s0, $a0, $zero
    or         $v0, $v0, $v1
    sd         $s5, 0x50($sp)
    sll        $a3, $a3, 16
    sd         $s3, 0x30($sp)
    sll        $8, $8, 16
    sd         $s1, 0x10($sp)
    sll        $9, $9, 16
    sd         $ra, 0x60($sp)
    addiu      $v1, $zero, 0x4C
    addiu      $a0, $zero, 0x4E
    sra        $s1, $a3, 16
    sra        $s5, $8, 16
    sra        $s3, $9, 16
    sd         $v1, 0x8($s0)
    sd         $v0, 0x0($s0)
    .word 0x16a0000e
    sd        $a0, 0x18($s0)
    daddu      $a0, $s4, $zero
    daddu      $a1, $s2, $zero
    jal        func_00100610
    daddu     $a2, $s1, $zero
    dsll32     $v0, $v0, 16
    andi       $v1, $s3, 0xF
    dsra32     $v0, $v0, 16
    dsll       $v1, $v1, 24
    or         $v0, $v0, $v1
    ori        $a0, $zero, 0x8000
    dsll       $a0, $a0, 17
    .word 0x1000000a
    or        $v0, $v0, $a0
    daddu      $a0, $s4, $zero
    daddu      $a1, $s2, $zero
    jal        func_00100610
    daddu     $a2, $s1, $zero
    dsll32     $v0, $v0, 16
    andi       $v1, $s3, 0xF
    dsra32     $v0, $v0, 16
    dsll       $v1, $v1, 24
    or         $v0, $v0, $v1
    sd         $v0, 0x10($s0)
    sra        $v0, $s1, 1
    sra        $v1, $s2, 1
    dsll32     $v0, $v0, 16
    addiu      $a0, $zero, 0x800
    dsll32     $v1, $v1, 16
    dsra32     $v0, $v0, 16
    dsra32     $v1, $v1, 16
    dsubu      $v0, $a0, $v0
    dsubu      $a0, $a0, $v1
    dsll32     $v0, $v0, 4
    addiu      $a2, $s2, -0x1
    addiu      $a1, $s1, -0x1
    dsll       $a0, $a0, 4
    ld         $v1, 0x40($s0)
    ld         $a3, 0x50($s0)
    dsll32     $a1, $a1, 16
    or         $a0, $a0, $v0
    dsll       $a2, $a2, 16
    addiu      $11, $zero, 0x1
    or         $a2, $a2, $a1
    addiu      $v0, $zero, 0x18
    or         $v1, $v1, $11
    or         $a3, $a3, $11
    addiu      $a1, $zero, 0x40
    addiu      $8, $zero, 0x1A
    addiu      $9, $zero, 0x46
    addiu      $10, $zero, 0x45
    sd         $v0, 0x28($s0)
    sd         $a0, 0x20($s0)
    andi       $v0, $s4, 0x2
    sd         $a1, 0x38($s0)
    sd         $a2, 0x30($s0)
    sd         $8, 0x48($s0)
    sd         $v1, 0x40($s0)
    sd         $9, 0x58($s0)
    sd         $a3, 0x50($s0)
    .word 0x10400004
    sd        $10, 0x68($s0)
    ld         $v0, 0x60($s0)
    .word 0x10000004
    or        $v0, $v0, $11
    ld         $v0, 0x60($s0)
    addiu      $v1, $zero, -0x2
    and        $v0, $v0, $v1
    sd         $v0, 0x60($s0)
    addiu      $v0, $zero, 0x47
    .word 0x12a00006
    sd        $v0, 0x78($s0)
    andi       $v0, $s5, 0x3
    lui        $v1, (0x10000 >> 16)
    dsll       $v0, $v0, 17
    .word 0x10000002
    or        $v0, $v0, $v1
    lui        $v0, (0x30000 >> 16)
    sd         $v0, 0x70($s0)
    sync
    ld         $ra, 0x60($sp)
    addiu      $v0, $zero, 0x8
    ld         $s5, 0x50($sp)
    ld         $s4, 0x40($sp)
    ld         $s3, 0x30($sp)
    ld         $s2, 0x20($sp)
    ld         $s1, 0x10($sp)
    ld         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x70
}
