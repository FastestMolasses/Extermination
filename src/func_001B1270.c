// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_0011E620(int, int, int, int);
extern void func_001B1470(int, int, int, int);

asm void func_001B1270(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lwc1 $f2, 0x4($a0)
    lwc1 $f0, 0x8($a0)
    lui $v0, (0xBF800000 >> 16)
    mtc1 $v0, $f1
    sub.s $f2, $f12, $f2
    mul.s $f12, $f1, $f2
    jal func_0011E620
    sub.s $f13, $f13, $f0
    jal func_001B1470
    mov.s $f12, $f0
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
