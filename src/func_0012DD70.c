// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00102948(int, int, int, int);
extern void func_001287F0(int, int, int, int);
extern void func_00128830(int, int, int, int);
extern void func_001B12B0(int, int, int, int);
extern void func_001B5360(int, int, int, int);

asm void func_0012DD70(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a1, $zero
    lbu $a1, 0x7($a0)
    addiu $v1, $zero, 0x1
    .word 0x10a30020
    paddub $s1, $a0, $zero
    .word 0x10a00003
    nop
    .word 0x10000038
    lq $ra, 0x20($sp)
    jal func_001B5360
    nop
    lui $v0, (0x3FC90FDB >> 16)
    ori $v0, $v0, (0x3FC90FDB & 0xFFFF)
    mtc1 $v0, $f12
    lui $v0, (0x3D8EFA35 >> 16)
    ori $v0, $v0, (0x3D8EFA35 & 0xFFFF)
    mtc1 $v0, $f14
    jal func_001B12B0
    lwc1 $f13, 0xC0($s1)
    swc1 $f0, 0xC0($s1)
    lw $a0, 0xE4($s0)
    addiu $v1, $zero, 0x100
    .word 0x14830028
    nop
    mtc1 $zero, $f12
    addiu $a2, $zero, 0x9
    paddub $a0, $s1, $zero
    jal func_001287F0
    paddub $a1, $s0, $zero
    sw $zero, 0xC0($s1)
    lbu $v1, 0x7($s1)
    addiu $v1, $v1, 0x1
    sb $v1, 0x7($s1)
    sw $zero, 0xD8($s0)
    .word 0x1000001c
    sh $zero, 0xF4($s0)
    lh $v1, 0xF4($s0)
    andi $v1, $v1, 0x1000
    .word 0x10600018
    nop
    mtc1 $zero, $f12
    lui $v0, (0x3F800000 >> 16)
    mtc1 $v0, $f14
    jal func_00128830
    mov.s $f13, $f12
    mtc1 $zero, $f12
    paddub $a0, $s1, $zero
    paddub $a1, $s0, $zero
    jal func_001287F0
    paddub $a2, $zero, $zero
    lui $v0, (0x40C00000 >> 16)
    mtc1 $v0, $f12
    addiu $a2, $zero, 0x2
    paddub $a0, $s1, $zero
    jal func_001287F0
    paddub $a1, $s0, $zero
    addiu $v0, $zero, 0x1
    sb $v0, 0x5($s1)
    sb $zero, 0x6($s1)
    addiu $a0, $s0, 0x50
    addiu $a1, $s1, 0xB0
    jal func_00102948
    sb $zero, 0x7($s1)
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
