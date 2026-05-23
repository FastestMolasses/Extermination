// VU0 macro-mode leaf — mwccmips accepts Sony's VU syntax in inline
// asm verbatim, and the target object assembles via .word directives
// (tools/decomp/asm_fixup.py). Reproduced as `asm void`.
asm void func_001028E8(void) {
    lqc2 $vf4, 0x0($a1)
    lqc2 $vf5, 0x0($a2)
    vmul.xyzw $vf6, $vf4, $vf5
    jr $ra
    sqc2 $vf6, 0x0($a0)
}
