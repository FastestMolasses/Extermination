// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00110AB8(int, int, int, int);
extern void func_00121870(int, int, int, int);

asm void func_00110B38(void) {
    addiu $sp, $sp, -0x30
    sd $s1, 0x10($sp)
    sd $s0, 0x0($sp)
    sd $ra, 0x20($sp)
    jal func_00110AB8
    daddu $s1, $a2, $zero
    daddu $s0, $v0, $zero
    daddu $a0, $s1, $zero
    lw $a2, 0x60($s0)
    jal func_00121870
    daddu $a1, $s0, $zero
    lw $v0, 0x60($s0)
    ld $ra, 0x20($sp)
    ld $s1, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
