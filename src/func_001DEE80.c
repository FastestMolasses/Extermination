// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001DEDB0(int, int, int, int);

asm void func_001DEE80(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_001DEDB0
    nop
    lw $v1, 0x0($a1)
    sw $v1, 0x10($v0)
    lw $v1, 0x4($a1)
    sw $v1, 0x14($v0)
    lw $v1, 0x8($a1)
    sw $v1, 0x18($v0)
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
