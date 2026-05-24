// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0011DF78(int, int, int, int);
extern void func_001B12B0(int, int, int, int);
extern void func_001B1470(int, int, int, int);

asm void func_0013BA20(void) {
    addiu      $sp, $sp, -0x40
    .word 0x7fbf0030
    .word 0x7fb10020
    .word 0x7fb00010
    .word 0xe7b40000
    .word 0x8483002c
    .word 0x3c02ffff
    .word 0x34427fff
    .word 0x70a08628
    and        $v1, $v1, $v0
    .word 0x10600004
    .word 0x70808e28
    addiu      $v0, $zero, 0x4
    .word 0x14620003
    nop
    .word 0xc62000c4
    .word 0xe6000060
    .word 0xc62100c4
    .word 0xc6000060
    jal        func_001B1470
    sub.s     $f12, $f1, $f0
    mov.s      $f20, $f0
    jal        func_0011DF78
    mov.s     $f12, $f0
    .word 0x3c023d0e
    .word 0x3442fa35
    mtc1       $v0, $f1
    nop
    c.le.s     $f0, $f1
    nop
    .word 0x4501000d
    nop
    mtc1       $zero, $f0
    nop
    c.lt.s     $f20, $f0
    nop
    .word 0x45000007
    mov.s     $f20, $f1
    .word 0x3c02bd0e
    .word 0x3442fa35
    mtc1       $v0, $f20
    .word 0x10000002
    nop
    mov.s      $f20, $f1
    mtc1       $zero, $f1
    nop
    c.lt.s     $f20, $f1
    nop
    .word 0x45000010
    nop
    .word 0xc62000c8
    c.lt.s     $f0, $f1
    nop
    .word 0x45010007
    .word 0x3c023d7a
    .word 0x3c023d07
    .word 0x3442d419
    mtc1       $v0, $f14
    .word 0x10000014
    .word 0x3c023d0e
    .word 0x3c023d7a
    .word 0x344235de
    mtc1       $v0, $f14
    .word 0x1000000e
    nop
    .word 0xc62000c8
    c.le.s     $f0, $f1
    nop
    .word 0x45000007
    .word 0x3c023d7a
    .word 0x3c023d07
    .word 0x3442d419
    mtc1       $v0, $f14
    .word 0x10000004
    nop
    .word 0x3c023d7a
    .word 0x344235de
    mtc1       $v0, $f14
    .word 0x3c023d0e
    .word 0x3442fa35
    mtc1       $v0, $f0
    .word 0xc62d00c8
    div.s      $f1, $f20, $f0
    .word 0x3c023fc6
    .word 0x3442d3f2
    mtc1       $v0, $f0
    nop
    mul.s      $f0, $f0, $f1
    jal        func_001B12B0
    neg.s     $f12, $f0
    .word 0xe62000c8
    .word 0xc62000c4
    .word 0xe6000060
    .word 0x7bbf0030
    .word 0x7bb10020
    .word 0x7bb00010
    .word 0xc7b40000
    jr         $ra
    addiu     $sp, $sp, 0x40
}
