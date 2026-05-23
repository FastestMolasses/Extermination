// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001BC150(int, int, int, int);
extern void func_001C64F0(int, int, int, int);

asm void func_001BC240(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    lui $v0, (0x3F800000 >> 16)
    sq $s0, 0x0($sp)
    mtc1 $v0, $f12
    paddub $s0, $a1, $zero
    jal func_001C64F0
    paddub $s1, $a0, $zero
    sh $v0, 0xE($s0)
    jal func_001BC150
    paddub $a0, $s1, $zero
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
