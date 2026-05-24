// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00102738(int, int, int, int);
extern void func_001028B8(int, int, int, int);
extern void func_001028D0(int, int, int, int);
extern void func_00103230(int, int, int, int);
extern void func_0011DBB8(int, int, int, int);
extern void func_0011DF78(int, int, int, int);
extern void func_0011E748(int, int, int, int);

asm void func_001A58B0(void) {
    addiu      $sp, $sp, -0x110
    .word 0x7fbf00a0
    .word 0x7fbe0090
    .word 0x7fb70080
    .word 0x7fb60070
    .word 0x7fb50060
    .word 0x7fb40050
    .word 0x7fb30040
    .word 0x7fb20030
    .word 0x7fb10020
    .word 0x7fb00010
    .word 0x70c0ae28
    addiu      $s0, $s5, 0x4
    .word 0x70a0b628
    addiu      $a2, $sp, 0xD0
    .word 0xe7b40000
    .word 0x70001e28
    .word 0x72002e28
    .word 0xc4a00000
    addiu      $v1, $v1, 0x1
    slti       $v0, $v1, 0x3
    .word 0xe4c00000
    addiu      $a1, $a1, 0x4
    .word 0x1440fffa
    addiu     $a2, $a2, 0x4
    .word 0xc4800000
    .word 0x3c023f80
    addiu      $a1, $sp, 0xD0
    .word 0xe7a000b0
    .word 0xc4800004
    .word 0xafa000c8
    .word 0xafa000c0
    .word 0xe7a000b8
    .word 0xafa000cc
    .word 0xafa000bc
    addiu      $a0, $sp, 0xC0
    .word 0xafa200c4
    .word 0xafa000dc
    jal        func_00102738
    .word 0xafa000b4
    addiu      $a0, $sp, 0xD0
    mov.s      $f20, $f0
    jal        func_00102738
    addiu     $a1, $sp, 0xB0
    .word 0xc601000c
    addiu      $a0, $sp, 0xE0
    addiu      $a1, $sp, 0xC0
    sub.s      $f0, $f1, $f0
    div.s      $f12, $f0, $f20
    nop
    nop
    jal        func_00103230
    nop
    addiu      $a0, $sp, 0xE0
    addiu      $a1, $sp, 0xB0
    jal        func_001028B8
    .word 0x70803628
    .word 0x92a30002
    addiu      $s1, $s0, 0x10
    .word 0x7000a628
    sll        $v0, $v1, 1
    addu       $v0, $v0, $v1
    addiu      $v0, $v0, 0x4
    sll        $v0, $v0, 2
    .word 0x10000027
    addu      $s2, $s0, $v0
    .word 0xc6200000
    addiu      $fp, $sp, 0x104
    addiu      $s7, $sp, 0x108
    addiu      $s3, $sp, 0x10C
    .word 0xe7a00100
    .word 0xc6200004
    addiu      $a0, $sp, 0xF0
    addiu      $a1, $sp, 0xE0
    addiu      $a2, $sp, 0x100
    .word 0xe7c00000
    .word 0xc6200008
    .word 0xe6e00000
    jal        func_001028D0
    .word 0xae600000
    .word 0xc6400000
    addiu      $a0, $sp, 0xF0
    addiu      $a1, $sp, 0x100
    .word 0xe7a00100
    .word 0xc6400004
    .word 0xe7c00000
    .word 0xc6400008
    .word 0xe6e00000
    jal        func_00102738
    .word 0xae600000
    .word 0x3c023727
    .word 0x3442c5ac
    mtc1       $v0, $f1
    nop
    c.le.s     $f0, $f1
    nop
    .word 0x45030004
    addiu     $s1, $s1, 0xC
    .word 0x1000006d
    .word 0x70001628
    addiu      $s1, $s1, 0xC
    addiu      $s2, $s2, 0xC
    addiu      $s4, $s4, 0x1
    nop
    .word 0x92a20002
    slt        $v0, $s4, $v0
    .word 0x1440ffd7
    nop
    .word 0xc7a100d0
    .word 0xc7a000d8
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
    .word 0xc7ac00d4
    .word 0x3c017000
    .word 0xc4213680
    div.s      $f0, $f0, $f1
    .word 0x3c017000
    .word 0xe4203684
    .word 0xc6000004
    mtc1       $zero, $f1
    nop
    c.le.s     $f0, $f1
    nop
    .word 0x45010024
    .word 0x3c02ff7f
    .word 0xc7a000e4
    .word 0x3c027f7f
    .word 0x3443c99e
    .word 0x3c023f80
    .word 0xe6c00000
    .word 0xaec30004
    .word 0xaec20008
    .word 0xaec0000c
    .word 0xc7a000d4
    c.lt.s     $f0, $f1
    nop
    .word 0x4500000c
    nop
    .word 0x3c017000
    jal        func_0011DBB8
    .word 0xc42c3684
    .word 0x3c023fc9
    .word 0x34420fdb
    mtc1       $v0, $f1
    .word 0x3c017000
    sub.s      $f0, $f1, $f0
    neg.s      $f0, $f0
    .word 0x1000002c
    .word 0xe420319c
    .word 0x3c017000
    jal        func_0011DBB8
    .word 0xc42c3684
    .word 0x3c023fc9
    .word 0x34420fdb
    mtc1       $v0, $f1
    .word 0x3c017000
    sub.s      $f0, $f1, $f0
    .word 0x10000022
    .word 0xe420319c
    .word 0x3c02ff7f
    .word 0x3442c99e
    .word 0xaec20000
    .word 0xc7a000e4
    .word 0x3c02bf80
    .word 0xe6c00004
    .word 0xaec00008
    .word 0xaec2000c
    .word 0xc7a000d4
    c.lt.s     $f0, $f1
    nop
    .word 0x4500000c
    nop
    .word 0x3c017000
    jal        func_0011DBB8
    .word 0xc42c3684
    .word 0x3c023fc9
    .word 0x34420fdb
    mtc1       $v0, $f1
    .word 0x3c017000
    sub.s      $f0, $f1, $f0
    neg.s      $f0, $f0
    .word 0x1000000a
    .word 0xe42031ac
    .word 0x3c017000
    jal        func_0011DBB8
    .word 0xc42c3684
    .word 0x3c023fc9
    .word 0x34420fdb
    mtc1       $v0, $f1
    .word 0x3c017000
    sub.s      $f0, $f1, $f0
    .word 0xe42031ac
    addiu      $v0, $zero, 0x1
    .word 0x7bbf00a0
    .word 0x7bbe0090
    .word 0x7bb70080
    .word 0x7bb60070
    .word 0x7bb50060
    .word 0x7bb40050
    .word 0x7bb30040
    .word 0x7bb20030
    .word 0x7bb10020
    .word 0x7bb00010
    .word 0xc7b40000
    jr         $ra
    addiu     $sp, $sp, 0x110
}
