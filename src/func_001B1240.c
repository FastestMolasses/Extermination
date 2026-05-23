// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_0011E620(int, int, int, int);
extern void func_001B1470(int, int, int, int);

asm void func_001B1240(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lwc1 $f1, 0x0($a0)
    lwc1 $f0, 0x8($a0)
    sub.s $f12, $f12, $f1
    jal func_0011E620
    sub.s $f13, $f13, $f0
    jal func_001B1470
    mov.s $f12, $f0
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
