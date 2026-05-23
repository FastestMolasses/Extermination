// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00112D18(int, int, int, int);
extern void func_00113680(int, int, int, int);

asm void func_00200700(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_00112D18
    paddub $a0, $zero, $zero
    jal func_00113680
    nop
    lq $ra, 0x0($sp)
    addiu $v1, $zero, 0x1
    movn $v1, $zero, $v0
    paddub $v0, $v1, $zero
    jr $ra
    addiu $sp, $sp, 0x10
}
