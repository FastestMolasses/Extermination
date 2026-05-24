// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00121E30(int, int, int, int);

asm void func_001226E8(void) {
    addiu      $sp, $sp, -0x70
    sd         $s4, 0x50($sp)
    sd         $s2, 0x30($sp)
    addiu      $s4, $a0, 0x14
    sd         $s0, 0x10($sp)
    sd         $ra, 0x60($sp)
    daddu      $s0, $a1, $zero
    sd         $s3, 0x40($sp)
    sd         $s1, 0x20($sp)
    lw         $v0, 0x10($a0)
    sll        $v0, $v0, 2
    addu       $s2, $s4, $v0
    addiu      $s2, $s2, -0x4
    lw         $s3, 0x0($s2)
    jal        func_00121E30
    daddu     $a0, $s3, $zero
    daddu      $a2, $v0, $zero
    addiu      $v0, $zero, 0x20
    slti       $v1, $a2, 0xB
    subu       $v0, $v0, $a2
    .word 0x1060001a
    sw        $v0, 0x0($s0)
    addiu      $v0, $zero, 0xB
    .word 0x3c043ff0
    subu       $v0, $v0, $a2
    .word 0x3c03ffff
    dsrl32     $v1, $v1, 0
    srlv       $v0, $s3, $v0
    and        $s1, $s1, $v1
    or         $v0, $v0, $a0
    sltu       $v1, $s4, $s2
    dsll32     $v0, $v0, 0
    or         $s1, $s1, $v0
    .word 0x10600002
    daddu     $a0, $zero, $zero
    lw         $a0, -0x4($s2)
    addiu      $v0, $zero, 0xB
    addiu      $v1, $a2, 0x15
    subu       $v0, $v0, $a2
    sllv       $v1, $s3, $v1
    srlv       $v0, $a0, $v0
    or         $v1, $v1, $v0
    addiu      $a0, $zero, -0x1
    dsll32     $a0, $a0, 0
    dsll32     $v1, $v1, 0
    .word 0x1000001f
    and       $s1, $s1, $a0
    sltu       $v0, $s4, $s2
    .word 0x10400003
    daddu     $a3, $zero, $zero
    addiu      $s2, $s2, -0x4
    lw         $a3, 0x0($s2)
    addiu      $a2, $a2, -0xB
    .word 0x10c0001a
    negu      $v0, $a2
    .word 0x3c053ff0
    srlv       $v0, $a3, $v0
    sllv       $v1, $s3, $a2
    or         $v0, $v0, $a1
    .word 0x3c04ffff
    dsrl32     $a0, $a0, 0
    or         $v1, $v1, $v0
    and        $s1, $s1, $a0
    dsll32     $v1, $v1, 0
    sltu       $v0, $s4, $s2
    or         $s1, $s1, $v1
    .word 0x10400002
    daddu     $s3, $zero, $zero
    lw         $s3, -0x4($s2)
    negu       $v0, $a2
    sllv       $v1, $a3, $a2
    srlv       $v0, $s3, $v0
    addiu      $a0, $zero, -0x1
    dsll32     $a0, $a0, 0
    or         $v1, $v1, $v0
    and        $s1, $s1, $a0
    dsll32     $v1, $v1, 0
    dsrl32     $v1, $v1, 0
    .word 0x10000007
    or        $s1, $s1, $v1
    .word 0x3c033ff0
    or         $v1, $s3, $v1
    dsll32     $v0, $a3, 0
    dsrl32     $v0, $v0, 0
    dsll32     $s1, $v1, 0
    or         $s1, $s1, $v0
    daddu      $v0, $s1, $zero
    ld         $ra, 0x60($sp)
    ld         $s4, 0x50($sp)
    ld         $s3, 0x40($sp)
    ld         $s2, 0x30($sp)
    ld         $s1, 0x20($sp)
    ld         $s0, 0x10($sp)
    jr         $ra
    addiu     $sp, $sp, 0x70
}
