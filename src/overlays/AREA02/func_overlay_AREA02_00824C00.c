// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_122BB8(int, int, int, int);

asm void func_overlay_AREA02_00824C00(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    jal func_122BB8
    paddub $s0, $a0, $zero
    andi $v1, $v0, 0xF
    addiu $v0, $zero, 0xFF
    subu $v0, $v0, $v1
    sll $v0, $v0, 7
    srl $v1, $v0, 7
    sll $8, $v1, 16
    sll $v0, $v1, 8
    or $8, $8, $v0
    lui $v0, (0x41600000 >> 16)
    mtc1 $v0, $f12
    or $8, $8, $v1
}
