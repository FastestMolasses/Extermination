// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00121AF8(int, int, int, int);

asm void func_00121F78(void) {
    addiu $sp, $sp, -0x20
    sd $s0, 0x0($sp)
    daddu $s0, $a1, $zero
    sd $ra, 0x10($sp)
    jal func_00121AF8
    addiu $a1, $zero, 0x1
    daddu $v1, $v0, $zero
    addiu $a0, $zero, 0x1
    sw $s0, 0x14($v1)
    ld $ra, 0x10($sp)
    ld $s0, 0x0($sp)
    sw $a0, 0x10($v1)
    jr $ra
    addiu $sp, $sp, 0x20
}
