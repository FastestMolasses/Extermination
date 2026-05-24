// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0011C7B0(int, int, int, int);
extern void func_0011CCC8(int, int, int, int);
extern void func_0011D770(int, int, int, int);

asm void func_0011E2A8(void) {
    mfc1       $v0, $f12
    addiu      $sp, $sp, -0x20
    daddu      $a0, $v0, $zero
    .word 0x3c037fff
    .word 0x3463ffff
    .word 0x3c023f49
    and        $a0, $a0, $v1
    .word 0x34420fd8
    slt        $v0, $v0, $a0
    .word 0x14400006
    .word 0xffbf0010
    mtc1       $zero, $f13
    jal        func_0011D770
    daddu     $a0, $zero, $zero
    .word 0x1000002b
    .word 0xdfbf0010
    .word 0x3c027f7f
    .word 0x3442ffff
    slt        $v0, $v0, $a0
    .word 0x10400003
    nop
    .word 0x10000023
    sub.s     $f0, $f12, $f12
    jal        func_0011C7B0
    daddu     $a0, $sp, $zero
    andi       $v1, $v0, 0x3
    addiu      $v0, $zero, 0x1
    .word 0x10620010
    slti      $v0, $v1, 0x2
    .word 0x10400005
    addiu     $v0, $zero, 0x2
    .word 0x10600007
    .word 0xc7ac0000
    .word 0x10000014
    nop
    .word 0x1062000d
    .word 0xc7ac0000
    .word 0x10000010
    nop
    addiu      $a0, $zero, 0x1
    jal        func_0011D770
    .word 0xc7ad0004
    .word 0x1000000f
    .word 0xdfbf0010
    .word 0xc7ac0000
    jal        func_0011CCC8
    .word 0xc7ad0004
    .word 0x1000000a
    .word 0xdfbf0010
    addiu      $a0, $zero, 0x1
    jal        func_0011D770
    .word 0xc7ad0004
    .word 0x10000004
    neg.s     $f0, $f0
    jal        func_0011CCC8
    .word 0xc7ad0004
    neg.s      $f0, $f0
    .word 0xdfbf0010
    jr         $ra
    addiu     $sp, $sp, 0x20
}
