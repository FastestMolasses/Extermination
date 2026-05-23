// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00203B80(int, int, int, int);

asm void func_00203C30(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    jal func_00203B80
    paddub $s0, $a0, $zero
    .word 0x10400003
    nop
    .word 0x10000009
    paddub $v0, $zero, $zero
    lw $a0, 0x8($s0)
    lw $v0, 0x0($s0)
    sll $v1, $a0, 4
    subu $a0, $v1, $a0
    sll $v1, $a0, 2
    addu $v1, $a0, $v1
    sll $v1, $v1, 14
    addu $v0, $v0, $v1
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
