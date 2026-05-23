// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011E620(int, int, int, int);
extern void func_001B1470(int, int, int, int);

asm void func_001B1380(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    swc1 $f20, 0x0($sp)
    lwc1 $f3, 0x0($a0)
    lwc1 $f1, 0x8($a0)
    lwc1 $f0, 0x8($a1)
    lwc1 $f2, 0x0($a1)
    mov.s $f20, $f12
    sub.s $f13, $f1, $f0
    jal func_0011E620
    sub.s $f12, $f3, $f2
    jal func_001B1470
    sub.s $f12, $f0, $f20
    mtc1 $zero, $f1
    nop
    c.lt.s $f0, $f1
    nop
    .word 0x45010004
    paddub $v0, $zero, $zero
    .word 0x10000002
    addiu $v0, $zero, 0x1
    paddub $v0, $zero, $zero
    lq $ra, 0x10($sp)
    lwc1 $f20, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
