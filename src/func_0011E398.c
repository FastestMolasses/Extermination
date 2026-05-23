// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011C7B0(int, int, int, int);
extern void func_0011D878(int, int, int, int);

asm void func_0011E398(void) {
    mfc1 $v0, $f12
    addiu $sp, $sp, -0x20
    daddu $a0, $v0, $zero
    lui $v1, (0x7FFFFFFF >> 16)
    ori $v1, $v1, (0x7FFFFFFF & 0xFFFF)
    lui $v0, (0x3F490FDA >> 16)
    and $a0, $a0, $v1
    ori $v0, $v0, (0x3F490FDA & 0xFFFF)
    slt $v0, $v0, $a0
    .word 0x14400004
    sd $ra, 0x10($sp)
    mtc1 $zero, $f13
    .word 0x10000010
    addiu $a0, $zero, 0x1
    lui $v0, (0x7F7FFFFF >> 16)
    ori $v0, $v0, (0x7F7FFFFF & 0xFFFF)
    slt $v0, $v0, $a0
    .word 0x10400003
    nop
    .word 0x1000000b
    sub.s $f0, $f12, $f12
    jal func_0011C7B0
    daddu $a0, $sp, $zero
    andi $v0, $v0, 0x1
    addiu $a0, $zero, 0x1
    sll $v0, $v0, 1
    lwc1 $f12, 0x0($sp)
    lwc1 $f13, 0x4($sp)
    subu $a0, $a0, $v0
    jal func_0011D878
    nop
    ld $ra, 0x10($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
