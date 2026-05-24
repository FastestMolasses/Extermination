// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_11E748(int, int, int, int);

asm void func_overlay_AREA01_00824F30(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lui $at, (0x8106C0 >> 16)
    lw $v0, (0x8106C0 & 0xFFFF)($at)
    lwc1 $f2, 0xB0($a0)
    lwc1 $f1, 0xB8($a0)
    lw $v0, 0x118($v0)
    lwc1 $f3, 0xC0($v0)
    lwc1 $f0, 0xC8($v0)
    sub.s $f2, $f2, $f3
    sub.s $f0, $f1, $f0
    mula.s $f2, $f2
    jal func_11E748
    madd.s $f12, $f0, $f0
    lui $v0, (0x41000000 >> 16)
    mtc1 $v0, $f1
}
