// EE asm-void: leaf uses EE/COP2/VU instructions; reproduced verbatim
// (mwccmips accepts Sony's syntax directly).
asm void func_001029C0(void) {
    vsub.xyzw $vf4, $vf0, $vf0
    vadd.w $vf4, $vf4, $vf0
    vmr32.xyzw $vf5, $vf4
    vmr32.xyzw $vf6, $vf5
    vmr32.xyzw $vf7, $vf6
    sqc2 $vf4, 0x30($a0)
    sqc2 $vf5, 0x20($a0)
    sqc2 $vf6, 0x10($a0)
    jr $ra
    sqc2 $vf7, 0x0($a0)
}
