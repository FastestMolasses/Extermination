// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00100268(int, int, int, int);
extern void func_0010BC90(int, int, int, int);
extern void func_0010BCD0(int, int, int, int);

asm void func_001009C8(void) {
    addiu $sp, $sp, -0x20
    sd $s0, 0x0($sp)
    sd $ra, 0x10($sp)
    jal func_00100268
    nop
    daddu $s0, $v0, $zero
    lw $v0, 0x8($s0)
    .word 0x14400010
    nop
    jal func_0010BC90
    nop
    lh $v1, 0x0($s0)
    addiu $v0, $zero, 0x1
    .word 0x14620014
    ld $ra, 0x10($sp)
    lui $v1, (0x12001000 >> 16)
    ori $v1, $v1, (0x12001000 & 0xFFFF)
    ld $v0, 0x0($v1)
    dsrl $v0, $v0, 13
    andi $v0, $v0, 0x1
    dsll32 $v0, $v0, 0
    dsra32 $v0, $v0, 0
    .word 0x1000000c
    ld $s0, 0x0($sp)
    jal func_0010BCD0
    nop
    dsra $v0, $v0, 13
    lh $v1, 0x0($s0)
    andi $a0, $v0, 0x1
    addiu $v0, $zero, 0x1
    .word 0x14620003
    ld $ra, 0x10($sp)
    dsll32 $v0, $a0, 0
    dsra32 $v0, $v0, 0
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
