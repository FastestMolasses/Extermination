// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00102738(int, int, int, int);
extern void func_001028B8(int, int, int, int);
extern void func_001028D0(int, int, int, int);
extern void func_00103230(int, int, int, int);
extern void func_0011DBB8(int, int, int, int);
extern void func_0011DF78(int, int, int, int);
extern void func_0011E748(int, int, int, int);

asm void func_0019F330(void) {
    addiu      $sp, $sp, -0x110
    .word 0x7fbf0090
    .word 0x7fb70080
    .word 0x7fb60070
    .word 0x7fb50060
    .word 0x7fb40050
    .word 0x7fb30040
    .word 0x7fb20030
    .word 0x7fb10020
    .word 0x7fb00010
    .word 0x70c0be28
    .word 0x70e0b628
    .word 0xe7b50004
    addiu      $a2, $sp, 0xA0
    addiu      $a3, $sp, 0xB0
    .word 0xe7b40000
    .word 0x70001e28
    .word 0xc4800000
    addiu      $v1, $v1, 0x1
    slti       $v0, $v1, 0x3
    .word 0xe4c00000
    .word 0xc4a00000
    addiu      $a0, $a0, 0x4
    addiu      $a2, $a2, 0x4
    .word 0xe4e00000
    addiu      $a1, $a1, 0x4
    .word 0x1440fff6
    addiu     $a3, $a3, 0x4
    addiu      $a0, $sp, 0xF0
    addiu      $a1, $sp, 0xB0
    addiu      $a2, $sp, 0xA0
    .word 0xafa000ac
    jal        func_001028D0
    .word 0xafa000bc
    addiu      $a0, $sp, 0x100
    .word 0x70002e28
    .word 0x72c01e28
    .word 0xc4600024
    addiu      $a1, $a1, 0x1
    slti       $v0, $a1, 0x3
    .word 0xe4800000
    addiu      $v1, $v1, 0x4
    .word 0x1440fffa
    addiu     $a0, $a0, 0x4
    .word 0xafa0010c
    .word 0xc6d40030
    addiu      $a0, $sp, 0xF0
    jal        func_00102738
    addiu     $a1, $sp, 0x100
    addiu      $a0, $sp, 0x100
    mov.s      $f21, $f0
    jal        func_00102738
    addiu     $a1, $sp, 0xA0
    sub.s      $f0, $f20, $f0
    addiu      $a0, $sp, 0xC0
    addiu      $a1, $sp, 0xF0
    div.s      $f12, $f0, $f21
    nop
    nop
    jal        func_00103230
    nop
    addiu      $a0, $sp, 0xC0
    addiu      $a1, $sp, 0xA0
    jal        func_001028B8
    .word 0x70803628
    .word 0x3c017000
    .word 0x8c243204
    .word 0x8ec5001c
    .word 0x8ec30020
    .word 0x7000ae28
    .word 0x3c017000
    .word 0x8c223200
    addu       $s0, $a0, $a1
    .word 0x10000038
    addu      $s1, $v0, $v1
    .word 0x86050000
    .word 0x3c017000
    .word 0x8c2231fc
    addiu      $s2, $sp, 0xE4
    addiu      $s3, $sp, 0xE8
    sll        $v1, $a1, 1
    addu       $v1, $v1, $a1
    sll        $v1, $v1, 2
    addu       $v1, $v0, $v1
    .word 0xc4600000
    addiu      $s4, $sp, 0xEC
    addiu      $a0, $sp, 0xD0
    addiu      $a1, $sp, 0xC0
    .word 0xe7a000e0
    .word 0x86070000
    addiu      $a2, $sp, 0xE0
    sll        $v1, $a3, 1
    addu       $v1, $v1, $a3
    sll        $v1, $v1, 2
    addu       $v1, $v0, $v1
    .word 0xc4600004
    .word 0xe6400000
    .word 0x86070000
    sll        $v1, $a3, 1
    addu       $v1, $v1, $a3
    sll        $v1, $v1, 2
    addu       $v0, $v0, $v1
    .word 0xc4400008
    .word 0xe6600000
    jal        func_001028D0
    .word 0xae800000
    .word 0xc6200000
    addiu      $a0, $sp, 0xD0
    addiu      $a1, $sp, 0xE0
    .word 0xe7a000e0
    .word 0xc6200004
    .word 0xe6400000
    .word 0xc6200008
    .word 0xe6600000
    jal        func_00102738
    .word 0xae800000
    .word 0x3c023727
    .word 0x3442c5ac
    mtc1       $v0, $f1
    nop
    c.le.s     $f0, $f1
    nop
    .word 0x45030004
    addiu     $s0, $s0, 0x2
    .word 0x10000048
    .word 0x70001628
    addiu      $s0, $s0, 0x2
    addiu      $s1, $s1, 0xC
    addiu      $s5, $s5, 0x1
    nop
    .word 0x92c20018
    slt        $v0, $s5, $v0
    .word 0x1440ffc6
    nop
    addiu      $v1, $sp, 0xC0
    .word 0x70002e28
    .word 0x72e02628
    .word 0xc4600000
    addiu      $a1, $a1, 0x1
    slti       $v0, $a1, 0x3
    .word 0xe4800000
    addiu      $v1, $v1, 0x4
    .word 0x1440fffa
    addiu     $a0, $a0, 0x4
    .word 0xc7a10100
    .word 0xc7a00108
    mula.s     $f1, $f1
    jal        func_0011E748
    madd.s    $f12, $f0, $f0
    .word 0x3c0238d1
    .word 0x3442b717
    mtc1       $v0, $f1
    .word 0x3c017000
    c.lt.s     $f0, $f1
    nop
    .word 0x45000006
    .word 0xe4203680
    .word 0x3c027f7f
    .word 0x3442c99e
    .word 0x3c017000
    .word 0x10000008
    .word 0xac223684
    jal        func_0011DF78
    .word 0xc7ac0104
    .word 0x3c017000
    .word 0xc4213680
    div.s      $f0, $f0, $f1
    .word 0x3c017000
    .word 0xe4203684
    .word 0xc7a10104
    mtc1       $zero, $f0
    nop
    c.lt.s     $f1, $f0
    nop
    .word 0x4500000c
    nop
    .word 0x3c017000
    jal        func_0011DBB8
    .word 0xc42c3684
    .word 0x3c023fc9
    .word 0x34420fdb
    mtc1       $v0, $f1
    nop
    sub.s      $f0, $f1, $f0
    neg.s      $f0, $f0
    .word 0x1000000a
    .word 0xe6e0000c
    .word 0x3c017000
    jal        func_0011DBB8
    .word 0xc42c3684
    .word 0x3c023fc9
    .word 0x34420fdb
    mtc1       $v0, $f1
    nop
    sub.s      $f0, $f1, $f0
    .word 0xe6e0000c
    addiu      $v0, $zero, 0x1
    .word 0x7bbf0090
    .word 0x7bb70080
    .word 0x7bb60070
    .word 0x7bb50060
    .word 0x7bb40050
    .word 0x7bb30040
    .word 0x7bb20030
    .word 0x7bb10020
    .word 0x7bb00010
    .word 0xc7b50004
    .word 0xc7b40000
    jr         $ra
    addiu     $sp, $sp, 0x110
}
