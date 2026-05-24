// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1FB9F0(int, int, int, int);

asm void func_overlay_AREA00_008253A0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lbu $v1, 0x4($a1)
    addiu $v0, $zero, 0x1
    .word 0x10620008
    paddub $s0, $a1, $zero
    .word 0x10600004
    addiu $v0, $v1, 0x1
    .word 0x10000017
    paddub $v0, $zero, $zero
    addiu $v0, $v1, 0x1
    sb $v0, 0x4($s0)
    sb $zero, 0x5($s0)
    lbu $v0, 0x5($s0)
    addiu $v0, $v0, 0x1
    sb $v0, 0x5($s0)
    lbu $v0, 0x5($s0)
    andi $v0, $v0, 0xF
    .word 0x14400006
    nop
    addiu $a1, $zero, 0x1000
    addiu $a0, $zero, 0x19A
    paddub $a2, $a1, $zero
    jal func_1FB9F0
    paddub $a3, $a1, $zero
    lbu $v0, 0x5($s0)
    slti $at, $v0, 0x31
    .word 0x14200003
    nop
    .word 0x10000002
    addiu $v0, $zero, 0x1
    paddub $v0, $zero, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
