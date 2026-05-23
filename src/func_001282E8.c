// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001277B0(int, int, int, int);
extern void func_001278C0(int, int, int, int);

asm void func_001282E8(void) {
    addiu $sp, $sp, -0x30
    sd $ra, 0x20($sp)
    addiu $a0, $sp, 0x10
    swc1 $f12, 0x10($sp)
    jal func_001278C0
    daddu $a1, $sp, $zero
    lw $v0, 0x4($sp)
    daddu $a0, $sp, $zero
    sltiu $v0, $v0, 0x1
    jal func_001277B0
    sw $v0, 0x4($sp)
    ld $ra, 0x20($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
