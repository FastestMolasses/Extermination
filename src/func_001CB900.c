// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001CB6B0(int, int, int, int);
extern void func_001CB9B0(int, int, int, int);

asm void func_001CB900(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s1, $a0, $zero
    paddub $s0, $a1, $zero
    jal func_001CB9B0
    paddub $a0, $a2, $zero
    addiu $a2, $zero, 0x8
    paddub $a0, $s1, $zero
    paddub $a1, $s0, $zero
    jal func_001CB6B0
    paddub $a3, $v0, $zero
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
