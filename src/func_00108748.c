// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00108640(int, int, int, int);
extern void func_00108660(int, int, int, int);

asm void func_00108748(void) {
    addiu $sp, $sp, -0x30
    addiu $a1, $zero, 0x1
    sd $s0, 0x0($sp)
    sd $s1, 0x10($sp)
    sd $ra, 0x20($sp)
    jal func_00108640
    daddu $s0, $a0, $zero
    daddu $s1, $v0, $zero
    daddu $a0, $s0, $zero
    jal func_00108660
    addiu $a1, $zero, 0x1
    daddu $v0, $s1, $zero
    ld $ra, 0x20($sp)
    ld $s1, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
