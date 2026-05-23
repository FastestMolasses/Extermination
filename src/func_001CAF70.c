// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001CAA00(int, int, int, int);
extern void func_001CABA0(int, int, int, int);

asm void func_001CAF70(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    jal func_001CAA00
    paddub $s0, $a0, $zero
    lw $a1, 0x90($s0)
    jal func_001CABA0
    paddub $a0, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
