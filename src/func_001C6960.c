// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001029C0(int, int, int, int);
extern void func_001C9940(int, int, int, int);

asm void func_001C6960(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    jal func_001029C0
    addiu $a0, $s0, 0xD0
    lbu $a1, 0xC($s0)
    addiu $a0, $s0, 0x110
    jal func_001C9940
    addiu $a2, $s0, 0xD0
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
