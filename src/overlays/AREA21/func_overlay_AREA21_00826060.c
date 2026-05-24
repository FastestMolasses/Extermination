// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1D1F80(int, int, int, int);
extern void func_207F80(int, int, int, int);

asm void func_overlay_AREA21_00826060(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    addiu $a0, $zero, 0x1
    swc1 $f20, 0x0($sp)
    addiu $a1, $zero, 0x2
    mov.s $f20, $f12
    jal func_1D1F80
    paddub $a2, $a0, $zero
    lui $v0, (0x30202020 >> 16)
    addiu $a0, $zero, 0x1
    addiu $a1, $zero, 0x70F0
    addiu $a3, $zero, 0x7B10
    ori $a2, $zero, 0x84B0
    ori $8, $zero, 0x8540
    jal func_207F80
    ori $9, $v0, (0x30202020 & 0xFFFF)
}
