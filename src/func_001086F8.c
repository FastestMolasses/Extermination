// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00108640(int, int, int, int);
extern void func_00108660(int, int, int, int);

asm void func_001086F8(void) {
    addiu $sp, $sp, -0x40
    sd $s1, 0x10($sp)
    sd $s0, 0x0($sp)
    daddu $s1, $a1, $zero
    sd $s2, 0x20($sp)
    sd $ra, 0x30($sp)
    jal func_00108640
    daddu $s0, $a0, $zero
    daddu $s2, $v0, $zero
    daddu $a0, $s0, $zero
    jal func_00108660
    daddu $a1, $s1, $zero
    daddu $v0, $s2, $zero
    ld $ra, 0x30($sp)
    ld $s2, 0x20($sp)
    ld $s1, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
