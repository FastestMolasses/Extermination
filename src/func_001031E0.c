// Copy 3 floats: lwc1/swc1 with $f0,$f1,$f0 sequence
asm void func_001031E0(float *a0, float *a1) {
    lwc1 $f0, 0x0($5)
    swc1 $f0, 0x0($4)
    lwc1 $f1, 0x4($5)
    swc1 $f1, 0x4($4)
    lwc1 $f0, 0x8($5)
    jr $ra
    swc1 $f0, 0x8($4)
}
