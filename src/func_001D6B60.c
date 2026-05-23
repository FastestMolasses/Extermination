// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001D1F20(int, int, int, int);
extern void func_001D6930(int, int, int, int);

asm void func_001D6B60(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    paddub $s1, $a0, $zero
    jal func_001D6930
    sq $s0, 0x0($sp)
    paddub $s0, $v0, $zero
    jal func_001D1F20
    paddub $a0, $s1, $zero
    paddub $v0, $s0, $zero
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
