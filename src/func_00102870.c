// VU scalar divide xyz only: load vec from a1, scale xyz by 1/f12, store to a0
asm void func_00102870(void *a0, void *a1, float a2) {
    lqc2 $vf4, 0x0($5)
    mfc1 $8, $f12
    qmtc2.ni $8, $vf5
    .word 0x4A6503BC
    .word 0x4A0003BF
    .word 0x4BC0211C
    jr $ra
    sqc2 $vf4, 0x0($4)
}
