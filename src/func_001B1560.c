// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011DF78(int, int, int, int);
extern void func_001B1240(int, int, int, int);
extern void func_001B1470(int, int, int, int);

asm void func_001B1560(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s0, 0x10($sp)
    swc1 $f20, 0x0($sp)
    mov.s $f20, $f12
    lwc1 $f12, 0x0($a1)
    lwc1 $f13, 0x8($a1)
    paddub $s0, $a0, $zero
    jal func_001B1240
    addiu $a0, $s0, 0xB0
    lwc1 $f1, 0xC4($s0)
    jal func_001B1470
    sub.s $f12, $f0, $f1
    jal func_0011DF78
    mov.s $f12, $f0
    c.le.s $f0, $f20
    nop
    .word 0x45000004
    paddub $v0, $zero, $zero
    .word 0x10000002
    addiu $v0, $zero, 0x1
    paddub $v0, $zero, $zero
    lq $ra, 0x20($sp)
    lq $s0, 0x10($sp)
    lwc1 $f20, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
