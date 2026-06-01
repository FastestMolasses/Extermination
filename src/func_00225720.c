// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void float_to_int(int, int, int, int);
extern void func_00207D00(int, int, int, int);
extern void func_00207E40(int, int, int, int);

asm void func_00225720(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x70808628
    addiu      $a0, $zero, 0x1
    jal        func_00207D00
    .word 0x70002e28
    .word 0x3c024080
    .word 0x34498080
    .word 0x3c022004
    .word 0x34423f06
    dsll32     $v1, $v0, 0
    addiu      $a3, $zero, 0x100
    .word 0x3c022132
    .word 0x34421f00
    addiu      $a0, $zero, 0x1
    addiu      $a1, $zero, 0x7000
    addiu      $a2, $zero, 0x7F00
    .word 0x70e04628
    jal        func_00207E40
    or        $10, $v0, $v1
    .word 0x3c024080
    .word 0x34498080
    .word 0x3c022004
    .word 0x34423f86
    dsll32     $v1, $v0, 0
    addiu      $a3, $zero, 0x100
    .word 0x3c022132
    .word 0x34421f40
    addiu      $a0, $zero, 0x1
    addiu      $a2, $zero, 0x7F00
    ori        $a1, $zero, 0x8000
    .word 0x70e04628
    jal        func_00207E40
    or        $10, $v0, $v1
    .word 0x3c024080
    .word 0x34498080
    .word 0x3c022004
    .word 0x34423e86
    dsll32     $v1, $v0, 0
    addiu      $a3, $zero, 0x100
    .word 0x3c022132
    .word 0x34421d40
    addiu      $a0, $zero, 0x1
    addiu      $a1, $zero, 0x7000
    addiu      $a2, $zero, 0x7A90
    .word 0x70e04628
    jal        func_00207E40
    or        $10, $v0, $v1
    .word 0x3c024080
    .word 0x34498080
    .word 0x3c022004
    .word 0x34423e06
    dsll32     $v1, $v0, 0
    addiu      $a3, $zero, 0x100
    .word 0x3c022132
    .word 0x34421d00
    addiu      $a0, $zero, 0x1
    addiu      $a2, $zero, 0x7A90
    ori        $a1, $zero, 0x8000
    .word 0x70e04628
    jal        func_00207E40
    or        $10, $v0, $v1
    addiu      $a0, $zero, 0x1
    jal        func_00207D00
    addiu     $a1, $zero, 0x3
    .word 0x3c027080
    .word 0x34498080
    .word 0x3c022004
    .word 0x34424a05
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0xDD42
    addiu      $a3, $zero, 0x80
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x2180
    addiu      $a0, $zero, 0x1
    addiu      $a2, $zero, 0x7900
    ori        $a1, $zero, 0x8800
    .word 0x70e04628
    jal        func_00207E40
    or        $10, $v0, $v1
    lbu        $v1, 0x14($s0)
    addiu      $v0, $zero, 0x2
    .word 0x14620013
    .word 0x3c027080
    .word 0x3c027080
    .word 0x34498080
    .word 0x3c022004
    .word 0x34424085
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0x9D32
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x21C0
    addiu      $a0, $zero, 0x1
    addiu      $a1, $zero, 0x7080
    addiu      $a2, $zero, 0x7900
    addiu      $a3, $zero, 0x100
    addiu      $8, $zero, 0x40
    jal        func_00207E40
    or        $10, $v0, $v1
    .word 0x10000011
    lbu       $a2, 0x15($s0)
    .word 0x3c027080
    .word 0x34498080
    .word 0x3c022004
    .word 0x34424005
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0x9D32
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x21A0
    addiu      $a0, $zero, 0x1
    addiu      $a1, $zero, 0x7080
    addiu      $a2, $zero, 0x7900
    addiu      $a3, $zero, 0x100
    addiu      $8, $zero, 0x40
    jal        func_00207E40
    or        $10, $v0, $v1
    lbu        $a2, 0x15($s0)
    addiu      $v1, $zero, 0x2
    .word 0x14c3003b
    nop
    lbu        $v1, 0xA($s0)
    .word 0x3c024180
    mtc1       $v0, $f0
    sll        $v0, $v1, 1
    addu       $v0, $v0, $v1
    sll        $v0, $v0, 3
    addiu      $v0, $v0, 0x72
    sra        $v0, $v0, 1
    addiu      $v0, $v0, 0x790
    mtc1       $v0, $f1
    nop
    cvt.s.w    $f1, $f1
    jal        float_to_int
    mul.s     $f12, $f0, $f1
    .word 0x70403628
    .word 0x3c024080
    .word 0x34498080
    .word 0x3c022004
    .word 0x34424405
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0xA132
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x2100
    addiu      $a0, $zero, 0x1
    addiu      $a1, $zero, 0x7000
    addiu      $a3, $zero, 0x100
    addiu      $8, $zero, 0x40
    jal        func_00207E40
    or        $10, $v0, $v1
    lbu        $v1, 0xA($s0)
    .word 0x3c024180
    mtc1       $v0, $f0
    sll        $v0, $v1, 1
    addu       $v0, $v0, $v1
    sll        $v0, $v0, 3
    addiu      $v0, $v0, 0x72
    sra        $v0, $v0, 1
    addiu      $v0, $v0, 0x790
    mtc1       $v0, $f1
    nop
    cvt.s.w    $f1, $f1
    jal        float_to_int
    mul.s     $f12, $f0, $f1
    .word 0x70403628
    .word 0x3c024080
    .word 0x34498080
    .word 0x3c022004
    .word 0x34424485
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0xA132
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x2140
    addiu      $a0, $zero, 0x1
    addiu      $a3, $zero, 0x100
    addiu      $8, $zero, 0x40
    ori        $a1, $zero, 0x8000
    jal        func_00207E40
    or        $10, $v0, $v1
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
