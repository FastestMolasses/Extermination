// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1C67E0(int, int, int, int);

asm void func_overlay_AREA08_00824490(void) {
    .word 0x14830011
    nop
    lui $v0, (0x4398EBCD >> 16)
    ori $v0, $v0, (0x4398EBCD & 0xFFFF)
    sw $v0, 0xB0($s0)
    lui $v0, (0x435C0000 >> 16)
    sw $v0, 0xB4($s0)
    lui $v0, (0x44160B14 >> 16)
    ori $v0, $v0, (0x44160B14 & 0xFFFF)
    sw $v0, 0xB8($s0)
    lui $v0, (0xBF63DEC6 >> 16)
    mtc1 $zero, $f12
    ori $v0, $v0, (0xBF63DEC6 & 0xFFFF)
    addiu $a1, $zero, 0x4
    mov.s $f13, $f12
    sw $v0, 0xC4($s0)
    jal func_1C67E0
    paddub $a0, $s0, $zero
    lh $a0, 0x28($s0)
    addiu $v1, $zero, 0x7D0
    .word 0x14830007
    nop
    lui $v0, (0x41A00000 >> 16)
    mtc1 $v0, $f12
    mtc1 $zero, $f13
    addiu $a1, $zero, 0x7
    jal func_1C67E0
    paddub $a0, $s0, $zero
    lh $a0, 0x28($s0)
    addiu $v1, $zero, 0x884
    .word 0x14830007
    nop
    lui $v0, (0x41A00000 >> 16)
    mtc1 $v0, $f12
    mtc1 $zero, $f13
    paddub $a0, $s0, $zero
    jal func_1C67E0
    paddub $a1, $zero, $zero
    lh $a0, 0x28($s0)
    slti $v1, $a0, 0xC1C
    .word 0x14600014
    nop
    addiu $v1, $zero, 0xC1C
    .word 0x14830007
    nop
    lui $v0, (0x41A00000 >> 16)
    mtc1 $zero, $f13
    mtc1 $v0, $f12
    addiu $a1, $zero, 0xA
    jal func_1C67E0
    paddub $a0, $s0, $zero
    lwc1 $f0, 0xB0($s0)
    lui $v1, (0x3D8F5C29 >> 16)
    ori $v1, $v1, (0x3D8F5C29 & 0xFFFF)
    mtc1 $v1, $f1
    nop
    sub.s $f0, $f0, $f1
    swc1 $f0, 0xB0($s0)
    lwc1 $f0, 0xB8($s0)
    add.s $f0, $f0, $f1
    swc1 $f0, 0xB8($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
