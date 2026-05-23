// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001662D0(int, int, int, int);

asm void func_001833F0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    addiu $v0, $zero, 0x2
    sb $v0, 0x23F($a0)
    paddub $s0, $a0, $zero
    jal func_001662D0
    sw $zero, 0x24C($a0)
    lbu $a0, 0x4($s0)
    addiu $v1, $zero, 0x1
    .word 0x14830003
    nop
    lui $at, (0x70003B8D >> 16)
    sb $zero, (0x70003B8D & 0xFFFF)($at)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
