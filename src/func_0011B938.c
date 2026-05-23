// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011BA10(int, int, int, int);

asm void func_0011B938(void) {
    addiu $sp, $sp, -0x20
    sd $s0, 0x0($sp)
    sd $ra, 0x10($sp)
    jal func_0011BA10
    daddu $s0, $a0, $zero
    .word 0x10400003
    addiu $v1, $zero, 0x1
    .word 0x14430003
    nop
    .word 0x1000000c
    daddu $v0, $zero, $zero
    .word 0x16000003
    lui $v0, (0x10003C10 >> 16)
    .word 0x10000005
    addiu $v1, $zero, 0x2
    .word 0x16030006
    addiu $v0, $zero, 0x1
    lui $v0, (0x10003C10 >> 16)
    addiu $v1, $zero, 0x4
    ori $v0, $v0, (0x10003C10 & 0xFFFF)
    sw $v1, 0x0($v0)
    addiu $v0, $zero, 0x1
    ld $ra, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
