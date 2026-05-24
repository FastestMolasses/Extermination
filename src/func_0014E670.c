// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00122BB8(int, int, int, int);
extern void func_001B12B0(int, int, int, int);

asm void func_0014E670(void) {
    addiu      $sp, $sp, -0x40
    .word 0x7fbf0030
    .word 0x7fb10020
    .word 0x7fb00010
    .word 0xe7b40000
    .word 0x70808e28
    .word 0x90840007
    addiu      $v1, $zero, 0x1
    .word 0x10830018
    .word 0x70a08628
    .word 0x10800003
    nop
    .word 0x1000003a
    .word 0x7bbf0030
    .word 0x86030056
    .word 0x14600036
    nop
    addiu      $v0, $a0, 0x1
    .word 0xa2220007
    .word 0x82020059
    jal        func_00122BB8
    .word 0xa2020058
    sra        $v0, $v0, 17
    andi       $v0, $v0, 0x7F
    .word 0xa6020056
    .word 0x82020059
    .word 0x10400005
    .word 0x3c024020
    .word 0x3c023f00
    .word 0x10000003
    .word 0xae02003c
    .word 0x3c024020
    .word 0xae02003c
    .word 0x82020059
    .word 0x10400006
    nop
    .word 0x3c023f1c
    .word 0x344261ab
    mtc1       $v0, $f20
    .word 0x10000003
    .word 0xc601003c
    mtc1       $zero, $f20
    .word 0xc601003c
    .word 0x3c024049
    .word 0x34420fdb
    mtc1       $v0, $f2
    .word 0x8e230174
    .word 0x3c024334
    mul.s      $f1, $f2, $f1
    .word 0xc46d0078
    mtc1       $v0, $f0
    mov.s      $f12, $f20
    div.s      $f14, $f1, $f0
    nop
    nop
    jal        func_001B12B0
    nop
    .word 0x8e230174
    .word 0xe4600078
    .word 0x8e230174
    .word 0xc4600078
    c.eq.s     $f20, $f0
    nop
    .word 0x45000006
    nop
    .word 0xa2200007
    .word 0x82040059
    addiu      $v1, $zero, 0x1
    subu       $v1, $v1, $a0
    .word 0xa2030059
    .word 0x7bbf0030
    .word 0x7bb10020
    .word 0x7bb00010
    .word 0xc7b40000
    jr         $ra
    addiu     $sp, $sp, 0x40
}
