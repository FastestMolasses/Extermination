// EE asm-void: leaf uses EE/COP2/VU instructions; reproduced verbatim
// (mwccmips accepts Sony's syntax directly).
asm void func_00102738(void) {
    lqc2 $vf4, 0x0($a0)
    lqc2 $vf5, 0x0($a1)
    vmul.xyz $vf5, $vf4, $vf5
    vaddy.x $vf5, $vf5, $vf5y
    vaddz.x $vf5, $vf5, $vf5z
    qmfc2.ni $v0, $vf5
    mtc1 $v0, $f0
    jr $ra
    nop
}
