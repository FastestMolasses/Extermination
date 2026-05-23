// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001D1C50(int, int, int, int);
extern void func_001D1EA0(int, int, int, int);
extern void func_001D2830(int, int, int, int);

asm void func_001D1EF0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_001D1C50
    nop
    addiu $a0, $zero, 0x3
    jal func_001D2830
    addiu $a1, $zero, 0x1
    jal func_001D1EA0
    paddub $a0, $zero, $zero
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
