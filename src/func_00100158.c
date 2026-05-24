// Hybrid: branches/j as .word, jal with extern decls
extern void AddIntcHandler2(int, int, int, int);
extern void SetCPUTimer(int, int, int, int);
extern void func_00100268(int, int, int, int);
extern void func_0010C290(int, int, int, int);
extern void SetGsCrt(int, int, int, int);

asm void func_00100158(void) {
    addiu      $sp, $sp, -0x50
    sll        $a0, $a0, 16
    sd         $s3, 0x30($sp)
    sll        $a1, $a1, 16
    sd         $s2, 0x20($sp)
    sll        $a2, $a2, 16
    sd         $s1, 0x10($sp)
    sll        $a3, $a3, 16
    sd         $ra, 0x40($sp)
    sra        $a0, $a0, 16
    sd         $s0, 0x0($sp)
    sra        $s1, $a1, 16
    sra        $s2, $a2, 16
    .word 0x10800006
    sra       $s3, $a3, 16
    addiu      $v0, $zero, 0x1
    .word 0x10820028
    ld        $ra, 0x40($sp)
    .word 0x1000002c
    ld        $s3, 0x30($sp)
    jal        func_00100268
    nop
    daddu      $s0, $v0, $zero
    lui        $v1, (0x12001000 >> 16)
    .word 0x34631000
    addiu      $v0, $zero, 0x200
    sd         $v0, 0x0($v1)
    ori        $a0, $zero, 0xFF00
    sh         $s1, 0x0($s0)
    ld         $v0, 0x0($v1)
    sh         $s2, 0x2($s0)
    dsrl       $v0, $v0, 16
    andi       $v0, $v0, 0xFF
    .word 0x3042ffff
    jal        SetCPUTimer
    sh        $v0, 0x6($s0)
    sltu       $v1, $zero, $s3
    lw         $v0, 0x8($s0)
    .word 0x10400008
    sh        $v1, 0x4($s0)
    jal        func_0010C290
    addiu     $a0, $zero, 0x2
    lw         $a1, 0xC($s0)
    jal        AddIntcHandler2
    addiu     $a0, $zero, 0x2
    sw         $zero, 0xC($s0)
    sw         $zero, 0x8($s0)
    andi       $a0, $s1, 0x1
    andi       $a1, $s2, 0xFF
    andi       $a2, $s3, 0x1
    ld         $ra, 0x40($sp)
    ld         $s3, 0x30($sp)
    ld         $s2, 0x20($sp)
    ld         $s1, 0x10($sp)
    ld         $s0, 0x0($sp)
    j         SetGsCrt
    addiu     $sp, $sp, 0x50
    lui        $v0, (0x12001000 >> 16)
    addiu      $v1, $zero, 0x100
    .word 0x34421000
    sd         $v1, 0x0($v0)
    ld         $ra, 0x40($sp)
    ld         $s3, 0x30($sp)
    ld         $s2, 0x20($sp)
    ld         $s1, 0x10($sp)
    ld         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x50
}
