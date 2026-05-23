// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001D38F0(int, int, int, int);
extern void func_001D3C30(int, int, int, int);

asm void func_001CA940(void) {
    addiu $sp, $sp, -0x10
    .word 0x1080000d
    sq $ra, 0x0($sp)
    andi $v0, $a0, 0x1
    .word 0x10400006
    paddub $a0, $a1, $zero
    jal func_001D3C30
    paddub $a0, $a1, $zero
    .word 0x10000009
    lq $ra, 0x0($sp)
    paddub $a0, $a1, $zero
    jal func_001D38F0
    nop
    .word 0x10000003
    nop
    jal func_001D38F0
    paddub $a0, $a1, $zero
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
