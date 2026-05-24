// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1CD520(int, int, int, int);

asm void func_overlay_AREA02_00824C40(void) {
    lui $v0, (0x40800000 >> 16)
    mtc1 $v0, $f14
    addiu $a1, $zero, 0x2
    mov.s $f13, $f12
    lui $v0, (0x20045B05 >> 16)
    ori $v0, $v0, (0x20045B05 & 0xFFFF)
    dsll32 $v1, $v0, 0
    ori $v0, $zero, 0x9942
    dsll $v0, $v0, 16
    ori $v0, $v0, 0x1EF0
    paddub $a2, $s0, $zero
    paddub $a0, $zero, $zero
    jal func_1CD520
    or $a3, $v0, $v1
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
