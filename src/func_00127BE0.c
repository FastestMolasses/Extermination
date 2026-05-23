// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001277B0(int, int, int, int);
extern void func_001278C0(int, int, int, int);
extern void func_00127950(int, int, int, int);

asm void func_00127BE0(void) {
    addiu $sp, $sp, -0x60
    sd $s0, 0x40($sp)
    addiu $a0, $sp, 0x30
    sd $ra, 0x50($sp)
    daddu $a1, $sp, $zero
    swc1 $f12, 0x30($sp)
    jal func_001278C0
    swc1 $f13, 0x34($sp)
    addiu $s0, $sp, 0x10
    addiu $a0, $sp, 0x34
    jal func_001278C0
    daddu $a1, $s0, $zero
    lw $v0, 0x14($sp)
    daddu $a1, $s0, $zero
    addiu $a2, $sp, 0x20
    daddu $a0, $sp, $zero
    xori $v0, $v0, 0x1
    jal func_00127950
    sw $v0, 0x14($sp)
    jal func_001277B0
    daddu $a0, $v0, $zero
    ld $ra, 0x50($sp)
    ld $s0, 0x40($sp)
    jr $ra
    addiu $sp, $sp, 0x60
}
