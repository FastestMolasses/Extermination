// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00110B38(int, int, int, int);
extern void func_001B5C90(int, int, int, int);
extern void func_001B5CC0(int, int, int, int);
extern void func_001B5D70(int, int, int, int);
extern void func_001B5E20(int, int, int, int);

asm void func_001B5940(void) {
    addiu $sp, $sp, -0x60
    sq $ra, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s2, $a0, $zero
    lw $a0, 0x4($a1)
    paddub $s1, $a1, $zero
    paddub $s0, $a2, $zero
    lw $a1, 0x8($a1)
    jal func_00110B38
    addiu $a2, $sp, 0x40
    .word 0x14400003
    nop
    .word 0x10000075
    paddub $v0, $zero, $zero
    lbu $v0, 0x40($sp)
    .word 0x14400072
    paddub $v0, $zero, $zero
    lhu $v0, 0x0($s2)
    sh $v0, 0x2($s2)
    lbu $v1, 0x42($sp)
    lbu $v0, 0x43($sp)
    sll $v1, $v1, 8
    or $v0, $v1, $v0
    xori $v0, $v0, 0xFFFF
    .word 0x12000035
    sh $v0, 0x0($s2)
    lbu $v0, 0x44($sp)
    sb $v0, 0x26($s1)
    lbu $v0, 0x45($sp)
    sb $v0, 0x27($s1)
    lbu $v0, 0x46($sp)
    sb $v0, 0x24($s1)
    lbu $v0, 0x47($sp)
    sb $v0, 0x25($s1)
    lbu $a1, 0x25($s1)
    jal func_001B5CC0
    lbu $a0, 0x24($s1)
    sb $v0, 0x17($s1)
    lbu $v0, 0x17($s1)
    .word 0x1440000d
    nop
    lw $v0, 0x4($s1)
    .word 0x14400026
    nop
    lhu $v0, 0x0($s2)
    andi $v0, $v0, 0xF000
    .word 0x10400022
    nop
    paddub $a1, $s1, $zero
    jal func_001B5E20
    paddub $a0, $s2, $zero
    .word 0x1000001e
    lhu $v0, 0x4($s2)
    lw $v0, 0x4($s1)
    .word 0x14400010
    nop
    lhu $v0, 0x0($s2)
    andi $v0, $v0, 0xFFF
    sh $v0, 0x0($s2)
    lbu $a0, 0x24($s1)
    jal func_001B5D70
    paddub $a1, $zero, $zero
    lhu $v1, 0x0($s2)
    or $v0, $v1, $v0
    sh $v0, 0x0($s2)
    lbu $a0, 0x25($s1)
    jal func_001B5D70
    addiu $a1, $zero, 0x1
    lhu $v1, 0x0($s2)
    or $v0, $v1, $v0
    sh $v0, 0x0($s2)
    jal func_001B5C90
    lbu $a0, 0x24($s1)
    sb $v0, 0x24($s1)
    jal func_001B5C90
    lbu $a0, 0x25($s1)
    .word 0x10000004
    sb $v0, 0x25($s1)
    paddub $a1, $s1, $zero
    jal func_001B5E20
    paddub $a0, $s2, $zero
    lhu $v0, 0x4($s2)
    sh $v0, 0x6($s2)
    lhu $v1, 0x2($s2)
    lhu $v0, 0x0($s2)
    not $v1, $v1
    and $v0, $v1, $v0
    sh $v0, 0x4($s2)
    lhu $v1, 0x0($s2)
    lhu $v0, 0x2($s2)
    andi $v1, $v1, 0xF000
    andi $v0, $v0, 0xF000
    andi $v1, $v1, 0xFFFF
    andi $v0, $v0, 0xFFFF
    .word 0x1462001d
    addiu $v0, $zero, 0x20
    .word 0x1060001a
    nop
    lh $v0, 0xA($s2)
    addiu $v0, $v0, -0x1
    sh $v0, 0xA($s2)
    lh $v0, 0xA($s2)
    .word 0x1440000e
    nop
    lhu $a0, 0x0($s2)
    lui $v0, (0xFFFF0FFF >> 16)
    ori $v1, $v0, (0xFFFF0FFF & 0xFFFF)
    addiu $v0, $zero, 0xA
    andi $a0, $a0, 0xF000
    sh $a0, 0x8($s2)
    lhu $a0, 0x4($s2)
    lhu $a1, 0x8($s2)
    and $v1, $a0, $v1
    or $v1, $a1, $v1
    sh $v1, 0x8($s2)
    .word 0x1000000b
    sh $v0, 0xA($s2)
    lhu $v1, 0x4($s2)
    lui $v0, (0xFFFF0FFF >> 16)
    ori $v0, $v0, (0xFFFF0FFF & 0xFFFF)
    and $v0, $v1, $v0
    .word 0x10000005
    sh $v0, 0x8($s2)
    addiu $v0, $zero, 0x20
    sh $v0, 0xA($s2)
    lhu $v0, 0x4($s2)
    sh $v0, 0x8($s2)
    .word 0x10000002
    addiu $v0, $zero, 0x1
    paddub $v0, $zero, $zero
    lq $ra, 0x30($sp)
    lq $s2, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x60
}
