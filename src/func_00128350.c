// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00127728(int, int, int, int);
extern void func_001278C0(int, int, int, int);

asm void func_00128350(void) {
    addiu $sp, $sp, -0x30
    sd $ra, 0x20($sp)
    addiu $a0, $sp, 0x10
    swc1 $f12, 0x10($sp)
    jal func_001278C0
    daddu $a1, $sp, $zero
    lw $a3, 0xC($sp)
    lw $a0, 0x0($sp)
    dsll32 $a3, $a3, 0
    lw $a1, 0x4($sp)
    lw $a2, 0x8($sp)
    jal func_00127728
    dsrl $a3, $a3, 2
    ld $ra, 0x20($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
