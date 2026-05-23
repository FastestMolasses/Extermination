// Multi-call non-leaf — asm void with extern decls for every callee.
extern void CreateSema(int, int, int, int);
extern void SignalSema(int, int, int, int);

asm void func_00204B30(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    jal SignalSema
    lw $a0, 0x40($a0)
    lw $v1, 0x10($s0)
    lw $v0, 0x14($s0)
    lw $a0, 0x40($s0)
    sll $v1, $v1, 11
    jal CreateSema
    addu $s0, $v0, $v1
    paddub $v0, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
