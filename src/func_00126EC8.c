// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00126AB8(int, int, int, int);
extern void func_00126BE8(int, int, int, int);
extern void func_00126C88(int, int, int, int);

asm void func_00126EC8(void) {
    addiu $sp, $sp, -0x90
    sd $a0, 0x60($sp)
    sd $a1, 0x68($sp)
    addiu $a0, $sp, 0x60
    sd $s0, 0x70($sp)
    sd $ra, 0x80($sp)
    jal func_00126BE8
    daddu $a1, $sp, $zero
    addiu $s0, $sp, 0x20
    addiu $a0, $sp, 0x68
    jal func_00126BE8
    daddu $a1, $s0, $zero
    daddu $a1, $s0, $zero
    addiu $a2, $sp, 0x40
    jal func_00126C88
    daddu $a0, $sp, $zero
    jal func_00126AB8
    daddu $a0, $v0, $zero
    ld $ra, 0x80($sp)
    ld $s0, 0x70($sp)
    jr $ra
    addiu $sp, $sp, 0x90
}
