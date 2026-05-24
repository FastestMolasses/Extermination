// Simple nonleaf asm void
extern void func_00207D00(int, int, int, int);
extern void func_00207E40(int, int, int, int);
extern void func_0020A7A0(int, int, int, int);

asm void func_0020F170(void) {
    lui        $v0, (0x2003C8A5 >> 16)
    .word 0x3442c8a5
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0x9D42
    addiu      $sp, $sp, -0x10
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x1DB0
    sq         $ra, 0x0($sp)
    jal        func_0020A7A0
    or        $a0, $v0, $v1
    lui        $v0, (0x40808080 >> 16)
    .word 0x34498080
    lui        $v0, (0x2003C405 >> 16)
    .word 0x3442c405
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0xDD32
    addiu      $a3, $zero, 0x80
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x1D00
    addiu      $a0, $zero, 0x1
    addiu      $a1, $zero, 0x7BC0
    addiu      $a2, $zero, 0x7CF0
    paddub     $8, $a3, $zero
    jal        func_00207E40
    or        $10, $v0, $v1
    lui        $v0, (0x40808080 >> 16)
    .word 0x34498080
    lui        $v0, (0x2003CA85 >> 16)
    .word 0x3442ca85
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0xE142
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x1D40
    addiu      $a0, $zero, 0x1
    addiu      $a1, $zero, 0x7800
    addiu      $a3, $zero, 0x100
    addiu      $8, $zero, 0x80
    ori        $a2, $zero, 0x8300
    jal        func_00207E40
    or        $10, $v0, $v1
    addiu      $a0, $zero, 0x1
    jal        func_00207D00
    addiu     $a1, $zero, 0x3
    lui        $v0, (0x80808080 >> 16)
    .word 0x34498080
    lui        $v0, (0x2003CA25 >> 16)
    .word 0x3442ca25
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0xDD42
    addiu      $a3, $zero, 0x80
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x1D20
    addiu      $a0, $zero, 0x1
    addiu      $a1, $zero, 0x7000
    ori        $a2, $zero, 0x8300
    paddub     $8, $a3, $zero
    jal        func_00207E40
    or        $10, $v0, $v1
    lui        $v0, (0x80808080 >> 16)
    .word 0x34498080
    lui        $v0, (0x2003C885 >> 16)
    .word 0x3442c885
    dsll32     $v1, $v0, 0
    ori        $v0, $zero, 0x9D42
    dsll       $v0, $v0, 16
    ori        $v0, $v0, 0x1DA0
    addiu      $a0, $zero, 0x1
    addiu      $a1, $zero, 0x7100
    addiu      $a2, $zero, 0x7900
    addiu      $a3, $zero, 0x80
    addiu      $8, $zero, 0x40
    jal        func_00207E40
    or        $10, $v0, $v1
    lq         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x10
}
