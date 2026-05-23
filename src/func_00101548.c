// Multi-call non-leaf — asm void with extern decls for every callee.
extern void AddIntcHandler(int, int, int, int);
extern void func_00100268(int, int, int, int);
extern void func_0010C2F8(int, int, int, int);

asm void func_00101548(void) {
    addiu $sp, $sp, -0x40
    sd $s0, 0x0($sp)
    sd $s2, 0x20($sp)
    sd $s1, 0x10($sp)
    sd $ra, 0x30($sp)
    jal func_00100268
    daddu $s0, $a0, $zero
    daddu $s1, $v0, $zero
    daddu $a1, $s0, $zero
    lw $s2, 0x8($s1)
    addiu $a2, $zero, -0x1
    sw $s0, 0x8($s1)
    jal AddIntcHandler
    addiu $a0, $zero, 0x2
    sw $v0, 0xC($s1)
    jal func_0010C2F8
    addiu $a0, $zero, 0x2
    daddu $v0, $s2, $zero
    ld $ra, 0x30($sp)
    ld $s2, 0x20($sp)
    ld $s1, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
