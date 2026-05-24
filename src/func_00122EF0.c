// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00123168(int, int, int, int);

asm void func_00122EF0(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fb00000
    daddu      $s0, $a0, $zero
    andi       $v0, $s0, 0x7
    .word 0x14400038
    .word 0x7fbf0010
    andi       $v0, $s0, 0xF
    .word 0x3c030101
    .word 0x34630101
    dsll       $v1, $v1, 16
    ori        $v1, $v1, 0x101
    dsll       $v1, $v1, 16
    ori        $v1, $v1, 0x101
    .word 0x3c048080
    .word 0x34848080
    dsll       $a0, $a0, 16
    ori        $a0, $a0, 0x8080
    dsll       $a0, $a0, 16
    ori        $a0, $a0, 0x8080
    .word 0x14400018
    .word 0xde060000
    .word 0x7a020000
    .word 0x70633b89
    .word 0x70844389
    .word 0x70471a48
    .word 0x700214e9
    .word 0x70621c89
    .word 0x70681c89
    .word 0x706313a9
    or         $v1, $v0, $v1
    .word 0x1460001e
    daddu     $a0, $s0, $zero
    addiu      $a2, $a0, 0x10
    .word 0x78c20000
    .word 0x70021ce9
    .word 0x70471248
    .word 0x70431489
    .word 0x70481489
    .word 0x70421ba9
    or         $v0, $v0, $v1
    .word 0x5040fff8
    addiu     $a2, $a2, 0x10
    .word 0x10000012
    daddu     $a0, $a2, $zero
    daddu      $a3, $v1, $zero
    daddu      $8, $a0, $zero
    dsubu      $v1, $a2, $v1
    nor        $v0, $zero, $a2
    and        $v1, $v1, $v0
    and        $v1, $v1, $a0
    .word 0x1460000a
    daddu     $a0, $s0, $zero
    addiu      $a2, $s0, 0x8
    .word 0xdcc20000
    nor        $v1, $zero, $v0
    dsubu      $v0, $v0, $a3
    and        $v0, $v0, $v1
    and        $v0, $v0, $8
    .word 0x5040fffa
    addiu     $a2, $a2, 0x8
    daddu      $a0, $a2, $zero
    .word 0x80820000
    nop
    nop
    nop
    nop
    .word 0x5440fffa
    addiu     $a0, $a0, 0x1
    jal        func_00123168
    nop
    daddu      $v0, $s0, $zero
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
