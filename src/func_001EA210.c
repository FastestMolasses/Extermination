// Multi-call non-leaf — asm void with extern decls for every callee.
extern void float_to_int(int, int, int, int);
extern void func_001D2DE0(int, int, int, int);

asm void func_001EA210(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal float_to_int
    nop
    addiu $a0, $zero, 0x2
    jal func_001D2DE0
    paddub $a1, $v0, $zero
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
