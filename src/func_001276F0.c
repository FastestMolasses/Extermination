// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00126AB8(int, int, int, int);
extern void func_00126BE8(int, int, int, int);

asm void func_001276F0(void) {
    addiu $sp, $sp, -0x40
    sd $a0, 0x20($sp)
    daddu $a1, $sp, $zero
    sd $ra, 0x30($sp)
    jal func_00126BE8
    addiu $a0, $sp, 0x20
    lw $v0, 0x4($sp)
    daddu $a0, $sp, $zero
    sltiu $v0, $v0, 0x1
    jal func_00126AB8
    sw $v0, 0x4($sp)
    ld $ra, 0x30($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
