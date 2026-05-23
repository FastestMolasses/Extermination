// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001C7420(int, int, int, int);
extern void func_001CB2C0(int, int, int, int);
extern void func_001D1F80(int, int, int, int);
extern void func_001D3F50(int, int, int, int);

asm void func_001CB360(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    addiu $a1, $zero, 0x3F5
    paddub $s0, $a0, $zero
    jal func_001C7420
    paddub $a2, $zero, $zero
    addiu $a1, $zero, 0x3F3
    paddub $a0, $s0, $zero
    jal func_001CB2C0
    paddub $a2, $zero, $zero
    addiu $a1, $zero, 0x1
    paddub $a0, $zero, $zero
    jal func_001D1F80
    paddub $a2, $zero, $zero
    jal func_001D3F50
    lw $a0, 0x44($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
