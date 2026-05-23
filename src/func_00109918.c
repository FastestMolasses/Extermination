// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00109CF8(int, int, int, int);

asm void func_00109918(void) {
    addiu $sp, $sp, -0x10
    lui $v0, (0xFFFFFFF >> 16)
    sd $ra, 0x0($sp)
    ori $v0, $v0, (0xFFFFFFF & 0xFFFF)
    and $a1, $a1, $v0
    lui $v1, (0x20000000 >> 16)
    lw $a3, 0x40($a0)
    or $a1, $a1, $v1
    addiu $v0, $zero, 0x1
    sw $v0, 0xB0($a3)
    sw $a1, 0xD8($a3)
    sw $a2, 0xE4($a3)
    sw $zero, 0xDC($a3)
    jal func_00109CF8
    sw $zero, 0xE0($a3)
    ld $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
