// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001749A0(int, int, int, int);
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(int, int, int, int);

asm void func_0021D250(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    addiu $v0, $zero, 0x2
    sb $v0, 0x0($a0)
    sw $zero, 0x220($a0)
    sb $v0, 0x4($a0)
    addiu $v0, $zero, 0x16
    sb $v0, 0x5($a0)
    sb $zero, 0x6($a0)
    addiu $v0, $zero, 0xE
    sb $zero, 0x7($a0)
    paddub $s0, $a0, $zero
    .word 0x14a00006
    sb $v0, 0x1F0($a0)
    lui $v0, (0x41000000 >> 16)
    mtc1 $v0, $f12
    addiu $a1, $zero, 0x72
    jal func_001749A0
    paddub $a2, $zero, $zero
    addiu $a0, $zero, 0x1
    addiu $a1, $zero, 0xEE
    addiu $a2, $zero, 0x3C
    jal func_001B61C0
    paddub $a3, $a0, $zero
    lui $v0, (0x43960000 >> 16)
    mtc1 $v0, $f12
    addiu $a1, $zero, 0x159
    paddub $a0, $s0, $zero
    jal func_001FBD50
    paddub $a2, $zero, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
