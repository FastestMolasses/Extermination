// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1B1240(int, int, int, int);
extern void func_1B12B0(int, int, int, int);

asm void func_overlay_AREA01_00824FA0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lui $at, (0x8106C0 >> 16)
    lw $v0, (0x8106C0 & 0xFFFF)($at)
    paddub $s0, $a0, $zero
    lw $v0, 0x118($v0)
    lwc1 $f12, 0xC0($v0)
    lwc1 $f13, 0xC8($v0)
    jal func_1B1240
    addiu $a0, $s0, 0xB0
    lwc1 $f13, 0xC4($s0)
    lui $v0, (0x3D32B8C3 >> 16)
    ori $v0, $v0, (0x3D32B8C3 & 0xFFFF)
    mtc1 $v0, $f14
    jal func_1B12B0
}
