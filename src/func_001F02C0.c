// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00102948(int, int, int, int);
extern void func_001FBD50(int, int, int, int);

asm void func_001F02C0(void) {
    addiu $sp, $sp, -0x320
    sq $ra, 0x20($sp)
    sq $s0, 0x10($sp)
    paddub $s0, $a1, $zero
    paddub $a1, $a0, $zero
    swc1 $f20, 0x0($sp)
    addiu $a0, $sp, 0xE0
    jal func_00102948
    mov.s $f20, $f12
    addiu $a0, $sp, 0x30
    mov.s $f12, $f20
    paddub $a1, $s0, $zero
    jal func_001FBD50
    paddub $a2, $zero, $zero
    lq $ra, 0x20($sp)
    lq $s0, 0x10($sp)
    lwc1 $f20, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x320
}
