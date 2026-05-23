// VU lerp xyz keep w: load vf4 from a1, vf5 from a2, t from f12, lerp xyz only, store to a0
asm void func_00103200(void *a0, void *a1, void *a2, float a3) {
    lqc2 $vf4, 0x0($5)
    lqc2 $vf5, 0x0($6)
    mfc1 $8, $f12
    qmtc2.ni $8, $vf6
    .word 0x4A29233C
    .word 0x4B0001C3
    .word 0x4B063A2C
    .word 0x4BC621BC
    .word 0x4BC82A48
    jr $ra
    sqc2 $vf9, 0x0($4)
}
