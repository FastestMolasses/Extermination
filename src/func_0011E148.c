// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0011DE60(int, int, int, int);

asm void func_0011E148(void) {
    addiu      $sp, $sp, -0x20
    mfc1       $a1, $f12
    .word 0xffbf0000
    daddu      $a2, $a1, $zero
    .word 0x3c077f80
    and        $v0, $a2, $a3
    sra        $v1, $v0, 23
    .word 0x1460001c
    .word 0xe7b40010
    .word 0x3c027fff
    .word 0x3442ffff
    and        $v0, $a2, $v0
    .word 0x14400005
    .word 0xdfbf0000
    mtc1       $a1, $f0
    .word 0x10000046
    .word 0xc7b40010
    nop
    .word 0x3c014c00
    mtc1       $at, $f0
    mtc1       $a1, $f1
    mul.s      $f1, $f1, $f0
    mfc1       $a1, $f1
    daddu      $a2, $a1, $zero
    and        $v1, $a1, $a3
    .word 0x3c02ffff
    sra        $v1, $v1, 23
    .word 0x34423cb0
    slt        $v0, $a0, $v0
    .word 0x10400006
    addiu     $v1, $v1, -0x19
    .word 0x3c010da2
    .word 0x34214260
    mtc1       $at, $f0
    .word 0x10000031
    mul.s     $f0, $f1, $f0
    addiu      $v0, $zero, 0xFF
    .word 0x14620004
    addu      $v1, $v1, $a0
    mtc1       $a1, $f2
    .word 0x1000002b
    add.s     $f0, $f2, $f2
    slti       $v0, $v1, 0xFF
    .word 0x10400015
    nop
    .word 0x1860000a
    .word 0xdfbf0000
    .word 0x3c02807f
    sll        $v1, $v1, 23
    .word 0x3442ffff
    and        $v0, $a2, $v0
    or         $a1, $v0, $v1
    mtc1       $a1, $f0
    .word 0x10000020
    .word 0xc7b40010
    nop
    slti       $v0, $v1, -0x18
    .word 0x1040000f
    ori       $v0, $zero, 0xC350
    slt        $v0, $v0, $a0
    .word 0x3c010da2
    .word 0x34214260
    mtc1       $at, $f20
    .word 0x10400004
    nop
    .word 0x3c017149
    .word 0x3421f2ca
    mtc1       $at, $f20
    mtc1       $a1, $f13
    jal        func_0011DE60
    mov.s     $f12, $f20
    .word 0x1000000c
    mul.s     $f0, $f0, $f20
    addiu      $v1, $v1, 0x19
    .word 0x3c02807f
    .word 0x3442ffff
    sll        $v1, $v1, 23
    and        $v0, $a2, $v0
    or         $a1, $v0, $v1
    .word 0x3c013300
    mtc1       $at, $f0
    mtc1       $a1, $f1
    mul.s      $f0, $f1, $f0
    nop
    .word 0xdfbf0000
    .word 0xc7b40010
    jr         $ra
    addiu     $sp, $sp, 0x20
}
