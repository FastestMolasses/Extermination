// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001CD370(int, int, int, int);

asm void func_001CFA60(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    swc1 $f12, 0x44($a0)
    swc1 $f13, 0x4C($a0)
    lui $v0, (0x3F800000 >> 16)
    sw $v0, 0x48($a0)
    lui $v0, (0x358637BD >> 16)
    ori $v0, $v0, (0x358637BD & 0xFFFF)
    sw $v0, 0x50($a0)
    paddub $s0, $a0, $zero
    sw $zero, 0x54($a0)
    paddub $s1, $a1, $zero
    jal func_001CD370
    paddub $a0, $zero, $zero
    sw $v0, 0x40($s0)
    lq $v1, 0x0($s1)
    sq $v1, 0x0($s0)
    lq $v1, 0x10($s1)
    sq $v1, 0x10($s0)
    lq $v1, 0x20($s1)
    sq $v1, 0x20($s0)
    lq $v1, 0x30($s1)
    sq $v1, 0x30($s0)
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
