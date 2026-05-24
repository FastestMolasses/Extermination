// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1D80B0(int, int, int, int);

asm void func_overlay_AREA16_008256F0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    addiu $v1, $a0, 0x1F0
    sq $s0, 0x0($sp)
    addiu $s0, $v1, 0xF0
    lw $a0, 0x2E0($a0)
    addiu $v1, $zero, -0x1
    .word 0x10830005
    nop
    jal func_1D80B0
    nop
    addiu $v1, $zero, -0x1
    sw $v1, 0x0($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
}
