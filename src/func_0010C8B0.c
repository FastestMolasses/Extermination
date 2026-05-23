// Multi-call non-leaf — asm void with extern decls for every callee.
extern void sceSifDmaStat(int, int, int, int);

asm void func_0010C8B0(void) {
    addiu $sp, $sp, -0x20
    daddu $v0, $a1, $zero
    andi $a2, $a2, 0xFFFF
    sw $a0, 0x0($sp)
    sd $ra, 0x10($sp)
    daddu $a1, $sp, $zero
    sw $v0, 0x4($sp)
    addiu $a0, $zero, -0x5
    jal sceSifDmaStat
    sw $a2, 0x8($sp)
    ld $ra, 0x10($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
