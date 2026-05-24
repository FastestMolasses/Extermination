// Simple nonleaf asm void
extern void func_001CCF70(int, int, int, int);
extern void func_001CFA60(int, int, int, int);
extern void func_001CFBE0(int, int, int, int);

asm void func_001D04B0(void) {
    addiu      $sp, $sp, -0xC0
    sq         $ra, 0x50($sp)
    sq         $s3, 0x40($sp)
    sq         $s2, 0x30($sp)
    sq         $s1, 0x20($sp)
    sq         $s0, 0x10($sp)
    swc1       $f21, 0x4($sp)
    paddub     $s3, $a0, $zero
    swc1       $f20, 0x0($sp)
    addiu      $a0, $s3, 0x30
    mov.s      $f21, $f12
    mov.s      $f20, $f13
    paddub     $s2, $a1, $zero
    jal        func_001CCF70
    paddub    $s1, $a2, $zero
    addiu      $a0, $sp, 0x60
    mov.s      $f12, $f21
    paddub     $s0, $v0, $zero
    mov.s      $f13, $f20
    jal        func_001CFA60
    paddub    $a1, $s3, $zero
    addiu      $a3, $sp, 0x60
    paddub     $a0, $s0, $zero
    paddub     $a1, $s2, $zero
    paddub     $a2, $s1, $zero
    jal        func_001CFBE0
    paddub    $8, $zero, $zero
    lq         $ra, 0x50($sp)
    lq         $s3, 0x40($sp)
    lq         $s2, 0x30($sp)
    lq         $s1, 0x20($sp)
    lq         $s0, 0x10($sp)
    lwc1       $f21, 0x4($sp)
    lwc1       $f20, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0xC0
}
