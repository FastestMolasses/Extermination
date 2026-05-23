// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00126BE8(int, int, int, int);
extern void func_00127398(int, int, int, int);

asm void func_001274B0(void) {
    addiu $sp, $sp, -0x70
    sd $a0, 0x40($sp)
    sd $a1, 0x48($sp)
    addiu $a0, $sp, 0x40
    sd $s0, 0x50($sp)
    sd $ra, 0x60($sp)
    jal func_00126BE8
    daddu $a1, $sp, $zero
    addiu $s0, $sp, 0x20
    addiu $a0, $sp, 0x48
    jal func_00126BE8
    daddu $a1, $s0, $zero
    daddu $a1, $s0, $zero
    jal func_00127398
    daddu $a0, $sp, $zero
    ld $ra, 0x60($sp)
    ld $s0, 0x50($sp)
    jr $ra
    addiu $sp, $sp, 0x70
}
