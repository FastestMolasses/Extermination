// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001B4CF0(int, int, int, int);
extern void func_001EFE00(int, int, int, int);

asm void func_001513B0(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lh $v0, 0x36($a0)
    paddub $s1, $a1, $zero
    .word 0x14400003
    paddub $s2, $a0, $zero
    .word 0x10000068
    paddub $v0, $zero, $zero
    lb $v0, 0x60($s1)
    .word 0x10400007
    addiu $v0, $zero, 0x2
    addiu $v0, $zero, 0x1
    sb $v0, 0x0($s2)
    sh $zero, 0x36($s2)
    .word 0x10000060
    paddub $v0, $zero, $zero
    addiu $v0, $zero, 0x2
    sb $v0, 0x0($s2)
    addiu $v0, $zero, -0x1
    sb $v0, 0x60($s1)
    lh $v1, 0x36($s2)
    andi $v0, $v1, 0xFFF
    dsll32 $s0, $v0, 16
    andi $v0, $v1, 0x8000
    .word 0x10400005
    dsra32 $s0, $s0, 16
    sll $v0, $s0, 2
    addu $v0, $v0, $s0
    dsll32 $s0, $v0, 16
    dsra32 $s0, $s0, 16
    lb $v0, 0x61($s1)
    .word 0x10400005
    nop
    lh $v0, 0x50($s1)
    addu $v0, $v0, $s0
    .word 0x10000002
    sh $v0, 0x50($s1)
    sh $s0, 0x50($s1)
    addiu $v0, $zero, 0x19
    sb $v0, 0x61($s1)
    lb $v0, 0x62($s1)
    .word 0x1440000b
    nop
    lh $v0, 0x36($s2)
    andi $v0, $v0, 0x4000
    .word 0x10400007
    nop
    addiu $v0, $zero, 0x3C
    sb $v0, 0x62($s1)
    lui $v0, (0x80000027 >> 16)
    ori $a0, $v0, (0x80000027 & 0xFFFF)
    jal func_001EFE00
    paddub $a1, $s2, $zero
    lh $v1, 0x34($s2)
    dsll32 $v0, $s0, 16
    dsra32 $v0, $v0, 16
    slt $at, $v0, $v1
    .word 0x14200007
    subu $v0, $v1, $s0
    paddub $a0, $s2, $zero
    jal func_001B4CF0
    sh $zero, 0x34($s2)
    .word 0x10000004
    lh $v1, 0x52($s1)
    subu $v0, $v1, $s0
    sh $v0, 0x34($s2)
    lh $v1, 0x52($s1)
    addiu $v0, $zero, 0x1
    addu $v1, $v1, $s0
    sh $v1, 0x52($s1)
    lbu $v1, 0x4($s2)
    .word 0x14620015
    nop
    lh $v0, 0x34($s2)
    .word 0x1040000a
    addiu $v0, $zero, 0x2
    lh $v0, 0x50($s1)
    slti $at, $v0, 0x19
    .word 0x10200005
    nop
    lh $v0, 0x36($s2)
    andi $v0, $v0, 0xA000
    .word 0x1040000a
    nop
    addiu $v0, $zero, 0x2
    sb $v0, 0x4($s2)
    sb $zero, 0x5($s2)
    sb $zero, 0x6($s2)
    addiu $v0, $zero, 0x1E
    sh $zero, 0x50($s1)
    sb $v0, 0x60($s1)
    .word 0x10000013
    addiu $v0, $zero, 0x1
    lh $v1, 0x36($s2)
    andi $v0, $v1, 0x5000
    .word 0x10400005
    andi $v0, $v1, 0x2000
    addiu $v0, $zero, 0xA
    .word 0x10000008
    sb $v0, 0x60($s1)
    andi $v0, $v1, 0x2000
    .word 0x10400004
    nop
    addiu $v0, $zero, 0x1E
    .word 0x10000002
    sb $v0, 0x60($s1)
    sb $zero, 0x60($s1)
    addiu $v0, $zero, 0x1
    sb $v0, 0x0($s2)
    sh $zero, 0x36($s2)
    paddub $v0, $zero, $zero
    lq $ra, 0x30($sp)
    lq $s2, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
