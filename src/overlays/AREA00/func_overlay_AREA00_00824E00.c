// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1C67E0(int, int, int, int);

asm void func_overlay_AREA00_00824E00(void) {
    addiu $sp, $sp, -0x20
    mtc1 $zero, $f13
    sq $ra, 0x10($sp)
    lui $v0, (0x3F19999A >> 16)
    sq $s0, 0x0($sp)
    ori $v0, $v0, (0x3F19999A & 0xFFFF)
    sw $v0, 0x2C8($a0)
    lui $v0, (0x40266666 >> 16)
    ori $v0, $v0, (0x40266666 & 0xFFFF)
    sw $v0, 0x2DC($a0)
    lui $v0, (0x40C00000 >> 16)
    addiu $s0, $a0, 0x1F0
    mtc1 $v0, $f12
    jal func_1C67E0
    addiu $a1, $zero, 0x6
    addiu $v0, $zero, 0xF0
    sh $v0, 0xD0($s0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    addiu $v0, $zero, 0x1
    jr $ra
    addiu $sp, $sp, 0x20
}
