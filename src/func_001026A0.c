// 4x4 matrix × vector: load matrix from a1, vector from a2, store result to a0
asm void func_001026A0(void *a0, void *a1, void *a2) {
    lqc2 $vf4, 0x0($5)
    lqc2 $vf5, 0x10($5)
    lqc2 $vf6, 0x20($5)
    lqc2 $vf7, 0x30($5)
    lqc2 $vf8, 0x0($6)
    .word 0x4BE821BC
    .word 0x4BE828BD
    .word 0x4BE830BE
    .word 0x4BE83A4B
    jr $ra
    sqc2 $vf9, 0x0($4)
}
