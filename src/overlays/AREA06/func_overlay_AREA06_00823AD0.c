// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1EFE00(int, int, int, int);

asm void func_overlay_AREA06_00823AD0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lbu $v1, 0x0($a1)
    andi $v1, $v1, 0x2
    .word 0x14600006
    paddub $s0, $a0, $zero
    lui $v0, (0x80000044 >> 16)
    jal func_1EFE00
    ori $a0, $v0, (0x80000044 & 0xFFFF)
    addiu $v1, $zero, 0x3C
    sw $v1, 0x1F0($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
