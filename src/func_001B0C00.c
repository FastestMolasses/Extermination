// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001AEDE0(int, int, int, int);
extern void func_001FAD70(int, int, int, int);

asm void func_001B0C00(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    jal func_001AEDE0
    paddub $a1, $zero, $zero
    addiu $a2, $zero, 0x1
    paddub $a0, $zero, $zero
    jal func_001FAD70
    paddub $a1, $s0, $zero
    addiu $a0, $zero, 0x1
    paddub $a1, $s0, $zero
    jal func_001FAD70
    paddub $a2, $a0, $zero
    addiu $a0, $zero, 0x2
    addiu $a2, $zero, 0x1
    jal func_001FAD70
    paddub $a1, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
