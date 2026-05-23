// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011E860(int, int, int, int);

asm void func_001B5DC0(void) {
    addiu $sp, $sp, -0x10
    andi $v0, $a0, 0xFF
    sq $ra, 0x0($sp)
    jal func_0011E860
    addiu $a0, $v0, -0x80
    slti $at, $v0, 0x31
    .word 0x10200004
    slti $at, $v0, 0x59
    .word 0x1000000c
    paddub $v0, $zero, $zero
    slti $at, $v0, 0x59
    .word 0x10200004
    slti $at, $v0, 0x7B
    .word 0x10000007
    addiu $v0, $zero, 0x1
    slti $at, $v0, 0x7B
    .word 0x10200004
    addiu $v0, $zero, 0x3
    .word 0x10000002
    addiu $v0, $zero, 0x2
    addiu $v0, $zero, 0x3
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
