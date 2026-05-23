// Multi-call non-leaf — asm void with extern decls for every callee.
extern void CreateSema(int, int, int, int);
extern void SignalSema(int, int, int, int);

asm void func_00204B80(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    jal SignalSema
    lw $a0, 0x40($a0)
    lw $v0, 0x14($s0)
    addiu $v0, $v0, 0x7FF
    sra $v0, $v0, 11
    sll $v0, $v0, 11
    sw $v0, 0x14($s0)
    jal CreateSema
    lw $a0, 0x40($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
