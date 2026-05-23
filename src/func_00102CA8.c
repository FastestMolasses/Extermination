// VU clamp: load vec from a1, clamp to [f12, f13], store to a0
asm void func_00102CA8(void *a0, void *a1, float a2, float a3) {
    mfc1 $8, $f12
    mfc1 $9, $f13
    lqc2 $vf6, 0x0($5)
    qmtc2.ni $8, $vf4
    qmtc2.ni $9, $vf5
    .word 0x4BE43190
    .word 0x4BE53194
    jr $ra
    sqc2 $vf6, 0x0($4)
}
