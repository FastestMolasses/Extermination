// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001E7780(int, int, int, int);
extern void func_001F0310(int, int, int, int);

asm void func_001D0660(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_001F0310
    nop
    jal func_001E7780
    nop
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
