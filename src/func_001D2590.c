// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_0011E398(int, int, int, int);
extern void func_001D25F0(int, int, int, int);

asm void func_001D2590(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    lui $v0, (0x40000000 >> 16)
    swc1 $f20, 0x0($sp)
    mtc1 $v0, $f0
    mov.s $f20, $f12
    div.s $f12, $f13, $f0
    nop
    nop
    jal func_0011E398
    nop
    nop
    nop
    div.s $f12, $f20, $f0
    nop
    nop
    jal func_001D25F0
    nop
    lq $ra, 0x10($sp)
    lwc1 $f20, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
