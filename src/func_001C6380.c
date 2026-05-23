// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001C94B0(int, int, int, int);
extern void func_001C9610(int, int, int, int);

asm void func_001C6380(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    addiu $a0, $s0, 0xD0
    addiu $a1, $s0, 0xB0
    addiu $a2, $s0, 0xC0
    jal func_001C94B0
    addiu $a3, $s0, 0x60
    lbu $a1, 0xC($s0)
    addiu $a0, $s0, 0x110
    jal func_001C9610
    addiu $a2, $s0, 0xD0
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
