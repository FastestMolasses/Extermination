// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001749A0(int, int, int, int);
extern void func_001885D0(int, int, int, int);
extern void func_001885F0(int, int, int, int);

asm void func_0017FC80(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x20($sp)
    sq $s0, 0x10($sp)
    swc1 $f20, 0x0($sp)
    lbu $v0, 0x2F1($a0)
    mov.s $f20, $f12
    .word 0x1440000a
    paddub $s0, $a0, $zero
    jal func_001885D0
    nop
    paddub $a0, $s0, $zero
    mov.s $f12, $f20
    paddub $a1, $v0, $zero
    jal func_001749A0
    paddub $a2, $zero, $zero
    .word 0x10000009
    lq $ra, 0x20($sp)
    jal func_001885F0
    nop
    paddub $a0, $s0, $zero
    mov.s $f12, $f20
    paddub $a1, $v0, $zero
    jal func_001749A0
    paddub $a2, $zero, $zero
    lq $ra, 0x20($sp)
    lq $s0, 0x10($sp)
    lwc1 $f20, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
