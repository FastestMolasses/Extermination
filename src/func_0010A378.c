// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_0010A3A8(int, int, int, int);
extern void func_00122D10(int, int, int, int);

asm void func_0010A378(void) {
    daddu $a2, $a1, $zero
    addiu $sp, $sp, -0x110
    daddu $a1, $a0, $zero
    sd $ra, 0x100($sp)
    jal func_00122D10
    daddu $a0, $sp, $zero
    jal func_0010A3A8
    daddu $a0, $sp, $zero
    ld $ra, 0x100($sp)
    jr $ra
    addiu $sp, $sp, 0x110
}
