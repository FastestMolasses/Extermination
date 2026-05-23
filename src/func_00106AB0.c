// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001063E8(int, int, int, int);
extern void func_001066F8(int, int, int, int);
extern void func_00106830(int, int, int, int);

asm void func_00106AB0(void) {
    addiu $sp, $sp, -0x20
    sd $ra, 0x10($sp)
    jal func_001063E8
    sd $s0, 0x0($sp)
    lui $v0, (0x10002020 >> 16)
    ori $v0, $v0, (0x10002020 & 0xFFFF)
    lw $v1, 0x0($v0)
    andi $v1, $v1, 0x7
    negu $v1, $v1
    andi $a0, $v1, 0x7
    .word 0x10800007
    addiu $s0, $zero, 0x1
    jal func_00106830
    nop
    .word 0x10000003
    nop
    jal func_00106830
    nop
    jal func_001066F8
    addiu $a0, $zero, 0x18
    .word 0x1450fffb
    addiu $a0, $zero, 0x8
    ld $ra, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
