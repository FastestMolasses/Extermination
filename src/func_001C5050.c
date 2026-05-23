// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00102948(int, int, int, int);
extern void func_001D7FA0(int, int, int, int);

asm void func_001C5050(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x20($sp)
    sq $s0, 0x10($sp)
    paddub $s0, $a0, $zero
    swc1 $f20, 0x0($sp)
    addiu $a0, $sp, 0x30
    mov.s $f20, $f12
    jal func_00102948
    addiu $a1, $s0, 0x80
    lui $v0, (0x3F800000 >> 16)
    addiu $a0, $s0, 0xB0
    mtc1 $zero, $f13
    mtc1 $v0, $f12
    addiu $a1, $sp, 0x30
    addiu $a2, $zero, 0x2
    jal func_001D7FA0
    swc1 $f20, 0x3C($sp)
    lq $ra, 0x20($sp)
    lq $s0, 0x10($sp)
    lwc1 $f20, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
