// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001278C0(int, int, int, int);
extern void func_00127FA0(int, int, int, int);

asm void func_001280B8(void) {
    addiu $sp, $sp, -0x50
    sd $s0, 0x30($sp)
    addiu $a0, $sp, 0x20
    sd $ra, 0x40($sp)
    daddu $a1, $sp, $zero
    swc1 $f12, 0x20($sp)
    jal func_001278C0
    swc1 $f13, 0x24($sp)
    addiu $s0, $sp, 0x10
    addiu $a0, $sp, 0x24
    jal func_001278C0
    daddu $a1, $s0, $zero
    daddu $a1, $s0, $zero
    jal func_00127FA0
    daddu $a0, $sp, $zero
    ld $ra, 0x40($sp)
    ld $s0, 0x30($sp)
    jr $ra
    addiu $sp, $sp, 0x50
}
