// Multi-call non-leaf — asm void with extern decls for every callee.
extern void dmac_channel_base(int, int, int, int);
extern void func_00101F08(int, int, int, int);
extern void func_00102468(int, int, int, int);

asm void func_00200830(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    paddub $s1, $a0, $zero
    addiu $a0, $zero, 0x1
    jal dmac_channel_base
    sq $s0, 0x0($sp)
    paddub $s0, $v0, $zero
    paddub $a0, $s0, $zero
    paddub $a1, $zero, $zero
    jal func_00102468
    paddub $a2, $zero, $zero
    paddub $a1, $s1, $zero
    jal func_00101F08
    paddub $a0, $s0, $zero
    paddub $a0, $s0, $zero
    paddub $a1, $zero, $zero
    jal func_00102468
    paddub $a2, $zero, $zero
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
