// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001B0DC0(int, int, int, int);
extern void func_001C62C0(int, int, int, int);
extern void func_001C63E0(int, int, int, int);

asm void func_001B1020(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a3, $zero
    paddub $s2, $a0, $zero
    jal func_001B0DC0
    paddub $s1, $a2, $zero
    .word 0x10400003
    nop
    .word 0x1000000f
    addiu $v0, $zero, 0x1
    lbu $v1, 0x4($s2)
    addiu $v0, $zero, -0x1
    addiu $v1, $v1, 0x1
    .word 0x16220005
    sb $v1, 0x4($s2)
    jal func_001C62C0
    paddub $a0, $s2, $zero
    .word 0x10000006
    paddub $v0, $zero, $zero
    dsll32 $a1, $s0, 16
    dsra32 $a1, $a1, 16
    jal func_001C63E0
    paddub $a0, $s2, $zero
    paddub $v0, $zero, $zero
    lq $ra, 0x30($sp)
    lq $s2, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
