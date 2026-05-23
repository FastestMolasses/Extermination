// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00179B90(int, int, int, int);
extern void func_001FBD50(int, int, int, int);

asm void func_00182AB0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    jal func_00179B90
    paddub $s0, $a0, $zero
    addiu $a1, $v0, 0x11B
    lui $v0, (0x43960000 >> 16)
    mtc1 $v0, $f12
    paddub $a0, $s0, $zero
    jal func_001FBD50
    paddub $a2, $zero, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
