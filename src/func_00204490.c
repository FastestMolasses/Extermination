// Multi-call non-leaf — asm void with extern decls for every callee.
extern void CreateSema(int, int, int, int);
extern void SignalSema(int, int, int, int);

asm void func_00204490(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s1, $a0, $zero
    lw $a0, 0x40($a0)
    jal SignalSema
    paddub $s0, $a1, $zero
    lw $v0, 0x14($s1)
    dsll32 $v1, $s0, 0
    dsra32 $v1, $v1, 0
    addu $v0, $v0, $s0
    sw $v0, 0x14($s1)
    ld $v0, 0x48($s1)
    daddu $v0, $v0, $v1
    sd $v0, 0x48($s1)
    jal CreateSema
    lw $a0, 0x40($s1)
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
