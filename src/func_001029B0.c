// VU0 macro-mode leaf — mwccmips accepts Sony's VU syntax in inline
// asm verbatim, and the target object assembles via .word directives
// (tools/decomp/asm_fixup.py). Reproduced as `asm void`.
asm void func_001029B0(void) {
    lqc2 $vf4, 0x0($a1)
    vitof0.xyzw $vf5, $vf4
    jr $ra
    sqc2 $vf5, 0x0($a0)
}
