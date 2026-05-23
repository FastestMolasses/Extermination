// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001749A0(int, int, int, int);
extern void func_0017B490(int, int, int, int);

asm void func_00174A50(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s0, 0x10($sp)
    swc1 $f20, 0x0($sp)
    lbu $a2, 0x235($a0)
    mov.s $f20, $f12
    paddub $s0, $a0, $zero
    paddub $a1, $zero, $zero
    jal func_0017B490
    paddub $a3, $zero, $zero
    dsll32 $a1, $v0, 16
    paddub $a0, $s0, $zero
    mov.s $f12, $f20
    dsra32 $a1, $a1, 16
    jal func_001749A0
    paddub $a2, $zero, $zero
    lq $ra, 0x20($sp)
    lq $s0, 0x10($sp)
    lwc1 $f20, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
