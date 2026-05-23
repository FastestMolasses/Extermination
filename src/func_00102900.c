// VU scalar multiply xyzw: load vec from a1, multiply by f12 scalar, store to a0
asm void func_00102900(void *a0, void *a1, float a2) {
    lqc2 $vf4, 0x0($5)
    mfc1 $8, $f12
    qmtc2.ni $8, $vf5
    .word 0x4BE52198
    jr $ra
    sqc2 $vf6, 0x0($4)
}
