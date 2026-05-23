// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001D49D0(int, int, int, int);
extern void func_001D4B10(int, int, int, int);

asm void func_001D4B50(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    jal func_001D49D0
    paddub $s0, $a0, $zero
    jal func_001D4B10
    paddub $a0, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
