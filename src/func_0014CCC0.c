// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001FBD50(int, int, int, int);

asm void func_0014CCC0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    lui $v0, (0x43960000 >> 16)
    sq $s0, 0x0($sp)
    mtc1 $v0, $f12
    addiu $a1, $zero, 0x875
    paddub $s0, $a0, $zero
    jal func_001FBD50
    paddub $a2, $zero, $zero
    addiu $v1, $zero, 0x3
    sb $v1, 0x4($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
