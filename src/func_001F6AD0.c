// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001F6640(int, int, int, int);
extern void func_001F6760(int, int, int, int);

asm void func_001F6AD0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_001F6760
    nop
    jal func_001F6640
    paddub $a0, $v0, $zero
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
