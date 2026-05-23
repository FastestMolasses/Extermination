// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00174A50(int, int, int, int);

asm void func_001798D0(void) {
    addiu $sp, $sp, -0x20
    mtc1 $zero, $f12
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    sw $zero, 0x38($a0)
    sw $zero, 0x21C($a0)
    sb $zero, 0x25C($a0)
    jal func_00174A50
    paddub $s0, $a0, $zero
    sb $zero, 0x5($s0)
    sb $zero, 0x6($s0)
    sb $zero, 0x1F0($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
