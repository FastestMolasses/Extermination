// EE asm-void: leaf uses EE/COP2/VU instructions; reproduced verbatim
// (mwccmips accepts Sony's syntax directly).
asm void func_0011AE00(void) {
    ld $v0, 0x8($a1)
    addiu $sp, $sp, -0x10
    ld $v1, 0x0($a1)
    andi $a0, $a0, 0xFFFF
    sd $v0, 0x8($sp)
    cfc2.ni $a2, $vi1
    sd $v1, 0x0($sp)
    qmfc2.ni $a1, $vf1
    lq $v0, 0x0($sp)
    qmtc2.ni $v0, $vf1
    ctc2.ni $a0, $vi1
    vsqi.xyzw $vf1, ($vi1++)
    ctc2.ni $a2, $vi1
    qmtc2.ni $a1, $vf1
    jr $ra
    addiu $sp, $sp, 0x10
}
