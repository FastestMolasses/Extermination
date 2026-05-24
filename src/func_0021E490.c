// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_001749A0(int, int, int, int);
extern void func_00175900(int, int, int, int);
extern void func_001764E0(int, int, int, int);
extern void func_00179880(int, int, int, int);
extern void func_0017C540(int, int, int, int);
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(int, int, int, int);
extern void func_0021C120(int, int, int, int);
extern void func_0021C190(int, int, int, int);
extern void func_0021D530(int, int, int, int);

asm void func_0021E490(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x90830006
    addiu      $v0, $zero, 0x3
    .word 0x10620046
    .word 0x70808628
    addiu      $v0, $zero, 0x2
    .word 0x10620037
    nop
    addiu      $v0, $zero, 0x1
    .word 0x10620027
    nop
    .word 0x10600004
    .word 0x3c024396
    .word 0x10000051
    .word 0x72002628
    .word 0x3c024396
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0x153
    jal        func_001FBD50
    .word 0x70003628
    addiu      $a1, $zero, 0xC0
    addiu      $a2, $zero, 0x5
    addiu      $a3, $zero, 0x1
    jal        func_001B61C0
    .word 0x70002628
    .word 0x92020006
    addiu      $v0, $v0, 0x1
    .word 0xa2020006
    .word 0xa2000007
    .word 0x92020236
    .word 0x1440000a
    .word 0x3c023f80
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0x1F
    .word 0x72002628
    jal        func_001749A0
    .word 0x70003628
    .word 0x10000008
    .word 0xae0002ec
    .word 0x3c023f80
    mtc1       $v0, $f12
    addiu      $a1, $zero, 0x57
    .word 0x72002628
    jal        func_001749A0
    .word 0x70003628
    .word 0xae0002ec
    .word 0x1000002e
    nop
    .word 0xc601003c
    .word 0x3c0241b0
    mtc1       $v0, $f0
    nop
    c.le.s     $f1, $f0
    nop
    .word 0x45000026
    nop
    addiu      $v0, $v1, 0x1
    jal        func_0021C120
    .word 0xa2020006
    .word 0x10000021
    nop
    jal        func_0021C190
    nop
    .word 0x10400006
    .word 0x3c023dcc
    .word 0x92020006
    addiu      $v0, $v0, 0x1
    .word 0x10000019
    .word 0xa2020006
    .word 0x3c023dcc
    .word 0x3442cccd
    .word 0x10000015
    .word 0xae020204
    .word 0x8e020200
    andi       $v0, $v0, 0x1000
    .word 0x10400010
    .word 0x3c023e80
    addiu      $v0, $zero, 0x3C
    .word 0xa602020e
    .word 0x92030005
    addiu      $v0, $zero, 0x18
    .word 0x14620005
    nop
    jal        func_0021D530
    nop
    .word 0x10000007
    nop
    jal        func_0017C540
    .word 0xa200025c
    .word 0x10000003
    nop
    .word 0x3c023e80
    .word 0xae020204
    .word 0x72002628
    jal        func_001764E0
    nop
    addiu      $a1, $s0, 0x2EC
    jal        func_00179880
    .word 0x72002628
    addiu      $a1, $zero, 0x1
    jal        func_00175900
    .word 0x72002628
    .word 0x7bbf0010
    .word 0x7bb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
