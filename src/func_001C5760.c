// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00102948(int, int, int, int);
extern void func_001AFC10(int, int, int, int);
extern void func_001C22A0(int, int, int, int);
extern void func_001C6380(int, int, int, int);
extern void func_001F54E0(int, int, int, int);

asm void func_001C5760(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lbu $v1, 0x4($a0)
    addiu $v0, $zero, 0x3
    .word 0x10620030
    paddub $s0, $a0, $zero
    addiu $v0, $zero, 0x2
    .word 0x5062002e
    paddub $a0, $s0, $zero
    addiu $v0, $zero, 0x1
    .word 0x1062000e
    nop
    .word 0x10600003
    nop
    .word 0x10000026
    nop
    jal func_001C22A0
    nop
    .word 0x14400025
    nop
    jal func_001C6380
    paddub $a0, $s0, $zero
    addiu $v1, $zero, 0x1
    .word 0x10000020
    sb $v1, 0x4($s0)
    lwc1 $f0, 0xA0($s0)
    lui $at, (0x70003B68 >> 16)
    swc1 $f0, 0x80($s0)
    lwc1 $f0, 0xA4($s0)
    swc1 $f0, 0x84($s0)
    lwc1 $f0, 0xA8($s0)
    swc1 $f0, 0x88($s0)
    lwc1 $f0, 0xAC($s0)
    swc1 $f0, 0x8C($s0)
    lw $v0, (0x70003B68 & 0xFFFF)($at)
    andi $v0, $v0, 0x1
    .word 0x14400006
    nop
    addiu $a0, $sp, 0x20
    jal func_00102948
    addiu $a1, $s0, 0x80
    lui $v0, (0x3F800000 >> 16)
    sw $v0, 0x2C($sp)
    lbu $v0, 0xA($s0)
    .word 0x10400004
    addiu $a1, $s0, 0x80
    jal func_001C6380
    paddub $a0, $s0, $zero
    addiu $a1, $s0, 0x80
    jal func_001F54E0
    paddub $a0, $s0, $zero
    .word 0x10000005
    lq $ra, 0x10($sp)
    paddub $a0, $s0, $zero
    jal func_001AFC10
    nop
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
