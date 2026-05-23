// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001098D8(int, int, int, int);
extern void func_00204AE0(int, int, int, int);

asm void func_00206B30(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    jal func_00204AE0
    addiu $a0, $s0, 0x48
    jal func_001098D8
    paddub $a0, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    addiu $v0, $zero, 0x1
    jr $ra
    addiu $sp, $sp, 0x20
}
