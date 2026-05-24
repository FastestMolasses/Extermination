// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1C67E0(int, int, int, int);

asm void func_overlay_AREA08_00824450(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lh $a1, 0x28($a0)
    addiu $v1, $zero, 0x14
    addiu $a1, $a1, 0x1
    sh $a1, 0x28($a0)
    lh $a1, 0x28($a0)
    .word 0x14a30005
    paddub $s0, $a0, $zero
    mtc1 $zero, $f12
    addiu $a1, $zero, 0x8
    jal func_1C67E0
    mov.s $f13, $f12
    lh $a0, 0x28($s0)
    addiu $v1, $zero, 0xB3
}
