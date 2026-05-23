// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_00122BB8(int, int, int, int);

asm void func_001C1500(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s0, 0x10($sp)
    swc1 $f22, 0x8($sp)
    swc1 $f21, 0x4($sp)
    swc1 $f20, 0x0($sp)
    sw $a1, 0x22C($a0)
    sw $zero, 0x210($a0)
    lui $v0, (0x3F800000 >> 16)
    addiu $s0, $a0, 0x1F0
    mov.s $f22, $f12
    mov.s $f21, $f13
    mov.s $f20, $f14
    jal func_00122BB8
    sw $v0, 0x214($a0)
    sw $v0, 0x34($s0)
    sw $zero, 0x38($s0)
    swc1 $f22, 0x28($s0)
    swc1 $f21, 0x2C($s0)
    swc1 $f20, 0x30($s0)
    lq $ra, 0x20($sp)
    lq $s0, 0x10($sp)
    lwc1 $f22, 0x8($sp)
    lwc1 $f21, 0x4($sp)
    lwc1 $f20, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
