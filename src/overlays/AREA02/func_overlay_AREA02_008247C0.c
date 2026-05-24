// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1C6120(int, int, int, int);
extern void func_1CA6E0(int, int, int, int);

asm void func_overlay_AREA02_008247C0(void) {
    addiu $sp, $sp, -0x40
    sq $ra, 0x30($sp)
    sq $s2, 0x20($sp)
    sq $s1, 0x10($sp)
    sq $s0, 0x0($sp)
    lbu $a1, 0xD($a0)
    paddub $s0, $a0, $zero
    lui $at, (0x290000 >> 16)
    jal func_1C6120
    lw $a0, -0x5A64($at)
    paddub $a1, $v0, $zero
    jal func_1CA6E0
    paddub $a0, $s0, $zero
    lui $at, (0x290000 >> 16)
    lw $v0, -0x5918($at)
    sw $v0, 0x40($s0)
}
