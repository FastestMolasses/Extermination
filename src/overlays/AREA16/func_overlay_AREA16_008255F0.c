// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4

asm void func_overlay_AREA16_008255F0(void) {
    lw $v0, 0x8($a2)
    .word 0x10400004
    nop
    ori $v0, $zero, 0xFFFF
    .word 0x10000002
    sh $v0, 0x2E($a0)
    sh $zero, 0x2E($a0)
    jr $ra
    addiu $v0, $zero, 0x1
}
